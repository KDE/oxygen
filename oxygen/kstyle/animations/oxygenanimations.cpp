//////////////////////////////////////////////////////////////////////////////
// oxygenanimations.cpp
// container for all animation engines
// -------------------
//
// Copyright (c) 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
// Copyright (c) 2006, 2007 Casper Boemann <cbr@boemann.dk>
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenanimations.h"
#include "oxygenanimations.moc"
#include "oxygenstyleconfigdata.h"

namespace Oxygen
{

    //____________________________________________________________
    Animations::Animations( QObject* parent ):
        QObject( parent )
    {

        widgetEnabilityEngine_ = new WidgetStateEngine( this );
        spinBoxEngine_ = new SpinBoxEngine( this );
        comboBoxEngine_ = new WidgetStateEngine( this );
        toolButtonEngine_ = new WidgetStateEngine( this );

        registerEngine( dockSeparatorEngine_ = new DockSeparatorEngine( this ) );
        registerEngine( widgetStateEngine_ = new WidgetStateEngine( this ) );
        registerEngine( toolBarEngine_ = new WidgetStateEngine( this ) );
        registerEngine( lineEditEngine_ = new WidgetStateEngine( this ) );
        registerEngine( progressBarEngine_ = new ProgressBarEngine( this ) );
        registerEngine( menuBarEngine_ = new MenuBarEngineV1( this ) );
        registerEngine( menuEngine_ = new MenuEngineV1( this ) );
        registerEngine( scrollBarEngine_ = new ScrollBarEngine( this ) );
        registerEngine( sliderEngine_ = new SliderEngine( this ) );
        registerEngine( tabBarEngine_ = new TabBarEngine( this ) );
    }

    //____________________________________________________________
    void Animations::setupEngines( void )
    {

        // default enability, duration and maxFrame
        bool animationsEnabled( OxygenStyleConfigData::animationsEnabled() );

        // enability
        widgetEnabilityEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        widgetStateEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        comboBoxEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        toolButtonEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        lineEditEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        scrollBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        sliderEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        spinBoxEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        tabBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        dockSeparatorEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );

        progressBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::progressBarAnimationsEnabled() );
        progressBarEngine_->setBusyIndicatorEnabled( animationsEnabled &&  OxygenStyleConfigData::progressBarAnimated() );

        menuBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::menuBarAnimationsEnabled() );
        menuEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::menuAnimationsEnabled() );
        toolBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::toolBarAnimationsEnabled() );

        // duration
        widgetEnabilityEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        widgetStateEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        comboBoxEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        toolButtonEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        lineEditEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        scrollBarEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        sliderEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        spinBoxEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        tabBarEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        toolBarEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        dockSeparatorEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );

        progressBarEngine_->setDuration( OxygenStyleConfigData::progressBarAnimationsDuration() );
        progressBarEngine_->setBusyStepDuration( OxygenStyleConfigData::progressBarBusyStepDuration() );

        menuBarEngine_->setDuration( OxygenStyleConfigData::menuBarAnimationsDuration() );
        menuEngine_->setDuration( OxygenStyleConfigData::menuAnimationsDuration() );

    }

    //____________________________________________________________
    void Animations::registerWidget( QWidget* widget ) const
    {

        if( !widget ) return;

        // all widgets are registered to the enability engine.
        widgetEnabilityEngine().registerWidget( widget, AnimationEnable );

        // install animation timers
        // for optimization, one should put with most used widgets here first
        if( widget->inherits( "QToolButton" ) )
        {

            toolButtonEngine().registerWidget( widget, AnimationHover );
            if( widget->parent() && widget->parent()->inherits( "QToolBar" ) ) toolBarEngine().registerWidget( widget, AnimationHover );
            else widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        } else if( widget->inherits( "QAbstractButton" ) ) { widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }
        else if( widget->inherits( "QDial" ) ) { widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        // scrollbar
        else if( widget->inherits( "QScrollBar" ) ) { scrollBarEngine().registerWidget( widget ); }
        else if( widget->inherits( "QSlider" ) ) { sliderEngine().registerWidget( widget ); }
        else if( widget->inherits( "QProgressBar" ) ) { progressBarEngine().registerWidget( widget ); }
        else if( widget->inherits( "QSplitterHandle" ) ) { widgetStateEngine().registerWidget( widget, AnimationHover ); }
        else if( widget->inherits( "QMainWindow" ) ) { dockSeparatorEngine().registerWidget( widget ); }

        // menu
        else if( widget->inherits( "QMenu" ) ) { menuEngine().registerWidget( widget ); }
        else if( widget->inherits( "QMenuBar" ) ) { menuBarEngine().registerWidget( widget ); }
        else if( widget->inherits( "QTabBar" ) ) { tabBarEngine().registerWidget( widget ); }

        // editors
        else if( widget->inherits( "QComboBox" ) ) {
            comboBoxEngine().registerWidget( widget, AnimationHover );
            lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );
        } else if( widget->inherits( "QSpinBox" ) ) {
            spinBoxEngine().registerWidget( widget );
            lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );
        }
        else if( widget->inherits( "QLineEdit" ) ) { lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }
        else if( widget->inherits( "QTextEdit" ) ) { lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        // lists
        else if( widget->inherits( "QAbstractItemView" ) ) { lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        return;

    }

    //____________________________________________________________
    void Animations::unregisterWidget( QWidget* widget ) const
    {

        if( !widget ) return;

        /*
        these are the engines that have not been stored
        inside the list, because they can be register widgets in combination
        with other engines
        */
        widgetEnabilityEngine().unregisterWidget( widget );
        spinBoxEngine().unregisterWidget( widget );
        comboBoxEngine().unregisterWidget( widget );
        toolButtonEngine().unregisterWidget( widget );

        // the following allows some optimisation of widget unregistration
        // it assumes that a widget can be registered atmost in one of the
        // engines stored in the list.
        foreach( const BaseEngine::Pointer& engine, engines_ )
        { if( engine && engine.data()->unregisterWidget( widget ) ) break; }

    }

}
