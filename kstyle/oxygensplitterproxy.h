#ifndef oxygensplitterproxy_h
#define oxygensplitterproxy_h

//////////////////////////////////////////////////////////////////////////////
// oxygensplitterproxy.h
// Extended hit area for Splitters
// -------------------
//
// SPDX-FileCopyrightText: 2011 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Based on Bespin splitterproxy code
// SPDX-FileCopyrightText: 2011 Thomas Luebking <thomas.luebking@web.de>
//
//
// SPDX-License-Identifier: LGPL-2.0-only
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

    //* factory
    class SplitterFactory: public QObject
    {

        public:

        //* constructor
        explicit SplitterFactory( QObject* parent ):
            QObject( parent )
            {}

        //* enabled state
        void setEnabled( bool );

        //* register widget
        bool registerWidget( QWidget* );

        //* unregister widget
        void unregisterWidget( QWidget* );

        private:

        //* enabled state
        bool _enabled = false;

        //* needed to block ChildAdded events when creating proxy
        AddEventFilter _addEventFilter;

        //* pointer to SplitterProxy
        using SplitterProxyPointer = WeakPointer<SplitterProxy>;

        //* registered widgets
        using WidgetMap = QMap<QWidget*, SplitterProxyPointer >;
        WidgetMap _widgets;

    };

    //* splitter 'proxy' widget, with extended hit area
    class SplitterProxy : public QWidget
    {

        public:

        //* constructor
        explicit SplitterProxy( QWidget*, bool = false );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        //* enable state
        void setEnabled( bool );

        //* enable state
        bool enabled( void ) const
        { return _enabled; }

        protected:

        //* event handler
        bool event( QEvent* ) override;

        private:

        //* reset 'true' splitter widget
        void clearSplitter( void );

        //* keep track of 'true' splitter widget
        void setSplitter( QWidget* );

        //* enabled state
        bool _enabled = false;

        //* splitter object
        WeakPointer<QWidget> _splitter;

        //* hook
        QPoint _hook;

        //* timer id
        int _timerId = 0;

    };

}

#endif
