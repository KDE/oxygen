#ifndef oxygenmenubarengine_h
#define oxygenmenubarengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenubarengine.h
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
#include "oxygenmenubardata.h"

namespace Oxygen
{

    //* stores menubar hovered action and timeLine
    class MenuBarBaseEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit MenuBarBaseEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register menubar
        virtual bool registerWidget( QWidget* ) = 0;

        //* true if widget is animated
        virtual bool isAnimated( const QObject*, const QPoint& )
        { return false; }

        //* animation opacity
        virtual qreal opacity( const QObject*, const QPoint& )
        { return -1; }

        //* return 'hover' rect position when widget is animated
        virtual QRect currentRect( const QObject*, const QPoint& )
        { return QRect(); }

        //* animated rect
        virtual QRect animatedRect( const QObject* )
        { return QRect(); }

        //* timer
        virtual bool isTimerActive( const QObject* )
        { return false; }

        //* enable state
        void setEnabled( bool ) override = 0;

        //* duration
        void setDuration( int ) override = 0;

        //* duration
        virtual void setFollowMouseDuration( int )
        {}

    };

    //* fading menubar animation
    class MenuBarEngineV1: public MenuBarBaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit MenuBarEngineV1( QObject* parent ):
            MenuBarBaseEngine( parent )
        {}

        //* constructor
        MenuBarEngineV1( QObject* parent, MenuBarBaseEngine* other );

        //* register menubar
        bool registerWidget( QWidget* ) override;

        //* true if widget is animated
        bool isAnimated( const QObject* object, const QPoint& point ) override;

        //* animation opacity
        qreal opacity( const QObject* object, const QPoint& point ) override
        { return isAnimated( object, point ) ? _data.find( object ).data()->opacity( point ): AnimationData::OpacityInvalid; }

        //* return 'hover' rect position when widget is animated
        QRect currentRect( const QObject* object, const QPoint& point) override
        { return isAnimated( object, point ) ? _data.find( object ).data()->currentRect( point ): QRect(); }

        //* enable state
        void setEnabled( bool value ) override
        {
            BaseEngine::setEnabled( value );
            _data.setEnabled( value );
        }

        //* duration
        void setDuration( int duration ) override
        {
            BaseEngine::setDuration( duration );
            _data.setDuration( duration );
        }

        //* return list of registered widgets
        WidgetList registeredWidgets( void ) const override;

        protected Q_SLOTS:

        //* remove widget from map
        bool unregisterWidget( QObject* object ) override
        { return _data.unregisterWidget( object ); }

        private:

        //* data map
        DataMap<MenuBarDataV1> _data;

    };

    //* follow-mouse menubar animation
    class MenuBarEngineV2: public MenuBarBaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit MenuBarEngineV2( QObject* parent ):
            MenuBarBaseEngine( parent )
        {}

        //* constructor
        MenuBarEngineV2( QObject* parent, MenuBarBaseEngine* other );

        //* register menubar
        bool registerWidget( QWidget* ) override;

        //* true if widget is animated
        bool isAnimated( const QObject* object, const QPoint& point ) override;

        //* animation opacity
        qreal opacity( const QObject* object, const QPoint& point ) override
        { return isAnimated( object, point ) ? _data.find( object ).data()->opacity(): AnimationData::OpacityInvalid; }

        //* return 'hover' rect position when widget is animated
        QRect currentRect( const QObject*, const QPoint& ) override;

        //* return 'hover' rect position when widget is animated
        QRect animatedRect( const QObject* ) override;

        //* timer associated to the data
        bool isTimerActive( const QObject* ) override;

        //* enable state
        void setEnabled( bool value ) override
        {
            BaseEngine::setEnabled( value );
            _data.setEnabled( value );
        }

        //* duration
        void setDuration( int value ) override
        {
            BaseEngine::setDuration( value );
            _data.setDuration( value );
        }

        //* duration
        int followMouseDuration( void ) const
        { return _followMouseDuration; }

        //* duration
        void setFollowMouseDuration( int duration ) override
        {
            _followMouseDuration = duration;
            foreach( const DataMap<MenuBarDataV2>::Value& value, _data )
            { if( value ) value.data()->setFollowMouseDuration( duration ); }
        }

        //* return list of registered widgets
        WidgetList registeredWidgets( void ) const override;

        protected Q_SLOTS:

        //* remove widget from map
        bool unregisterWidget( QObject* object ) override
        { return _data.unregisterWidget( object ); }

        private:

        //* follow mouse animation duration
        int _followMouseDuration;

        //* data map
        DataMap<MenuBarDataV2> _data;

    };

}

#endif
