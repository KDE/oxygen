#ifndef oxygencombobox_datah
#define oxygencombobox_datah

//////////////////////////////////////////////////////////////////////////////
// oxygencomboboxdata.h
// data container for QComboBox transition
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygentransitiondata.h"

#include <QBasicTimer>
#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QTimerEvent>

namespace Oxygen
{

//* generic data
class ComboBoxData : public TransitionData
{
    Q_OBJECT

public:
    //* constructor
    ComboBoxData(QObject *, QComboBox *, int);

    //* event filter
    bool eventFilter(QObject *, QEvent *) override;

protected:
    //* timer event
    void timerEvent(QTimerEvent *) override;

protected Q_SLOTS:

    //* initialize animation
    bool initializeAnimation(void) override;

    //* animate
    bool animate(void) override;

private Q_SLOTS:

    //* triggered when item is activated in combobox
    void indexChanged(void);

    //* called when target is destroyed
    void targetDestroyed(void);

private:
    //* target rect
    /** return rect corresponding to the area to be updated when animating */
    QRect targetRect(void) const
    {
        return _target ? _target.data()->rect().adjusted(5, 5, -5, -5) : QRect();
    }

    //* needed to start animations out of parent paintEvent
    QBasicTimer _timer;

    //* target
    WeakPointer<QComboBox> _target;
};
}

#endif
