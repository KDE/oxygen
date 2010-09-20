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
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFocusEvent>
#include <QtGui/QHoverEvent>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSlider>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionComboBox>
#include <QtGui/QStyleOptionSlider>
#include <QtGui/QToolButton>

#include <QtCore/QTimer>

#include <ctime>

extern Qt::Key asciiToKey(char ascii);

namespace Oxygen
{

    //_______________________________________________________________________
    int Simulator::_defaultDelay = 250;

    //_______________________________________________________________________
    Simulator::~Simulator( void )
    {}

    //_______________________________________________________________________
    void Simulator::wait( int delay )
    { _events.push_back( Event( Event::Wait, 0, delay ) ); }

    //_______________________________________________________________________
    void Simulator::enter( QWidget* reciever, const QPoint& position, int delay  )
    {
        Event event( Event::Enter, reciever, delay );
        event._position = position;
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::click( QWidget* reciever, int delay  )
    {

        QPoint position;
        if( QCheckBox* checkbox = qobject_cast<QCheckBox*>( reciever ) )
        {

            QStyleOptionButton option;
            option.initFrom( checkbox );
            position = checkbox->style()->subElementRect(
                QStyle::SE_CheckBoxClickRect,
                &option,
                checkbox).center();

        } else if( QRadioButton* radiobutton = qobject_cast<QRadioButton*>( reciever ) ) {

            QStyleOptionButton option;
            option.initFrom( radiobutton );
            position = radiobutton->style()->subElementRect(
                QStyle::SE_RadioButtonClickRect,
                &option,
                radiobutton).center();

        } else {

            position = reciever->rect().center();

        }

        click( reciever, position, delay );

    }

    //_______________________________________________________________________
    void Simulator::click( QWidget* reciever, const QPoint& position, int delay )
    {
        Event event( Event::Click, reciever, delay );
        event._position = position;
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::slide( QWidget* reciever, const QPoint& position, int delay )
    {
        Event event( Event::Slide, reciever, delay );
        event._position = position;
        _events.push_back( event );

    }

    //_______________________________________________________________________
    void Simulator::selectItem( QWidget* reciever, int row, int column, int delay )
    {
        Event event( Event::SelectItem, reciever, delay );
        event._position = QPoint( column, row );
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::clearSelection( QWidget* reciever, int delay )
    { _events.push_back( Event( Event::ClearSelection, reciever, delay ) ); }

    //_______________________________________________________________________
    void Simulator::selectComboBoxItem( QWidget* reciever, int index, int delay )
    {
        Event event( Event::SelectComboBoxItem, reciever, delay );
        event._position.setX( index );
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::selectMenuItem( QWidget* reciever, int index, int delay )
    {
        Event event( Event::SelectMenuItem, reciever, delay );
        event._position.setX( index );
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::selectTab( QTabWidget* tabwidget, int index, int delay )
    {
        foreach( QObject* child, tabwidget->children() )
        {
            if( QTabBar* tabbar = qobject_cast<QTabBar*>( child ) )
            {
                selectTab( tabbar, index, delay );
                break;
            }
        }
    }

    //_______________________________________________________________________
    void Simulator::selectTab( QTabBar* reciever, int index, int delay )
    {
        Event event( Event::SelectTab, reciever, delay );
        event._position.setX( index );
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::writeText( QWidget* reciever, QString text, int delay )
    {
        Event event( Event::WriteText, reciever, delay );
        event._text = text;
        _events.push_back( event );
    }

    //_______________________________________________________________________
    void Simulator::clearText( QWidget* reciever, int delay )
    { _events.push_back( Event( Event::ClearText, reciever, delay ) ); }

    //_______________________________________________________________________
    void Simulator::run( void )
    {

        if( _events.isEmpty() ) return;

        emit stateChanged( true );
        foreach( const Event& event, _events )
        { processEvent( event ); }

        // add last event to reset previousWidget and previousPosition
        if( _previousWidget )
        {

            postEvent( _previousWidget.data(), QEvent::Leave );
            if( _previousWidget.data()->testAttribute( Qt::WA_Hover ) )
            {
                const QPoint oldPosition( _previousWidget.data()->mapFromGlobal( _previousPosition ) );
                const QPoint newPosition( _previousWidget.data()->mapFromGlobal( QPoint( -1, -1 ) ) );
                postHoverEvent( _previousWidget.data(), QEvent::HoverLeave, newPosition, oldPosition );
            }

            _previousWidget.clear();
            _previousPosition = QPoint(-1, -1 );

        }

        _events.clear();
        emit stateChanged( false );

        return;
    }

    //_______________________________________________________________________
    void Simulator::timerEvent( QTimerEvent* event )
    {
        if( event->timerId() == _timer.timerId() )
        {

            _timer.stop();

        } else if( event->timerId() == _pendingEventsTimer.timerId() ) {

            _pendingEventsTimer.stop();
            if( _pendingWidget )
            {
                foreach( QEvent* event, _pendingEvents )
                {
                    postQEvent( _pendingWidget.data(), event );
                    postDelay( 150 );
                }
            }

            _pendingEvents.clear();
            _pendingWidget.clear();

        } else return QObject::timerEvent( event );

    }

    //_______________________________________________________________________
    void Simulator::processEvent( const Event& event )
    {

        if( !event._reciever )
        {

            if( event._type == Event::Wait )
            {
                if( event._delay > 0 )
                {

                    postDelay( event._delay );

                } else if( event._delay == -1 && _defaultDelay > 0 ) {

                    postDelay( event._delay );

                }

            }

            return;

        }

        QWidget* reciever( event._reciever.data() );

        switch( event._type )
        {

            // Enter event
            case Event::Enter:
            {

                // store position
                const QPoint& position( event._position );

                // leave previous widget
                if( _previousWidget && _previousWidget.data() != reciever )
                {
                    postEvent( _previousWidget.data(), QEvent::Leave );
                    if( _previousWidget.data()->testAttribute( Qt::WA_Hover ) )
                    {
                        const QPoint oldPosition( _previousWidget.data()->mapFromGlobal( _previousPosition ) );
                        const QPoint newPosition( _previousWidget.data()->mapFromGlobal( reciever->mapToGlobal( position ) ) );
                        postHoverEvent( _previousWidget.data(), QEvent::HoverLeave, newPosition, oldPosition );
                    }
                }

                // enter or move in current widget
                if( !reciever->rect().contains( reciever->mapFromGlobal( _previousPosition ) ) )
                {

                    // enter current widget if needed
                    postEvent( reciever, QEvent::Enter );
                    if( reciever->testAttribute( Qt::WA_Hover ) )
                    {
                        const QPoint oldPosition( reciever->mapFromGlobal( _previousPosition ) );
                        const QPoint newPosition( position );
                        postHoverEvent( reciever, QEvent::HoverEnter, newPosition, oldPosition );
                    }

                } else if( reciever->mapFromGlobal( _previousPosition ) != position ) {

                    // move mouse if needed
                    postMouseEvent( reciever, QEvent::MouseMove, Qt::NoButton, position );
                    if( reciever->testAttribute( Qt::WA_Hover ) )
                    {
                        const QPoint oldPosition( reciever->mapFromGlobal( _previousPosition ) );
                        const QPoint newPosition( position );
                        postHoverEvent( reciever, QEvent::HoverMove, newPosition, oldPosition );
                    }

                }

                // update previous widget and position
                _previousWidget = reciever;
                _previousPosition = reciever->mapToGlobal( position );
                break;
            }

            // click event
            case Event::Click:
            {
                postMouseClickEvent( reciever, Qt::LeftButton, event._position );
                break;
            }

            // slide
            case Event::Slide:
            {
                const QPoint& delta( event._position );

                // calculate begin position depending on widget type
                QPoint begin;
                if( const QSlider* slider = qobject_cast<const QSlider*>( reciever ) )
                {

                    // this is copied from QSlider::initStyleOption
                    QStyleOptionSlider option;
                    option.initFrom( slider );
                    option.orientation = slider->orientation();
                    option.sliderPosition = slider->sliderPosition();
                    option.minimum = slider->minimum();
                    option.maximum = slider->maximum();
                    option.upsideDown = (slider->orientation() == Qt::Horizontal) ?
                        ( slider->invertedAppearance() != (option.direction == Qt::RightToLeft))
                        : (!slider->invertedAppearance() );

                    QRect handleRect( slider->style()->subControlRect(
                        QStyle::CC_Slider, &option, QStyle::SC_SliderHandle,
                        slider ) );

                    if( !handleRect.isValid() ) break;
                    begin = handleRect.center();

                } else if( const QScrollBar* scrollbar = qobject_cast<const QScrollBar*>( reciever ) ) {

                    // this is copied from QSlider::initStyleOption
                    QStyleOptionSlider option;
                    option.initFrom( scrollbar );
                    option.orientation = scrollbar->orientation();
                    option.sliderPosition = scrollbar->sliderPosition();
                    option.minimum = scrollbar->minimum();
                    option.maximum = scrollbar->maximum();
                    option.upsideDown = scrollbar->invertedAppearance();
                    if( scrollbar->orientation() == Qt::Horizontal )
                    { option.state |= QStyle::State_Horizontal; }

                    QRect handleRect( scrollbar->style()->subControlRect(
                        QStyle::CC_ScrollBar, &option, QStyle::SC_ScrollBarSlider,
                        scrollbar ) );

                    if( !handleRect.isValid() ) break;
                    begin = handleRect.center();

                } else {

                    begin = reciever->rect().center();

                }
                const QPoint end( begin + delta );
                postMouseEvent( reciever, QEvent::MouseMove, Qt::NoButton, begin );
                postMouseEvent( reciever, QEvent::MouseButtonPress, Qt::LeftButton, begin, Qt::LeftButton );
                reciever->setFocus();
                postDelay( 50 );
                const int steps = 10;
                for( int i=0; i<steps; ++i )
                {
                    QPoint current(
                        begin.x() + qreal(i*( end.x()-begin.x() ))/(steps-1),
                        begin.y() + qreal(i*( end.y()-begin.y() ))/(steps-1) );
                    postMouseEvent( reciever, QEvent::MouseMove, Qt::NoButton, current, Qt::LeftButton, Qt::NoModifier );
                    postDelay( 20 );
                }

                postMouseEvent( reciever, QEvent::MouseButtonRelease, Qt::LeftButton, end );
                break;
            }

            case Event::SelectItem:
            {

                const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>( reciever );
                if( !( view && view->model() ) ) break;

                const int column( event._position.x() );
                const int row( event._position.y() );

                // find index
                const QModelIndex modelIndex( view->model()->index( row, column ) );
                if( !modelIndex.isValid() ) break;

                // get rect
                QRect r( view->visualRect( modelIndex ) );
                if( !r.isValid() ) break;

                // send event
                postMouseEvent( view->viewport(), QEvent::MouseMove, Qt::NoButton, r.center(), Qt::NoButton, Qt::NoModifier );
                postDelay(100);
                postMouseClickEvent( view->viewport(), Qt::LeftButton, r.center() );
                break;

            }

            case Event::ClearSelection:
            {
                const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>( reciever );
                postMouseEvent( view->viewport(), QEvent::MouseMove, Qt::NoButton, view->viewport()->rect().bottomRight(), Qt::NoButton, Qt::NoModifier );
                postDelay(100);
                postMouseClickEvent( view->viewport(), Qt::LeftButton, view->viewport()->rect().bottomRight() );
                break;
            }

            case Event::SelectComboBoxItem:
            {

                const QComboBox* combobox = qobject_cast<const QComboBox*>( reciever );
                if( !combobox ) break;

                // get arrow rect
                QStyleOptionComboBox option;
                option.initFrom( combobox );
                QRect arrowRect( combobox->style()->subControlRect( QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxArrow, combobox ) );

                // first click
                postMouseClickEvent( reciever, Qt::LeftButton, arrowRect.center() );
                postDelay( 100 );

                // select item in view
                QAbstractItemView* view = combobox->view();
                const int row( event._position.x() );
                const int column( 0 );

                // find index
                const QModelIndex modelIndex( view->model()->index( row, column ) );
                if( !modelIndex.isValid() ) break;

                // get rect
                QRect r( view->visualRect( modelIndex ) );
                if( !r.isValid() ) break;

                // send event
                postMouseEvent( view->viewport(), QEvent::MouseMove, Qt::NoButton, r.center(), Qt::NoButton, Qt::NoModifier );
                postDelay(100);
                postMouseClickEvent( view->viewport(), Qt::LeftButton, r.center() );
                break;

            }


            case Event::SelectMenuItem:
            {

                // retrieve menu
                QMenu* menu( 0 );
                if( const QToolButton* button = qobject_cast<const QToolButton*>( reciever ) ) menu = button->menu();
                else if( const QPushButton* button = qobject_cast<const QPushButton*>( reciever ) ) menu = button->menu();

                // abort if not found
                if( !menu ) break;

                // get action and geometry
                const int row( event._position.x() );
                QList<QAction*> actions( menu->actions() );
                if( row >= actions.size() ) break;

                menu->sizeHint();
                QRect r( menu->actionGeometry( actions[row] ) );
                if( !r.isValid() ) break;

                /*!
                HACK: As soon as leftMouseButton is pressed on a button with menu,
                the menu is shown and code is interrupted until an action is selected in the menu.
                As a consequence, one must first generate the events, execute them with a delay, and then
                click on the button (before delay is expired). This way, the menu events will be executed
                even if the menu is visible (and blocking further code execution).
                */
                _pendingWidget = menu;
                _pendingEvents.push_back( new QMouseEvent(
                    QEvent::MouseMove,
                    r.center(),
                    Qt::NoButton,
                    Qt::NoButton,
                    Qt::NoModifier ) );

                _pendingEvents.push_back( new QMouseEvent(
                    QEvent::MouseButtonPress,
                    r.center(),
                    Qt::LeftButton,
                    Qt::NoButton,
                    Qt::NoModifier ) );

                _pendingEvents.push_back( new QMouseEvent(
                    QEvent::MouseButtonRelease,
                    r.center(),
                    Qt::LeftButton,
                    Qt::NoButton,
                    Qt::NoModifier ) );

                _pendingEventsTimer.start( 150, this );

                // click
                postMouseEvent( reciever, QEvent::MouseButtonPress, Qt::LeftButton, reciever->rect().center(), Qt::NoButton, Qt::NoModifier );
                break;

            }

            case Event::SelectTab:
            {

                const QTabBar* tabbar = qobject_cast<const QTabBar*>( reciever );
                if( !tabbar ) break;

                const int index( event._position.x() );

                const QRect r( tabbar->tabRect( index ) );
                if( !r.isValid() ) break;

                postMouseClickEvent( reciever, Qt::LeftButton, r.center() );
                break;

            }

            case Event::WriteText:
            {

                reciever->setFocus();
                const QString& text( event._text );
                for( int i=0; i < text.length(); ++i )
                {
                    const Qt::Key key( toKey( text.at(i) ) );
                    const QString local( text.at(i) );
                    postKeyEvent( reciever, QEvent::KeyPress, key, local, Qt::NoModifier );
                    postKeyEvent( reciever, QEvent::KeyRelease, key, local, Qt::NoModifier );
                    postDelay( 20 );
                }
                break;

            }

            case Event::ClearText:
            {

                // select all and backspace
                reciever->setFocus();
                postKeyEvent( reciever, QEvent::KeyPress, Qt::Key_A, "a", Qt::ControlModifier );
                postKeyEvent( reciever, QEvent::KeyRelease, Qt::Key_A, "a", Qt::ControlModifier );
                postDelay( 20 );
                postKeyClickEvent( reciever, Qt::Key_Backspace, QString() );

            }

            default: break;

        }

        // delay
        if( event._delay > 0 ) postDelay( event._delay );
        else if( event._delay == -1 && _defaultDelay > 0 ) postDelay( _defaultDelay );

        return;

    }

    //_______________________________________________________________________
    void Simulator::postEvent( QWidget* reciever, QEvent::Type type )
    { postQEvent( reciever, new QEvent( type ) ); }

    //_______________________________________________________________________
    void Simulator::postHoverEvent( QWidget* reciever, QEvent::Type type, const QPoint& newPosition, const QPoint& oldPosition )
    { postQEvent( reciever, new QHoverEvent( type, newPosition, oldPosition ) ); }

    //_______________________________________________________________________
    void Simulator::postMouseClickEvent( QWidget* reciever, Qt::MouseButton button, const QPoint& position  )
    {

        // button press and button release
        postMouseEvent( reciever, QEvent::MouseButtonPress, button, position, button  );
        reciever->setFocus();
        postDelay(50);
        postMouseEvent( reciever, QEvent::MouseButtonRelease, button, position, button );

    }

    //_______________________________________________________________________
    void Simulator::postMouseEvent(
        QWidget* reciever,
        QEvent::Type type,
        Qt::MouseButton button,
        const QPoint& position,
        Qt::MouseButtons buttons,
        Qt::KeyboardModifiers modifiers )
    {
        postQEvent( reciever, new QMouseEvent(
            type,
            position,
            reciever->mapToGlobal( position ),
            button,
            buttons,
            modifiers ) );
    }

    //_______________________________________________________________________
    void Simulator::postKeyClickEvent( QWidget* reciever, Qt::Key key, QString text, Qt::KeyboardModifiers modifiers )
    {
        postKeyModifiersEvent( reciever, QEvent::KeyPress, modifiers );
        postKeyEvent( reciever, QEvent::KeyPress, key, text, modifiers );
        postKeyEvent( reciever, QEvent::KeyRelease, key, text, modifiers );
        postKeyModifiersEvent( reciever, QEvent::KeyRelease, modifiers );

    }

    //_______________________________________________________________________
    void Simulator::postKeyModifiersEvent( QWidget* reciever, QEvent::Type type, Qt::KeyboardModifiers modifiers )
    {

        if( modifiers == Qt::NoModifier ) return;

        switch( type )
        {

            case QEvent::KeyPress:
            {
                if( modifiers & Qt::ShiftModifier)
                { postKeyEvent( reciever, QEvent::KeyPress, Qt::Key_Shift, QString() ); }

                if( modifiers & Qt::ControlModifier )
                { postKeyEvent( reciever, QEvent::KeyPress, Qt::Key_Control, QString(), modifiers & Qt::ShiftModifier ); }

                if( modifiers & Qt::AltModifier )
                { postKeyEvent( reciever, QEvent::KeyPress, Qt::Key_Alt, QString(), modifiers & (Qt::ShiftModifier|Qt::ControlModifier) ); }

                if( modifiers & Qt::MetaModifier )
                { postKeyEvent( reciever, QEvent::KeyPress, Qt::Key_Meta, QString(), modifiers & (Qt::ShiftModifier|Qt::ControlModifier|Qt::AltModifier) ); }

                break;

            }

            case QEvent::KeyRelease:
            {

                if( modifiers & Qt::MetaModifier )
                { postKeyEvent( reciever, QEvent::KeyRelease, Qt::Key_Meta, QString() ); }

                if( modifiers & Qt::AltModifier )
                { postKeyEvent( reciever, QEvent::KeyRelease, Qt::Key_Alt, QString(), modifiers & Qt::MetaModifier ); }

                if( modifiers & Qt::ControlModifier )
                { postKeyEvent( reciever, QEvent::KeyRelease, Qt::Key_Control, QString(), modifiers & (Qt::MetaModifier|Qt::AltModifier) ); }

                if( modifiers & Qt::ShiftModifier)
                { postKeyEvent( reciever, QEvent::KeyRelease, Qt::Key_Shift, QString(), modifiers & (Qt::MetaModifier|Qt::AltModifier|Qt::ControlModifier) ); }

            }

            default: break;
        }

    }

    //_______________________________________________________________________
    void Simulator::postKeyEvent( QWidget* reciever, QEvent::Type type, Qt::Key key, QString text, Qt::KeyboardModifiers modifiers )
    { postQEvent( reciever, new QKeyEvent( type, key, modifiers, text ) ); }

    //_______________________________________________________________________
    void Simulator::postDelay( int delay )
    {
        // this is largely inspired from qtestlib's qsleep implementation
        _timer.start( delay, this );
        while( _timer.isActive() )
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, delay);
            int ms( 10 );
            struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
            nanosleep(&ts, NULL);
        }

    }

    //_______________________________________________________________________
    Qt::Key Simulator::toKey( QChar a ) const
    { return (Qt::Key) QKeySequence( a )[0]; }

    //_______________________________________________________________________
    void Simulator::postQEvent( QWidget* reciever, QEvent* event )
    { qApp->postEvent( reciever, event ); }
}
