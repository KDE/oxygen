#ifndef oxygenanimations_h
#define oxygenanimations_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimations.h
// container for all animation engines
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenbusyindicatorengine.h"
#include "oxygendockseparatorengine.h"
#include "oxygenheaderviewengine.h"
#include "oxygenmdiwindowengine.h"
#include "oxygenmenubarengine.h"
#include "oxygenmenuengine.h"
#include "oxygenprogressbarengine.h"
#include "oxygenscrollbarengine.h"
#include "oxygenspinboxengine.h"
#include "oxygensplitterengine.h"
#include "oxygentabbarengine.h"
#include "oxygentoolbarengine.h"
#include "oxygentoolboxengine.h"
#include "oxygenwidgetstateengine.h"

#include <QObject>
#include <QList>

namespace Oxygen
{

    //* stores engines
    class Animations: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        explicit Animations( QObject* );

        //* register animations corresponding to given widget, depending on its type.
        void registerWidget( QWidget* widget ) const;

        /** unregister all animations associated to a widget */
        void unregisterWidget( QWidget* widget ) const;

        //* enable state engine
        WidgetStateEngine& widgetEnableStateEngine( void ) const
        { return *_widgetEnableStateEngine; }

        //* abstractButton engine
        WidgetStateEngine& widgetStateEngine( void ) const
        { return *_widgetStateEngine; }

        //* editable combobox arrow hover engine
        WidgetStateEngine& comboBoxEngine( void ) const
        { return *_comboBoxEngine; }

        //* Tool buttons arrow hover engine
        WidgetStateEngine& toolButtonEngine( void ) const
        { return *_toolButtonEngine; }

        //* item view engine
        WidgetStateEngine& inputWidgetEngine( void ) const
        { return *_inputWidgetEngine; }

        //* splitter engine
        SplitterEngine& splitterEngine( void ) const
        { return *_splitterEngine; }

        //* busy indicator
        BusyIndicatorEngine& busyIndicatorEngine( void ) const
        { return *_busyIndicatorEngine; }

        //* dock separators engine
        DockSeparatorEngine& dockSeparatorEngine( void ) const
        { return *_dockSeparatorEngine; }

        //* header view engine
        HeaderViewEngine& headerViewEngine( void ) const
        { return *_headerViewEngine; }

        //* progressbar engine
        ProgressBarEngine& progressBarEngine( void ) const
        { return *_progressBarEngine; }

        //* menubar engine
        MenuBarBaseEngine& menuBarEngine( void ) const
        { return *_menuBarEngine; }

        //* menu engine
        MenuBaseEngine& menuEngine( void ) const
        { return *_menuEngine; }

        //* scrollbar engine
        ScrollBarEngine& scrollBarEngine( void ) const
        { return *_scrollBarEngine; }

        //* spinbox engine
        SpinBoxEngine& spinBoxEngine( void ) const
        { return *_spinBoxEngine; }

        //* tabbar
        TabBarEngine& tabBarEngine( void ) const
        { return *_tabBarEngine; }

        //* toolbar
        ToolBarEngine& toolBarEngine( void ) const
        { return *_toolBarEngine; }

        //* toolbox
        ToolBoxEngine& toolBoxEngine( void ) const
        { return *_toolBoxEngine; }

        //* mdi windows
        MdiWindowEngine& mdiWindowEngine( void ) const
        { return *_mdiWindowEngine; }

        //* setup engines
        void setupEngines( void );

        private Q_SLOTS:

        //* enregister engine
        void unregisterEngine( QObject* );

        private:

        //* register new engine
        void registerEngine( BaseEngine* engine );

        //* busy indicator
        BusyIndicatorEngine* _busyIndicatorEngine = nullptr;

        //* dock separator handle hover effect
        DockSeparatorEngine* _dockSeparatorEngine = nullptr;

        //* headerview hover effect
        HeaderViewEngine* _headerViewEngine = nullptr;

        //* widget enable state engine
        WidgetStateEngine* _widgetEnableStateEngine = nullptr;

        //* abstract button engine
        WidgetStateEngine* _widgetStateEngine = nullptr;

        //* editable combobox arrow hover effect
        WidgetStateEngine* _comboBoxEngine = nullptr;

        //* mennu toolbutton arrow hover effect
        WidgetStateEngine* _toolButtonEngine = nullptr;

        //* item view engine
        WidgetStateEngine* _inputWidgetEngine = nullptr;

        //* QSplitter engine
        SplitterEngine* _splitterEngine = nullptr;

        //* progressbar engine
        ProgressBarEngine* _progressBarEngine = nullptr;

        //* menubar engine
        MenuBarBaseEngine* _menuBarEngine = nullptr;

        //* menu engine
        MenuBaseEngine* _menuEngine = nullptr;

        //* scrollbar engine
        ScrollBarEngine* _scrollBarEngine = nullptr;

        //* spinbox engine
        SpinBoxEngine* _spinBoxEngine = nullptr;

        //* tabbar engine
        TabBarEngine* _tabBarEngine = nullptr;

        //* toolbar engine
        ToolBarEngine* _toolBarEngine = nullptr;

        //* toolbar engine
        ToolBoxEngine* _toolBoxEngine = nullptr;

        //* mdi window
        MdiWindowEngine* _mdiWindowEngine = nullptr;

        //* keep list of existing engines
        QList< BaseEngine::Pointer > _engines;

    };

}

#endif
