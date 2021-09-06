//////////////////////////////////////////////////////////////////////////////
// oxygentoolbarengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygentoolbarengine.h"

#include <QEvent>

namespace Oxygen
{

    //____________________________________________________________
    void ToolBarEngine::registerWidget( QWidget* widget )
    {

        if( !widget ) return;

        // create new data class
        if( !_data.contains( widget ) )
        {
            DataMap<ToolBarData>::Value value( new ToolBarData( this, widget, duration() ) );
            value.data()->setFollowMouseDuration( followMouseDuration() );
            _data.insert( widget, value, enabled() );
        }

        // connect destruction signal
        disconnect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)) );
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)) );

    }

    //____________________________________________________________
    BaseEngine::WidgetList ToolBarEngine::registeredWidgets( void ) const
    {
        WidgetList out;

        // the typedef is needed to make Krazy happy
        typedef DataMap<ToolBarData>::Value Value;
        for ( const Value &value : std::as_const(_data) )
        { if( value ) out.insert( value.data()->target().data() ); }
        return out;
    }

    //____________________________________________________________
    bool ToolBarEngine::isAnimated( const QObject* object )
    {
        if( !enabled() ) return false;

        DataMap<ToolBarData>::Value data( _data.find( object ) );
        if( !data ) return false;
        if( Animation::Pointer animation = data.data()->animation() ) return animation.data()->isRunning();
        else return false;
    }

    //____________________________________________________________
    bool ToolBarEngine::isFollowMouseAnimated( const QObject* object )
    {
        if( !enabled() ) return false;

        DataMap<ToolBarData>::Value data( _data.find( object ) );
        if( !data ) return false;
        if( Animation::Pointer animation = data.data()->progressAnimation() ) return animation.data()->isRunning();
        else return false;
    }

    //____________________________________________________________
    QRect ToolBarEngine::currentRect( const QObject* object )
    {
        if( !enabled() ) return QRect();
        DataMap<ToolBarData>::Value data( _data.find( object ) );
        return data ? data.data()->currentRect():QRect();
    }

    //____________________________________________________________
    QRect ToolBarEngine::animatedRect( const QObject* object )
    {
        if( !enabled() ) return QRect();
        DataMap<ToolBarData>::Value data( _data.find( object ) );
        return data ? data.data()->animatedRect():QRect();
    }

    //____________________________________________________________
    bool ToolBarEngine::isTimerActive( const QObject* object )
    {
        if( !enabled() ) return false;
        DataMap<ToolBarData>::Value data( _data.find( object ) );
        return data ? data.data()->timer().isActive():false;
    }

}
