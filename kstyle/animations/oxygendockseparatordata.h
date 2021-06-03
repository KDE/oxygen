#ifndef oxygendockseparator_data_h
#define oxygendockseparator_data_h

//////////////////////////////////////////////////////////////////////////////
// oxygendockseparatordata.h
// generic data container for widgetstate hover (mouse-over) animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygengenericdata.h"
#include "oxygenanimation.h"

namespace Oxygen
{

    //* dock widget splitters hover effect
    class DockSeparatorData: public AnimationData
    {

        Q_OBJECT

        //* declare opacity property
        Q_PROPERTY( qreal verticalOpacity READ verticalOpacity WRITE setVerticalOpacity )
        Q_PROPERTY( qreal horizontalOpacity READ horizontalOpacity WRITE setHorizontalOpacity )

        public:

        //* constructor
        DockSeparatorData( QObject* parent, QWidget* target, int duration );

        //@}

        /*!
        returns true if hover has Changed
        and starts timer accordingly
        */
        void updateRect( const QRect&, const Qt::Orientation&, bool hovered );

        //* returns true if current splitter is animated
        bool isAnimated( QRect r, const Qt::Orientation& orientation ) const
        { return orientation == Qt::Vertical ? _verticalData.isAnimated( r ):_horizontalData.isAnimated( r ); }

        //* opacity for given orientation
        qreal opacity( const Qt::Orientation& orientation ) const
        { return orientation == Qt::Vertical ? verticalOpacity():horizontalOpacity(); }

        //* duration
        void setDuration( int duration ) override
        {
            horizontalAnimation().data()->setDuration( duration );
            verticalAnimation().data()->setDuration( duration );
        }

        //*@name horizontal splitter data
        //@{

        Animation::Pointer horizontalAnimation( void ) const
        { return _horizontalData._animation; }

        const QRect& horizontalRect( void ) const
        { return _horizontalData._rect; }

        void setHorizontalRect( const QRect& r )
        { _horizontalData._rect = r; }

        qreal horizontalOpacity( void ) const
        { return _horizontalData._opacity; }

        void setHorizontalOpacity( qreal value )
        {

            value = digitize( value );
            if( _horizontalData._opacity == value ) return;
            _horizontalData._opacity = value;
            if( target() && !horizontalRect().isEmpty() ) target().data()->update( horizontalRect() );

        }

        //@}


        //*@name vertical splitter data
        //@{

        Animation::Pointer verticalAnimation( void ) const
        { return _verticalData._animation; }

        const QRect& verticalRect( void ) const
        { return _verticalData._rect; }

        void setVerticalRect( const QRect& r )
        { _verticalData._rect = r; }

        qreal verticalOpacity( void ) const
        { return _verticalData._opacity; }

        void setVerticalOpacity( qreal value )
        {
            value = digitize( value );
            if( _verticalData._opacity == value ) return;
            _verticalData._opacity = value;
            if( target() && !verticalRect().isEmpty() ) target().data()->update( verticalRect() );
        }

        //@}


        private:

        //* stores data needed for animation
        class Data
        {

            public:

            //* constructor
            Data( void ):
                _opacity( AnimationData::OpacityInvalid )
                {}

            //* true if is animated
            bool isAnimated( QRect r ) const
            { return r == _rect && _animation.data()->isRunning(); }

            //* animation pointer
            Animation::Pointer _animation;

            //* opacity variable
            qreal _opacity;

            //* stores active separator rect
            QRect _rect;

        };

        //* horizontal
        Data _horizontalData;

        //* vertical
        Data _verticalData;

    };

}

#endif
