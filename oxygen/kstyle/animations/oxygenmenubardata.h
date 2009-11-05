#ifndef oxygenmenubardata_h
#define oxygenmenubardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenubardata.h
// data container for QMenuBar animations
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

#include "oxygenwidgetdata.h"
#include <QtGui/QMenuBar>
#include <QtCore/QBasicTimer>
#include <QtCore/QPointer>

namespace Oxygen
{

    //! widget index
    enum WidgetIndex
    {
        Current,
        Previous
    };

    //! menubar data
    class MenuBarDataV1: public WidgetData
    {

        Q_OBJECT

        //! declare opacity property
        Q_PROPERTY( qreal currentOpacity READ currentOpacity WRITE setCurrentOpacity )
        Q_PROPERTY( qreal previousOpacity READ previousOpacity WRITE setPreviousOpacity )

        public:

        //! constructor
        MenuBarDataV1( QWidget* parent, int duration );

        //! destructor
        virtual ~MenuBarDataV1( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! timeLines
        virtual const Animation::Pointer& currentAnimation( void ) const
        { return currentAnimation_; }

        //! timeLines
        virtual const Animation::Pointer& previousAnimation( void ) const
        { return previousAnimation_; }

        //! return animation matching given point
        virtual Animation::Pointer animation( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentAnimation();
            else if( previousRect().contains( point ) ) return previousAnimation();
            else return Animation::Pointer();
        }

        //! return animation matching given point
        virtual qreal opacity( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentOpacity();
            else if( previousRect().contains( point ) ) return previousOpacity();
            else return OpacityInvalid;
        }

        // return rect matching QPoint
        virtual QRect currentRect( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentRect();
            else if( previousRect().contains( point ) ) return previousRect();
            else return QRect();
        }

        //! animation associated to given Widget index
        virtual const Animation::Pointer& animation( WidgetIndex index ) const
        { return index == Current ? currentAnimation():previousAnimation(); }

        //! opacity associated to given Widget index
        virtual qreal opacity( WidgetIndex index ) const
        { return index == Current ? currentOpacity():previousOpacity(); }

        //! opacity associated to given Widget index
        virtual const QRect& currentRect( WidgetIndex index ) const
        { return index == Current ? currentRect():previousRect(); }

        //! duration
        virtual void setDuration( int duration )
        {
            currentAnimation().data()->setDuration( duration );
            previousAnimation().data()->setDuration( duration );
        }

        //! current opacity
        virtual qreal currentOpacity( void ) const
        { return currentOpacity_; }

        //! current opacity
        virtual void setCurrentOpacity( qreal value )
        { currentOpacity_ = value; }

        //! current rect
        virtual const QRect& currentRect( void ) const
        { return currentRect_; }

        //! previous opacity
        virtual qreal previousOpacity( void ) const
        { return previousOpacity_; }

        //! previous opacity
        virtual void setPreviousOpacity( qreal value )
        { previousOpacity_ = value; }

        //! previous rect
        virtual const QRect& previousRect( void ) const
        { return previousRect_; }

        protected:

        //!@name current action handling
        //@{

        //! guarded action pointer
        typedef QPointer<QAction> ActionPointer;

        //! current action
        virtual const ActionPointer& currentAction( void ) const
        { return currentAction_; }

        //! current action
        virtual void setCurrentAction( QAction* action )
        { currentAction_ = ActionPointer( action ); }

        //! current action
        virtual void clearCurrentAction( void )
        { currentAction_ = ActionPointer(); }

        //@}

        //!@name rect handling
        //@{

        //! current rect
        virtual void setCurrentRect( const QRect& rect )
        { currentRect_ = rect; }

        //! current rect
        virtual void clearCurrentRect( void )
        { currentRect_ = QRect(); }

        //! previous rect
        virtual void setPreviousRect( const QRect& rect )
        { previousRect_ = rect; }

        //! previous rect
        virtual void clearPreviousRect( void )
        { previousRect_ = QRect(); }

        //@}

        // leave event
        template< typename T > inline void enterEvent( const QObject* object );

        // leave event
        template< typename T > inline void leaveEvent( const QObject* object );

        //! mouse move event
        template< typename T > inline void mouseMoveEvent( const QObject* object );

        //! menubar enterEvent
        virtual void enterEvent( const QObject* object )
        { enterEvent<QMenuBar>( object ); }

        //! menubar enterEvent
        virtual void leaveEvent( const QObject* object )
        { leaveEvent<QMenuBar>( object ); }

        //! menubar mouseMoveEvent
        virtual void mouseMoveEvent( const QObject* object )
        { mouseMoveEvent<QMenuBar>( object ); }

        private:

        //! time line
        Animation::Pointer currentAnimation_;

        //! time line
        Animation::Pointer previousAnimation_;

        //! current opacity
        qreal currentOpacity_;

        //! previous opacity
        qreal previousOpacity_;

        //! current action
        ActionPointer currentAction_;

        // current rect
        QRect currentRect_;

        // previous rect
        QRect previousRect_;

    };

}

#include "oxygenmenubardata_imp.h"
#endif
