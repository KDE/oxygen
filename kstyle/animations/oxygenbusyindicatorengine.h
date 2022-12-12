#ifndef oxygenbusyindicatorengine_h
#define oxygenbusyindicatorengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbusyindicatorengine.h
// handle progress bar busy indicator
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimation.h"
#include "oxygenbaseengine.h"
#include "oxygenbusyindicatordata.h"
#include "oxygendatamap.h"

namespace Oxygen
{

//* handles progress bar animations
class BusyIndicatorEngine : public BaseEngine
{
    Q_OBJECT

    //* declare opacity property
    Q_PROPERTY(qreal value READ value WRITE setValue)

public:
    //* constructor
    explicit BusyIndicatorEngine(QObject *);

    //*@name accessors
    //@{

    //* true if widget is animated
    bool isAnimated(const QObject *);

    //* value
    qreal value(void) const
    {
        return _value;
    }

    //@}

    //*@name modifiers
    //@{

    //* register progressbar
    bool registerWidget(QObject *);

    //* duration
    void setDuration(int) override;

    //* set object as animated
    void setAnimated(const QObject *, bool);

    //* opacity
    void setValue(qreal value);

    //@}

public Q_SLOTS:

    //* remove widget from map
    bool unregisterWidget(QObject *) override;

private:
    //* returns data associated to widget
    DataMap<BusyIndicatorData>::Value data(const QObject *);

    //* map widgets to progressbar data
    DataMap<BusyIndicatorData> _data;

    //* animation
    Animation::Pointer _animation;

    //* value
    qreal _value = 0;
};
}

#endif
