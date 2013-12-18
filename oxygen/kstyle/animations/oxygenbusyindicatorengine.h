#ifndef oxygenbusyindicatorengine_h
#define oxygenbusyindicatorengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbusyindicatorengine.h
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

#include "oxygenbaseengine.h"

#include <QBasicTimer>
#include <QSet>
#include <QWidget>
#include <QTimerEvent>

namespace Oxygen
{

    //! handles progress bar animations
    class BusyIndicatorEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //! busy value property name
        static const char* const busyValuePropertyName;

        //! constructor
        explicit BusyIndicatorEngine( QObject* object ):
            BaseEngine( object )
        {}

        //! destructor
        virtual ~BusyIndicatorEngine( void )
        {}

        //! register menubar
        virtual bool registerWidget( QWidget* );

        //! start busy timer
        virtual void startBusyTimer( void )
        {
            if( !_timer.isActive() )
            { _timer.start( duration(), this ); }
        }

        //! duration
        virtual void setDuration( int );

        public Q_SLOTS:

        //! remove widget from map
        virtual bool unregisterWidget( QObject* object )
        {

            if( !object ) return false;
            ProgressBarSet::iterator iter( _dataSet.find( object ) );
            if( iter == _dataSet.end() ) return false;

            _dataSet.erase( iter );
            return true;

        }

        protected:

        //! timer event
        virtual void timerEvent( QTimerEvent* );

        //! store set of of progress bars
        typedef QSet<QObject*> ProgressBarSet;
        ProgressBarSet _dataSet;

        //! timer
        QBasicTimer _timer;

    };

}

#endif
