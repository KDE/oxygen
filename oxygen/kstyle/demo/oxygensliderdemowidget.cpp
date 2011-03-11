//////////////////////////////////////////////////////////////////////////////
// oxygensliderdemowidget.cpp
// oxygen sliders demo widget
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

#include "oxygensliderdemowidget.h"
#include "oxygensliderdemowidget.moc"

#include <QtGui/QMenu>
#include <QtGui/QStyleOptionSlider>

namespace Oxygen
{

    //_____________________________________________________________
    SliderDemoWidget::SliderDemoWidget( QWidget* parent ):
        DemoWidget( parent ),
        _locked( false )
    {

        ui.setupUi( this );
        connect( ui.horizontalSlider, SIGNAL( valueChanged( int ) ), SLOT( updateSliders( int ) ) );
        connect( ui.horizontalScrollBar, SIGNAL( valueChanged( int ) ), SLOT( updateSliders( int ) ) );
        connect( ui.verticalSlider, SIGNAL( valueChanged( int ) ), SLOT( updateSliders( int ) ) );
        connect( ui.verticalScrollBar, SIGNAL( valueChanged( int ) ), SLOT( updateSliders( int ) ) );
        connect( ui.dial, SIGNAL( valueChanged( int ) ), SLOT( updateSliders( int ) ) );

    }

    //_____________________________________________________________
    void SliderDemoWidget::benchmark( void )
    {

        if( !isVisible() ) return;

        // horizontal
        simulator().slide( ui.horizontalSlider, QPoint( 50, 0 ) );
        simulator().slide( ui.horizontalSlider, QPoint( -50, 0 ) );

        simulator().slide( ui.horizontalScrollBar, QPoint( 50, 0 ) );
        simulator().slide( ui.horizontalScrollBar, QPoint( -50, 0 ) );

        // vertical
        simulator().slide( ui.verticalScrollBar, QPoint( 0, 50 ) );
        simulator().slide( ui.verticalScrollBar, QPoint( 0, -50 ) );

        simulator().slide( ui.verticalSlider, QPoint( 0, 50 ) );
        simulator().slide( ui.verticalSlider, QPoint( 0, -50 ) );

        // dial button
        // nothing for now.

        simulator().run();
    }

    //_____________________________________________________________
    void SliderDemoWidget::updateSliders( int value )
    {
        if( _locked ) return;
        _locked = true;
        ui.horizontalSlider->setValue( value );
        ui.verticalSlider->setValue( value );
        ui.progressBar->setValue( value );
        ui.progressBar_3->setValue( value );
        ui.horizontalScrollBar->setValue( value );
        ui.verticalScrollBar->setValue( value );
        ui.dial->setValue( value );
        _locked = false;
    }
}
