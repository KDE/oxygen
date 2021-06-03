#ifndef oxygenwindowmanager_h
#define oxygenwindowmanager_h

/*************************************************************************
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *************************************************************************/

#include "oxygen.h"
#include "config-liboxygen.h"

#include <QEvent>

#include <QBasicTimer>
#include <QObject>
#include <QQuickItem>
#include <QSet>
#include <QString>
#include <QWidget>

#if OXYGEN_HAVE_KWAYLAND
namespace KWayland
{
namespace Client
{
    class Pointer;
    class Seat;
}
}
#endif

namespace Oxygen
{

    class WindowManager: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        explicit WindowManager( QObject* );

        //* initialize
        /** read relevant options from config */
        void initialize( void );

        //* register widget
        void registerWidget( QWidget* );

        //* register quick item
        void registerQuickItem( QQuickItem* );

        //* unregister widget
        void unregisterWidget( QWidget* );

        //* event filter [reimplemented]
        bool eventFilter( QObject*, QEvent* ) override;

        protected:

        //* timer event,
        /** used to start drag if button is pressed for a long enough time */
        void timerEvent( QTimerEvent* ) override;

        private:

        //* mouse press event
        bool mousePressEvent( QObject*, QEvent* );

        //* mouse move event
        bool mouseMoveEvent( QObject*, QEvent* );

        //* mouse release event
        bool mouseReleaseEvent( QObject*, QEvent* );

        //*@name configuration
        //@{

        //* enable state
        bool enabled( void ) const
        { return _enabled; }

        //* enable state
        void setEnabled( bool value )
        { _enabled = value; }

        //* returns true if window manager is used for moving
        bool useWMMoveResize( void ) const
        { return supportWMMoveResize() && _useWMMoveResize; }

        //* use window manager for moving, when available
        void setUseWMMoveResize( bool value )
        { _useWMMoveResize = value; }

        //* drag mode
        int dragMode( void ) const
        { return _dragMode; }

        //* drag mode
        void setDragMode( int value )
        { _dragMode = value; }

        //* drag distance (pixels)
        void setDragDistance( int value )
        { _dragDistance = value; }

        //* drag delay (msec)
        void setDragDelay( int value )
        { _dragDelay = value; }

        //* set list of whiteListed widgets
        /**
        white list is read from options and is used to adjust
        per-app window dragging issues
        */
        void initializeWhiteList();

        //* set list of blackListed widgets
        /**
        black list is read from options and is used to adjust
        per-app window dragging issues
        */
        void initializeBlackList( void );

        //* initializes the Wayland specific parts
        void initializeWayland();

        //* The Wayland Seat's hasPointer property changed
        void waylandHasPointerChanged( bool hasPointer );

        //@}

        //* returns true if widget is dragable
        bool isDragable( QWidget* );

        //* returns true if widget is dragable
        bool isBlackListed( QWidget* );

        //* returns true if widget is dragable
        bool isWhiteListed( QWidget* ) const;

        //* returns true if drag can be started from current widget
        bool canDrag( QWidget* );

        //* returns true if drag can be started from current widget and position
        /** child at given position is passed as second argument */
        bool canDrag( QWidget*, QWidget*, const QPoint& );

        //* reset drag
        void resetDrag( void );

        //* start drag
        void startDrag( QWindow*, const QPoint& );

        //* X11 specific implementation for startDrag
        void startDragX11( QWindow*, const QPoint& );

        //* Wayland specific implementation for startDrag
        void startDragWayland( QWindow*, const QPoint& );

        //* returns true if window manager is used for moving
        /** right now this is true only for X11 */
        bool supportWMMoveResize( void ) const;

        //* utility function
        bool isDockWidgetTitle( const QWidget* ) const;

        //*@name lock
        //@{

        void setLocked( bool value )
        { _locked = value; }

        //* lock
        bool isLocked( void ) const
        { return _locked; }

        //@}

        //* returns first widget matching given class, or 0L if none
        template<typename T> T findParent( const QWidget* ) const;

        //* enability
        bool _enabled;

        //* use WM moveResize
        bool _useWMMoveResize;

        //* drag mode
        int _dragMode;

        //* drag distance
        /** this is copied from kwin::geometry */
        int _dragDistance;

        //* drag delay
        /** this is copied from kwin::geometry */
        int _dragDelay;

        //* wrapper for exception id
        class ExceptionId: public QPair<QString, QString>
        {
            public:

            //* constructor
            explicit ExceptionId( const QString& value )
            {
                const QStringList args( value.split( QChar::fromLatin1( '@' ) ) );
                if( args.isEmpty() ) return;
                second = args[0].trimmed();
                if( args.size()>1 ) first = args[1].trimmed();
            }

            const QString& appName( void ) const
            { return first; }

            const QString& className( void ) const
            { return second; }

        };

        //* exception set
        using ExceptionSet = QSet<ExceptionId>;

        //* list of white listed special widgets
        /**
        it is read from options and is used to adjust
        per-app window dragging issues
        */
        ExceptionSet _whiteList;

        //* list of black listed special widgets
        /**
        it is read from options and is used to adjust
        per-app window dragging issues
        */
        ExceptionSet _blackList;

        //* drag point
        QPoint _dragPoint;
        QPoint _globalDragPoint;

        //* drag timer
        QBasicTimer _dragTimer;

        //* target being dragged
        /** Weak pointer is used in case the target gets deleted while drag is in progress */
        WeakPointer<QWidget> _target;

        WeakPointer<QQuickItem> _quickTarget;

        //* true if drag is about to start
        bool _dragAboutToStart;

        //* true if drag is in progress
        bool _dragInProgress;

        //* true if drag is locked
        bool _locked;

        //* cursor override
        /** used to keep track of application cursor being overridden when dragging in non-WM mode */
        bool _cursorOverride;

        //* application event filter
        QObject* _appEventFilter;

        #if OXYGEN_HAVE_KWAYLAND
        //* The Wayland seat object which needs to be passed to move requests.
        KWayland::Client::Seat* _seat;
        //* The Wayland pointer object where we get pointer events on.
        KWayland::Client::Pointer* _pointer;
        //* latest searial which needs to be passed to the move requests.
        quint32 _waylandSerial;
        #endif

        //* allow access of all private members to the app event filter
        friend class AppEventFilter;

    };

    //____________________________________________________________________
    template<typename T>
        T WindowManager::findParent( const QWidget* widget ) const
    {

        if( !widget ) return 0L;
        for( QWidget* parent = widget->parentWidget(); parent; parent = parent->parentWidget() )
        { if( T cast = qobject_cast<T>(parent) ) return cast; }

        return 0L;
    }

}

#endif
