//////////////////////////////////////////////////////////////////////////////
// oxygentabdemowidget.cpp
// oxygen tabwidget demo widget
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

#include "oxygentabdemowidget.h"
#include "oxygentabdemowidget.moc"
#include "oxygentabwidget.moc"

#include <QtGui/QToolButton>
#include <KIcon>

namespace Oxygen
{

    //______________________________________________________________
    TabDemoWidget::TabDemoWidget( QWidget* parent ):
        DemoWidget( parent ),
        _left( new QToolButton(0) ),
        _right( new QToolButton(0) )
    {
        ui.setupUi( this );
        connect( ui.tabPositionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(changeTabPosition(int)) );
        connect( ui.textPositionComboBox, SIGNAL(currentIndexChanged(int)), SLOT(changeTextPosition(int)) );
        connect( ui.documentModeCheckBox, SIGNAL(toggled(bool)), SLOT(toggleDocumentMode(bool)) );
        connect( ui.cornerWidgetsCheckBox, SIGNAL(toggled(bool)), SLOT(toggleCornerWidgets(bool)) );
        connect( ui.tabBarVisibilityCheckBox, SIGNAL(toggled(bool)), ui.tabWidget, SLOT(toggleTabBarVisibility(bool)) );
        ui.textPositionComboBox->setCurrentIndex( 1 );

        _left->setIcon( KIcon( "tab-new" ) );
        _left->setVisible( false );

        _right->setIcon( KIcon( "tab-close" ) );
        _right->setVisible( false );

    }

    //______________________________________________________________
    void TabDemoWidget::benchmark( void )
    {
        if( !isVisible() ) return;

        if( true )
        {
            simulator().selectComboBoxItem( ui.tabPositionComboBox, 1 );
            simulator().selectComboBoxItem( ui.tabPositionComboBox, 2 );
            simulator().selectComboBoxItem( ui.tabPositionComboBox, 3 );
            simulator().selectComboBoxItem( ui.tabPositionComboBox, 0 );

            simulator().selectComboBoxItem( ui.textPositionComboBox, 0 );
            simulator().selectComboBoxItem( ui.textPositionComboBox, 2 );
            simulator().selectComboBoxItem( ui.textPositionComboBox, 1 );
        }

        if( true )
        {

            simulator().click( ui.documentModeCheckBox );
            simulator().click( ui.documentModeCheckBox );

            simulator().click( ui.cornerWidgetsCheckBox );
            simulator().click( ui.cornerWidgetsCheckBox );

            simulator().click( ui.tabBarVisibilityCheckBox );
            simulator().click( ui.tabBarVisibilityCheckBox );
        }

        if( true )
        {
            simulator().selectTab( ui.tabWidget, 1 );
            simulator().selectTab( ui.tabWidget, 2 );
            simulator().selectTab( ui.tabWidget, 3 );
            simulator().selectTab( ui.tabWidget, 0 );
        }

        // run
        simulator().run();

    }

}
