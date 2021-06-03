#ifndef oxygenenable_data_h
#define oxygenenable_data_h

//////////////////////////////////////////////////////////////////////////////
// oxygenenabledata.h
// generic data container for widget enable/disable animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenwidgetstatedata.h"

namespace Oxygen
{

    //* Enable data
    class EnableData: public WidgetStateData
    {

        Q_OBJECT

        public:

        //* constructor
        EnableData( QObject* parent, QWidget* target, int duration, bool state = true ):
        WidgetStateData( parent, target, duration, state )
        { target->installEventFilter( this ); }

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

    };

}

#endif
