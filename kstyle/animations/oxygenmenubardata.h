#ifndef oxygenmenubar_data_h
#define oxygenmenubar_data_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmenubardata.h
// data container for QMenuBar animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenanimationdata.h"
#include "oxygen.h"

#include <QMenuBar>
#include <QBasicTimer>

namespace Oxygen
{

    //* widget index
    enum WidgetIndex
    {
        Current,
        Previous
    };

    //* menubar data
    class MenuBarData: public AnimationData
    {

        Q_OBJECT

        public:

        //* constructor
        MenuBarData( QObject* parent, QWidget* target );

        protected:

        bool _isMenu = false;
        int _motions = -1;

    };

    //* menubar data
    class MenuBarDataV1: public MenuBarData
    {

        Q_OBJECT

        //* declare opacity property
        Q_PROPERTY( qreal currentOpacity READ currentOpacity WRITE setCurrentOpacity )
        Q_PROPERTY( qreal previousOpacity READ previousOpacity WRITE setPreviousOpacity )

        public:

        //* constructor
        MenuBarDataV1( QObject* parent, QWidget* target, int duration );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        //* animations
        const Animation::Pointer& currentAnimation( void ) const
        { return _current._animation; }

        //* animations
        const Animation::Pointer& previousAnimation( void ) const
        { return _previous._animation; }

        //* return animation matching given point
        Animation::Pointer animation( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentAnimation();
            else if( previousRect().contains( point ) ) return previousAnimation();
            else return Animation::Pointer();
        }

        //* return animation matching given point
        qreal opacity( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentOpacity();
            else if( previousRect().contains( point ) ) return previousOpacity();
            else return OpacityInvalid;
        }

        // return rect matching QPoint
        QRect currentRect( const QPoint& point ) const
        {
            if( currentRect().contains( point ) ) return currentRect();
            else if( previousRect().contains( point ) ) return previousRect();
            else return QRect();
        }

        //* animation associated to given Widget index
        const Animation::Pointer& animation( WidgetIndex index ) const
        { return index == Current ? currentAnimation():previousAnimation(); }

        //* opacity associated to given Widget index
        qreal opacity( WidgetIndex index ) const
        { return index == Current ? currentOpacity():previousOpacity(); }

        //* opacity associated to given Widget index
        const QRect& currentRect( WidgetIndex index ) const
        { return index == Current ? currentRect():previousRect(); }

        //* duration
        void setDuration( int duration ) override
        {
            currentAnimation().data()->setDuration( duration );
            previousAnimation().data()->setDuration( duration );
        }

        //* current opacity
        qreal currentOpacity( void ) const
        { return _current._opacity; }

        //* current opacity
        void setCurrentOpacity( qreal value )
        {
            value = digitize( value );
            if( _current._opacity == value ) return;
            _current._opacity = value;
            setDirty();
        }

        //* current rect
        const QRect& currentRect( void ) const
        { return _current._rect; }

        //* previous opacity
        qreal previousOpacity( void ) const
        { return _previous._opacity; }

        //* previous opacity
        void setPreviousOpacity( qreal value )
        {
            value = digitize( value );
            if( _previous._opacity == value ) return;
            _previous._opacity = value;
            setDirty();
        }

        //* previous rect
        const QRect& previousRect( void ) const
        { return _previous._rect; }

        protected:

        //*@name current action handling
        //@{

        //* guarded action pointer
        using ActionPointer = WeakPointer<QAction>;

        //* current action
        virtual const ActionPointer& currentAction( void ) const
        { return _currentAction; }

        //* current action
        virtual void setCurrentAction( QAction* action )
        { _currentAction = ActionPointer( action ); }

        //* current action
        virtual void clearCurrentAction( void )
        { _currentAction = ActionPointer(); }

        //@}

        //*@name rect handling
        //@{

        //* current rect
        virtual void setCurrentRect( const QRect& rect )
        { _current._rect = rect; }

        //* current rect
        virtual void clearCurrentRect( void )
        { _current._rect = QRect(); }

        //* previous rect
        virtual void setPreviousRect( const QRect& rect )
        { _previous._rect = rect; }

        //* previous rect
        virtual void clearPreviousRect( void )
        { _previous._rect = QRect(); }

        //@}

        // leave event
        template< typename T > inline void enterEvent( const QObject* object );

        // leave event
        template< typename T > inline void leaveEvent( const QObject* object );

        //* mouse move event
        template< typename T > inline void mouseMoveEvent( const QObject* object );

        //* mouse move event
        template< typename T > inline void mousePressEvent( const QObject* object );

        //* menubar enterEvent
        virtual void enterEvent( const QObject* object )
        { enterEvent<QMenuBar>( object ); }

        //* menubar enterEvent
        virtual void leaveEvent( const QObject* object )
        { leaveEvent<QMenuBar>( object ); }

        //* menubar mouseMoveEvent
        virtual void mouseMoveEvent( const QObject* object )
        { mouseMoveEvent<QMenuBar>( object ); }

        //* menubar mousePressEvent
        virtual void mousePressEvent( const QObject* object )
        { mousePressEvent<QMenuBar>( object ); }

        private:

        //* container for needed animation data
        class Data
        {
            public:

            //* default constructor
            Data( void ):
                _opacity(0)
                {}

            Animation::Pointer _animation;
            qreal _opacity;
            QRect _rect;
        };

        //* current tab animation data (for hover enter animations)
        Data _current;

        //* previous tab animations data (for hover leave animations)
        Data _previous;

        //* current action
        ActionPointer _currentAction;

    };


    //* menubar data
    class MenuBarDataV2: public MenuBarData
    {

        Q_OBJECT
        Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )
        Q_PROPERTY( qreal progress READ progress  WRITE setProgress )

        public:

        //* constructor
        MenuBarDataV2( QObject* parent, QWidget* target, int duration );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        //* return animation associated to action at given position, if any
        virtual const Animation::Pointer& animation( void ) const
        { return _animation; }

        //* return animation associated to action at given position, if any
        virtual const Animation::Pointer& progressAnimation( void ) const
        { return _progressAnimation; }

        //* duration
        void setDuration( int duration ) override
        { animation().data()->setDuration( duration ); }

        //* duration
        virtual void setFollowMouseDuration( int duration )
        { progressAnimation().data()->setDuration( duration ); }

        //* return 'hover' rect position when widget is animated
        virtual const QRect& animatedRect( void ) const
        { return _animatedRect; }

        //* current rect
        virtual const QRect& currentRect( void ) const
        { return _currentRect; }

        //* timer
        const QBasicTimer& timer( void ) const
        { return _timer; }

        //* animation opacity
        virtual qreal opacity( void ) const
        { return _opacity; }

        //* animation opacity
        virtual void setOpacity( qreal value )
        {
            value = digitize( value );
            if( _opacity == value ) return;
            _opacity = value;
            setDirty();
        }

        //* animation progress
        virtual qreal progress( void ) const
        { return _progress; }

        //* animation progress
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

        //* animated rect
        virtual void clearAnimatedRect( void )
        { _animatedRect = QRect(); }

        //* updated animated rect
        virtual void updateAnimatedRect( void );

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        //*@name current action handling
        //@{

        //* guarded action pointer
        using ActionPointer = WeakPointer<QAction>;

        //* current action
        virtual const ActionPointer& currentAction( void ) const
        { return _currentAction; }

        //* current action
        virtual void setCurrentAction( QAction* action )
        { _currentAction = ActionPointer( action ); }

        //* current action
        virtual void clearCurrentAction( void )
        { _currentAction = ActionPointer(); }

        //@}

        //*@name rect handling
        //@{

        //* current rect
        virtual void setCurrentRect( const QRect& rect )
        { _currentRect = rect; }

        //* current rect
        virtual void clearCurrentRect( void )
        { _currentRect = QRect(); }

        //* previous rect
        virtual const QRect& previousRect( void ) const
        { return _previousRect; }

        //* previous rect
        virtual void setPreviousRect( const QRect& rect )
        { _previousRect = rect; }

        //* previous rect
        virtual void clearPreviousRect( void )
        { _previousRect = QRect(); }

        //@}

        // leave event
        template< typename T > inline void enterEvent( const QObject* object );

        // leave event
        template< typename T > inline void leaveEvent( const QObject* object );

        //* mouse move event
        template< typename T > inline void mouseMoveEvent( const QObject* object );

        //* menubar enterEvent
        virtual void enterEvent( const QObject* object )
        { enterEvent<QMenuBar>( object ); }

        //* menubar enterEvent
        virtual void leaveEvent( const QObject* object )
        { leaveEvent<QMenuBar>( object ); }

        //* menubar mouseMoveEvent
        virtual void mouseMoveEvent( const QObject* object )
        { mouseMoveEvent<QMenuBar>( object ); }

        private:

        //* fade animation
        Animation::Pointer _animation;

        //* progress animation
        Animation::Pointer _progressAnimation;

        //* opacity
        qreal _opacity = 0;

        //* opacity
        qreal _progress = 0;

        //* timer
        /** this allows to add some delay before starting leaveEvent animation */
        QBasicTimer _timer;

        //* current action
        ActionPointer _currentAction;

        // current rect
        QRect _currentRect;

        // previous rect
        QRect _previousRect;

        // animated rect
        QRect _animatedRect;

        //* true if toolbar was entered at least once (this prevents some initialization glitches)
        bool _entered = false;

    };
}

#include "oxygenmenubardata_imp.h"
#endif
