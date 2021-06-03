#ifndef oxygenaddeventfilter_h
#define oxygenaddeventfilter_h

//////////////////////////////////////////////////////////////////////////////
// oxygenaddeventfilter.h
// used to block add a child to a widget, blocking AddChild parent events
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Largely inspired from BeSpin style
// SPDX-FileCopyrightText: 2007 Thomas Luebking <thomas.luebking@web.de>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QObject>
#include <QEvent>

namespace Oxygen
{

    class AddEventFilter: public QObject
    {

        public:

        //* constructor
        AddEventFilter( void ):
            QObject()
            {}

        //* event filter
        /*! blocks all AddChild events */
        bool eventFilter( QObject*, QEvent* event ) override
        { return event->type() == QEvent::ChildAdded; }

    };

}

#endif
