//////////////////////////////////////////////////////////////////////////////
// oxygendockseparatordata.cpp
// generic data container for widgetstate hover (mouse-over) animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygendockseparatordata.h"

#include <QTextStream>
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
