#ifndef oxygenanimationconfigitem_h
#define oxygenanimationconfigitem_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigitem.h
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygen_config_export.h"

#include <QAbstractButton>
#include <QWidget>

class Ui_AnimationConfigItem;

namespace Oxygen
{

class OXYGEN_CONFIG_EXPORT AnimationConfigItem : public QWidget
{
    Q_OBJECT

public:
    //* constructor
    explicit AnimationConfigItem(QWidget *parent, const QString &title = QString(), const QString &description = QString());

    //* destructor
    ~AnimationConfigItem(void) override;

    //* title
    void setTitle(const QString &);

    //* title
    QString title(void) const;

    //* description
    void setDescription(const QString &);

    //* description
    const QString &description(void) const
    {
        return _description;
    }

    //* enability
    void setEnabled(const bool &);

    //* enability
    bool enabled(void) const;

    //* config widget
    virtual QWidget *configurationWidget(void) const = 0;

    //* initialize config widget
    virtual void initializeConfigurationWidget(QWidget *) = 0;

    //* configuration button
    QAbstractButton *configurationButton(void) const;

Q_SIGNALS:

    //* emmited when changed
    void changed(void);

protected Q_SLOTS:

    //* about info
    virtual void about(void);

protected:
    //* set configuration widget
    virtual void setConfigurationWidget(QWidget *widget);

private:
    //* description
    QString _description;

    //* ui
    Ui_AnimationConfigItem *ui;
};
}

#endif
