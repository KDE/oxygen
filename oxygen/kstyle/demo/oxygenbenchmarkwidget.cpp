//////////////////////////////////////////////////////////////////////////////
// oxygenbenchmarkwidget.cpp
// oxygen buttons demo widget
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

#include "oxygenbenchmarkwidget.h"
#include "oxygenbenchmarkwidget.moc"

#include <KIcon>
#include <QtGui/QAbstractItemView>

namespace Oxygen
{

    //_______________________________________________
    BenchmarkWidget::BenchmarkWidget( QWidget* parent ):
        DemoWidget( parent )
    {

        // setup ui
        ui.setupUi( this );
        ui.runButton->setIcon( KIcon( "system-run" ) );
        ui.grabMouseCheckBox->setChecked( Simulator::grabMouse() );
        connect( ui.grabMouseCheckBox, SIGNAL(toggled(bool)), SLOT(updateGrabMouse(bool)) );
        connect( ui.runButton, SIGNAL(clicked()), SLOT(run()) );

    }

    //_______________________________________________
    void BenchmarkWidget::init( KPageWidget* pageWidget )
    {

        _pageWidget = pageWidget;

        // load widgets from pageWidget
        const QAbstractItemModel* model( pageWidget->model() );
        if( !model ) return;

        for( int i=0; i<model->rowCount(); ++i )
        {;
            QModelIndex index( model->index( i, 0 ) );
            if( !index.isValid() ) continue;

            // get header and widget
            QString header = model->data( index, Qt::DisplayRole ).toString();
            QWidget* widget( qvariant_cast<QWidget*>( model->data( index, KPageModel::WidgetRole ) ) );
            DemoWidget* demoWidget( qobject_cast<DemoWidget*>( widget ) );
            if( !demoWidget ) continue;

            // do not add oneself to the list
            if( qobject_cast<BenchmarkWidget*>( widget ) ) continue;

            // add checkbox
            QCheckBox* checkbox( new QCheckBox( this ) );
            checkbox->setText( header );

            const bool hasBenchmark( demoWidget->metaObject()->indexOfSlot( "benchmark()" ) >= 0 );
            checkbox->setEnabled( hasBenchmark );
            checkbox->setChecked( hasBenchmark );

            if( hasBenchmark )
            { connect( this, SIGNAL(runBenchmark()), demoWidget, SLOT(benchmark()) ); }

            ui.verticalLayout->addWidget( checkbox );

            _widgets.push_back( WidgetPair(checkbox, demoWidget) );

            connect( checkbox, SIGNAL(toggled(bool)), SLOT(updateButtonState()) );

        }

    }

    //_______________________________________________
    void BenchmarkWidget::updateButtonState( void )
    {
        bool enabled( false );
        foreach( const WidgetPair& widget, _widgets )
        {
            if( widget.first->isEnabled() && widget.first->isChecked() )
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

        // check pagewidget
        if( !_pageWidget ) return;

        // disable button and groupbox
        ui.runButton->setEnabled( false );
        Simulator::setGrabMouse( ui.grabMouseCheckBox->isChecked() );
        for( int i=0; i < _widgets.size(); ++i )
        {

            // check state
            if( !( _widgets[i].first->isEnabled() && _widgets[i].first->isChecked() ) )
            { continue; }

            if( simulator().aborted() ) return;
            else {
                selectPage( i );
                emit runBenchmark();
            }

        }

        // re-select last page
        selectPage( _widgets.size() );

        // disable button and groupbox
        ui.runButton->setEnabled( true );
    }

    //_______________________________________________
    void BenchmarkWidget::selectPage( int index ) const
    {

        // check pagewidget
        if( !_pageWidget ) return;

        // try find item view from pageView
        QAbstractItemView* view(0);
        foreach( QObject* object, _pageWidget.data()->children() )
        {
            if( (view = qobject_cast<QAbstractItemView*>( object )) )
            { break; }
        }

        // select in list
        if( view )
        {

            simulator().selectItem( view, index );
            simulator().run();

        } else if( QAbstractItemModel* model = _pageWidget.data()->model() ) {

            _pageWidget.data()->KPageView::setCurrentPage( model->index( index, 0 ) );

        }

        return;

    }

}
