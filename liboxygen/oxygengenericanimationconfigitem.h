#ifndef oxygengenericanimationconfigitem_h
#define oxygengenericanimationconfigitem_h

//////////////////////////////////////////////////////////////////////////////
// oxygengenericanimationconfigitem.h
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygen_config_export.h"
#include "oxygenanimationconfigitem.h"

#include <QFrame>
#include <QPointer>
#include <QSpinBox>

class Ui_GenericAnimationConfigBox;

namespace Oxygen
{

class OXYGEN_CONFIG_EXPORT GenericAnimationConfigBox : public QFrame
{
    Q_OBJECT

public:
    //* constructor
    explicit GenericAnimationConfigBox(QWidget *);

    //* destructor
    virtual ~GenericAnimationConfigBox();

    //* duration spin box
    QSpinBox *durationSpinBox(void) const;

private:
    Ui_GenericAnimationConfigBox *ui = nullptr;
};

//* generic animation config item
class OXYGEN_CONFIG_EXPORT GenericAnimationConfigItem : public AnimationConfigItem
{
    Q_OBJECT

public:
    //* constructor
    explicit GenericAnimationConfigItem(QWidget *parent, const QString &title = QString(), const QString &description = QString())
        : AnimationConfigItem(parent, title, description)
    {
    }

    //* configure
    void initializeConfigurationWidget(QWidget *) override;

    //* configuration widget
    QWidget *configurationWidget(void) const override
    {
        return _configurationWidget.data();
    }

    //* duration
    virtual int duration(void) const
    {
        return _configurationWidget ? _configurationWidget.data()->durationSpinBox()->value() : 0;
    }

public Q_SLOTS:

    //* duration
    virtual void setDuration(int value)
    {
        if (_configurationWidget) {
            _configurationWidget.data()->durationSpinBox()->setValue(value);
        }
    }

private:
    //* configuration widget
    QPointer<GenericAnimationConfigBox> _configurationWidget;
};
}

#endif
