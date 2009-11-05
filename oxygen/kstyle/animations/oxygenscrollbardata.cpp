//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbardata.cpp
// data container for QTabBar animations
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenscrollbardata.h"
#include "oxygenscrollbardata.moc"

#include <QtGui/QHoverEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QStyleOptionSlider>

Q_GUI_EXPORT QStyleOptionSlider qt_qscrollbarStyleOption(QScrollBar*);

namespace Oxygen
{

    ScrollBarData::ScrollBarData( QWidget* parent, int duration ):
        SliderData( parent, duration ),
        addLineArrowHovered_( false ),
        subLineArrowHovered_( false ),
        addLineAnimation_( new Animation( duration, this ) ),
        subLineAnimation_( new Animation( duration, this ) ),
        addLineOpacity_( 0 ),
        subLineOpacity_( 0 )
    {

        connect( addLineAnimation().data(), SIGNAL( finished( void ) ), SLOT( clearAddLineRect( void ) ) );
        connect( subLineAnimation().data(), SIGNAL( finished( void ) ), SLOT( clearSubLineRect( void ) ) );

        // setup animation
        setupAnimation( addLineAnimation(), "addLineOpacity" );
        setupAnimation( subLineAnimation(), "subLineOpacity" );

    }

    //______________________________________________
    const Animation::Pointer& ScrollBarData::animation( QStyle::SubControl subcontrol ) const
    {
        switch( subcontrol )
        {
            default:
            case QStyle::SC_ScrollBarSlider:
            return animation();

            case QStyle::SC_ScrollBarAddLine:
            return addLineAnimation();

            case QStyle::SC_ScrollBarSubLine:
            return subLineAnimation();
        }

    }

    //______________________________________________
    qreal ScrollBarData::opacity( QStyle::SubControl subcontrol ) const
    {
        switch( subcontrol )
        {
            default:
            case QStyle::SC_ScrollBarSlider:
            return opacity();

            case QStyle::SC_ScrollBarAddLine:
            return addLineOpacity();

            case QStyle::SC_ScrollBarSubLine:
            return subLineOpacity();
        }

    }

    //______________________________________________
    void ScrollBarData::hoverMoveEvent(  QObject* object, QEvent* event )
    {

        // try cast object to scrollbar
        QScrollBar* scrollBar( qobject_cast<QScrollBar*>( object ) );
        if( !scrollBar || scrollBar->isSliderDown() ) return;

        // retrieve scrollbar option
        QStyleOptionSlider opt( qt_qscrollbarStyleOption( qobject_cast<QScrollBar*>( object ) ) );

        // cast event
        QHoverEvent *he = static_cast<QHoverEvent*>(event);
        if( !he ) return;

        // need to properly handle arrow hover.
        // probably need to add two more timers for the arrows (add and sub)
        QStyle::SubControl hoverControl = scrollBar->style()->hitTestComplexControl(QStyle::CC_ScrollBar, &opt, he->pos(), scrollBar);

        updateSlider( hoverControl );
        updateAddLineArrow( hoverControl );
        updateSubLineArrow( hoverControl );
        return;

    }


    //______________________________________________
    void ScrollBarData::hoverLeaveEvent(  QObject* object, QEvent* event )
    {
        Q_UNUSED( object );
        Q_UNUSED( event );
        updateSlider( QStyle::SC_None );
        updateSubLineArrow( QStyle::SC_None );
        updateAddLineArrow( QStyle::SC_None );
    }


    //_____________________________________________________________________
    void ScrollBarData::updateSlider( QStyle::SubControl hoverControl )
    {
        if( hoverControl == QStyle::SC_ScrollBarSlider )
        {

            if( !sliderHovered() ) {
                setSliderHovered( true );
                animation().data()->setDirection( Animation::Forward );
                if( !animation().data()->isRunning() ) animation().data()->start();
            }

        } else {

            if( sliderHovered() )
            {
                setSliderHovered( false );
                animation().data()->setDirection( Animation::Backward );
                if( !animation().data()->isRunning() ) animation().data()->start();
            }

        }
    }

    //_____________________________________________________________________
    void ScrollBarData::updateSubLineArrow( QStyle::SubControl hoverControl )
    {
        if( hoverControl == QStyle::SC_ScrollBarSubLine )
        {

            if( !subLineArrowHovered() )
            {
                setSubLineArrowHovered( true );
                subLineAnimation().data()->setDirection( Animation::Forward );
                if( !subLineAnimation().data()->isRunning() ) subLineAnimation().data()->start();
             }

        } else {

            if( subLineArrowHovered() )
            {
                setSubLineArrowHovered( false );
                subLineAnimation().data()->setDirection( Animation::Backward );
                if( !subLineAnimation().data()->isRunning() ) subLineAnimation().data()->start();
            }

        }
    }

    //_____________________________________________________________________
    void ScrollBarData::updateAddLineArrow( QStyle::SubControl hoverControl )
    {
        if( hoverControl == QStyle::SC_ScrollBarAddLine )
        {

            if( !addLineArrowHovered() )
            {
                setAddLineArrowHovered( true );
                addLineAnimation().data()->setDirection( Animation::Forward );
                if( !addLineAnimation().data()->isRunning() ) addLineAnimation().data()->start();
            }

        } else {

            if( addLineArrowHovered() )
            {
                setAddLineArrowHovered( false );
                addLineAnimation().data()->setDirection( Animation::Backward );
                if( !addLineAnimation().data()->isRunning() ) addLineAnimation().data()->start();
            }

        }
    }

}
