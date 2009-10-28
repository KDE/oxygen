//////////////////////////////////////////////////////////////////////////////
// oxygenmenubardata.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenmenubardata.h"
#include "oxygenmenubardata.moc"

namespace Oxygen
{

    //______________________________________________
    MenuBarDataV1::MenuBarDataV1( QObject* parent, QWidget* target, int maxFrame, int duration ):
        WidgetData( parent, target ),
        currentTimeLine_( new TimeLine( duration, this ) ),
        previousTimeLine_( new TimeLine( duration, this ) )
    {

        // setup timeLine
        currentTimeLine()->setDirection( QTimeLine::Forward );
        currentTimeLine()->setFrameRange( 0, maxFrame );
        currentTimeLine()->setCurveShape( QTimeLine::LinearCurve );
        connect( currentTimeLine().data(), SIGNAL( frameChanged( int ) ), SLOT( setDirty( void ) ) );
        connect( currentTimeLine().data(), SIGNAL( finished() ), SLOT( setDirty( void ) ) );

        previousTimeLine()->setDirection( QTimeLine::Backward );
        previousTimeLine()->setFrameRange( 0, maxFrame );
        previousTimeLine()->setCurveShape( QTimeLine::LinearCurve );
        connect( previousTimeLine().data(), SIGNAL( frameChanged( int ) ), SLOT( setDirty( void ) ) );
        connect( previousTimeLine().data(), SIGNAL( finished() ), SLOT( setDirty( void ) ) );

    }

    //______________________________________________
    bool MenuBarDataV1::eventFilter( QObject* object, QEvent* event )
    {

        if( !enabled() ) return false;

        // check event type
        switch( event->type() )
        {

            case QEvent::Enter:
            {
                // first need to call proper event processing
                // then implement transition
                object->event( event );
                enterEvent( object );
                return true;
            }

            case QEvent::Leave:
            {
                // first need to call proper event processing
                // then implement transition
                object->event( event );
                leaveEvent( object );
                return true;
            }

            case QEvent::MouseMove:
            {
                // first need to call proper event processing
                // then implement transition
                object->event( event );
                mouseMoveEvent( object );
                return true;
            }

            default: break;

        }

        // always forward event
        return false;

    }

}
