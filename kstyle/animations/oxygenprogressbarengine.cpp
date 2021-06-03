//////////////////////////////////////////////////////////////////////////////
// oxygenprogressbarengine.cpp
// handle progress bar animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenprogressbarengine.h"

namespace Oxygen
{

    //_______________________________________________
    bool ProgressBarEngine::registerWidget( QWidget* widget )
    {

        // check widget validity
        if( !widget ) return false;

        // create new data class
        if( !_data.contains( widget ) ) _data.insert( widget, new ProgressBarData( this, widget, duration() ), enabled() );

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );

        return true;

    }

    //____________________________________________________________
    bool ProgressBarEngine::isAnimated( const QObject* object )
    {

        DataMap<ProgressBarData>::Value data( ProgressBarEngine::data( object ) );
        return ( data && data.data()->animation() && data.data()->animation().data()->isRunning() );

    }

    //____________________________________________________________
    DataMap<ProgressBarData>::Value ProgressBarEngine::data( const QObject* object )
    { return _data.find( object ).data(); }

}
