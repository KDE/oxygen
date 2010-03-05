//////////////////////////////////////////////////////////////////////////////
// oxygenmenuengine.cpp
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmenuengine.h"
#include "oxygenmenuengine.moc"

#include <QtCore/QEvent>

namespace Oxygen
{

    //____________________________________________________________
    bool MenuEngineV1::registerWidget( QWidget* widget )
    {

        if( !( enabled() && widget ) ) return false;

        // create new data class
        if( !data_.contains( widget ) ) data_.insert( widget, new MenuDataV1( this, widget, duration() ) );

        // connect destruction signal
        disconnect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        connect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        return true;
    }

   //____________________________________________________________
    bool MenuEngineV1::isAnimated( const QObject* object, WidgetIndex index )
    {
        DataMap<MenuDataV1>::Value data( data_.find( object ) );
        if( !data )
        {
            return false;
        }

        if( Animation::Pointer animation = data.data()->animation( index ) ) {

            return animation.data()->isRunning();

        } else return false;
    }

    //____________________________________________________________
    bool MenuEngineV2::registerWidget( QWidget* widget )
    {

        if( !( enabled() && widget ) ) return false;

        // create new data class
        if( !data_.contains( widget ) ) data_.insert( widget, new MenuDataV2( this, widget, duration() ) );

        // connect destruction signal
        disconnect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        connect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        return true;
    }


    //____________________________________________________________
    QRect MenuEngineV2::currentRect( const QObject* object, WidgetIndex )
    {
        if( !enabled() ) return QRect();
        DataMap<MenuDataV2>::Value data( data_.find( object ) );
        return data ? data.data()->currentRect():QRect();

    }

    //____________________________________________________________
    bool MenuEngineV2::isAnimated( const QObject* object, WidgetIndex index )
    {
        DataMap<MenuDataV2>::Value data( data_.find( object ) );
        if( !data )
        {
            return false;
        }

        if( Animation::Pointer animation = data.data()->animation() ) {

            switch( index )
            {
                case Oxygen::Previous:
                return animation.data()->direction() == Animation::Backward && animation.data()->isRunning();

                case Oxygen::Current:
                return animation.data()->direction() == Animation::Forward && animation.data()->isRunning();

            }

        } else return false;
    }

    //____________________________________________________________
    QRect MenuEngineV2::animatedRect( const QObject* object )
    {
        if( !enabled() ) return QRect();
        DataMap<MenuDataV2>::Value data( data_.find( object ) );
        return data ? data.data()->animatedRect():QRect();

    }

    //____________________________________________________________
    bool MenuEngineV2::isTimerActive( const QObject* object )
    {
        if( !enabled() ) return false;
        DataMap<MenuDataV2>::Value data( data_.find( object ) );
        return data ? data.data()->timer().isActive():false;

    }

}
