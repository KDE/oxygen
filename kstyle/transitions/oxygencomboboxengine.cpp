// krazy:excludeall=qclasses

//////////////////////////////////////////////////////////////////////////////
// oxygencomboboxengine.cpp
// stores event filters and maps widgets to animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygencomboboxengine.h"

namespace Oxygen
{

    //____________________________________________________________
    bool ComboBoxEngine::registerWidget( QComboBox* widget )
    {

        if( !widget ) return false;
        if( !_data.contains( widget ) ) { _data.insert( widget, new ComboBoxData( this, widget, duration() ), enabled() ); }

        // connect destruction signal
        disconnect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)) );
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)) );

        return true;

    }

}
