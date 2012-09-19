#ifndef oxygenblurhelper_h
#define oxygenblurhelper_h

//////////////////////////////////////////////////////////////////////////////
// oxygenblurhelper.h
// handle regions passed to kwin for blurring
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Loosely inspired (and largely rewritten) from BeSpin style
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

#include "oxygenstylehelper.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtCore/QTimerEvent>

#include <QtGui/QDockWidget>
#include <QtGui/QMenu>
#include <QtGui/QRegion>
#include <QtGui/QToolBar>

#ifdef Q_WS_X11
#include <X11/Xdefs.h>
#endif

namespace Oxygen
{
    class BlurHelper: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        BlurHelper( QObject*, StyleHelper& );

        //! destructor
        virtual ~BlurHelper( void )
        {}

        //! enable state
        void setEnabled( bool value )
        { _enabled = value; }

        //! enabled
        bool enabled( void ) const
        { return _enabled; }

        //! register widget
        void registerWidget( QWidget* );

        //! register widget
        void unregisterWidget( QWidget* );

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        protected:

        //! timer event
        /*! used to perform delayed blur region update of pending widgets */
        virtual void timerEvent( QTimerEvent* event )
        {

            if( event->timerId() == _timer.timerId() )
            {
                _timer.stop();
                update();
            } else QObject::timerEvent( event );

        }

        //! install event filter to object, in a unique way
        void addEventFilter( QObject* object )
        {
            object->removeEventFilter( this );
            object->installEventFilter( this );
        }

        //! get list of blur-behind regions matching a given widget
        QRegion blurRegion( QWidget* ) const;

        //! trim blur region to remove unnecessary areas (recursive)
        void trimBlurRegion( QWidget*, QWidget*, QRegion& ) const;

        //! update blur region for all pending widgets
        /*! a timer is used to allow some buffering of the update requests */
        void delayedUpdate( void )
        {
            if( !_timer.isActive() )
            { _timer.start( 10, this ); }
        }

        //! update blur region for all pending widgets
        void update( void )
        {

            foreach( const WidgetPointer& widget, _pendingWidgets )
            { if( widget ) update( widget.data() ); }

            _pendingWidgets.clear();

        }

        //! update blur regions for given widget
        void update( QWidget* ) const;

        //! clear blur regions for given widget
        void clear( QWidget* ) const;

        //! returns true if a given widget is opaque
        bool isOpaque( const QWidget* widget ) const;

        //! true if widget is a transparent window
        /*! some additional checks are performed to make sure stuff like plasma tooltips
        don't get their blur region overwritten */
        bool isTransparent( const QWidget* widget ) const;

        protected slots:

        //! wiget destroyed
        void widgetDestroyed( QObject* object )
        { _widgets.remove( object ); }

        private:

        //! helper
        StyleHelper& _helper;

        //! enability
        bool _enabled;

        //! list of widgets for which blur region must be updated
        typedef QPointer<QWidget> WidgetPointer;
        typedef QHash<QWidget*, WidgetPointer> WidgetSet;
        WidgetSet _pendingWidgets;

        //! set of registered widgets
        QSet<const QObject*> _widgets;

        //! delayed update timer
        QBasicTimer _timer;

        #ifdef Q_WS_X11
        //! blur atom
        Atom _blurAtom;
        Atom _opaqueAtom;
        #endif

    };

}

#endif
