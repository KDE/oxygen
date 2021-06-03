//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigitem.cpp
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimationconfigitem.h"
#include "ui_oxygenanimationconfigitem.h"

#include "liboxygen.h"

#include <QTextStream>
#include <QIcon>
#include <KLocalizedString>
#include <KMessageBox>

namespace Oxygen
{

    //_______________________________________________
    AnimationConfigItem::AnimationConfigItem( QWidget* parent, const QString& title, const QString& description ):
        QWidget( parent ),
        ui( new Ui_AnimationConfigItem() )
    {

        ui->setupUi( this );
        layout()->setContentsMargins(0, 0, 0, 0);

        ui->configurationButton->setIcon( QIcon::fromTheme( QStringLiteral("configure") ) );
        ui->descriptionButton->setIcon( QIcon::fromTheme( QStringLiteral( "dialog-information") ) );

        connect( ui->enableCheckBox, SIGNAL(toggled(bool)), SIGNAL(changed()) );
        connect( ui->descriptionButton, SIGNAL(clicked()), SLOT(about()) );

        setTitle( title );
        setDescription( description );

    }

    //________________________________________________________________
    AnimationConfigItem::~AnimationConfigItem( void )
    { delete ui; }

    //________________________________________________________________
    void AnimationConfigItem::setTitle( const QString& value )
    { ui->enableCheckBox->setText( value ); }

    //________________________________________________________________
    QString AnimationConfigItem::title( void ) const
    { return ui->enableCheckBox->text(); }

    //________________________________________________________________
    void AnimationConfigItem::setDescription( const QString& value )
    {
        _description = value;
        ui->descriptionButton->setEnabled( !_description.isEmpty() );
    }

    //________________________________________________________________
    void AnimationConfigItem::setEnabled( const bool& value )
    { ui->enableCheckBox->setChecked( value ); }

    //________________________________________________________________
    bool AnimationConfigItem::enabled( void ) const
    { return ui->enableCheckBox->isChecked(); }

    //________________________________________________________________
    QAbstractButton* AnimationConfigItem::configurationButton( void ) const
    { return ui->configurationButton; }

    //_______________________________________________
    void AnimationConfigItem::setConfigurationWidget( QWidget* widget )
    {
        widget->setEnabled( ui->enableCheckBox->isChecked() );
        connect( ui->enableCheckBox, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)) );
        connect( ui->configurationButton, SIGNAL(toggled(bool)), widget, SLOT(setVisible(bool)) );
    }

    //_______________________________________________
    void AnimationConfigItem::about( void )
    {
        if( description().isEmpty() ) return;
        KMessageBox::information( this, description(), i18n( "oxygen-settings - information" ) );
        return;
    }

}
