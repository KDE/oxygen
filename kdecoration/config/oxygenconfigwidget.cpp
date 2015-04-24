//////////////////////////////////////////////////////////////////////////////
// oxygenconfigurationui.cpp
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

#include "oxygenconfigwidget.h"
#include "oxygenexceptionlist.h"

#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLayout>

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusMessage>

namespace Oxygen
{

    //_________________________________________________________
    ConfigWidget::ConfigWidget( QWidget* parent, const QVariantList &args ):
        KCModule(parent, args),
        m_configuration( KSharedConfig::openConfig( QStringLiteral( "oxygenrc" ) ) ),
        m_changed( false )
    {

        m_ui.setupUi( this );

        // shadow configuration
        m_ui.activeShadowConfiguration->setGroup( QPalette::Active );
        m_ui.inactiveShadowConfiguration->setGroup( QPalette::Inactive );

        // track ui changes
        connect( m_ui.titleAlignment, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( m_ui.buttonSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( m_ui.drawBorderOnMaximizedWindows, SIGNAL(clicked()), SLOT(updateChanged()) );
        connect( m_ui.drawSizeGrip, SIGNAL(clicked()), SLOT(updateChanged()) );
        connect( m_ui.animationConfigWidget, SIGNAL(changed(bool)), SLOT(updateChanged()) );

        // track exception changes
        connect( m_ui.exceptions, SIGNAL(changed(bool)), SLOT(updateChanged()) );

        // track shadow configuration changes
        connect( m_ui.activeShadowConfiguration, SIGNAL(changed(bool)), SLOT(updateChanged()) );
        connect( m_ui.inactiveShadowConfiguration, SIGNAL(changed(bool)), SLOT(updateChanged()) );

    }

    //_________________________________________________________
    void ConfigWidget::load( void )
    {

        // create internal settings and load from rc files
        m_internalSettings = InternalSettingsPtr( new InternalSettings() );
        m_internalSettings->load();

        // assign to ui
        m_ui.titleAlignment->setCurrentIndex( m_internalSettings->titleAlignment() );
        m_ui.buttonSize->setCurrentIndex( m_internalSettings->buttonSize() );
        m_ui.drawBorderOnMaximizedWindows->setChecked( m_internalSettings->drawBorderOnMaximizedWindows() );
        m_ui.drawSizeGrip->setChecked( m_internalSettings->drawSizeGrip() );

        // load animations
        m_ui.animationConfigWidget->setInternalSettings( m_internalSettings );
        m_ui.animationConfigWidget->load();

        // load shadows
        m_ui.activeShadowConfiguration->readConfig( m_configuration.data() );
        m_ui.inactiveShadowConfiguration->readConfig( m_configuration.data() );

        // load exceptions
        ExceptionList exceptions;
        exceptions.readConfig( m_configuration );
        m_ui.exceptions->setExceptions( exceptions.get() );

        setChanged( false );

    }

    //_________________________________________________________
    void ConfigWidget::save( void )
    {

        // create internal settings and load from rc files
        m_internalSettings = InternalSettingsPtr( new InternalSettings() );
        m_internalSettings->load();

        // apply modifications from ui
        m_internalSettings->setTitleAlignment( m_ui.titleAlignment->currentIndex() );
        m_internalSettings->setButtonSize( m_ui.buttonSize->currentIndex() );
        m_internalSettings->setDrawBorderOnMaximizedWindows( m_ui.drawBorderOnMaximizedWindows->isChecked() );
        m_internalSettings->setDrawSizeGrip( m_ui.drawSizeGrip->isChecked() );

        // save animations
        m_ui.animationConfigWidget->setInternalSettings( m_internalSettings );
        m_ui.animationConfigWidget->save();

        // save configuration
        m_internalSettings->save();

        // save standard configuration
        ExceptionList::writeConfig( m_internalSettings.data(), m_configuration.data() );

        // save shadows
        m_ui.activeShadowConfiguration->writeConfig( m_configuration.data() );
        m_ui.inactiveShadowConfiguration->writeConfig( m_configuration.data() );

        // get list of exceptions and write
        InternalSettingsList exceptions( m_ui.exceptions->exceptions() );
        ExceptionList( exceptions ).writeConfig( m_configuration );

        // sync configuration
        m_configuration->sync();
        setChanged( false );

        // needed to tell kwin to reload when running from external kcmshell
        {
            QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
            QDBusConnection::sessionBus().send(message);
        }

        // needed for oxygen style to reload shadows
        {
            QDBusMessage message( QDBusMessage::createSignal("/OxygenDecoration",  "org.kde.Oxygen.Style", "reparseConfiguration") );
            QDBusConnection::sessionBus().send(message);
        }

    }

    //_________________________________________________________
    void ConfigWidget::defaults( void )
    {

        // create internal settings and load from rc files
        m_internalSettings = InternalSettingsPtr( new InternalSettings() );
        m_ui.animationConfigWidget->setInternalSettings( m_internalSettings );
        m_internalSettings->setDefaults();

        // assign to ui
        m_ui.titleAlignment->setCurrentIndex( m_internalSettings->titleAlignment() );
        m_ui.buttonSize->setCurrentIndex( m_internalSettings->buttonSize() );
        m_ui.drawBorderOnMaximizedWindows->setChecked( m_internalSettings->drawBorderOnMaximizedWindows() );
        m_ui.drawSizeGrip->setChecked( m_internalSettings->drawSizeGrip() );

        // load shadows
        m_ui.activeShadowConfiguration->readDefaults( m_configuration.data() );
        m_ui.inactiveShadowConfiguration->readDefaults( m_configuration.data() );

        // load animations
        m_ui.animationConfigWidget->setInternalSettings( m_internalSettings );
        m_ui.animationConfigWidget->load();

        setChanged( false );

    }

    //_______________________________________________
    void ConfigWidget::updateChanged( void )
    {

        // check configuration
        if( !m_internalSettings ) return;

        // track modifications
        bool modified( false );

        if( m_ui.titleAlignment->currentIndex() != m_internalSettings->titleAlignment() ) modified = true;
        else if( m_ui.buttonSize->currentIndex() != m_internalSettings->buttonSize() ) modified = true;
        else if( m_ui.drawBorderOnMaximizedWindows->isChecked() !=  m_internalSettings->drawBorderOnMaximizedWindows() ) modified = true;
        else if( m_ui.drawSizeGrip->isChecked() !=  m_internalSettings->drawSizeGrip() ) modified = true;

        // animations
        else if( m_ui.animationConfigWidget->isChanged() ) modified = true;

        // exceptions
        else if( m_ui.exceptions->isChanged() ) modified = true;

        // shadow configurations
        else if( m_ui.activeShadowConfiguration->isChanged() ) modified = true;
        else if( m_ui.inactiveShadowConfiguration->isChanged() ) modified = true;

        setChanged( modified );

    }
}
