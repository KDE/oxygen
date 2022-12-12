#ifndef oxygendemowidget_h
#define oxygendemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygendemowidget.h
// base class for oxygen demo widgets
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QWidget>

#include "oxygensimulator.h"

namespace Oxygen
{

class DemoWidget : public QWidget
{
    Q_OBJECT

public:
    //! constructo
    explicit DemoWidget(QWidget *parent)
        : QWidget(parent)
        , _simulator(new Simulator(this))
    {
    }

    //! simulator
    Simulator &simulator(void) const
    {
        return *_simulator;
    }

private:
    //! simulator
    Simulator *_simulator;
};
}

#endif
