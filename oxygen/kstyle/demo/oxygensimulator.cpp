//////////////////////////////////////////////////////////////////////////////
// oxygensimulator.cpp
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

#include "oxygensimulator.h"
#include "oxygensimulator.moc"

#include <QtGui/QAbstractItemView>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QFocusEvent>
#include <QtGui/QHoverEvent>

#include <QtCore/QTextStream>

namespace Oxygen
{

    //__________________________________________
    Simulator::~Simulator( void )
    {
        // stop timer
        if( _timer.isActive() ) _timer.stop();

        // delete remaining pending events
        foreach( const Event& event, _events )
        { delete event.event(); }

        // and clear list
        _events.clear();
    }

    //__________________________________________
    void Simulator::toggleCheckBox( QCheckBox* checkbox )
    { postMouseClickEvent( checkbox ); }

    //__________________________________________
    void Simulator::toggleComboBox( QComboBox* combobox, int delay )
    {

        // click on tab orientation combobox
        postMouseClickEvent( combobox );
        postDelay( delay );
        postMouseClickEvent( combobox->view() );

    }

    //__________________________________________
    void Simulator::postEvent( QWidget* recipient, QEvent::Type type )
    { _events.push_back( Event( recipient, new QEvent( type ) ) ); }

    //__________________________________________
    void Simulator::postFocusEvent( QWidget* recipient, QEvent::Type type )
    { _events.push_back( Event( recipient, new QFocusEvent( type, Qt::MouseFocusReason ) ) ); }

    //__________________________________________
    void Simulator::postHoverEnterEvent( QWidget* recipient )
    {
        const QPoint oldPosition( recipient->rect().topLeft() - QPoint( 1, 0 ) );
        const QPoint newPosition( recipient->rect().center() );
        _events.push_back( Event( recipient, new QHoverEvent( QEvent::HoverEnter, oldPosition, newPosition ) ) );
    }

    //__________________________________________
    void Simulator::postHoverLeaveEvent( QWidget* recipient )
    {
        const QPoint oldPosition( recipient->rect().center() );
        const QPoint newPosition( recipient->rect().topLeft() - QPoint( 1, 0 ) );
        _events.push_back( Event( recipient, new QHoverEvent( QEvent::HoverMove, oldPosition, newPosition ) ) );
        _events.push_back( Event( recipient, new QHoverEvent( QEvent::HoverLeave, oldPosition, newPosition ) ) );
    }

    //__________________________________________
    void Simulator::postMouseEvent( QWidget* recipient, QEvent::Type type, Qt::MouseButton button , const QPoint& position )
    { _events.push_back( Event( recipient, new QMouseEvent( type, position, button, Qt::NoButton, Qt::NoModifier ) ) ); }

    //__________________________________________
    void Simulator::run( void )
    {

        // stop timer if already running
        if( _timer.isActive() ) _timer.stop();

        // first flush invalid events
        while( !( _events.isEmpty() || _events.front().isValid() ) )
        { _events.pop_front(); }

        // check events list size
        if( _events.isEmpty() ) return;

        // start timer using the delay from the first element
        _timer.start( _events.front().delay(), this );

        return;
    }

    //__________________________________________
    void Simulator::timerEvent( QTimerEvent* event )
    {
        if( event->timerId() != _timer.timerId() )
        { return QObject::timerEvent( event ); }

        // stop timer
        _timer.stop();

        // post current event to application
        //QTextStream( stdout ) << "Oxygen::Simulator::timerEvent - processing event " << &_events.front() << endl;
        if( _events.front().reciever() && _events.front().event() )
        { qApp->postEvent( _events.front().reciever().data(), _events.front().event() ); }

        // remove from list
        _events.pop_front();

        // process next event
        run();

    }

}
