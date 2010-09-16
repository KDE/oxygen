#ifndef oxygensimulator_h
#define oxygensimulator_h

//////////////////////////////////////////////////////////////////////////////
// oxygensimulator.h
// simulates event chain passed to the application
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

#include <QtGui/QWidget>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>

#include <QtCore/QBasicTimer>
#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QTimerEvent>
#include <QtCore/QWeakPointer>

namespace Oxygen
{
    class Simulator: public QObject
    {
        Q_OBJECT

        public:

        //! constructor
        Simulator( QObject* parent ):
            QObject( parent )
            {}

        //! destructor
        virtual ~Simulator( void );

        //!@name high level interface
        //@{

        //! click on checkbox
        void toggleCheckBox( QCheckBox* );

        //! click on combobox and release
        void toggleComboBox( QComboBox*, int delay = 200 );

        //@}

        //!@name low level interface
        //@{

        //! mouse click event
        void postMouseClickEvent( QWidget* widget, Qt::MouseButton button = Qt::LeftButton )
        {
            postEvent( widget, QEvent::Enter );
            postHoverEnterEvent( widget );
            postMouseEvent( widget, QEvent::MouseMove, Qt::NoButton );
            postMouseEvent( widget, QEvent::MouseButtonPress, button  );
            postMouseEvent( widget, QEvent::MouseButtonRelease, button );
            postEvent( widget, QEvent::Leave );
            postHoverLeaveEvent( widget );

        }

        //! mouse event
        void postMouseEvent( QWidget* widget, QEvent::Type type, Qt::MouseButton button = Qt::LeftButton )
        { postMouseEvent( widget, type, button, widget->rect().center() ); }

        //! 'basic' event
        void postEvent( QWidget*, QEvent::Type );

        //! focus event
        void postFocusEvent( QWidget*, QEvent::Type );

        //! hover enter
        void postHoverEnterEvent( QWidget* );

        //! hover leave
        void postHoverLeaveEvent( QWidget* );

        //! mouse event
        void postMouseEvent( QWidget*, QEvent::Type, Qt::MouseButton , const QPoint& );

        //! delay
        inline void postDelay( int delay );

        //@}

        //! run stored events
        void run( void );

        protected:

        //! timer event
        void timerEvent( QTimerEvent* );

        private:

        //! event
        class Event
        {
            public:

            //! constructor
            Event( QWidget* reciever, QEvent* event, int delay = 100 ):
                _reciever( reciever ),
                _event( event ),
                _delay( delay )
            {}

            //! destructor
            virtual ~Event( void )
            {}

            //! validity
            virtual bool isValid( void ) const
            { return delay() > 0; }

            //! reciever
            typedef QWeakPointer<QWidget> WidgetPointer;
            virtual const WidgetPointer& reciever( void ) const
            { return _reciever; }

            //! event
            virtual QEvent* event( void ) const
            { return _event; }

            //! delay
            int delay( void ) const
            { return _delay; }

            private:

            //! reciever
            WidgetPointer _reciever;

            //! event
            QEvent* _event;

            //! delay
            int _delay;

        };

        //! timer
        QBasicTimer _timer;

        //! list of events
        typedef QList<Event> EventList;
        EventList _events;

    };

    //___________________________________________
    void Simulator::postDelay( int delay )
    { _events.push_back( Event( 0, 0, delay ) ); }

}

#endif
