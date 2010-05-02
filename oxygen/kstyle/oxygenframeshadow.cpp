//////////////////////////////////////////////////////////////////////////////
// oxygenframeshadow.h
// handle sunken frames' shadows
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Largely inspired from skulpture widget style
// Copyright (c) 2007-2009 Christoph Feck <christoph@maxiom.de>
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

#include "oxygenframeshadow.h"
#include "oxygenframeshadow.moc"

#include <QtGui/QAbstractScrollArea>
#include <QtGui/QApplication>
#include <QtGui/QFrame>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtCore/QTextStream>

#include <KColorUtils>

namespace Oxygen
{

    //____________________________________________________________________________________
    void FrameShadowManager::installShadows( QWidget* widget, StyleHelper& helper )
    {

        QWidget* parent( widget->parentWidget() );
        while( parent && !parent->isTopLevel() )
        {
            if( parent->inherits( "KHTMLView" ) ) return;
            parent = parent->parentWidget();
        }

        removeShadows(widget);

        widget->installEventFilter(this);
        installShadow( widget, helper, Left );
        installShadow( widget, helper, Top );
        installShadow( widget, helper, Right );
        installShadow( widget, helper, Bottom );

    }

    //____________________________________________________________________________________
    void FrameShadowManager::removeShadows( QWidget* widget )
    {

        widget->removeEventFilter( this );

        const QList<QObject* > children = widget->children();
        foreach( QObject *child, children )
        {
            if( FrameShadow* shadow = qobject_cast<FrameShadow *>(child) )
            {
                shadow->hide();
                shadow->setParent(0);
                shadow->deleteLater();
            }
        }

    }

    //____________________________________________________________________________________
    bool FrameShadowManager::eventFilter( QObject* object, QEvent* event )
    {

        switch( event->type() )
        {
            case QEvent::Resize:
            updateShadowsGeometry( object );
            break;

            default: break;
        }

        return QObject::eventFilter( object, event );

    }

    //____________________________________________________________________________________
    void FrameShadowManager::updateShadowsGeometry( QObject* object ) const
    {

        const QList<QObject *> children = object->children();
        foreach( QObject *child, children )
        {
            if( FrameShadow* shadow = qobject_cast<FrameShadow *>(child) )
            { shadow->updateGeometry(); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowManager::updateState( const QWidget* widget, bool focus, bool hover, qreal opacity, AnimationMode mode ) const
    {

        const QList<QObject *> children = widget->children();
        foreach( QObject *child, children )
        {
            if( FrameShadow* shadow = qobject_cast<FrameShadow *>(child) )
            { shadow->updateState( focus, hover, opacity, mode ); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowManager::installShadow( QWidget* widget, StyleHelper& helper, ShadowArea area ) const
    {
        FrameShadow *shadow = new FrameShadow( area, helper );
        shadow->hide();
        shadow->setParent(widget);
        shadow->updateGeometry();
        shadow->show();
    }

    //____________________________________________________________________________________
    void FrameShadow::init()
    {

        setAttribute(Qt::WA_OpaquePaintEvent, false);

        setFocusPolicy(Qt::NoFocus);
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setContextMenuPolicy(Qt::NoContextMenu);

        // grab viewport widget
        QWidget *viewport( FrameShadow::viewport() );
        if( !viewport && parentWidget() && parentWidget()->inherits( "Q3ListView" ) )
        { viewport = parentWidget(); }

        // set cursor from viewport
        if (viewport) setCursor(viewport->cursor());

    }

    //____________________________________________________________________________________
    void FrameShadow::updateGeometry()
    {

        QWidget *widget = parentWidget();
        if( !widget ) return;

        QRect cr = widget->contentsRect();
        switch (shadowArea())
        {

            case Top:
            cr.setHeight(SHADOW_SIZE_TOP);
            break;

            case Left:
            cr.setWidth(SHADOW_SIZE_LEFT);
            cr.adjust(0, SHADOW_SIZE_TOP, 0, -SHADOW_SIZE_BOTTOM);
            break;

            case Bottom:
            cr.setTop(cr.bottom() - SHADOW_SIZE_BOTTOM + 1);
            break;

            case Right:
            cr.setLeft(cr.right() - SHADOW_SIZE_RIGHT + 1);
            cr.adjust(0, SHADOW_SIZE_TOP, 0, -SHADOW_SIZE_BOTTOM);
            break;

            case Unknown:
            default:
            return;
        }

        setGeometry(cr);
    }

    //____________________________________________________________________________________
    bool FrameShadow::event(QEvent *e)
    {

        // paintEvents are handled separately
        if (e->type() == QEvent::Paint) return QWidget::event(e);

        QWidget *viewport( FrameShadow::viewport() );
        if (!viewport) return false;

        switch (e->type())
        {

            case QEvent::DragEnter:
            case QEvent::DragMove:
            case QEvent::DragLeave:
            case QEvent::Drop:
            {
                setAcceptDrops(viewport->acceptDrops());
                QObject *o = viewport;
                return o->event(e);
            }
            break;

            case QEvent::Enter:
            setCursor(viewport->cursor());
            setAcceptDrops(viewport->acceptDrops());
            break;

            case QEvent::ContextMenu:
            {

                QContextMenuEvent *me = static_cast<QContextMenuEvent *>(e);
                QContextMenuEvent *ne = new QContextMenuEvent(me->reason(), parentWidget()->mapFromGlobal(me->globalPos()), me->globalPos());
                QApplication::sendEvent(viewport, ne);
                e->accept();
                return true;
            }
            break;

            case QEvent::MouseButtonPress: releaseMouse();
            case QEvent::MouseMove:
            case QEvent::MouseButtonRelease:
            {
                QMouseEvent *me = static_cast<QMouseEvent *>(e);
                QMouseEvent *ne = new QMouseEvent(e->type(), parentWidget()->mapFromGlobal(me->globalPos()), me->globalPos(), me->button(), me->buttons(), me->modifiers());
                QApplication::sendEvent(viewport, ne);
                e->accept();
                return true;
            }
            break;

            default:
            break;
        }

        e->ignore();
        return false;

    }

    //____________________________________________________________________________________
    void FrameShadow::updateState( bool focus, bool hover, qreal opacity, AnimationMode mode )
    {
        bool changed( false );
        if( _focus != focus ) { _focus = focus; changed = true; }
        if( _hover != hover ) { _hover = hover; changed = true; }
        if( _opacity != opacity ) { _opacity = opacity; changed = true; }
        if( _mode != mode ) { _mode = mode; changed = true; }
        if( changed ) update() ;
    }

    //____________________________________________________________________________________
    void FrameShadow::paintEvent(QPaintEvent *event )
    {

        // this fixes shadows in frames that change frameStyle() after polish()
        if (QFrame *frame = qobject_cast<QFrame *>(parentWidget()))
        { if (frame->frameStyle() != (QFrame::StyledPanel | QFrame::Sunken)) return; }


        QWidget *parent = parentWidget();
        QRect r = parent->contentsRect();
        r.translate(mapFromParent(QPoint(0, 0)));

        QColor base( palette().color(QPalette::Window) );
        TileSet::Tiles tiles;
        switch( _area )
        {
            case Top:
            {
                tiles = TileSet::Left|TileSet::Top|TileSet::Right;
                r.adjust( -3, -3, 3, 0 );
                break;
            }

            case Bottom:
            {
                tiles = TileSet::Left|TileSet::Bottom|TileSet::Right;
                r.adjust( -3, 0, 3, 3 );
                break;
            }

            case Left:
            {
                tiles = TileSet::Left;
                r.adjust( -3, -3, 0, 3 );
                break;
            }

            case Right:
            {
                tiles = TileSet::Right;
                r.adjust( 0, -3, 3, 3 );
                break;
            }

            default: return;
        }

        QPainter painter(this);
        painter.setClipRegion( event->region() );
        _helper.renderHole( &painter, palette().color( QPalette::Window ), r, _focus, _hover, _opacity, _mode, tiles );

        return;

    }

    //____________________________________________________________________________________
    QWidget* FrameShadow::viewport( void ) const
    {

        if( !parentWidget() )  return NULL;

        if( QAbstractScrollArea *widget = qobject_cast<QAbstractScrollArea *>(parentWidget()) )
        {

            return widget->viewport();

        } else return NULL;

    }

}
