//////////////////////////////////////////////////////////////////////////////
// oxygenbusyindicatorengine.cpp
// handle progress bar busy indicator
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

#include "oxygenbusyindicatorengine.h"
#include "oxygenbusyindicatorengine.moc"

#include <QProgressBar>
#include <QVariant>

namespace Oxygen
{

    //_______________________________________________
    const char* const BusyIndicatorEngine::busyValuePropertyName = "_kde_oxygen_busy_value";

    //_______________________________________________
    bool BusyIndicatorEngine::registerWidget( QWidget* widget )
    {

        // check widget validity
        if( !widget ) return false;

        // insert
        if( enabled() && !_dataSet.contains( widget ) )
        {
            widget->setProperty( busyValuePropertyName, 0 );
            _dataSet.insert( widget );

            // connect destruction signal
            connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );

            return true;

        } else return false;

    }


    //____________________________________________________________
    void BusyIndicatorEngine::setDuration( int value )
    {

        if( duration() == value ) return;
        BaseEngine::setDuration( value );

        // restart timer with specified time
        if( _timer.isActive() )
        {
            _timer.stop();
            _timer.start( value, this );
        }

    }

    //_______________________________________________
    void BusyIndicatorEngine::timerEvent( QTimerEvent* event )
    {

        // check enability and timer
        if( !( enabled() && event->timerId() == _timer.timerId() ) )
        { return BaseEngine::timerEvent( event ); }

        bool animated( false );

        // loop over objects in map
        for( ProgressBarSet::iterator iter = _dataSet.begin(); iter != _dataSet.end(); ++iter )
        {

            // cast to progressbar
            QProgressBar* progressBar( qobject_cast<QProgressBar*>( *iter ) );

            // check cast, visibility and range
            if( progressBar && progressBar->isVisible() && progressBar->minimum() == 0 && progressBar->maximum() == 0  )
            {

                // update animation flag
                animated = true;

                // update value
                progressBar->setProperty( busyValuePropertyName, progressBar->property( busyValuePropertyName ).toInt()+1 );
                progressBar->update();

            } else if( *iter ) { (*iter)->setProperty( busyValuePropertyName, 0 ); }

        }

        if( !animated ) _timer.stop();

    }

}
