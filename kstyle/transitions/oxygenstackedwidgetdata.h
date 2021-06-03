#ifndef oxygenstackedwidget_datah
#define oxygenstackedwidget_datah

//////////////////////////////////////////////////////////////////////////////
// oxygenstackedwidgetdata.h
// data container for QStackedWidget transition
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygentransitiondata.h"

#include <QStackedWidget>

namespace Oxygen
{

    //* generic data
    class StackedWidgetData: public TransitionData
    {

        Q_OBJECT

        public:

        //* constructor
        StackedWidgetData( QObject*, QStackedWidget*, int );

        protected Q_SLOTS:

        //* initialize animation
        bool initializeAnimation( void ) override;

        //* animate
        bool animate( void ) override;

        private Q_SLOTS:

        //* finish animation
        void finishAnimation( void );

        //* called when target is destroyed
        void targetDestroyed( void );

        private:

        //* target
        WeakPointer<QStackedWidget> _target;

        //* current index
        int _index = 0;

    };

}

#endif
