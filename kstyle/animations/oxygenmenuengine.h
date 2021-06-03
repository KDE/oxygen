#ifndef oxygenmenuengine_h
#define oxygenmenuengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenuengine.h
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenmenudata.h"

namespace Oxygen
{

    //* stores menu hovered action and timeLine
    class MenuBaseEngine: public BaseEngine
    {
        Q_OBJECT

        public:

        //* constructor
        explicit MenuBaseEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register menubar
        virtual bool registerWidget( QWidget* ) = 0;

        //* true if widget is animated
        virtual bool isAnimated( const QObject*, WidgetIndex )
        { return false; }

        //* opacity
        virtual qreal opacity( const QObject*, WidgetIndex )
        { return -1; }

        //* return 'hover' rect position when widget is animated
        virtual QRect currentRect( const QObject*, WidgetIndex )
        { return QRect(); }

        //* return 'hover' rect position when widget is animated
        virtual QRect animatedRect( const QObject* )
        { return QRect(); }

        //* timer associated to the data
        virtual bool isTimerActive( const QObject* )
        { return false; }

        //* enable state
        void setEnabled( bool value ) override = 0;

        //* duration
        void setDuration( int ) override = 0;

        //* duration
        virtual void setFollowMouseDuration( int )
        {}

    };

    //* stores menu hovered action and timeLine
    class MenuEngineV1: public MenuBaseEngine
    {
        Q_OBJECT

        public:

        //* constructor
        explicit MenuEngineV1( QObject* parent ):
            MenuBaseEngine( parent )
        {}

        //* constructor
        MenuEngineV1( QObject* parent, MenuBaseEngine* other );

        //* register menubar
        bool registerWidget( QWidget* ) override;

        //* true if widget is animated
        bool isAnimated( const QObject* object, WidgetIndex index ) override;

        //* animation opacity
        qreal opacity( const QObject* object, WidgetIndex index ) override
        {
            if( !isAnimated( object, index ) ) return AnimationData::OpacityInvalid;
            else return _data.find(object).data()->opacity( index );
        }

        //* return 'hover' rect position when widget is animated
        QRect currentRect( const QObject* object, WidgetIndex index ) override
        {
            if( !isAnimated( object, index ) ) return QRect();
            else return _data.find(object).data()->currentRect( index );
        }

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

        public Q_SLOTS:

        //* remove widget from map
        bool unregisterWidget( QObject* object ) override
        { return _data.unregisterWidget( object ); }

        private:

        //* data map
        DataMap<MenuDataV1> _data;

    };

    //* stores menu hovered action and timeLine
    class MenuEngineV2: public MenuBaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit MenuEngineV2( QObject* parent ):
            MenuBaseEngine( parent )
        {}

        //* constructor
        MenuEngineV2( QObject* parent, MenuBaseEngine* other );

        //* register menu
        bool registerWidget( QWidget* ) override;

        //* return timeLine associated to action at given position, if any
        bool isAnimated( const QObject*, WidgetIndex ) override;

        //* animation opacity
        qreal opacity( const QObject* object, WidgetIndex index ) override
        {
            if( !isAnimated( object, index ) ) return AnimationData::OpacityInvalid;
            else return _data.find(object).data()->opacity();
        }

        //* return 'hover' rect position when widget is animated
        QRect currentRect( const QObject* object, WidgetIndex index ) override;

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
        virtual int followMouseDuration( void ) const
        { return _followMouseDuration; }

        //* duration
        void setFollowMouseDuration( int duration ) override
        {
            _followMouseDuration = duration;
            foreach( const DataMap<MenuDataV2>::Value& value, _data )
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
        DataMap<MenuDataV2> _data;

    };

}

#endif
