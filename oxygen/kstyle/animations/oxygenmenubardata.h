#ifndef oxygenmenubardata_h
#define oxygenmenubardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenubardata.h
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

#include "oxygenwidgetdata.h"
#include <QtGui/QMenuBar>
#include <QtCore/QBasicTimer>

namespace Oxygen
{

    //! menubar data
    class MenuBarDataV1: public WidgetData
    {

        Q_OBJECT

        public:

        //! constructor
        MenuBarDataV1( QObject* parent, QWidget* target, int maxFrame, int duration );

        //! destructor
        virtual ~MenuBarDataV1( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! timeLines
        const TimeLine::Pointer& currentTimeLine( void ) const
        { return currentTimeLine_; }

        //! timeLines
        const TimeLine::Pointer& previousTimeLine( void ) const
        { return previousTimeLine_; }

        //! duration
        virtual void setDuration( int duration )
        {
            currentTimeLine_->setDuration( duration );
            previousTimeLine_->setDuration( duration );
        }

        //! maxFrame
        virtual void setMaxFrame( int maxFrame )
        {
            previousTimeLine_->setFrameRange( 0, maxFrame );
            currentTimeLine_->setFrameRange( 0, maxFrame );
        }

        //! current rect
        virtual const QRect& currentRect( void ) const
        { return currentRect_; }

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
        TimeLine::Pointer currentTimeLine_;

        //! time line
        TimeLine::Pointer previousTimeLine_;

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
