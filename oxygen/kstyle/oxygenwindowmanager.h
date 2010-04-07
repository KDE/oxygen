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

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>
#include <QtCore/QBasicTimer>
#include <QtGui/QWidget>

#ifdef Q_WS_X11
#include <X11/Xdefs.h>
#endif

namespace Oxygen
{

    class WindowManager: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        explicit WindowManager( QObject* );

        //! enability
        bool enabled( void ) const
        { return enabled_; }

        //! enability
        void setEnabled( bool value )
        { enabled_ = value; }

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

        //! returns true if widget is dragable
        bool isDragableWidget( QWidget* ) const;

        //! returns true if drag can be started from current widget and position
        bool canDrag( QWidget*, const QPoint& ) const;

        //! reset drag
        void resetDrag( void );

        //! start drag
        void startDrag( QWidget*, const QPoint& );

        private:

        //! enability
        bool enabled_;

        //! drag distance
        /*! this is copied from kwin::geometry */
        int dragDistance_;

        //! drag delay
        /*! this is copied from kwin::geometry */
        int dragDelay_;

        //! drag point
        QPoint dragPoint_;

        //! drag timer
        QBasicTimer dragTimer_;

        //! target being dragged
        /*! QWeakPointer is used in case the target gets deleted while drag is in progress */
        QWeakPointer<QWidget> target_;

        //! true if drag is in progress
        bool dragInProgress_;

        #ifdef Q_WS_X11
        //! move resize X11 atom
        Atom netMoveResize_;
        #endif

    };

}

#endif
