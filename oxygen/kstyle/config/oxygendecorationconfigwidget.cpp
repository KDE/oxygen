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
        ConfigWidget( parent ),
        inactiveShadowConf_( "InactiveShadow" ),
        activeShadowConf_( "ActiveShadow" )

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
        connect( ui.inactiveShadowConfiguration_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.activeShadowConfiguration_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.useNarrowButtonSpacing_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.drawSeparator_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.drawTitleOutline_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( ui.buttonSize_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.titleAlignment_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.frameBorder_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.frameBorder_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.sizeGripMode_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.blendColor_, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );

        // shadow configurations
        setupShadowConf( inactiveShadowUi, ui.inactiveShadowConfiguration_ );
        setupShadowConf( activeShadowUi, ui.activeShadowConfiguration_ );

    }

    //_______________________________________________
    void DecorationConfigWidget::save( void )
    {
        saveShadowConf( inactiveShadowUi, inactiveShadowConf_ );
        saveShadowConf( activeShadowUi, activeShadowConf_ );

        OxygenStyleConfigData::setTabsEnabled( ui.tabsEnabled_->isChecked() );
        OxygenStyleConfigData::setUseAnimations( ui.useAnimations_->isChecked() );
        OxygenStyleConfigData::setAnimateTitleChange( ui.animateTitleChange_->isChecked() );
        OxygenStyleConfigData::setUseDropShadows( ui.inactiveShadowConfiguration_->isChecked() );
        OxygenStyleConfigData::setUseOxygenShadows( ui.activeShadowConfiguration_->isChecked() );
        OxygenStyleConfigData::setUseNarrowButtonSpacing( ui.useNarrowButtonSpacing_->isChecked() );
        OxygenStyleConfigData::setDrawSeparator( ui.drawSeparator_->isChecked() );
        OxygenStyleConfigData::setDrawTitleOutline( ui.drawTitleOutline_->isChecked() );
        OxygenStyleConfigData::setButtonSize( ui.buttonSize_->currentIndex() );
        OxygenStyleConfigData::setTitleAlignment( ui.titleAlignment_->currentIndex() );
        OxygenStyleConfigData::setFrameBorder( frameBorder( ui.frameBorder_->currentIndex() ) );
        OxygenStyleConfigData::setSizeGripMode( sizeGripMode( ui.sizeGripMode_->currentIndex() ) );
        OxygenStyleConfigData::setBlendColor( blendColor( ui.blendColor_->currentIndex() ) );

        setChanged( false );
    }

    //_______________________________________________
    void DecorationConfigWidget::load( void )
    {

        // shadow configurations
        loadShadowConf( inactiveShadowUi, inactiveShadowConf_ );
        loadShadowConf( activeShadowUi, activeShadowConf_ );

        ui.tabsEnabled_->setChecked( OxygenStyleConfigData::tabsEnabled() );
        ui.useAnimations_->setChecked( OxygenStyleConfigData::useAnimations() );
        ui.animateTitleChange_->setChecked( OxygenStyleConfigData::animateTitleChange() );
        ui.inactiveShadowConfiguration_->setChecked( OxygenStyleConfigData::useDropShadows() );
        ui.activeShadowConfiguration_->setChecked( OxygenStyleConfigData::useOxygenShadows() );
        ui.useNarrowButtonSpacing_->setChecked( OxygenStyleConfigData::useNarrowButtonSpacing() );
        ui.drawSeparator_->setChecked( OxygenStyleConfigData::drawSeparator() );
        ui.drawTitleOutline_->setChecked( OxygenStyleConfigData::drawTitleOutline() );
        ui.buttonSize_->setCurrentIndex( OxygenStyleConfigData::buttonSize() );
        ui.titleAlignment_->setCurrentIndex( OxygenStyleConfigData::titleAlignment() );

        // frame border
        for( int i=0; i<9; ++i )
        {
            if( frameBorder(i) == OxygenStyleConfigData::frameBorder() )
            {
                ui.frameBorder_->setCurrentIndex( i );
                break;
            }
        }

        // blend color
        for( int i=0; i<2; ++i )
        {
            if( blendColor(i) ==  OxygenStyleConfigData::blendColor() )
            {
                ui.blendColor_->setCurrentIndex( i );
                break;
            }
        }

        // size grip mode
        for( int i=0; i<2; ++i )
        {
            if( sizeGripMode(i) == OxygenStyleConfigData::sizeGripMode() )
            {
                ui.sizeGripMode_->setCurrentIndex( i );
                break;
            }
        }

    }

    //_______________________________________________
    void DecorationConfigWidget::defaults( void )
    {
        inactiveShadowConf_.setDefaults();
        activeShadowConf_.setDefaults();
    }

    //_______________________________________________
    void DecorationConfigWidget::updateChanged( void )
    {
        bool modified( false );
        if( shadowConfChanged( inactiveShadowUi, inactiveShadowConf_ ) ) modified = true;
        else if( shadowConfChanged( activeShadowUi, activeShadowConf_ ) ) modified = true;
        else if( ui.tabsEnabled_->isChecked() != OxygenStyleConfigData::tabsEnabled() ) modified = true;
        else if( ui.useAnimations_->isChecked() != OxygenStyleConfigData::useAnimations() ) modified = true;
        else if( ui.animateTitleChange_->isChecked() != OxygenStyleConfigData::animateTitleChange() ) modified = true;
        else if( ui.inactiveShadowConfiguration_->isChecked() != OxygenStyleConfigData::useDropShadows() ) modified = true;
        else if( ui.activeShadowConfiguration_->isChecked() != OxygenStyleConfigData::useOxygenShadows() ) modified = true;
        else if( ui.useNarrowButtonSpacing_->isChecked() != OxygenStyleConfigData::useNarrowButtonSpacing() ) modified = true;
        else if( ui.drawSeparator_->isChecked() != OxygenStyleConfigData::drawSeparator() ) modified = true;
        else if( ui.drawTitleOutline_->isChecked() != OxygenStyleConfigData::drawTitleOutline() ) modified = true;
        else if( ui.buttonSize_->currentIndex() != OxygenStyleConfigData::buttonSize() ) modified = true;
        else if( ui.titleAlignment_->currentIndex() != OxygenStyleConfigData::titleAlignment() ) modified = true;
        else if( frameBorder( ui.frameBorder_->currentIndex() ) != OxygenStyleConfigData::frameBorder() ) modified = true;
        else if( blendColor( ui.blendColor_->currentIndex() ) != OxygenStyleConfigData::blendColor() ) modified = true;
        else if( sizeGripMode( ui.sizeGripMode_->currentIndex() ) != OxygenStyleConfigData::sizeGripMode() ) modified = true;
        setChanged( modified );

    }

    //_______________________________________________
    void DecorationConfigWidget::setupShadowConf( Ui_ShadowConfigBox& ui, QWidget* widget )
    {
        ui.setupUi( widget );
        ui.outerColor_->setEnabled( false );
        connect( ui.useOuterColor_, SIGNAL( toggled( bool ) ), ui.outerColor_, SLOT( setEnabled( bool ) ) );
        connect( ui.size_, SIGNAL( valueChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.innerColor_, SIGNAL( changed( const QColor& ) ), SLOT( updateChanged() ) );
        connect( ui.outerColor_, SIGNAL( changed( const QColor& ) ), SLOT( updateChanged() ) );
        connect( ui.verticalOffset_, SIGNAL( valueChanged( int ) ), SLOT( updateChanged() ) );
        connect( ui.useOuterColor_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
    }

    //_______________________________________________
    void DecorationConfigWidget::saveShadowConf( const Ui_ShadowConfigBox& ui, ShadowConfigData& conf )
    {
        conf.setSize( ui.size_->value() );
        conf.setVerticalOffset( qreal( ui.verticalOffset_->value() )/10 );
        conf.setInnerColor( ui.innerColor_->color() );
        conf.setOuterColor( ui.outerColor_->color() );
        conf.setUseOuterColor( ui.useOuterColor_->isChecked() );
        conf.writeConfig();
    }

    //_______________________________________________
    void DecorationConfigWidget::loadShadowConf( const Ui_ShadowConfigBox& ui, ShadowConfigData& conf )
    {
        conf.readConfig();
        ui.size_->setValue( conf.size() );
        ui.verticalOffset_->setValue( conf.verticalOffset() * 10 );
        ui.innerColor_->setColor( conf.innerColor() );
        ui.outerColor_->setColor( conf.outerColor() );
        ui.useOuterColor_->setChecked( conf.useOuterColor() );
    }

    //_______________________________________________
    bool DecorationConfigWidget::shadowConfChanged( const Ui_ShadowConfigBox& ui, const ShadowConfigData& conf ) const
    {
        bool modified( false );
        if( ui.size_->value() != conf.size() ) modified = true;
        else if( ui.verticalOffset_->value() != int( conf.verticalOffset() * 10 ) ) modified = true;
        else if( ui.innerColor_->color() != conf.innerColor() ) modified = true;
        else if( ui.outerColor_->color() != conf.outerColor() ) modified = true;
        else if( ui.useOuterColor_->isChecked() != conf.useOuterColor() ) modified = true;
        return modified;

    }

    //_______________________________________________
    QString DecorationConfigWidget::frameBorder( int index ) const
    {

        switch( index )
        {
            case 0: return "No Border";
            case 1: default: return "No Side Border";
            case 2: return "Tiny";
            case 3: return "Normal";
            case 4: return "Large";
            case 5: return "Very Large";
            case 6: return "Huge";
            case 7: return "Very Huge";
            case 8: return "Oversized";
        }
    }

    //_______________________________________________
    QString DecorationConfigWidget::blendColor( int index ) const
    {
        switch( index )
        {
            case 0: return "Solid Color";
            case 1: default: return "Radial Gradient";
        }
    }

    //_______________________________________________
    QString DecorationConfigWidget::sizeGripMode( int index ) const
    {
        switch( index )
        {
            case 0: return "Always Hide Extra Size Grip";
            case 1: default: return "Show Extra Size Grip When Needed";
        }
    }

}
