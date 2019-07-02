#ifndef oxygendemodialog_h
#define oxygendemodialog_h

//////////////////////////////////////////////////////////////////////////////
// oxygendemodialog.h
// oxygen demo dialog
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

#include "oxygendemowidget.h"

#include <KPageDialog>

#include <QList>
#include <QWidget>
#include <QCheckBox>

namespace Oxygen
{

    class ButtonDemoWidget;
    class FrameDemoWidget;
    class InputDemoWidget;
    class ListDemoWidget;
    class MdiDemoWidget;
    class SliderDemoWidget;
    class TabDemoWidget;
    class DemoDialog: public KPageDialog
    {
        Q_OBJECT

        public:

        //* constructor
        explicit DemoDialog( QWidget* parent = nullptr );

        Q_SIGNALS:

        //* emitted when dialog is closed
        void abortSimulations( void );

        protected:

        //* close event
        void closeEvent( QCloseEvent* ) override;

        //* hide event
        void hideEvent( QHideEvent* ) override;

        private Q_SLOTS:

        //* update window title when page is changed
        void updateWindowTitle( KPageWidgetItem* );

        //* update page enability
        void updateEnableState( KPageWidgetItem* );

        //* toggle enable state
        void toggleEnable( bool );

        //* toggle RightToLeft
        void toggleRightToLeft( bool );

        private:

        //* enable state checkbox
        QCheckBox* _enableCheckBox = nullptr;

        //* reverse layout checkbox
        QCheckBox* _rightToLeftCheckBox = nullptr;

    };

}

#endif
