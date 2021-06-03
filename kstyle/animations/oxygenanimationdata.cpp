//////////////////////////////////////////////////////////////////////////////
// oxygenanimationdata.cpp
// base class data container needed for widget animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimationdata.h"

namespace Oxygen
{

    const qreal AnimationData::OpacityInvalid = -1;
    int AnimationData::_steps = 0;

    //_________________________________________________________________________________
    void AnimationData::setupAnimation( const Animation::Pointer& animation, const QByteArray& property )
    {

        // setup animation
        animation.data()->setStartValue( 0.0 );
        animation.data()->setEndValue( 1.0 );
        animation.data()->setTargetObject( this );
        animation.data()->setPropertyName( property );

    }

}
