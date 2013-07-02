#ifndef oxygenbenchmarkwidget_h
#define oxygenbenchmarkwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbenchmarkwidget.h
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

#include <QWidget>
#include <QCheckBox>
#include <QPair>
#include <QVector>
#include <QPointer>
#include <KPageWidget>

#include "oxygendemowidget.h"
#include "ui_oxygenbenchmarkwidget.h"

namespace Oxygen
{
    class BenchmarkWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit BenchmarkWidget( QWidget* = 0 );

        //! destructor
        virtual ~BenchmarkWidget( void )
        {}


        //! setup widgets
        void init( KPageWidget* );

        Q_SIGNALS:

        void runBenchmark( void );

        protected Q_SLOTS:

        //! button state
        void updateButtonState( void );

        //! grabMouse
        void updateGrabMouse( bool value )
        { Simulator::setGrabMouse( value ); }

        //! run
        void run( void );

        protected:

        //! select page from index in parent page widget
        void selectPage( int ) const;

        private:

        //! ui
        Ui_BenchmarkWidget ui;

        //! pointer to pagewidget
        QPointer<KPageWidget> _pageWidget;

        //! map checkboxes to demo widgets
        typedef QPointer<DemoWidget> DemoWidgetPointer;
        typedef QPair<QCheckBox*, DemoWidgetPointer> WidgetPair;
        typedef QVector<WidgetPair> WidgetList;
        WidgetList _widgets;

    };

}

#endif
