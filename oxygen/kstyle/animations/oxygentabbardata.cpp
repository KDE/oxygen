//////////////////////////////////////////////////////////////////////////////
// oxygentabbardata.cpp
// data container for QTabBar animations
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
    TabBarData::TabBarData( QWidget* parent, int duration ):
        WidgetData( parent ),
        currentIndex_( -1 ),
        previousIndex_( -1 ),
        currentIndexAnimation_( new Animation( duration, this ) ),
        previousIndexAnimation_( new Animation( duration, this ) ),
        currentOpacity_( 0 ),
        previousOpacity_( 0 )
    {
        setupAnimation( currentIndexAnimation(), "currentOpacity" );
        setupAnimation( previousIndexAnimation(), "previousOpacity" );

        currentIndexAnimation().data()->setDirection( Animation::Forward );
        previousIndexAnimation().data()->setDirection( Animation::Backward );
    }

    //______________________________________________
    Animation::Pointer TabBarData::animation( const QPoint& position ) const
    {

        if( !enabled() )  return Animation::Pointer();

        const QTabBar* local( qobject_cast<const QTabBar*>( target().data() ) );
        if( !local ) return Animation::Pointer();

        int index( local->tabAt( position ) );
        if( index < 0 ) return Animation::Pointer();
        else if( index == currentIndex() ) return currentIndexAnimation();
        else if( index == previousIndex() ) return previousIndexAnimation();
        else return Animation::Pointer();

    }

    //______________________________________________
    qreal TabBarData::opacity( const QPoint& position ) const
    {

        if( !enabled() ) return OpacityInvalid;

        const QTabBar* local( qobject_cast<const QTabBar*>( target().data() ) );
        if( !local ) return OpacityInvalid;

        int index( local->tabAt( position ) );
        if( index < 0 ) return OpacityInvalid;
        else if( index == currentIndex() ) return currentOpacity();
        else if( index == previousIndex() ) return previousOpacity();
        else return OpacityInvalid;

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

        // otherwise: stop current index animation
        // move current index to previous, and trigger previousIndexAnimation
        if( currentIndexAnimation().data()->isRunning() ) currentIndexAnimation().data()->stop();
        if( currentIndex() >= 0 )
        {

            setPreviousIndex( currentIndex() );
            setCurrentIndex( -1 );
            previousIndexAnimation().data()->restart();

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
                previousIndexAnimation().data()->restart();
            }

            if( currentTabIndex >= 0 )
            {
                setCurrentIndex( currentTabIndex );
                currentIndexAnimation().data()->restart();
            }

        }

        return;

    }

}
