//////////////////////////////////////////////////////////////////////////////
// oxygenappearanceconfigwidget.cpp
// appearance configuration widget
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

#include "oxygenappearanceconfigwidget.h"
#include "oxygenappearanceconfigwidget.moc"
#include "oxygenstyleconfigdata.h"
#include <KIcon>

namespace Oxygen
{
    //________________________________________
    AppearanceConfigWidget::AppearanceConfigWidget( QWidget* parent ):
        ConfigWidget( parent )
    {
        ui.setupUi( this );
        layout()->setMargin(0);

        // connections
        connect( ui.viewDrawTriangularExpander_, SIGNAL( toggled( bool ) ), ui.viewTriangularExpanderSize_, SLOT( setEnabled( bool ) ) );
        ui.viewTriangularExpanderSize_->setEnabled( false );

        connect( ui.toolBarDrawItemSeparator_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.checkDrawX_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.viewDrawFocusIndicator_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.viewDrawTreeBranchLines_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.viewDrawTriangularExpander_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.viewTriangularExpanderSize_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.tabStylePlain_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.tabStyleSingle_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.scrollBarBevel_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.scrollBarColored_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.scrollBarWidth_, SIGNAL( valueChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.menuHighlightStrong_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.menuHighlightSubtle_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.menuHighlightDark_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );

    }

    //_______________________________________________
    void AppearanceConfigWidget::save( void )
    {

        OxygenStyleConfigData::setToolBarDrawItemSeparator( ui.toolBarDrawItemSeparator_->isChecked() );
        OxygenStyleConfigData::setCheckBoxStyle( ui.checkDrawX_->isChecked() ? OxygenStyleConfigData::CS_X : OxygenStyleConfigData::CS_CHECK );
        OxygenStyleConfigData::setViewDrawFocusIndicator( ui.viewDrawFocusIndicator_->isChecked() );
        OxygenStyleConfigData::setViewDrawTreeBranchLines( ui.viewDrawTreeBranchLines_->isChecked() );
        OxygenStyleConfigData::setViewDrawTriangularExpander( ui.viewDrawTriangularExpander_->isChecked() );
        OxygenStyleConfigData::setViewTriangularExpanderSize( ui.viewTriangularExpanderSize_->currentIndex() );
        OxygenStyleConfigData::setTabStyle( ui.tabStylePlain_->isChecked() ? OxygenStyleConfigData::TS_PLAIN : OxygenStyleConfigData::TS_SINGLE );
        OxygenStyleConfigData::setScrollBarBevel( ui.scrollBarBevel_->isChecked() );
        OxygenStyleConfigData::setScrollBarColored( ui.scrollBarColored_->isChecked() );
        OxygenStyleConfigData::setScrollBarWidth( ui.scrollBarWidth_->value() );
        if( ui.menuHighlightStrong_->isChecked() ) OxygenStyleConfigData::setMenuHighlightMode( OxygenStyleConfigData::MM_STRONG );
        else if( ui.menuHighlightSubtle_->isChecked() ) OxygenStyleConfigData::setMenuHighlightMode( OxygenStyleConfigData::MM_SUBTLE );
        else OxygenStyleConfigData::setMenuHighlightMode( OxygenStyleConfigData::MM_DARK );
        setChanged( false );
    }

    //_______________________________________________
    void AppearanceConfigWidget::load( void )
    {
        ui.toolBarDrawItemSeparator_->setChecked( OxygenStyleConfigData::toolBarDrawItemSeparator() );
        ui.checkDrawX_->setChecked( OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_X );
        ui.viewDrawFocusIndicator_->setChecked( OxygenStyleConfigData::viewDrawFocusIndicator() );
        ui.viewDrawTreeBranchLines_->setChecked( OxygenStyleConfigData::viewDrawTreeBranchLines() );
        ui.viewDrawTriangularExpander_->setChecked( OxygenStyleConfigData::viewDrawTriangularExpander() );
        ui.viewTriangularExpanderSize_->setCurrentIndex( OxygenStyleConfigData::viewTriangularExpanderSize() );
        ui.tabStylePlain_->setChecked( OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_PLAIN );
        ui.tabStyleSingle_->setChecked( OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_SINGLE );
        ui.scrollBarBevel_->setChecked( OxygenStyleConfigData::scrollBarBevel() );
        ui.scrollBarColored_->setChecked( OxygenStyleConfigData::scrollBarColored() );
        ui.scrollBarWidth_->setValue( OxygenStyleConfigData::scrollBarWidth() );
        ui.menuHighlightStrong_->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG );
        ui.menuHighlightSubtle_->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );
        ui.menuHighlightDark_->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_DARK );
        setChanged( false );
    }

    //_______________________________________________
    void AppearanceConfigWidget::updateChanged( void )
    {
        bool modified( false );
        if( ui.toolBarDrawItemSeparator_->isChecked() != OxygenStyleConfigData::toolBarDrawItemSeparator() ) modified = true;
        else if( ui.checkDrawX_->isChecked() != (OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_X) ) modified = true;
        else if( ui.viewDrawFocusIndicator_->isChecked() != OxygenStyleConfigData::viewDrawFocusIndicator() ) modified = true;
        else if( ui.viewDrawTreeBranchLines_->isChecked() != OxygenStyleConfigData::viewDrawTreeBranchLines() ) modified = true;
        else if( ui.viewDrawTriangularExpander_->isChecked() != OxygenStyleConfigData::viewDrawTriangularExpander() ) modified = true;
        else if( ui.viewTriangularExpanderSize_->currentIndex() != OxygenStyleConfigData::viewTriangularExpanderSize() ) modified = true;
        else if( ui.tabStylePlain_->isChecked() != (OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_PLAIN) ) modified = true;
        else if( ui.tabStyleSingle_->isChecked() != (OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_SINGLE) ) modified = true;
        else if( ui.scrollBarBevel_->isChecked() != OxygenStyleConfigData::scrollBarBevel() ) modified = true;
        else if( ui.scrollBarColored_->isChecked() != OxygenStyleConfigData::scrollBarColored() ) modified = true;
        else if( ui.scrollBarWidth_->value() != OxygenStyleConfigData::scrollBarWidth() ) modified = true;
        else if( ui.menuHighlightStrong_->isChecked() != (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG) ) modified = true;
        else if( ui.menuHighlightSubtle_->isChecked() != (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE) ) modified = true;
        else if( ui.menuHighlightDark_->isChecked() != (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_DARK) ) modified = true;
        setChanged( modified );
    }

}
