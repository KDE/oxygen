#ifndef oxygenblurhelper_h
#define oxygenblurhelper_h

//////////////////////////////////////////////////////////////////////////////
// oxygenblurhelper.h
// handle regions passed to kwin for blurring
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Loosely inspired (and largely rewritten) from BeSpin style
// SPDX-FileCopyrightText: 2007 Thomas Luebking <thomas.luebking@web.de>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenstylehelper.h"
#include "oxygen.h"

#include <QBasicTimer>
#include <QHash>
#include <QObject>
#include <QSet>
#include <QTimerEvent>

#include <QDockWidget>
#include <QMenu>
#include <QRegion>
#include <QToolBar>

#if OXYGEN_HAVE_X11
#include <xcb/xcb.h>
#endif

namespace Oxygen
{
    class BlurHelper: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        BlurHelper( QObject*, StyleHelper& );

        //* enable state
        void setEnabled( bool value )
        { _enabled = value; }

        //* enabled
        bool enabled( void ) const
        { return _enabled; }

        //* register widget
        void registerWidget( QWidget* );

        //* register widget
        void unregisterWidget( QWidget* );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        protected:

        //* timer event
        /** used to perform delayed blur region update of pending widgets */
        void timerEvent( QTimerEvent* event ) override
        {

            if( event->timerId() == _timer.timerId() )
            {
                _timer.stop();
                update();
            } else QObject::timerEvent( event );

        }

        private Q_SLOTS:

        //* wiget destroyed
        void widgetDestroyed( QObject* object )
        { _widgets.remove( object ); }

        private:

        //* install event filter to object, in a unique way
        void addEventFilter( QObject* object )
        {
            object->removeEventFilter( this );
            object->installEventFilter( this );
        }

        //* get list of blur-behind regions matching a given widget
        QRegion blurRegion( QWidget* ) const;

        //* trim blur region to remove unnecessary areas (recursive)
        void trimBlurRegion( QWidget*, QWidget*, QRegion& ) const;

        //* update blur region for all pending widgets
        void update( void )
        {

            for ( const WidgetPointer &widget : std::as_const(_pendingWidgets) )
            { if( widget ) update( widget.data() ); }

            _pendingWidgets.clear();

        }

        //* update blur regions for given widget
        void update( QWidget* ) const;

        //* clear blur regions for given widget
        void clear( QWidget* ) const;

        //* returns true if a given widget is opaque
        bool isOpaque( const QWidget* widget ) const;

        //* true if widget is a transparent window
        /** some additional checks are performed to make sure stuff like plasma tooltips
        don't get their blur region overwritten */
        bool isTransparent( const QWidget* widget ) const;

        //* helper
        StyleHelper& _helper;

        //* enability
        bool _enabled;

        //* list of widgets for which blur region must be updated
        using WidgetPointer = WeakPointer<QWidget>;
        using WidgetSet = QHash<QWidget*, WidgetPointer>;
        WidgetSet _pendingWidgets;

        //* set of registered widgets
        QSet<const QObject*> _widgets;

        //* delayed update timer
        QBasicTimer _timer;

        #if OXYGEN_HAVE_X11
        //* blur atom
        xcb_atom_t _blurAtom;
        xcb_atom_t _opaqueAtom;
        #endif

    };

}

#endif
