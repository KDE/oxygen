//////////////////////////////////////////////////////////////////////////////
// ShadowConfigurationui->cpp
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenshadowconfigwidget.h"
#include "oxygenactiveshadowconfiguration.h"
#include "oxygeninactiveshadowconfiguration.h"
#include "ui_oxygenshadowconfigurationui.h"

#include "oxygenutil.h"

#include <KGlobal>
#include <KLocale>
#include <QtGui/QLabel>
#include <QtGui/QLayout>

namespace Oxygen
{

    //_________________________________________________________
    ShadowConfigWidget::ShadowConfigWidget( QWidget* parent ):
        QGroupBox( parent ),
        ui( new Ui_ShadowConfiguraionUI() ),
        _group( QPalette::Inactive ),
        _changed( false )
    {

        KGlobal::locale()->insertCatalog("liboxygenstyleconfig");

        setCheckable( true );
        ui->setupUi( this );

        // connections
        connect( ui->shadowSize, SIGNAL(valueChanged(int)), SLOT(updateChanged()) );
        connect( ui->verticalOffset, SIGNAL(valueChanged(int)), SLOT(updateChanged()) );
        connect( ui->innerColor, SIGNAL(changed(QColor)), SLOT(updateChanged()) );
        connect( ui->outerColor, SIGNAL(changed(QColor)), SLOT(updateChanged()) );
        connect( ui->useOuterColor, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( this, SIGNAL(toggled(bool)), SLOT(updateChanged()) );

    }

    //_________________________________________________________
    ShadowConfigWidget::~ShadowConfigWidget( void )
    { delete ui; }

    //_________________________________________________________
    void ShadowConfigWidget::writeConfig( KConfig* config ) const
    {

        if( _group == QPalette::Active )
        {

            ActiveShadowConfiguration::setShadowSize( ui->shadowSize->value() );
            ActiveShadowConfiguration::setVerticalOffset( 0.1*ui->verticalOffset->value() );
            ActiveShadowConfiguration::setInnerColor( ui->innerColor->color() );
            ActiveShadowConfiguration::setOuterColor( ui->outerColor->color() );
            ActiveShadowConfiguration::setUseOuterColor( ui->useOuterColor->isChecked() );

            ActiveShadowConfiguration::setEnabled( isChecked() );
            Util::writeConfig( ActiveShadowConfiguration::self(), config );

        } else if( _group == QPalette::Inactive ) {

            InactiveShadowConfiguration::setShadowSize( ui->shadowSize->value() );
            InactiveShadowConfiguration::setVerticalOffset( 0.1*ui->verticalOffset->value() );
            InactiveShadowConfiguration::setInnerColor( ui->innerColor->color() );
            InactiveShadowConfiguration::setOuterColor( ui->outerColor->color() );
            InactiveShadowConfiguration::setUseOuterColor( ui->useOuterColor->isChecked() );

            InactiveShadowConfiguration::setEnabled( isChecked() );
            Util::writeConfig( InactiveShadowConfiguration::self(), config );

        }

    }

    //_________________________________________________________
    void ShadowConfigWidget::updateChanged( void )
    {
        if( _group == QPalette::Active )
        {

            setChanged(
                ( ui->shadowSize->value() != ActiveShadowConfiguration::shadowSize() ) ||
                ( ui->verticalOffset->value() != 10*ActiveShadowConfiguration::verticalOffset() ) ||
                ( ui->innerColor->color() != ActiveShadowConfiguration::innerColor() ) ||
                ( ui->useOuterColor->isChecked() != ActiveShadowConfiguration::useOuterColor() ) ||
                ( ui->outerColor->color() != ActiveShadowConfiguration::outerColor() ) ||
                ( isChecked() != ActiveShadowConfiguration::enabled() ) );

        } else if( _group == QPalette::Inactive ) {

            setChanged(
                ( ui->shadowSize->value() != InactiveShadowConfiguration::shadowSize() ) ||
                ( ui->verticalOffset->value() != 10*InactiveShadowConfiguration::verticalOffset() ) ||
                ( ui->innerColor->color() != InactiveShadowConfiguration::innerColor() ) ||
                ( ui->useOuterColor->isChecked() != InactiveShadowConfiguration::useOuterColor() ) ||
                ( ui->outerColor->color() != InactiveShadowConfiguration::outerColor() ) ||
                ( isChecked() != InactiveShadowConfiguration::enabled() ) );

        }

    }

    //_________________________________________________________
    void ShadowConfigWidget::readConfig( KConfig* config, bool defaults )
    {
        if( _group == QPalette::Active )
        {

            if( defaults ) ActiveShadowConfiguration::self()->setDefaults();
            else Util::readConfig( ActiveShadowConfiguration::self(), config );

            ui->shadowSize->setValue( ActiveShadowConfiguration::shadowSize() );
            ui->verticalOffset->setValue( 10*ActiveShadowConfiguration::verticalOffset() );
            ui->innerColor->setColor( ActiveShadowConfiguration::innerColor() );
            ui->outerColor->setColor( ActiveShadowConfiguration::outerColor() );
            ui->useOuterColor->setChecked( ActiveShadowConfiguration::useOuterColor() );

            setChecked( ActiveShadowConfiguration::enabled() );

        } else if( _group == QPalette::Inactive ) {

            if( defaults ) InactiveShadowConfiguration::self()->setDefaults();
            else Util::readConfig( InactiveShadowConfiguration::self(), config );

            ui->shadowSize->setValue( InactiveShadowConfiguration::shadowSize() );
            ui->verticalOffset->setValue( 10*InactiveShadowConfiguration::verticalOffset() );
            ui->innerColor->setColor( InactiveShadowConfiguration::innerColor() );
            ui->outerColor->setColor( InactiveShadowConfiguration::outerColor() );
            ui->useOuterColor->setChecked( InactiveShadowConfiguration::useOuterColor() );

            setChecked( InactiveShadowConfiguration::enabled() );

        }

    }

}
