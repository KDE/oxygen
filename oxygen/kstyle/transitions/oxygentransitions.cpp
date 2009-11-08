//////////////////////////////////////////////////////////////////////////////
// oxygentransitions.cpp
// container for all transition engines
// -------------------
//
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

#include "oxygentransitions.h"
#include "oxygentransitions.moc"
#include "oxygenstyleconfigdata.h"

namespace Oxygen
{

    //________________________________________________________--
    Transitions::Transitions( QObject* parent ):
        QObject( parent ),
        comboBoxEngine_( new ComboBoxEngine( this ) ),
        labelEngine_( new LabelEngine( this ) ),
        stackedWidgetEngine_( new StackedWidgetEngine( this ) )
    {}

    //________________________________________________________--
    void Transitions::setupEngines( void )
    {

        // default enability, duration and maxFrame
        bool animationsEnabled( OxygenStyleConfigData::animationsEnabled() );

        // enability
        comboBoxEngine().setEnabled( animationsEnabled && OxygenStyleConfigData::labelTransitionsEnabled() );
        labelEngine().setEnabled( animationsEnabled && OxygenStyleConfigData::labelTransitionsEnabled() );
        stackedWidgetEngine().setEnabled( animationsEnabled && OxygenStyleConfigData::stackedWidgetTransitionsEnabled() );

        // durations
        comboBoxEngine().setDuration( OxygenStyleConfigData::labelTransitionsDuration() );
        labelEngine().setDuration( OxygenStyleConfigData::labelTransitionsDuration() );
        stackedWidgetEngine().setDuration( OxygenStyleConfigData::stackedWidgetTransitionsDuration() );

    }

    //____________________________________________________________
    bool Transitions::registerWidget( QWidget* widget ) const
    {

        if( !widget ) return false;

        if( QLabel* label = qobject_cast<QLabel*>( widget ) ) {

            return labelEngine().registerWidget( label );

        } else if( QComboBox* comboBox = qobject_cast<QComboBox*>( widget ) ) {

            return comboBoxEngine().registerWidget( comboBox );

        } else if( QStackedWidget* stack = qobject_cast<QStackedWidget*>( widget ) ) {

            return stackedWidgetEngine().registerWidget( stack );

        }

        return false;

    }

}
