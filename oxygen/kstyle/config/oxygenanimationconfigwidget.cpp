//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.cpp
// animation configuration widget
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

#include "oxygenanimationconfigwidget.h"
#include "oxygenanimationconfigwidget.moc"
#include "oxygenanimationconfigitem.h"
#include "oxygenstyleconfigdata.h"

#include <QtGui/QButtonGroup>
#include <KLocale>

namespace Oxygen
{

    //_______________________________________________
    AnimationConfigWidget::AnimationConfigWidget( QWidget* parent ):
        ConfigWidget( parent ),
        row_(0)
    {

        QGridLayout* layout( new QGridLayout() );
        layout->setSpacing(0);
        layout->setMargin(0);
        setLayout( layout );

        setupItem( layout, genericAnimations_ = new GenericAnimationConfigItem( this,
            i18n("Focus, mouse-over and enability"),
            i18n("Configure widgets' focus and mouse-over animated highlight") ) );

        setupItem( layout, toolBarAnimations_ = new FollowMouseAnimationConfigItem( this,
            i18n("Tool bar highlight" ),
            i18n("Configure tool bars' mouse-over animated highlight" ) ) );
        toolBarAnimations_->hideDurationSpinBox();

        setupItem( layout, menuBarAnimations_ = new FollowMouseAnimationConfigItem( this,
            i18n("Menu bar highlight" ),
            i18n("Configure menu bars' mouse-over animated highlight" ) ) );

        setupItem( layout, menuAnimations_ = new FollowMouseAnimationConfigItem( this,
            i18n("Menu highlight" ),
            i18n("Configure menus' mouse-over animated highlight" ) ) );

        setupItem( layout, progressBarAnimations_ = new GenericAnimationConfigItem( this,
            i18n( "Progress bar animation" ),
            i18n( "Configure progress bars' steps animation" ) ) );

       setupItem( layout, progressBarBusyAnimations_ = new GenericAnimationConfigItem( this,
            i18n( "Busy indicator steps" ),
            i18n( "Configure progress bars' busy indicator animation" ) ) );

        setupItem( layout, stackedWidgetAnimations_ = new GenericAnimationConfigItem( this,
            i18n( "Tab transitions" ), i18n( "Configure fading transition between tabs" ) ) );

        setupItem( layout, labelAnimations_ = new GenericAnimationConfigItem( this,
            i18n( "Label transitions" ), i18n( "Configure fading transition when a label's text is changed" ) ) );

        setupItem( layout, lineEditAnimations_ = new GenericAnimationConfigItem( this,
            i18n( "Text editor transitions" ), i18n( "Configure fading transition when an editor's text is changed" ) ) );

        setupItem( layout, comboBoxAnimations_ = new GenericAnimationConfigItem( this,
            i18n( "Combo box transitions" ), i18n( "Configure fading transition when a combo box's selected choice is changed" ) ) );

        layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ), row_, 1, 1, 1 );
        ++row_;

    }

    //_______________________________________________
    void AnimationConfigWidget::load( void )
    {

        genericAnimations_->setEnabled( OxygenStyleConfigData::genericAnimationsEnabled() );
        genericAnimations_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );

        toolBarAnimations_->setEnabled( OxygenStyleConfigData::toolBarAnimationType() != OxygenStyleConfigData::TB_NONE );
        toolBarAnimations_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        toolBarAnimations_->setFollowMouseDuration( OxygenStyleConfigData::toolBarAnimationsDuration() );

        switch( OxygenStyleConfigData::toolBarAnimationType() )
        {
            case OxygenStyleConfigData::TB_FOLLOW_MOUSE:
            toolBarAnimations_->setType( 1 );
            break;

            case OxygenStyleConfigData::TB_FADE:
            default:
            toolBarAnimations_->setType( 0 );
            break;
        }

        menuBarAnimations_->setEnabled( OxygenStyleConfigData::menuBarAnimationType() != OxygenStyleConfigData::MB_NONE );
        menuBarAnimations_->setDuration( OxygenStyleConfigData::menuBarAnimationsDuration() );
        menuBarAnimations_->setFollowMouseDuration( OxygenStyleConfigData::menuBarFollowMouseAnimationsDuration() );
        switch( OxygenStyleConfigData::menuBarAnimationType() )
        {
            case OxygenStyleConfigData::MB_FOLLOW_MOUSE:
            menuBarAnimations_->setType( 1 );
            break;

            case OxygenStyleConfigData::MB_FADE:
            default:
            menuBarAnimations_->setType( 0 );
            break;
        }

        menuAnimations_->setEnabled( OxygenStyleConfigData::menuAnimationType() != OxygenStyleConfigData::ME_NONE );
        menuAnimations_->setDuration( OxygenStyleConfigData::menuAnimationsDuration() );
        menuAnimations_->setFollowMouseDuration( OxygenStyleConfigData::menuFollowMouseAnimationsDuration() );
        switch( OxygenStyleConfigData::menuAnimationType() )
        {
            case OxygenStyleConfigData::ME_FOLLOW_MOUSE:
            menuAnimations_->setType( 1 );
            break;

            case OxygenStyleConfigData::ME_FADE:
            default:
            menuAnimations_->setType( 0 );
            break;
        }

        progressBarAnimations_->setEnabled( OxygenStyleConfigData::progressBarAnimationsEnabled() );
        progressBarAnimations_->setDuration( OxygenStyleConfigData::progressBarAnimationsDuration() );

        progressBarBusyAnimations_->setEnabled( OxygenStyleConfigData::progressBarAnimated() );
        progressBarBusyAnimations_->setDuration( OxygenStyleConfigData::progressBarBusyStepDuration() );

        stackedWidgetAnimations_->setEnabled( OxygenStyleConfigData::stackedWidgetTransitionsEnabled() );
        stackedWidgetAnimations_->setDuration( OxygenStyleConfigData::stackedWidgetTransitionsDuration() );

        labelAnimations_->setEnabled( OxygenStyleConfigData::labelTransitionsEnabled() );
        labelAnimations_->setDuration( OxygenStyleConfigData::labelTransitionsDuration() );

        lineEditAnimations_->setEnabled( OxygenStyleConfigData::lineEditTransitionsEnabled() );
        lineEditAnimations_->setDuration( OxygenStyleConfigData::lineEditTransitionsDuration() );

        comboBoxAnimations_->setEnabled( OxygenStyleConfigData::comboBoxTransitionsEnabled() );
        comboBoxAnimations_->setDuration( OxygenStyleConfigData::comboBoxTransitionsDuration() );
    }

    //_______________________________________________
    void AnimationConfigWidget::save( void )
    {

        OxygenStyleConfigData::setGenericAnimationsEnabled( genericAnimations_->enabled() );
        OxygenStyleConfigData::setGenericAnimationsDuration( genericAnimations_->duration() );

        OxygenStyleConfigData::setToolBarAnimationsDuration( toolBarAnimations_->followMouseDuration() );
        if( !toolBarAnimations_->enabled() ) OxygenStyleConfigData::setToolBarAnimationType( OxygenStyleConfigData::TB_NONE );
        else if( toolBarAnimations_->type() == 1 ) OxygenStyleConfigData::setToolBarAnimationType( OxygenStyleConfigData::TB_FOLLOW_MOUSE );
        else OxygenStyleConfigData::setToolBarAnimationType( OxygenStyleConfigData::TB_FADE );

        OxygenStyleConfigData::setMenuBarAnimationsDuration( menuBarAnimations_->duration() );
        OxygenStyleConfigData::setMenuBarFollowMouseAnimationsDuration( menuBarAnimations_->followMouseDuration() );
        if( !menuBarAnimations_->enabled() ) OxygenStyleConfigData::setMenuBarAnimationType( OxygenStyleConfigData::MB_NONE );
        else if( menuBarAnimations_->type() == 1 ) OxygenStyleConfigData::setMenuBarAnimationType( OxygenStyleConfigData::MB_FOLLOW_MOUSE );
        else OxygenStyleConfigData::setMenuBarAnimationType( OxygenStyleConfigData::MB_FADE );

        OxygenStyleConfigData::setMenuAnimationsDuration( menuAnimations_->duration() );
        OxygenStyleConfigData::setMenuFollowMouseAnimationsDuration( menuAnimations_->followMouseDuration() );
        if( !menuAnimations_->enabled() ) OxygenStyleConfigData::setMenuAnimationType( OxygenStyleConfigData::ME_NONE );
        else if( menuAnimations_->type() == 1 ) OxygenStyleConfigData::setMenuAnimationType( OxygenStyleConfigData::ME_FOLLOW_MOUSE );
        else OxygenStyleConfigData::setMenuAnimationType( OxygenStyleConfigData::ME_FADE );

        OxygenStyleConfigData::setProgressBarAnimationsEnabled( progressBarAnimations_->enabled() );
        OxygenStyleConfigData::setProgressBarAnimationsDuration( progressBarAnimations_->duration() );

        OxygenStyleConfigData::setProgressBarAnimated( progressBarBusyAnimations_->enabled() );
        OxygenStyleConfigData::setProgressBarBusyStepDuration( progressBarBusyAnimations_->duration() );

        OxygenStyleConfigData::setStackedWidgetTransitionsEnabled( stackedWidgetAnimations_->enabled() );
        OxygenStyleConfigData::setStackedWidgetTransitionsDuration( stackedWidgetAnimations_->duration() );

        OxygenStyleConfigData::setLabelTransitionsEnabled( labelAnimations_->enabled() );
        OxygenStyleConfigData::setLabelTransitionsDuration( labelAnimations_->duration() );

        OxygenStyleConfigData::setLineEditTransitionsEnabled( lineEditAnimations_->enabled() );
        OxygenStyleConfigData::setLineEditTransitionsDuration( lineEditAnimations_->duration() );

        OxygenStyleConfigData::setComboBoxTransitionsEnabled( comboBoxAnimations_->enabled() );
        OxygenStyleConfigData::setComboBoxTransitionsDuration( comboBoxAnimations_->duration() );
        setChanged( false );

    }

    //_______________________________________________
    void AnimationConfigWidget::updateChanged( void )
    {
        bool modified( false );
        if( genericAnimations_->enabled() != OxygenStyleConfigData::genericAnimationsEnabled() ) { modified = true; }
        else if( genericAnimations_->duration() != OxygenStyleConfigData::genericAnimationsDuration() ) { modified = true; }

        else if( toolBarAnimations_->duration() != OxygenStyleConfigData::genericAnimationsDuration() ) { modified = true; }
        else if( toolBarAnimations_->followMouseDuration() != OxygenStyleConfigData::toolBarAnimationsDuration() ) { modified = true; }
        else if( OxygenStyleConfigData::toolBarAnimationType() == OxygenStyleConfigData::TB_NONE && toolBarAnimations_->enabled() ) { modified = true; }
        else if( OxygenStyleConfigData::toolBarAnimationType() == OxygenStyleConfigData::TB_FOLLOW_MOUSE && !( toolBarAnimations_->type() == 1 && toolBarAnimations_->enabled() ) ) { modified = true; }
        else if( OxygenStyleConfigData::toolBarAnimationType() == OxygenStyleConfigData::TB_FADE && !( toolBarAnimations_->type() == 0 && toolBarAnimations_->enabled() )) { modified = true; }

        else if( menuBarAnimations_->duration() != OxygenStyleConfigData::menuBarAnimationsDuration() ) { modified = true; }
        else if( menuBarAnimations_->followMouseDuration() != OxygenStyleConfigData::menuBarFollowMouseAnimationsDuration() ) { modified = true; }
        else if( OxygenStyleConfigData::menuBarAnimationType() == OxygenStyleConfigData::MB_NONE && menuBarAnimations_->enabled() ) { modified = true; }
        else if( OxygenStyleConfigData::menuBarAnimationType() == OxygenStyleConfigData::MB_FOLLOW_MOUSE && !( menuBarAnimations_->type() == 1 && menuBarAnimations_->enabled() ) ) { modified = true; }
        else if( OxygenStyleConfigData::menuBarAnimationType() == OxygenStyleConfigData::MB_FADE && !( menuBarAnimations_->type() == 0 && menuBarAnimations_->enabled() ) ) { modified = true; }

        else if( menuAnimations_->duration() != OxygenStyleConfigData::menuAnimationsDuration() ) { modified = true; }
        else if( menuAnimations_->followMouseDuration() != OxygenStyleConfigData::menuFollowMouseAnimationsDuration() ) { modified = true; }
        else if( OxygenStyleConfigData::menuAnimationType() == OxygenStyleConfigData::ME_NONE && menuAnimations_->enabled() ) { modified = true; }
        else if( OxygenStyleConfigData::menuAnimationType() == OxygenStyleConfigData::ME_FOLLOW_MOUSE && !( menuAnimations_->type() == 1 && menuAnimations_->enabled() ) ) { modified = true; }
        else if( OxygenStyleConfigData::menuAnimationType() == OxygenStyleConfigData::ME_FADE && !( menuAnimations_->type() == 0 && menuAnimations_->enabled() ) ) { modified = true; }

        else if( progressBarAnimations_->enabled() != OxygenStyleConfigData::progressBarAnimationsEnabled() ) { modified = true; }
        else if( progressBarAnimations_->duration() != OxygenStyleConfigData::progressBarAnimationsDuration() ) { modified = true; }

        else if( progressBarBusyAnimations_->enabled() != OxygenStyleConfigData::progressBarAnimated() ) { modified = true; }
        else if( progressBarBusyAnimations_->duration() != OxygenStyleConfigData::progressBarBusyStepDuration() ) { modified = true; }

        else if( stackedWidgetAnimations_->enabled() != OxygenStyleConfigData::stackedWidgetTransitionsEnabled() ) { modified = true; }
        else if( stackedWidgetAnimations_->duration() != OxygenStyleConfigData::stackedWidgetTransitionsDuration() ) { modified = true; }

        else if( labelAnimations_->enabled() != OxygenStyleConfigData::labelTransitionsEnabled() ) { modified = true; }
        else if( labelAnimations_->duration() != OxygenStyleConfigData::labelTransitionsDuration() ) { modified = true; }

        else if( lineEditAnimations_->enabled() != OxygenStyleConfigData::lineEditTransitionsEnabled() ) { modified = true; }
        else if( lineEditAnimations_->duration() != OxygenStyleConfigData::lineEditTransitionsDuration() ) { modified = true; }

        else if( comboBoxAnimations_->enabled() != OxygenStyleConfigData::comboBoxTransitionsEnabled() ) { modified = true; }
        else if( comboBoxAnimations_->duration() != OxygenStyleConfigData::comboBoxTransitionsDuration() ) { modified = true; }

        setChanged( modified );

    }

    //_______________________________________________
    void AnimationConfigWidget::updateItems( bool state )
    {
        if( !state ) return;
        foreach( AnimationConfigItem* item, findChildren<AnimationConfigItem*>() )
        { if( item->configurationWidget()->isVisible() ) item->configurationButton()->setChecked( false ); }
    }

    //_______________________________________________
    void AnimationConfigWidget::setupItem( QGridLayout* layout, AnimationConfigItem* item )
    {
        layout->addWidget( item, row_, 0, 1, 2 );
        ++row_;

        connect( item->configurationButton(), SIGNAL( toggled( bool ) ), SLOT( updateItems( bool ) ) );

        item->initializeConfigurationWidget( this );
        layout->addWidget( item->configurationWidget(), row_, 1, 1, 1 );
        if( row_ == 1 ) layout->addItem( new QSpacerItem( 25, 0 ), row_, 0, 1, 1 );
        ++row_;

        item->configurationWidget()->setVisible( false );
        connect( item, SIGNAL( changed( void ) ), SLOT( updateChanged( void ) ) );
    }

}
