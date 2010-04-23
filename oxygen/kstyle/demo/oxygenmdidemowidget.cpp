//////////////////////////////////////////////////////////////////////////////
// oxygenmdidemowidget.cpp
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

#include "oxygenmdidemowidget.h"
#include "oxygenmdidemowidget.moc"

#include <QtGui/QMdiSubWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>
#include <KIcon>

namespace Oxygen
{

    //______________________________________________________________
    MdiDemoWidget::MdiDemoWidget( QWidget* parent ):
        QWidget( parent )
    {
        setLayout( new QVBoxLayout() );
        QMenuBar* menuBar = new QMenuBar( this );
        layout()->addWidget( menuBar );

        QWidget* widget = new QWidget( this );
        layout()->addWidget( widget );
        ui.setupUi( widget );

        QMenu* menu = menuBar->addMenu( "&Layout" );
        QAction* action;
        connect( action = menu->addAction( i18n( "Tile" ) ), SIGNAL( triggered( void ) ), ui.mdiArea, SLOT( tileSubWindows( void ) ) );
        //action->trigger();

        connect( action = menu->addAction( i18n( "Cascade" ) ), SIGNAL( triggered( void ) ), ui.mdiArea, SLOT( cascadeSubWindows( void ) ) );

        menu = menuBar->addMenu( "&Tools" );
        connect( action = menu->addAction( KIcon( "arrow-right" ), i18n( "Select Next Window" ) ), SIGNAL( triggered( void ) ), ui.mdiArea, SLOT( activateNextSubWindow() ) );
        action->setShortcut( Qt::CTRL + Qt::Key_Tab );
        addAction( action );

        connect( action = menu->addAction( KIcon( "arrow-left" ), i18n( "Select Previous Window" ) ), SIGNAL( triggered( void ) ), ui.mdiArea, SLOT( activatePreviousSubWindow() ) );
        action->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_Tab );
        addAction( action );

    }

}
