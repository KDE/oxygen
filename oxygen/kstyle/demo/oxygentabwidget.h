#ifndef oxygentabwidget_h
#define oxygentabwidget_h

#include <KTabWidget>
#include <KTabBar>

namespace Oxygen
{
    class TabWidget: public KTabWidget
    {

        Q_OBJECT

        public:

        TabWidget( QWidget* parent ):
        KTabWidget( parent )
        {}

        public slots:

        // toggle tabbar visibility
        void toggleTabBarVisibility( bool value )
        { if( tabBar() ) tabBar()->setVisible( !value ); }

    };

}

#endif
