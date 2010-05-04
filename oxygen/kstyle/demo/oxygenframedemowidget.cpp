//////////////////////////////////////////////////////////////////////////////
// oxygenframedemowidget.cpp
// oxygen tabwidget demo dialog
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

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QButtonGroup>
#include <QtGui/QRadioButton>

#include <KComboBox>
#include <KLocale>
#include <KTabWidget>

namespace Oxygen
{

    //_____________________________________________________________
    FrameDemoWidget::FrameDemoWidget( QWidget* parent ):
        QWidget( parent )
    {
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->setMargin(0);
        setLayout( vLayout );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        vLayout->addItem( hLayout );

        // label and combobox
        QLabel* label;
        hLayout->addWidget( label = new QLabel( i18n( "Layout Direction:" ), this ) );

        KComboBox* comboBox;
        hLayout->addWidget( comboBox = new KComboBox( this ) );
        comboBox->addItems( QStringList()
            << i18n( "Left to Right" )
            << i18n( "Right to Left" )
            << i18n( "Top to Bottom" )
            << i18n( "Bottom to Top" ) );
        hLayout->addStretch( 1 );

        // boxes
        vLayout->addWidget( widget = new QWidget( this ) );
        widget->setLayout( boxLayout = new QBoxLayout( QBoxLayout::LeftToRight ) );
        boxLayout->setMargin(0);

        // groupbox
        QGroupBox* groupBox = new QGroupBox( i18n( "GroupBox" ), widget );
        boxLayout->addWidget( groupBox );
        {
            QVBoxLayout* vLayout = new QVBoxLayout();
            vLayout->setMargin(0);
            groupBox->setLayout( vLayout );
            vLayout->addStretch( 1 );
        }

        // frame
        frame = new QFrame( widget );
        frame->setFrameStyle( QFrame::StyledPanel|QFrame::Raised );
        boxLayout->addWidget( frame );
        QButtonGroup* group = new QButtonGroup( this );

        {
            QVBoxLayout* vLayout = new QVBoxLayout();
            vLayout->setMargin(0);
            frame->setLayout( vLayout );

            QLabel* label;
            vLayout->addWidget( label = new QLabel( i18n("Frame"), frame ) );
            label->setAlignment( Qt::AlignCenter );
            label->setMargin(4);

            QRadioButton* radioButton;
            group->addButton( radioButton = new QRadioButton( i18n( "Raised" ), frame ), 0 );
            vLayout->addWidget( radioButton );
            radioButton->setChecked( true );

            group->addButton( radioButton = new QRadioButton( i18n( "Plain" ), frame ), 1 );
            vLayout->addWidget( radioButton );

            group->addButton( radioButton = new QRadioButton( i18n( "Sunken" ), frame ), 2 );
            vLayout->addWidget( radioButton );

            vLayout->addStretch( 1 );
        }

        // tab widget
        KTabWidget* tabWidget = new KTabWidget( widget );
        tabWidget->addTab( new QWidget(), i18n( "Tab Widget" ) );
        boxLayout->addWidget( tabWidget );

        // connections
        connect( comboBox, SIGNAL( currentIndexChanged( int ) ), SLOT( updateLayoutDirection( int ) ) );
        connect( group, SIGNAL( buttonClicked( int ) ), SLOT( updateFrameStyle( int ) ) );

    }

    //_____________________________________________________________
    void FrameDemoWidget::updateFrameStyle( int value )
    {
        switch( value )
        {
            default: case 0: frame->setFrameStyle( QFrame::StyledPanel|QFrame::Raised ); break;
            case 1: frame->setFrameStyle( QFrame::StyledPanel|QFrame::Plain ); break;
            case 2: frame->setFrameStyle( QFrame::StyledPanel|QFrame::Sunken ); break;
        }
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
        if( direction != boxLayout->direction() )
        {
            boxLayout->setDirection( direction );
            boxLayout->update();
        }

    }

}
