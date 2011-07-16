//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigitem.cpp
// animation configuration item
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimationconfigitem.h"
#include "oxygenanimationconfigitem.moc"
#include "ui_oxygenanimationconfigitem.h"

#include <QtCore/QTextStream>
#include <KIcon>
#include <KLocale>
#include <KMessageBox>

namespace Oxygen
{

    //_______________________________________________
    AnimationConfigItem::AnimationConfigItem( QWidget* parent, const QString& title, const QString& description ):
        QWidget( parent ),
        ui( new Ui_AnimationConfigItem() )
    {

        ui->setupUi( this );
        layout()->setMargin(0);

        ui->configurationButton->setIcon( KIcon("configure") );
        ui->descriptionButton->setIcon(KIcon("dialog-information"));

        connect( ui->enableCheckBox, SIGNAL( toggled( bool ) ), SIGNAL( changed( void ) ) );
        connect( ui->descriptionButton, SIGNAL( clicked( void ) ), SLOT( about( void ) ) );

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
    KPushButton* AnimationConfigItem::configurationButton( void ) const
    { return ui->configurationButton; }

    //_______________________________________________
    void AnimationConfigItem::setConfigurationWidget( QWidget* widget )
    {
        widget->setEnabled( ui->enableCheckBox->isChecked() );
        connect( ui->enableCheckBox, SIGNAL( toggled( bool ) ), widget, SLOT( setEnabled( bool ) ) );
        connect( ui->configurationButton, SIGNAL( toggled( bool ) ), widget, SLOT( setVisible( bool ) ) );
    }

    //_______________________________________________
    void AnimationConfigItem::about( void )
    {
        if( description().isEmpty() ) return;
        KMessageBox::information( this, description(), i18n( "oxygen-settings - information" ) );
        return;
    }

}
