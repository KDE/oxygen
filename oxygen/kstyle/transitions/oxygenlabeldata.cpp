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

namespace Oxygen
{

    //______________________________________________________
    LabelData::LabelData( QObject* parent, QLabel* target, int duration ):
        TransitionData( parent, target, duration ),
        target_( target ),
        text_( target->text() ),
        pixmap_( target->pixmap() )
    { target_.data()->installEventFilter( this ); }

    //___________________________________________________________________
    bool LabelData::eventFilter( QObject* object, QEvent* event )
    {

        if( object != target_ ) return TransitionData::eventFilter( object, event );
        switch( event->type() )
        {
            case QEvent::Paint:
            {
                if( enabled() && target_ && ( target_.data()->text() != text_ || target_.data()->pixmap() != pixmap_ ) )
                {

                    // need to update flags on fly
                    bool transparent( target_.data()->isTopLevel() && target_.data()->testAttribute( Qt::WA_NoSystemBackground ) );
                    transition().data()->setFlags( transparent ? TransitionWidget::Transparent : TransitionWidget::GrabFromWindow );

                    // update text and pixmap
                    text_ = target_.data()->text();
                    pixmap_ = target_.data()->pixmap();

                    // try start animation
                    if( initializeAnimation() )
                    {

                        timer_.start( 0, this );
                        return true;

                    } else break;

                } else if( transition().data()->isAnimated() ) {

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
        if( !( enabled() && target_ && target_.data()->isVisible() ) ) return false;
        transition().data()->setOpacity(0);
        transition().data()->setGeometry( target_.data()->rect() );
        transition().data()->setStartPixmap( transition().data()->endPixmap() );
        transition().data()->show();
        transition().data()->raise();
        return true;
    }

    //___________________________________________________________________
    bool LabelData::animate( void )
    {

        if( !enabled() ) return false;

        // check enability
        transition().data()->endAnimation();
        transition().data()->setEndPixmap( transition().data()->grab( target_.data() ) );
        transition().data()->animate();
        return true;

    }

}
