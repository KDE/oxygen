//////////////////////////////////////////////////////////////////////////////
// oxygenlabeldata.cpp
// data container for QLabel transition
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenlabeldata.h"
#include "oxygenlabeldata.moc"

#include <QtCore/QEvent>
#include <QtCore/QTextStream>
#include <QtGui/QPainter>

namespace Oxygen
{

    //______________________________________________________
    LabelData::LabelData( QObject* parent, QLabel* target, int duration ):
        TransitionData( parent, target, duration ),
        target_( target )
    {
        target_.data()->installEventFilter( this );
        bool hasProxy( target_.data()->graphicsProxyWidget() );
        transition().data()->setFlags( hasProxy ? TransitionWidget::Transparent : TransitionWidget::GrabFromWindow );

        connect( target_.data(), SIGNAL( destroyed() ), SLOT( targetDestroyed() ) );

    }

    //___________________________________________________________________
    bool LabelData::eventFilter( QObject* object, QEvent* event )
    {

        if( object != target_.data() ) return TransitionData::eventFilter( object, event );
        switch( event->type() )
        {

            case QEvent::Show:
            /*
            at show event, on set the old text to current
            to avoid animate the "first" paint event.
            text mnemonic is always removed to avoid triggering the animation when only the
            latter is changed
            */
            text_ = target_.data()->text().remove( '&' );
            break;

            case QEvent::Paint:
            {
                // remove mnemonic from text
                if( enabled() && target_  )
                {

                    // remove showMnemonic from text before comparing
                    QString text( target_.data()->text().remove( '&' ) );
                    if( text == text_ )
                    {
                        if( transition().data()->isAnimated() && TransitionWidget::paintEnabled() ) return true;
                        else break;
                    }

                    // update text and pixmap
                    text_ = text;

                    if( !( enabled() && transition() && target_ && target_.data()->isVisible() ) ) break;

                    if( transition().data()->isAnimated() )
                    { transition().data()->endAnimation(); }

                    initializeAnimation();
                    timer_.start( 0, this );

                    if( !transition().data()->startPixmap().isNull() && TransitionWidget::paintEnabled() ) 
                    {
                        
                        transition().data()->show();
                        transition().data()->raise();
                        return true;
                        
                    } else break;
                    
                } else if( transition().data()->isAnimated() && TransitionWidget::paintEnabled() ) {

                    // disable painting when label is animated.
                    // since it is obscured by transition
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
        if( event->timerId() == timer_.timerId() )
        {

            timer_.stop();
            animate();

        } else return QObject::timerEvent( event );

    }

    //___________________________________________________________________
    bool LabelData::initializeAnimation( void )
    {

        transition().data()->setOpacity(0);
        QRect current( target_.data()->geometry() );
        if( widgetRect_.isValid() && widgetRect_ != current )
        {

            widgetRect_ = current;
            transition().data()->resetStartPixmap();
            transition().data()->resetEndPixmap();
            return false;

        }

        transition().data()->setStartPixmap( transition().data()->currentPixmap() );
        transition().data()->setGeometry( target_.data()->rect() );
        widgetRect_ = current;
        return true;
    }

    //___________________________________________________________________
    bool LabelData::animate( void )
    {

        if( !( enabled() && transition() ) ) return false;

        // check enability
        transition().data()->setEndPixmap( transition().data()->grab( target_.data() ) );

        if( !transition().data()->startPixmap().isNull() )
        {

            transition().data()->animate();
            return true;

        } else return false;

    }

    //___________________________________________________________________
    void LabelData::targetDestroyed( void )
    {
        setEnabled( false );
        target_.clear();
    }

}
