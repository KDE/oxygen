#ifndef oxygenprogressbarengine_h
#define oxygenprogressbarengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenprogressbarengine.h
// handle progress bar animations
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
#include "oxygenprogressbardata.h"
#include "oxygendatamap.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QSet>
#include <QtCore/QObject>
#include <QtCore/QTimerEvent>
#include <QtGui/QWidget>
#include <QtGui/QProgressBar>

namespace Oxygen
{

    //! handles progress bar animations
    class ProgressBarEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //! constructor
        ProgressBarEngine( QObject* object ):
        BaseEngine( object ),
        _busyIndicatorEnabled( true ),
        _busyStepDuration( 50 )
        {}

        //! destructor
        virtual ~ProgressBarEngine( void )
        {}

        //! register menubar
        virtual bool registerWidget( QWidget* );

        //! true if widget is animated
        virtual bool isAnimated( const QObject* object );

        //! animation opacity
        virtual int value( const QObject* object )
        { return isAnimated( object ) ? data( object ).data()->value():0 ; }

        //! enability
        virtual void setEnabled( bool value )
        {
            BaseEngine::setEnabled( value );
            _data.setEnabled( value );
        }

        //! duration
        virtual void setDuration( int value )
        {
            BaseEngine::setDuration( value );
            _data.setDuration( value );
        }

        //! busy indicator enability
        virtual void setBusyIndicatorEnabled( bool value )
        { _busyIndicatorEnabled = value; }

        virtual bool busyIndicatorEnabled( void ) const
        { return _busyIndicatorEnabled; }

        //! busy indicator step duration
        virtual void setBusyStepDuration( int value );

        //! busy indicator step duration
        virtual int busyStepDuration( void ) const
        { return _busyStepDuration; }

        //! start busy timer
        virtual void startBusyTimer( void )
        {
            if( !_timer.isActive() )
            { _timer.start( busyStepDuration(), this ); }
        }

        public slots:

        //! remove widget from map
        virtual bool unregisterWidget( QObject* object )
        {
            if( !object ) return false;
            _dataSet.remove( object );
            return _data.unregisterWidget( object );
        }

        protected:

        //! timer event
        virtual void timerEvent( QTimerEvent* );

        //! returns data associated to widget
        DataMap<ProgressBarData>::Value data( const QObject* );

        private:

        //! map widgets to progressbar data
        DataMap<ProgressBarData> _data;

        //! store set of of progress bars
        typedef QSet<QObject*> ProgressBarSet;
        ProgressBarSet _dataSet;

        //! busy indicator enabled
        bool _busyIndicatorEnabled;

        //! busy indicator step duration
        int _busyStepDuration;

        //! timer
        QBasicTimer _timer;

    };

}

#endif
