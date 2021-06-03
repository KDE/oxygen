#ifndef oxygenbuttondemowidget_h
#define oxygenbuttondemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbuttondemowidget.h
// oxygen buttons demo widget
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygendemowidget.h"
#include "ui_oxygenbuttondemowidget.h"

#include <QWidget>
#include <QList>
#include <QToolBar>
#include <QToolButton>

namespace Oxygen
{
    class ButtonDemoWidget: public DemoWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ButtonDemoWidget( QWidget* = nullptr );

        public Q_SLOTS:

        void benchmark( void );

        private Q_SLOTS:

        //* change text position in tool buttons
        void textPosition( int );

        //* change tool button icon size
        void iconSize( int );

        //* set buttons as flat
        void toggleFlat( bool );

        private:

        void installMenu( QPushButton* );
        void installMenu( QToolButton* );

        Ui_ButtonDemoWidget ui;
        QToolBar* _toolBar = nullptr;
        QList<QPushButton*> _pushButtons;
        QList<QToolButton*> _toolButtons;
    };

}

#endif
