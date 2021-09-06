
//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetstateengine.h
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenwidgetstateengine.h"

#include "oxygenenabledata.h"

namespace Oxygen
{

    //____________________________________________________________
    bool WidgetStateEngine::registerWidget( QWidget* widget, AnimationModes mode )
    {

        if( !widget ) return false;
        if( mode&AnimationHover && !_hoverData.contains( widget ) ) { _hoverData.insert( widget, new WidgetStateData( this, widget, duration() ), enabled() ); }
        if( mode&AnimationFocus && !_focusData.contains( widget ) ) { _focusData.insert( widget, new WidgetStateData( this, widget, duration() ), enabled() ); }
        if( mode&AnimationEnable && !_enableData.contains( widget ) ) { _enableData.insert( widget, new EnableData( this, widget, duration() ), enabled() ); }

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );

        return true;

    }

    //____________________________________________________________
    BaseEngine::WidgetList WidgetStateEngine::registeredWidgets( AnimationModes mode ) const
    {

        WidgetList out;

        // the typedef is needed to make Krazy happy
        typedef DataMap<WidgetStateData>::Value Value;

        if( mode&AnimationHover )
        {
            for ( const Value &value : std::as_const(_hoverData) )
            { if( value ) out.insert( value.data()->target().data() ); }
        }

        if( mode&AnimationFocus )
        {
            for ( const Value &value : std::as_const(_focusData) )
            { if( value ) out.insert( value.data()->target().data() ); }
        }

        if( mode&AnimationEnable )
        {
            for ( const Value &value : std::as_const(_enableData) )
            { if( value ) out.insert( value.data()->target().data() ); }
        }

        return out;

    }

    //____________________________________________________________
    bool WidgetStateEngine::updateState( const QObject* object, AnimationMode mode, bool value )
    {
        DataMap<WidgetStateData>::Value data( WidgetStateEngine::data( object, mode ) );
        return ( data && data.data()->updateState( value ) );
    }

    //____________________________________________________________
    bool WidgetStateEngine::isAnimated( const QObject* object, AnimationMode mode )
    {

        DataMap<WidgetStateData>::Value data( WidgetStateEngine::data( object, mode ) );
        return ( data && data.data()->animation() && data.data()->animation().data()->isRunning() );

    }

    //____________________________________________________________
    DataMap<WidgetStateData>::Value WidgetStateEngine::data( const QObject* object, AnimationMode mode )
    {

        switch( mode )
        {
            case AnimationHover: return _hoverData.find( object ).data();
            case AnimationFocus: return _focusData.find( object ).data();
            case AnimationEnable: return _enableData.find( object ).data();
            default: return DataMap<WidgetStateData>::Value();
        }

    }

}
