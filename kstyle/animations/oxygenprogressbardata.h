#ifndef oxygenprogressbardata_h
#define oxygenprogressbardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenprogressbardata.h
// data container for progressbar animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygengenericdata.h"
#include <QObject>
#include <QTextStream>

namespace Oxygen
{

//* generic data
class ProgressBarData : public GenericData
{
    Q_OBJECT

public:
    //* constructor
    ProgressBarData(QObject *parent, QWidget *widget, int duration);

    //* event filter
    bool eventFilter(QObject *, QEvent *) override;

    //* progressbar value (during animation)
    int value(void) const
    {
        return _startValue + opacity() * (_endValue - _startValue);
    }

private Q_SLOTS:

    //* triggered by progressBar::valueChanged
    void valueChanged(int);

private:
    //* animation starting value
    int _startValue = 0;

    //* animation ending value
    int _endValue = 0;
};
}

#endif
