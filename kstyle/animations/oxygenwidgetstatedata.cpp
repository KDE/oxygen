//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetstatedata.cpp
// generic data container for widget widgetstate (mouse-over) animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenwidgetstatedata.h"

namespace Oxygen
{

//______________________________________________
bool WidgetStateData::updateState(bool value)
{
    if (_state == value)
        return false;
    else {
        _state = value;
        animation().data()->setDirection(_state ? Animation::Forward : Animation::Backward);
        if (!animation().data()->isRunning())
            animation().data()->start();
        return true;
    }
}
}
