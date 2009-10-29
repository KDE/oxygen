#ifndef oxygenscrollbarengine_h
#define oxygenscrollbarengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbarengine.h
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

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenscrollbardata.h"

namespace Oxygen
{

    //! stores scrollbar hovered action and timeLine
    class ScrollBarEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //! constructor
        ScrollBarEngine( QObject* parent ):
        BaseEngine( parent )
        {}

        //! destructor
        virtual ~ScrollBarEngine( void )
        {}

        //! register scrollbar
        virtual bool registerWidget( QWidget* );

        //! true if widget is animated
        virtual bool isAnimated( const QObject* object, QStyle::SubControl subcontrol )
        { return (bool) timeLine( object, subcontrol ); }

        //! animation opacity
        virtual qreal opacity( const QObject* object, QStyle::SubControl subcontrol )
        {
            TimeLine::Pointer timeLine( ScrollBarEngine::timeLine( object, subcontrol ) );
            return timeLine ? timeLine->ratio() : -1;
        }

        //! subcontrol rect associated to object
        virtual QRect subControlRect( const QObject*, QStyle::SubControl );

        //! subcontrol rect
        virtual void setSubControlRect( const QObject*, QStyle::SubControl, const QRect& );

        //! enability
        virtual void setEnabled( bool value )
        {
            BaseEngine::setEnabled( value );
            data_.setEnabled( value );
        }

        //! duration
        virtual void setDuration( int value )
        {
            BaseEngine::setDuration( value );
            data_.setDuration( value );
        }

        //! max frame
        virtual void setMaxFrame( int value )
        {
            BaseEngine::setMaxFrame( value );
            data_.setMaxFrame( value );
        }

        protected slots:

        //! remove widget from map
        virtual void unregisterWidget( QObject* object )
        { if( object ) data_.remove( object ); }

        protected:

        //! return timeLine associated to object for given subcontrol, if any
        virtual TimeLine::Pointer timeLine( const QObject*, QStyle::SubControl );

        private:

        //! data map
        DataMap<ScrollBarData> data_;

    };

}

#endif
