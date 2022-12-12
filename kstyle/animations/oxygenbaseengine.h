#ifndef oxygenbaseengine_h
#define oxygenbaseengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbaseengine.h
// base engine
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygen.h"

#include <QObject>
#include <QSet>

namespace Oxygen
{

//* base class for all animation engines
/** it is used to store configuration values used by all animations stored in the engine */
class BaseEngine : public QObject
{
    Q_OBJECT

public:
    using Pointer = WeakPointer<BaseEngine>;

    //* constructor
    explicit BaseEngine(QObject *parent)
        : QObject(parent)
    {
    }

    //*@name accessors
    //@{

    //* enability
    virtual bool enabled(void) const
    {
        return _enabled;
    }

    //* duration
    virtual int duration(void) const
    {
        return _duration;
    }

    //* list of widgets
    using WidgetList = QSet<QWidget *>;

    //* returns registered widgets
    virtual WidgetList registeredWidgets(void) const
    {
        return WidgetList();
    }

    //@}

    //*@name modifiers
    //@{

    //* enability
    virtual void setEnabled(bool value)
    {
        _enabled = value;
    }

    //* duration
    virtual void setDuration(int value)
    {
        _duration = value;
    }

    //* unregister widget
    virtual bool unregisterWidget(QObject *object) = 0;

    //@}

private:
    //* engine enability
    bool _enabled = true;

    //* animation duration
    int _duration = 200;
};
}

#endif
