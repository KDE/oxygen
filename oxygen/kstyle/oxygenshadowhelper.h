#ifndef oxygenshadowhelper_h
#define oxygenshadowhelper_h

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowhelper.h
// handle shadow pixmaps passed to window manager via X property
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygentileset.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QMap>

#ifdef Q_WS_X11
#include <X11/Xdefs.h>
#endif

namespace Oxygen
{

    //! forward declaration
    class ShadowCache;
    class StyleHelper;

    //! handle shadow pixmaps passed to window manager via X property
    class ShadowHelper: public QObject
    {

        Q_OBJECT

        public:

        //!@name property names
        static const char* const netWMShadowAtomName;
        static const char* const netWMForceShadowPropertyName;
        static const char* const netWMSkipShadowPropertyName;

        //! constructor
        ShadowHelper( QObject*, StyleHelper& );

        //! destructor
        virtual ~ShadowHelper( void );

        //! reset
        void reset( void );

        //! register widget
        bool registerWidget( QWidget*, bool force = false );

        //! unregister widget
        void unregisterWidget( QWidget* );

        //! reload config
        void reloadConfig( void );

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        protected slots:

        //! unregister widget
        void objectDeleted( QObject* );

        protected:

        //! true if widget is a menu
        bool isMenu( QWidget* ) const;

        //! true if widget is a tooltip
        bool isToolTip( QWidget* ) const;

        //! dock widget
        bool isDockWidget( QWidget* ) const;

        //! toolbar
        bool isToolBar( QWidget* ) const;

        //! accept widget
        bool acceptWidget( QWidget* ) const;

        //! shadow cache
        const ShadowCache& shadowCache( void ) const
        { return *_shadowCache; }

        //! shadow cache
        ShadowCache& shadowCache( void )
        { return *_shadowCache; }

        // create pixmap handles from tileset
        const QVector<Qt::HANDLE>& createPixmapHandles( bool isDockWidget );

        // create pixmap handle from pixmap
        Qt::HANDLE createPixmap( const QPixmap& ) const;

        //! install shadow X11 property on given widget
        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */
        bool installX11Shadows( QWidget* );

        //! uninstall shadow X11 property on given widget
        void uninstallX11Shadows( QWidget* ) const;

        private:

        //! helper
        StyleHelper& _helper;

        //! cache
        ShadowCache* _shadowCache;

        //! set of registered widgets
        QMap<QWidget*, WId> _widgets;

        //!@name shadow tilesets
        //@{
        TileSet _tiles;
        TileSet _dockTiles;
        //@}

        //! number of pixmaps
        enum { numPixmaps = 8 };

        //!@name pixmaps
        //@{
        QVector<Qt::HANDLE> _pixmaps;
        QVector<Qt::HANDLE> _dockPixmaps;
        //@}

        //! shadow size
        int _size;

        #ifdef Q_WS_X11
        //! shadow atom
        Atom _atom;
        #endif

    };

}

#endif
