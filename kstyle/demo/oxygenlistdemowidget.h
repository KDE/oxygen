#ifndef oxygenlistdemowidget_h
#define oxygenlistdemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenlistdemowidget.h
// oxygen lists (and trees) demo widget
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QWidget>

#include "oxygendemowidget.h"
#include "ui_oxygenlistdemowidget.h"

namespace Oxygen
{

    class ListDemoWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ListDemoWidget( QWidget* = nullptr );

        public Q_SLOTS:

        //* benchmark
        void benchmark( void );

        private:

        //* ui
        Ui_ListDemoWidget ui;

    };

}

#endif
