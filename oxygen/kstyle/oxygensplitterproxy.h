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

#include "oxygenaddeventfilter.h"

namespace Oxygen
{

    //! factory
    class SplitterFactory: public QObject
    {

        public:

        //! constructor
        SplitterFactory( QObject* parent ):
            QObject( parent )
            {}

        //! destructor
        virtual ~SplitterFactory( void )
        {}

        //! register widget
        bool registerWidget( QWidget* );

        private:

        //! needed to block ChildAdded events when creating proxy
        AddEventFilter _addEventFilter;

    };

    //! splitter 'proxy' widget, with extended hit area
    class SplitterProxy : public QWidget
    {

        public:

        //! constructor
        SplitterProxy( QWidget*, QWidget* );

        //! destructor
        virtual ~SplitterProxy( void );

        protected:

        //! event handler
        virtual bool event( QEvent* );

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        protected:

        // set enabled
        void setEnabled( bool );

        private:

        //! needed to block ChildAdded events when reparenting
        AddEventFilter _addEventFilter;

        //! splitter object
        QWidget *_splitter;

        //! hook
        QPoint _hook;

    };

}

#endif
