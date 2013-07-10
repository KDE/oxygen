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

#include <QIcon>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QShortcut>
#include <QTextStream>
#include <QTimer>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KLibrary>
#include <KStandardShortcut>

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

        updateWindowTitle();

        setWindowTitle( i18n( "Oxygen Settings" ) );

        // install Quit shortcut
        foreach( const QKeySequence& sequence, KStandardShortcut::quit() )
        { connect( new QShortcut( sequence, this ), SIGNAL(activated()), SLOT(close()) ); }

        setLayout( new QVBoxLayout() );

        // button box
        _buttonBox = new QDialogButtonBox(
            QDialogButtonBox::RestoreDefaults|
            QDialogButtonBox::Reset|
            QDialogButtonBox::Apply|
            QDialogButtonBox::Ok|
            QDialogButtonBox::Cancel,
            Qt::Horizontal, this );

        connect( _buttonBox->button( QDialogButtonBox::Cancel ), SIGNAL(clicked()), SLOT(close()) );

        // tab widget
        _pageWidget = new KPageWidget( this );
        layout()->addWidget( _pageWidget );
        layout()->addWidget( _buttonBox );

        // connections
        connect( _pageWidget, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), SLOT(updateWindowTitle(KPageWidgetItem*)) );

        // generic page
        KPageWidgetItem *page;

        // style
        page = loadStyleConfig();
        page->setName( i18n("Widget Style") );
        page->setHeader( i18n("Modify the appearance of widgets") );
        page->setIcon( QIcon::fromTheme( QLatin1String( "preferences-desktop-theme" ) ) );
        _pageWidget->addPage( page );

        if( _stylePluginObject )
        {
            connect( _stylePluginObject, SIGNAL(changed(bool)), this, SLOT(updateStyleChanged(bool)) );
            connect( _stylePluginObject, SIGNAL(changed(bool)), this, SLOT(updateChanged()) );

            connect( _buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()), _stylePluginObject, SLOT(reset()) );
            connect( _buttonBox->button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()), _stylePluginObject, SLOT(defaults()) );
            connect( this, SIGNAL(pluginSave()), _stylePluginObject, SLOT(save()) );
            connect( this, SIGNAL(pluginToggleExpertMode(bool)), _stylePluginObject, SLOT(toggleExpertMode(bool)) );

        }

        // decoration
        page = loadDecorationConfig();
        page->setName( i18n("Window Decorations") );
        page->setHeader( i18n("Modify the appearance of window decorations") );
        page->setIcon( QIcon::fromTheme( QLatin1String( "preferences-system-windows" ) ) );
        _pageWidget->addPage( page );

        if( _decorationPluginObject )
        {
            connect( _decorationPluginObject, SIGNAL(changed(bool)), this, SLOT(updateDecorationChanged(bool)) );
            connect( _decorationPluginObject, SIGNAL(changed(bool)), this, SLOT(updateChanged()) );

            connect( _buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()), _decorationPluginObject, SLOT(load()) );
            connect( _buttonBox->button( QDialogButtonBox::RestoreDefaults ), SIGNAL(clicked()), _decorationPluginObject, SLOT(defaults()) );

            connect( this, SIGNAL(pluginSave()), _decorationPluginObject, SLOT(save()) );
            connect( this, SIGNAL(pluginToggleExpertMode(bool)), _decorationPluginObject, SLOT(toggleExpertMode(bool)) );

        }

        // expert mode
        emit pluginToggleExpertMode( true );

        // button connections
        connect( _buttonBox->button( QDialogButtonBox::Apply ), SIGNAL(clicked()), SLOT(save()) );
        connect( _buttonBox->button( QDialogButtonBox::Ok ), SIGNAL(clicked()), SLOT(save()) );
        updateChanged();

    }

    //_______________________________________________________________
    void ConfigDialog::save( void )
    {

        // trigger pluggins to save themselves
        emit pluginSave();

        // reset 'changed' flags
        updateStyleChanged( false );
        updateDecorationChanged( false );
        updateChanged();

    }

    //_______________________________________________________________
    void ConfigDialog::updateChanged( void )
    {
        bool modified( changed() );
        _buttonBox->button( QDialogButtonBox::Apply )->setEnabled( modified );
        _buttonBox->button( QDialogButtonBox::Reset )->setEnabled( modified );
        _buttonBox->button( QDialogButtonBox::Ok )->setEnabled( modified );
        updateWindowTitle( _pageWidget->currentPage() );
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
        KLibrary* library = new KLibrary( QLatin1String( "libkstyle_oxygen_config" ) );

        if (library->load())
        {
            KLibrary::void_function_ptr alloc_ptr = library->resolveFunction( "allocate_kstyle_config" );
            if (alloc_ptr != NULL)
            {

                // pointer to decoration plugin allocation function
                QWidget* (*allocator)( QWidget* );
                allocator = (QWidget* (*)(QWidget* parent))alloc_ptr;

                // create container
                QWidget* container = new QWidget();
                container->setLayout( new QVBoxLayout() );
                container->setObjectName( QLatin1String( "oxygen-settings-container" ) );

                // allocate config object
                _stylePluginObject = (QObject*)(allocator( container ));
                container->layout()->addWidget( static_cast<QWidget*>( _stylePluginObject ) );
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
        KLibrary* library = new KLibrary( QLatin1String( "kwin_oxygen_config" ) );

        if (library->load())
        {
            KLibrary::void_function_ptr alloc_ptr = library->resolveFunction( "allocate_config" );
            if (alloc_ptr != NULL)
            {

                // pointer to decoration plugin allocation function
                QObject* (*allocator)( KConfigGroup&, QWidget* );
                allocator = (QObject* (*)(KConfigGroup& conf, QWidget* parent))alloc_ptr;

                // create container
                QWidget* container = new QWidget();
                container->setLayout( new QVBoxLayout() );

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
