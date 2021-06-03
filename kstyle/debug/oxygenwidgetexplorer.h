#ifndef oxygenwidgetexplorer_h
#define oxygenwidgetexplorer_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetexplorer.h
// print widget's and parent's information on mouse click
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QEvent>
#include <QObject>
#include <QMap>
#include <QSet>
#include <QWidget>

namespace Oxygen
{

    //* print widget's and parent's information on mouse click
    class WidgetExplorer: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        explicit WidgetExplorer( QObject* );

        //* enable
        bool enabled( void ) const;

        //* enable
        void setEnabled( bool );

        //* widget rects
        void setDrawWidgetRects( bool value )
        { _drawWidgetRects = value; }

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        private:

        //* event type
        QString eventType( const QEvent::Type& ) const;

        //* print widget information
        QString widgetInformation( const QWidget* ) const;

        //* enable state
        bool _enabled = false;

        //* widget rects
        bool _drawWidgetRects = false;

        //* map event types to string
        QMap<QEvent::Type, QString > _eventTypes;

    };

}

#endif
