//////////////////////////////////////////////////////////////////////////////
// oxygentransitionwidget.cpp
// stores event filters and maps widgets to transitions for transitions
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygentransitionwidget.h"
#include "oxygentransitionwidget.moc"

#include <cassert>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QStyleOption>
#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>

namespace Oxygen
{

    //________________________________________________
    TransitionWidget::TransitionWidget( QWidget* parent, int duration ):
        QWidget( parent ),
        grabFromWindow_( false ),
        paintEnabled_( true ),
        animation_( new Animation( duration, this ) ),
        opacity_(0)
    {

        // background flags
        setAttribute(Qt::WA_NoSystemBackground );
        setAutoFillBackground( false );

        // setup animation
        animation_.data()->setStartValue( 0 );
        animation_.data()->setEndValue( 1.0 );
        animation_.data()->setTargetObject( this );
        animation_.data()->setPropertyName( "opacity" );

        // setup connections
        connect( animation_.data(), SIGNAL( valueChanged( const QVariant& ) ), SLOT( setDirty( void ) ) );
        connect( animation_.data(), SIGNAL( finished( void ) ), SLOT( setDirty( void ) ) );
        connect( animation_.data(), SIGNAL( finished( void ) ), SIGNAL( finished( void ) ) );

    }

    //________________________________________________
    bool TransitionWidget::canGrab( QWidget* widget ) const
    {
        if( !widget ) widget = parentWidget();
        if( !widget ) return false;
        if( widget->isTopLevel() && widget->testAttribute( Qt::WA_NoSystemBackground ) ) return false;
        return true;
    }

    //________________________________________________
    QPixmap TransitionWidget::grab( QWidget* widget, QRect rect )
    {

        // change rect
        if( rect.isNull() ) rect = widget->rect();

        // initialize pixmap
        QPixmap out( rect.size() );
        out.fill( Qt::transparent );
        paintEnabled_ = false;

        if( grabFromWindow_ )
        {

            rect = rect.translated( widget->mapTo( widget->window(), widget->rect().topLeft() ) );
            widget = widget->window();
            out = QPixmap::grabWidget( widget, rect );


        } else {

            grabBackground( out, widget, rect );
            grabWidget( out, widget, rect );

        }

        paintEnabled_ = true;

        return out;

    }

    //________________________________________________
    void TransitionWidget::paintEvent( QPaintEvent* event )
    {

        // fully transparent case
        if( opacity() >= 1.0 && endPixmap().isNull() ) return;
        if( !paintEnabled_ ) return;

        // get rect
        QRect rect = event->rect();
        if( !rect.isValid() ) rect = TransitionWidget::rect();

        // local pixmap
        QPixmap local( size() );
        local.fill( Qt::transparent );
        {
            QPainter p( &local );
            p.setClipRect( event->rect() );

            // start pixmap
            if( opacity() < 1 && !startPixmap_.isNull() )
            {
                // draw pixmap
                p.drawPixmap( QPoint(0,0), startPixmap_ );

                // opacity mask
                if( opacity() > 0 )
                {
                    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                    QColor color( Qt::black );
                    color.setAlphaF( 1.0-opacity() );
                    p.fillRect(rect, color );
                }
            }

            p.end();

        }

        // copy local pixmap to widget
        {
            QPainter p( this );
            p.setClipRect( event->rect() );

            // end pixmap
            if( opacity() > 0 && !endPixmap_.isNull() )
            { p.drawPixmap( QPoint(), endPixmap() ); }

            p.drawPixmap( QPoint(), local );
            p.end();
        }

    }

    //________________________________________________
    void TransitionWidget::grabBackground( QPixmap& pixmap, QWidget* widget, QRect& rect ) const
    {
        if (!widget) return;

        QWidgetList widgets;
        if( widget->autoFillBackground() )
        { widgets.push_back( widget ); }

        QWidget *parent(0);

        // get highest level parent
        for( parent = widget->parentWidget(); parent; parent = parent->parentWidget() )
        {

            if( !parent->isVisible()) continue;
            widgets.push_back( parent );

            // stop at topLevel
            if( parent->isTopLevel() || parent->autoFillBackground() ) break;

        }

        if( !parent ) parent = widget;

        // painting
        QPainter p(&pixmap);
        p.setClipRect( rect );
        const QBrush backgroundBrush = parent->palette().brush( parent->backgroundRole());
        if( backgroundBrush.style() == Qt::TexturePattern)
        {

            p.drawTiledPixmap( rect, backgroundBrush.texture(), widget->mapTo( parent, rect.topLeft() ) );

        } else {

            p.fillRect( pixmap.rect(), backgroundBrush );

        }

        if( parent->isTopLevel() && parent->testAttribute(Qt::WA_StyledBackground))
        {
            QStyleOption option;
            option.initFrom(parent);
            option.rect = rect;
            option.rect.translate( widget->mapTo( parent, rect.topLeft() ) );
            p.translate(-option.rect.topLeft());
            parent->style()->drawPrimitive ( QStyle::PE_Widget, &option, &p, parent );
        }
        p.end();

        // draw all widgets in parent list
        // backward
        QPaintEvent event(rect);
        for( int i = widgets.size() - 1; i>=0; i-- )
        {
            QWidget* w = widgets.at(i);
            QPainter::setRedirected( w, &pixmap, widget->mapTo(w, rect.topLeft() ) );
            event = QPaintEvent(QRect( QPoint(), rect.size()));
            QCoreApplication::sendEvent(w, &event);
            QPainter::restoreRedirected(w);
        }

    }

    //________________________________________________
    void TransitionWidget::grabWidget( QPixmap& pixmap, QWidget* widget, QRect& rect ) const
    {

        // render main widget
        widget->render( &pixmap, pixmap.rect().topLeft(), rect, QWidget::DrawChildren );
        return;

        // need to render children one by one to skip this widget
        // in case it is already visible.
        QList<QWidget*> widgets( widget->findChildren<QWidget*>() );
        foreach( QWidget* w, widgets )
        {
            if( w->parent() == widget && w != this && w->isVisibleTo( widget ) )
            { w->render( &pixmap, w->mapToParent( w->rect().topLeft() ) - rect.topLeft(), w->rect(), QWidget::DrawChildren ); }
        }
    }

}
