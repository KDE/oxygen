//////////////////////////////////////////////////////////////////////////////
// oxygendecorationconfigwidget.cpp
// decoration configuration widget
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

#include "oxygendecorationconfigwidget.h"
#include "oxygendecorationconfigwidget.moc"
#include "oxygenstyleconfigdata.h"
#include <KIcon>

namespace Oxygen
{
    //________________________________________
    DecorationConfigWidget::DecorationConfigWidget( QWidget* parent ):
        ConfigWidget( parent )
    {
        ui.setupUi( this );
        layout()->setMargin(0);

        // connections
        connect( ui.useAnimations_, SIGNAL( toggled( bool ) ), ui.animateTitleChange_, SLOT( setEnabled( bool ) ) );
        connect( ui.drawTitleOutline_, SIGNAL( toggled( bool ) ), ui.drawSeparator_, SLOT( setDisabled( bool ) ) );
        ui.animateTitleChange_->setEnabled( false );

        connect( ui.tabsEnabled_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.useAnimations_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.animateTitleChange_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.useDropShadows_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.useOxygenShadows_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.useNarrowButtonSpacing_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.drawSeparator_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.drawTitleOutline_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.buttonSize_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.titleAlignment_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );

    }

    //_______________________________________________
    void DecorationConfigWidget::save( void )
    {
        OxygenStyleConfigData::setTabsEnabled( ui.tabsEnabled_->isChecked() );
        OxygenStyleConfigData::setUseAnimations( ui.useAnimations_->isChecked() );
        OxygenStyleConfigData::setAnimateTitleChange( ui.animateTitleChange_->isChecked() );
        OxygenStyleConfigData::setUseDropShadows( ui.useDropShadows_->isChecked() );
        OxygenStyleConfigData::setUseOxygenShadows( ui.useOxygenShadows_->isChecked() );
        OxygenStyleConfigData::setUseNarrowButtonSpacing( ui.useNarrowButtonSpacing_->isChecked() );
        OxygenStyleConfigData::setDrawSeparator( ui.drawSeparator_->isChecked() );
        OxygenStyleConfigData::setDrawTitleOutline( ui.drawTitleOutline_->isChecked() );
        OxygenStyleConfigData::setButtonSize( ui.buttonSize_->currentIndex() );
        OxygenStyleConfigData::setTitleAlignment( ui.titleAlignment_->currentIndex() );
        setChanged( false );
    }

    //_______________________________________________
    void DecorationConfigWidget::load( void )
    {
        ui.tabsEnabled_->setChecked( OxygenStyleConfigData::tabsEnabled() );
        ui.useAnimations_->setChecked( OxygenStyleConfigData::useAnimations() );
        ui.animateTitleChange_->setChecked( OxygenStyleConfigData::animateTitleChange() );
        ui.useDropShadows_->setChecked( OxygenStyleConfigData::useDropShadows() );
        ui.useOxygenShadows_->setChecked( OxygenStyleConfigData::useOxygenShadows() );
        ui.useNarrowButtonSpacing_->setChecked( OxygenStyleConfigData::useNarrowButtonSpacing() );
        ui.drawSeparator_->setChecked( OxygenStyleConfigData::drawSeparator() );
        ui.drawTitleOutline_->setChecked( OxygenStyleConfigData::drawTitleOutline() );
        ui.buttonSize_->setCurrentIndex( OxygenStyleConfigData::buttonSize() );
        ui.titleAlignment_->setCurrentIndex( OxygenStyleConfigData::titleAlignment() );
        setChanged( false );
    }

    //_______________________________________________
    void DecorationConfigWidget::updateChanged( void )
    {
        bool modified( false );
        if( ui.tabsEnabled_->isChecked() != OxygenStyleConfigData::tabsEnabled() ) modified = true;
        else if( ui.useAnimations_->isChecked() != OxygenStyleConfigData::useAnimations() ) modified = true;
        else if( ui.animateTitleChange_->isChecked() != OxygenStyleConfigData::animateTitleChange() ) modified = true;
        else if( ui.useDropShadows_->isChecked() != OxygenStyleConfigData::useDropShadows() ) modified = true;
        else if( ui.useOxygenShadows_->isChecked() != OxygenStyleConfigData::useOxygenShadows() ) modified = true;
        else if( ui.useNarrowButtonSpacing_->isChecked() != OxygenStyleConfigData::useNarrowButtonSpacing() ) modified = true;
        else if( ui.drawSeparator_->isChecked() != OxygenStyleConfigData::drawSeparator() ) modified = true;
        else if( ui.drawTitleOutline_->isChecked() != OxygenStyleConfigData::drawTitleOutline() ) modified = true;
        else if( ui.buttonSize_->currentIndex() != OxygenStyleConfigData::buttonSize() ) modified = true;
        else if( ui.titleAlignment_->currentIndex() != OxygenStyleConfigData::titleAlignment() ) modified = true;

        setChanged( modified );

    }

}
