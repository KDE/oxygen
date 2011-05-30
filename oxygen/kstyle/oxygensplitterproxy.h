#ifndef oxygensplitterproxy_h
#define oxygensplitterproxy_h

//////////////////////////////////////////////////////////////////////////////
// oxygensplitterproxy.h
// Extended hit area for Splitters
// -------------------
//
// Copyright (C) 2011 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Based on Bespin splitterproxy code
// Copyright (C) 2011 Thomas Luebking <thomas.luebking@web.de>
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
//////////////////////////////////////////////////////////////////////////////

#include <QtCore/QEvent>
#include <QtGui/QHoverEvent>
#include <QtGui/QMainWindow>
#include <QtGui/QMouseEvent>
#include <QtGui/QSplitterHandle>
#include <QtGui/QWidget>

namespace Oxygen
{

    //! splitter 'proxy' widget, with extended hit area
    class SplitterProxy : public QWidget
    {

        public:

        //! constructor
        SplitterProxy( void );

        //! destructor
        virtual ~SplitterProxy( void )
        {}

        //! add widget on which splitter proxy is to be added
        bool registerWidget( QWidget* );

        protected:

        //! event handler
        virtual bool event( QEvent* );

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        protected:

        // store splitter
        void setSplitter( QWidget* );

        private:

        //! child addition event filter
        class ChildAddEventFilter : public QObject
        {

            public:

            //! constructor
            ChildAddEventFilter( QObject* parent ):
                QObject( parent )
                {}

            //! destructor
            virtual ~ChildAddEventFilter( void )
            {}

            //! event filter
            virtual bool eventFilter( QObject*, QEvent *event )
            // { return (ev->type() == QEvent::ChildAdded || ev->type() == QEvent::ChildInserted); }
            { return event->type() == QEvent::ChildAdded; }

        };

        //! child add event filter
        ChildAddEventFilter* _childAddEventFilter;

        //! splitter object
        QWidget *_splitter;

        //! hook
        QPoint _hook;

    };

}

#endif
