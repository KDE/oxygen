//////////////////////////////////////////////////////////////////////////////
// oxygenmenuengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmenuengine.h"

namespace Oxygen
{

//____________________________________________________________
MenuEngineV1::MenuEngineV1(QObject *parent, MenuBaseEngine *other)
    : MenuBaseEngine(parent)
{
    if (other) {
        const auto otherWidgets = other->registeredWidgets();
        for (QWidget *widget : otherWidgets) {
            registerWidget(widget);
        }
    }
}

//____________________________________________________________
bool MenuEngineV1::registerWidget(QWidget *widget)
{
    if (!widget)
        return false;

    // create new data class
    if (!_data.contains(widget))
        _data.insert(widget, new MenuDataV1(this, widget, duration()), enabled());

    // connect destruction signal
    connect(widget, SIGNAL(destroyed(QObject *)), this, SLOT(unregisterWidget(QObject *)), Qt::UniqueConnection);
    return true;
}

//____________________________________________________________
bool MenuEngineV1::isAnimated(const QObject *object, WidgetIndex index)
{
    DataMap<MenuDataV1>::Value data(_data.find(object));
    if (!data) {
        return false;
    }

    if (Animation::Pointer animation = data.data()->animation(index)) {
        return animation.data()->isRunning();

    } else
        return false;
}

//____________________________________________________________
BaseEngine::WidgetList MenuEngineV1::registeredWidgets(void) const
{
    WidgetList out;

    // the typedef is needed to make Krazy happy
    typedef DataMap<MenuDataV1>::Value Value;
    for (const Value &value : std::as_const(_data)) {
        if (value)
            out.insert(value.data()->target().data());
    }

    return out;
}

//____________________________________________________________
MenuEngineV2::MenuEngineV2(QObject *parent, MenuBaseEngine *other)
    : MenuBaseEngine(parent)
    , _followMouseDuration(150)
{
    if (other) {
        const auto otherWidgets = other->registeredWidgets();
        for (QWidget *widget : otherWidgets) {
            registerWidget(widget);
        }
    }
}

//____________________________________________________________
bool MenuEngineV2::registerWidget(QWidget *widget)
{
    if (!widget)
        return false;

    // create new data class
    if (!_data.contains(widget)) {
        DataMap<MenuDataV2>::Value value(new MenuDataV2(this, widget, duration()));
        value.data()->setFollowMouseDuration(followMouseDuration());
        _data.insert(widget, value, enabled());
    }

    // connect destruction signal
    connect(widget, SIGNAL(destroyed(QObject *)), this, SLOT(unregisterWidget(QObject *)), Qt::UniqueConnection);

    return true;
}

//____________________________________________________________
QRect MenuEngineV2::currentRect(const QObject *object, WidgetIndex)
{
    if (!enabled())
        return QRect();
    DataMap<MenuDataV2>::Value data(_data.find(object));
    return data ? data.data()->currentRect() : QRect();
}

//____________________________________________________________
bool MenuEngineV2::isAnimated(const QObject *object, WidgetIndex index)
{
    DataMap<MenuDataV2>::Value data(_data.find(object));
    if (!data) {
        return false;
    }

    switch (index) {
    case Oxygen::Previous: {
        if (Animation::Pointer animation = data.data()->animation()) {
            return animation.data()->direction() == Animation::Backward && animation.data()->isRunning();
        } else
            return false;
    }

    case Oxygen::Current: {
        if (data.data()->animation() && data.data()->animation().data()->isRunning())
            return true;
        else
            return false;
    }

    default:
        return false;
    }
}

//____________________________________________________________
QRect MenuEngineV2::animatedRect(const QObject *object)
{
    if (!enabled())
        return QRect();
    DataMap<MenuDataV2>::Value data(_data.find(object));
    return data ? data.data()->animatedRect() : QRect();
}

//____________________________________________________________
bool MenuEngineV2::isTimerActive(const QObject *object)
{
    if (!enabled())
        return false;
    DataMap<MenuDataV2>::Value data(_data.find(object));
    return data ? data.data()->timer().isActive() : false;
}

//____________________________________________________________
BaseEngine::WidgetList MenuEngineV2::registeredWidgets(void) const
{
    WidgetList out;

    // the typedef is needed to make Krazy happy
    typedef DataMap<MenuDataV2>::Value Value;
    for (const Value &value : std::as_const(_data)) {
        if (value)
            out.insert(value.data()->target().data());
    }

    return out;
}
}
