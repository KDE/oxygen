//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbardata.cpp
// stores event filters and maps widgets to timelines for animations
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

#include "oxygenscrollbardata.h"
#include "oxygenscrollbardata.moc"

#include <QtGui/QHoverEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QStyleOptionSlider>

Q_GUI_EXPORT QStyleOptionSlider qt_qscrollbarStyleOption(QScrollBar*);

namespace Oxygen
{

    ScrollBarData::ScrollBarData( QObject* parent, QWidget* target, int maxFrame, int duration ):
        SliderData( parent, target, maxFrame, duration ),
        addLineArrowHovered_( false ),
        subLineArrowHovered_( false ),
        addLineTimeLine_( new TimeLine( duration, this ) ),
        subLineTimeLine_( new TimeLine( duration, this ) )
    {

        // setup timeLine
        addLineTimeLine()->setFrameRange( 0, maxFrame );
        addLineTimeLine()->setCurveShape( QTimeLine::EaseInOutCurve );
        connect( addLineTimeLine_.data(), SIGNAL( frameChanged( int ) ), SLOT( setDirty( void ) ) );
        connect( addLineTimeLine_.data(), SIGNAL( finished( void ) ), SLOT( clearAddLineRect( void ) ) );
        connect( addLineTimeLine_.data(), SIGNAL( finished( void ) ), SLOT( setDirty( void ) ) );


        // setup timeLine
        subLineTimeLine()->setFrameRange( 0, maxFrame );
        subLineTimeLine()->setCurveShape( QTimeLine::EaseInOutCurve );
        connect( subLineTimeLine_.data(), SIGNAL( frameChanged( int ) ), SLOT( setDirty( void ) ) );
        connect( subLineTimeLine_.data(), SIGNAL( finished( void ) ), SLOT( clearSubLineRect( void ) ) );
        connect( subLineTimeLine_.data(), SIGNAL( finished( void ) ), SLOT( setDirty( void ) ) );

    }

    //______________________________________________
    const TimeLine::Pointer& ScrollBarData::timeLine( QStyle::SubControl subcontrol ) const
    {
        switch( subcontrol )
        {
            default:
            case QStyle::SC_ScrollBarSlider:
            return timeLine();

            case QStyle::SC_ScrollBarAddLine:
            return addLineTimeLine();

            case QStyle::SC_ScrollBarSubLine:
            return subLineTimeLine();
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
                timeLine()->setDirection( QTimeLine::Forward );
                if( !timeLine()->isRunning() ) timeLine()->start();
            }

        } else {

            if( sliderHovered() )
            {
                setSliderHovered( false );
                timeLine()->setDirection( QTimeLine::Backward );
                if( !timeLine()->isRunning() ) timeLine()->start();
            }

        }
    }

    //_____________________________________________________________________
    void ScrollBarData::updateSubLineArrow( QStyle::SubControl hoverControl )
    {
        if( hoverControl == QStyle::SC_ScrollBarSubLine )
        {

            if( !subLineArrowHovered() ) {
                setSubLineArrowHovered( true );
                subLineTimeLine()->setDirection( QTimeLine::Forward );
                if( !subLineTimeLine()->isRunning() ) subLineTimeLine()->start();
             }

        } else {

            if( subLineArrowHovered() )
            {
                setSubLineArrowHovered( false );
                subLineTimeLine()->setDirection( QTimeLine::Backward );
                if( !subLineTimeLine()->isRunning() ) subLineTimeLine()->start();
            }

        }
    }

    //_____________________________________________________________________
    void ScrollBarData::updateAddLineArrow( QStyle::SubControl hoverControl )
    {
        if( hoverControl == QStyle::SC_ScrollBarAddLine )
        {

            if( !addLineArrowHovered() ) {

                setAddLineArrowHovered( true );
                addLineTimeLine()->setDirection( QTimeLine::Forward );
                if( !addLineTimeLine()->isRunning() ) addLineTimeLine()->start();
            }

        } else {

            if( addLineArrowHovered() )
            {
                setAddLineArrowHovered( false );
                addLineTimeLine()->setDirection( QTimeLine::Backward );
                if( !addLineTimeLine()->isRunning() ) addLineTimeLine()->start();
            }

        }
    }

}
