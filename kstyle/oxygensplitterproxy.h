#ifndef oxygensplitterproxy_h
#define oxygensplitterproxy_h

//////////////////////////////////////////////////////////////////////////////
// oxygensplitterproxy.h
// Extended hit area for Splitters
// -------------------
//
// Copyright (C) 2011 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygen.h"

#include <QEvent>
#include <QMap>
#include <QHoverEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSplitterHandle>
#include <QWidget>

#include "oxygenaddeventfilter.h"

namespace Oxygen
{

    class SplitterProxy;

    //! factory
    class SplitterFactory: public QObject
    {

        public:

        //! constructor
        explicit SplitterFactory( QObject* parent ):
            QObject( parent ),
            _enabled( false )
            {}

        //! destructor
        virtual ~SplitterFactory( void )
        {}

        //! enabled state
        void setEnabled( bool );

        //! register widget
        bool registerWidget( QWidget* );

        //! unregister widget
        void unregisterWidget( QWidget* );

        private:

        //! enabled state
        bool _enabled;

        //! needed to block ChildAdded events when creating proxy
        AddEventFilter _addEventFilter;

        //! pointer to SplitterProxy
        using SplitterProxyPointer = WeakPointer<SplitterProxy>;

        //! registered widgets
        using WidgetMap = QMap<QWidget*, SplitterProxyPointer >;
        WidgetMap _widgets;

    };

    //! splitter 'proxy' widget, with extended hit area
    class SplitterProxy : public QWidget
    {

        public:

        //! constructor
        explicit SplitterProxy( QWidget*, bool = false );

        //! destructor
        virtual ~SplitterProxy( void );

        //! event filter
        bool eventFilter( QObject*, QEvent* ) Q_DECL_OVERRIDE;

        //! enable state
        void setEnabled( bool );

        //! enable state
        bool enabled( void ) const
        { return _enabled; }

        protected:

        //! event handler
        bool event( QEvent* ) Q_DECL_OVERRIDE;

        protected:

        //! reset 'true' splitter widget
        void clearSplitter( void );

        //! keep track of 'true' splitter widget
        void setSplitter( QWidget* );

        private:

        //! enabled state
        bool _enabled;

        //! splitter object
        WeakPointer<QWidget> _splitter;

        //! hook
        QPoint _hook;

        //! timer id
        int _timerId;

    };

}

#endif
