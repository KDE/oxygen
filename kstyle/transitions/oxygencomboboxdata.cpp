// krazy:excludeall=qclasses

//////////////////////////////////////////////////////////////////////////////
// oxygencomboboxdata.cpp
// data container for QComboBox transition
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygencomboboxdata.h"

namespace Oxygen
{

//______________________________________________________
ComboBoxData::ComboBoxData(QObject *parent, QComboBox *target, int duration)
    : TransitionData(parent, target, duration)
    , _target(target)
{
    _target.data()->installEventFilter(this);
    connect(_target.data(), SIGNAL(destroyed()), SLOT(targetDestroyed()));
    connect(_target.data(), SIGNAL(currentIndexChanged(int)), SLOT(indexChanged()));
}

//___________________________________________________________________
void ComboBoxData::indexChanged(void)
{
    if (recursiveCheck())
        return;

    if (transition().data()->isAnimated()) {
        transition().data()->endAnimation();
    }

    if (initializeAnimation())
        animate();
    else
        transition().data()->hide();
}

//___________________________________________________________________
bool ComboBoxData::eventFilter(QObject *object, QEvent *event)
{
    // make sure engine is enabled
    if (!(enabled() && object == _target.data())) {
        return TransitionData::eventFilter(object, event);
    }

    // make sure that target is not editable
    if (_target.data()->isEditable()) {
        return TransitionData::eventFilter(object, event);
    }

    switch (event->type()) {
    case QEvent::Show:
    case QEvent::Resize:
    case QEvent::Move:
        if (!recursiveCheck() && _target.data()->isVisible()) {
            _timer.start(0, this);
        }
        break;

    default:
        break;
    }

    return TransitionData::eventFilter(object, event);
}

//___________________________________________________________________
void ComboBoxData::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _timer.timerId()) {
        _timer.stop();
        if (enabled() && transition() && _target && !_target.data()->isVisible()) {
            setRecursiveCheck(true);
            transition().data()->setEndPixmap(transition().data()->grab(_target.data(), targetRect()));
            setRecursiveCheck(false);
        }

    } else
        return TransitionData::timerEvent(event);
}

//___________________________________________________________________
bool ComboBoxData::initializeAnimation(void)
{
    if (!(enabled() && _target && _target.data()->isVisible()))
        return false;
    if (_target.data()->isEditable()) {
        /*
        do nothing for editable comboboxes because
        lineEditor animations are handled directly
        */
        return false;
    }

    transition().data()->setOpacity(0);
    transition().data()->setGeometry(targetRect());
    transition().data()->setStartPixmap(transition().data()->currentPixmap());
    transition().data()->show();
    transition().data()->raise();
    return true;
}

//___________________________________________________________________
bool ComboBoxData::animate(void)
{
    // check enability
    if (!enabled())
        return false;

    // grab
    setRecursiveCheck(true);
    transition().data()->setEndPixmap(transition().data()->grab(_target.data(), targetRect()));
    setRecursiveCheck(false);

    // start animation
    transition().data()->animate();

    return true;
}

//___________________________________________________________________
void ComboBoxData::targetDestroyed(void)
{
    setEnabled(false);
    _target.clear();
}
}
