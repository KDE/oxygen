#ifndef oxygenmenu_data_h
#define oxygenmenu_data_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenudata.h
// data container for QMenu animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmenubardata.h"
#include <QMenu>

namespace Oxygen
{

//* menubar data
/*!
most members are identical to menubar data. The one that are not are
using templatized versions, because QMenuBar and QMenu API are very similar
*/
class MenuDataV1 : public MenuBarDataV1
{
    Q_OBJECT

public:
    //* constructor
    MenuDataV1(QObject *parent, QWidget *target, int duration)
        : MenuBarDataV1(parent, target, duration)
    {
    }

protected:
    //* menubar enterEvent
    void enterEvent(const QObject *object) override
    {
        MenuBarDataV1::enterEvent<QMenu>(object);
    }

    //* menubar enterEvent
    void leaveEvent(const QObject *object) override
    {
        MenuBarDataV1::leaveEvent<QMenu>(object);
    }

    //* menubar mouseMoveEvent
    void mouseMoveEvent(const QObject *object) override
    {
        MenuBarDataV1::mouseMoveEvent<QMenu>(object);
    }

    //* menubar mousePressEvent
    void mousePressEvent(const QObject *object) override
    {
        MenuBarDataV1::mousePressEvent<QMenu>(object);
    }
};

//* menubar data
/*!
most members are identical to menubar data. The one that are not are
using templatized versions, because QMenuBar and QMenu API are very similar
*/
class MenuDataV2 : public MenuBarDataV2
{
    Q_OBJECT

public:
    //* constructor
    MenuDataV2(QObject *parent, QWidget *target, int duration)
        : MenuBarDataV2(parent, target, duration)
    {
        setEntered(false);
    }

protected:
    //* menubar enterEvent
    void enterEvent(const QObject *object) override
    {
        MenuBarDataV2::enterEvent<QMenu>(object);
    }

    //* menubar enterEvent
    void leaveEvent(const QObject *object) override
    {
        MenuBarDataV2::leaveEvent<QMenu>(object);
    }

    //* menubar mouseMoveEvent
    void mouseMoveEvent(const QObject *object) override
    {
        MenuBarDataV2::mouseMoveEvent<QMenu>(object);
    }
};
}

#endif
