//////////////////////////////////////////////////////////////////////////////
// oxygenmenubarengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmenubarengine.h"

#include <QEvent>

namespace Oxygen
{

    //____________________________________________________________
    MenuBarEngineV1::MenuBarEngineV1( QObject* parent, MenuBarBaseEngine* other ):
        MenuBarBaseEngine( parent )
    {
        if( other )
        {
            const auto otherWidgets = other->registeredWidgets();
            for ( QWidget *widget : otherWidgets )
            { registerWidget( widget ); }
        }
    }

    //____________________________________________________________
    bool MenuBarEngineV1::registerWidget( QWidget* widget )
    {

        if( !widget ) return false;

        // create new data class
        if( !_data.contains( widget ) ) _data.insert( widget, new MenuBarDataV1( this, widget, duration() ), enabled() );

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );
        return true;

    }

    //____________________________________________________________
    bool MenuBarEngineV1::isAnimated( const QObject* object, const QPoint& position )
    {
        DataMap<MenuBarDataV1>::Value data( _data.find( object ) );
        if( !data ) return false;
        if( Animation::Pointer animation = data.data()->animation( position ) ) return animation.data()->isRunning();
        else return false;
    }

    //____________________________________________________________
    BaseEngine::WidgetList MenuBarEngineV1::registeredWidgets( void ) const
    {

        WidgetList out ;

        // the typedef is needed to make Krazy happy
        typedef DataMap<MenuBarDataV1>::Value Value;
        for ( const Value &value : std::as_const(_data) )
        { if( value ) out.insert( value.data()->target().data() ); }

        return out;

    }

    //____________________________________________________________
    MenuBarEngineV2::MenuBarEngineV2( QObject* parent, MenuBarBaseEngine* other ):
        MenuBarBaseEngine( parent ),
        _followMouseDuration( 150 )
    {
        if( other )
        {
            const auto otherWidgets = other->registeredWidgets();
            for ( QWidget *widget : otherWidgets )
            { registerWidget( widget ); }
        }
    }

    //____________________________________________________________
    bool MenuBarEngineV2::registerWidget( QWidget* widget )
    {

        if( !widget ) return false;

        // create new data class
        if( !_data.contains( widget ) )
        {
            DataMap<MenuBarDataV2>::Value value( new MenuBarDataV2( this, widget, duration() ) );
            value.data()->setFollowMouseDuration( followMouseDuration() );
            _data.insert( widget, value, enabled() );
        }

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );
        return true;

    }


    //____________________________________________________________
    bool MenuBarEngineV2::isAnimated( const QObject* object, const QPoint& )
    {
        if( !enabled() ) return false;
        DataMap<MenuBarDataV2>::Value data( _data.find( object ) );
        if( !data ) return false;
        if( data.data()->animation() && data.data()->animation().data()->isRunning() ) return true;
        else if( Animation::Pointer animation = data.data()->progressAnimation() ) return animation.data()->isRunning();
        else return false;

    }

    //____________________________________________________________
    QRect MenuBarEngineV2::currentRect( const QObject* object, const QPoint& )
    {
        if( !enabled() ) return QRect();
        DataMap<MenuBarDataV2>::Value data( _data.find( object ) );
        return data ? data.data()->currentRect():QRect();
    }

    //____________________________________________________________
    QRect MenuBarEngineV2::animatedRect( const QObject* object )
    {
        if( !enabled() ) return QRect();
        DataMap<MenuBarDataV2>::Value data( _data.find( object ) );
        return data ? data.data()->animatedRect():QRect();

    }

    //____________________________________________________________
    bool MenuBarEngineV2::isTimerActive( const QObject* object )
    {
        if( !enabled() ) return false;
        DataMap<MenuBarDataV2>::Value data( _data.find( object ) );
        return data ? data.data()->timer().isActive():false;
    }

    //____________________________________________________________
    BaseEngine::WidgetList MenuBarEngineV2::registeredWidgets( void ) const
    {

        WidgetList out;

        // the typedef is needed to make Krazy happy
        typedef DataMap<MenuBarDataV2>::Value Value;
        for ( const Value &value : std::as_const(_data) )
        { if( value ) out.insert( value.data()->target().data() ); }

        return out;

    }

}
