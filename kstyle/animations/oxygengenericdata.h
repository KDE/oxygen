#ifndef oxygengeneric_data_h
#define oxygengeneric_data_h

//////////////////////////////////////////////////////////////////////////////
// oxygengenericdata.h
// generic data container for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimation.h"
#include "oxygenanimationdata.h"

#include <QObject>
#include <QTextStream>
namespace Oxygen
{

//* generic data
class GenericData : public AnimationData
{
    Q_OBJECT

    //* declare opacity property
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    //* constructor
    GenericData(QObject *parent, QWidget *widget, int duration);

    //* return animation object
    virtual const Animation::Pointer &animation() const
    {
        return _animation;
    }

    //* opacity
    virtual qreal opacity(void) const
    {
        return _opacity;
    }

    //* duration
    void setDuration(int duration) override
    {
        _animation.data()->setDuration(duration);
    }

    //* opacity
    virtual void setOpacity(qreal value)
    {
        value = digitize(value);
        if (_opacity == value)
            return;

        _opacity = value;
        setDirty();
    }

private:
    //* animation handling
    Animation::Pointer _animation;

    //* opacity variable
    qreal _opacity = 0;
};
}

#endif
