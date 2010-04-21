//////////////////////////////////////////////////////////////////////////////
// oxygendemodialog.cpp
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
#include "oxygendemodialog.h"
#include "oxygendemodialog.moc"

#include "oxygenbuttondemowidget.h"
#include "oxygeninputdemowidget.h"
#include "oxygentabdemowidget.h"

#include <QtCore/QTextStream>
#include <QtGui/QShortcut>
#include <QtGui/QDialogButtonBox>

#include <KIcon>
#include <KLocale>
#include <KPushButton>
#include <KStandardShortcut>

namespace Oxygen
{
    //_______________________________________________________________
    DemoDialog::DemoDialog( QWidget* parent ):
        KDialog( parent ),
        enableCheckBox_( 0 ),
        rightToLeftCheckBox_( 0 )
    {
        setButtons( Ok );
        showButtonSeparator( false );

        setWindowTitle( i18n( "Oxygen Demo" ) );

        // install Quit shortcut
        connect( new QShortcut( KStandardShortcut::quit().primary(), this ), SIGNAL( activated() ), SLOT( close() ) );
        connect( new QShortcut( KStandardShortcut::quit().alternate(), this ), SIGNAL( activated() ), SLOT( close() ) );

        // tab widget
        pageWidget_ = new KPageWidget( this );
        setMainWidget( pageWidget_ );

        connect( pageWidget_, SIGNAL( currentPageChanged( KPageWidgetItem*, KPageWidgetItem* ) ), SLOT( updateWindowTitle( KPageWidgetItem* ) ) );
        connect( pageWidget_, SIGNAL( currentPageChanged( KPageWidgetItem*, KPageWidgetItem* ) ), SLOT( updateEnableState( KPageWidgetItem* ) ) );
        KPageWidgetItem *page;

        // inputs
        {
            page = new KPageWidgetItem( inputDemoWidget_ = new InputDemoWidget() );
            page->setName( "Input Widgets" );
            page->setIcon( KIcon( "edit-rename" ) );
            page->setHeader( "Shows the appearance of text input widgets" );
            pageWidget_->addPage( page );
        }

        // buttons
        {
            page = new KPageWidgetItem( buttonDemoWidget_ = new ButtonDemoWidget() );
            page->setName( "Buttons" );
            page->setIcon( KIcon( "go-jump-locationbar" ) );
            page->setHeader( "Shows the appearance of buttons" );
            pageWidget_->addPage( page );
        }

        // lists
        {
            QWidget* widget;
            page = new KPageWidgetItem( widget = new QWidget() );
            listDemoWidgetUi_.setupUi( widget );
            page->setName( "Lists" );
            page->setIcon( KIcon( "view-list-tree" ) );
            page->setHeader( "Shows the appearance of lists and trees" );
            pageWidget_->addPage( page );
        }

        // tab
        {
            page = new KPageWidgetItem( tabDemoWidget_ = new TabDemoWidget() );
            page->setName( "Tab Widgets" );
            page->setIcon( KIcon( "tab-detach" ) );
            page->setHeader( "Shows the appearance of tab widgets" );
            pageWidget_->addPage( page );
        }

        // customize menu
        QList<QDialogButtonBox*> children( findChildren<QDialogButtonBox*>() );
        if( !children.isEmpty() )
        {
            QDialogButtonBox* buttonBox( children.front() );

            enableCheckBox_ = new QCheckBox( i18n( "Enabled" ) );
            enableCheckBox_->setChecked( true );
            connect( enableCheckBox_, SIGNAL( toggled( bool ) ), SLOT( toggleEnable( bool ) ) );
            buttonBox->addButton( enableCheckBox_, QDialogButtonBox::ResetRole );

            rightToLeftCheckBox_ = new QCheckBox( i18n( "Right to left layout" ) );
            connect( rightToLeftCheckBox_, SIGNAL( toggled( bool ) ), SLOT( toggleRightToLeft( bool ) ) );
            buttonBox->addButton( rightToLeftCheckBox_, QDialogButtonBox::ResetRole );

        }

    }

    //_______________________________________________________________
    void DemoDialog::updateWindowTitle( KPageWidgetItem* item )
    {

        QString title;
        QTextStream what( &title );
        if( item )
        {
            what << item->name();
            what << " - ";
        }

        what << i18n( "Oxygen Demo" );
        setWindowTitle( title );
    }

    //_______________________________________________________________
    void DemoDialog::updateEnableState( KPageWidgetItem* item )
    {
        if( !( item && item->widget() && enableCheckBox_ ) ) return;
        item->widget()->setEnabled( enableCheckBox_->isChecked() );
    }

    //_______________________________________________________________
    void DemoDialog::toggleEnable( bool value )
    {
        if( !( pageWidget_->currentPage() && pageWidget_->currentPage()->widget() ) ) return;
        pageWidget_->currentPage()->widget()->setEnabled( value );
    }

    //_______________________________________________________________
    void DemoDialog::toggleRightToLeft( bool value )
    { qApp->setLayoutDirection( value ? Qt::RightToLeft:Qt::LeftToRight ); }

}
