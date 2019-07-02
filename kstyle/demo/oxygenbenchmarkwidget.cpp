//////////////////////////////////////////////////////////////////////////////
// oxygenbenchmarkwidget.cpp
// oxygen buttons demo widget
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

#include "oxygenbenchmarkwidget.h"

#include <QIcon>
#include <QAbstractItemView>

namespace Oxygen
{

    //_______________________________________________
    BenchmarkWidget::BenchmarkWidget( QWidget* parent ):
        DemoWidget( parent )
    {

        // setup ui
        ui.setupUi( this );
        ui.runButton->setIcon( QIcon::fromTheme( QStringLiteral( "system-run" ) ) );
        ui.grabMouseCheckBox->setChecked( Simulator::grabMouse() );
        connect( ui.grabMouseCheckBox, SIGNAL(toggled(bool)), SLOT(updateGrabMouse(bool)) );
        connect( ui.runButton, SIGNAL(clicked()), SLOT(run()) );

    }

    //_______________________________________________
    void BenchmarkWidget::init( KPageDialog* dialog, QVector<KPageWidgetItem*> items )
    {

        _pageDialog = dialog;

        for( auto&& item:items )
        {


            // get header and widget
            auto header = item->header();
            auto demoWidget( qobject_cast<DemoWidget*>( item->widget() ) );
            if( !demoWidget ) continue;

            // do not add oneself to the list
            if( qobject_cast<BenchmarkWidget*>( demoWidget ) ) continue;

            // add checkbox
            QCheckBox* checkbox( new QCheckBox( this ) );
            checkbox->setText( header );

            const bool hasBenchmark( demoWidget->metaObject()->indexOfSlot( "benchmark()" ) >= 0 );
            checkbox->setEnabled( hasBenchmark );
            checkbox->setChecked( hasBenchmark );

            if( hasBenchmark )
            { connect( this, SIGNAL(runBenchmark()), demoWidget, SLOT(benchmark()) ); }

            ui.verticalLayout->addWidget( checkbox );

            _items.append( ItemPair(checkbox, item) );

            connect( checkbox, SIGNAL(toggled(bool)), SLOT(updateButtonState()) );

        }

    }

    //_______________________________________________
    void BenchmarkWidget::updateButtonState( void )
    {
        bool enabled( false );
        for( auto&& item:_items )
        {
            if( item.first->isEnabled() && item.first->isChecked() )
            {
                enabled = true;
                break;
            }
        }

        ui.runButton->setEnabled( enabled );
    }

    //_______________________________________________
    void BenchmarkWidget::run( void )
    {

        // disable button and groupbox
        ui.runButton->setEnabled( false );
        Simulator::setGrabMouse( ui.grabMouseCheckBox->isChecked() );
        for( int index = 0; index < _items.size(); ++index )
        {

            auto item( _items[index] );

            // check state
            if( !( item.first->isEnabled() && item.first->isChecked() ) )
            { continue; }

            if( simulator().aborted() ) return;
            else {

                selectPage( index );
                emit runBenchmark();

            }

        }

        // re-select last page
        selectPage( _items.size() );

        // disable button and groupbox
        ui.runButton->setEnabled( true );
    }

    //_______________________________________________
    void BenchmarkWidget::selectPage( int index ) const
    {

        // check dialog
        if( !_pageDialog ) return;

        // try find item view from pageView
        auto view( _pageDialog.data()->findChild<QAbstractItemView*>() );

        // select in list
        if( view )
        {

            simulator().selectItem( view, index );
            simulator().run();

        }

        return;

    }

}
