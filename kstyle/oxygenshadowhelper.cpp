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

#include "oxygenshadowhelper.h"

#include "oxygenpropertynames.h"
#include "oxygenshadowcache.h"
#include "oxygenstylehelper.h"

#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <QMenu>
#include <QPainter>
#include <QToolBar>
#include <QTextStream>

namespace Oxygen
{

    //_____________________________________________________
    ShadowHelper::ShadowHelper( QObject* parent, StyleHelper& helper ):
        QObject( parent ),
        _helper( helper ),
        _shadowCache( new ShadowCache( helper ) ),
        _size( 0 )
    {
    }

    //_______________________________________________________
    ShadowHelper::~ShadowHelper( void )
    {

        qDeleteAll( _shadows );
        delete _shadowCache;

    }

    //______________________________________________
    void ShadowHelper::reset( void )
    {
        _platformTiles.clear();
        _platformDockTiles.clear();

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
        installShadows( widget );
        _widgets.insert( widget );

        // install event filter
        widget->removeEventFilter( this );
        widget->installEventFilter( this );

        // connect destroy signal
        connect( widget, SIGNAL(destroyed(QObject*)), SLOT(widgetDeleted(QObject*)) );

        return true;

    }

    //_______________________________________________________
    void ShadowHelper::unregisterWidget( QWidget* widget )
    {
        if( _widgets.remove( widget ) )
        {
            // uninstall the event filter
            widget->removeEventFilter( this );

            // disconnect all signals
            disconnect( widget, nullptr, this, nullptr );

            // uninstall the shadow
            uninstallShadows( widget );
        }
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
            _helper.roundCorner( QPalette().color( QPalette::Window ) ).render( cornerRect, &painter );
        }

        // recreate tileset
        _dockTiles = TileSet( pixmap, pixmapSize.width()/2, pixmapSize.height()/2, 1, 1 );

        // update property for registered widgets
        for( QWidget* widget : _widgets )
        { installShadows( widget ); }

    }

    //_______________________________________________________
    bool ShadowHelper::eventFilter( QObject* object, QEvent* event )
    {

        // check event type
        if( event->type() != QEvent::WinIdChange ) return false;

        // cast widget
        QWidget* widget( static_cast<QWidget*>( object ) );

        // install shadows on the widget again
        installShadows( widget );

        return false;

    }

    //_______________________________________________________
    void ShadowHelper::widgetDeleted( QObject* object )
    {
        QWidget* widget( static_cast<QWidget*>( object ) );
        _widgets.remove( widget );
    }

    //_______________________________________________________
    void ShadowHelper::windowDeleted( QObject* object )
    {
        QWindow* window( static_cast<QWindow*>( object ) );
        _shadows.remove( window );
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
    const QVector<KWindowShadowTile::Ptr>& ShadowHelper::createPlatformTiles( bool isDockWidget )
    {

        // make sure size is valid
        if( _size <= 0 ) return _platformTiles;

        // make sure shadow tiles are not already initialized
        if( isDockWidget )
        {
            // make sure shadow tiles are not already initialized
            if( _platformDockTiles.isEmpty() && _dockTiles.isValid() )
            {

                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 1 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 2 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 5 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 8 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 7 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 6 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 3 ) ) );
                _platformDockTiles.append( createPlatformTile( _dockTiles.pixmap( 0 ) ) );

            }

        } else if( _platformTiles.isEmpty() && _tiles.isValid() ) {

            _platformTiles.append( createPlatformTile( _tiles.pixmap( 1 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 2 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 5 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 8 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 7 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 6 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 3 ) ) );
            _platformTiles.append( createPlatformTile( _tiles.pixmap( 0 ) ) );

        }

        // return relevant list of shadow tiles
        return isDockWidget ? _platformDockTiles : _platformTiles;

    }

    //______________________________________________
    KWindowShadowTile::Ptr ShadowHelper::createPlatformTile(const QPixmap& pixmap)
    {

        KWindowShadowTile::Ptr tile = KWindowShadowTile::Ptr::create();
        tile->setImage( pixmap.toImage() );
        return tile;

    }

    //_______________________________________________________
    void ShadowHelper::installShadows( QWidget* widget )
    {
        if( !widget ) return;

        // only toplevel widgets can cast drop-shadows
        if( !widget->isWindow() ) return;

        // widget must have valid native window
        if( !widget->testAttribute( Qt::WA_WState_Created ) ) return;

        // create shadow tiles if needed
        const bool isDockWidget( this->isDockWidget( widget ) || this->isToolBar( widget ) );
        const QVector<KWindowShadowTile::Ptr>& tiles = createPlatformTiles( isDockWidget );
        if( tiles.count() != numTiles ) return;

        // get the underlying window for the widget
        QWindow* window = widget->windowHandle();

        // find a shadow associated with the widget
        KWindowShadow*& shadow = _shadows[ window ];

        if( !shadow )
        {
            // if there is no shadow yet, create one
            shadow = new KWindowShadow( window );

            // connect destroy signal
            connect( window, &QWindow::destroyed, this, &ShadowHelper::windowDeleted );
        }

        if( shadow->isCreated() )
        { shadow->destroy(); }

        shadow->setTopTile( tiles[0] );
        shadow->setTopRightTile( tiles[1] );
        shadow->setRightTile( tiles[2] );
        shadow->setBottomRightTile( tiles[3] );
        shadow->setBottomTile( tiles[4] );
        shadow->setBottomLeftTile( tiles[5] );
        shadow->setLeftTile( tiles[6] );
        shadow->setTopLeftTile( tiles[7] );
        shadow->setWindow( window );
        shadow->setPadding( shadowMargins( widget ) );
        shadow->create();
    }

    //_______________________________________________________
    QMargins ShadowHelper::shadowMargins( QWidget* widget ) const
    {
        // const qreal devicePixelRatio( _helper.devicePixelRatio( isDockWidget ?
        // _dockTiles.pixmap( 0 ):_tiles.pixmap( 0 ) ) );
        const qreal devicePixelRatio( qApp->devicePixelRatio() );

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
            const QMargins margins = widget->contentsMargins();
            const int top = margins.top();
            const int bottom = margins.bottom();
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
    void ShadowHelper::uninstallShadows( QWidget* widget )
    {
        delete _shadows.take( widget->windowHandle() );
    }

}
