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
    DockSeparatorData::DockSeparatorData( QObject* parent, QWidget* target, int duration ):
        AnimationData( parent, target )
    {

        // setup animation
        _horizontalData._animation = new Animation( duration, this );
        _horizontalData._animation.data()->setStartValue( 0.0 );
        _horizontalData._animation.data()->setEndValue( 1.0 );
        _horizontalData._animation.data()->setTargetObject( this );
        _horizontalData._animation.data()->setPropertyName( "horizontalOpacity" );

        // setup animation
        _verticalData._animation = new Animation( duration, this );
        _verticalData._animation.data()->setStartValue( 0.0 );
        _verticalData._animation.data()->setEndValue( 1.0 );
        _verticalData._animation.data()->setTargetObject( this );
        _verticalData._animation.data()->setPropertyName( "verticalOpacity" );

    }

    //______________________________________________
    void DockSeparatorData::updateRect( const QRect& r, const Qt::Orientation& orientation, bool hovered )
    {

        Data& data( orientation == Qt::Vertical ? _verticalData:_horizontalData );

        if( hovered )
        {
            data._rect = r;
            if( data._animation.data()->direction() == Animation::Backward )
            {
                if( data._animation.data()->isRunning() ) data._animation.data()->stop();
                data._animation.data()->setDirection( Animation::Forward );
                data._animation.data()->start();
            }

        } else if( data._animation.data()->direction() == Animation::Forward && r == data._rect  ) {

            if( data._animation.data()->isRunning() ) data._animation.data()->stop();
            data._animation.data()->setDirection( Animation::Backward );
            data._animation.data()->start();

        }

    }

}
