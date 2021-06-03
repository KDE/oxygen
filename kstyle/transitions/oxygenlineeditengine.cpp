// krazy:excludeall=qclasses

//////////////////////////////////////////////////////////////////////////////
// oxygenlineeditengine.cpp
// stores event filters and maps widgets to animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenlineeditengine.h"

namespace Oxygen
{

    //____________________________________________________________
    bool LineEditEngine::registerWidget( QLineEdit* widget )
    {

        // check enability and widget validity
        if( !widget ) return false;

        // do not register widget if painted in a scene
        if( widget->graphicsProxyWidget() ) return false;

        // insert in map if needed
        if( !_data.contains( widget ) ) {

            _data.insert( widget, new LineEditData( this, widget, duration() ), enabled() );

        }

        // connect destruction signal
        disconnect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)) );
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)) );

        return true;

    }

}
