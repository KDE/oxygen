#ifndef oxygentabdemowidget_h
#define oxygentabdemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygentabdemowidget.h
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

#include <QtGui/QWidget>
#include <QtGui/QToolButton>

#include "ui_oxygentabdemowidget.h"
#include "oxygendemowidget.h"

namespace Oxygen
{

    class TabDemoWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit TabDemoWidget( QWidget* = 0 );

        //! destructor
        virtual ~TabDemoWidget( void )
        {}

        public slots:

        //! benchmark
        void benchmark( void );

        protected slots:

        //! show/hide corner buttons
        void toggleCornerWidgets( bool value )
        {
            if( value )
            {
                ui.tabWidget->setCornerWidget( _left, Qt::TopLeftCorner );
                ui.tabWidget->setCornerWidget( _right, Qt::TopRightCorner );
            } else {
                ui.tabWidget->setCornerWidget( 0, Qt::TopLeftCorner );
                ui.tabWidget->setCornerWidget( 0, Qt::TopRightCorner );
            }

            _left->setVisible( value );
            _right->setVisible( value );
            ui.tabWidget->adjustSize();

        }

        //! change document mode
        void toggleDocumentMode( bool value )
        { ui.tabWidget->setDocumentMode( value ); }

        // change tab position
        void changeTabPosition( int index )
        {
            switch( index )
            {
                case 1:
                ui.tabWidget->setTabPosition( QTabWidget::South );
                break;

                case 2:
                ui.tabWidget->setTabPosition( QTabWidget::West );
                break;

                case 3:
                ui.tabWidget->setTabPosition( QTabWidget::East );
                break;

                default:
                case 0:
                ui.tabWidget->setTabPosition( QTabWidget::North );
                break;
            }

        }

        // change tab position
        void changeTextPosition( int index )
        {
            switch( index )
            {

                case 0:
                ui.tabWidget->hideText();
                ui.tabWidget->showIcons();
                break;

                case 1:
                ui.tabWidget->showText();
                ui.tabWidget->hideIcons();
                break;

                default:
                case 2:
                ui.tabWidget->showText();
                ui.tabWidget->showIcons();
                break;

            }
        }

        private:

        //! ui
        Ui_TabDemoWidget ui;

        //! tabbar left button
        QToolButton* _left;

        //! tabbar right button
        QToolButton* _right;

    };

}

#endif
