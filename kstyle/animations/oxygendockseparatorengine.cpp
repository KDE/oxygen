//////////////////////////////////////////////////////////////////////////////
// oxygendockseparatorengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygendockseparatorengine.h"

#include <QEvent>

namespace Oxygen
{

    //____________________________________________________________
    bool DockSeparatorEngine::registerWidget( QWidget* widget )
    {

        if( !widget ) return false;

        // create new data class
        if( !_data.contains( widget ) ) _data.insert( widget, new DockSeparatorData( this, widget, duration() ), enabled() );

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );
        return true;

    }

}
