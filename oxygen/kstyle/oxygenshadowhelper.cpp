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

#include "oxygenshadowhelper.h"
#include "oxygenshadowhelper.moc"
#include "oxygenshadowcache.h"

#include <KConfig>

#include <QtGui/QDockWidget>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QToolBar>
#include <QtCore/QTextStream>
#include <QtCore/QEvent>

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

namespace Oxygen
{

    const char* const ShadowHelper::netWMShadowAtomName( "_KDE_NET_WM_SHADOW" );
    const char* const ShadowHelper::netWMForceShadowPropertyName( "_KDE_NET_WM_FORCE_SHADOW" );
    const char* const ShadowHelper::netWMSkipShadowPropertyName( "_KDE_NET_WM_SKIP_SHADOW" );

    //_____________________________________________________
    ShadowHelper::ShadowHelper( QObject* parent, Helper& helper ):
        QObject( parent ),
        _shadowCache( new ShadowCache( helper ) ),
        _size( 0 )
        #ifdef Q_WS_X11
        ,_atom( None )
        #endif
    {}

    //_______________________________________________________
    ShadowHelper::~ShadowHelper( void )
    {

        #ifdef Q_WS_X11
        // round pixmaps
        foreach( const Qt::HANDLE& value, _pixmaps  )
        { XFreePixmap( QX11Info::display(), value ); }
        #endif

        delete _shadowCache;

    }

    //______________________________________________
    void ShadowHelper::reset( void )
    {
        #ifdef Q_WS_X11
        // round pixmaps
        foreach( const Qt::HANDLE& value, _pixmaps  )
        { XFreePixmap( QX11Info::display(), value ); }
        #endif

        _pixmaps.clear();

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

        connect( widget, SIGNAL( destroyed( QObject* ) ), SLOT( objectDeleted( QObject* ) ) );

        return true;

    }

    //_______________________________________________________
    void ShadowHelper::unregisterWidget( QWidget* widget )
    {
        if( _widgets.remove( widget ) )
        { uninstallX11Shadows( widget ); }
    }

    //_______________________________________________________
    void ShadowHelper::reloadConfig( void )
    {

        // shadow cache
        KConfig config( "oxygenrc" );
        if( !shadowCache().readConfig( config ) ) return;

        // reset
        reset();

        // recreate handles and store size
        _size = shadowCache().shadowSize();
        _tiles = *shadowCache().tileSet( ShadowCache::Key() );

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
    bool ShadowHelper::isMenu( QWidget* widget ) const
    { return qobject_cast<QMenu*>( widget ); }

    //_______________________________________________________
    bool ShadowHelper::isToolTip( QWidget* widget ) const
    { return widget->inherits( "QTipLabel" ) || (widget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip; }

    //_______________________________________________________
    bool ShadowHelper::acceptWidget( QWidget* widget ) const
    {

        if( widget->property( netWMSkipShadowPropertyName ).toBool() ) return false;
        if( widget->property( netWMForceShadowPropertyName ).toBool() ) return true;

        // menus
        if( qobject_cast<QMenu*>( widget ) ) return true;

        // combobox dropdown lists
        if( widget->inherits( "QComboBoxPrivateContainer" ) ) return true;

        // tooltips
        if( (widget->inherits( "QTipLabel" ) || (widget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip ) &&
            !widget->inherits( "Plasma::ToolTip" ) )
        { return true; }

        // detached widgets
        if( qobject_cast<QToolBar*>( widget ) || qobject_cast<QDockWidget*>( widget ) )
        { return true; }

        // reject
        return false;
    }

    //______________________________________________
    void ShadowHelper::createPixmapHandles( void )
    {

        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */

        // create atom
        #ifdef Q_WS_X11
        if( !_atom ) _atom = XInternAtom( QX11Info::display(), netWMShadowAtomName, False);
        #endif

        // make sure size is valid
        if( _size <= 0 ) return;

        // make sure pixmaps are not already initialized
        if( _pixmaps.empty() && _tiles.isValid() )
        {

            _pixmaps.push_back( createPixmap( _tiles.pixmap( 1 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 2 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 5 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 8 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 7 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 6 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 3 ) ) );
            _pixmaps.push_back( createPixmap( _tiles.pixmap( 0 ) ) );

        }

    }

    //______________________________________________
    Qt::HANDLE ShadowHelper::createPixmap( const QPixmap& source ) const
    {

        // do nothing for invalid pixmaps
        if( source.isNull() ) return 0;

        /*
        in some cases, pixmap handle is invalid. This is the case notably
        when Qt uses to RasterEngine. In this case, we create an X11 Pixmap
        explicitly and draw the source pixmap on it.
        */

        #ifdef Q_WS_X11
        const int width( source.width() );
        const int height( source.height() );

        // create X11 pixmap
        Pixmap pixmap = XCreatePixmap( QX11Info::display(), QX11Info::appRootWindow(), width, height, 32 );

        // create explicitly shared QPixmap from it
        QPixmap dest( QPixmap::fromX11Pixmap( pixmap, QPixmap::ExplicitlyShared ) );

        // create surface for pixmap
        {
            QPainter painter( &dest );
            painter.setCompositionMode( QPainter::CompositionMode_Source );
            painter.drawPixmap( 0, 0, source );

            // add opacity
            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            painter.fillRect( dest.rect(), QColor( 0, 0, 0, 150 ) );

        }


        return pixmap;
        #else
        return 0;
        #endif

    }

//_______________________________________________________
    bool ShadowHelper::installX11Shadows( QWidget* widget )
    {

        // check widget and shadow
        if( !widget ) return false;

        #ifdef Q_WS_X11
        #ifndef QT_NO_XRENDER

        // TODO: also check for NET_WM_SUPPORTED atom, before installing shadow

        /*
        From bespin code. Supposibly prevent playing with some 'pseudo-widgets'
        that have winId matching some other -random- window
        */
        if( !(widget->testAttribute(Qt::WA_WState_Created) || widget->internalWinId() ))
        { return false; }

        // create pixmap handles if needed
        createPixmapHandles();

        // make sure that pixmaps are valid
        if( _pixmaps.size() != numPixmaps ) return false;

        // create data
        // add pixmap handles
        QVector<unsigned long> data;
        foreach( const Qt::HANDLE& value, _pixmaps )
        { data.push_back( value ); }

        // add padding
        /*
        all 4 paddings are identical, since offsets are handled when generating the pixmaps.
        there is one extra pixel needed with respect to actual shadow size, to deal with how
        menu backgrounds are rendered
        */
        if( isToolTip( widget ) )
        {

            data << _size << _size << _size << _size;

        } else {

            data << _size - 1 << _size - 1 << _size - 1 << _size - 1;

        }

        XChangeProperty(
            QX11Info::display(), widget->winId(), _atom, XA_CARDINAL, 32, PropModeReplace,
            reinterpret_cast<const unsigned char *>(data.constData()), data.size() );

        return true;

        #endif
        #endif

        return false;

    }

    //_______________________________________________________
    void ShadowHelper::uninstallX11Shadows( QWidget* widget ) const
    {

        #ifdef Q_WS_X11
        if( !widget ) return;
        XDeleteProperty(QX11Info::display(), widget->winId(), _atom);
        #endif

    }

    //_______________________________________________________
    void ShadowHelper::uninstallX11Shadows( WId id ) const
    {

        #ifdef Q_WS_X11
        XDeleteProperty(QX11Info::display(), id, _atom);
        #endif

    }

}
