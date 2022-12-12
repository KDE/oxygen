#ifndef oxygenlabel_datah
#define oxygenlabel_datah

//////////////////////////////////////////////////////////////////////////////
// oxygenlabeldata.h
// data container for QLabel transition
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygentransitiondata.h"

#include <QBasicTimer>
#include <QLabel>
#include <QString>

namespace Oxygen
{

//* generic data
class LabelData : public TransitionData
{
    Q_OBJECT

public:
    //* constructor
    LabelData(QObject *, QLabel *, int);

    //* event filter
    bool eventFilter(QObject *, QEvent *) override;

    //* returns true if animations are locked
    bool isLocked(void) const
    {
        return _animationLockTimer.isActive();
    }

    //* start lock animation timer
    void lockAnimations(void)
    {
        _animationLockTimer.start(_lockTime, this);
    }

    //* start lock animation timer
    void unlockAnimations(void)
    {
        _animationLockTimer.stop();
    }

protected:
    //* timer event
    void timerEvent(QTimerEvent *) override;

protected Q_SLOTS:

    //* initialize animation
    bool initializeAnimation(void) override;

    //* animate
    bool animate(void) override;

private Q_SLOTS:

    //* called when target is destroyed
    void targetDestroyed(void);

private:
    //* true if transparent
    bool transparent(void) const
    {
        return transition() && transition().data()->testFlag(TransitionWidget::Transparent);
    }

private:
    //* lock time (milliseconds
    static const int _lockTime;

    //* timer used to disable animations when triggered too early
    QBasicTimer _animationLockTimer;

    //* needed to start animations out of parent paintEvent
    QBasicTimer _timer;

    //* target
    WeakPointer<QLabel> _target;

    //* old text
    QString _text;

    //* widget rect
    /** needed to properly handle QLabel geometry changes */
    QRect _widgetRect;
};
}

#endif
