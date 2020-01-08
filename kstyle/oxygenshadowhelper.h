#ifndef oxygenshadowhelper_h
#define oxygenshadowhelper_h

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowhelper.h
// handle shadow pixmaps passed to window manager via X property
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright (c) 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
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

#include "oxygen.h"
#include "oxygentileset.h"

#include <KWindowShadow>

#include <QObject>
#include <QMap>
#include <QMargins>
#include <QSet>

namespace Oxygen
{

    //* forward declaration
    class ShadowCache;
    class StyleHelper;

    //* handle shadow pixmaps passed to window manager via X property
    class ShadowHelper: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        ShadowHelper( QObject*, StyleHelper& );

        //* destructor
        ~ShadowHelper( void ) override;

        //* reset
        void reset( void );

        //* register widget
        bool registerWidget( QWidget*, bool force = false );

        //* unregister widget
        void unregisterWidget( QWidget* );

        //* reparse cache config (must be followed by loadConfig)
        void reparseCacheConfig( void );

        //* load config
        void loadConfig( void );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        private Q_SLOTS:

        //* unregister widget
        void objectDeleted( QObject* );

        private:

        //* true if widget is a menu
        bool isMenu( QWidget* ) const;

        //* true if widget is a tooltip
        bool isToolTip( QWidget* ) const;

        //* dock widget
        bool isDockWidget( QWidget* ) const;

        //* toolbar
        bool isToolBar( QWidget* ) const;

        //* accept widget
        bool acceptWidget( QWidget* ) const;

        //* shadow cache
        const ShadowCache& shadowCache( void ) const
        { return *_shadowCache; }

        //* shadow cache
        ShadowCache& shadowCache( void )
        { return *_shadowCache; }

        // create shadow tiles from tileset
        const QVector<KWindowShadowTile::Ptr>& createPlatformTiles( bool isDockWidget );

        // create a shadow tile from pixmap
        KWindowShadowTile::Ptr createPlatformTile( const QPixmap& );

        //* installs shadow on given widget in a platform independent way
        void installShadows( QWidget * );

        //* uninstalls shadow on given widget in a platform independent way
        void uninstallShadows( QWidget * );

        //* gets the shadow margins for the given widget
        QMargins shadowMargins( QWidget* ) const;

        //* helper
        StyleHelper& _helper;

        //* cache
        ShadowCache* _shadowCache;

        //* set of registered widgets
        QSet<QWidget*> _widgets;

        //* map of managed shadows
        QMap<QWidget*, KWindowShadow*> _shadows;

        //*@name shadow tilesets
        //@{
        TileSet _tiles;
        TileSet _dockTiles;
        //@}

        //* number of shadow tiles
        enum { numTiles = 8 };

        //*@name shared shadow tiles
        //@{
        QVector<KWindowShadowTile::Ptr> _platformTiles;
        QVector<KWindowShadowTile::Ptr> _platformDockTiles;
        //@}

        //* shadow size
        int _size;

    };

}

#endif
