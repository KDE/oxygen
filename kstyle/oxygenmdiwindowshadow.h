#ifndef oxygenmdiwindowshadow_h
#define oxygenmdiwindowshadow_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmdiwindowshadow.h
// handle MDI windows' shadows
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Largely inspired from skulpture widget style
// Copyright (c) 2007-2009 Christoph Feck <christoph@maxiom.de>
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

#include <QEvent>
#include <QObject>
#include <QSet>

#include <QWidget>
#include <QPaintEvent>
#include <KColorScheme>

#include "oxygenstylehelper.h"
#include "oxygentileset.h"

namespace Oxygen
{

    //* frame shadow
    /** this allows the shadow to be painted over the widgets viewport */
    class MdiWindowShadow: public QWidget
    {

        Q_OBJECT

        public:

        //* shadow size (hard coded)
        enum { ShadowSize = 10 };

        //* constructor
        explicit MdiWindowShadow( QWidget*, TileSet );

        //* update geometry
        void updateGeometry( void );

        //* update ZOrder
        void updateZOrder( void );

        //* set associated window
        void setWidget( QWidget* value )
        { _widget = value; }

        //* associated window
        QWidget* widget( void ) const
        { return _widget; }

        protected:

        //* painting
        void paintEvent(QPaintEvent *) override;

        private:

        //* associated widget
        QWidget* _widget = nullptr;

        //* tileset rect, used for painting
        QRect _shadowTilesRect;

        //* tileset used to draw shadow
        TileSet _shadowTiles;

    };

    //* shadow manager
    class MdiWindowShadowFactory: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        MdiWindowShadowFactory( QObject*, StyleHelper& );

        //* register widget
        bool registerWidget( QWidget* );

        //* unregister
        void unregisterWidget( QWidget* );

        //* true if widget is registered
        bool isRegistered( const QObject* widget ) const
        { return _registeredWidgets.contains( widget ); }

        //* event filter
        bool eventFilter( QObject*, QEvent*) override;

        private Q_SLOTS:

        //* triggered by object destruction
        void widgetDestroyed( QObject* );

        private:

        //* find shadow matching a given object
        MdiWindowShadow* findShadow( QObject* ) const;

        //* install shadows on given widget
        void installShadow( QObject* );

        //* remove shadows from widget
        void removeShadow( QObject* );

        //* hide shadows
        void hideShadows( QObject* object ) const
        {
            if( MdiWindowShadow* windowShadow = findShadow( object ) )
            { windowShadow->hide(); }
        }

        //* update ZOrder
        void updateShadowZOrder( QObject* object ) const
        {
            if( MdiWindowShadow* windowShadow = findShadow( object ) )
            {
                if( !windowShadow->isVisible() ) windowShadow->show();
                windowShadow->updateZOrder();
            }
        }

        //* update shadows geometry
        void updateShadowGeometry( QObject* object ) const
        {
            if( MdiWindowShadow* windowShadow = findShadow( object ) )
            { windowShadow->updateGeometry(); }
        }

        //* update shadows
        void update( QObject* object ) const
        {
            if( MdiWindowShadow* windowShadow = findShadow( object ) )
            { windowShadow->update(); }
        }

        //* set of registered widgets
        QSet<const QObject*> _registeredWidgets;

        //* tileset used to draw shadow
        TileSet _shadowTiles;

    };

}

#endif
