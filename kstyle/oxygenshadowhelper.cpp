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

namespace Oxygen
{

    const char ShadowHelper::netWMShadowAtomName[] = "_KDE_NET_WM_SHADOW";

    //_____________________________________________________
    ShadowHelper::ShadowHelper( QObject* parent, StyleHelper& helper ):
        QObject( parent ),
        _helper( helper ),
        _supported( checkSupported() ),
        _shadowCache( new ShadowCache( helper ) ),
        _size( 0 )
        #if OXYGEN_HAVE_X11
        ,_gc( 0 ),
        _atom( 0 )
        #endif
    {}

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

        // do nothing if not supported
        if( !_supported ) return false;

        // make sure widget is not already registered
        if( _widgets.contains( widget ) ) return false;

        // check if widget qualifies
        if( !( force || acceptWidget( widget ) ) )
        { return false; }

        // store in map and add destroy signal connection
        widget->removeEventFilter( this );
        widget->installEventFilter( this );
        _widgets.insert( widget, 0 );

        /*
        need to install shadow directly when widget "created" state is already set
        since WinID changed is never called when this is the case
        */
        if( widget->testAttribute(Qt::WA_WState_Created) && installX11Shadows( widget ) )
        {  _widgets.insert( widget, widget->winId() ); }

        connect( widget, SIGNAL(destroyed(QObject*)), SLOT(objectDeleted(QObject*)) );

        return true;

    }

    //_______________________________________________________
    void ShadowHelper::unregisterWidget( QWidget* widget )
    {
        if( _widgets.remove( widget ) )
        { uninstallX11Shadows( widget ); }
    }

    //_______________________________________________________
    void ShadowHelper::reparseCacheConfig( void )
    {
        // shadow cache
        shadowCache().readConfig();
    }

    //_______________________________________________________
    void ShadowHelper::loadConfig( void )
    {

        // reset
        reset();

        // retrieve shadow pixmap
        _size = shadowCache().shadowSize();

        QPixmap pixmap( shadowCache().pixmap( ShadowCache::Key() ) );
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
        { installX11Shadows( iter.key() ); }

    }

    //_______________________________________________________
    bool ShadowHelper::eventFilter( QObject* object, QEvent* event )
    {

        // check event type
        if( event->type() != QEvent::WinIdChange ) return false;

        // cast widget
        QWidget* widget( static_cast<QWidget*>( object ) );

        // install shadows and update winId
        if( installX11Shadows( widget ) )
        { _widgets.insert( widget, widget->winId() ); }

        return false;

    }

    //_______________________________________________________
    void ShadowHelper::objectDeleted( QObject* object )
    { _widgets.remove( static_cast<QWidget*>( object ) ); }

    //_______________________________________________________
    bool ShadowHelper::checkSupported( void ) const
    {

        // create atom
        #if OXYGEN_HAVE_X11

        // make sure we are on X11
        if( !_helper.isX11() ) return false;

        // create atom
        xcb_atom_t netSupportedAtom( _helper.createAtom( "_NET_SUPPORTED" ) );
        if( !netSupportedAtom ) return false;

        // store connection locally
        xcb_connection_t* connection( _helper.connection() );

        // get property
        const quint32 maxLength = std::string().max_size();
        xcb_get_property_cookie_t cookie( xcb_get_property( connection, 0, QX11Info::appRootWindow(), netSupportedAtom, XCB_ATOM_ATOM, 0, (maxLength+3) / 4 ) );
        Helper::ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( connection, cookie, nullptr ) );
        if( !reply ) return false;

        // get reply length and data
        const int count( xcb_get_property_value_length( reply.data() )/sizeof( xcb_atom_t ) );
        xcb_atom_t *atoms = reinterpret_cast<xcb_atom_t*>( xcb_get_property_value( reply.data() ) );

        bool found( false );
        for( int i = 0; i < count && !found; ++i )
        {
            // get atom name and print
            xcb_atom_t atom( atoms[i] );

            xcb_get_atom_name_cookie_t cookie( xcb_get_atom_name( connection, atom ) );
            Helper::ScopedPointer<xcb_get_atom_name_reply_t> reply( xcb_get_atom_name_reply( connection, cookie, 0 ) );
            if( !reply ) continue;

            // get name and compare
            const QString name( QByteArray( xcb_get_atom_name_name( reply.data() ), xcb_get_atom_name_name_length( reply.data() ) ) );
            if( strcmp( netWMShadowAtomName, xcb_get_atom_name_name( reply.data() ) ) == 0 ) found = true;

        }

        return found;

        #else
        return false;
        #endif

    }

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
    bool ShadowHelper::installX11Shadows( QWidget* widget )
    {

        // do nothing if not supported
        if( !_supported ) return false;

        // check widget and shadow
        if( !widget ) return false;
        if( !_helper.isX11() ) return false;

        #if OXYGEN_HAVE_X11
        #ifndef QT_NO_XRENDER

        /*
        From bespin code. Supposibly prevent playing with some 'pseudo-widgets'
        that have winId matching some other -random- window
        */
        if( !(widget->testAttribute(Qt::WA_WState_Created) || widget->internalWinId() ))
        { return false; }

        // create pixmap handles if needed
        const bool isDockWidget( this->isDockWidget( widget ) || this->isToolBar( widget ) );
        const QVector<quint32>& pixmaps( createPixmapHandles( isDockWidget ) );
        if( pixmaps.size() != numPixmaps ) return false;

        // create data
        // add pixmap handles
        QVector<quint32> data;
        foreach( const quint32& value, pixmaps )
        { data.append( value ); }

        // get devicePixelRatio
        // for testing purposes only
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

        if( isToolTip( widget ) && widget->inherits( "QBalloonTip" ) )
        {

            // balloon tip needs special margins to deal with the arrow
            int top = 0;
            int bottom = 0;
            widget->getContentsMargins(nullptr, &top, nullptr, &bottom );

            // also need to decrement default size further due to extra hard coded round corner
            const int size = (_size - 2)*devicePixelRatio;

            // it seems arrow can be either to the top or the bottom. Adjust margins accordingly
            if( top > bottom ) data << size - (top - bottom) << size << size << size;
            else data << size << size << size - (bottom - top) << size;

        } else {

            const int size = _size*devicePixelRatio;
            data << size << size << size << size;

        }

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
        if( !_supported ) return;
        if( !_helper.isX11() ) return;
        if( !( widget && widget->testAttribute(Qt::WA_WState_Created) ) ) return;
        xcb_delete_property( _helper.connection(), widget->winId(), _atom);
        #else
        Q_UNUSED( widget )
        #endif

    }

}
