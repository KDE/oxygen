#ifndef oxygenbuttondemowidget_h
#define oxygenbuttondemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbuttondemowidget.h
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

#include <QtGui/QWidget>
#include <QtGui/QToolButton>
#include <QtCore/QList>
#include <KToolBar>

#include "oxygendemowidget.h"
#include "ui_oxygenbuttondemowidget.h"

namespace Oxygen
{
    class ButtonDemoWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit ButtonDemoWidget( QWidget* = 0 );

        //! destructor
        virtual ~ButtonDemoWidget( void )
        {}

        public slots:

        void benchmark( void );

        protected slots:

        //! change text position in tool buttons
        void textPosition( int );

        //! change tool button icon size
        void iconSize( int );

        //! set buttons as flat
        void toggleFlat( bool );

        protected:

        void installMenu( QPushButton* );
        void installMenu( QToolButton* );

        private:

        Ui_ButtonDemoWidget ui;
        KToolBar* _toolBar;
        QList<QPushButton*> _pushButtons;
        QList<QToolButton*> _toolButtons;
    };

}

#endif
