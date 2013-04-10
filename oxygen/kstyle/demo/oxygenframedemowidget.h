#ifndef oxygenframedemowidget_h
#define oxygenframedemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenframedemowidget.h
// oxygen frames demo widget
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
#include <QtGui/QBoxLayout>
#include <QtGui/QFrame>

#include "oxygendemowidget.h"
#include "ui_oxygenframedemowidget.h"

namespace Oxygen
{
    class FrameDemoWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit FrameDemoWidget( QWidget* = 0 );

        //! destructor
        virtual ~FrameDemoWidget( void )
        {}

        protected slots:

        //! groupbox
        void toggleFlatGroupBox( bool value )
        { ui.groupBox->setFlat( value ); }

        //! frame style
        void toggleRaisedFrame( bool value )
        { if( value ) ui.frame->setFrameStyle( QFrame::StyledPanel|QFrame::Raised ); }

        void togglePlainFrame( bool value )
        { if( value ) ui.frame->setFrameStyle( QFrame::StyledPanel|QFrame::Plain ); }

        void toggleSunkenFrame( bool value )
        { if( value ) ui.frame->setFrameStyle( QFrame::StyledPanel|QFrame::Sunken ); }

        //! layout direction
        void updateLayoutDirection( int );

        public slots:

        // benchmarking
        void benchmark( void );

        private:

        Ui_FrameDemoWidget ui;

    };

}

#endif
