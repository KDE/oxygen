//////////////////////////////////////////////////////////////////////////////
// oxygenhoverdata.cpp
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

#include "oxygenhoverdata.h"
#include "oxygenhoverdata.moc"

#include <QtGui/QHoverEvent>

namespace Oxygen
{

    //______________________________________________
    bool HoverData::eventFilter( QObject*, QEvent* event )
    {

        if( !enabled() ) return false;

        // check event type
        switch( event->type() )
        {

            // enter event
            case QEvent::HoverEnter:
            {
                timeLine()->setDirection( QTimeLine::Forward );
                if( !timeLine()->isRunning() ) timeLine()->start();
                break;
            }

            // leave event
            case QEvent::HoverLeave:
            {
                timeLine()->setDirection( QTimeLine::Backward );
                if( !timeLine()->isRunning() ) timeLine()->start();
                break;
            }

            default: break;

        }

        return false;

    }

}
