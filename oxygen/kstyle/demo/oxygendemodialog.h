#ifndef oxygendemodialog_h
#define oxygendemodialog_h

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

#include <KDialog>
#include <KPageWidget>
#include <KPushButton>
#include <QtGui/QWidget>
#include <QtGui/QCheckBox>

#include "ui_oxygenlistdemowidget.h"

namespace Oxygen
{

    class ButtonDemoWidget;
    class FrameDemoWidget;
    class InputDemoWidget;
    class MdiDemoWidget;
    class SliderDemoWidget;
    class TabDemoWidget;
    class DemoDialog: public KDialog
    {
        Q_OBJECT

        public:

        //! constructor
        explicit DemoDialog( QWidget* parent = 0 );

        //! destructor
        virtual ~DemoDialog( void )
        {}

        protected slots:

        //! update window title when page is changed
        virtual void updateWindowTitle( KPageWidgetItem* );

        //! update page enability
        virtual void updateEnableState( KPageWidgetItem* );

        //! toggle enable state
        virtual void toggleEnable( bool );

        //! toggle RightToLeft
        virtual void toggleRightToLeft( bool );

        private:

        //! main paged widget
        KPageWidget* _pageWidget;

        //! button widgets
        ButtonDemoWidget* buttonDemoWidget_;

        //! input widgets
        InputDemoWidget* _inputDemoWidget;

        //! input widgets
        FrameDemoWidget* _frameDemoWidget;

        //! tab widget
        TabDemoWidget* _tabDemoWidget;

        //! tab widget
        SliderDemoWidget* _sliderDemoWidget;

        //! mdi
        MdiDemoWidget* _mdiDemoWidget;

        //! list widgets ui
        Ui_ListDemoWidget _listDemoWidgetUi;

        //! benchmark buton
        KPushButton* _benchmarkButton;

        //! enable state checkbox
        QCheckBox* _enableCheckBox;

        //! reverse layout checkbox
        QCheckBox* _rightToLeftCheckBox;

    };

}

#endif
