//////////////////////////////////////////////////////////////////////////////
// oxygensplitterproxy.cpp
// Extended hit area for Splitters
// -------------------
//
// Copyright (C) 2011 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Based on Bespin splitterproxy code
// Copyright (C) 2011 Thomas Luebking <thomas.luebking@web.de>
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
//////////////////////////////////////////////////////////////////////////////

#include "oxygensplitterproxy.h"
#include "oxygenmetrics.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtGui/QPainter>

namespace Oxygen
{

    //____________________________________________________________________
    bool SplitterFactory::registerWidget( QWidget *widget )
    {

        // check widget type
        if( qobject_cast<QMainWindow*>( widget ) )
        {

            WidgetMap::iterator iter( _widgets.find( widget ) );
            if( iter == _widgets.end() || !iter.value() )
            {
                widget->installEventFilter( &_addEventFilter );
                SplitterProxy* proxy( new SplitterProxy( widget ) );
                widget->removeEventFilter( &_addEventFilter );

                widget->installEventFilter( proxy );

                _widgets.insert( widget, proxy );

            } else {

                widget->removeEventFilter( iter.value().data() );
                widget->installEventFilter( iter.value().data() );

            }

            return true;

        } else if( qobject_cast<QSplitterHandle*>( widget ) ) {

            QWidget* window( widget->window() );
            WidgetMap::iterator iter( _widgets.find( window ) );
            if( iter == _widgets.end() || !iter.value() )
            {


                window->installEventFilter( &_addEventFilter );
                SplitterProxy* proxy( new SplitterProxy( window ) );
                window->removeEventFilter( &_addEventFilter );

                widget->installEventFilter( proxy );
                _widgets.insert( window, proxy );

            } else {

                widget->removeEventFilter( iter.value().data() );
                widget->installEventFilter( iter.value().data() );

            }

            return true;

        } else return false;

    }

    //____________________________________________________________________
    void SplitterFactory::unregisterWidget( QWidget *widget )
    {
        WidgetMap::iterator iter( _widgets.find( widget ) );
        if( iter != _widgets.end() )
        {
            if( iter.value() ) iter.value().data()->deleteLater();
            _widgets.erase( iter );
        }

    }

    //____________________________________________________________________
    SplitterProxy::SplitterProxy( QWidget* parent ):
        QWidget( parent ),
        _timerId( 0 )
    {
        setAttribute( Qt::WA_TranslucentBackground, true );
        setAttribute( Qt::WA_OpaquePaintEvent, false );
        hide();
    }

    //____________________________________________________________________
    SplitterProxy::~SplitterProxy( void )
    {}


    //____________________________________________________________________
    bool SplitterProxy::eventFilter( QObject* object, QEvent* event )
    {

        if( mouseGrabber() ) return false;

        switch( event->type() )
        {

            case QEvent::HoverEnter:
            if( !isVisible() )
            {

                // cast to splitter handle
                if( QSplitterHandle* handle = qobject_cast<QSplitterHandle*>( object ) )
                { setSplitter( handle ); }

            }

            return false;

            case QEvent::HoverMove:
            case QEvent::HoverLeave:
            return isVisible() && object == _splitter.data();

            case QEvent::MouseMove:
            case QEvent::Timer:
            case QEvent::Move:
            return false;

            case QEvent::CursorChange:
            if( QWidget *window = qobject_cast<QMainWindow*>( object ) )
            {
                if( window->cursor().shape() == Qt::SplitHCursor || window->cursor().shape() == Qt::SplitVCursor )
                { setSplitter( window ); }
            }
            return false;

            case QEvent::WindowDeactivate:
            case QEvent::MouseButtonRelease:
            clearSplitter();
            return false;

            default:
            return false;

        }

    }

    //____________________________________________________________________
    bool SplitterProxy::event( QEvent *event )
    {
        switch( event->type() )
        {

            case QEvent::MouseMove:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            {

                // check splitter
                if( !_splitter ) return false;

                event->accept();

                // grab on mouse press
                if( event->type() == QEvent::MouseButtonPress) grabMouse();

                parentWidget()->setUpdatesEnabled(false);
                resize(1,1);
                parentWidget()->setUpdatesEnabled(true);

                // cast to mouse event
                QMouseEvent *mouseEvent( static_cast<QMouseEvent*>( event ) );

                // get relevant position to post mouse drag event to application
                const QPoint pos( (event->type() == QEvent::MouseMove) ? _splitter.data()->mapFromGlobal(QCursor::pos()) : _hook );
                QMouseEvent mouseEvent2(
                    mouseEvent->type(), pos,
                    _splitter.data()->mapToGlobal(pos),
                    mouseEvent->button(),
                    mouseEvent->buttons(), mouseEvent->modifiers());

                QCoreApplication::sendEvent( _splitter.data(), &mouseEvent2 );

                // release grab on mouse-Release
                if( event->type() == QEvent::MouseButtonRelease && mouseGrabber() == this )
                { releaseMouse(); }

                return true;

            }

            case QEvent::Timer:
            if( static_cast<QTimerEvent*>( event )->timerId() != _timerId )
            { return QWidget::event( event ); }

            if( mouseGrabber() == this )
            { return true; }

            /*
            Fall through is intended.
            We somehow lost a QEvent::Leave and gonna fix that from here
            */

            case QEvent::HoverLeave:
            case QEvent::Leave:
            {

                // reset splitter
                if( isVisible() && !rect().contains( mapFromGlobal( QCursor::pos() ) ) )
                { clearSplitter(); }
                return true;

            }

            default:
            return QWidget::event( event );

        }

    }

    //____________________________________________________________________
    void SplitterProxy::setSplitter( QWidget* widget )
    {

        // check if changed
        if( _splitter.data() == widget ) return;

        _splitter = widget;
        _hook = _splitter.data()->mapFromGlobal(QCursor::pos());

        QRect r( 0, 0, 2*Splitter_ExtendedWidth, 2*Splitter_ExtendedWidth );
        r.moveCenter( parentWidget()->mapFromGlobal( QCursor::pos() ) );
        setGeometry(r);
        setCursor( _splitter.data()->cursor().shape() );

        raise();
        show();

        /*
        timer used to automatically hide proxy
        in case leave events are lost
        */
        if( !_timerId ) _timerId = startTimer(150);
    }


    //____________________________________________________________________
    void SplitterProxy::clearSplitter( void )
    {

        // check if changed
        if( !_splitter ) return;

        // release mouse
        if( mouseGrabber() == this ) releaseMouse();

        // hide
        hide();

        // set hover event
        if( _splitter )
        {
            QHoverEvent hoverEvent(
                qobject_cast<QSplitterHandle*>(_splitter.data()) ? QEvent::HoverLeave : QEvent::HoverMove,
                _splitter.data()->mapFromGlobal(QCursor::pos()), _hook);
            QCoreApplication::sendEvent( _splitter.data(), &hoverEvent );
            _splitter.clear();

        }

        // kill timer if any
        if( _timerId )
        {
            killTimer( _timerId );
            _timerId = 0;
        }

    }

}
