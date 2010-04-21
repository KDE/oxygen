//////////////////////////////////////////////////////////////////////////////
// oxygentabdemowidget.cpp
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

#include "oxygentabdemowidget.h"
#include "oxygentabdemowidget.moc"
#include "oxygentabwidget.moc"

#include <QToolButton>

namespace Oxygen
{
    //! constructor
    TabDemoWidget::TabDemoWidget( QWidget* parent ):
        QWidget( parent ),
        left_( new QToolButton(0) ),
        right_( new QToolButton(0) )
    {
        ui.setupUi( this );
        connect( ui.tabPositionComboBox, SIGNAL( currentIndexChanged( int ) ), SLOT( changeTabPosition( int ) ) );
        connect( ui.documentModeCheckBox, SIGNAL( toggled( bool ) ), SLOT( toggleDocumentMode( bool ) ) );
        connect( ui.cornerWidgetsCheckBox, SIGNAL( toggled( bool ) ), SLOT( toggleCornerWidgets( bool ) ) );
        connect( ui.tabBarVisibilityCheckBox, SIGNAL( toggled( bool ) ), ui.tabWidget, SLOT( toggleTabBarVisibility( bool ) ) );

        left_->setIcon( KIcon( "tab-new" ) );
        left_->setVisible( false );

        right_->setIcon( KIcon( "tab-close" ) );
        right_->setVisible( false );

    }


}
