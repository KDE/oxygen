#ifndef oxygensimulator_h
#define oxygensimulator_h

//////////////////////////////////////////////////////////////////////////////
// oxygensimulator.h
// simulates event chain passed to the application
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include <QtGui/QAbstractButton>
#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

#include <QtCore/QBasicTimer>
#include <QtCore/QEvent>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QTimerEvent>
#include <QtCore/QWeakPointer>

#include <KLocale>

namespace Oxygen
{
    class Simulator: public QObject
    {
        Q_OBJECT

        public:

        //! constructor
        explicit Simulator( QObject* parent ):
            QObject( parent ),
            _previousPosition( -1, -1 ),
            _aborted( false )
            {}

        //! destructor
        virtual ~Simulator( void );

        //!@name high level interface
        //@{

        //! click on button
        void click( QWidget* receiver, int delay = -1 );

        //! click on button
        void click( QWidget*, const QPoint&, int = -1 );

        //! slide
        void slide( QWidget* receiver, const QPoint& delta, int delay = -1 );

        //! select item
        void selectItem( QWidget*, int row, int column = 0, int = -1 );

        //! select combobox item
        void selectComboBoxItem( QWidget*, int, int = -1 );

        //! select menu item
        void selectMenuItem( QWidget*, int, int = -1 );

        //! select tab in tabwidget
        void selectTab( QTabWidget*, int, int = -1 );

        //! select tab in tabbar
        void selectTab( QTabBar*, int, int = -1 );

        //! write sample text
        void writeSampleText( QWidget* widget, int delay = -1 )
        { writeText( widget, i18n( "This is a sample text" ), delay ); }

        //! write string
        void writeText( QWidget*, QString, int = -1 );

        //! clear text
        void clearText( QWidget*, int = -1 );

        //! delay
        void wait( int delay );

        //@}

        //! true if aborted
        bool aborted( void ) const
        { return _aborted; }

        //! run stored events
        void run( void );

        //! gab mouse
        static bool grabMouse( void )
        { return _grabMouse; }

        //! mouse grab
        static void setGrabMouse( bool value )
        { _grabMouse = value; }

        //! default delay
        static void setDefaultDelay( int value )
        { _defaultDelay = value; }

        signals:

        //! emitted when simulator starts and stops
        void stateChanged( bool );

        public slots:

        //! abort simulations
        void abort( void );

        protected:

        //! timer event
        void timerEvent( QTimerEvent* );

        //!@name low level interface
        //@{

        //! enter widget
        bool enter( QWidget* receiver, int delay = -1 )
        { return enter( receiver, receiver->rect().center(), delay ); }

        //! enter receiver
        bool enter( QWidget*, const QPoint&, int = -1 );

        //! mouse click event
        void postMouseClickEvent( QWidget* widget )
        { postMouseClickEvent( widget, Qt::LeftButton, widget->rect().center() ); }

        //! mouse click event
        void postMouseClickEvent( QWidget*, Qt::MouseButton, const QPoint& );

        //! 'basic' event
        void postEvent( QWidget*, QEvent::Type ) const;

        //! hover
        void postHoverEvent( QWidget*, QEvent::Type, const QPoint&, const QPoint& ) const;

        //! mouse event
        void postMouseEvent( QWidget*, QEvent::Type, Qt::MouseButton , const QPoint&, Qt::MouseButtons = Qt::NoButton, Qt::KeyboardModifiers = Qt::NoModifier ) const;

        //! key event
        void postKeyClickEvent( QWidget*, Qt::Key, QString, Qt::KeyboardModifiers = Qt::NoModifier ) const;

        //! key event
        void postKeyModifiersEvent( QWidget*, QEvent::Type, Qt::KeyboardModifiers ) const;

        //! key event
        void postKeyEvent( QWidget*, QEvent::Type, Qt::Key, QString, Qt::KeyboardModifiers = Qt::NoModifier ) const;

        //! delay
        void postDelay( int );

        //! set focus to widget
        void setFocus( QWidget* );

        //! move cursor
        void moveCursor( const QPoint&, int steps = 10 );
        //@}

        private:

        typedef QWeakPointer<QWidget> WidgetPointer;

        //! event
        class Event
        {
            public:

            enum Type
            {
                Wait,
                Click,
                Slide,
                SelectItem,
                SelectComboBoxItem,
                SelectMenuItem,
                SelectTab,
                WriteText,
                ClearText
            };

            //! constructor
            Event( Type type, QWidget* receiver, int delay = 0 ):
                _type( type ),
                _receiver( receiver ),
                _delay( delay )
            {}

            //! destructor
            virtual ~Event( void )
            {}

            Type _type;
            WidgetPointer _receiver;
            QPoint _position;
            QString _text;
            int _delay;

        };

        //! process event
        void processEvent( const Event& );

        //! process Qt event
        void postQEvent( QWidget*, QEvent* ) const;

        //! convert QChar to key
        Qt::Key toKey( QChar ) const;

        //! list of events
        typedef QList<Event> EventList;
        EventList _events;

        //! previous position in global coordinates
        /*! this is needed to have proper handling of enter/leave/hover events */
        QPoint _previousPosition;

        //! previous widget
        WidgetPointer _previousWidget;

        //! basic timer, for wait
        QBasicTimer _timer;

        //! pending events timer
        QBasicTimer _pendingEventsTimer;

        //! pending event
        WidgetPointer _pendingWidget;
        QList<QEvent*> _pendingEvents;

        //! true when simulations must be aborted
        bool _aborted;

        //! true if simulations also grab mouse
        static bool _grabMouse;

        //! default delay
        static int _defaultDelay;

    };

}

#endif
