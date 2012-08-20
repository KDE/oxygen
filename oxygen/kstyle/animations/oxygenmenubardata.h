#ifndef oxygenmenubar_datah
#define oxygenmenubar_datah

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
    class MenuBarData: public AnimationData
    {

        Q_OBJECT

        public:

        //! constructor
        MenuBarData( QObject* parent, QWidget* target );

        //! destructor
        virtual ~MenuBarData( void )
        {}

        protected:

        bool _isMenu;
        int _motions;

    };

    //! menubar data
    class MenuBarDataV1: public MenuBarData
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

        //! animations
        virtual const Animation::Pointer& currentAnimation( void ) const
        { return _current._animation; }

        //! animations
        virtual const Animation::Pointer& previousAnimation( void ) const
        { return _previous._animation; }

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
        { return _current._opacity; }

        //! current opacity
        virtual void setCurrentOpacity( qreal value )
        {
            value = digitize( value );
            if( _current._opacity == value ) return;
            _current._opacity = value;
            setDirty();
        }

        //! current rect
        virtual const QRect& currentRect( void ) const
        { return _current._rect; }

        //! previous opacity
        virtual qreal previousOpacity( void ) const
        { return _previous._opacity; }

        //! previous opacity
        virtual void setPreviousOpacity( qreal value )
        {
            value = digitize( value );
            if( _previous._opacity == value ) return;
            _previous._opacity = value;
            setDirty();
        }

        //! previous rect
        virtual const QRect& previousRect( void ) const
        { return _previous._rect; }

        protected:

        //!@name current action handling
        //@{

        //! guarded action pointer
        typedef QWeakPointer<QAction> ActionPointer;

        //! current action
        virtual const ActionPointer& currentAction( void ) const
        { return _currentAction; }

        //! current action
        virtual void setCurrentAction( QAction* action )
        { _currentAction = ActionPointer( action ); }

        //! current action
        virtual void clearCurrentAction( void )
        { _currentAction = ActionPointer(); }

        //@}

        //!@name rect handling
        //@{

        //! current rect
        virtual void setCurrentRect( const QRect& rect )
        { _current._rect = rect; }

        //! current rect
        virtual void clearCurrentRect( void )
        { _current._rect = QRect(); }

        //! previous rect
        virtual void setPreviousRect( const QRect& rect )
        { _previous._rect = rect; }

        //! previous rect
        virtual void clearPreviousRect( void )
        { _previous._rect = QRect(); }

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
                _opacity(0)
                {}

            Animation::Pointer _animation;
            qreal _opacity;
            QRect _rect;
        };

        //! current tab animation data (for hover enter animations)
        Data _current;

        //! previous tab animations data (for hover leave animations)
        Data _previous;

        //! current action
        ActionPointer _currentAction;

    };


    //! menubar data
    class MenuBarDataV2: public MenuBarData
    {

        Q_OBJECT
        Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )
        Q_PROPERTY( qreal progress READ progress  WRITE setProgress )

        public:

        //! constructor
        MenuBarDataV2( QObject* parent, QWidget* target, int duration );

        //! destructor
        virtual ~MenuBarDataV2( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! return animation associated to action at given position, if any
        virtual const Animation::Pointer& animation( void ) const
        { return _animation; }

        //! return animation associated to action at given position, if any
        virtual const Animation::Pointer& progressAnimation( void ) const
        { return _progressAnimation; }

        //! duration
        virtual void setDuration( int duration )
        { animation().data()->setDuration( duration ); }

        //! duration
        virtual void setFollowMouseDuration( int duration )
        { progressAnimation().data()->setDuration( duration ); }

        //! return 'hover' rect position when widget is animated
        virtual const QRect& animatedRect( void ) const
        { return _animatedRect; }

        //! current rect
        virtual const QRect& currentRect( void ) const
        { return _currentRect; }

        //! timer
        const QBasicTimer& timer( void ) const
        { return _timer; }

        //! animation opacity
        virtual qreal opacity( void ) const
        { return _opacity; }

        //! animation opacity
        virtual void setOpacity( qreal value )
        {
            value = digitize( value );
            if( _opacity == value ) return;
            _opacity = value;
            setDirty();
        }

        //! animation progress
        virtual qreal progress( void ) const
        { return _progress; }

        //! animation progress
        virtual void setProgress( qreal value )
        {
            value = digitize( value );
            if( _progress == value ) return;
            _progress = value;
            updateAnimatedRect();
        }

        protected:

        virtual void setEntered( bool value )
        { _entered = value; }

        //! animated rect
        virtual void clearAnimatedRect( void )
        { _animatedRect = QRect(); }

        //! updated animated rect
        virtual void updateAnimatedRect( void );

        //! timer event
        virtual void timerEvent( QTimerEvent* );

        //!@name current action handling
        //@{

        //! guarded action pointer
        typedef QWeakPointer<QAction> ActionPointer;

        //! current action
        virtual const ActionPointer& currentAction( void ) const
        { return _currentAction; }

        //! current action
        virtual void setCurrentAction( QAction* action )
        { _currentAction = ActionPointer( action ); }

        //! current action
        virtual void clearCurrentAction( void )
        { _currentAction = ActionPointer(); }

        //@}

        //!@name rect handling
        //@{

        //! current rect
        virtual void setCurrentRect( const QRect& rect )
        { _currentRect = rect; }

        //! current rect
        virtual void clearCurrentRect( void )
        { _currentRect = QRect(); }

        //! previous rect
        virtual const QRect& previousRect( void ) const
        { return _previousRect; }

        //! previous rect
        virtual void setPreviousRect( const QRect& rect )
        { _previousRect = rect; }

        //! previous rect
        virtual void clearPreviousRect( void )
        { _previousRect = QRect(); }

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

        //! fade animation
        Animation::Pointer _animation;

        //! progress animation
        Animation::Pointer _progressAnimation;

        //! opacity
        qreal _opacity;

        //! opacity
        qreal _progress;

        //! timer
        /*! this allows to add some delay before starting leaveEvent animation */
        QBasicTimer _timer;

        //! current action
        ActionPointer _currentAction;

        // current rect
        QRect _currentRect;

        // previous rect
        QRect _previousRect;

        // animated rect
        QRect _animatedRect;

        //! true if toolbar was entered at least once (this prevents some initialization glitches)
        bool _entered;

    };
}

#include "oxygenmenubardata_imp.h"
#endif
