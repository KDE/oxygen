//////////////////////////////////////////////////////////////////////////////
// oxygensliderdata.cpp
// data container for QSlider animations
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

#include "oxygensliderdata.h"
#include "oxygensliderdata.moc"

#include <QtGui/QHoverEvent>
#include <QtGui/QSlider>
#include <QtGui/QStyleOptionSlider>

Q_GUI_EXPORT QStyleOptionSlider qt_qsliderStyleOption(QSlider*);


namespace Oxygen
{

    //______________________________________________
    bool SliderData::eventFilter( QObject* object, QEvent* event )
    {

        if( object != target().data() )
        { return GenericData::eventFilter( object, event ); }

        // check event type
        switch( event->type() )
        {

            case QEvent::HoverEnter:
            case QEvent::HoverMove:
            hoverMoveEvent( object, event );
            break;

            case QEvent::HoverLeave:
            hoverLeaveEvent( object, event );
            break;

            default: break;

        }

        return GenericData::eventFilter( object, event );

    }


    //______________________________________________
    void SliderData::hoverMoveEvent(  QObject* object, QEvent* event )
    {

        // try cast object to slider
        QSlider* slider( qobject_cast<QSlider*>( object ) );
        if( !slider || slider->isSliderDown() ) return;

        // retrieve slider option
        QStyleOptionSlider opt( qt_qsliderStyleOption( qobject_cast<QSlider*>( object ) ) );

        // cast event
        QHoverEvent *he = static_cast<QHoverEvent*>(event);
        if( !he ) return;

        QStyle::SubControl hoverControl = slider->style()->hitTestComplexControl(QStyle::CC_Slider, &opt, he->pos(), slider);
        updateSlider( hoverControl );

    }

    //______________________________________________
    void SliderData::hoverLeaveEvent(  QObject* object, QEvent* event )
    {
        Q_UNUSED( object );
        Q_UNUSED( event );
        updateSlider( QStyle::SC_None );
    }

    //_____________________________________________________________________
    void SliderData::updateSlider( QStyle::SubControl hoverControl )
    {

        if( hoverControl == QStyle::SC_SliderHandle )
        {

            if( !sliderHovered() ) {
                setSliderHovered( true );
                if( enabled() )
                {
                    animation().data()->setDirection( Animation::Forward );
                    if( !animation().data()->isRunning() ) animation().data()->start();
                } else setDirty();
            }

        } else {

            if( sliderHovered() )
            {
                setSliderHovered( false );
                if( enabled() )
                {
                    animation().data()->setDirection( Animation::Backward );
                    if( !animation().data()->isRunning() ) animation().data()->start();
                } else setDirty();
            }

        }
    }

}
