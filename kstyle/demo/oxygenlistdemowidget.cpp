//////////////////////////////////////////////////////////////////////////////
// oxygenlistdemowidget.cpp
// oxygen lists (and trees) demo widget
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

#include "oxygenlistdemowidget.h"

namespace Oxygen
{

    //______________________________________________________________
    ListDemoWidget::ListDemoWidget( QWidget* parent ):
        DemoWidget( parent )
    {
        ui.setupUi( this );
        ui.treeWidget->sortByColumn( 0, Qt::AscendingOrder );
    }

    //______________________________________________________________
    void ListDemoWidget::benchmark( void )
    {
        if( !isVisible() ) return;

        if( true )
        {
            simulator().selectItem( ui.listWidget, 0 );
            simulator().selectItem( ui.listWidget, 1 );
            simulator().selectItem( ui.listWidget, 2 );
        }

        if( true )
        {
            simulator().selectItem( ui.treeWidget, 0, 0 );
            simulator().selectItem( ui.treeWidget, 1, 0 );
            simulator().selectItem( ui.treeWidget, 2, 0 );
        }

        if( true )
        {

            simulator().selectItem( ui.tableWidget, 0, 0 );
            simulator().selectItem( ui.tableWidget, 0, 1 );
            simulator().selectItem( ui.tableWidget, 0, 2 );

            simulator().selectItem( ui.tableWidget, 1, 0 );
            simulator().selectItem( ui.tableWidget, 1, 1 );
            simulator().selectItem( ui.tableWidget, 1, 2 );

            simulator().selectItem( ui.tableWidget, 2, 0 );
            simulator().selectItem( ui.tableWidget, 2, 1 );
            simulator().selectItem( ui.tableWidget, 2, 2 );
        }

        if( true )
        {
            QSplitterHandle* handle( ui.splitter->handle(1) );
            simulator().slide( handle, QPoint( 0, -20 ) );
            simulator().slide( handle, QPoint( 0, 20 ) );

            handle = ui.splitter->handle(2);
            simulator().slide( handle, QPoint( 0, 20 ) );
            simulator().slide( handle, QPoint( 0, -20 ) );
        }

        simulator().run();

    }

}
