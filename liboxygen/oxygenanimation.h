#ifndef oxygenanimation_h
#define oxygenanimation_h
//////////////////////////////////////////////////////////////////////////////
// oxygenanimation.h
// stores event filters and maps widgets to animations for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QPointer>
#include <QPropertyAnimation>
#include <QVariant>

#include "oxygen_export.h"

#include "liboxygen.h"

namespace Oxygen
{

class OXYGEN_EXPORT Animation : public QPropertyAnimation
{
    Q_OBJECT

public:
    //! TimeLine shared pointer
    using Pointer = WeakPointer<Animation>;

    //! constructor
    Animation(int duration, QObject *parent)
        : QPropertyAnimation(parent)
    {
        setDuration(duration);
    }

    //! true if running
    bool isRunning(void) const
    {
        return state() == Animation::Running;
    }

    //! restart
    void restart(void)
    {
        if (isRunning())
            stop();
        start();
    }
};
}

#endif
