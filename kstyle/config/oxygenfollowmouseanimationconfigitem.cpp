//////////////////////////////////////////////////////////////////////////////
// oxygenfollowmouseanimationconfigitem.cpp
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenfollowmouseanimationconfigitem.h"
#include "ui_oxygenfollowmouseanimationconfigbox.h"

namespace Oxygen
{

//_______________________________________________
FollowMouseAnimationConfigBox::FollowMouseAnimationConfigBox(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui_FollowMouseAnimationConfigBox())
{
    ui->setupUi(this);
    ui->followMouseDurationSpinBox->setEnabled(false);
    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int)));
}

//_______________________________________________
FollowMouseAnimationConfigBox::~FollowMouseAnimationConfigBox(void)
{
    delete ui;
}

//_______________________________________________
KComboBox *FollowMouseAnimationConfigBox::typeComboBox(void) const
{
    return ui->typeComboBox;
}

//_______________________________________________
QSpinBox *FollowMouseAnimationConfigBox::durationSpinBox(void) const
{
    return ui->durationSpinBox;
}

//_______________________________________________
QLabel *FollowMouseAnimationConfigBox::durationLabel(void) const
{
    return ui->durationLabel;
}

//_______________________________________________
QSpinBox *FollowMouseAnimationConfigBox::followMouseDurationSpinBox(void) const
{
    return ui->followMouseDurationSpinBox;
}

//_______________________________________________
void FollowMouseAnimationConfigBox::typeChanged(int value)
{
    ui->followMouseDurationLabel->setEnabled(value == 1);
    ui->followMouseDurationSpinBox->setEnabled(value == 1);
}

//_______________________________________________
void FollowMouseAnimationConfigItem::initializeConfigurationWidget(QWidget *parent)
{
    Q_ASSERT(!_configurationWidget);
    _configurationWidget = new FollowMouseAnimationConfigBox(parent);
    setConfigurationWidget(_configurationWidget.data());

    connect(_configurationWidget.data()->typeComboBox(), SIGNAL(currentIndexChanged(int)), SIGNAL(changed()));
    connect(_configurationWidget.data()->durationSpinBox(), SIGNAL(valueChanged(int)), SIGNAL(changed()));
    connect(_configurationWidget.data()->followMouseDurationSpinBox(), SIGNAL(valueChanged(int)), SIGNAL(changed()));
}
}
