#ifndef oxygenbaseanimationconfigwidget_h
#define oxygenbaseanimationconfigwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbaseanimationconfigwidget.h
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygen_config_export.h"

#include <QCheckBox>
#include <QLayout>
#include <QWidget>

class Ui_AnimationConfigWidget;

namespace Oxygen
{
class AnimationConfigItem;

class OXYGEN_CONFIG_EXPORT BaseAnimationConfigWidget : public QWidget
{
    Q_OBJECT

public:
    //* constructor
    explicit BaseAnimationConfigWidget(QWidget * = nullptr);

    //* destructor
    virtual ~BaseAnimationConfigWidget(void);

    //* true if changed
    virtual bool isChanged(void) const
    {
        return _changed;
    }

Q_SIGNALS:

    //* emmited when layout is changed
    void layoutChanged(void);

    //* emmited when changed
    void changed(bool);

public Q_SLOTS:

    //* read current configuration
    virtual void load(void) = 0;

    //* save current configuration
    virtual void save(void) = 0;

protected Q_SLOTS:

    //* update visible ites
    virtual void updateItems(bool);

    //* check whether configuration is changed and emit appropriate signal if yes
    virtual void updateChanged() = 0;

protected:
    //* get global animations enabled checkbox
    QCheckBox *animationsEnabled(void) const;

    //* add item to ui
    virtual void setupItem(QGridLayout *, AnimationConfigItem *);

    //* set changed state
    virtual void setChanged(bool value)
    {
        _changed = value;
        emit changed(value);
    }

    //* user interface
    Ui_AnimationConfigWidget *ui = nullptr;

    //* row index
    int _row = 0;

private:
    //* changed state
    bool _changed = false;
};
}

#endif
