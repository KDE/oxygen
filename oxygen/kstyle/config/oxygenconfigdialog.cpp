//////////////////////////////////////////////////////////////////////////////
// oxygenconfigdialog.cpp
// oxygen configuration dialog
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
#include "oxygenconfigdialog.h"
#include "oxygenconfigdialog.moc"

#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QShortcut>

#include <KConfigGroup>
#include <KGlobalSettings>
#include <KLocale>
#include <KLibrary>
#include <KPushButton>
#include <KStandardShortcut>
#include <KVBox>

namespace Oxygen
{
    //_______________________________________________________________
    ConfigDialog::ConfigDialog( QWidget* parent ):
        KDialog( parent ),
        _stylePluginObject(0),
        _decorationPluginObject( 0 ),
        _styleChanged( false ),
        _decorationChanged( false )
   {
        setButtons( Default|Reset|Apply|Ok|Cancel );

        updateWindowTitle();

        setWindowTitle( i18n( "Oxygen Settings" ) );

        // install Quit shortcut
        connect( new QShortcut( KStandardShortcut::quit().primary(), this ), SIGNAL(activated()), SLOT(close()) );
        connect( new QShortcut( KStandardShortcut::quit().alternate(), this ), SIGNAL(activated()), SLOT(close()) );

        // tab widget
        pageWidget_ = new KPageWidget( this );
        setMainWidget( pageWidget_ );

        connect( pageWidget_, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), SLOT(updateWindowTitle(KPageWidgetItem*)) );

        KPageWidgetItem *page;

        // style
        page = loadStyleConfig();
        page->setName( i18n("Widget Style") );
        page->setHeader( i18n("Modify the appearance of widgets") );
        page->setIcon( KIcon( "preferences-desktop-theme" ) );
        pageWidget_->addPage( page );

        if( _stylePluginObject )
        {
            connect( _stylePluginObject, SIGNAL(changed(bool)), this, SLOT(updateStyleChanged(bool)) );
            connect( _stylePluginObject, SIGNAL(changed(bool)), this, SLOT(updateChanged()) );

            connect( this, SIGNAL(pluginSave(KConfigGroup&)), _stylePluginObject, SLOT(save()) );
            connect( this, SIGNAL(pluginDefault()), _stylePluginObject, SLOT(defaults()) );
            connect( this, SIGNAL(pluginReset(KConfigGroup)), _stylePluginObject, SLOT(reset()) );
            connect( this, SIGNAL(pluginToggleExpertMode(bool)), _stylePluginObject, SLOT(toggleExpertMode(bool)) );

        }

        // decoration
        page = loadDecorationConfig();
        page->setName( i18n("Window Decorations") );
        page->setHeader( i18n("Modify the appearance of window decorations") );
        page->setIcon( KIcon( "preferences-system-windows" ) );
        pageWidget_->addPage( page );

        if( _decorationPluginObject )
        {
            connect( _decorationPluginObject, SIGNAL(changed(bool)), this, SLOT(updateDecorationChanged(bool)) );
            connect( _decorationPluginObject, SIGNAL(changed(bool)), this, SLOT(updateChanged()) );

            connect( this, SIGNAL(pluginSave(KConfigGroup&)), _decorationPluginObject, SLOT(save(KConfigGroup&)) );
            connect( this, SIGNAL(pluginReset(KConfigGroup)), _decorationPluginObject, SLOT(load(KConfigGroup)) );
            connect( this, SIGNAL(pluginDefault()), _decorationPluginObject, SLOT(defaults()) );
            connect( this, SIGNAL(pluginToggleExpertMode(bool)), _decorationPluginObject, SLOT(toggleExpertMode(bool)) );
        }

        // expert mode
        emit pluginToggleExpertMode( true );

        // connections
        connect( button( Default ), SIGNAL(clicked()), SLOT(defaults()) );
        connect( button( Reset ), SIGNAL(clicked()), SLOT(reset()) );
        connect( button( Apply ), SIGNAL(clicked()), SLOT(save()) );
        connect( button( Ok ), SIGNAL(clicked()), SLOT(save()) );
        updateChanged();

    }

    //_______________________________________________________________
    void ConfigDialog::defaults( void )
    { emit pluginDefault(); }

    //_______________________________________________________________
    void ConfigDialog::reset( void )
    {
        KConfigGroup config;
        emit pluginReset( config );
    }

    //_______________________________________________________________
    void ConfigDialog::save( void )
    {

        // trigger pluggins to save themselves
        KConfigGroup config;
        emit pluginSave( config );

        // this is needed to trigger decoration update
        KGlobalSettings::self()->emitChange(KGlobalSettings::StyleChanged);

        // reset 'changed' flags
        updateStyleChanged( false );
        updateDecorationChanged( false );
        updateChanged();
    }

    //_______________________________________________________________
    void ConfigDialog::updateChanged( void )
    {
        bool modified( changed() );
        button( Apply )->setEnabled( modified );
        button( Reset )->setEnabled( modified );
        button( Ok )->setEnabled( modified );
        updateWindowTitle( pageWidget_->currentPage() );
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

        // load decoration from plugin
        KLibrary* library = new KLibrary( "kstyle_oxygen_config" );

        if (library->load())
        {
            KLibrary::void_function_ptr alloc_ptr = library->resolveFunction("allocate_kstyle_config");
            if (alloc_ptr != NULL)
            {

                // pointer to decoration plugin allocation function
                QWidget* (*allocator)( QWidget* );
                allocator = (QWidget* (*)(QWidget* parent))alloc_ptr;

                // create container
                KVBox* container = new KVBox();
                container->setObjectName( "oxygen-settings-container" );

                // allocate config object
                _stylePluginObject = (QObject*)(allocator( container ));
                return new KPageWidgetItem( container );

            } else {

                // fall back to warning label
                QLabel* label = new QLabel();
                label->setText( i18n( "Unable to find oxygen style configuration plugin" ) );
                return new KPageWidgetItem( label );

            }

        } else {
            delete library;

            // fall back to warning label
            QLabel* label = new QLabel();
            label->setText( i18n( "Unable to find oxygen style configuration plugin" ) );
            return new KPageWidgetItem( label );

        }

    }

    //_______________________________________________________________
    KPageWidgetItem* ConfigDialog::loadDecorationConfig( void )
    {

        // load decoration from plugin
        KLibrary* library = new KLibrary( "kwin_oxygen_config" );

        if (library->load())
        {
            KLibrary::void_function_ptr alloc_ptr = library->resolveFunction("allocate_config");
            if (alloc_ptr != NULL)
            {

                // pointer to decoration plugin allocation function
                QObject* (*allocator)( KConfigGroup&, QWidget* );
                allocator = (QObject* (*)(KConfigGroup& conf, QWidget* parent))alloc_ptr;

                // create container
                KVBox* container = new KVBox();

                // allocate config object
                KConfigGroup config;
                _decorationPluginObject = (QObject*)(allocator( config, container ));
                return new KPageWidgetItem( container );

            } else {

                // fall back to warning label
                QLabel* label = new QLabel();
                label->setText( i18n( "Unable to find oxygen decoration configuration plugin" ) );
                return new KPageWidgetItem( label );

            }

        } else {
            delete library;

            // fall back to warning label
            QLabel* label = new QLabel();
            label->setText( i18n( "Unable to find oxygen decoration configuration plugin" ) );
            return new KPageWidgetItem( label );

        }

    }

}
