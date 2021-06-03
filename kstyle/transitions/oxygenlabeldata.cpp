//////////////////////////////////////////////////////////////////////////////
// oxygenlabeldata.cpp
// data container for QLabel transition
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenlabeldata.h"

#include <QEvent>
#include <QTextStream>
#include <QPainter>

namespace Oxygen
{

    // use 300 milliseconds for animation lock
    const int LabelData::_lockTime = 300;

    //______________________________________________________
    LabelData::LabelData( QObject* parent, QLabel* target, int duration ):
        TransitionData( parent, target, duration ),
        _target( target )
    {
        _target.data()->installEventFilter( this );

        const bool hasProxy( _target.data()->graphicsProxyWidget() );
        const bool hasMessageWidget( hasParent( target, "KMessageWidget" ) );

        transition().data()->setFlags( hasProxy||hasMessageWidget ? TransitionWidget::Transparent : TransitionWidget::GrabFromWindow );

        connect( _target.data(), SIGNAL(destroyed()), SLOT(targetDestroyed()) );

    }

    //___________________________________________________________________
    bool LabelData::eventFilter( QObject* object, QEvent* event )
    {

        if( object != _target.data() ) return TransitionData::eventFilter( object, event );
        switch( event->type() )
        {

            case QEvent::Show:
            /*
            at show event, on set the old text to current
            to avoid animate the "first" paint event.
            text mnemonic is always removed to avoid triggering the animation when only the
            latter is changed
            */
            _text = _target.data()->text().remove( QChar::fromLatin1( '&' ) );
            break;

            case QEvent::Paint:
            {

                if( enabled() && _target  )
                {

                    // remove showMnemonic from text before comparing
                    QString text( _target.data()->text().remove( QChar::fromLatin1( '&' ) ) );
                    if( text == _text )
                    {
                        if(
                            transparent() &&
                            transition().data()->isAnimated() &&
                            TransitionWidget::paintEnabled() ) return true;
                        else break;
                    }

                    // update text and pixmap
                    _text = text;

                    if( !(transition() && _target.data()->isVisible() ) ) break;

                    if( transition().data()->isAnimated() )
                    { transition().data()->endAnimation(); }

                    // check whether animations are locked
                    if( isLocked() )
                    {

                        // hide transition widget
                        transition().data()->hide();

                        // restart the lock timer
                        // and abort transition
                        lockAnimations();
                        break;
                    }

                    // restart the lock timer
                    // and prepare transition
                    lockAnimations();
                    initializeAnimation();
                    _timer.start( 0, this );

                    if( !transition().data()->startPixmap().isNull() && TransitionWidget::paintEnabled() )
                    {

                        // show the transition widget
                        // and disable this event painting
                        transition().data()->show();
                        transition().data()->raise();
                        if( transparent() ) return true;
                        else break;

                    } else {

                        // hide transition widget and abort transition
                        transition().data()->hide();
                        break;

                    }

                } else if( transition().data()->isAnimated() && TransitionWidget::paintEnabled() ) {

                    // disable painting when transition is running
                    // since label is obscured by transition widget
                    return true;

                } else break;
            }

            default: break;
        }

        return TransitionData::eventFilter( object, event );

    }

    //___________________________________________________________________
    void LabelData::timerEvent( QTimerEvent* event )
    {
        if( event->timerId() == _timer.timerId() )
        {

            _timer.stop();

            // check transition and widget validity
            if( !( enabled() && _target && transition() ) ) return;

            // assign end pixmap
            transition().data()->setEndPixmap( transition().data()->grab( _target.data() ) );

            // start animation
            animate();

        } else if( event->timerId() == _animationLockTimer.timerId() ) {

            unlockAnimations();

            // check transition and widget validity
            if( !( enabled() && _target && transition() ) ) return;

            // reassign end pixmap for the next transition to be properly initialized
            transition().data()->setEndPixmap( transition().data()->grab( _target.data() ) );

        } else return TransitionData::timerEvent( event );

    }

    //___________________________________________________________________
    bool LabelData::initializeAnimation( void )
    {

        transition().data()->setOpacity(0);
        QRect current( _target.data()->geometry() );
        if( _widgetRect.isValid() && _widgetRect != current )
        {

            _widgetRect = current;
            transition().data()->resetStartPixmap();
            transition().data()->resetEndPixmap();
            return false;

        }

        transition().data()->setStartPixmap( transition().data()->currentPixmap() );
        transition().data()->setGeometry( _target.data()->rect() );
        _widgetRect = current;
        return true;
    }

    //___________________________________________________________________
    bool LabelData::animate( void )
    {

        if( transition().data()->startPixmap().isNull() ) return false;

        transition().data()->animate();
        return true;

    }

    //___________________________________________________________________
    void LabelData::targetDestroyed( void )
    {
        setEnabled( false );
        _target.clear();
    }

}
