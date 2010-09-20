#ifndef oxygentabwidget_h
#define oxygentabwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygendemodialog.h
// oxygen demo dialog
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

#include <KTabWidget>
#include <KTabBar>
#include <KIcon>

namespace Oxygen
{
    class TabWidget: public KTabWidget
    {

        Q_OBJECT

        public:

        TabWidget( QWidget* parent ):
        KTabWidget( parent )
        {

            // make tabs movable, for further debugging
            tabBar()->setMovable( true );

        }


        void adjustTabBarSize( void )
        {  if( tabBar() ) tabBar()->adjustSize(); }

        //! show icons
        void showIcons( void )
        {
            // add icons to tabs
            tabBar()->setTabIcon( 0, KIcon( "document-open-folder" ) );
            tabBar()->setTabIcon( 1, KIcon( "document-open-folder" ) );
            tabBar()->setTabIcon( 2, KIcon( "document-open-folder" ) );
            tabBar()->setTabIcon( 3, KIcon( "document-open-folder" ) );
        }

        void hideIcons( void )
        {
            // add icons to tabs
            tabBar()->setTabIcon( 0, QIcon() );
            tabBar()->setTabIcon( 1, QIcon() );
            tabBar()->setTabIcon( 2, QIcon() );
            tabBar()->setTabIcon( 3, QIcon() );
        }

        void showText( void )
        {
            tabBar()->setTabText( 0, "First Tab" );
            tabBar()->setTabText( 1, "Second Tab" );
            tabBar()->setTabText( 2, "Third Tab" );
            tabBar()->setTabText( 3, "Fourth Tab" );
        }

        void hideText( void )
        {
            tabBar()->setTabText( 0, "" );
            tabBar()->setTabText( 1, "" );
            tabBar()->setTabText( 2, "" );
            tabBar()->setTabText( 3, "" );
        }

        public slots:

        // toggle tabbar visibility
        void toggleTabBarVisibility( bool value )
        { if( tabBar() ) tabBar()->setVisible( !value ); }

    };

}

#endif
