//////////////////////////////////////////////////////////////////////////////
// oxygenconfigdialog.cpp
// oxygen configuration dialog
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "oxygenconfigdialog.h"
#include "../oxygen.h"
#include "config-liboxygen.h"

#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QLibrary>
#include <QPushButton>
#include <QShortcut>
#include <QTextStream>

#if OXYGEN_USE_KDE4
#include <KIcon>
#include <KLibrary>
#endif

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginLoader>
#include <KStandardShortcut>

#include <QDBusConnection>
#include <QDBusMessage>

namespace Oxygen
{
    //_______________________________________________________________
    ConfigDialog::ConfigDialog( QWidget* parent ):
        QDialog( parent ),
        _stylePluginObject(0),
        _decorationPluginObject( 0 ),
        _styleChanged( false ),
        _decorationChanged( false )
   {

        setWindowTitle( i18n( "Oxygen Settings" ) );
        updateWindowTitle();

        // ui
        setupUi(this);

        #if OXYGEN_USE_KDE4
        // install Quit shortcut
        connect( new QShortcut( KStandardShortcut::quit().primary(), this ), SIGNAL(activated()), SLOT(close()) );
        connect( new QShortcut( KStandardShortcut::quit().alternate(), this ), SIGNAL(activated()), SLOT(close()) );
        #else
        foreach( const QKeySequence& sequence, KStandardShortcut::quit() )
        { connect( new QShortcut( sequence, this ), SIGNAL(activated()), SLOT(close()) ); }
        #endif

        connect( buttonBox->button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), SLOT(close()) );

        // connections
        connect( pageWidget, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), SLOT(updateWindowTitle(KPageWidgetItem*)) );

        // generic page
        KPageWidgetItem *page;

        // style
        page = loadStyleConfig();
        page->setName( i18n("Widget Style") );
        page->setHeader( i18n("Modify the appearance of widgets") );
        #if OXYGEN_USE_KDE4
        page->setIcon( KIcon( "preferences-desktop-theme" ) );
        #else
        page->setIcon( QIcon::fromTheme( QStringLiteral( "preferences-desktop-theme" ) ) );
        #endif
        pageWidget->addPage( page );

        if( _stylePluginObject )
        {
            connect( _stylePluginObject, SIGNAL(changed(bool)), this, SLOT(updateStyleChanged(bool)) );
            connect( _stylePluginObject, SIGNAL(changed(bool)), this, SLOT(updateChanged()) );

            connect( buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()), _stylePluginObject, SLOT(reset()) );
            connect( buttonBox->button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()), _stylePluginObject, SLOT(defaults()) );
            connect( this, SIGNAL(pluginSave()), _stylePluginObject, SLOT(save()) );
            connect( this, SIGNAL(pluginToggleExpertMode(bool)), _stylePluginObject, SLOT(toggleExpertMode(bool)) );

        }

        // decoration
        page = loadDecorationConfig();
        page->setName( i18n("Window Decorations") );
        page->setHeader( i18n("Modify the appearance of window decorations") );
        #if OXYGEN_USE_KDE4
        page->setIcon( KIcon( "preferences-system-windows" ) );
        #else
        page->setIcon( QIcon::fromTheme( QStringLiteral( "preferences-system-windows" ) ) );
        #endif
        pageWidget->addPage( page );

        if( _decorationPluginObject )
        {
            connect( _decorationPluginObject, SIGNAL(changed(bool)), this, SLOT(updateDecorationChanged(bool)) );
            connect( _decorationPluginObject, SIGNAL(changed(bool)), this, SLOT(updateChanged()) );

            connect( buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()), _decorationPluginObject, SLOT(load()) );
            connect( buttonBox->button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()), _decorationPluginObject, SLOT(defaults()) );

            connect( this, SIGNAL(pluginSave()), _decorationPluginObject, SLOT(save()) );
            connect( this, SIGNAL(pluginToggleExpertMode(bool)), _decorationPluginObject, SLOT(toggleExpertMode(bool)) );

        }

        // expert mode
        emit pluginToggleExpertMode( true );

        // button connections
        connect( buttonBox->button( QDialogButtonBox::Apply ), SIGNAL(clicked()), SLOT(save()) );
        connect( buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(clicked()), SLOT(save()) );
        updateChanged();

    }

    //_______________________________________________________________
    void ConfigDialog::save( void )
    {

        // trigger pluggins to save themselves
        emit pluginSave();

        /*
         * emit dbus message to kwin instances
         * this is copied from kwin/kcmkwin/kwindecoration/kwindecoration.cpp
         */
        QDBusMessage message = QDBusMessage::createSignal("/KWin", "org.kde.KWin", "reloadConfig");
        QDBusConnection::sessionBus().send(message);

        // reset 'changed' flags
        updateStyleChanged( false );
        updateDecorationChanged( false );
        updateChanged();

    }

    //_______________________________________________________________
    void ConfigDialog::updateChanged( void )
    {
        bool modified( changed() );
        buttonBox->button( QDialogButtonBox::Apply )->setEnabled( modified );
        buttonBox->button( QDialogButtonBox::Reset )->setEnabled( modified );
        buttonBox->button( QDialogButtonBox::Ok )->setEnabled( modified );
        updateWindowTitle( pageWidget->currentPage() );
    }

    //_______________________________________________________________
    void ConfigDialog::updateWindowTitle( KPageWidgetItem* item )
    {
        QString title;
        QTextStream what( &title );
        if( item )
        {
            what << item->name();
            if( changed() ) what << " [modified]";
            what << " - ";
        }

        what << i18n( "Oxygen Settings" );
        setWindowTitle( title );
    }

    //_______________________________________________________________
    KPageWidgetItem* ConfigDialog::loadStyleConfig( void )
    {

        // load style from plugin
        #if OXYGEN_USE_KDE4
        KLibrary library( "kstyle_oxygen_config" );
        #else
        QLibrary library( KPluginLoader::findPlugin( QStringLiteral( "kstyle_oxygen_config" ) ) );
        #endif

        if( library.load() )
        {

            #if OXYGEN_USE_KDE4
            KLibrary::void_function_ptr alloc_ptr = library.resolveFunction("allocate_kstyle_config");
            #else
            QFunctionPointer alloc_ptr = library.resolve( "allocate_kstyle_config" );
            #endif
            if( alloc_ptr != nullptr )
            {

                // pointer to decoration plugin allocation function
                QWidget* (*allocator)( QWidget* );
                allocator = (QWidget* (*)(QWidget* parent))alloc_ptr;

                // create container
                QWidget* container = new QWidget();
                container->setLayout( new QVBoxLayout() );
                container->setObjectName( QStringLiteral( "oxygen-settings-container" ) );
                container->layout()->setMargin( 0 );

                // allocate config object
                _stylePluginObject = (QObject*)(allocator( container ));
                container->layout()->addWidget( static_cast<QWidget*>( _stylePluginObject ) );
                return new KPageWidgetItem( container );

            }

        }

        // fall back to warning label
        QLabel* label = new QLabel();
        label->setMargin(5);
        label->setAlignment( Qt::AlignCenter );
        label->setText( i18n( "Unable to find oxygen style configuration plugin" ) );
        return new KPageWidgetItem( label );

    }

    //_______________________________________________________________
    KPageWidgetItem* ConfigDialog::loadDecorationConfig( void )
    {

        // load style from plugin
        #if OXYGEN_USE_KDE4
        KLibrary library( "kwin_oxygen_config" );
        #else
        QLibrary library( KPluginLoader::findPlugin( QStringLiteral( "kwin/kdecorations/config/kwin_oxygen_config" ) ) );
        #endif

        if( library.load() )
        {

            #if OXYGEN_USE_KDE4
            KLibrary::void_function_ptr alloc_ptr = library.resolveFunction("allocate_config");
            #else
            QFunctionPointer alloc_ptr = library.resolve( "allocate_config" );
            #endif

            if( alloc_ptr != nullptr )
            {

                // cast resolved function
                QObject* (*allocator)( KConfig*, QWidget* );
                allocator = ( QObject* (*)(KConfig*, QWidget*) )alloc_ptr;

                // create container
                QWidget* container = new QWidget();
                container->setLayout( new QVBoxLayout() );
                container->layout()->setMargin( 0 );

                _decorationPluginObject = (QObject*)(allocator( nullptr, container ));

                if( _decorationPluginObject ) return new KPageWidgetItem( container );

            } else { QTextStream( stdout ) << "unable to resolve function allocate_config" << endl; }

        } else { QTextStream( stdout ) << "unable to load plugin kwin_oxygen_config" << endl; }

        // fall back to warning label
        QLabel* label = new QLabel();
        label->setMargin(5);
        label->setAlignment( Qt::AlignCenter );
        label->setText( i18n( "Unable to find oxygen decoration configuration plugin" ) );
        return new KPageWidgetItem( label );

    }

}
