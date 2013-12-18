#ifndef oxygensliderdemowidget_h
#define oxygensliderdemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygensliderdemowidget.h
// oxygen sliders demo widget
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
#include <QtGui/QProgressBar>
#include <QtGui/QCheckBox>

#include "oxygendemowidget.h"
#include "ui_oxygensliderdemowidget.h"

namespace Oxygen
{

    class ProgressBar: public QObject
    {
        Q_OBJECT

        public:

        //! constructor
        ProgressBar( QObject*, QProgressBar*, QCheckBox* );

        //! set value
        void setValue( int );

        protected slots:

        //! toggle bussy state
        void toggleBusy( bool );

        private:

        //! progressBar
        QProgressBar* _progressBar;

        //! checkbox
        QCheckBox* _checkBox;

        //! saved value
        int _value;

    };

    class SliderDemoWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit SliderDemoWidget( QWidget* = 0 );

        //! destructor
        virtual ~SliderDemoWidget( void )
        {}

        public slots:

        //! benchmark
        void benchmark( void );

        protected slots:

        void updateSliders( int );

        private:

        bool _locked;

        Ui_SliderDemoWidget ui;

        //! progressbars
        ProgressBar* _progressBar1;
        ProgressBar* _progressBar2;

    };

}

#endif
