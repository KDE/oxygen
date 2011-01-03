//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetexplorer.cpp
// print widget's and parent's information on mouse click
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

#include "oxygenwidgetexplorer.h"
#include "oxygenwidgetexplorer.moc"

#include <QtCore/QTextStream>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

namespace Oxygen
{

    //________________________________________________
    WidgetExplorer::WidgetExplorer( QObject* parent ):
        QObject( parent ),
        _enabled( false ),
        _drawWidgetRects( false )
    {

        _eventTypes.insert( QEvent::Enter, "Enter" );
        _eventTypes.insert( QEvent::Leave, "Leave" );

        _eventTypes.insert( QEvent::HoverMove, "HoverMove" );
        _eventTypes.insert( QEvent::HoverEnter, "HoverEnter" );
        _eventTypes.insert( QEvent::HoverLeave, "HoverLeave" );

        _eventTypes.insert( QEvent::MouseMove, "MouseMove" );
        _eventTypes.insert( QEvent::MouseButtonPress, "MouseButtonPress" );
        _eventTypes.insert( QEvent::MouseButtonRelease, "MouseButtonRelease" );

        _eventTypes.insert( QEvent::FocusIn, "FocusIn" );
        _eventTypes.insert( QEvent::FocusOut, "FocusOut" );

        // _eventTypes.insert( QEvent::Paint, "Paint" );

    }

    //________________________________________________
    void WidgetExplorer::setEnabled( bool value )
    {
        if( value == _enabled ) return;
        _enabled = value;

        qApp->removeEventFilter( this );
        if( _enabled )  qApp->installEventFilter( this );
    }

    //________________________________________________
    bool WidgetExplorer::eventFilter( QObject* object, QEvent* event )
    {

        if( object->isWidgetType() )
        {
            QString type( _eventTypes[event->type()] );
            if( !type.isEmpty() )
            {
                QTextStream( stdout ) << "Oxygen::WidgetExplorer::eventFilter - widget: " << object << " (" << object->metaObject()->className() << ")";
                QTextStream( stdout ) << " type: " << type  << endl;
            }
        }

        switch( event->type() )
        {
            case QEvent::Paint:
            if( _drawWidgetRects )
            {
                QWidget* widget( qobject_cast<QWidget*>( object ) );
                if( !widget ) return false;

                QPainter painter( widget );
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setBrush( Qt::NoBrush );
                painter.setPen( Qt::red );
                painter.drawRect( widget->rect() );
                painter.end();
            }
            break;

            case QEvent::MouseButtonPress:
            {

                // cast event and check button
                QMouseEvent* mouseEvent( static_cast<QMouseEvent*>( event ) );
                if( mouseEvent->button() != Qt::LeftButton ) break;

                // case widget and check (should not be necessary)
                QWidget* widget( qobject_cast<QWidget*>(object) );
                if( !widget ) return false;

                QTextStream( stdout )
                    << "Oxygen::WidgetExplorer::eventFilter -"
                    << " event: " << event << " type: " << eventType( event->type() )
                    << " widget: " << widgetInformation( widget )
                    << endl;

                // print parent information
                QWidget* parent( widget->parentWidget() );
                while( parent )
                {
                    QTextStream( stdout ) << "    parent: " << widgetInformation( parent ) << endl;
                    parent = parent->parentWidget();
                }
                QTextStream( stdout ) << "" << endl;

            }
            break;

            default: break;

        }

        // always return false to go on with normal chain
        return false;

    }

    //________________________________________________
    QString WidgetExplorer::eventType( const QEvent::Type& type ) const
    {
        switch( type )
        {
            case QEvent::MouseButtonPress: return "MouseButtonPress";
            case QEvent::MouseButtonRelease: return "MouseButtonRelease";
            case QEvent::MouseMove: return "MouseMove";
            default: return "Unknown";
        }
    }

    //________________________________________________
    QString WidgetExplorer::widgetInformation( const QWidget* widget ) const
    {

        QRect r( widget->geometry() );
        QString className( widget->metaObject()->className() );
        QString out;
        QTextStream( &out ) << widget << " (" << className << ")"
            << " position: " << r.x() << "," << r.y()
            << " size: " << r.width() << "," << r.height()
            << " hover: " << widget->testAttribute( Qt::WA_Hover );
        return out;
    }

}
