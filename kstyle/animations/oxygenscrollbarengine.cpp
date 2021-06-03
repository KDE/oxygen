//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbarengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenscrollbarengine.h"

#include <QEvent>

namespace Oxygen
{

    //____________________________________________________________
    bool ScrollBarEngine::registerWidget( QWidget* widget )
    {

        // check widget
        /*
        note: widget is registered even if animation is disabled because OxygenScrollBarData
        is also used in non-animated mode to store arrow rect for hover highlight
        */
        if( !widget ) return false;

        // create new data class
        if( !_data.contains( widget ) ) _data.insert( widget, new ScrollBarData( this, widget, duration() ), enabled() );

        // connect destruction signal
        connect( widget, SIGNAL(destroyed(QObject*)), this, SLOT(unregisterWidget(QObject*)), Qt::UniqueConnection );
        return true;
    }


   //____________________________________________________________
    bool ScrollBarEngine::isAnimated( const QObject* object, QStyle::SubControl control )
    {

        if( DataMap<ScrollBarData>::Value data = _data.find( object ) )
        {
            if( Animation::Pointer animation = data.data()->animation( control ) ) return animation.data()->isRunning();

        }

        return false;

    }

}
