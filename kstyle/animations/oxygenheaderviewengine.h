#ifndef oxygenheaderviewengine_h
#define oxygenheaderviewengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenheaderviewengine.h
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenheaderviewdata.h"

namespace Oxygen
{

//* stores headerview hovered action and timeLine
class HeaderViewEngine : public BaseEngine
{
    Q_OBJECT

public:
    //* constructor
    explicit HeaderViewEngine(QObject *parent)
        : BaseEngine(parent)
    {
    }

    //* register headerview
    bool registerWidget(QWidget *);

    //* true if widget hover state is changed
    bool updateState(const QObject *, const QPoint &, bool);

    //* true if widget is animated
    bool isAnimated(const QObject *object, const QPoint &point)
    {
        if (DataMap<HeaderViewData>::Value data = _data.find(object)) {
            if (Animation::Pointer animation = data.data()->animation(point))
                return animation.data()->isRunning();
        }
        return false;
    }

    //* animation opacity
    qreal opacity(const QObject *object, const QPoint &point)
    {
        return isAnimated(object, point) ? _data.find(object).data()->opacity(point) : AnimationData::OpacityInvalid;
    }

    //* enability
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

public Q_SLOTS:

    //* remove widget from map
    bool unregisterWidget(QObject *object) override
    {
        return _data.unregisterWidget(object);
    }

private:
    //* data map
    DataMap<HeaderViewData> _data;
};
}

#endif
