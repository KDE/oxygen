//////////////////////////////////////////////////////////////////////////////
// oxygendockseparatordata.cpp
// generic data container for widgetstate hover (mouse-over) animations
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

#include "oxygendockseparatordata.h"
#include "oxygendockseparatordata.moc"

#include <QtCore/QTextStream>
namespace Oxygen
{

    //______________________________________________
    void DockSeparatorData::updateRect( QRect r, bool hovered )
    {

        if( hovered )
        {
            setRect( r );
            if( animation().data()->direction() == Animation::Backward )
            {
                if( animation().data()->isRunning() ) animation().data()->stop();
                animation().data()->setDirection( Animation::Forward );
                animation().data()->start();
            }
            
        } else if( animation().data()->direction() == Animation::Forward && r == rect()  ) {

            if( animation().data()->isRunning() ) animation().data()->stop();
            animation().data()->setDirection( Animation::Backward );
            animation().data()->start();

        }

    }

}
