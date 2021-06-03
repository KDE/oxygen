#ifndef oxygenshadowhelper_h
#define oxygenshadowhelper_h

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowhelper.h
// handle shadow pixmaps passed to window manager via X property
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// SPDX-FileCopyrightText: 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
//
// SPDX-License-Identifier: MIT
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
        void widgetDeleted( QObject* );

        //* unregister window
        void windowDeleted( QObject* );

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
        QMap<QWindow*, KWindowShadow*> _shadows;

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
