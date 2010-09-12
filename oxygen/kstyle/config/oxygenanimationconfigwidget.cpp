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
#include <QtGui/QHoverEvent>
#include <QtCore/QTextStream>
#include <KLocale>

namespace Oxygen
{

    //_______________________________________________
    AnimationConfigWidget::AnimationConfigWidget( QWidget* parent ):
        QWidget( parent ),
        changed_( false ),
        row_(0)
    {

        ui.setupUi( this );
        QGridLayout* layout( qobject_cast<QGridLayout*>( AnimationConfigWidget::layout() ) );
        row_ = layout->rowCount();

        setupItem( layout, genericAnimations_ = new GenericAnimationConfigItem( this,
            i18n("Focus, mouseover and widget state transition"),
            i18n("Configure widgets' focus and mouseover highlight animation, as well as widget enabled/disabled state transition") ) );

        setupItem( layout, toolBarAnimations_ = new FollowMouseAnimationConfigItem( this,
            i18n("Toolbar highlight" ),
            i18n("Configure toolbars' mouseover highlight animation" ) ) );
        toolBarAnimations_->hideDurationSpinBox();

        setupItem( layout, menuBarAnimations_ = new FollowMouseAnimationConfigItem( this,
            i18n("Menu bar highlight" ),
            i18n("Configure menu bars' mouseover highlight animation" ) ) );

        setupItem( layout, menuAnimations_ = new FollowMouseAnimationConfigItem( this,
            i18n("Menu highlight" ),
            i18n("Configure menus' mouseover highlight animation" ) ) );

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

        // add spacers to the first column, previous row to finalize layout
        layout->addItem( new QSpacerItem( 25, 0 ), row_-1, 0, 1, 1 );

        // add vertical spacer
        layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ), row_, 1, 1, 1 );
        ++row_;

        connect( ui.animationsEnabled_, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        foreach( AnimationConfigItem* item, findChildren<AnimationConfigItem*>() )
        {
            item->QWidget::setEnabled( false );
            connect( ui.animationsEnabled_, SIGNAL( toggled( bool ) ), item, SLOT( setEnabled( bool ) ) );
        }

    }

    //_______________________________________________
    void AnimationConfigWidget::load( void )
    {

        ui.animationsEnabled_->setChecked( StyleConfigData::animationsEnabled() );
        genericAnimations_->setEnabled( StyleConfigData::genericAnimationsEnabled() );
        genericAnimations_->setDuration( StyleConfigData::genericAnimationsDuration() );

        toolBarAnimations_->setEnabled( StyleConfigData::toolBarAnimationType() != StyleConfigData::TB_NONE );
        toolBarAnimations_->setDuration( StyleConfigData::genericAnimationsDuration() );
        toolBarAnimations_->setFollowMouseDuration( StyleConfigData::toolBarAnimationsDuration() );

        switch( StyleConfigData::toolBarAnimationType() )
        {
            case StyleConfigData::TB_FOLLOW_MOUSE:
            toolBarAnimations_->setType( 1 );
            break;

            case StyleConfigData::TB_FADE:
            default:
            toolBarAnimations_->setType( 0 );
            break;
        }

        menuBarAnimations_->setEnabled( StyleConfigData::menuBarAnimationType() != StyleConfigData::MB_NONE );
        menuBarAnimations_->setDuration( StyleConfigData::menuBarAnimationsDuration() );
        menuBarAnimations_->setFollowMouseDuration( StyleConfigData::menuBarFollowMouseAnimationsDuration() );
        switch( StyleConfigData::menuBarAnimationType() )
        {
            case StyleConfigData::MB_FOLLOW_MOUSE:
            menuBarAnimations_->setType( 1 );
            break;

            case StyleConfigData::MB_FADE:
            default:
            menuBarAnimations_->setType( 0 );
            break;
        }

        menuAnimations_->setEnabled( StyleConfigData::menuAnimationType() != StyleConfigData::ME_NONE );
        menuAnimations_->setDuration( StyleConfigData::menuAnimationsDuration() );
        menuAnimations_->setFollowMouseDuration( StyleConfigData::menuFollowMouseAnimationsDuration() );
        switch( StyleConfigData::menuAnimationType() )
        {
            case StyleConfigData::ME_FOLLOW_MOUSE:
            menuAnimations_->setType( 1 );
            break;

            case StyleConfigData::ME_FADE:
            default:
            menuAnimations_->setType( 0 );
            break;
        }

        progressBarAnimations_->setEnabled( StyleConfigData::progressBarAnimationsEnabled() );
        progressBarAnimations_->setDuration( StyleConfigData::progressBarAnimationsDuration() );

        progressBarBusyAnimations_->setEnabled( StyleConfigData::progressBarAnimated() );
        progressBarBusyAnimations_->setDuration( StyleConfigData::progressBarBusyStepDuration() );

        stackedWidgetAnimations_->setEnabled( StyleConfigData::stackedWidgetTransitionsEnabled() );
        stackedWidgetAnimations_->setDuration( StyleConfigData::stackedWidgetTransitionsDuration() );

        labelAnimations_->setEnabled( StyleConfigData::labelTransitionsEnabled() );
        labelAnimations_->setDuration( StyleConfigData::labelTransitionsDuration() );

        lineEditAnimations_->setEnabled( StyleConfigData::lineEditTransitionsEnabled() );
        lineEditAnimations_->setDuration( StyleConfigData::lineEditTransitionsDuration() );

        comboBoxAnimations_->setEnabled( StyleConfigData::comboBoxTransitionsEnabled() );
        comboBoxAnimations_->setDuration( StyleConfigData::comboBoxTransitionsDuration() );
    }

    //_______________________________________________
    void AnimationConfigWidget::save( void )
    {

        StyleConfigData::setAnimationsEnabled( ui.animationsEnabled_->isChecked() );
        StyleConfigData::setGenericAnimationsEnabled( genericAnimations_->enabled() );
        StyleConfigData::setGenericAnimationsDuration( genericAnimations_->duration() );

        StyleConfigData::setToolBarAnimationsDuration( toolBarAnimations_->followMouseDuration() );
        if( !toolBarAnimations_->enabled() ) StyleConfigData::setToolBarAnimationType( StyleConfigData::TB_NONE );
        else if( toolBarAnimations_->type() == 1 ) StyleConfigData::setToolBarAnimationType( StyleConfigData::TB_FOLLOW_MOUSE );
        else StyleConfigData::setToolBarAnimationType( StyleConfigData::TB_FADE );

        StyleConfigData::setMenuBarAnimationsDuration( menuBarAnimations_->duration() );
        StyleConfigData::setMenuBarFollowMouseAnimationsDuration( menuBarAnimations_->followMouseDuration() );
        if( !menuBarAnimations_->enabled() ) StyleConfigData::setMenuBarAnimationType( StyleConfigData::MB_NONE );
        else if( menuBarAnimations_->type() == 1 ) StyleConfigData::setMenuBarAnimationType( StyleConfigData::MB_FOLLOW_MOUSE );
        else StyleConfigData::setMenuBarAnimationType( StyleConfigData::MB_FADE );

        StyleConfigData::setMenuAnimationsDuration( menuAnimations_->duration() );
        StyleConfigData::setMenuFollowMouseAnimationsDuration( menuAnimations_->followMouseDuration() );
        if( !menuAnimations_->enabled() ) StyleConfigData::setMenuAnimationType( StyleConfigData::ME_NONE );
        else if( menuAnimations_->type() == 1 ) StyleConfigData::setMenuAnimationType( StyleConfigData::ME_FOLLOW_MOUSE );
        else StyleConfigData::setMenuAnimationType( StyleConfigData::ME_FADE );

        StyleConfigData::setProgressBarAnimationsEnabled( progressBarAnimations_->enabled() );
        StyleConfigData::setProgressBarAnimationsDuration( progressBarAnimations_->duration() );

        StyleConfigData::setProgressBarAnimated( progressBarBusyAnimations_->enabled() );
        StyleConfigData::setProgressBarBusyStepDuration( progressBarBusyAnimations_->duration() );

        StyleConfigData::setStackedWidgetTransitionsEnabled( stackedWidgetAnimations_->enabled() );
        StyleConfigData::setStackedWidgetTransitionsDuration( stackedWidgetAnimations_->duration() );

        StyleConfigData::setLabelTransitionsEnabled( labelAnimations_->enabled() );
        StyleConfigData::setLabelTransitionsDuration( labelAnimations_->duration() );

        StyleConfigData::setLineEditTransitionsEnabled( lineEditAnimations_->enabled() );
        StyleConfigData::setLineEditTransitionsDuration( lineEditAnimations_->duration() );

        StyleConfigData::setComboBoxTransitionsEnabled( comboBoxAnimations_->enabled() );
        StyleConfigData::setComboBoxTransitionsDuration( comboBoxAnimations_->duration() );
        setChanged( false );

    }

    //_______________________________________________
    void AnimationConfigWidget::updateChanged( void )
    {

        bool modified( false );
        if( ui.animationsEnabled_->isChecked() != StyleConfigData::animationsEnabled() ) modified = true;
        else if( genericAnimations_->enabled() != StyleConfigData::genericAnimationsEnabled() ) modified = true;
        else if( genericAnimations_->duration() != StyleConfigData::genericAnimationsDuration() ) modified = true;

        else if( toolBarAnimations_->duration() != StyleConfigData::genericAnimationsDuration() ) modified = true;
        else if( toolBarAnimations_->followMouseDuration() != StyleConfigData::toolBarAnimationsDuration() ) modified = true;
        else if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_NONE && toolBarAnimations_->enabled() ) modified = true;
        else if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_FOLLOW_MOUSE && !( toolBarAnimations_->type() == 1 && toolBarAnimations_->enabled() ) ) modified = true;
        else if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_FADE && !( toolBarAnimations_->type() == 0 && toolBarAnimations_->enabled() )) modified = true;

        else if( menuBarAnimations_->duration() != StyleConfigData::menuBarAnimationsDuration() ) modified = true;
        else if( menuBarAnimations_->followMouseDuration() != StyleConfigData::menuBarFollowMouseAnimationsDuration() ) modified = true;
        else if( StyleConfigData::menuBarAnimationType() == StyleConfigData::MB_NONE && menuBarAnimations_->enabled() ) modified = true;
        else if( StyleConfigData::menuBarAnimationType() == StyleConfigData::MB_FOLLOW_MOUSE && !( menuBarAnimations_->type() == 1 && menuBarAnimations_->enabled() ) ) modified = true;
        else if( StyleConfigData::menuBarAnimationType() == StyleConfigData::MB_FADE && !( menuBarAnimations_->type() == 0 && menuBarAnimations_->enabled() ) ) modified = true;

        else if( menuAnimations_->duration() != StyleConfigData::menuAnimationsDuration() ) modified = true;
        else if( menuAnimations_->followMouseDuration() != StyleConfigData::menuFollowMouseAnimationsDuration() ) modified = true;
        else if( StyleConfigData::menuAnimationType() == StyleConfigData::ME_NONE && menuAnimations_->enabled() ) modified = true;
        else if( StyleConfigData::menuAnimationType() == StyleConfigData::ME_FOLLOW_MOUSE && !( menuAnimations_->type() == 1 && menuAnimations_->enabled() ) ) modified = true;
        else if( StyleConfigData::menuAnimationType() == StyleConfigData::ME_FADE && !( menuAnimations_->type() == 0 && menuAnimations_->enabled() ) ) modified = true;

        else if( progressBarAnimations_->enabled() != StyleConfigData::progressBarAnimationsEnabled() ) modified = true;
        else if( progressBarAnimations_->duration() != StyleConfigData::progressBarAnimationsDuration() ) modified = true;

        else if( progressBarBusyAnimations_->enabled() != StyleConfigData::progressBarAnimated() ) modified = true;
        else if( progressBarBusyAnimations_->duration() != StyleConfigData::progressBarBusyStepDuration() ) modified = true;

        else if( stackedWidgetAnimations_->enabled() != StyleConfigData::stackedWidgetTransitionsEnabled() ) modified = true;
        else if( stackedWidgetAnimations_->duration() != StyleConfigData::stackedWidgetTransitionsDuration() ) modified = true;

        else if( labelAnimations_->enabled() != StyleConfigData::labelTransitionsEnabled() ) modified = true;
        else if( labelAnimations_->duration() != StyleConfigData::labelTransitionsDuration() ) modified = true;

        else if( lineEditAnimations_->enabled() != StyleConfigData::lineEditTransitionsEnabled() ) modified = true;
        else if( lineEditAnimations_->duration() != StyleConfigData::lineEditTransitionsDuration() ) modified = true;

        else if( comboBoxAnimations_->enabled() != StyleConfigData::comboBoxTransitionsEnabled() ) modified = true;
        else if( comboBoxAnimations_->duration() != StyleConfigData::comboBoxTransitionsDuration() ) modified = true;

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
        ++row_;

        item->configurationWidget()->setVisible( false );
        connect( item->configurationButton(), SIGNAL( toggled( bool ) ), SIGNAL( layoutChanged( void ) ) );
        connect( item, SIGNAL( changed( void ) ), SLOT( updateChanged( void ) ) );
    }

}
