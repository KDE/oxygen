//////////////////////////////////////////////////////////////////////////////
// oxygenmdidemowidget.cpp
// oxygen mdi windows demo widget
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmdidemowidget.h"

#include <QMdiSubWindow>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>

namespace Oxygen
{

    //______________________________________________________________
    MdiDemoWidget::MdiDemoWidget( QWidget* parent ):
        DemoWidget( parent )
    {
        setLayout( new QVBoxLayout() );
#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
        layout()->setMargin(0);
#else
        layout()->setContentsMargins(0, 0, 0, 0);
#endif
        QMenuBar* menuBar = new QMenuBar( this );
        layout()->addWidget( menuBar );

        QWidget* widget = new QWidget( this );
        layout()->addWidget( widget );
        ui.setupUi( widget );

        QMenu* menu = menuBar->addMenu( i18n( "Layout" ) );
        connect( menu->addAction( i18n( "Tile" ) ), SIGNAL(triggered()), this, SLOT(setLayoutTiled()) );
        connect( menu->addAction( i18n( "Cascade" ) ), SIGNAL(triggered()), this, SLOT(setLayoutCascade()) );
        connect( menu->addAction( i18n( "Tabs" ) ), SIGNAL(triggered()), this, SLOT(setLayoutTabbed()) );

        menu = menuBar->addMenu( i18n( "Tools" ) );
        QAction* action;
        connect( action = menu->addAction( QIcon::fromTheme( QStringLiteral( "arrow-right" ) ), i18n( "Select Next Window" ) ), SIGNAL(triggered()), ui.mdiArea, SLOT(activateNextSubWindow()) );
        action->setShortcut( Qt::CTRL | Qt::Key_Tab );
        addAction( action );

        connect( action = menu->addAction( QIcon::fromTheme( QStringLiteral( "arrow-left" ) ), i18n( "Select Previous Window" ) ), SIGNAL(triggered()), ui.mdiArea, SLOT(activatePreviousSubWindow()) );
        action->setShortcut( Qt::CTRL | Qt::SHIFT | Qt::Key_Tab );
        addAction( action );

    }

    //______________________________________________________________
    void MdiDemoWidget::setLayoutTiled( void )
    {
        ui.mdiArea->setViewMode( QMdiArea::SubWindowView );
        ui.mdiArea->tileSubWindows();
    }

    //______________________________________________________________
    void MdiDemoWidget::setLayoutCascade( void )
    {
        ui.mdiArea->setViewMode( QMdiArea::SubWindowView );
        ui.mdiArea->cascadeSubWindows();
    }

    //______________________________________________________________
    void MdiDemoWidget::setLayoutTabbed( void )
    { ui.mdiArea->setViewMode( QMdiArea::TabbedView ); }

    //______________________________________________________________
    void MdiDemoWidget::benchmark( void )
    {
        if( !isVisible() ) return;

        if( true )
        {
            // slide windows
            const auto children = ui.mdiArea->findChildren<QMdiSubWindow*>();
            for ( QMdiSubWindow *window : children )
            {
                simulator().click( window );
                simulator().slide( window, QPoint( 20, 20 ) );
                simulator().slide( window, QPoint( -20, -20 ) );
            }

        }

        if( true )
        {
            const auto children = ui.toolBox->findChildren<QAbstractButton*>();

            for ( QAbstractButton* button : children )
            { simulator().click( button ); }

            for ( QAbstractButton *button : children )
            { simulator().click( button ); }
        }

        simulator().run();

    }
}
