//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetdata.cpp
// stores event filters and maps tabbars to timelines for animations
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

#include "oxygentabbardata.h"
#include "oxygentabbardata.moc"

#include <QtGui/QHoverEvent>
#include <QtGui/QTabBar>

namespace Oxygen
{

    //______________________________________________
    TabBarData::TabBarData( QObject* parent, QWidget* target, int maxFrame, int duration ):
        WidgetData( parent, target ),
        currentIndex_( -1 ),
        previousIndex_( -1 ),
        currentIndexTimeLine_( new TimeLine( duration, this ) ),
        previousIndexTimeLine_( new TimeLine( duration, this ) )
    {

        // setup timeLines
        currentIndexTimeLine()->setFrameRange( 0, maxFrame );
        currentIndexTimeLine()->setCurveShape( QTimeLine::EaseInOutCurve );
        currentIndexTimeLine()->setDirection( QTimeLine::Forward );
        connect( currentIndexTimeLine().data(), SIGNAL( frameChanged( int ) ), SLOT( setDirty( void ) ) );
        connect( currentIndexTimeLine().data(), SIGNAL( finished() ), SLOT( setDirty( void ) ) );

        // setup timeLines
        previousIndexTimeLine()->setFrameRange( 0, maxFrame );
        previousIndexTimeLine()->setCurveShape( QTimeLine::EaseInOutCurve );
        previousIndexTimeLine()->setDirection( QTimeLine::Backward );
        connect( previousIndexTimeLine().data(), SIGNAL( frameChanged( int ) ), SLOT( setDirty( void ) ) );
        connect( previousIndexTimeLine().data(), SIGNAL( finished() ), SLOT( setDirty( void ) ) );

    }

    //______________________________________________
    TimeLine::Pointer TabBarData::timeLine( const QObject* object, const QPoint& position ) const
    {

        if( !enabled() )  return TimeLine::Pointer();

        const QTabBar* local( qobject_cast<const QTabBar*>( object ) );
        if( !local ) return TimeLine::Pointer();

        int index( local->tabAt( position ) );
        if( index < 0 ) return TimeLine::Pointer();
        else if( index == currentIndex() ) return currentIndexTimeLine();
        else if( index == previousIndex() ) return previousIndexTimeLine();
        else return TimeLine::Pointer();

    }

    //____________________________________________________________
    bool TabBarData::eventFilter( QObject* object, QEvent* event )
    {

        if( !enabled() ) return false;

        // check event type
        switch( event->type() )
        {

            case QEvent::HoverEnter:
            {
                enterEvent( object );
                return false;
            }

            case QEvent::HoverLeave:
            {
                leaveEvent( object );
                return false;
            }

            case QEvent::HoverMove:
            {
                mouseMoveEvent( object, static_cast<QHoverEvent*>( event )->pos() );
                return false;
            }

            default: break;

        }

        // always forward event
        return false;

    }


    //________________________________________________________________________
    void TabBarData::enterEvent( const QObject* )
    { return; }

    //________________________________________________________________________
    void TabBarData::leaveEvent( const QObject* )
    {

        // otherwise: stop current index timeLine
        // move current index to previous, and trigger previousIndexTimeLine
        if( currentIndexTimeLine()->isRunning() ) currentIndexTimeLine()->stop();
        if( currentIndex() >= 0 )
        {

            setPreviousIndex( currentIndex() );
            setCurrentIndex( -1 );
            previousIndexTimeLine()->restart();

        }

        return;

    }

    //________________________________________________________________________
    void TabBarData::mouseMoveEvent( const QObject* object, const QPoint& position )
    {

        const QTabBar* local = qobject_cast<const QTabBar*>( object );
        if( !local ) return;

        int currentTabIndex( local->tabAt( position ) );
        if( currentTabIndex == currentIndex() ) return;

        if( currentTabIndex >= 0 || !local->documentMode() )
        {

            if( currentIndex() >= 0 )
            {
                setPreviousIndex( currentIndex() );
                setCurrentIndex( -1 );
                previousIndexTimeLine()->restart();
            }

            if( currentTabIndex >= 0 )
            {
                setCurrentIndex( currentTabIndex );
                currentIndexTimeLine()->restart();
            }

        }

        return;

    }

}
