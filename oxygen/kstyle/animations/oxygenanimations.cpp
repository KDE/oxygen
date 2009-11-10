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
        QObject( parent ),
        widgetEnabilityEngine_( new WidgetStateEngine( this ) ),
        widgetStateEngine_( new WidgetStateEngine( this ) ),
        toolBarEngine_( new WidgetStateEngine( this ) ),
        lineEditEngine_( new WidgetStateEngine( this ) ),
        menuBarEngine_( new MenuBarEngineV1( this ) ),
        menuEngine_( new MenuEngineV1( this ) ),
        scrollBarEngine_( new ScrollBarEngine( this ) ),
        sliderEngine_( new SliderEngine( this ) ),
        tabBarEngine_( new TabBarEngine( this ) )
    {}

    //____________________________________________________________
    void Animations::setupEngines( void )
    {

        // default enability, duration and maxFrame
        bool animationsEnabled( OxygenStyleConfigData::animationsEnabled() );

        // enability
        widgetEnabilityEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        widgetStateEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        lineEditEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        scrollBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        sliderEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );
        tabBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::genericAnimationsEnabled() );

        menuBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::menuBarAnimationsEnabled() );
        menuEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::menuAnimationsEnabled() );
        toolBarEngine_->setEnabled( animationsEnabled &&  OxygenStyleConfigData::toolBarAnimationsEnabled() );

        // duration
        widgetEnabilityEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        widgetStateEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        lineEditEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        scrollBarEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        sliderEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        tabBarEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );
        toolBarEngine_->setDuration( OxygenStyleConfigData::genericAnimationsDuration() );

        menuBarEngine_->setDuration( OxygenStyleConfigData::menuBarAnimationsDuration() );
        menuEngine_->setDuration( OxygenStyleConfigData::menuAnimationsDuration() );

    }

    //____________________________________________________________
    bool Animations::registerWidget( QWidget* widget ) const
    {

        if( !widget ) return false;

        widgetEnabilityEngine().registerWidget( widget, AnimationEnable );

        // install animation timers
        // for optimization, one should put with most used widgets here first
        if( widget->inherits( "QToolButton" ) )
        {

            if( widget->parent() && widget->parent()->inherits( "QToolBar" ) ) return toolBarEngine().registerWidget( widget, AnimationHover );
            else return widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        } else if( widget->inherits( "QAbstractButton" ) ) {

            return widgetStateEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        }

        // scrollbar
        else if( widget->inherits( "QScrollBar" ) ) {

            return scrollBarEngine().registerWidget( widget );

        } else if( widget->inherits( "QSlider" ) ) {

            return sliderEngine().registerWidget( widget );

        }

        // menu
        else if( widget->inherits( "QMenu" ) ) { return menuEngine().registerWidget( widget ); }
        else if( widget->inherits( "QMenuBar" ) ) { return menuBarEngine().registerWidget( widget ); }
        else if( widget->inherits( "QTabBar" ) ) { return tabBarEngine().registerWidget( widget ); }

        // editors
        else if( widget->inherits( "QComboBox" ) ) {

          return lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        } else if( widget->inherits( "QSpinBox" ) ) {

            return lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        } else if( widget->inherits( "QLineEdit" ) ) { return lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }
        else if( widget->inherits( "QTextEdit" ) ) { return lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        // lists
        else if( widget->inherits( "QAbstractItemView" ) ) { return lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus ); }

        return false;

    }

}
