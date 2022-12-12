//////////////////////////////////////////////////////////////////////////////
// ShadowConfigurationui->cpp
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenshadowconfigwidget.h"
#include "oxygenactiveshadowconfiguration.h"
#include "oxygeninactiveshadowconfiguration.h"
#include "ui_oxygenshadowconfigurationui.h"

#include "config-liboxygen.h"

#include <KLocalizedString>
#include <QLabel>
#include <QLayout>

namespace Oxygen
{

//_________________________________________________________
ShadowConfigWidget::ShadowConfigWidget(QWidget *parent)
    : QGroupBox(parent)
    , ui(new Ui_ShadowConfiguraionUI())
{
    setCheckable(true);
    ui->setupUi(this);

    // connections
    connect(ui->shadowSize, SIGNAL(valueChanged(int)), SLOT(updateChanged()));
    connect(ui->verticalOffset, SIGNAL(valueChanged(int)), SLOT(updateChanged()));
    connect(ui->innerColor, SIGNAL(changed(QColor)), SLOT(updateChanged()));
    connect(ui->outerColor, SIGNAL(changed(QColor)), SLOT(updateChanged()));
    connect(ui->useOuterColor, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(this, SIGNAL(toggled(bool)), SLOT(updateChanged()));
}

//_________________________________________________________
ShadowConfigWidget::~ShadowConfigWidget(void)
{
    delete ui;
}

//_________________________________________________________
void ShadowConfigWidget::save(void) const
{
    if (_group == QPalette::Active) {
        ActiveShadowConfiguration::setShadowSize(ui->shadowSize->value());
        ActiveShadowConfiguration::setVerticalOffset(0.1 * ui->verticalOffset->value());
        ActiveShadowConfiguration::setInnerColor(ui->innerColor->color());
        ActiveShadowConfiguration::setOuterColor(ui->outerColor->color());
        ActiveShadowConfiguration::setUseOuterColor(ui->useOuterColor->isChecked());

        ActiveShadowConfiguration::setEnabled(isChecked());
        ActiveShadowConfiguration::self()->save();

    } else if (_group == QPalette::Inactive) {
        InactiveShadowConfiguration::setShadowSize(ui->shadowSize->value());
        InactiveShadowConfiguration::setVerticalOffset(0.1 * ui->verticalOffset->value());
        InactiveShadowConfiguration::setInnerColor(ui->innerColor->color());
        InactiveShadowConfiguration::setOuterColor(ui->outerColor->color());
        InactiveShadowConfiguration::setUseOuterColor(ui->useOuterColor->isChecked());

        InactiveShadowConfiguration::setEnabled(isChecked());
        InactiveShadowConfiguration::self()->save();
    }
}

//_________________________________________________________
void ShadowConfigWidget::updateChanged(void)
{
    if (_group == QPalette::Active) {
        setChanged((ui->shadowSize->value() != ActiveShadowConfiguration::shadowSize())
                   || (ui->verticalOffset->value() != 10 * ActiveShadowConfiguration::verticalOffset())
                   || (ui->innerColor->color() != ActiveShadowConfiguration::innerColor())
                   || (ui->useOuterColor->isChecked() != ActiveShadowConfiguration::useOuterColor())
                   || (ui->outerColor->color() != ActiveShadowConfiguration::outerColor()) || (isChecked() != ActiveShadowConfiguration::enabled()));

    } else if (_group == QPalette::Inactive) {
        setChanged((ui->shadowSize->value() != InactiveShadowConfiguration::shadowSize())
                   || (ui->verticalOffset->value() != 10 * InactiveShadowConfiguration::verticalOffset())
                   || (ui->innerColor->color() != InactiveShadowConfiguration::innerColor())
                   || (ui->useOuterColor->isChecked() != InactiveShadowConfiguration::useOuterColor())
                   || (ui->outerColor->color() != InactiveShadowConfiguration::outerColor()) || (isChecked() != InactiveShadowConfiguration::enabled()));
    }
}

//_________________________________________________________
void ShadowConfigWidget::load(bool defaults)
{
    if (_group == QPalette::Active) {
        if (defaults)
            ActiveShadowConfiguration::self()->setDefaults();
        else {
            ActiveShadowConfiguration::self()->load();
        }

        ui->shadowSize->setValue(ActiveShadowConfiguration::shadowSize());
        ui->verticalOffset->setValue(10 * ActiveShadowConfiguration::verticalOffset());
        ui->innerColor->setColor(ActiveShadowConfiguration::innerColor());
        ui->outerColor->setColor(ActiveShadowConfiguration::outerColor());
        ui->useOuterColor->setChecked(ActiveShadowConfiguration::useOuterColor());

        setChecked(ActiveShadowConfiguration::enabled());

    } else if (_group == QPalette::Inactive) {
        if (defaults)
            InactiveShadowConfiguration::self()->setDefaults();
        else {
            InactiveShadowConfiguration::self()->load();
        }

        ui->shadowSize->setValue(InactiveShadowConfiguration::shadowSize());
        ui->verticalOffset->setValue(10 * InactiveShadowConfiguration::verticalOffset());
        ui->innerColor->setColor(InactiveShadowConfiguration::innerColor());
        ui->outerColor->setColor(InactiveShadowConfiguration::outerColor());
        ui->useOuterColor->setChecked(InactiveShadowConfiguration::useOuterColor());

        setChecked(InactiveShadowConfiguration::enabled());
    }
}
}
