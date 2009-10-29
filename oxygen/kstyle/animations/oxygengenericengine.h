#ifndef oxygengenericengine_h
#define oxygengenericengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygengenericengine.h
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
#include "oxygenhoverdata.h"
#include "oxygenfocusdata.h"


namespace Oxygen
{

    //! animation mode
    enum AnimationMode
    {
        AnimationNone = 0,
        AnimationHover = 1<<0,
        AnimationFocus = 1<<1
    };

    Q_DECLARE_FLAGS(AnimationModes, AnimationMode)

    //! used for simple widgets
    class GenericEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //! constructor
        GenericEngine( QObject* parent ):
        BaseEngine( parent )
        {}

        //! destructor
        virtual ~GenericEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( QWidget*, unsigned int mode );

        //! true if widget is animated
        virtual bool isAnimated( const QObject* object, AnimationMode mode )
        { return (bool) timeLine( object, mode ); }

        //! animation opacity
        virtual qreal opacity( const QObject* object, AnimationMode mode )
        {
            TimeLine::Pointer timeLine( GenericEngine::timeLine( object, mode ) );
            return timeLine ? timeLine->ratio() : -1;
        }

        //! duration
        virtual void setEnabled( bool value )
        {
            BaseEngine::setEnabled( value );
            hoverData_.setEnabled( value );
            focusData_.setEnabled( value );
        }

        //! duration
        virtual void setDuration( int value )
        {
            BaseEngine::setDuration( value );
            hoverData_.setDuration( value );
            focusData_.setDuration( value );
        }

        //! max frame
        virtual void setMaxFrame( int value )
        {
            BaseEngine::setMaxFrame( value );
            hoverData_.setMaxFrame( value );
            focusData_.setMaxFrame( value );
        }

        protected slots:

        //! remove widget from map
        virtual void unregisterWidget( QObject* object )
        {
            if( object )
            {
                hoverData_.remove( object );
                focusData_.remove( object );
            }
        }

        protected:

        //! returns timeLine associated to widget
        TimeLine::Pointer timeLine( const QObject*, AnimationMode );

        private:

        //! maps
        DataMap<HoverData> hoverData_;
        DataMap<FocusData> focusData_;

    };

}

#endif

