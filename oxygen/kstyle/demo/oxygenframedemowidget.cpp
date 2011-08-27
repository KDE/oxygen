//////////////////////////////////////////////////////////////////////////////
// oxygenframedemowidget.cpp
// oxygen frames demo widget
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

#include "oxygenframedemowidget.h"
#include "oxygenframedemowidget.moc"

#include <QtGui/QButtonGroup>

#include <KComboBox>
#include <KLocale>
#include <KTabWidget>

namespace Oxygen
{

    //_____________________________________________________________
    FrameDemoWidget::FrameDemoWidget( QWidget* parent ):
        DemoWidget( parent )
    {

        ui.setupUi( this );
        QButtonGroup* group = new QButtonGroup( this );
        group->addButton( ui.raisedFrameRadioButton );
        group->addButton( ui.plainFrameRadioButton );
        group->addButton( ui.sunkenFrameRadioButton );

        connect( ui.raisedFrameRadioButton, SIGNAL(toggled(bool)), SLOT(toggleRaisedFrame(bool)) );
        connect( ui.plainFrameRadioButton, SIGNAL(toggled(bool)), SLOT(togglePlainFrame(bool)) );
        connect( ui.sunkenFrameRadioButton, SIGNAL(toggled(bool)), SLOT(toggleSunkenFrame(bool)) );

        connect( ui.directionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateLayoutDirection(int)) );
        connect( ui.flatGroupBoxCheckBox, SIGNAL(toggled(bool)), SLOT(toggleFlatGroupBox(bool)) );

    }

    //_____________________________________________________________
    void FrameDemoWidget::updateLayoutDirection( int value )
    {

        QBoxLayout::Direction direction;
        switch( value )
        {
            default: case 0: direction = QBoxLayout::LeftToRight; break;
            case 1: direction =  QBoxLayout::RightToLeft; break;
            case 2: direction =  QBoxLayout::TopToBottom; break;
            case 3: direction =  QBoxLayout::BottomToTop; break;
        }

        if( direction != ui.frameLayout->direction() )
        {
            ui.frameLayout->setDirection( direction );
            ui.frameLayout->update();
        }
    }

    //_____________________________________________________________
    void FrameDemoWidget::benchmark( void )
    {
        if( !isVisible() ) return;

        if( true )
        {
            simulator().selectComboBoxItem( ui.directionComboBox, 1 );
            simulator().selectComboBoxItem( ui.directionComboBox, 2 );
            simulator().selectComboBoxItem( ui.directionComboBox, 3 );
            simulator().selectComboBoxItem( ui.directionComboBox, 0 );
        }

        if( true )
        {
            simulator().click( ui.flatGroupBoxCheckBox );
            simulator().click( ui.flatGroupBoxCheckBox );
        }

        if( true )
        {
            simulator().click( ui.plainFrameRadioButton );
            simulator().click( ui.sunkenFrameRadioButton );
            simulator().click( ui.raisedFrameRadioButton );
        }

        simulator().run();

    }

}
