#ifndef oxygendockseparatorengine_h
#define oxygendockseparatorengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygendockseparatorengine.h
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygendockseparatordata.h"

namespace Oxygen
{

//* stores dockseparator hovered action and timeLine
class DockSeparatorEngine : public BaseEngine
{
    Q_OBJECT

public:
    //* constructor
    explicit DockSeparatorEngine(QObject *parent)
        : BaseEngine(parent)
    {
    }

    //* register dockseparator
    bool registerWidget(QWidget *);

    //* update rect and hover value for data matching widget
    void updateRect(const QObject *object, const QRect &r, const Qt::Orientation orientation, bool hovered)
    {
        if (DataMap<DockSeparatorData>::Value data = _data.find(object)) {
            data.data()->updateRect(r, orientation, hovered);
        }
    }

    //* returns true if object is animated
    bool isAnimated(const QObject *object, const QRect &r, const Qt::Orientation orientation)
    {
        if (DataMap<DockSeparatorData>::Value data = _data.find(object)) {
            return data.data()->isAnimated(r, orientation);
        } else
            return false;
    }

    //* returns true if object is animated
    qreal opacity(const QObject *object, const Qt::Orientation orientation)
    {
        if (DataMap<DockSeparatorData>::Value data = _data.find(object)) {
            return data.data()->opacity(orientation);
        } else
            return AnimationData::OpacityInvalid;
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
    DataMap<DockSeparatorData> _data;
};
}

#endif
