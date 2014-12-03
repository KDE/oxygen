//////////////////////////////////////////////////////////////////////////////
// oxygenframeshadow.h
// handle sunken frames' shadows
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include <QDebug>
#include <QAbstractScrollArea>
#include <QApplication>
#include <QFrame>
#include <QMouseEvent>
#include <QPainter>
#include <QSplitter>

#include <KColorUtils>

namespace Oxygen
{

    //____________________________________________________________________________________
    bool FrameShadowFactory::registerWidget( QWidget* widget, StyleHelper& helper )
    {

        if( !widget ) return false;
        if( isRegistered( widget ) ) return false;

        // check whether widget is a frame, and has the proper shape
        bool accepted = false;
        bool flat = false;

        // cast to frame and check
        QFrame* frame( qobject_cast<QFrame*>( widget ) );
        if( frame )
        {
            // also do not install on QSplitter
            /*
            due to Qt, splitters are set with a frame style that matches the condition below,
            though no shadow should be installed, obviously
            */
            if( qobject_cast<QSplitter*>( widget ) ) return false;

            // further checks on frame shape, and parent
            if( frame->frameStyle() == (QFrame::StyledPanel | QFrame::Sunken) ) accepted = true;
            else if( widget->parent() && widget->parent()->inherits( "QComboBoxPrivateContainer" ) )
            {

                accepted = true;
                flat = true;

            }

        } else if( widget->inherits( "KTextEditor::View" ) ) accepted = true;

        if( !accepted ) return false;

        // make sure that the widget is not embedded into a KHTMLView
        QWidget* parent( widget->parentWidget() );
        while( parent && !parent->isTopLevel() )
        {
            if( parent->inherits( "KHTMLView" ) ) return false;
            parent = parent->parentWidget();
        }

        // store in set
        _registeredWidgets.insert( widget );

        // catch object destruction
        connect( widget, SIGNAL(destroyed(QObject*)), SLOT(widgetDestroyed(QObject*)) );

        // install shadow
        installShadows( widget, helper, flat );

        return true;

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::unregisterWidget( QWidget* widget )
    {
        if( !isRegistered( widget ) ) return;
        _registeredWidgets.remove( widget );
        removeShadows( widget );
    }

    //____________________________________________________________________________________
    bool FrameShadowFactory::eventFilter( QObject* object, QEvent* event )
    {

        switch( event->type() )
        {
            // TODO: possibly implement ZOrderChange event, to make sure that
            // the shadow is always painted on top
            case QEvent::ZOrderChange:
            {
                raiseShadows( object );
                break;
            }

            case QEvent::Show:
            updateShadowsGeometry( object );
            update( object );
            break;

            case QEvent::Resize:
            updateShadowsGeometry( object );
            break;

            default: break;
        }

        return QObject::eventFilter( object, event );

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::installShadows( QWidget* widget, StyleHelper& helper, bool flat )
    {

        removeShadows(widget);

        widget->installEventFilter(this);

        widget->installEventFilter( &_addEventFilter );
        if( !flat )
        {
            installShadow( widget, helper, ShadowAreaLeft );
            installShadow( widget, helper, ShadowAreaRight );
        }

        installShadow( widget, helper, ShadowAreaTop, flat );
        installShadow( widget, helper, ShadowAreaBottom, flat );
        widget->removeEventFilter( &_addEventFilter );

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::removeShadows( QWidget* widget )
    {

        widget->removeEventFilter( this );

        const QList<QObject* > children = widget->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase*>(child) )
            {
                shadow->hide();
                shadow->setParent(0);
                shadow->deleteLater();
            }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::updateShadowsGeometry( QObject* object ) const
    {

        const QList<QObject *> children = object->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase *>(child) )
            { shadow->updateGeometry(); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::updateShadowsGeometry( const QObject* object, QRect rect ) const
    {

        const QList<QObject *> children = object->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase *>(child) )
            { shadow->updateGeometry( rect ); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::raiseShadows( QObject* object ) const
    {

        const QList<QObject *> children = object->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase *>(child) )
            { shadow->raise(); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::update( QObject* object ) const
    {

        const QList<QObject* > children = object->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase *>(child) )
            { shadow->update();}
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::setHasContrast( const QWidget* widget, bool value ) const
    {

        const QList<QObject *> children = widget->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase *>(child) )
            { shadow->setHasContrast( value ); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::updateState( const QWidget* widget, bool focus, bool hover, qreal opacity, AnimationMode mode ) const
    {

        const QList<QObject *> children = widget->children();
        foreach( QObject *child, children )
        {
            if( FrameShadowBase* shadow = qobject_cast<FrameShadowBase *>(child) )
            { shadow->updateState( focus, hover, opacity, mode ); }
        }

    }

    //____________________________________________________________________________________
    void FrameShadowFactory::installShadow( QWidget* widget, StyleHelper& helper, ShadowArea area, bool flat ) const
    {
        FrameShadowBase *shadow(0);
        if( flat ) shadow = new FlatFrameShadow( area, helper );
        else shadow = new SunkenFrameShadow( area, helper );
        shadow->setParent(widget);
        shadow->hide();
    }

    //____________________________________________________________________________________
    void FrameShadowFactory::widgetDestroyed( QObject* object )
    { _registeredWidgets.remove( object ); }

    //____________________________________________________________________________________
    void FrameShadowBase::init()
    {

        setAttribute(Qt::WA_OpaquePaintEvent, false);

        setFocusPolicy(Qt::NoFocus);
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setContextMenuPolicy(Qt::NoContextMenu);

        // grab viewport widget
        QWidget *viewport( FrameShadowBase::viewport() );
        if( !viewport && parentWidget() )
        { viewport = parentWidget(); }

        // set cursor from viewport
        if (viewport) setCursor(viewport->cursor());

    }

     //____________________________________________________________________________________
    QWidget* FrameShadowBase::viewport( void ) const
    {

        if( !parentWidget() ) return nullptr;
        else if( QAbstractScrollArea *widget = qobject_cast<QAbstractScrollArea *>(parentWidget()) ) {

            return widget->viewport();

        } else return nullptr;

    }

    //____________________________________________________________________________________
    void SunkenFrameShadow::updateGeometry( QRect rect )
    {

        // show on first call
        if( isHidden() ) show();

        // store offsets between passed rect and parent widget rect
        QRect parentRect( parentWidget()->contentsRect() );
        setMargins( QMargins(
            rect.left() - parentRect.left(),
            rect.top() - parentRect.top(),
            rect.right() - parentRect.right(),
            rect.bottom() - parentRect.bottom() ) );

        // adjust geometry to take out part that is not rendered anyway
        rect.adjust( 1, 1, -1, -1 );

        // adjust geometry
        const int shadowSize( 3 );
        switch( shadowArea() )
        {

            case ShadowAreaTop:
            rect.setHeight( shadowSize );
            break;

            case ShadowAreaBottom:
            rect.setTop( rect.bottom() - shadowSize + 1 );
            break;

            case ShadowAreaLeft:
            rect.setWidth(shadowSize);
            rect.adjust(0, shadowSize, 0, -shadowSize );
            break;


            case ShadowAreaRight:
            rect.setLeft(rect.right() - shadowSize + 1 );
            rect.adjust(0, shadowSize, 0, -shadowSize );
            break;

            default:
            return;
        }

        setGeometry(rect);

    }

    //____________________________________________________________________________________
    void SunkenFrameShadow::updateState( bool focus, bool hover, qreal opacity, AnimationMode mode )
    {
        bool changed( false );
        if( _hasFocus != focus ) { _hasFocus = focus; changed |= true; }
        if( _mouseOver != hover ) { _mouseOver = hover; changed |= !_hasFocus; }
        if( _mode != mode )
        {

            _mode = mode;
            changed |=
                (_mode == AnimationNone) ||
                (_mode == AnimationFocus) ||
                (_mode == AnimationHover && !_hasFocus );

        }

        if( _opacity != opacity ) { _opacity = opacity; changed |= (_mode != AnimationNone ); }
        if( changed )
        {

            if( QWidget* viewport = this->viewport() )
            {

                // need to disable viewport updates to avoid some redundant painting
                // besides it fixes one visual glitch (from Qt) in QTableViews
                viewport->setUpdatesEnabled( false );
                update() ;
                viewport->setUpdatesEnabled( true );

            } else update();

        }
    }

    //____________________________________________________________________________________
    void SunkenFrameShadow::paintEvent(QPaintEvent *event )
    {

        // this fixes shadows in frames that change frameStyle() after polish()
        if( QFrame *frame = qobject_cast<QFrame *>( parentWidget() ) )
        { if (frame->frameStyle() != (QFrame::StyledPanel | QFrame::Sunken)) return; }

        const QRect parentRect( parentWidget()->contentsRect().translated( mapFromParent( QPoint( 0, 0 ) ) ) );
        const QRect rect( parentRect.adjusted( margins().left(), margins().top(), margins().right(), margins().bottom() ) );

        // render
        QPainter painter(this);
        painter.setClipRegion( event->region() );

        StyleOptions options( HoleOutline );
        if( _hasFocus ) options |= Focus;
        if( _mouseOver ) options |= Hover;
        if( hasContrast() ) options |= HoleContrast;
        _helper.renderHole( &painter, palette().color( QPalette::Window ), rect, options, _opacity, _mode, TileSet::Ring );

        return;

    }

    //____________________________________________________________________________________
    void FlatFrameShadow::updateGeometry()
    { if( QWidget *widget = parentWidget() ) updateGeometry( widget->contentsRect() ); }

    //____________________________________________________________________________________
    void FlatFrameShadow::updateGeometry( QRect rect )
    {

        // show on first call
        if( isHidden() ) show();

        // store offsets between passed rect and parent widget rect
        QRect parentRect( parentWidget()->contentsRect() );
        setMargins( QMargins(
            rect.left() - parentRect.left(),
            rect.top() - parentRect.top(),
            rect.right() - parentRect.right(),
            rect.bottom() - parentRect.bottom() ) );

        const int shadowSize( 3 );
        switch( shadowArea() )
        {

            case ShadowAreaTop:
            rect.setHeight( shadowSize );
            break;

            case ShadowAreaBottom:
            rect.setTop( rect.bottom() - shadowSize + 1 );
            break;

            default:
            return;
        }

        setGeometry(rect);
    }


    //____________________________________________________________________________________
    void FlatFrameShadow::paintEvent(QPaintEvent *event )
    {

        // this fixes shadows in frames that change frameStyle() after polish()
        if( QFrame *frame = qobject_cast<QFrame *>( parentWidget() ) )
        { if( frame->frameStyle() != QFrame::NoFrame ) return; }

        const QWidget* parent( parentWidget() );
        const QRect parentRect( parent->contentsRect() );
        const QRect rect( parentRect.adjusted( margins().left(), margins().top(), margins().right(), margins().bottom() ) );

        QPixmap pixmap( _helper.highDpiPixmap( size() ) );
        {

            pixmap.fill( Qt::transparent );
            QPainter painter( &pixmap );
            painter.setClipRegion( event->region() );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.translate( -geometry().topLeft() );
            painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
            painter.setPen( Qt::NoPen );
            _helper.renderMenuBackground( &painter, geometry(), parent, parent->palette() );

            // mask
            painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
            painter.setBrush( Qt::black );
            painter.drawRoundedRect( QRectF(rect), 2.5, 2.5 );

        }

        QPainter painter( this );
        painter.setClipRegion( event->region() );
        painter.fillRect( rect, Qt::transparent );
        painter.drawPixmap( QPoint(0,0), pixmap );

        return;

    }

}
