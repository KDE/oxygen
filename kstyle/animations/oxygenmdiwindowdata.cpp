//////////////////////////////////////////////////////////////////////////////
// oxygenmdiwindowdata.cpp
// mdi window data container for window titlebar buttons
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmdiwindowdata.h"

namespace Oxygen
{

//________________________________________________
MdiWindowData::MdiWindowData(QObject *parent, QWidget *target, int duration)
    : AnimationData(parent, target)
{
    _currentData._animation = new Animation(duration, this);
    _previousData._animation = new Animation(duration, this);
    setupAnimation(currentAnimation(), "currentOpacity");
    setupAnimation(previousAnimation(), "previousOpacity");

    currentAnimation().data()->setDirection(Animation::Forward);
    previousAnimation().data()->setDirection(Animation::Backward);
}

//______________________________________________
bool MdiWindowData::updateState(int primitive, bool state)
{
    if (state) {
        if (primitive != _currentData._primitive) {
            _previousData.updateSubControl(_currentData._primitive);
            _currentData.updateSubControl(primitive);
            return true;

        } else
            return false;

    } else {
        bool changed(false);
        if (primitive == _currentData._primitive) {
            changed |= _currentData.updateSubControl(0);
            changed |= _previousData.updateSubControl(primitive);
        }

        return changed;
    }
}

//______________________________________________
bool MdiWindowData::Data::updateSubControl(int value)
{
    if (_primitive == value)
        return false;
    else {
        _primitive = value;
        if (_animation.data()->isRunning())
            _animation.data()->stop();
        if (_primitive != 0)
            _animation.data()->start();
        return true;
    }
}
}
