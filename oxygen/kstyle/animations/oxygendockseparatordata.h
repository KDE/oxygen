#ifndef oxygendockseparator_datah
#define oxygendockseparator_datah

//////////////////////////////////////////////////////////////////////////////
// oxygendockseparatordata.h
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

#include "oxygengenericdata.h"
#include "oxygenanimation.h"

namespace Oxygen
{

    //! dock widget splitters hover effect
    class DockSeparatorData: public AnimationData
    {

        Q_OBJECT

        //! declare opacity property
        Q_PROPERTY( qreal verticalOpacity READ verticalOpacity WRITE setVerticalOpacity )
        Q_PROPERTY( qreal horizontalOpacity READ horizontalOpacity WRITE setHorizontalOpacity )

        public:

        //! constructor
        DockSeparatorData( QObject* parent, QWidget* target, int duration );

        //! destructor
        virtual ~DockSeparatorData( void )
        {}

        //@}

        /*!
        returns true if hover has Changed
        and starts timer accordingly
        */
        virtual void updateRect( const QRect&, const Qt::Orientation&, bool hovered );

        //! returns true if current splitter is animated
        virtual bool isAnimated( QRect r, const Qt::Orientation& orientation ) const
        { return orientation == Qt::Vertical ? _verticalData.isAnimated( r ):_horizontalData.isAnimated( r ); }

        //! opacity for given orientation
        qreal opacity( const Qt::Orientation& orientation ) const
        { return orientation == Qt::Vertical ? verticalOpacity():horizontalOpacity(); }

        //! duration
        virtual void setDuration( int duration )
        {
            horizontalAnimation().data()->setDuration( duration );
            verticalAnimation().data()->setDuration( duration );
        }

        //!@name horizontal splitter data
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


        //!@name vertical splitter data
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

        //! stores data needed for animation
        class Data
        {

            public:

            //! constructor
            Data( void ):
                _opacity( AnimationData::OpacityInvalid )
                {}

            //! true if is animated
            bool isAnimated( QRect r ) const
            { return r == _rect && _animation.data()->isRunning(); }

            //! animation pointer
            Animation::Pointer _animation;

            //! opacity variable
            qreal _opacity;

            //! stores active separator rect
            QRect _rect;

        };

        //! horizontal
        Data _horizontalData;

        //! vertical
        Data _verticalData;

    };

}

#endif
