//////////////////////////////////////////////////////////////////////////////
// oxygenblurhelper.cpp
// handle regions passed to kwin for blurring
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Loosely inspired (and largely rewritten) from BeSpin style
// Copyright (C) 2007 Thomas Luebking <thomas.luebking@web.de>
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

#include "oxygenblurhelper.h"
#include "oxygenblurhelper.moc"

#include "oxygenstyleconfigdata.h"

#include <QtCore/QEvent>
#include <QtCore/QVector>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

namespace Oxygen
{

    //___________________________________________________________
    BlurHelper::BlurHelper( QObject* parent, StyleHelper& helper ):
        QObject( parent ),
        _helper( helper ),
        _enabled( false )
    {

        #ifdef Q_WS_X11

        // create atom
        _blurAtom = XInternAtom( QX11Info::display(), "_KDE_NET_WM_BLUR_BEHIND_REGION", False);
        _opaqueAtom = XInternAtom( QX11Info::display(), "_NET_WM_OPAQUE_REGION", False);

        #endif

    }

    //___________________________________________________________
    void BlurHelper::registerWidget( QWidget* widget )
    {

        // check if already registered
        if( _widgets.contains( widget ) ) return;

        // install event filter
        addEventFilter( widget );

        // add to widgets list
        _widgets.insert( widget );

        // cleanup on destruction
        connect( widget, SIGNAL( destroyed( QObject* ) ), SLOT( widgetDestroyed( QObject* ) ) );

        // schedule shadow area repaint
        _pendingWidgets.insert( widget, widget );
        delayedUpdate();

    }

    //___________________________________________________________
    void BlurHelper::unregisterWidget( QWidget* widget )
    {
        // remove event filter
        widget->removeEventFilter( this );

        // remove from widgets
        _widgets.remove( widget );

        if( isTransparent( widget ) ) clear( widget );
    }

    //___________________________________________________________
    bool BlurHelper::eventFilter( QObject* object, QEvent* event )
    {

        // do nothing if not enabled
        if( !enabled() ) return false;

        switch( event->type() )
        {

            case QEvent::Hide:
            {
                QWidget* widget( qobject_cast<QWidget*>( object ) );
                if( widget && isOpaque( widget ) && isTransparent( widget->window() ) )
                {
                    QWidget* window( widget->window() );
                    _pendingWidgets.insert( window, window );
                    delayedUpdate();
                }
                break;

            }

            case QEvent::Show:
            case QEvent::Resize:
            {

                // cast to widget and check
                QWidget* widget( qobject_cast<QWidget*>( object ) );
                if( !widget ) break;
                if( isTransparent( widget ) )
                {

                    _pendingWidgets.insert( widget, widget );
                    delayedUpdate();

                } else if( isOpaque( widget ) ) {

                    QWidget* window( widget->window() );
                    if( isTransparent( window ) )
                    {
                        _pendingWidgets.insert( window, window );
                        delayedUpdate();
                    }

                }

                break;
            }

            default: break;

        }

        // never eat events
        return false;

    }

    //___________________________________________________________
    QRegion BlurHelper::blurRegion( QWidget* widget ) const
    {

        if( !widget->isVisible() ) return QRegion();

        // get main region
        QRegion region;
        if(
            qobject_cast<const QDockWidget*>( widget ) ||
            qobject_cast<const QMenu*>( widget )  ||
            qobject_cast<const QToolBar*>( widget )  ||
            widget->inherits( "QComboBoxPrivateContainer" ) )
        {

            region = _helper.roundedMask( widget->rect() );

        } else region = widget->mask().isEmpty() ? widget->rect():widget->mask();


        // trim blur region to remove unnecessary areas
        trimBlurRegion( widget, widget, region );
        return region;

    }

    //___________________________________________________________
    void BlurHelper::trimBlurRegion( QWidget* parent, QWidget* widget, QRegion& region ) const
    {


        // loop over children
        foreach( QObject* childObject, widget->children() )
        {
            QWidget* child( qobject_cast<QWidget*>( childObject ) );
            if( !(child && child->isVisible()) ) continue;

            if( isOpaque( child ) )
            {

                const QPoint offset( child->mapTo( parent, QPoint( 0, 0 ) ) );
                if( child->mask().isEmpty() )
                {
                    const QRect rect( child->rect().translated( offset ).adjusted( 1, 1, -1, -1 ) );
                    region -= rect;

                }  else region -= child->mask().translated( offset );

            } else { trimBlurRegion( parent, child, region ); }

        }

        return;

    }

    //___________________________________________________________
    void BlurHelper::update( QWidget* widget ) const
    {

        #ifdef Q_WS_X11

        /*
        directly from bespin code. Supposibly prevent playing with some 'pseudo-widgets'
        that have winId matching some other -random- window
        */
        if( !(widget->testAttribute(Qt::WA_WState_Created) || widget->internalWinId() ))
        { return; }

        const QRegion blurRegion( this->blurRegion( widget ) );
        const QRegion opaqueRegion = QRegion(0, 0, widget->width(), widget->height()) - blurRegion;
        if( blurRegion.isEmpty() ) {

            clear( widget );

        } else {

            QVector<unsigned long> data;
            foreach( const QRect& rect, blurRegion.rects() )
            { data << rect.x() << rect.y() << rect.width() << rect.height(); }

            XChangeProperty(
                QX11Info::display(), widget->winId(), _blurAtom, XA_CARDINAL, 32, PropModeReplace,
                reinterpret_cast<const unsigned char *>(data.constData()), data.size() );

            data.clear();
            foreach( const QRect& rect, opaqueRegion.rects() )
            { data << rect.x() << rect.y() << rect.width() << rect.height(); }

            XChangeProperty(
                QX11Info::display(), widget->winId(), _opaqueAtom, XA_CARDINAL, 32, PropModeReplace,
                reinterpret_cast<const unsigned char *>(data.constData()), data.size() );
        }

        // force update
        if( widget->isVisible() )
        { widget->update(); }

        #endif

    }


    //___________________________________________________________
    void BlurHelper::clear( QWidget* widget ) const
    {
        #ifdef Q_WS_X11
        XDeleteProperty( QX11Info::display(), widget->winId(), _blurAtom );
        XDeleteProperty( QX11Info::display(), widget->winId(), _opaqueAtom );
        #endif

    }

    //___________________________________________________________
    bool BlurHelper::isOpaque( const QWidget* widget ) const
    {

        return
            (!widget->isWindow()) &&
            ( (widget->autoFillBackground() && widget->palette().color( widget->backgroundRole() ).alpha() == 0xff ) ||
            widget->testAttribute(Qt::WA_OpaquePaintEvent) );

    }

    //___________________________________________________________
    bool BlurHelper::isTransparent( const QWidget* widget ) const
    {

        return
            widget->isWindow() &&
            widget->testAttribute( Qt::WA_TranslucentBackground ) &&

            // widgets using qgraphicsview
            !( widget->graphicsProxyWidget() ||
            widget->inherits( "Plasma::Dialog" ) ) &&

            // flags and special widgets
            ( widget->testAttribute( Qt::WA_StyledBackground ) ||
            qobject_cast<const QMenu*>( widget ) ||
            qobject_cast<const QDockWidget*>( widget ) ||
            qobject_cast<const QToolBar*>( widget ) ) &&
            _helper.hasAlphaChannel( widget );
    }

}
