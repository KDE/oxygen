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
#include "oxygenfollowmouseanimationconfigitem.h"
#include "oxygengenericanimationconfigitem.h"
#include "oxygenstyleconfigdata.h"

#include <QtGui/QButtonGroup>
#include <QtGui/QHoverEvent>
#include <QtCore/QTextStream>
#include <KLocale>

namespace Oxygen
{

    //_______________________________________________
    AnimationConfigWidget::AnimationConfigWidget( QWidget* parent ):
        BaseAnimationConfigWidget( parent )
    {

        QGridLayout* layout( qobject_cast<QGridLayout*>( BaseAnimationConfigWidget::layout() ) );

        setupItem( layout, _genericAnimations = new GenericAnimationConfigItem( this,
            i18n("Focus, mouseover and widget state transition"),
            i18n("Configure widgets' focus and mouseover highlight animation, as well as widget enabled/disabled state transition") ) );

        setupItem( layout, _toolBarAnimations = new FollowMouseAnimationConfigItem( this,
            i18n("Toolbar highlight" ),
            i18n("Configure toolbars' mouseover highlight animation" ) ) );
        _toolBarAnimations->hideDurationSpinBox();

        setupItem( layout, _menuBarAnimations = new FollowMouseAnimationConfigItem( this,
            i18n("Menu bar highlight" ),
            i18n("Configure menu bars' mouseover highlight animation" ) ) );

        setupItem( layout, _menuAnimations = new FollowMouseAnimationConfigItem( this,
            i18n("Menu highlight" ),
            i18n("Configure menus' mouseover highlight animation" ) ) );

        setupItem( layout, _progressBarAnimations = new GenericAnimationConfigItem( this,
            i18n( "Progress bar animation" ),
            i18n( "Configure progress bars' steps animation" ) ) );

        setupItem( layout, _stackedWidgetAnimations = new GenericAnimationConfigItem( this,
            i18n( "Tab transitions" ), i18n( "Configure fading transition between tabs" ) ) );

        setupItem( layout, _labelAnimations = new GenericAnimationConfigItem( this,
            i18n( "Label transitions" ), i18n( "Configure fading transition when a label's text is changed" ) ) );

        setupItem( layout, _lineEditAnimations = new GenericAnimationConfigItem( this,
            i18n( "Text editor transitions" ), i18n( "Configure fading transition when an editor's text is changed" ) ) );

        setupItem( layout, _comboBoxAnimations = new GenericAnimationConfigItem( this,
            i18n( "Combo box transitions" ), i18n( "Configure fading transition when a combo box's selected choice is changed" ) ) );

        // add a separator
        QFrame* frame = new QFrame( this );
        frame->setFrameStyle( QFrame::HLine|QFrame::Sunken );
        layout->addWidget( frame, _row, 0, 1, 2 );
        ++_row;

        // progress bar busy animation
        setupItem( layout, _progressBarBusyAnimations = new GenericAnimationConfigItem( this,
            i18n( "Busy indicator steps" ),
            i18n( "Configure progress bars' busy indicator animation" ) ) );

        // add spacers to the first column, previous row to finalize layout
        layout->addItem( new QSpacerItem( 25, 0 ), _row-1, 0, 1, 1 );

        // add vertical spacer
        layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ), _row, 1, 1, 1 );
        ++_row;

        connect( animationsEnabled(), SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        foreach( AnimationConfigItem* item, findChildren<AnimationConfigItem*>() )
        {
            if( item != _progressBarBusyAnimations )
            {
                item->QWidget::setEnabled( false );
                connect( animationsEnabled(), SIGNAL(toggled(bool)), item, SLOT(setEnabled(bool)) );
            }
        }

    }

    //_______________________________________________
    AnimationConfigWidget::~AnimationConfigWidget( void )
    {}

    //_______________________________________________
    void AnimationConfigWidget::load( void )
    {

        animationsEnabled()->setChecked( StyleConfigData::animationsEnabled() );
        _genericAnimations->setEnabled( StyleConfigData::genericAnimationsEnabled() );
        _genericAnimations->setDuration( StyleConfigData::genericAnimationsDuration() );

        _toolBarAnimations->setEnabled( StyleConfigData::toolBarAnimationType() != StyleConfigData::TB_NONE );
        _toolBarAnimations->setDuration( StyleConfigData::genericAnimationsDuration() );
        _toolBarAnimations->setFollowMouseDuration( StyleConfigData::toolBarAnimationsDuration() );

        switch( StyleConfigData::toolBarAnimationType() )
        {
            case StyleConfigData::TB_FOLLOW_MOUSE:
            _toolBarAnimations->setType( 1 );
            break;

            case StyleConfigData::TB_FADE:
            default:
            _toolBarAnimations->setType( 0 );
            break;
        }

        _menuBarAnimations->setEnabled( StyleConfigData::menuBarAnimationType() != StyleConfigData::MB_NONE );
        _menuBarAnimations->setDuration( StyleConfigData::menuBarAnimationsDuration() );
        _menuBarAnimations->setFollowMouseDuration( StyleConfigData::menuBarFollowMouseAnimationsDuration() );
        switch( StyleConfigData::menuBarAnimationType() )
        {
            case StyleConfigData::MB_FOLLOW_MOUSE:
            _menuBarAnimations->setType( 1 );
            break;

            case StyleConfigData::MB_FADE:
            default:
            _menuBarAnimations->setType( 0 );
            break;
        }

        _menuAnimations->setEnabled( StyleConfigData::menuAnimationType() != StyleConfigData::ME_NONE );
        _menuAnimations->setDuration( StyleConfigData::menuAnimationsDuration() );
        _menuAnimations->setFollowMouseDuration( StyleConfigData::menuFollowMouseAnimationsDuration() );
        switch( StyleConfigData::menuAnimationType() )
        {
            case StyleConfigData::ME_FOLLOW_MOUSE:
            _menuAnimations->setType( 1 );
            break;

            case StyleConfigData::ME_FADE:
            default:
            _menuAnimations->setType( 0 );
            break;
        }

        _progressBarAnimations->setEnabled( StyleConfigData::progressBarAnimationsEnabled() );
        _progressBarAnimations->setDuration( StyleConfigData::progressBarAnimationsDuration() );

        _progressBarBusyAnimations->setEnabled( StyleConfigData::progressBarAnimated() );
        _progressBarBusyAnimations->setDuration( StyleConfigData::progressBarBusyStepDuration() );

        _stackedWidgetAnimations->setEnabled( StyleConfigData::stackedWidgetTransitionsEnabled() );
        _stackedWidgetAnimations->setDuration( StyleConfigData::stackedWidgetTransitionsDuration() );

        _labelAnimations->setEnabled( StyleConfigData::labelTransitionsEnabled() );
        _labelAnimations->setDuration( StyleConfigData::labelTransitionsDuration() );

        _lineEditAnimations->setEnabled( StyleConfigData::lineEditTransitionsEnabled() );
        _lineEditAnimations->setDuration( StyleConfigData::lineEditTransitionsDuration() );

        _comboBoxAnimations->setEnabled( StyleConfigData::comboBoxTransitionsEnabled() );
        _comboBoxAnimations->setDuration( StyleConfigData::comboBoxTransitionsDuration() );
    }

    //_______________________________________________
    void AnimationConfigWidget::save( void )
    {

        StyleConfigData::setAnimationsEnabled( animationsEnabled()->isChecked() );
        StyleConfigData::setGenericAnimationsEnabled( _genericAnimations->enabled() );
        StyleConfigData::setGenericAnimationsDuration( _genericAnimations->duration() );

        StyleConfigData::setToolBarAnimationsDuration( _toolBarAnimations->followMouseDuration() );
        if( !_toolBarAnimations->enabled() ) StyleConfigData::setToolBarAnimationType( StyleConfigData::TB_NONE );
        else if( _toolBarAnimations->type() == 1 ) StyleConfigData::setToolBarAnimationType( StyleConfigData::TB_FOLLOW_MOUSE );
        else StyleConfigData::setToolBarAnimationType( StyleConfigData::TB_FADE );

        StyleConfigData::setMenuBarAnimationsDuration( _menuBarAnimations->duration() );
        StyleConfigData::setMenuBarFollowMouseAnimationsDuration( _menuBarAnimations->followMouseDuration() );
        if( !_menuBarAnimations->enabled() ) StyleConfigData::setMenuBarAnimationType( StyleConfigData::MB_NONE );
        else if( _menuBarAnimations->type() == 1 ) StyleConfigData::setMenuBarAnimationType( StyleConfigData::MB_FOLLOW_MOUSE );
        else StyleConfigData::setMenuBarAnimationType( StyleConfigData::MB_FADE );

        StyleConfigData::setMenuAnimationsDuration( _menuAnimations->duration() );
        StyleConfigData::setMenuFollowMouseAnimationsDuration( _menuAnimations->followMouseDuration() );
        if( !_menuAnimations->enabled() ) StyleConfigData::setMenuAnimationType( StyleConfigData::ME_NONE );
        else if( _menuAnimations->type() == 1 ) StyleConfigData::setMenuAnimationType( StyleConfigData::ME_FOLLOW_MOUSE );
        else StyleConfigData::setMenuAnimationType( StyleConfigData::ME_FADE );

        StyleConfigData::setProgressBarAnimationsEnabled( _progressBarAnimations->enabled() );
        StyleConfigData::setProgressBarAnimationsDuration( _progressBarAnimations->duration() );

        StyleConfigData::setProgressBarAnimated( _progressBarBusyAnimations->enabled() );
        StyleConfigData::setProgressBarBusyStepDuration( _progressBarBusyAnimations->duration() );

        StyleConfigData::setStackedWidgetTransitionsEnabled( _stackedWidgetAnimations->enabled() );
        StyleConfigData::setStackedWidgetTransitionsDuration( _stackedWidgetAnimations->duration() );

        StyleConfigData::setLabelTransitionsEnabled( _labelAnimations->enabled() );
        StyleConfigData::setLabelTransitionsDuration( _labelAnimations->duration() );

        StyleConfigData::setLineEditTransitionsEnabled( _lineEditAnimations->enabled() );
        StyleConfigData::setLineEditTransitionsDuration( _lineEditAnimations->duration() );

        StyleConfigData::setComboBoxTransitionsEnabled( _comboBoxAnimations->enabled() );
        StyleConfigData::setComboBoxTransitionsDuration( _comboBoxAnimations->duration() );
        setChanged( false );

    }

    //_______________________________________________
    void AnimationConfigWidget::updateChanged( void )
    {

        bool modified( false );
        if( animationsEnabled()->isChecked() != StyleConfigData::animationsEnabled() ) modified = true;
        else if( _genericAnimations->enabled() != StyleConfigData::genericAnimationsEnabled() ) modified = true;
        else if( _genericAnimations->duration() != StyleConfigData::genericAnimationsDuration() ) modified = true;

        else if( _toolBarAnimations->duration() != StyleConfigData::genericAnimationsDuration() ) modified = true;
        else if( _toolBarAnimations->followMouseDuration() != StyleConfigData::toolBarAnimationsDuration() ) modified = true;
        else if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_NONE && _toolBarAnimations->enabled() ) modified = true;
        else if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_FOLLOW_MOUSE && !( _toolBarAnimations->type() == 1 && _toolBarAnimations->enabled() ) ) modified = true;
        else if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_FADE && !( _toolBarAnimations->type() == 0 && _toolBarAnimations->enabled() )) modified = true;

        else if( _menuBarAnimations->duration() != StyleConfigData::menuBarAnimationsDuration() ) modified = true;
        else if( _menuBarAnimations->followMouseDuration() != StyleConfigData::menuBarFollowMouseAnimationsDuration() ) modified = true;
        else if( StyleConfigData::menuBarAnimationType() == StyleConfigData::MB_NONE && _menuBarAnimations->enabled() ) modified = true;
        else if( StyleConfigData::menuBarAnimationType() == StyleConfigData::MB_FOLLOW_MOUSE && !( _menuBarAnimations->type() == 1 && _menuBarAnimations->enabled() ) ) modified = true;
        else if( StyleConfigData::menuBarAnimationType() == StyleConfigData::MB_FADE && !( _menuBarAnimations->type() == 0 && _menuBarAnimations->enabled() ) ) modified = true;

        else if( _menuAnimations->duration() != StyleConfigData::menuAnimationsDuration() ) modified = true;
        else if( _menuAnimations->followMouseDuration() != StyleConfigData::menuFollowMouseAnimationsDuration() ) modified = true;
        else if( StyleConfigData::menuAnimationType() == StyleConfigData::ME_NONE && _menuAnimations->enabled() ) modified = true;
        else if( StyleConfigData::menuAnimationType() == StyleConfigData::ME_FOLLOW_MOUSE && !( _menuAnimations->type() == 1 && _menuAnimations->enabled() ) ) modified = true;
        else if( StyleConfigData::menuAnimationType() == StyleConfigData::ME_FADE && !( _menuAnimations->type() == 0 && _menuAnimations->enabled() ) ) modified = true;

        else if( _progressBarAnimations->enabled() != StyleConfigData::progressBarAnimationsEnabled() ) modified = true;
        else if( _progressBarAnimations->duration() != StyleConfigData::progressBarAnimationsDuration() ) modified = true;

        else if( _progressBarBusyAnimations->enabled() != StyleConfigData::progressBarAnimated() ) modified = true;
        else if( _progressBarBusyAnimations->duration() != StyleConfigData::progressBarBusyStepDuration() ) modified = true;

        else if( _stackedWidgetAnimations->enabled() != StyleConfigData::stackedWidgetTransitionsEnabled() ) modified = true;
        else if( _stackedWidgetAnimations->duration() != StyleConfigData::stackedWidgetTransitionsDuration() ) modified = true;

        else if( _labelAnimations->enabled() != StyleConfigData::labelTransitionsEnabled() ) modified = true;
        else if( _labelAnimations->duration() != StyleConfigData::labelTransitionsDuration() ) modified = true;

        else if( _lineEditAnimations->enabled() != StyleConfigData::lineEditTransitionsEnabled() ) modified = true;
        else if( _lineEditAnimations->duration() != StyleConfigData::lineEditTransitionsDuration() ) modified = true;

        else if( _comboBoxAnimations->enabled() != StyleConfigData::comboBoxTransitionsEnabled() ) modified = true;
        else if( _comboBoxAnimations->duration() != StyleConfigData::comboBoxTransitionsDuration() ) modified = true;

        setChanged( modified );

    }

}
