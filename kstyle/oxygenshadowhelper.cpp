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

#include "oxygenshadowhelper.h"

#include "oxygenpropertynames.h"
#include "oxygenshadowcache.h"
#include "oxygenstylehelper.h"

#include <QDockWidget>
#include <QMenu>
#include <QPainter>
#include <QToolBar>
#include <QTextStream>
#include <QEvent>

#if OXYGEN_HAVE_X11
#include <QX11Info>
#endif

#if OXYGEN_HAVE_KWAYLAND
#include <KWayland/Client/buffer.h>
#include <KWayland/Client/connection_thread.h>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/shadow.h>
#include <KWayland/Client/shm_pool.h>
#include <KWayland/Client/surface.h>
#endif

namespace Oxygen
{

    const char ShadowHelper::netWMShadowAtomName[] = "_KDE_NET_WM_SHADOW";

    //_____________________________________________________
    ShadowHelper::ShadowHelper( QObject* parent, StyleHelper& helper ):
        QObject( parent ),
        _helper( helper ),
        _shadowCache( new ShadowCache( helper ) ),
        _size( 0 )
        #if OXYGEN_HAVE_X11
        ,_gc( 0 ),
        _atom( 0 )
        #endif
        #if OXYGEN_HAVE_KWAYLAND
        , _shadowManager( Q_NULLPTR )
        , _shmPool( Q_NULLPTR )
        #endif
    {
        initializeWayland();
    }

    //_______________________________________________________
    ShadowHelper::~ShadowHelper( void )
    {

        #if OXYGEN_HAVE_X11
        if( _helper.isX11() )
        {
            foreach( const quint32& value, _pixmaps  ) xcb_free_pixmap( _helper.connection(), value );
            foreach( const quint32& value, _dockPixmaps  ) xcb_free_pixmap( _helper.connection(), value );
        }
        #endif

        delete _shadowCache;

    }

    //_______________________________________________________
    void ShadowHelper::initializeWayland()
    {
        #if OXYGEN_HAVE_KWAYLAND
        if( !Helper::isWayland() ) return;

        using namespace KWayland::Client;
        auto connection = ConnectionThread::fromApplication( this );
        if( !connection ) {
            return;
        }
        Registry *registry = new Registry( this );
        registry->create( connection );
        connect(registry, &Registry::interfacesAnnounced, this,
            [registry, this] {
                const auto interface = registry->interface( Registry::Interface::Shadow );
                if( interface.name != 0 ) {
                    _shadowManager = registry->createShadowManager( interface.name, interface.version, this );
                }
                const auto shmInterface = registry->interface( Registry::Interface::Shm );
                if( shmInterface.name != 0 ) {
                    _shmPool = registry->createShmPool( shmInterface.name, shmInterface.version, this );
                }
            }
        );

        registry->setup();
        connection->roundtrip();
        #endif
    }

    //______________________________________________
    void ShadowHelper::reset( void )
    {
        #if OXYGEN_HAVE_X11
        if( _helper.isX11() )
        {
            // round pixmaps
            foreach( const quint32& value, _pixmaps  ) xcb_free_pixmap( _helper.connection(), value );
            foreach( const quint32& value, _dockPixmaps  ) xcb_free_pixmap( _helper.connection(), value );
        }
        #endif

        _pixmaps.clear();
        _dockPixmaps.clear();

        _tiles = TileSet();
        _dockTiles = TileSet();

        // reset size
        _size = 0;

    }

    //_______________________________________________________
    bool ShadowHelper::registerWidget( QWidget* widget, bool force )
    {

        // make sure widget is not already registered
        if( _widgets.contains( widget ) ) return false;

        // check if widget qualifies
        if( !( force || acceptWidget( widget ) ) )
        { return false; }

        // try create shadow directly
        if( installShadows( widget ) ) _widgets.insert( widget, widget->winId() );
        else _widgets.insert( widget, 0 );

        // install event filter
        widget->removeEventFilter( this );
        widget->installEventFilter( this );

        // connect destroy signal
        connect( widget, SIGNAL(destroyed(QObject*)), SLOT(objectDeleted(QObject*)) );

        return true;

    }

    //_______________________________________________________
    void ShadowHelper::unregisterWidget( QWidget* widget )
    {
        if( _widgets.remove( widget ) )
        { uninstallShadows( widget ); }
    }

    //_______________________________________________________
    void ShadowHelper::reparseCacheConfig( void )
    {
        // shadow cache
        _shadowCache->readConfig();
    }

    //_______________________________________________________
    void ShadowHelper::loadConfig( void )
    {

        // reset
        reset();

        // retrieve shadow pixmap
        _size = _shadowCache->shadowSize();

        QPixmap pixmap( _shadowCache->pixmap( ShadowCache::Key() ) );
        const QSize pixmapSize( pixmap.size()/_helper.devicePixelRatio( pixmap ) );
        if( !pixmap.isNull() )
        {
            QPainter painter( &pixmap );

            // add transparency
            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            painter.fillRect( pixmap.rect(), QColor( 0, 0, 0, 150 ) );
        }

        // recreate tileset
        _tiles = TileSet( pixmap, pixmapSize.width()/2, pixmapSize.height()/2, 1, 1 );

        if( !pixmap.isNull() )
        {
            QPainter painter( &pixmap );

            // add round corners
            const QRect cornerRect( (pixmap.width()-10)/2, (pixmap.height()-10)/2, 10, 10 );
            _helper.roundCorner( QPalette().color( QPalette::Window ) )->render( cornerRect, &painter );
        }

        // recreate tileset
        _dockTiles = TileSet( pixmap, pixmapSize.width()/2, pixmapSize.height()/2, 1, 1 );

        // update property for registered widgets
        for( QMap<QWidget*,WId>::const_iterator iter = _widgets.constBegin(); iter != _widgets.constEnd(); ++iter )
        { installShadows( iter.key() ); }

    }

    //_______________________________________________________
    bool ShadowHelper::eventFilter( QObject* object, QEvent* event )
    {

        // check event type
        if( event->type() != QEvent::WinIdChange ) return false;

        // cast widget
        QWidget* widget( static_cast<QWidget*>( object ) );

        // install shadows and update winId
        if( installShadows( widget ) )
        { _widgets.insert( widget, widget->winId() ); }

        return false;

    }

    //_______________________________________________________
    void ShadowHelper::objectDeleted( QObject* object )
    { _widgets.remove( static_cast<QWidget*>( object ) ); }

    //_______________________________________________________
    bool ShadowHelper::isMenu( QWidget* widget ) const
    { return qobject_cast<QMenu*>( widget ); }

    //_______________________________________________________
    bool ShadowHelper::isToolTip( QWidget* widget ) const
    { return widget->inherits( "QTipLabel" ) || (widget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip; }

    //_______________________________________________________
    bool ShadowHelper::isDockWidget( QWidget* widget ) const
    { return qobject_cast<QDockWidget*>( widget ); }

    //_______________________________________________________
    bool ShadowHelper::isToolBar( QWidget* widget ) const
    { return qobject_cast<QToolBar*>( widget ) || widget->inherits( "Q3ToolBar" ); }

    //_______________________________________________________
    bool ShadowHelper::acceptWidget( QWidget* widget ) const
    {

        // flags
        if( widget->property( PropertyNames::netWMSkipShadow ).toBool() ) return false;
        if( widget->property( PropertyNames::netWMForceShadow ).toBool() ) return true;

        // menus
        if( isMenu( widget ) ) return true;

        // combobox dropdown lists
        if( widget->inherits( "QComboBoxPrivateContainer" ) ) return true;

        // tooltips
        if( isToolTip( widget ) && !widget->inherits( "Plasma::ToolTip" ) )
        { return true; }

        // detached widgets
        if( isDockWidget( widget ) || isToolBar( widget ) )
        { return true; }

        // reject
        return false;
    }

    //______________________________________________
    const QVector<quint32>& ShadowHelper::createPixmapHandles( bool isDockWidget )
    {

        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */

        // create atom
        #if OXYGEN_HAVE_X11
        if( !_atom && _helper.isX11() ) _atom = _helper.createAtom( QLatin1String( netWMShadowAtomName ) );
        #endif

        // make sure size is valid
        if( _size <= 0 ) return _pixmaps;

        // make sure pixmaps are not already initialized
        if( isDockWidget )
        {
            // make sure pixmaps are not already initialized
            if( _dockPixmaps.empty() && _dockTiles.isValid() )
            {

                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 1 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 2 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 5 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 8 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 7 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 6 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 3 ) ) );
                _dockPixmaps.append( createPixmap( _dockTiles.pixmap( 0 ) ) );

            }

        } else if( _pixmaps.empty() && _tiles.isValid() ) {

            _pixmaps.append( createPixmap( _tiles.pixmap( 1 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 2 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 5 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 8 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 7 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 6 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 3 ) ) );
            _pixmaps.append( createPixmap( _tiles.pixmap( 0 ) ) );

        }

        // return relevant list of pixmap handles
        return isDockWidget ? _dockPixmaps:_pixmaps;

    }

    //______________________________________________
    quint32 ShadowHelper::createPixmap( const QPixmap& source )
    {

        // do nothing for invalid pixmaps
        if( source.isNull() ) return 0;
        if( !_helper.isX11() ) return 0;

        /*
        in some cases, pixmap handle is invalid. This is the case notably
        when Qt uses to RasterEngine. In this case, we create an X11 Pixmap
        explicitly and draw the source pixmap on it.
        */

        #if OXYGEN_HAVE_X11

        const int width( source.width() );
        const int height( source.height() );

        // create X11 pixmap
        xcb_pixmap_t pixmap = xcb_generate_id( _helper.connection() );
        xcb_create_pixmap( _helper.connection(), 32, pixmap, QX11Info::appRootWindow(), width, height );

        // create gc
        if( !_gc )
        {
            _gc = xcb_generate_id( _helper.connection() );
            xcb_create_gc( _helper.connection(), _gc, pixmap, 0, 0x0 );
        }

        // create image from QPixmap and assign to pixmap
        QImage image( source.toImage() );
        xcb_put_image( _helper.connection(), XCB_IMAGE_FORMAT_Z_PIXMAP, pixmap, _gc, image.width(), image.height(), 0, 0, 0, 32, image.byteCount(), image.constBits());

        return pixmap;

        #else
        return 0;
        #endif

    }

    //_______________________________________________________
    bool ShadowHelper::installShadows( QWidget* widget )
    {
        if( !widget ) return false;

        /*
        From bespin code. Supposibly prevent playing with some 'pseudo-widgets'
        that have winId matching some other -random- window
        */
        if( !(widget->testAttribute(Qt::WA_WState_Created) && widget->internalWinId() ))
        { return false; }

        if( Helper::isX11() ) return installX11Shadows( widget );
        if( Helper::isWayland() ) return installWaylandShadows( widget );

        return false;
    }

    //_______________________________________________________
    bool ShadowHelper::installX11Shadows( QWidget* widget )
    {

        #if OXYGEN_HAVE_X11
        #ifndef QT_NO_XRENDER

        // create pixmap handles if needed
        const bool isDockWidget( this->isDockWidget( widget ) || this->isToolBar( widget ) );
        const QVector<quint32>& pixmaps( createPixmapHandles( isDockWidget ) );
        if( pixmaps.size() != numPixmaps ) return false;

        // create data
        // add pixmap handles
        QVector<quint32> data;
        foreach( const quint32& value, pixmaps )
        { data.append( value ); }

        const QMargins margins = shadowMargins( widget );
        data << int(margins.top()) << int(margins.right()) << int(margins.bottom()) << int(margins.left());

        xcb_change_property( _helper.connection(), XCB_PROP_MODE_REPLACE, widget->winId(), _atom, XCB_ATOM_CARDINAL, 32, data.size(), data.constData() );
        xcb_flush( _helper.connection() );

        return true;

        #endif
        #endif

        return false;

    }

    //_______________________________________________________
    void ShadowHelper::uninstallX11Shadows( QWidget* widget ) const
    {

        #if OXYGEN_HAVE_X11
        xcb_delete_property( _helper.connection(), widget->winId(), _atom);
        #else
        Q_UNUSED( widget )
        #endif

    }

    //_______________________________________________________
    bool ShadowHelper::installWaylandShadows( QWidget* widget )
    {
        #if OXYGEN_HAVE_KWAYLAND
        if( widget->windowHandle()->parent() ) return false;
        if( !_shadowManager || !_shmPool ) return false;

        const bool isDockWidget( this->isDockWidget( widget ) || this->isToolBar( widget ) );
        const TileSet &tiles = isDockWidget ? _dockTiles : _tiles;

        if( !tiles.isValid() ) return false;

        // create shadow
        using namespace KWayland::Client;
        auto s = Surface::fromWindow( widget->windowHandle() );
        if( !s ) return false;

        auto shadow = _shadowManager->createShadow( s, widget );
        if( !shadow->isValid() ) return false;

        // add the shadow elements
        shadow->attachTop( _shmPool->createBuffer( tiles.pixmap( 1 ).toImage() ) );
        shadow->attachTopRight( _shmPool->createBuffer( tiles.pixmap( 2 ).toImage() ) );
        shadow->attachRight( _shmPool->createBuffer( tiles.pixmap( 5 ).toImage() ) );
        shadow->attachBottomRight( _shmPool->createBuffer( tiles.pixmap( 8 ).toImage() ) );
        shadow->attachBottom( _shmPool->createBuffer( tiles.pixmap( 7 ).toImage() ) );
        shadow->attachBottomLeft( _shmPool->createBuffer( tiles.pixmap( 6 ).toImage() ) );
        shadow->attachLeft( _shmPool->createBuffer( tiles.pixmap( 3 ).toImage() ) );
        shadow->attachTopLeft( _shmPool->createBuffer( tiles.pixmap( 0 ).toImage() ) );

        shadow->setOffsets( shadowMargins( widget ) );
        shadow->commit();
        s->commit( Surface::CommitFlag::None );

        return true;
        #else
        Q_UNUSED( widget );
        #endif

        return false;
    }

    //_______________________________________________________
    QMargins ShadowHelper::shadowMargins( QWidget* widget ) const
    {
        // const qreal devicePixelRatio( _helper.devicePixelRatio( isDockWidget ?
        // _dockTiles.pixmap( 0 ):_tiles.pixmap( 0 ) ) );
        const qreal devicePixelRatio( 1.0 );

        // add padding
        /*
        in most cases all 4 paddings are identical, since offsets are handled when generating the pixmaps.
        There is one extra pixel needed with respect to actual shadow size, to deal with how
        menu backgrounds are rendered.
        Some special care is needed for QBalloonTip, since the later have an arrow
        */

        int topSize = 0;
        int rightSize = 0;
        int bottomSize = 0;
        int leftSize = 0;

        if( isToolTip( widget ) && widget->inherits( "QBalloonTip" ) )
        {

            // balloon tip needs special margins to deal with the arrow
            int top = 0;
            int bottom = 0;
            widget->getContentsMargins(nullptr, &top, nullptr, &bottom );

            // also need to decrement default size further due to extra hard coded round corner
            const int size = (_size - 2)*devicePixelRatio;

            // it seems arrow can be either to the top or the bottom. Adjust margins accordingly
            if( top > bottom )
            {
                topSize = size - (top - bottom);
                rightSize = size;
                bottomSize = size;
                leftSize = size;
            } else {
                topSize = size;
                rightSize = size;
                bottomSize = size - (bottom - top);
                leftSize = size;
            }

        } else {

            const int size = _size*devicePixelRatio;
            topSize = size;
            rightSize = size;
            bottomSize = size;
            leftSize = size;

        }

        return QMargins( leftSize, topSize, rightSize, bottomSize );
    }

    //_______________________________________________________
    void ShadowHelper::uninstallShadows( QWidget* widget ) const
    {
        if( !( widget && widget->testAttribute(Qt::WA_WState_Created) ) ) return;
        if( Helper::isX11() ) uninstallX11Shadows( widget );
        if( Helper::isWayland() ) uninstallWaylandShadows( widget );
    }

    //_______________________________________________________
    void ShadowHelper::uninstallWaylandShadows( QWidget* widget ) const
    {
        #if OXYGEN_HAVE_KWAYLAND
        if( widget->windowHandle()->parent() ) return;
        if( !_shadowManager ) return;

        using namespace KWayland::Client;
        auto s = Surface::fromWindow( widget->windowHandle() );
        if( !s ) return;

        _shadowManager->removeShadow( s );
        s->commit( Surface::CommitFlag::None );
        #else
        Q_UNUSED( widget )
        #endif
    }

}
