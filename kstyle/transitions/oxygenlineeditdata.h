#ifndef oxygenlineedit_datah
#define oxygenlineedit_datah

//////////////////////////////////////////////////////////////////////////////
// oxygenlineeditdata.h
// data container for QLineEdit transition
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygentransitiondata.h"

#include <QBasicTimer>
#include <QLineEdit>
#include <QString>

namespace Oxygen
{

//* generic data
class LineEditData : public TransitionData
{
    Q_OBJECT

public:
    //* constructor
    LineEditData(QObject *, QLineEdit *, int);

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

protected Q_SLOTS:

    //* initialize animation
    bool initializeAnimation(void) override;

    //* animate
    bool animate(void) override;

protected:
    //* timer event
    void timerEvent(QTimerEvent *) override;

private Q_SLOTS:

    //* text edited
    void textEdited(void);

    //* selection changed
    void selectionChanged(void);

    //* text changed
    void textChanged(void);

    //* called when target is destroyed
    void targetDestroyed(void);

private:
    //* target rect
    /** return rect corresponding to the area to be updated when animating */
    QRect targetRect(void) const
    {
        if (!_target)
            return QRect();
        QRect out(_target.data()->rect());
        if (_hasClearButton && _clearButtonRect.isValid()) {
            out.setRight(_clearButtonRect.left());
        }

        return out;
    }

    //* check if target has clear button
    void checkClearButton(void);

    //* lock time (milliseconds
    static const int _lockTime;

    //* timer used to disable animations when triggered too early
    QBasicTimer _animationLockTimer;

    //* needed to start animations out of parent paintEvent
    QBasicTimer _timer;

    //* target
    WeakPointer<QLineEdit> _target;

    //* true if target has clean button
    bool _hasClearButton;

    //* clear button rect
    QRect _clearButtonRect;

    //* true if text was manually edited
    /** needed to trigger animation only on programatically enabled text */
    bool _edited;

    //* old text
    QString _text;

    //* widget rect
    /** needed to properly handle QLabel geometry changes */
    QRect _widgetRect;
};
}

#endif
