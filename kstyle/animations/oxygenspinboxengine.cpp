//////////////////////////////////////////////////////////////////////////////
// oxygenspinboxengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenspinboxengine.h"

#include <QEvent>

namespace Oxygen
{

//____________________________________________________________
bool SpinBoxEngine::registerWidget(QWidget *widget)
{
    if (!widget)
        return false;

    // create new data class
    if (!_data.contains(widget))
        _data.insert(widget, new SpinBoxData(this, widget, duration()), enabled());

    // connect destruction signal
    connect(widget, SIGNAL(destroyed(QObject *)), this, SLOT(unregisterWidget(QObject *)), Qt::UniqueConnection);
    return true;
}
}
