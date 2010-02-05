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

#include "oxygenanimationdata.h"
#include <QtGui/QMenuBar>
#include <QtCore/QBasicTimer>
#include <QtCore/QWeakPointer>

namespace Oxygen
{

    //! widget index
    enum WidgetIndex
    {
        Current,
        Previous
    };

    //! menubar data
    class MenuBarDataV1: public AnimationData
    {

        Q_OBJECT

        //! declare opacity property
        Q_PROPERTY( qreal currentOpacity READ currentOpacity WRITE setCurrentOpacity )
        Q_PROPERTY( qreal previousOpacity READ previousOpacity WRITE setPreviousOpacity )

        public:

        //! constructor
        MenuBarDataV1( QObject* parent, QWidget* target, int duration );

        //! destructor
        virtual ~MenuBarDataV1( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! timeLines
        virtual const Animation::Pointer& currentAnimation( void ) const
        { return current_.animation_; }

        //! timeLines
        virtual const Animation::Pointer& previousAnimation( void ) const
        { return previous_.animation_; }

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
        { return current_.opacity_; }

        //! current opacity
        virtual void setCurrentOpacity( qreal value )
        { current_.opacity_ = value; }

        //! current rect
        virtual const QRect& currentRect( void ) const
        { return current_.rect_; }

        //! previous opacity
        virtual qreal previousOpacity( void ) const
        { return previous_.opacity_; }

        //! previous opacity
        virtual void setPreviousOpacity( qreal value )
        { previous_.opacity_ = value; }

        //! previous rect
        virtual const QRect& previousRect( void ) const
        { return previous_.rect_; }

        protected:

        //!@name current action handling
        //@{

        //! guarded action pointer
        typedef QWeakPointer<QAction> ActionPointer;

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
        { current_.rect_ = rect; }

        //! current rect
        virtual void clearCurrentRect( void )
        { current_.rect_ = QRect(); }

        //! previous rect
        virtual void setPreviousRect( const QRect& rect )
        { previous_.rect_ = rect; }

        //! previous rect
        virtual void clearPreviousRect( void )
        { previous_.rect_ = QRect(); }

        //@}

        // leave event
        template< typename T > inline void enterEvent( const QObject* object );

        // leave event
        template< typename T > inline void leaveEvent( const QObject* object );

        //! mouse move event
        template< typename T > inline void mouseMoveEvent( const QObject* object );

        //! mouse move event
        template< typename T > inline void mousePressEvent( const QObject* object );

        //! menubar enterEvent
        virtual void enterEvent( const QObject* object )
        { enterEvent<QMenuBar>( object ); }

        //! menubar enterEvent
        virtual void leaveEvent( const QObject* object )
        { leaveEvent<QMenuBar>( object ); }

        //! menubar mouseMoveEvent
        virtual void mouseMoveEvent( const QObject* object )
        { mouseMoveEvent<QMenuBar>( object ); }

        //! menubar mousePressEvent
        virtual void mousePressEvent( const QObject* object )
        { mousePressEvent<QMenuBar>( object ); }

        private:

        //! container for needed animation data
        class Data
        {
            public:

            //! default constructor
            Data( void ):
                opacity_(0)
                {}

            Animation::Pointer animation_;
            qreal opacity_;
            QRect rect_;
        };

        //! current tab animation data (for hover enter animations)
        Data current_;

        //! previous tab animations data (for hover leave animations)
        Data previous_;

        //! current action
        ActionPointer currentAction_;

    };

}

#include "oxygenmenubardata_imp.h"
#endif
