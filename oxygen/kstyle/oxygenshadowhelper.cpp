//////////////////////////////////////////////////////////////////////////////
// oxygenshadowhelper.h
// handle shadow pixmaps passed to window manager via X property
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
//
// Loosely inspired (and largely rewritten) from BeSpin style
// Copyright (C) 2007 Thomas Luebking <thomas.luebking@web.de>
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

#include <QtGui/QMenu>
#include <QtCore/QTextStream>
#include <QtCore/QEvent>

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

namespace Oxygen
{

    //_____________________________________________________
    ShadowHelper::ShadowHelper( QObject* parent, Helper& helper ):
        QObject( parent ),
        _shadowCache( new ShadowCache( helper ) ),
        _shadowSize( 0 ),
        _atom( None )
    {

        #ifdef Q_WS_X11
        // create atom
        _atom = XInternAtom( QX11Info::display(), "_KDE_NET_WM_SHADOW", False);
        #endif

    }

    //_______________________________________________________
    ShadowHelper::~ShadowHelper( void )
    {

        // unregister all stored widgets
        /*
        for( QMap<QWidget*,WId>::const_iterator iter = _widgets.begin(); iter != _widgets.end(); iter++ )
        { uninstallX11Shadows( iter.value() ); }
        */

        // delete shadow cache
        delete _shadowCache;

    }

    //_______________________________________________________
    bool ShadowHelper::registerWidget( QWidget* widget )
    {

        // make sure widget is not already registered
        if( _widgets.contains( widget ) ) return false;

        // check widget type
        if( !( qobject_cast<QMenu*>( widget ) || widget->inherits( "QComboBoxPrivateContainer" ) ) )
        { return false; }

        // store in map and add destroy signal connection
        widget->removeEventFilter( this );
        widget->installEventFilter( this );

        _widgets.insert( widget, 0 );
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

        // reset tileset and shadow size
        _shadows = *shadowCache().tileSet( ShadowCache::Key() );
        _shadowSize = shadowCache().shadowSize();

        // update property for registered widgets
        for( QMap<QWidget*,WId>::const_iterator iter = _widgets.begin(); iter != _widgets.end(); iter++ )
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

        return true;

    }

    //_______________________________________________________
    void ShadowHelper::objectDeleted( QObject* object )
    { _widgets.remove( static_cast<QWidget*>( object ) ); }

    //_______________________________________________________
    bool ShadowHelper::installX11Shadows( QWidget* widget ) const
    {

        /*!
        shadow atom and property specification available at
        http://community.kde.org/KWin/Shadow
        */

        #ifdef Q_WS_X11
        #ifndef QT_NO_XRENDER

        // check widget and shadow
        if( !( widget && _shadows.isValid() ) )
        { return false; }

        // TODO: also check for NET_WM_SUPPORTED atom, before installing shadow

        /*
        From bespin code. Supposibly prevent playing with some 'pseudo-widgets'
        that have winId matching some other -random- window
        */
        if( !(widget->testAttribute(Qt::WA_WState_Created) || widget->internalWinId() ))
        { return false; }

        // create data
        // add pixmap handles
        QVector<unsigned long> data;
        data
            << _shadows.pixmap( 1 ).handle() // top
            << _shadows.pixmap( 2 ).handle() // top-right
            << _shadows.pixmap( 5 ).handle() // right
            << _shadows.pixmap( 8 ).handle() // bottom-right
            << _shadows.pixmap( 7 ).handle() // bottom
            << _shadows.pixmap( 6 ).handle() // bottom left
            << _shadows.pixmap( 3 ).handle() // left
            << _shadows.pixmap( 0 ).handle();

        // add padding
        /* all 4 paddings are identical, since offsets are handled when generating the pixmaps */
        data << _shadowSize << _shadowSize << _shadowSize << _shadowSize;

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
