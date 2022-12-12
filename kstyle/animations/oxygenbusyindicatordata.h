#ifndef oxygenbusyindicatordata_h
#define oxygenbusyindicatordata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbusyindicatordata.h
// data container for progressbar busy indicator
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QObject>

namespace Oxygen
{

class BusyIndicatorData : public QObject
{
    Q_OBJECT

public:
    //* constructor
    explicit BusyIndicatorData(QObject *parent)
        : QObject(parent)
    {
    }

    //*@name accessors
    //@{

    //* animated
    bool isAnimated(void) const
    {
        return _animated;
    }

    //@}

    //*@name modifiers
    //@{

    //* enabled
    void setEnabled(bool)
    {
    }

    //* enabled
    void setDuration(int)
    {
    }

    //* animated
    void setAnimated(bool value)
    {
        _animated = value;
    }

    //@}

private:
    //* animated
    bool _animated = false;
};
}

#endif
