//////////////////////////////////////////////////////////////////////////////
// oxygensliderdemowidget.cpp
// oxygen sliders demo widget
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include <QMenu>
#include <QStyleOptionSlider>

namespace Oxygen
{

    //_____________________________________________________________
    ProgressBar::ProgressBar( QObject* parent, QProgressBar* progressBar, QCheckBox* checkBox ):
        QObject( parent ),
        _progressBar( progressBar ),
        _checkBox( checkBox ),
        _value( 0 )
    { connect( _checkBox, SIGNAL(toggled(bool)), SLOT(toggleBusy(bool)) ); }

    //_____________________________________________________________
    void ProgressBar::toggleBusy( bool value )
    {

        if( value )
        {
            _value = _progressBar->value();
            _progressBar->setMinimum( 0 );
            _progressBar->setMaximum( 0 );

        } else {

            _progressBar->setMinimum( 0 );
            _progressBar->setMaximum( 100 );
            _progressBar->setValue( _value );

        }

        _progressBar->update();

    }

    //_____________________________________________________________
    void ProgressBar::setValue( int value )
    {
        if( !_checkBox->isChecked() )
        { _progressBar->setValue( value ); }
    }

    //_____________________________________________________________
    SliderDemoWidget::SliderDemoWidget( QWidget* parent ):
        DemoWidget( parent ),
        _locked( false )
    {

        ui.setupUi( this );

        _horizontalProgressBar = new ProgressBar( this, ui.horizontalProgressBar, ui.animateProgressBarCheckBox );
        _verticalProgressBar = new ProgressBar( this, ui.verticalProgressBar, ui.animateProgressBarCheckBox );

        connect( ui.invertProgressBarCheckBox, SIGNAL(toggled(bool)), _horizontalProgressBar, SLOT(toggleInvertedAppearance(bool)));
        connect( ui.invertProgressBarCheckBox, SIGNAL(toggled(bool)), _verticalProgressBar, SLOT(toggleInvertedAppearance(bool)));
        connect( ui.tickPositionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateTickPosition(int)) );

        connect( ui.horizontalSlider, SIGNAL(valueChanged(int)), SLOT(updateSliders(int)) );
        connect( ui.horizontalScrollBar, SIGNAL(valueChanged(int)), SLOT(updateSliders(int)) );
        connect( ui.verticalSlider, SIGNAL(valueChanged(int)), SLOT(updateSliders(int)) );
        connect( ui.verticalScrollBar, SIGNAL(valueChanged(int)), SLOT(updateSliders(int)) );
        connect( ui.dial, SIGNAL(valueChanged(int)), SLOT(updateSliders(int)) );

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
        _horizontalProgressBar->setValue( value );
        _verticalProgressBar->setValue( value );

        ui.horizontalSlider->setValue( value );
        ui.verticalSlider->setValue( value );
        ui.horizontalScrollBar->setValue( value );
        ui.verticalScrollBar->setValue( value );
        ui.dial->setValue( value );

        _locked = false;

    }

    //_____________________________________________________________
    void SliderDemoWidget::updateTickPosition( int value )
    {
        ui.horizontalSlider->setTickPosition( QSlider::TickPosition( value ) );
        ui.verticalSlider->setTickPosition( QSlider::TickPosition( value ) );
        ui.dial->setNotchesVisible( value > 0 );
    }
}
