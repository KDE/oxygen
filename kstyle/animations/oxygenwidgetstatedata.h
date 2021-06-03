#ifndef oxygenwidgetstatedata_h
#define oxygenwidgetstatedata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetstatedata.h
// generic data container for widgetstate hover (mouse-over) animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygengenericdata.h"

namespace Oxygen
{

    //* handle widget state (hover/focus/enable) changes
    class WidgetStateData: public GenericData
    {

        Q_OBJECT

        public:

        //* constructor
        WidgetStateData( QObject* parent, QWidget* target, int duration, bool state = false ):
            GenericData( parent, target, duration ),
            _state( state )
        {}

        /**
        returns true if hover has Changed
        and starts timer accordingly
        */
        bool updateState( bool value );

        private:

        bool _state = false;

    };

}

#endif
