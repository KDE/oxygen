//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbarengine.cpp
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

#include "oxygenscrollbarengine.h"
#include "oxygenscrollbarengine.moc"

#include <QtCore/QEvent>

namespace Oxygen
{

    //____________________________________________________________
    bool ScrollBarEngine::registerWidget( QWidget* widget )
    {

        if( !(enabled() && widget ) ) return false;

        // create new data class
        if( !data_.contains( widget ) ) data_.insert( widget, QPointer<ScrollBarData>( new ScrollBarData( this, widget, maxFrame(), duration() ) ) );

        // connect destruction signal
        disconnect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        connect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        return true;
    }

    //____________________________________________________________
    TimeLine::Pointer ScrollBarEngine::timeLine( const QObject* object, QStyle::SubControl control )
    {

        if( !enabled() ) return TimeLine::Pointer();

        TimeLine::Pointer out;
        if( QPointer<ScrollBarData> data = data_.find( object ) )
        { out = data->timeLine( control ); }

        return _timeLine( out );

    }

    //____________________________________________________________
    QRect ScrollBarEngine::subControlRect( const QObject* object, QStyle::SubControl subcontrol )
    {

        if( !enabled() ) return QRect();

        QRect out;
        if( QPointer<ScrollBarData> data = data_.find( object ) )
        { out = data->subControlRect( subcontrol ); }

        return out;

    }

    //____________________________________________________________
    void ScrollBarEngine::setSubControlRect( const QObject* object, QStyle::SubControl subcontrol, const QRect& rect )
    {

        if( !enabled() ) return;

        if( QPointer<ScrollBarData> data = data_.find( object ) )
        { data->setSubControlRect( subcontrol, rect ); }

        return;

    }

}
