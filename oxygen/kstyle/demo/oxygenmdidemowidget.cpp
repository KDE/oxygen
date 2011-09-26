//////////////////////////////////////////////////////////////////////////////
// oxygenmdidemowidget.cpp
// oxygen mdi windows demo widget
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
        DemoWidget( parent )
    {
        setLayout( new QVBoxLayout() );
        QMenuBar* menuBar = new QMenuBar( this );
        layout()->addWidget( menuBar );

        QWidget* widget = new QWidget( this );
        layout()->addWidget( widget );
        ui.setupUi( widget );

        QMenu* menu = menuBar->addMenu( "&Layout" );
        QAction* action;
        connect( action = menu->addAction( i18n( "Tile" ) ), SIGNAL(triggered()), ui.mdiArea, SLOT(tileSubWindows()) );
        //action->trigger();

        connect( action = menu->addAction( i18n( "Cascade" ) ), SIGNAL(triggered()), ui.mdiArea, SLOT(cascadeSubWindows()) );

        menu = menuBar->addMenu( "&Tools" );
        connect( action = menu->addAction( KIcon( "arrow-right" ), i18n( "Select Next Window" ) ), SIGNAL(triggered()), ui.mdiArea, SLOT(activateNextSubWindow()) );
        action->setShortcut( Qt::CTRL + Qt::Key_Tab );
        addAction( action );

        connect( action = menu->addAction( KIcon( "arrow-left" ), i18n( "Select Previous Window" ) ), SIGNAL(triggered()), ui.mdiArea, SLOT(activatePreviousSubWindow()) );
        action->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_Tab );
        addAction( action );

    }

    //______________________________________________________________
    void MdiDemoWidget::benchmark( void )
    {
        if( !isVisible() ) return;

        if( true )
        {
            // slide windows
            foreach( QMdiSubWindow* window, ui.mdiArea->findChildren<QMdiSubWindow*>() )
            {
                simulator().click( window );
                simulator().slide( window, QPoint( 20, 20 ) );
                simulator().slide( window, QPoint( -20, -20 ) );
            }

        }

        if( true )
        {
            foreach( QAbstractButton* button, ui.toolBox->findChildren<QAbstractButton*>() )
            { simulator().click( button ); }

            foreach( QAbstractButton* button, ui.toolBox->findChildren<QAbstractButton*>() )
            { simulator().click( button ); }
        }

        simulator().run();

    }
}
