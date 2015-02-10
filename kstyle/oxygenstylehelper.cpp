/*
 * Copyright 2013 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright 2007 Casper Boemann <cbr@boemann.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "oxygenstylehelper.h"

#include <KColorUtils>
#include <KColorScheme>

#include <QLinearGradient>
#include <QPainter>
#include <QTextStream>

#include <math.h>

#if OXYGEN_HAVE_X11
#include <QX11Info>
#endif

namespace Oxygen
{

    //______________________________________________________________________________
    StyleHelper::StyleHelper( KSharedConfigPtr config ):
        Helper( config )
    { init(); }

    //______________________________________________________________________________
    #if OXYGEN_USE_KDE4
    StyleHelper::StyleHelper( const QByteArray& name ):
        Helper( name )
    { init(); }
    #endif

    //______________________________________________________________________________
    void StyleHelper::invalidateCaches( void )
    {

        _dialSlabCache.clear();
        _roundSlabCache.clear();
        _sliderSlabCache.clear();
        _holeCache.clear();

        _midColorCache.clear();

        _dockWidgetButtonCache.clear();
        _progressBarCache.clear();
        _cornerCache.clear();
        _selectionCache.clear();
        _holeFlatCache.clear();
        _slopeCache.clear();
        _slitCache.clear();
        _dockFrameCache.clear();
        _scrollHoleCache.clear();
        _scrollHandleCache.clear();
        Helper::invalidateCaches();
    }


    //____________________________________________________________________
    void StyleHelper::setMaxCacheSize( int value )
    {

        // base class
        Helper::setMaxCacheSize( value );

        // assign max cache size
        _dialSlabCache.setMaxCacheSize( value );
        _roundSlabCache.setMaxCacheSize( value );
        _sliderSlabCache.setMaxCacheSize( value );
        _holeCache.setMaxCacheSize( value );
        _scrollHandleCache.setMaxCacheSize( value );

        _dockWidgetButtonCache.setMaxCost( value );
        _progressBarCache.setMaxCost( value );
        _cornerCache.setMaxCost( value );
        _selectionCache.setMaxCost( value );
        _holeFlatCache.setMaxCost( value );
        _slopeCache.setMaxCost( value );
        _slitCache.setMaxCost( value );
        _dockFrameCache.setMaxCost( value );
        _scrollHoleCache.setMaxCost( value );

    }

    //____________________________________________________________________
    void StyleHelper::renderWindowBackground( QPainter* painter, const QRect& clipRect, const QWidget* widget, const QColor& color, int y_shift, int gradientHeight)
    {

        if( _useBackgroundGradient )
        {

            // normal background gradient
            Helper::renderWindowBackground( painter, clipRect, widget, widget->window(), color, y_shift, gradientHeight );

        } else {

            // if background gradient is disabled, simply render flat background
            if ( clipRect.isValid() )
            { painter->setClipRegion( clipRect,Qt::IntersectClip ); }

            painter->fillRect( widget->rect(), color );
        }

        // background pixmap
        Helper::renderBackgroundPixmap( painter, clipRect, widget, widget->window(), y_shift, gradientHeight );

    }

    //____________________________________________________________________
    void StyleHelper::setHasBackgroundGradient( WId id, bool value ) const
    { Helper::setHasBackgroundGradient( id, value && _useBackgroundGradient ); }

    //____________________________________________________________________
    void StyleHelper::renderMenuBackground( QPainter* painter, const QRect& clipRect, const QWidget* widget, const QColor& color )
    {

        // get coordinates relative to the client area
        // this is stupid. One could use mapTo if this was taking const QWidget* and not
        // QWidget* as argument.
        const QWidget* w( widget );
        int x( 0 );
        int y( 0 );

        while( !w->isWindow() && w != w->parentWidget() )
        {
            x += w->geometry().x();
            y += w->geometry().y();
            w = w->parentWidget();
        }

        if ( clipRect.isValid() )
        {
            painter->save();
            painter->setClipRegion( clipRect,Qt::IntersectClip );
        }

        // calculate upper part height
        // special tricks are needed
        // to handle both window contents and window decoration
        QRect r = w->rect();
        const int height( w->frameGeometry().height() );
        const int splitY( qMin( 200, ( 3*height )/4 ) );

        const QRect upperRect( QRect( 0, 0, r.width(), splitY ) );
        const QPixmap tile( verticalGradient( color, splitY ) );
        painter->drawTiledPixmap( upperRect, tile );

        const QRect lowerRect( 0,splitY, r.width(), r.height() - splitY );
        painter->fillRect( lowerRect, backgroundBottomColor( color ) );

        if ( clipRect.isValid() )
        { painter->restore(); }

    }

    //____________________________________________________________________________________
    QColor StyleHelper::arrowColor( const QPalette& palette, StyleOptions options, qreal opacity, AnimationMode mode ) const
    {

        QColor glow( palette.color( QPalette::WindowText ) );
        if( mode == AnimationNone || opacity < 0 )
        {

            if( options & Hover ) glow = hoverColor( palette );
            else if( options & Focus ) glow = focusColor( palette );

        } else if( mode == AnimationHover ) {

            // animated color, hover
            if( options & Focus ) glow = focusColor( palette );
            if( glow.isValid() ) glow = KColorUtils::mix( glow,  hoverColor( palette ), opacity );

        } else if( mode == AnimationFocus ) {

            if( options & Hover ) glow = hoverColor( palette );
            if( glow.isValid() ) glow = KColorUtils::mix( glow,  focusColor( palette ), opacity );

        }

        return glow;
    }

    //____________________________________________________________________________________
    QColor StyleHelper::buttonGlowColor( QPalette::ColorGroup colorGroup, StyleOptions options, qreal opacity, AnimationMode mode ) const
    {

        QColor glow;
        if( mode == AnimationNone || opacity < 0 )
        {

            if( options & Hover ) glow = hoverColor( colorGroup );
            else if( options & Focus ) glow = focusColor( colorGroup );

        } else if( mode == AnimationHover ) {

            // animated color, hover
            if( options & Focus ) glow = focusColor( colorGroup );
            if( glow.isValid() ) glow = KColorUtils::mix( glow,  hoverColor( colorGroup ), opacity );
            else glow = alphaColor(  hoverColor( colorGroup ), opacity );

        } else if( mode == AnimationFocus ) {

            if( options & Hover ) glow = hoverColor( colorGroup );
            if( glow.isValid() ) glow = KColorUtils::mix( glow,  focusColor( colorGroup ), opacity );
            else glow = alphaColor(  focusColor( colorGroup ), opacity );

        }

        return glow;
    }

    //____________________________________________________________________________________
    QColor StyleHelper::frameGlowColor( QPalette::ColorGroup colorGroup, StyleOptions options, qreal opacity, AnimationMode mode ) const
    {

        QColor glow;
        if( mode == AnimationNone || opacity < 0 )
        {

            if( options & Focus ) glow = focusColor( colorGroup );
            else if( options & Hover ) glow = hoverColor( colorGroup );

        } else if( mode == AnimationFocus ) {

            if( options & Hover ) glow = hoverColor( colorGroup );
            if( glow.isValid() ) glow = KColorUtils::mix( glow,  focusColor( colorGroup ), opacity );
            else glow = alphaColor(  focusColor( colorGroup ), opacity );

        } else if( mode == AnimationHover ) {

            // animated color, hover
            if( options & Focus ) glow = focusColor( colorGroup );
            if( glow.isValid() ) glow = KColorUtils::mix( glow,  hoverColor( colorGroup ), opacity );
            else glow = alphaColor(  hoverColor( colorGroup ), opacity );

        }

        return glow;
    }

    //______________________________________________________________________________
    QPalette StyleHelper::disabledPalette( const QPalette& source, qreal ratio ) const
    {

        QPalette out( source );
        out.setColor( QPalette::Background, KColorUtils::mix( source.color( QPalette::Active, QPalette::Background ), source.color( QPalette::Disabled, QPalette::Background ), 1.0-ratio ) );
        out.setColor( QPalette::Highlight, KColorUtils::mix( source.color( QPalette::Active, QPalette::Highlight ), source.color( QPalette::Disabled, QPalette::Highlight ), 1.0-ratio ) );
        out.setColor( QPalette::WindowText, KColorUtils::mix( source.color( QPalette::Active, QPalette::WindowText ), source.color( QPalette::Disabled, QPalette::WindowText ), 1.0-ratio ) );
        out.setColor( QPalette::ButtonText, KColorUtils::mix( source.color( QPalette::Active, QPalette::ButtonText ), source.color( QPalette::Disabled, QPalette::ButtonText ), 1.0-ratio ) );
        out.setColor( QPalette::Text, KColorUtils::mix( source.color( QPalette::Active, QPalette::Text ), source.color( QPalette::Disabled, QPalette::Text ), 1.0-ratio ) );
        out.setColor( QPalette::Button, KColorUtils::mix( source.color( QPalette::Active, QPalette::Button ), source.color( QPalette::Disabled, QPalette::Button ), 1.0-ratio ) );
        return out;
    }

    //______________________________________________________________________________
    QPixmap StyleHelper::dockWidgetButton( const QColor& color, bool pressed, int size )
    {
        const quint64 key( ( colorKey(color) << 32 ) | ( size << 1 ) | quint64( pressed ) );
        QPixmap *pixmap = _dockWidgetButtonCache.object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( highDpiPixmap( size, size ) );
            pixmap->fill( Qt::transparent );

            const QColor light( calcLightColor( color ) );
            const QColor dark( calcDarkColor( color ) );

            QPainter painter( pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );
            const qreal u( size/18.0 );
            painter.translate( 0.5*u, ( 0.5-0.668 )*u );

            {
                // outline circle
                qreal penWidth = 1.2;
                QLinearGradient linearGradient( 0, u*( 1.665-penWidth ), 0, u*( 12.33+1.665-penWidth ) );
                linearGradient.setColorAt( 0, dark );
                linearGradient.setColorAt( 1, light );
                QRectF r( u*0.5*( 17-12.33+penWidth ), u*( 1.665+penWidth ), u*( 12.33-penWidth ), u*( 12.33-penWidth ) );
                painter.setPen( QPen( linearGradient, penWidth*u ) );
                painter.drawEllipse( r );
                painter.end();
            }

            _dockWidgetButtonCache.insert( key, pixmap );
        }

        return *pixmap;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::roundCorner( const QColor& color, int size )
    {
        const quint64 key( ( colorKey(color) << 32 )|size );
        TileSet *tileSet = _cornerCache.object( key );

        if ( !tileSet )
        {

            QPixmap pixmap = QPixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setRenderHint( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );

            QLinearGradient linearGradient = QLinearGradient( 0.0, size-4.5, 0.0, size+4.5 );
            linearGradient.setColorAt( 0.50, calcLightColor( backgroundTopColor( color ) ) );
            linearGradient.setColorAt( 0.51, backgroundBottomColor( color ) );

            // draw ellipse.
            painter.setBrush( linearGradient );
            painter.drawEllipse( QRectF( size-4, size-4, 8, 8 ) );

            // mask
            painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            painter.setBrush( Qt::black );
            painter.drawEllipse( QRectF( size-3, size-3, 6, 6 ) );

            tileSet = new TileSet( pixmap, size, size, 1, 1 );
            _cornerCache.insert( key, tileSet );

        }

        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slope( const QColor& color, qreal shade, int size )
    {
        const quint64 key( ( colorKey(color) << 32 )|( quint64( 256.0*shade )<<24 )|size );
        TileSet *tileSet = _slopeCache.object( key );

        if ( !tileSet )
        {

            QPixmap pixmap( highDpiPixmap( size*4 ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setPen( Qt::NoPen );

            // edges
            TileSet *slabTileSet = slab( color, shade, size );
            slabTileSet->render( QRect( 0, 0, size*4, size*5 ), &painter,
                TileSet::Left | TileSet::Right | TileSet::Top );

            int fixedSize( 28*devicePixelRatio( pixmap ) );
            painter.setWindow( 0, 0, fixedSize, fixedSize );

            // bottom
            QColor light = KColorUtils::shade( calcLightColor( color ), shade );
            QLinearGradient fillGradient( 0, -28, 0, 28 );
            light.setAlphaF( 0.4 ); fillGradient.setColorAt( 0.0, light );
            light.setAlphaF( 0.0 ); fillGradient.setColorAt( 1.0, light );
            painter.setBrush( fillGradient );
            painter.setCompositionMode( QPainter::CompositionMode_DestinationOver );
            painter.drawRect( 3, 9, 22, 17 );

            // fade bottom
            QLinearGradient maskGradient( 0, 7, 0, 28 );
            maskGradient.setColorAt( 0.0, Qt::black );
            maskGradient.setColorAt( 1.0, Qt::transparent );

            painter.setBrush( maskGradient );
            painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
            painter.drawRect( 0, 9, 28, 19 );

            painter.end();

            tileSet = new TileSet( pixmap, size, size, size*2, 2 );

            _slopeCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //__________________________________________________________________________________________________________
    TileSet* StyleHelper::progressBarIndicator( const QPalette& pal, int dimension )
    {

        const QColor highlight( pal.color( QPalette::Highlight ) );
        const quint64 key( ( colorKey(highlight) << 32 ) | dimension );

        TileSet *tileSet = _progressBarCache.object( key );
        if ( !tileSet )
        {

            QRect local( 0, 0, dimension, dimension );

            QPixmap pixmap( highDpiPixmap( local.size() ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setBrush( Qt::NoBrush );

            const QColor lhighlight( calcLightColor( highlight ) );
            const QColor color( pal.color( QPalette::Active, QPalette::Window ) );
            const QColor light( calcLightColor( color ) );
            const QColor dark( calcDarkColor( color ) );
            const QColor shadow( calcShadowColor( color ) );

            // shadow
            {
                painter.setPen( QPen( alphaColor( shadow, 0.4 ),0.6 ) );
                painter.drawRoundedRect( QRectF( local ).adjusted( 0.5, 0.5, -0.5, 0.5 ), 3.0, 3.0 );
            }

            // fill
            local.adjust( 1, 1, -1, 0 );
            {
                painter.setPen( Qt::NoPen );
                painter.setBrush( KColorUtils::mix( highlight, dark, 0.2 ) );
                painter.drawRoundedRect( local, 2.5, 2.5 );
            }

            // fake radial gradient
            {
                QPixmap pixmap( highDpiPixmap( local.size() ) );
                pixmap.fill( Qt::transparent );
                {
                    QRect pixmapRect( QPoint(0, 0), local.size() );
                    QLinearGradient mask( pixmapRect.topLeft(), pixmapRect.topRight() );
                    mask.setColorAt( 0.0, Qt::transparent );
                    mask.setColorAt( 0.4, Qt::black );
                    mask.setColorAt( 0.6, Qt::black );
                    mask.setColorAt( 1.0, Qt::transparent );

                    QLinearGradient radial( pixmapRect.topLeft(), pixmapRect.bottomLeft() );
                    radial.setColorAt( 0.0, KColorUtils::mix( lhighlight, light, 0.3 ) );
                    radial.setColorAt( 0.5, Qt::transparent );
                    radial.setColorAt( 0.6, Qt::transparent );
                    radial.setColorAt( 1.0, KColorUtils::mix( lhighlight, light, 0.3 ) );

                    QPainter painter( &pixmap );
                    painter.fillRect( pixmap.rect(), mask );
                    painter.setCompositionMode( QPainter::CompositionMode_SourceIn );
                    painter.fillRect( pixmapRect, radial );
                    painter.end();

                }

                painter.drawPixmap( QPoint( 1,1 ), pixmap );

            }

            // bevel
            {
                QLinearGradient bevel( QPointF( 0, 0.5 ) + local.topLeft(), QPointF( 0, -0.5 ) + local.bottomLeft() );
                bevel.setColorAt( 0, lhighlight );
                bevel.setColorAt( 0.5, highlight );
                bevel.setColorAt( 1, calcDarkColor( highlight ) );
                painter.setBrush( Qt::NoBrush );
                painter.setPen( QPen( bevel, 1 ) );
                painter.drawRoundedRect( QRectF(local).adjusted( 0.5, 0.5, -0.5, -0.5 ), 2.5, 2.5 );
            }

            // bright top edge
            {
                QLinearGradient lightHl( local.topLeft(),local.topRight() );
                lightHl.setColorAt( 0, Qt::transparent );
                lightHl.setColorAt( 0.5, KColorUtils::mix( highlight, light, 0.8 ) );
                lightHl.setColorAt( 1, Qt::transparent );

                painter.setPen( QPen( lightHl, 1 ) );
                painter.drawLine( QPointF( 0.5, 0.5 ) + local.topLeft(), QPointF( 0.5, 0.5 ) + local.topRight() );
            }

            painter.end();

            // generate tileSet and save in cache
            const int radius = qMin( 3, dimension/2 );
            tileSet = new TileSet( pixmap, radius, radius, dimension-2*radius, dimension-2*radius );
            _progressBarCache.insert( key, tileSet );
        }

        return tileSet;

    }

    //______________________________________________________________________________
    QPixmap StyleHelper::dialSlab( const QColor& color, const QColor& glow, qreal shade, int size )
    {
        Oxygen::Cache<QPixmap>::Value* cache =  _dialSlabCache.get( color );

        const quint64 key( ( colorKey(glow) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );
        if ( !pixmap )
        {
            pixmap = new QPixmap( highDpiPixmap( size ) );
            pixmap->fill( Qt::transparent );

            QRectF rect( 0, 0, size, size );

            QPainter painter( pixmap );
            painter.setPen( Qt::NoPen );
            painter.setRenderHints( QPainter::Antialiasing );

            // colors
            const QColor base( KColorUtils::shade( color, shade ) );
            const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
            const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
            const QColor mid( KColorUtils::shade( calcMidColor( color ), shade ) );
            const QColor shadow( calcShadowColor( color ) );

            // shadow
            drawShadow( painter, shadow, rect.width() );

            if( glow.isValid() )
            { drawOuterGlow( painter, glow, rect.width() ); }

            const qreal baseOffset( 3.5 );
            {
                //plain background
                QLinearGradient linearGradient( 0, baseOffset-0.5*rect.height(), 0, baseOffset+rect.height() );
                linearGradient.setColorAt( 0, light );
                linearGradient.setColorAt( 0.8, base );

                painter.setBrush( linearGradient );
                const qreal offset( baseOffset );
                painter.drawEllipse( rect.adjusted( offset, offset, -offset, -offset ) );
            }

            {
                // outline circle
                const qreal penWidth( 0.7 );
                QLinearGradient linearGradient( 0, baseOffset, 0, baseOffset + 2*rect.height() );
                linearGradient.setColorAt( 0, light );
                linearGradient.setColorAt( 1, mid );
                painter.setBrush( Qt::NoBrush );
                painter.setPen( QPen( linearGradient, penWidth ) );
                const qreal offset( baseOffset+0.5*penWidth );
                painter.drawEllipse( rect.adjusted( offset, offset, -offset, -offset ) );
            }


            cache->insert( key, pixmap );

        }

        return *pixmap;

    }

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::roundSlab( const QColor& color, const QColor& glow, qreal shade, int size )
    {

        Oxygen::Cache<QPixmap>::Value* cache( _roundSlabCache.get( color ) );

        const quint64 key( ( colorKey(glow) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( highDpiPixmap( size*3 ) );
            pixmap->fill( Qt::transparent );

            QPainter painter( pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );

            const int fixedSize( 21*devicePixelRatio( *pixmap ) );
            painter.setWindow( 0, 0, fixedSize, fixedSize );

            // draw normal shadow
            drawShadow( painter, calcShadowColor( color ), 21 );

            // draw glow.
            if( glow.isValid() )
            { drawOuterGlow( painter, glow, 21 ); }

            drawRoundSlab( painter, color, shade );

            painter.end();
            cache->insert( key, pixmap );

        }
        return *pixmap;
    }

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::sliderSlab( const QColor& color, const QColor& glow, bool sunken, qreal shade, int size )
    {

        Oxygen::Cache<QPixmap>::Value* cache( _sliderSlabCache.get( color ) );

        const quint64 key( ( colorKey(glow) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | (sunken << 23 ) | size );
        QPixmap *pixmap = cache->object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( highDpiPixmap( size*3 ) );
            pixmap->fill( Qt::transparent );

            QPainter painter( pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );

            if( color.isValid() ) drawShadow( painter, alphaColor( calcShadowColor( color ), 0.8 ), 21 );
            if( glow.isValid() ) drawOuterGlow( painter, glow, 21 );

            // draw slab
            drawSliderSlab( painter, color, sunken, shade );

            painter.end();
            cache->insert( key, pixmap );

        }
        return *pixmap;
    }

    //______________________________________________________________________________
    void StyleHelper::renderDebugFrame( QPainter* painter, const QRect& rect ) const
    {
        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );
        painter->setBrush( Qt::NoBrush );
        painter->setPen( Qt::red );
        painter->drawRect( QRectF( rect ).adjusted( 0.5, 0.5, -0.5, -0.5 ) );
        painter->restore();
    }

    //________________________________________________________________________________________________________
    void StyleHelper::fillHole( QPainter& painter, const QRect& rect, int offset ) const
    { painter.drawRoundedRect( rect.adjusted( offset, offset, -offset, -offset ), 4 - offset, 4 - offset ); }

    //____________________________________________________________________________________
    void StyleHelper::renderHole( QPainter* painter, const QColor& base, const QRect& rect, StyleOptions options, qreal opacity, Oxygen::AnimationMode mode, TileSet::Tiles tiles )
    {
        if( !rect.isValid() ) return;
        const QColor glow( frameGlowColor( QPalette::Active, options, opacity, mode ) );
        hole( base, glow, TileSet::DefaultSize, options )->render( rect, painter, tiles );
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::holeFlat( const QColor& color, qreal shade, bool fill, int size )
    {
        const quint64 key( ( colorKey(color) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size << 1 | fill );
        TileSet *tileSet = _holeFlatCache.object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( highDpiPixmap( size*2 ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );

            const int fixedSize( 14*devicePixelRatio( pixmap ) );
            painter.setWindow( 0, 0, fixedSize, fixedSize );

            if( fill )
            {

                // hole inside
                painter.setBrush( color );
                painter.drawRoundedRect( QRectF( 1, 0, 12, 13 ), 3.0, 3.0 );
                painter.setBrush( Qt::NoBrush );

                {
                    // shadow (top)
                    const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
                    QLinearGradient gradient( 0, -2, 0, 14 );
                    gradient.setColorAt( 0.0, dark );
                    gradient.setColorAt( 0.5, Qt::transparent );

                    painter.setPen( QPen( gradient, 1 ) );
                    painter.drawRoundedRect( QRectF( 1.5, 0.5, 11, 12 ), 2.5, 2.5 );
                }

                {

                    // contrast (bottom)
                    const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
                    QLinearGradient gradient( 0, 0, 0, 18 );
                    gradient.setColorAt( 0.5, Qt::transparent );
                    gradient.setColorAt( 1.0, light );

                    painter.setPen( QPen( gradient, 1 ) );
                    painter.drawRoundedRect( QRectF( 0.5, 0.5, 13, 13 ), 3.5, 3.5 );

                }

            } else {

                // hole inside
                painter.setBrush( color );
                painter.drawRoundedRect( QRectF( 2, 2, 10, 10 ), 3.0, 3.0 );
                painter.setBrush( Qt::NoBrush );

                {
                    // shadow (top)
                    const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
                    QLinearGradient gradient( 0, 1, 0, 12 );
                    gradient.setColorAt( 0.0, dark );
                    gradient.setColorAt( 0.5, Qt::transparent );

                    painter.setPen( QPen( gradient, 1 ) );
                    painter.drawRoundedRect( QRectF( 2.5, 2.5, 10, 10 ), 2.5, 2.5 );
                }

                {
                    // contrast (bottom)
                    const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
                    QLinearGradient gradient( 0, 1, 0, 12 );
                    gradient.setColorAt( 0.5, Qt::transparent );
                    gradient.setColorAt( 1.0, light );

                    painter.setPen( QPen( gradient, 1 ) );
                    painter.drawRoundedRect( QRectF( 2, 1.5, 10, 11 ), 3.0, 2.5 );

                }

            }

            painter.end();

            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );

            _holeFlatCache.insert( key, tileSet );
        }


        return tileSet;
    }


    //______________________________________________________________________________
    TileSet *StyleHelper::scrollHole( const QColor& color, Qt::Orientation orientation, bool smallShadow )
    {

        const quint64 key( colorKey(color) << 32 | ( orientation == Qt::Horizontal ? 2 : 0 ) | ( smallShadow ? 1 : 0 ) );
        TileSet *tileSet = _scrollHoleCache.object( key );
        if ( !tileSet )
        {
            QPixmap pixmap( highDpiPixmap( 15 ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );

            const QColor dark( calcDarkColor( color ) );
            const QColor light( calcLightColor( color ) );
            const QColor shadow( calcShadowColor( color ) );

            // use space for white border
            const QRect pixmapRect( 0, 0, 15, 15 );
            const QRect rect( pixmapRect.adjusted( 1, 1, -1, -1 ) );

            painter.setRenderHints( QPainter::Antialiasing );
            painter.setBrush( dark );
            painter.setPen( Qt::NoPen );

            // base
            const qreal radius( smallShadow ? 2.5:3.0 );
            painter.drawRoundedRect( rect, radius, radius );

            // slight shadow across the whole hole
            if( true )
            {
                QLinearGradient shadowGradient( rect.topLeft(),
                    orientation == Qt::Horizontal ?
                    rect.bottomLeft():rect.topRight() );

                shadowGradient.setColorAt( 0.0, alphaColor( shadow, 0.1 ) );
                shadowGradient.setColorAt( 0.6, Qt::transparent );
                painter.setBrush( shadowGradient );
                painter.drawRoundedRect( rect, radius, radius );

            }

            // first create shadow
            int shadowSize( 5 );
            QPixmap shadowPixmap( highDpiPixmap( shadowSize*2 ) );

            {
                shadowPixmap.fill( Qt::transparent );

                QPainter painter( &shadowPixmap );
                painter.setRenderHints( QPainter::Antialiasing );
                painter.setPen( Qt::NoPen );

                // fade-in shadow
                QColor shadowColor( calcShadowColor( color ) );
                if( smallShadow ) shadowColor = alphaColor( shadowColor, 0.6 );
                drawInverseShadow( painter, shadowColor, 1, 8, 0.0 );

                painter.end();

            }

            // render shadow
            TileSet(
                shadowPixmap, shadowSize, shadowSize, shadowSize,
                shadowSize, shadowSize-1, shadowSize, 2, 1 ).
                render( rect.adjusted( -1, -1, 1, 1 ), &painter, TileSet::Full );

            // light border
            QLinearGradient borderGradient( 0, pixmapRect.top(), 0, pixmapRect.bottom() );
            if( smallShadow && orientation == Qt::Vertical )
            {

                borderGradient.setColorAt( 0.8, Qt::transparent );
                borderGradient.setColorAt( 1.0, alphaColor( light, 0.5 ) );

            } else {

                borderGradient.setColorAt( 0.5, Qt::transparent );
                borderGradient.setColorAt( 1.0, alphaColor( light, 0.6 ) );

            }

            painter.setPen( QPen( borderGradient, 1.0 ) );
            painter.setBrush( Qt::NoBrush );
            painter.drawRoundedRect( QRectF( pixmapRect ).adjusted( 0.5, 0.5, -0.5, -0.5 ), radius+0.5, radius+0.5 );

            painter.end();
            tileSet = new TileSet( pixmap, 7, 7, 1, 1 );

            _scrollHoleCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::scrollHandle( const QColor& color, const QColor& glow, int size)
    {

        // get key
        Oxygen::Cache<TileSet>::Value* cache( _scrollHandleCache.get( glow ) );

        const quint64 key( ( colorKey(color) << 32 ) | size );
        TileSet *tileSet = cache->object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( highDpiPixmap( 2*size ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );

            const int fixedSize( 14*devicePixelRatio( pixmap ) );
            painter.setWindow( 0, 0, fixedSize, fixedSize );

            QPixmap shadowPixmap( highDpiPixmap( 10 ) );
            {

                shadowPixmap.fill( Qt::transparent );

                QPainter painter( &shadowPixmap );
                painter.setRenderHints( QPainter::Antialiasing );
                painter.setPen( Qt::NoPen );

                // shadow/glow
                drawOuterGlow( painter, glow, 10 );

                painter.end();
            }

            TileSet( shadowPixmap, 4, 4, 1, 1 ).render( QRect( 0, 0, 14, 14 ), &painter, TileSet::Full );

            // outline
            {
                const QColor mid( calcMidColor( color ) );
                QLinearGradient linearGradient( 0, 3, 0, 11 );
                linearGradient.setColorAt( 0, color );
                linearGradient.setColorAt( 1, mid );
                painter.setPen( Qt::NoPen );
                painter.setBrush( linearGradient );
                painter.drawRoundedRect( QRectF( 3, 3, 8, 8 ), 2.5, 2.5 );
            }

            // contrast
            {
                const QColor light( calcLightColor( color ) );
                QLinearGradient linearGradient( 0, 3, 0, 11 );
                linearGradient.setColorAt( 0., alphaColor( light, 0.9 ) );
                linearGradient.setColorAt( 0.5, alphaColor( light, 0.44 ) );
                painter.setBrush( linearGradient );
                painter.drawRoundedRect( QRectF( 3, 3, 8, 8 ), 2.5, 2.5 );
            }

            painter.end();

            // create tileset and return
            tileSet = new TileSet( pixmap, size-1, size, 1, 1 );
            cache->insert( key, tileSet );

        }

        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slitFocused( const QColor& glow )
    {
        const quint64 key( ( colorKey(glow) << 32 ) );
        TileSet *tileSet = _slitCache.object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( highDpiPixmap( 9 ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( glow );
            painter.drawRoundedRect( QRectF( 1.5, 1.5, 6, 6 ), 2.5, 2.5 );
            painter.end();

            tileSet = new TileSet( pixmap, 4, 4, 1, 1 );

            _slitCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //____________________________________________________________________
    TileSet *StyleHelper::dockFrame( const QColor& top, const QColor& bottom )
    {
        const quint64 key( colorKey(top) << 32 | colorKey(bottom) );
        TileSet *tileSet = _dockFrameCache.object( key );
        if ( !tileSet )
        {

            int size( 13 );
            QPixmap pm( size, size );
            pm.fill( Qt::transparent );

            QPainter painter( &pm );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setBrush( Qt::NoBrush );

            const QColor lightTop = alphaColor( calcLightColor( top ), 0.5 );
            const QColor lightBottom = alphaColor( calcLightColor( bottom ), 0.5 );
            const QColor darkTop = alphaColor( calcDarkColor( top ), 0.6 );
            const QColor darkBottom = alphaColor( calcDarkColor( bottom ), 0.6 );

            // dark frame
            {
                QLinearGradient linearGradient( 0, 0.5, 0, size-1.5 );
                linearGradient.setColorAt( 0.0, darkTop );
                linearGradient.setColorAt( 1.0, darkBottom );

                painter.setPen( QPen( linearGradient, 1 ) );
                painter.drawRoundedRect( QRectF( 1.5, 0.5, size-3, size-2 ), 4, 4 );
            }

            // bottom contrast
            {
                QLinearGradient linearGradient( 0, 0.5, 0, size-0.5 );
                linearGradient.setColorAt( 0.0, Qt::transparent );
                linearGradient.setColorAt( 1.0, lightBottom );
                painter.setPen( QPen( linearGradient, 1.0 ) );
                painter.drawRoundedRect( QRectF( 0.5, 0.5, size-1, size-1 ), 4.5, 4.5 );
            }

            // top contrast
            {
                QLinearGradient linearGradient( 0, 1.5, 0, size-2.5 );
                linearGradient.setColorAt( 0.0, lightTop );
                linearGradient.setColorAt( 1.0, Qt::transparent );
                painter.setPen( QPen( linearGradient, 1.0 ) );
                painter.drawRoundedRect( QRectF( 2.5, 1.5, size-5, size-4 ), 3.5, 3.5 );
            }

            painter.end();
            tileSet = new TileSet( pm, (size-1)/2, (size-1)/2, 1, 1 );

            _dockFrameCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //____________________________________________________________________
    TileSet *StyleHelper::selection( const QColor& color, int height, bool custom )
    {

        const quint64 key( ( colorKey(color) << 32 ) | ( height << 1 ) | custom );
        TileSet *tileSet = _selectionCache.object( key );
        if ( !tileSet )
        {

            const qreal rounding( 2.5 );

            QPixmap pixmap( highDpiPixmap( 32+16, height ) );
            pixmap.fill( Qt::transparent );

            QRectF r( 0, 0, 32+16, height );

            QPainter painter( &pixmap );
            painter.setRenderHint( QPainter::Antialiasing );

            // items with custom background brushes always have their background drawn
            // regardless of whether they are hovered or selected or neither so
            // the gradient effect needs to be more subtle

            {
                // fill
                const int lightenAmount( custom ? 110 : 130 );
                QLinearGradient gradient( 0, 0, 0, r.bottom() );
                gradient.setColorAt( 0, color.lighter( lightenAmount ) );
                gradient.setColorAt( 1, color );

                painter.setPen( Qt::NoPen );
                painter.setBrush( gradient );
                painter.drawRoundedRect( r, rounding+0.5, rounding+0.5 );

            }

            {
                // contrast
                QLinearGradient gradient( 0, 0, 0, r.bottom() );
                gradient.setColorAt( 0, color );
                gradient.setColorAt( 1, Qt::transparent );

                r.adjust( 0.5, 0.5, -0.5, -0.5 );
                painter.setPen( QPen( color, 1 ) );
                painter.setBrush( Qt::NoBrush );
                painter.drawRoundedRect( r, rounding, rounding );
            }

            tileSet = new TileSet( pixmap, 8, 0, 32, height );
            _selectionCache.insert( key, tileSet );

        }

        return tileSet;

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseGlow(
        QPainter& painter, const QColor& color,
        int pad, int size, int rsize ) const
    {

        const QRectF r( pad, pad, size, size );
        const qreal m( qreal( size )*0.5 );

        const qreal width( 3.5 );
        const qreal bias( _glowBias*7.0/rsize );
        const qreal k0( ( m-width )/( m-bias ) );
        QRadialGradient glowGradient( pad+m, pad+m, m-bias );
        for ( int i = 0; i < 8; i++ )
        {
            // inverse parabolic gradient
            qreal k1 = ( k0 * qreal( i ) + qreal( 8 - i ) ) * 0.125;
            qreal a = 1.0 - sqrt( i * 0.125 );
            glowGradient.setColorAt( k1, alphaColor( color, a ) );

        }

        glowGradient.setColorAt( k0, alphaColor( color, 0.0 ) );
        painter.setBrush( glowGradient );
        painter.drawEllipse( r );
    }

    //________________________________________________________________________________________________________
    bool StyleHelper::compositingActive( void ) const
    {
        #if OXYGEN_HAVE_X11
        if( isX11() )
        {

            // direct call to X
            xcb_get_selection_owner_cookie_t cookie( xcb_get_selection_owner( connection(), _compositingManagerAtom ) );
            ScopedPointer<xcb_get_selection_owner_reply_t> reply( xcb_get_selection_owner_reply( connection(), cookie, nullptr ) );
            return reply && reply->owner;

        } else {

            // use KWindowSystem
            return KWindowSystem::compositingActive();

        }
        #else
        // use KWindowSystem
        return KWindowSystem::compositingActive();
        #endif
    }

    //________________________________________________________________________________________________________
    bool StyleHelper::hasDecoration( const QWidget* widget ) const
    {
        if( !widget->isTopLevel() ) return false;
        if( widget->windowFlags() & (Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint) )
        { return false; }
        return true;
    }


    //________________________________________________________________________________________________________
    TileSet *StyleHelper::hole( const QColor& color, const QColor& glow, int size, StyleOptions options )
    {

        // get key
        Oxygen::Cache<TileSet>::Value* cache( _holeCache.get( glow ) );

        const quint64 key( ( colorKey(color) << 32 ) | (size << 4) | options );
        TileSet *tileSet = cache->object( key );

        if ( !tileSet )
        {

            // first create shadow
            const int shadowSize( (size*5)/7 );
            QPixmap shadowPixmap( highDpiPixmap( shadowSize*2 ) );

            // calc alpha channel and fade
            const int alpha( glow.isValid() ? glow.alpha():0 );

            {
                shadowPixmap.fill( Qt::transparent );

                QPainter painter( &shadowPixmap );
                painter.setRenderHints( QPainter::Antialiasing );
                painter.setPen( Qt::NoPen );
                const int fixedSize( 10*devicePixelRatio( shadowPixmap ) );
                painter.setWindow( 0, 0, fixedSize, fixedSize );

                // fade-in shadow
                if( alpha < 255 )
                {
                    QColor shadowColor( calcShadowColor( color ) );
                    shadowColor.setAlpha( 255-alpha );
                    drawInverseShadow( painter, shadowColor, 1, 8, 0.0 );
                }

                // fade-out glow
                if( alpha > 0 )
                { drawInverseGlow( painter, glow, 1, 8, shadowSize ); }

                painter.end();

            }

            // create pixmap
            QPixmap pixmap( highDpiPixmap( size*2 ) );
            pixmap.fill( Qt::transparent );

            QPainter painter( &pixmap );
            painter.setRenderHints( QPainter::Antialiasing );
            painter.setPen( Qt::NoPen );
            const int fixedSize( 14*devicePixelRatio( pixmap ) );
            painter.setWindow( 0, 0, fixedSize, fixedSize );

            // hole mask
            painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            painter.setBrush( Qt::black );

            painter.drawRoundedRect( QRectF( 1, 1, 12, 12 ), 2.5, 2.5 );
            painter.setCompositionMode( QPainter::CompositionMode_SourceOver );

            // render shadow
            TileSet(
                shadowPixmap, shadowSize, shadowSize, shadowSize,
                shadowSize, shadowSize-1, shadowSize, 2, 1 ).
                render( QRect( QPoint(0, 0), pixmap.size()/devicePixelRatio( pixmap ) ), &painter );

            if( (options&HoleOutline) && alpha < 255 )
            {
                QColor dark( calcDarkColor( color ) );
                dark.setAlpha( 255 - alpha );
                QLinearGradient blend( 0, 0, 0, 14 );
                blend.setColorAt( 0, Qt::transparent );
                blend.setColorAt( 0.8, dark );

                painter.setBrush( Qt::NoBrush );
                painter.setPen( QPen( blend, 1 ) );
                painter.drawRoundedRect( QRectF( 1.5, 1.5, 11, 11 ), 3.0, 3.0 );
                painter.setPen( Qt::NoPen );
            }

            if( options&HoleContrast )
            {
                QColor light( calcLightColor( color ) );
                QLinearGradient blend( 0, 0, 0, 18 );
                blend.setColorAt( 0.5, Qt::transparent );
                blend.setColorAt( 1.0, light );

                painter.setBrush( Qt::NoBrush );
                painter.setPen( QPen( blend, 1 ) );
                painter.drawRoundedRect( QRectF( 0.5, 0.5, 13, 13 ), 4.0, 4.0 );
                painter.setPen( Qt::NoPen );
            }

            painter.end();

            // create tileset and return
            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );
            cache->insert( key, tileSet );
        }

        return tileSet;

    }

    //__________________________________________________________________________________________________________
    void StyleHelper::drawRoundSlab( QPainter& painter, const QColor& color, qreal shade )
    {

        painter.save();

        // colors
        const QColor base( KColorUtils::shade( color, shade ) );
        const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );

        // bevel, part 1
        QLinearGradient bevelGradient1( 0, 10, 0, 18 );
        bevelGradient1.setColorAt( 0.0, light );
        bevelGradient1.setColorAt( 0.9, alphaColor( light, 0.85 ) );
        painter.setBrush( bevelGradient1 );
        painter.drawEllipse( QRectF( 3.0,3.0,15.0,15.0 ) );

        // bevel, part 2
        if ( _slabThickness > 0.0 ) {
            QLinearGradient bevelGradient2( 0, 7, 0, 28 );
            bevelGradient2.setColorAt( 0.0, light );
            bevelGradient2.setColorAt( 0.9, base );
            painter.setBrush( bevelGradient2 );
            painter.drawEllipse( QRectF( 3.6,3.6,13.8,13.8 ) );
        }

        // inside
        QLinearGradient innerGradient( 0, -17, 0, 20 );
        innerGradient.setColorAt( 0, light );
        innerGradient.setColorAt( 1, base );
        painter.setBrush( innerGradient );
        const qreal ic( 3.6 + _slabThickness );
        const qreal is( 21.0 - 2.0*ic );
        painter.drawEllipse( QRectF( ic, ic, is, is ) );

        painter.restore();

    }

    //__________________________________________________________________________________________________________
    void StyleHelper::drawSliderSlab( QPainter& painter, const QColor& color, bool sunken, qreal shade )
    {

        painter.save();

        const QColor light( KColorUtils::shade( calcLightColor(color), shade ) );
        const QColor dark( KColorUtils::shade( calcDarkColor(color), shade ) );

        painter.setPen(Qt::NoPen);

        {
            //plain background
            QLinearGradient linearGradient( 0, 3, 0, 21 );
            linearGradient.setColorAt( 0, light );
            linearGradient.setColorAt( 1, dark );

            const QRectF r( 3, 3, 15, 15 );
            painter.setBrush( linearGradient );
            painter.drawEllipse( r );

        }

        if( sunken )
        {
            //plain background
            QLinearGradient linearGradient( 0, 3, 0, 21 );
            linearGradient.setColorAt( 0, dark );
            linearGradient.setColorAt( 1, light );

            const QRectF r( 5, 5, 11, 11 );
            painter.setBrush( linearGradient );
            painter.drawEllipse( r );

        }

        {
            // outline circle
            const qreal penWidth( 1 );
            QLinearGradient linearGradient( 0, 3, 0, 30 );
            linearGradient.setColorAt( 0, light );
            linearGradient.setColorAt( 1, dark );

            const QRectF r( 3.5, 3.5, 14, 14 );
            painter.setPen( QPen( linearGradient, penWidth ) );
            painter.setBrush( Qt::NoBrush );
            painter.drawEllipse( r );
        }

        painter.restore();

    }

    //______________________________________________________________________________
    void StyleHelper::init( void )
    {

        _useBackgroundGradient = true;

        #if OXYGEN_HAVE_X11
        if( isX11() )
        {
            // create compositing screen
            const QString atomName( QStringLiteral( "_NET_WM_CM_S%1" ).arg( QX11Info::appScreen() ) );
            _compositingManagerAtom = createAtom( atomName );
        }
        #endif
    }

}
