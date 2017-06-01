#ifndef oxygenshadowhelper_h
#define oxygenshadowhelper_h

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowhelper.h
// handle shadow pixmaps passed to window manager via X property
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "config-liboxygen.h"

#include <QObject>
#include <QMap>
#include <QMargins>

#if OXYGEN_HAVE_X11
#include <xcb/xcb.h>
#endif

#if OXYGEN_HAVE_KWAYLAND
namespace KWayland
{
namespace Client
{
    class ShadowManager;
    class ShmPool;
}
}
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
        //@{
        static const char netWMShadowAtomName[];
        //@}

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

        //! reparse cache config (must be followed by loadConfig)
        void reparseCacheConfig( void );

        //! load config
        void loadConfig( void );

        //! event filter
        bool eventFilter( QObject*, QEvent* ) Q_DECL_OVERRIDE;

        protected Q_SLOTS:

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
        const QVector<quint32>& createPixmapHandles( bool isDockWidget );

        // create pixmap handle from pixmap
        quint32 createPixmap( const QPixmap& );

        //* installs shadow on given widget in a platform independent way
        bool installShadows( QWidget * );

        //* uninstalls shadow on given widget in a platform independent way
        void uninstallShadows( QWidget * ) const;

        //! install shadow X11 property on given widget
        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */
        bool installX11Shadows( QWidget* );

        //! uninstall shadow X11 property on given widget
        void uninstallX11Shadows( QWidget* ) const;

        //* install shadow on given widget for Wayland
        bool installWaylandShadows( QWidget * );

        //* uninstall shadow on given widget for Wayland
        void uninstallWaylandShadows( QWidget* ) const;

        //* initializes the Wayland specific parts
        void initializeWayland();

        //* gets the shadow margins for the given widget
        QMargins shadowMargins( QWidget* ) const;

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
        QVector<quint32> _pixmaps;
        QVector<quint32> _dockPixmaps;
        //@}

        //! shadow size
        int _size;

        #if OXYGEN_HAVE_X11

        //! graphical context
        xcb_gcontext_t _gc;

        //! shadow atom
        xcb_atom_t _atom;

        #endif

        #if OXYGEN_HAVE_KWAYLAND

        //* The Wayland shadow manager to create Shadows for Surfaces (QWindow)
        KWayland::Client::ShadowManager* _shadowManager;
        //* The Wayland Shared memory pool to share the shadow pixmaps with compositor
        KWayland::Client::ShmPool* _shmPool;

        #endif

    };

}

#endif
