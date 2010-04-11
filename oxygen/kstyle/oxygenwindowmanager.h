#ifndef oxygenwindowmanager_h
#define oxygenwindowmanager_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwindowmanager.h
// pass some window mouse press/release/move event actions to window manager
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Largely inspired from BeSpin window decoration
// Copyright (C) 2007 Thomas Luebking <thomas.luebking@web.de>
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

#include <QtCore/QEvent>

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>
#include <QtCore/QBasicTimer>
#include <QtGui/QWidget>

namespace Oxygen
{

    class WindowManager: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        explicit WindowManager( QObject* );

        //! destructor
        virtual ~WindowManager( void )
        {}

        //! enability
        bool enabled( void ) const
        { return enabled_; }

        //! enability
        void setEnabled( bool value )
        { enabled_ = value; }

        //! returns true if window manager is used for moving
        bool useWMMoveResize( void ) const
        { return supportWMMoveResize() && useWMMoveResize_; }

        //! use window manager for moving, when available
        void setUseWMMoveResize( bool value )
        { useWMMoveResize_ = value; }

        //! drag mode
        int dragMode( void ) const
        { return dragMode_; }

        //! drag mode
        void setDragMode( int value )
        { dragMode_ = value; }

        //! register widget
        void registerWidget( QWidget* );

        //! unregister widget
        void unregisterWidget( QWidget* );

        //! event filter [reimplemented]
        virtual bool eventFilter( QObject*, QEvent* );

        protected:

        //! timer event,
        /*! used to start drag if button is pressed for a long enough time */
        void timerEvent( QTimerEvent* );

        //! application-wise event.
        /*! needed to catch end of XMoveResize events */
        bool appMouseEvent( QObject*, QEvent* );

        //! mouse press event
        bool mousePressEvent( QObject*, QEvent* );

        //! mouse move event
        bool mouseMoveEvent( QObject*, QEvent* );

        //! mouse release event
        bool mouseReleaseEvent( QObject*, QEvent* );

        //! returns true if widget is dragable
        bool isDragable( QWidget* ) const;

        //! returns true if widget is dragable
        bool isBlackListed( QWidget* ) const;

        //! returns true if drag can be started from current widget and position
        bool canDrag( QWidget*, const QPoint& );

        //! reset drag
        void resetDrag( void );

        //! start drag
        void startDrag( QWidget*, const QPoint& );

        //! tag event as blacklisted
        void setEventBlackListed( QEvent* event )
        { blackListEvent_ = event; }

        //! true if event is blacklisted
        bool isEventBlackListed( QEvent* event ) const
        { return event == blackListEvent_; }

        //! clear blackListed event
        void clearBlackListedEvent( void )
        { blackListEvent_ = NULL; }

        //! returns true if window manager is used for moving
        /*! right now this is true only for X11 */
        bool supportWMMoveResize( void ) const;

        private:

        //! enability
        bool enabled_;

        //! use WM moveResize
        bool useWMMoveResize_;

        //! drag mode
        bool dragMode_;

        //! drag distance
        /*! this is copied from kwin::geometry */
        int dragDistance_;

        //! drag delay
        /*! this is copied from kwin::geometry */
        int dragDelay_;

        //! drag point
        QPoint dragPoint_;
        QPoint globalDragPoint_;

        //! drag timer
        QBasicTimer dragTimer_;

        //! target being dragged
        /*! QWeakPointer is used in case the target gets deleted while drag is in progress */
        QWeakPointer<QWidget> target_;

        //! pointer to current blacklisted event, if any
        QEvent* blackListEvent_;

        //! true if drag is in progress
        bool dragInProgress_;

    };

}

#endif
