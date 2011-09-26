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
#include "oxygenpropertynames.h"
#include "oxygenstyleconfigdata.h"

#include <QtGui/QAbstractItemView>
#include <QtGui/QComboBox>
#include <QtGui/QDial>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QScrollBar>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitterHandle>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>
#include <QtGui/QToolButton>
#include <QtGui/QGroupBox>

#include <KDebug>

namespace Oxygen
{

    //____________________________________________________________
    Animations::Animations( QObject* parent ):
        QObject( parent )
    {

        _widgetEnabilityEngine = new WidgetStateEngine( this );
        _spinBoxEngine = new SpinBoxEngine( this );
        _comboBoxEngine = new WidgetStateEngine( this );
        _toolButtonEngine = new WidgetStateEngine( this );
        _toolBoxEngine = new ToolBoxEngine( this );

        registerEngine( _splitterEngine = new SplitterEngine( this ) );
        registerEngine( _dockSeparatorEngine = new DockSeparatorEngine( this ) );
        registerEngine( _headerViewEngine = new HeaderViewEngine( this ) );
        registerEngine( _widgetStateEngine = new WidgetStateEngine( this ) );
        registerEngine( _lineEditEngine = new WidgetStateEngine( this ) );
        registerEngine( _progressBarEngine = new ProgressBarEngine( this ) );
        registerEngine( _menuBarEngine = new MenuBarEngineV1( this ) );
        registerEngine( _menuEngine = new MenuEngineV1( this ) );
        registerEngine( _scrollBarEngine = new ScrollBarEngine( this ) );
        registerEngine( _sliderEngine = new SliderEngine( this ) );
        registerEngine( _tabBarEngine = new TabBarEngine( this ) );
        registerEngine( _toolBarEngine = new ToolBarEngine( this ) );
        registerEngine( _mdiWindowEngine = new MdiWindowEngine( this ) );
    }

    //____________________________________________________________
    void Animations::setupEngines( void )
    {

        // animation steps
        AnimationData::setSteps( StyleConfigData::animationSteps() );

        {
            // default enability, duration and maxFrame
            bool animationsEnabled( StyleConfigData::animationsEnabled() );

            // enability
            _widgetEnabilityEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _widgetStateEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _comboBoxEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _toolButtonEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _toolBoxEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _lineEditEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _splitterEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _scrollBarEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _sliderEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _spinBoxEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _tabBarEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _dockSeparatorEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _headerViewEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );
            _mdiWindowEngine->setEnabled( animationsEnabled &&  StyleConfigData::genericAnimationsEnabled() );

            _progressBarEngine->setEnabled( animationsEnabled &&  StyleConfigData::progressBarAnimationsEnabled() );
            _progressBarEngine->setBusyIndicatorEnabled( StyleConfigData::progressBarAnimated() );

            // menubar engine
            int menuBarAnimationType( StyleConfigData::menuBarAnimationType() );
            if( menuBarAnimationType == StyleConfigData::MB_FADE && !qobject_cast<MenuBarEngineV1*>( _menuBarEngine ) )
            {
                if( _menuBarEngine )
                {

                    MenuBarEngineV1* newEngine = new MenuBarEngineV1( this, _menuBarEngine );
                    registerEngine( newEngine );
                    _menuBarEngine->deleteLater();
                    _menuBarEngine = newEngine;

                } else registerEngine( _menuBarEngine = new MenuBarEngineV1( this ) );

            } else if( menuBarAnimationType == StyleConfigData::MB_FOLLOW_MOUSE && !qobject_cast<MenuBarEngineV2*>( _menuBarEngine ) ) {

                if( _menuBarEngine )
                {

                    MenuBarEngineV2* newEngine = new MenuBarEngineV2( this, _menuBarEngine );
                    registerEngine( newEngine );
                    _menuBarEngine->deleteLater();
                    _menuBarEngine = newEngine;

                } else registerEngine( _menuBarEngine = new MenuBarEngineV1( this ) );

            }

            // menu engine
            int menuAnimationType( StyleConfigData::menuAnimationType() );
            if( menuAnimationType == StyleConfigData::ME_FADE && !qobject_cast<MenuEngineV1*>( _menuEngine ) )
            {

                if( _menuEngine )
                {

                    MenuEngineV1* newEngine = new MenuEngineV1( this, _menuEngine );
                    registerEngine( newEngine );
                    _menuEngine->deleteLater();
                    _menuEngine = newEngine;

                } else registerEngine( _menuEngine = new MenuEngineV1( this ) );

            } else if( menuAnimationType == StyleConfigData::ME_FOLLOW_MOUSE && !qobject_cast<MenuEngineV2*>( _menuEngine ) ) {

                if( _menuEngine )
                {

                    MenuEngineV2* newEngine = new MenuEngineV2( this, _menuEngine );
                    registerEngine( newEngine );
                    _menuEngine->deleteLater();
                    _menuEngine = newEngine;

                } else registerEngine( _menuEngine = new MenuEngineV1( this ) );

            }

            _menuBarEngine->setEnabled( animationsEnabled && menuBarAnimationType != StyleConfigData::MB_NONE );
            _menuEngine->setEnabled( animationsEnabled && menuAnimationType != StyleConfigData::ME_NONE );

            // toolbar engine
            int toolBarAnimationType( StyleConfigData::toolBarAnimationType() );
            if( toolBarAnimationType == StyleConfigData::TB_NONE || toolBarAnimationType == StyleConfigData::TB_FOLLOW_MOUSE )
            {

                // disable toolbar engine
                _toolBarEngine->setEnabled( animationsEnabled && toolBarAnimationType == StyleConfigData::TB_FOLLOW_MOUSE );

                // unregister all toolbuttons that belong to a toolbar
                foreach( QWidget* widget, _widgetStateEngine->registeredWidgets( AnimationHover|AnimationFocus ) )
                {
                    if( qobject_cast<QToolButton*>( widget ) && qobject_cast<QToolBar*>( widget->parentWidget() ) )
                    { _widgetStateEngine->unregisterWidget( widget ); }
                }

            } else if( toolBarAnimationType == StyleConfigData::TB_FADE ) {

                // disable toolbar engine
                _toolBarEngine->setEnabled( false );

                // retrieve all registered toolbars
                BaseEngine::WidgetList widgets( _toolBarEngine->registeredWidgets() );
                foreach( QWidget* widget, widgets )
                {
                    // get all toolbuttons
                    foreach( QObject* child, widget->children() )
                    {
                        if( QToolButton* toolButton = qobject_cast<QToolButton*>( child ) )
                        { _widgetStateEngine->registerWidget( toolButton, AnimationHover ); }
                    }
                }

            }

        }


        {

            // durations
            _widgetEnabilityEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _widgetStateEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _comboBoxEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _toolButtonEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _toolBoxEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _lineEditEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _splitterEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _scrollBarEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _sliderEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _spinBoxEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _tabBarEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _dockSeparatorEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _headerViewEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _mdiWindowEngine->setDuration( StyleConfigData::genericAnimationsDuration() );

            _progressBarEngine->setDuration( StyleConfigData::progressBarAnimationsDuration() );
            _progressBarEngine->setBusyStepDuration( StyleConfigData::progressBarBusyStepDuration() );

            _toolBarEngine->setDuration( StyleConfigData::genericAnimationsDuration() );
            _toolBarEngine->setFollowMouseDuration( StyleConfigData::toolBarAnimationsDuration() );

            _menuBarEngine->setDuration( StyleConfigData::menuBarAnimationsDuration() );
            _menuBarEngine->setFollowMouseDuration( StyleConfigData::menuBarFollowMouseAnimationsDuration() );

            _menuEngine->setDuration( StyleConfigData::menuAnimationsDuration() );
            _menuEngine->setFollowMouseDuration( StyleConfigData::menuFollowMouseAnimationsDuration() );

        }

    }

    //____________________________________________________________
    void Animations::registerWidget( QWidget* widget ) const
    {

        if( !widget ) return;

        // check against noAnimations propery
        QVariant propertyValue( widget->property( PropertyNames::noAnimations ) );
        if( propertyValue.isValid() && propertyValue.toBool() ) return;

        // these are needed to not register animations for kwin widgets
        if( widget->objectName() == "decoration widget" ) return;
        if( widget->inherits( "KCommonDecorationButton" ) ) return;
        if( widget->inherits( "QShapedPixmapWidget" ) ) return;

        // all widgets are registered to the enability engine.
        widgetEnabilityEngine().registerWidget( widget, AnimationEnable );

        // install animation timers
        // for optimization, one should put with most used widgets here first
        if( qobject_cast<QToolButton*>(widget) )
        {

            toolButtonEngine().registerWidget( widget, AnimationHover );
            bool isInToolBar( qobject_cast<QToolBar*>(widget->parent()) );
            if( isInToolBar )
            {

                if( StyleConfigData::toolBarAnimationType() == StyleConfigData::TB_FADE )
                { widgetStateEngine().registerWidget( widget, AnimationHover ); }

            } else widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        } else if( qobject_cast<QAbstractButton*>(widget) ) {

            if( qobject_cast<QToolBox*>( widget->parent() ) )
            { toolBoxEngine().registerWidget( widget ); }

            widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        } else if( qobject_cast<QDial*>(widget) ) {

            widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        }

        // groupboxes
        else if( QGroupBox* groupBox = qobject_cast<QGroupBox*>( widget ) )
        {
            if( groupBox->isCheckable() )
            { widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }
        }

        // scrollbar
        else if( qobject_cast<QScrollBar*>( widget ) ) { scrollBarEngine().registerWidget( widget ); }
        else if( qobject_cast<QSlider*>( widget ) ) { sliderEngine().registerWidget( widget ); }
        else if( qobject_cast<QProgressBar*>( widget ) ) { progressBarEngine().registerWidget( widget ); }
        else if( qobject_cast<QSplitterHandle*>( widget ) ) { splitterEngine().registerWidget( widget ); }
        else if( qobject_cast<QMainWindow*>( widget ) ) { dockSeparatorEngine().registerWidget( widget ); }
        else if( qobject_cast<QHeaderView*>( widget ) ) { headerViewEngine().registerWidget( widget ); }
        // menu
        else if( qobject_cast<QMenu*>( widget ) ) { menuEngine().registerWidget( widget ); }
        else if( qobject_cast<QMenuBar*>( widget ) ) { menuBarEngine().registerWidget( widget ); }
        else if( qobject_cast<QTabBar*>( widget ) ) { tabBarEngine().registerWidget( widget ); }
        else if( qobject_cast<QToolBar*>( widget ) ) { toolBarEngine().registerWidget( widget ); }

        // editors
        else if( qobject_cast<QComboBox*>( widget ) ) {
            comboBoxEngine().registerWidget( widget, AnimationHover );
            lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );
        } else if( qobject_cast<QSpinBox*>( widget ) ) {
            spinBoxEngine().registerWidget( widget );
            lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );
        }
        else if( qobject_cast<QLineEdit*>( widget ) ) { lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }
        else if( qobject_cast<QTextEdit*>( widget ) ) { lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        // lists
        else if( qobject_cast<QAbstractItemView*>( widget ) || widget->inherits("Q3ListView") )
        { lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        // mdi subwindows
        else if( qobject_cast<QMdiSubWindow*>( widget ) )
        { mdiWindowEngine().registerWidget( widget ); }

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
        toolBoxEngine().unregisterWidget( widget );

        // the following allows some optimization of widget unregistration
        // it assumes that a widget can be registered atmost in one of the
        // engines stored in the list.
        foreach( const BaseEngine::Pointer& engine, _engines )
        { if( engine && engine.data()->unregisterWidget( widget ) ) break; }

    }

    //_______________________________________________________________
    void Animations::unregisterEngine( QObject* object )
    {
        int index( _engines.indexOf( qobject_cast<BaseEngine*>(object) ) );
        if( index >= 0 ) _engines.removeAt( index );
    }

    //_______________________________________________________________
    void Animations::registerEngine( BaseEngine* engine )
    {
        _engines.push_back( engine );
        connect( engine, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterEngine(QObject*)) );
    }

}
