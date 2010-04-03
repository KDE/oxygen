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
#include "oxygenstyleconfigdata.h"

#include "oxygenanimationconfigwidget.h"
#include "oxygenappearanceconfigwidget.h"
#include "oxygendecorationconfigwidget.h"

#include <KGlobalSettings>
#include <KLocale>
#include <KPushButton>
#include <KPageWidget>
#include <KIcon>

namespace Oxygen
{
    //_______________________________________________________________
    ConfigDialog::ConfigDialog( QWidget* parent ):
        KDialog( parent )
    {
        setButtons( Default|Reset|Apply|Ok|Cancel );
        setDefaultButton( Cancel );
        showButtonSeparator( false );

        // tab widget
        KPageWidget* pageWidget( new KPageWidget( this ) );
        setMainWidget( pageWidget );

        // decoration
        KPageWidgetItem *page = new KPageWidgetItem( decorationConfigWidget_ = new DecorationConfigWidget() );
        page->setName( "Window Decorations" );
        page->setHeader( "Allows to modify the appearance of window decorations" );
        page->setIcon( KIcon( "preferences-system-windows" ) );
        pageWidget->addPage( page );

        // appearance
        page = new KPageWidgetItem( appearanceConfigWidget_ = new AppearanceConfigWidget() );
        page->setName( "Widget Style" );
        page->setHeader( "Allows to modify the appearance of widgets" );
        page->setIcon( KIcon( "preferences-desktop-theme" ) );
        pageWidget->addPage( page );

        page = new KPageWidgetItem( animationConfigWidget_ = new AnimationConfigWidget() );
        page->setName( "Widget Animations" );
        page->setHeader( "Allows the fine tuning of widget animations" );
        page->setIcon( KIcon( "preferences-desktop-theme" ) );
        pageWidget->addPage( page );

        // connections
        connect( appearanceConfigWidget_, SIGNAL( changed( bool ) ), SLOT( updateChanged( void ) ) );
        connect( animationConfigWidget_, SIGNAL( changed( bool ) ), SLOT( updateChanged( void ) ) );
        connect( decorationConfigWidget_, SIGNAL( changed( bool ) ), SLOT( updateChanged( void ) ) );
        connect( button( Default ), SIGNAL( clicked() ), SLOT( defaults() ) );
        connect( button( Reset ), SIGNAL( clicked() ), SLOT( load() ) );
        connect( button( Apply ), SIGNAL( clicked() ), SLOT( save() ) );
        connect( button( Ok ), SIGNAL( clicked() ), SLOT( save() ) );


    }

    //_______________________________________________________________
    void ConfigDialog::load( void )
    {
        OxygenStyleConfigData::self()->readConfig();
        appearanceConfigWidget_->load();
        animationConfigWidget_->load();
        decorationConfigWidget_->load();
        button( Apply )->setEnabled( false );
    }

    //_______________________________________________________________
    void ConfigDialog::defaults( void )
    {
        OxygenStyleConfigData::self()->setDefaults();
        appearanceConfigWidget_->load();
        animationConfigWidget_->load();
        decorationConfigWidget_->load();
    }

    //_______________________________________________________________
    void ConfigDialog::save( void )
    {
        appearanceConfigWidget_->save();
        animationConfigWidget_->save();
        decorationConfigWidget_->save();
        OxygenStyleConfigData::self()->writeConfig();
        KGlobalSettings::self()->emitChange(KGlobalSettings::StyleChanged);
    }

    //_______________________________________________________________
    void ConfigDialog::updateChanged( void )
    {
        bool changed( appearanceConfigWidget_->isChanged() || animationConfigWidget_->isChanged() || decorationConfigWidget_->isChanged() );
        button( Reset )->setEnabled( changed );
        button( Apply )->setEnabled( changed );
    }

}
