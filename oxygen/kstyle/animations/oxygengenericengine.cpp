
//////////////////////////////////////////////////////////////////////////////
// oxygengenericengine.h
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

#include "oxygengenericengine.h"
#include "oxygengenericengine.moc"

namespace Oxygen
{

    //____________________________________________________________
    bool GenericEngine::registerWidget( QWidget* widget, unsigned int mode )
    {

        if( !( enabled() && widget ) ) return false;
        if( mode&AnimationHover && !hoverData_.contains( widget ) ) { hoverData_.insert( widget, QPointer<HoverData>( new HoverData( this, widget, maxFrame(), duration() ) ) ); }
        if( mode&AnimationFocus && !focusData_.contains( widget ) ) { focusData_.insert( widget, QPointer<FocusData>( new FocusData( this, widget, maxFrame(), duration() ) ) ); }
        if( mode&AnimationEnable && !enableData_.contains( widget ) ) { enableData_.insert( widget, QPointer<EnableData>( new EnableData( this, widget, maxFrame(), duration() ) ) ); }

        // connect destruction signal
        disconnect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );
        connect( widget, SIGNAL( destroyed( QObject* ) ), this, SLOT( unregisterWidget( QObject* ) ) );

        return true;

    }

    //____________________________________________________________
    TimeLine::Pointer GenericEngine::timeLine( const QObject* object, AnimationMode mode )
    {

        if( !enabled() ) return TimeLine::Pointer();

        TimeLine::Pointer out;
        switch( mode )
        {
            case AnimationHover:
            {
                if( QPointer<HoverData> data = hoverData_.find( object ) )
                { out = data->timeLine(); }
                break;
            }

            case AnimationFocus:
            {
                if( QPointer<FocusData> data = focusData_.find( object ) )
                { out = data->timeLine(); }
                break;
            }

            case AnimationEnable:
            {
                if( QPointer<EnableData> data = enableData_.find( object ) )
                { out = data->timeLine(); }
                break;
            }

            default: break;

        }

        return _timeLine( out );

    }

}

