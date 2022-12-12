#ifndef oxygenmenubarengine_h
#define oxygenmenubarengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenubarengine.h
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenmenubardata.h"

namespace Oxygen
{

//* stores menubar hovered action and timeLine
class MenuBarBaseEngine : public BaseEngine
{
    Q_OBJECT

public:
    //* constructor
    explicit MenuBarBaseEngine(QObject *parent)
        : BaseEngine(parent)
    {
    }

    //* register menubar
    virtual bool registerWidget(QWidget *) = 0;

    //* true if widget is animated
    virtual bool isAnimated(const QObject *, const QPoint &)
    {
        return false;
    }

    //* animation opacity
    virtual qreal opacity(const QObject *, const QPoint &)
    {
        return -1;
    }

    //* return 'hover' rect position when widget is animated
    virtual QRect currentRect(const QObject *, const QPoint &)
    {
        return QRect();
    }

    //* animated rect
    virtual QRect animatedRect(const QObject *)
    {
        return QRect();
    }

    //* timer
    virtual bool isTimerActive(const QObject *)
    {
        return false;
    }

    //* enable state
    void setEnabled(bool) override = 0;

    //* duration
    void setDuration(int) override = 0;

    //* duration
    virtual void setFollowMouseDuration(int)
    {
    }
};

//* fading menubar animation
class MenuBarEngineV1 : public MenuBarBaseEngine
{
    Q_OBJECT

public:
    //* constructor
    explicit MenuBarEngineV1(QObject *parent)
        : MenuBarBaseEngine(parent)
    {
    }

    //* constructor
    MenuBarEngineV1(QObject *parent, MenuBarBaseEngine *other);

    //* register menubar
    bool registerWidget(QWidget *) override;

    //* true if widget is animated
    bool isAnimated(const QObject *object, const QPoint &point) override;

    //* animation opacity
    qreal opacity(const QObject *object, const QPoint &point) override
    {
        return isAnimated(object, point) ? _data.find(object).data()->opacity(point) : AnimationData::OpacityInvalid;
    }

    //* return 'hover' rect position when widget is animated
    QRect currentRect(const QObject *object, const QPoint &point) override
    {
        return isAnimated(object, point) ? _data.find(object).data()->currentRect(point) : QRect();
    }

    //* enable state
    void setEnabled(bool value) override
    {
        BaseEngine::setEnabled(value);
        _data.setEnabled(value);
    }

    //* duration
    void setDuration(int duration) override
    {
        BaseEngine::setDuration(duration);
        _data.setDuration(duration);
    }

    //* return list of registered widgets
    WidgetList registeredWidgets(void) const override;

protected Q_SLOTS:

    //* remove widget from map
    bool unregisterWidget(QObject *object) override
    {
        return _data.unregisterWidget(object);
    }

private:
    //* data map
    DataMap<MenuBarDataV1> _data;
};

//* follow-mouse menubar animation
class MenuBarEngineV2 : public MenuBarBaseEngine
{
    Q_OBJECT

public:
    //* constructor
    explicit MenuBarEngineV2(QObject *parent)
        : MenuBarBaseEngine(parent)
    {
    }

    //* constructor
    MenuBarEngineV2(QObject *parent, MenuBarBaseEngine *other);

    //* register menubar
    bool registerWidget(QWidget *) override;

    //* true if widget is animated
    bool isAnimated(const QObject *object, const QPoint &point) override;

    //* animation opacity
    qreal opacity(const QObject *object, const QPoint &point) override
    {
        return isAnimated(object, point) ? _data.find(object).data()->opacity() : AnimationData::OpacityInvalid;
    }

    //* return 'hover' rect position when widget is animated
    QRect currentRect(const QObject *, const QPoint &) override;

    //* return 'hover' rect position when widget is animated
    QRect animatedRect(const QObject *) override;

    //* timer associated to the data
    bool isTimerActive(const QObject *) override;

    //* enable state
    void setEnabled(bool value) override
    {
        BaseEngine::setEnabled(value);
        _data.setEnabled(value);
    }

    //* duration
    void setDuration(int value) override
    {
        BaseEngine::setDuration(value);
        _data.setDuration(value);
    }

    //* duration
    int followMouseDuration(void) const
    {
        return _followMouseDuration;
    }

    //* duration
    void setFollowMouseDuration(int duration) override
    {
        _followMouseDuration = duration;
        for (const DataMap<MenuBarDataV2>::Value &value : std::as_const(_data)) {
            if (value)
                value.data()->setFollowMouseDuration(duration);
        }
    }

    //* return list of registered widgets
    WidgetList registeredWidgets(void) const override;

protected Q_SLOTS:

    //* remove widget from map
    bool unregisterWidget(QObject *object) override
    {
        return _data.unregisterWidget(object);
    }

private:
    //* follow mouse animation duration
    int _followMouseDuration;

    //* data map
    DataMap<MenuBarDataV2> _data;
};
}

#endif
