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

#include <QtCore/QTextStream>
#include <QtCore/QCoreApplication>

namespace Oxygen
{

    //____________________________________________________________________
    bool SplitterFactory::registerWidget( QWidget *widget )
    {

        // check widget type
        if( qobject_cast<QMainWindow*>( widget ) )
        {

            widget->installEventFilter( &_addEventFilter );
            new SplitterProxy( widget, widget );
            widget->removeEventFilter( &_addEventFilter );
            return true;

        } else if( qobject_cast<QSplitterHandle*>( widget ) ) {

            QWidget* window( widget->window() );
            window->installEventFilter( &_addEventFilter );
            new SplitterProxy( widget->window(), widget );
            window->removeEventFilter( &_addEventFilter );

            return true;

        } else return false;

    }

    //____________________________________________________________________
    SplitterProxy::SplitterProxy( QWidget* parent, QWidget* target ):
        QWidget( parent ),
        _splitter( target )
    {
        hide();
        _splitter->installEventFilter( this );
    }

    //____________________________________________________________________
    SplitterProxy::~SplitterProxy( void )
    {}

    //____________________________________________________________________
    bool SplitterProxy::event( QEvent *event )
    {
        switch( event->type() )
        {

            case QEvent::Paint:
            { return true; }

            case QEvent::MouseMove:
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            {
                event->accept();

                // grab on mouse press
                if( event->type() == QEvent::MouseButtonPress) grabMouse();

                parentWidget()->setUpdatesEnabled(false);
                resize(1,1);
                parentWidget()->setUpdatesEnabled(true);

                // cast to mouse event
                QMouseEvent *mouseEvent( static_cast<QMouseEvent*>( event ) );

                // get relevant position to post mouse drag event to application
                const QPoint pos( (event->type() == QEvent::MouseMove) ? _splitter->mapFromGlobal(QCursor::pos()) : _hook );
                QMouseEvent mouseEvent2(
                    mouseEvent->type(), pos, _splitter->mapToGlobal(pos),
                    mouseEvent->button(),
                    mouseEvent->buttons(), mouseEvent->modifiers());

                QCoreApplication::sendEvent(_splitter, &mouseEvent2 );

                // release grab on mouse-Release
                if( event->type() == QEvent::MouseButtonRelease )
                { if (mouseGrabber() == this) releaseMouse(); }

                return true;

            }

            case QEvent::Leave:
            {

                // leave event and reset splitter
                QWidget::leaveEvent( event );
                if( !rect().contains( mapFromGlobal( QCursor::pos() ) ) )
                { setEnabled( false ); }
                return true;

            }

            default:
            return QWidget::event( event );

        }

        // fallback
        return QWidget::event( event );

    }

    //____________________________________________________________________
    bool SplitterProxy::eventFilter( QObject* object, QEvent* event )
    {

        if( mouseGrabber() ) return false;

        switch( event->type() )
        {

            case QEvent::HoverEnter:
            // cast to splitter handle
            if( !isVisible() && qobject_cast<QSplitterHandle*>( object ) )
            { setEnabled( true ); }

            return false;

            case QEvent::HoverMove:
            case QEvent::HoverLeave:
            if( isVisible() && object == _splitter )
            return true;

            case QEvent::MouseMove:
            case QEvent::Timer:
            case QEvent::Move:

            // just for performance - they can occur really often
            return false;

            case QEvent::CursorChange:
            if( QWidget *window = qobject_cast<QMainWindow*>( object ) )
            {
                if (window->cursor().shape() == Qt::SplitHCursor ||
                    window->cursor().shape() == Qt::SplitVCursor)
                    { setEnabled( true ); }
            }
            return false;

            case QEvent::MouseButtonRelease:
            if( qobject_cast<QSplitterHandle*>(object) || qobject_cast<QMainWindow*>(object) )
            { setEnabled( false ); }
            return false;

            case QEvent::ParentChange:
            {
                QWidget* window( static_cast<QWidget*>( object )->window() );
                if( window != parentWidget() )
                {
                    // need to reparent the proxy in case the window has changed
                    if( isVisible() ) hide();
                    window->installEventFilter( &_addEventFilter );
                    setParent( window );
                    window->removeEventFilter( &_addEventFilter );
                }
            }

            default:
            return false;

        }

        return false;
    }

    //____________________________________________________________________
    void SplitterProxy::setEnabled( bool value )
    {

        if( !value )
        {

            // release mouse
            if( mouseGrabber() == this ) releaseMouse();

            // hide
            hide();

            // set hover event
            if( _splitter )
            {
                QHoverEvent hoverEvent(
                    qobject_cast<QSplitterHandle*>(_splitter) ? QEvent::HoverLeave : QEvent::HoverMove,
                    _splitter->mapFromGlobal(QCursor::pos()), _hook);
                QCoreApplication::sendEvent( _splitter, &hoverEvent );
            }

            return;

        } else {

            _hook = _splitter->mapFromGlobal(QCursor::pos());

            QRect r( 0, 0, 2*Splitter_ExtendedWidth, 2*Splitter_ExtendedWidth );
            r.moveCenter( parentWidget()->mapFromGlobal( QCursor::pos() ) );
            setGeometry(r);
            setCursor( _splitter->cursor().shape() );

            raise();
            show();

        }

    }

}
