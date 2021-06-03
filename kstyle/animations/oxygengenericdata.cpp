//////////////////////////////////////////////////////////////////////////////
// oxygengenericdata.cpp
// generic data container for widget animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygengenericdata.h"


namespace Oxygen
{

    //______________________________________________
    GenericData::GenericData( QObject* parent, QWidget* target, int duration ):
        AnimationData( parent, target ),
        _animation( new Animation( duration, this ) )
    { setupAnimation( _animation, "opacity" ); }

}
