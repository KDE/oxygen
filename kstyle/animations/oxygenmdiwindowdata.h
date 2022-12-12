#ifndef oxygenmdiwindow_data_h
#define oxygenmdiwindow_data_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmdiwindowdata.h
// mdi window data container for window titlebar buttons
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimationdata.h"

#include <QStyle>

namespace Oxygen
{

//* handles mdiwindow arrows hover
class MdiWindowData : public AnimationData
{
    Q_OBJECT

    //* declare opacity property
    Q_PROPERTY(qreal currentOpacity READ currentOpacity WRITE setCurrentOpacity)
    Q_PROPERTY(qreal previousOpacity READ previousOpacity WRITE setPreviousOpacity)

public:
    //* constructor
    MdiWindowData(QObject *, QWidget *, int);

    //* animation state
    bool updateState(int primitive, bool value);

    //* animation state
    bool isAnimated(int primitive) const
    {
        return ((primitive == _currentData._primitive && currentAnimation().data()->isRunning())
                || (primitive == _previousData._primitive && previousAnimation().data()->isRunning()));
    }

    //* opacity
    qreal opacity(int primitive) const
    {
        if (primitive == _currentData._primitive)
            return currentOpacity();
        else if (primitive == _previousData._primitive)
            return previousOpacity();
        else
            return OpacityInvalid;
    }

    //* duration
    void setDuration(int duration) override
    {
        currentAnimation().data()->setDuration(duration);
        previousAnimation().data()->setDuration(duration);
    }

    //*@name current animation
    //@{

    //* opacity
    qreal currentOpacity(void) const
    {
        return _currentData._opacity;
    }

    //* opacity
    void setCurrentOpacity(qreal value)
    {
        value = digitize(value);
        if (_currentData._opacity == value)
            return;
        _currentData._opacity = value;
        setDirty();
    }

    //* animation
    Animation::Pointer currentAnimation(void) const
    {
        return _currentData._animation;
    }

    //@}
    //*@name previous animation
    //@{

    //* opacity
    qreal previousOpacity(void) const
    {
        return _previousData._opacity;
    }

    //* opacity
    void setPreviousOpacity(qreal value)
    {
        value = digitize(value);
        if (_previousData._opacity == value)
            return;
        _previousData._opacity = value;
        setDirty();
    }

    //* animation
    Animation::Pointer previousAnimation(void) const
    {
        return _previousData._animation;
    }

    //@}

private:
    //* container for needed animation data
    class Data
    {
    public:
        //* default constructor
        Data() = default;

        //* subcontrol
        bool updateSubControl(int);

        //* subcontrol
        int _primitive = 0;

        //* animation
        Animation::Pointer _animation;

        //* opacity
        qreal _opacity = 0;
    };

    //* current data
    Data _currentData;

    //* previous data
    Data _previousData;
};
}

#endif
