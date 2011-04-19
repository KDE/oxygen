/*
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

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

#include <math.h>

namespace Oxygen
{

    //______________________________________________________________________________
    StyleHelper::StyleHelper( const QByteArray &componentName ):
        Helper( componentName ),
        _debugArea( KDebug::registerArea( "Oxygen ( style )" ) )
    {
        _dockFrameCache.setMaxCost( 1 );
        _scrollHoleCache.setMaxCost( 10 );
    }

    //______________________________________________________________________________
    void StyleHelper::invalidateCaches( void )
    {

        _dialSlabCache.clear();
        _roundSlabCache.clear();
        _holeFocusedCache.clear();

        _midColorCache.clear();

        _progressBarCache.clear();
        _cornerCache.clear();
        _selectionCache.clear();
        _slabSunkenCache.clear();
        _slabInvertedCache.clear();
        _holeCache.clear();
        _holeFlatCache.clear();
        _slopeCache.clear();
        _grooveCache.clear();
        _slitCache.clear();
        _dockFrameCache.clear();
        _scrollHoleCache.clear();
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
        _holeFocusedCache.setMaxCacheSize( value );

        _progressBarCache.setMaxCost( value );
        _cornerCache.setMaxCost( value );
        _selectionCache.setMaxCost( value );
        _slabSunkenCache.setMaxCost( value );
        _slabInvertedCache.setMaxCost( value );
        _holeCache.setMaxCost( value );
        _holeFlatCache.setMaxCost( value );
        _slopeCache.setMaxCost( value );
        _grooveCache.setMaxCost( value );
        _slitCache.setMaxCost( value );
        _dockFrameCache.setMaxCost( value );
        _scrollHoleCache.setMaxCost( value );

    }

    //____________________________________________________________________
    void StyleHelper::renderMenuBackground( QPainter* p, const QRect& clipRect, const QWidget* widget, const QColor& color )
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

        if ( clipRect.isValid() ) {
            p->save();
            p->setClipRegion( clipRect,Qt::IntersectClip );
        }

        // calculate upper part height
        // special tricks are needed
        // to handle both window contents and window decoration
        QRect r = w->rect();
        const int height( w->frameGeometry().height() );
        const int splitY( qMin( 200, ( 3*height )/4 ) );

        const QRect upperRect( QRect( 0, 0, r.width(), splitY ) );
        const QPixmap tile( verticalGradient( color, splitY ) );
        p->drawTiledPixmap( upperRect, tile );

        const QRect lowerRect( 0,splitY, r.width(), r.height() - splitY );
        p->fillRect( lowerRect, backgroundBottomColor( color ) );

        if ( clipRect.isValid() )
        { p->restore(); }

    }

    //______________________________________________________________________________
    QPalette StyleHelper::mergePalettes( const QPalette& source, qreal ratio ) const
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
    QPixmap StyleHelper::windecoButton( const QColor& color, bool pressed, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) | ( size << 1 ) | quint64( pressed ) );
        QPixmap *pixmap = windecoButtonCache().object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( size, size );
            pixmap->fill( Qt::transparent );

            const QColor light( calcLightColor( color ) );
            const QColor dark( calcDarkColor( color ) );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            const qreal u( size/18.0 );
            p.translate( 0.5*u, ( 0.5-0.668 )*u );

            {
                // outline circle
                qreal penWidth = 1.2;
                QLinearGradient lg( 0, u*( 1.665-penWidth ), 0, u*( 12.33+1.665-penWidth ) );
                lg.setColorAt( 0, dark );
                lg.setColorAt( 1, light );
                QRectF r( u*0.5*( 17-12.33+penWidth ), u*( 1.665+penWidth ), u*( 12.33-penWidth ), u*( 12.33-penWidth ) );
                p.setPen( QPen( lg, penWidth*u ) );
                p.drawEllipse( r );
                p.end();
            }

            windecoButtonCache().insert( key, pixmap );
        }

        return *pixmap;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::roundCorner( const QColor& color, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 )|size );
        TileSet *tileSet = _cornerCache.object( key );

        if ( !tileSet )
        {

            QPixmap pixmap = QPixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHint( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );

            QLinearGradient lg = QLinearGradient( 0.0, size-4.5, 0.0, size+4.5 );
            lg.setColorAt( 0.0, calcLightColor( backgroundTopColor( color ) ) );
            lg.setColorAt( 0.51, backgroundBottomColor( color ) );
            lg.setColorAt( 1.0, backgroundBottomColor( color ) );

            // draw ellipse.
            p.setBrush( lg );
            p.drawEllipse( QRectF( size-4, size-4, 8, 8 ) );

            // mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( Qt::black );
            p.drawEllipse( QRectF( size-3, size-3, 6, 6 ) );

            tileSet = new TileSet( pixmap, size, size, 1, 1 );
            _cornerCache.insert( key, tileSet );

        }

        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slope( const QColor& color, qreal shade, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 )|( quint64( 256.0*shade )<<24 )|size );
        TileSet *tileSet = _slopeCache.object( key );

        if ( !tileSet )
        {

            QPixmap pixmap( size*4, size*4 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setPen( Qt::NoPen );

            // edges
            TileSet *slabTileSet = slab( color, shade, size );
            slabTileSet->render( QRect( 0, 0, size*4, size*5 ), &p,
                TileSet::Left | TileSet::Right | TileSet::Top );

            p.setWindow( 0,0,28,28 );

            // bottom
            QColor light = KColorUtils::shade( calcLightColor( color ), shade );
            QLinearGradient fillGradient( 0, -28, 0, 28 );
            light.setAlphaF( 0.4 ); fillGradient.setColorAt( 0.0, light );
            light.setAlphaF( 0.0 ); fillGradient.setColorAt( 1.0, light );
            p.setBrush( fillGradient );
            p.setCompositionMode( QPainter::CompositionMode_DestinationOver );
            p.drawRect( 3, 9, 22, 17 );

            // fade bottom
            QLinearGradient maskGradient( 0, 7, 0, 28 );
            maskGradient.setColorAt( 0.0, Qt::black );
            maskGradient.setColorAt( 1.0, Qt::transparent );

            p.setBrush( maskGradient );
            p.setCompositionMode( QPainter::CompositionMode_DestinationIn );
            p.drawRect( 0, 9, 28, 19 );

            p.end();

            tileSet = new TileSet( pixmap, size, size, size*2, 2 );

            _slopeCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    void StyleHelper::fillSlab( QPainter& p, const QRect& rect, int size ) const
    {
        const qreal s( qreal( size ) * ( 3.6 + ( 0.5 * _slabThickness ) ) / 7.0 );
        const QRectF r( QRectF( rect ).adjusted( s, s, -s, -s ) );
        if( !r.isValid() ) return;

        p.drawRoundedRect( r, s/2, s/2 );
    }

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::progressBarIndicator( const QPalette& pal, const QRect& rect )
    {

        const QColor highlight( pal.color( QPalette::Highlight ) );
        const quint64 key( ( quint64( highlight.rgba() ) << 32 ) | ( rect.width() << 16 ) | ( rect.height() ) );

        QPixmap *pixmap = _progressBarCache.object( key );
        if ( !pixmap )
        {

            QRect local( rect );
            local.adjust( -1, -2, 1, 1 );

            // set topLeft corner to 0.0
            local.translate( -local.topLeft() );

            pixmap = new QPixmap( local.size() );
            pixmap->fill( Qt::transparent );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setBrush( Qt::NoBrush );

            local.adjust( 1, 1, -1, -1 );
            const QColor lhighlight( calcLightColor( highlight ) );
            const QColor color( pal.color( QPalette::Active, QPalette::Window ) );
            const QColor light( calcLightColor( color ) );
            const QColor dark( calcDarkColor( color ) );
            const QColor shadow( calcShadowColor( color ) );

            // shadow
            p.setPen( QPen( alphaColor( shadow, 0.6 ),0.6 ) );
            p.drawRoundedRect( QRectF( local ).adjusted( 0.5, -0.5, 0.5, 1.5 ), 2, 2 );

            // fill
            p.setPen( Qt::NoPen );
            p.setBrush( KColorUtils::mix( highlight, dark, 0.2 ) );
            p.drawRect( local.adjusted( 1, 0, -1, 0 ) );

            // fake radial gradient
            local.adjust( 0, 0, -1, 0 );
            QPixmap pm( local.size() );
            pm.fill( Qt::transparent );
            {
                QRectF pmRect = pm.rect();
                QLinearGradient mask( pmRect.topLeft(), pmRect.topRight() );
                mask.setColorAt( 0.0, Qt::transparent );
                mask.setColorAt( 0.4, Qt::black );
                mask.setColorAt( 0.6, Qt::black );
                mask.setColorAt( 1.0, Qt::transparent );

                QLinearGradient radial( pmRect.topLeft(), pmRect.bottomLeft() );
                radial.setColorAt( 0.0, KColorUtils::mix( lhighlight, light, 0.3 ) );
                radial.setColorAt( 0.5, Qt::transparent );
                radial.setColorAt( 0.6, Qt::transparent );
                radial.setColorAt( 1.0, KColorUtils::mix( lhighlight, light, 0.3 ) );

                QPainter pp( &pm );
                pp.fillRect( pm.rect(), mask );
                pp.setCompositionMode( QPainter::CompositionMode_SourceIn );
                pp.fillRect( pm.rect(), radial );
                pp.end();

            }

            p.drawPixmap( QPoint( 1,1 ), pm );

            // bevel
            p.setRenderHint( QPainter::Antialiasing, false );
            QLinearGradient bevel( local.topLeft(), local.bottomLeft() );
            bevel.setColorAt( 0, lhighlight );
            bevel.setColorAt( 0.5, highlight );
            bevel.setColorAt( 1, calcDarkColor( highlight ) );
            p.setBrush( Qt::NoBrush );
            p.setPen( QPen( bevel, 1 ) );
            p.drawRoundedRect( local,2,2 );

            // bright top edge
            QLinearGradient lightHl( local.topLeft(),local.topRight() );
            lightHl.setColorAt( 0, Qt::transparent );
            lightHl.setColorAt( 0.5, KColorUtils::mix( highlight, light, 0.8 ) );
            lightHl.setColorAt( 1, Qt::transparent );

            p.setPen( QPen( lightHl, 1 ) );
            p.drawLine( local.topLeft(), local.topRight() );
            p.end();

            _progressBarCache.insert( key, pixmap );
        }

        return *pixmap;

    }

    //______________________________________________________________________________
    QPixmap StyleHelper::dialSlab( const QColor& color, qreal shade, int size )
    {
        Oxygen::Cache<QPixmap>::Value *cache = _dialSlabCache.get( color );

        const quint64 key( ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );
        if ( !pixmap )
        {
            pixmap = new QPixmap( size, size );
            pixmap->fill( Qt::transparent );

            const QRectF rect( pixmap->rect() );

            QPainter p( pixmap );
            p.setPen( Qt::NoPen );
            p.setRenderHints( QPainter::Antialiasing );

            // colors
            const QColor base( KColorUtils::shade( color, shade ) );
            const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
            const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
            const QColor mid( KColorUtils::shade( calcMidColor( color ), shade ) );
            const QColor shadow( calcShadowColor( color ) );

            // shadow
            drawShadow( p, shadow, rect.width() );

            const qreal baseOffset = 3.5;
            {
                //plain background
                QLinearGradient lg( 0, baseOffset-0.5*rect.height(), 0, baseOffset+rect.height() );
                lg.setColorAt( 0, light );
                lg.setColorAt( 0.8, base );

                p.setBrush( lg );
                const qreal offset( baseOffset );
                p.drawEllipse( rect.adjusted( offset, offset, -offset, -offset ) );
            }

            {
                // outline circle
                const qreal penWidth( 0.7 );
                QLinearGradient lg( 0, baseOffset, 0, baseOffset + 2*rect.height() );
                lg.setColorAt( 0, light );
                lg.setColorAt( 1, mid );
                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( lg, penWidth ) );
                const qreal offset( baseOffset+0.5*penWidth );
                p.drawEllipse( rect.adjusted( offset, offset, -offset, -offset ) );
            }


            cache->insert( key, pixmap );

        }

        return *pixmap;

    }

    //______________________________________________________________________________
    QPixmap StyleHelper::dialSlabFocused( const QColor& color, const QColor& glowColor, qreal shade, int size )
    {
        Oxygen::Cache<QPixmap>::Value* cache =  _dialSlabCache.get( color );

        const quint64 key( ( quint64( glowColor.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );
        if ( !pixmap )
        {
            pixmap = new QPixmap( size, size );
            pixmap->fill( Qt::transparent );

            QRectF rect( pixmap->rect() );

            QPainter p( pixmap );
            p.setPen( Qt::NoPen );
            p.setRenderHints( QPainter::Antialiasing );

            // colors
            const QColor base( KColorUtils::shade( color, shade ) );
            const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
            const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
            const QColor mid( KColorUtils::shade( calcMidColor( color ), shade ) );
            const QColor shadow( calcShadowColor( color ) );

            // shadow
            drawShadow( p, shadow, rect.width() );
            drawOuterGlow( p, glowColor, rect.width() );

            const qreal baseOffset( 3.5 );
            {
                //plain background
                QLinearGradient lg( 0, baseOffset-0.5*rect.height(), 0, baseOffset+rect.height() );
                lg.setColorAt( 0, light );
                lg.setColorAt( 0.8, base );

                p.setBrush( lg );
                const qreal offset( baseOffset );
                p.drawEllipse( rect.adjusted( offset, offset, -offset, -offset ) );
            }

            {
                // outline circle
                const qreal penWidth( 0.7 );
                QLinearGradient lg( 0, baseOffset, 0, baseOffset + 2*rect.height() );
                lg.setColorAt( 0, light );
                lg.setColorAt( 1, mid );
                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( lg, penWidth ) );
                const qreal offset( baseOffset+0.5*penWidth );
                p.drawEllipse( rect.adjusted( offset, offset, -offset, -offset ) );
            }


            cache->insert( key, pixmap );

        }

        return *pixmap;

    }

    //______________________________________________________________________________
    QPixmap StyleHelper::roundSlab( const QColor& color, qreal shade, int size )
    {

        Oxygen::Cache<QPixmap>::Value* cache( _roundSlabCache.get( color ) );

        const quint64 key( ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( size*3, size*3 );
            pixmap->fill( Qt::transparent );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0,0,21,21 );

            // shadow
            drawShadow( p, calcShadowColor( color ), 21 );
            drawRoundSlab( p, color, shade );
            p.end();

            cache->insert( key, pixmap );
        }

        return *pixmap;
    }

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::roundSlabFocused( const QColor& color, const QColor& glowColor, qreal shade, int size )
    {

        Oxygen::Cache<QPixmap>::Value* cache( _roundSlabCache.get( color ) );

        const quint64 key( ( quint64( glowColor.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( size*3, size*3 );
            pixmap->fill( Qt::transparent );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0,0,21,21 );

            drawShadow( p, calcShadowColor( color ), 21 );
            drawOuterGlow( p, glowColor, 21 );
            drawRoundSlab( p, color, shade );

            p.end();
            cache->insert( key, pixmap );

        }
        return *pixmap;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slabFocused( const QColor& color, const QColor& glowColor, qreal shade, int size )
    {
        Oxygen::Cache<TileSet>::Value* cache( _slabCache.get( color ) );

        const quint64 key( ( quint64( glowColor.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        TileSet *tileSet = cache->object( key );

        const qreal hScale( 1 );
        const int hSize( size*hScale );
        const int vSize( size );

        if ( !tileSet )
        {
            QPixmap pixmap( hSize*2,vSize*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );

            const int fixedSize( 14 );
            p.setWindow( 0,0,fixedSize*hScale, fixedSize );

            // draw all components
            if( color.isValid() ) drawShadow( p, calcShadowColor( color ), 14 );
            if( glowColor.isValid() ) drawOuterGlow( p, glowColor, 14 );
            if( color.isValid() ) drawSlab( p, color, shade );

            p.end();

            tileSet = new TileSet( pixmap, hSize, vSize, hSize, vSize, hSize-1, vSize, 2, 1 );

            cache->insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slabSunken( const QColor& color, qreal shade, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) );
        TileSet *tileSet = _slabSunkenCache.object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0,0,14,14 );

            // slab
            drawSlab( p, color, shade );

            // shadow
            p.setCompositionMode( QPainter::CompositionMode_SourceOver );
            drawInverseShadow( p, calcShadowColor( color ), 3, 8, 0.0 );

            p.end();

            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );

            _slabSunkenCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slabInverted( const QColor& color, qreal shade, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) );
        TileSet *tileSet = _slabInvertedCache.object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0,0,14,14 );

            const QColor base( KColorUtils::shade( color, shade ) );
            const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
            const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );

            // bevel, part 2
            QLinearGradient bevelGradient2( 0, 8, 0, -8 );
            bevelGradient2.setColorAt( 0.0, light );
            bevelGradient2.setColorAt( 0.9, base );
            p.setBrush( bevelGradient2 );
            p.drawEllipse( QRectF( 2.6,2.6,8.8,8.8 ) );

            // bevel, part 1
            qreal y = KColorUtils::luma( base );
            qreal yl = KColorUtils::luma( light );
            qreal yd = KColorUtils::luma( dark );
            QLinearGradient bevelGradient1( 0, 7, 0, 4 );
            bevelGradient1.setColorAt( 0.0, light );
            bevelGradient1.setColorAt( 0.9, dark );

            if ( y < yl && y > yd )
            {
                // no middle when color is very light/dark
                bevelGradient1.setColorAt( 0.5, base );
            }

            p.setBrush( bevelGradient1 );
            p.drawEllipse( QRectF( 3.4,3.4,7.2,7.2 ) );

            // inside mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( QBrush( Qt::black ) );
            p.drawEllipse( QRectF( 4.0,4.0,6.0,6.0 ) );

            // shadow
            p.setCompositionMode( QPainter::CompositionMode_DestinationOver );
            drawInverseShadow( p, calcShadowColor( color ), 4, 6, 0.5 );

            p.end();

            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );

            _slabInvertedCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //__________________________________________________________________________________________________________
    void StyleHelper::drawRoundSlab( QPainter& p, const QColor& color, qreal shade )
    {

        p.save();

        // colors
        const QColor base( KColorUtils::shade( color, shade ) );
        const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );

        // bevel, part 1
        QLinearGradient bevelGradient1( 0, 10, 0, 18 );
        bevelGradient1.setColorAt( 0.0, light );
        bevelGradient1.setColorAt( 0.9, alphaColor( light, 0.85 ) );
        p.setBrush( bevelGradient1 );
        p.drawEllipse( QRectF( 3.0,3.0,15.0,15.0 ) );

        // bevel, part 2
        if ( _slabThickness > 0.0 ) {
            QLinearGradient bevelGradient2( 0, 7, 0, 28 );
            bevelGradient2.setColorAt( 0.0, light );
            bevelGradient2.setColorAt( 0.9, base );
            p.setBrush( bevelGradient2 );
            p.drawEllipse( QRectF( 3.6,3.6,13.8,13.8 ) );
        }

        // inside
        QLinearGradient innerGradient( 0, -17, 0, 20 );
        innerGradient.setColorAt( 0, light );
        innerGradient.setColorAt( 1, base );
        p.setBrush( innerGradient );
        const qreal ic( 3.6 + _slabThickness );
        const qreal is( 21.0 - 2.0*ic );
        p.drawEllipse( QRectF( ic, ic, is, is ) );

        p.restore();

    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseShadow(
        QPainter& p, const QColor& color,
        int pad, int size, qreal fuzz ) const
    {

        const qreal m( qreal( size )*0.5 );
        const qreal offset( 0.8 );
        const qreal k0( ( m-2 ) / qreal( m+2.0 ) );
        QRadialGradient shadowGradient( pad+m, pad+m+offset, m+2 );
        for ( int i = 0; i < 8; i++ )
        {
            // sinusoidal gradient
            const qreal k1( ( qreal( 8 - i ) + k0 * qreal( i ) ) * 0.125 );
            const qreal a( ( cos( 3.14159 * i * 0.125 ) + 1.0 ) * 0.25 );
            shadowGradient.setColorAt( k1, alphaColor( color, a * _shadowGain ) );
        }
        shadowGradient.setColorAt( k0, alphaColor( color, 0.0 ) );
        p.setBrush( shadowGradient );
        p.drawEllipse( QRectF( pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0 ) );
    }

    //________________________________________________________________________________________________________
    void StyleHelper::drawInverseGlow(
        QPainter& p, const QColor& color,
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
        p.setBrush( glowGradient );
        p.drawEllipse( r );
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
    void StyleHelper::fillHole( QPainter& p, const QRect& rect, int size ) const
    {
        const qreal s( ( 3.0*size )/7.0 );
        p.drawRoundedRect( rect.adjusted( s,s,-s,-s ), 4, 4 );
    }

    //____________________________________________________________________________________
    void StyleHelper::renderHole( QPainter* p, const QColor& base, const QRect& r, bool focus, bool hover, qreal opacity, Oxygen::AnimationMode animationMode,  TileSet::Tiles tiles, bool outline )
    {
        if( !r.isValid() ) return;
        if( opacity >= 0 && ( animationMode & Oxygen::AnimationFocus ) )
        {

            // calculate proper glow color based on current settings and opacity
            const QColor glow( hover ?
                KColorUtils::mix( viewHoverBrush().brush( QPalette::Active ).color(), viewFocusBrush().brush( QPalette::Active ).color(), opacity ):
                alphaColor(  viewFocusBrush().brush( QPalette::Active ).color(), opacity ) );

            holeFocused( base, glow, 5, outline )->render( r, p, tiles );

        } else if ( focus ) {

            const QColor glow( viewFocusBrush().brush( QPalette::Active ).color() );
            holeFocused( base, glow, 5, outline )->render( r, p, tiles );

        } else if( opacity >= 0 && ( animationMode & Oxygen::AnimationHover ) ) {

            // calculate proper glow color based on current settings and opacity
            const QColor glow( alphaColor(  viewHoverBrush().brush( QPalette::Active ).color(), opacity ) );
            holeFocused( base, glow, 5, outline )->render( r, p, tiles );

        } else if ( hover ) {

            const QColor glow( viewHoverBrush().brush( QPalette::Active ).color() );
            holeFocused( base, glow, 5, outline )->render( r, p, tiles );

        } else {

            hole( base, 5, outline )->render( r, p, tiles );

        }

    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::hole( const QColor& color, int size, bool outline )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) | size << 1 | outline );
        TileSet *tileSet = _holeCache.object( key );

        if ( !tileSet )
        {

            // create pixmap
            QPixmap pixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );

            p.setWindow( 0, 0, 10, 10 );

            // hole mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( Qt::black );
            p.drawEllipse( 1, 1, 8, 8 );
            p.setCompositionMode( QPainter::CompositionMode_SourceOver );

            if( outline )
            {
                QLinearGradient blend( 0, 1, 0, 9 );
                blend.setColorAt( 0, Qt::transparent );
                blend.setColorAt( 1, calcDarkColor( color ) );

                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( blend, 1 ) );
                p.drawEllipse( 1, 1.5, 8, 7 );
                p.setPen( Qt::NoPen );
            }

            // fade out glow
            drawInverseShadow( p, calcShadowColor( color ), 1, 8, 0.0 );

            p.end();

            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );
            _holeCache.insert( key, tileSet );

        }

        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::holeFlat( const QColor& color, qreal shade, bool fill, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size << 1 | fill );
        TileSet *tileSet = _holeFlatCache.object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0,0,14,14 );

            // hole inside
            if( fill )
            {
                p.setBrush( color );
                p.drawRoundedRect( QRectF( 1, 0, 12, 13 ), 3.0, 3.0 );
                //p.drawRoundedRect( QRectF( 1, 0, 12, 13 ), 2.5, 2.5 );
                p.setBrush( Qt::NoBrush );

                {
                    const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
                    QLinearGradient gradient( 0, 0, 0, 14 );
                    gradient.setColorAt( 0.0, dark );
                    gradient.setColorAt( 0.5, Qt::transparent );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 1.5, 0.5, 11, 12 ), 3.0, 3.0 );
                }

                {

                    const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
                    QLinearGradient gradient( 0, 0, 0, 14 );
                    gradient.setColorAt( 0.5, Qt::transparent );
                    gradient.setColorAt( 1.0, light );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 0.5, 0.5, 13, 13 ), 3.5, 3.5 );
                    //p.drawRoundedRect( QRectF( 0.5, 0.5, 13, 13 ), 3.0, 3.0 );

                }

            } else {

                p.setBrush( color );
                p.drawRoundedRect( QRectF( 1, 1, 12, 12 ), 3.0, 3.0 );
                p.setBrush( Qt::NoBrush );

                {
                    const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
                    QLinearGradient gradient( 0, 1, 0, 12 );
                    gradient.setColorAt( 0.0, dark );
                    gradient.setColorAt( 0.5, Qt::transparent );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 1.5, 1.5, 11, 11 ), 2.5, 2.5 );
                }

                {
                    const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
                    QLinearGradient gradient( 0, 1, 0, 12 );
                    gradient.setColorAt( 0.5, Qt::transparent );
                    gradient.setColorAt( 1.0, light );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 1.5, 1.5, 11, 11 ), 2.5, 2.5 );

                }

            }

            p.end();

            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );

            _holeFlatCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::holeFocused( const QColor& color, const QColor& glowColor, int size, bool outline )
    {

        // get key
        Oxygen::Cache<TileSet>::Value* cache( _holeFocusedCache.get( glowColor ) );

        const quint64 key( ( quint64( color.rgba() ) << 32 ) | size << 1 | outline );
        TileSet *tileSet = cache->object( key );

        if ( !tileSet )
        {

            QPixmap pixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0, 0, 10, 10 );

            // hole mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( Qt::black );
            p.drawEllipse( 1, 1, 8, 8 );
            p.setCompositionMode( QPainter::CompositionMode_SourceOver );

            if( outline )
            {
                QLinearGradient blend( 0, 1, 0, 9 );
                blend.setColorAt( 0, Qt::transparent );
                blend.setColorAt( 1, calcDarkColor( color ) );

                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( blend, 1 ) );
                p.drawEllipse( 1, 1.5, 8, 7 );
                p.setPen( Qt::NoPen );
            }

            int alpha( glowColor.alpha() );

            // fade-in shadow
            if( alpha < 255 )
            {
                QColor shadowColor( calcShadowColor( color ) );
                shadowColor.setAlpha( 255-alpha );
                drawInverseShadow( p, calcShadowColor( color ), 1, 8, 0.0 );
            }

            // fade-out glow
            if( alpha > 0 )
            { drawInverseGlow( p, glowColor, 1, 8, size ); }

            p.end();

            tileSet = new TileSet( pixmap, size, size, size, size, size-1, size, 2, 1 );
            cache->insert( key, tileSet );
        }

        return tileSet;

    }

    //______________________________________________________________________________
    TileSet *StyleHelper::scrollHole( const QColor& color, Qt::Orientation orientation, bool smallShadow )
    {

        const quint64 key( quint64( color.rgba() ) << 32 | ( orientation == Qt::Horizontal ? 2 : 0 ) | ( smallShadow ? 1 : 0 ) );
        TileSet *tileSet = _scrollHoleCache.object( key );
        if ( !tileSet )
        {
            QPixmap pm( 15, 15 );
            pm.fill( Qt::transparent );

            QPainter p( &pm );

            const QColor dark( calcDarkColor( color ) );
            const QColor light( calcLightColor( color ) );
            const QColor shadow( calcShadowColor( color ) );

            // use space for white border
            const QRect r( QRect( 0,0,15,15 ) );
            const QRect rect( r.adjusted( 1, 0, -1, -1 ) );
            int shadowWidth( 0 );
            if( smallShadow ) shadowWidth = ( orientation == Qt::Horizontal ) ? 2 : 1;
            else shadowWidth = ( orientation == Qt::Horizontal ) ? 3 : 2;

            p.setRenderHints( QPainter::Antialiasing );
            p.setBrush( dark );
            p.setPen( Qt::NoPen );

            // base
            p.drawRoundedRect( rect, 4.5, 4.5 );

            // slight shadow across the whole hole
            QLinearGradient shadowGradient( rect.topLeft(),
                orientation == Qt::Horizontal ?
                rect.bottomLeft():rect.topRight() );

            shadowGradient.setColorAt( 0.0, alphaColor( shadow, 0.1 ) );
            shadowGradient.setColorAt( 0.6, Qt::transparent );
            p.setBrush( shadowGradient );
            p.drawRoundedRect( rect, 4.5, 4.5 );

            // strong shadow

            // left
            QLinearGradient l1 = QLinearGradient( rect.topLeft(), rect.topLeft()+QPoint( shadowWidth,0 ) );
            l1.setColorAt( 0.0, alphaColor( shadow, orientation == Qt::Horizontal ? 0.3 : 0.2 ) );
            l1.setColorAt( 0.5, alphaColor( shadow, orientation == Qt::Horizontal ? 0.1 : 0.1 ) );
            l1.setColorAt( 1.0, Qt::transparent );
            p.setBrush( l1 );
            p.drawRoundedRect( QRect( rect.topLeft(), rect.bottomLeft()+QPoint( shadowWidth,0 ) ), 4.5, 4.5 );

            // right
            l1 = QLinearGradient( rect.topRight(), rect.topRight()-QPoint( shadowWidth,0 ) );
            l1.setColorAt( 0.0, alphaColor( shadow, orientation == Qt::Horizontal ? 0.3 : 0.2 ) );
            l1.setColorAt( 0.5, alphaColor( shadow, orientation == Qt::Horizontal ? 0.1 : 0.1 ) );
            l1.setColorAt( 1.0, Qt::transparent );
            p.setBrush( l1 );
            p.drawRoundedRect( QRect( rect.topRight()-QPoint( shadowWidth,0 ), rect.bottomRight() ), 4.5, 4.5 );

            //top
            l1 = QLinearGradient( rect.topLeft(), rect.topLeft()+QPoint( 0,3 ) );
            l1.setColorAt( 0.0, alphaColor( shadow, 0.3 ) );
            l1.setColorAt( 1.0, Qt::transparent );
            p.setBrush( l1 );
            p.drawRoundedRect( QRect( rect.topLeft(),rect.topRight()+QPoint( 0,3 ) ), 4.5, 4.5 );

            // light border
            QLinearGradient borderGradient( r.topLeft()+QPoint( 0,r.height()/2-1 ), r.bottomLeft() );
            borderGradient.setColorAt( 0.0, Qt::transparent );
            borderGradient.setColorAt( 1.0, alphaColor( light, 0.8 ) );
            p.setPen( QPen( borderGradient, 1.0 ) );
            p.setBrush( Qt::NoBrush );
            p.drawRoundedRect( r.adjusted( 0.5,0,-0.5,0 ), 5.0, 5.0 );

            p.end();

            tileSet = new TileSet( pm, 7, 7, 1, 1 );

            _scrollHoleCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::groove( const QColor& color, qreal shade, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        TileSet *tileSet = _grooveCache.object( key );

        if ( !tileSet )
        {
            const int rsize( ( int )ceil( qreal( size ) * 3.0/7.0 ) );
            QPixmap pixmap( rsize*2, rsize*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 2,2,6,6 );

            // hole mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( Qt::black );
            p.drawEllipse( 4,4,2,2 );

            // shadow
            p.setCompositionMode( QPainter::CompositionMode_SourceOver );
            drawInverseShadow( p, calcShadowColor( color ), 3, 4, 0.0 );

            p.end();

            tileSet = new TileSet( pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 );

            _grooveCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slitFocused( const QColor& glowColor )
    {
        const quint64 key( ( quint64( glowColor.rgba() ) << 32 ) );
        TileSet *tileSet = _slitCache.object( key );

        if ( !tileSet )
        {
            QPixmap pixmap( 9,9 );
            QPainter p;

            pixmap.fill( Qt::transparent );

            p.begin( &pixmap );
            p.setPen( Qt::NoPen );
            p.setRenderHint( QPainter::Antialiasing );
            QRadialGradient rg = QRadialGradient( 4.5, 4.5, 3.5 );

            QColor tmpColor( alphaColor( glowColor, 180.0/255 ) );
            rg.setColorAt( 1.0, tmpColor );
            tmpColor.setAlpha( 0 );
            rg.setColorAt( 0.5, tmpColor );
            p.setBrush( rg );

            p.drawEllipse( QRectF( 1, 1, 7, 7 ) );

            p.end();

            tileSet = new TileSet( pixmap, 4, 4, 1, 1 );

            _slitCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //____________________________________________________________________
    TileSet *StyleHelper::dockFrame( const QColor& color, int w )
    {
        const quint64 key( quint64( color.rgba() ) << 32 | w );
        TileSet *tileSet = _dockFrameCache.object( key );
        if ( !tileSet )
        {
            // width should be odd
            if( !w&1 ) --w;

            // fixed height
            const int h( 9 );

            QPixmap pm( w, h );
            pm.fill( Qt::transparent );

            QPainter p( &pm );
            p.save();
            p.setRenderHints( QPainter::Antialiasing );
            p.setBrush( Qt::NoBrush );
            p.translate( 0.5, 0.5 );

            // create local rect
            QRect rect( 0, 0, w-1, h );

            const QColor light = calcLightColor( color );
            const QColor dark = calcDarkColor( color );

            {
                // left and right border
                QLinearGradient lg( QPoint( 0,0 ), QPoint( w,0 ) );
                lg.setColorAt( 0.0, alphaColor( light, 0.6 ) );
                lg.setColorAt( 0.1, Qt::transparent );
                lg.setColorAt( 0.9, Qt::transparent );
                lg.setColorAt( 1.0, alphaColor( light, 0.6 ) );
                p.setPen( QPen( lg,1 ) );
                p.drawRoundedRect( rect.adjusted( 0,-1,0,-1 ),4,5 );
                p.drawRoundedRect( rect.adjusted( 2,1,-2,-2 ),4,5 );
            }

            {
                QLinearGradient lg( QPoint( 0,0 ), QPoint( w,0 ) );
                lg.setColorAt( 0.0, dark );
                lg.setColorAt( 0.1, Qt::transparent );
                lg.setColorAt( 0.9, Qt::transparent );
                lg.setColorAt( 1.0, dark );
                p.setPen( QPen( lg, 1 ) );
                p.drawRoundedRect( rect.adjusted( 1,0,-1,-2 ),4,5 );
            }

            p.restore();

            // top and bottom border
            drawSeparator( &p, QRect( 0,0,w,2 ), color, Qt::Horizontal );
            drawSeparator( &p, QRect( 0,h-2,w,2 ), color, Qt::Horizontal );

            p.end();

            tileSet = new TileSet( pm, 4, 4, w-8, h-8 );

            _dockFrameCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //____________________________________________________________________
    TileSet *StyleHelper::selection( const QColor& color, int height, bool custom )
    {

        const quint64 key( ( quint64( color.rgba() ) << 32 ) | ( height << 1 ) | custom );
        TileSet *tileSet = _selectionCache.object( key );
        if ( !tileSet )
        {

            const qreal rounding( 2.5 );

            QPixmap pixmap( 32+16, height );
            pixmap.fill( Qt::transparent );

            QRect r( pixmap.rect().adjusted( 0, 0, -1, -1 ) );

            QPainter p( &pixmap );
            p.setRenderHint( QPainter::Antialiasing );
            p.translate( .5, .5 );

            {

                // background
                QPainterPath path;
                path.addRoundedRect( r, rounding, rounding );

                // items with custom background brushes always have their background drawn
                // regardless of whether they are hovered or selected or neither so
                // the gradient effect needs to be more subtle
                const int lightenAmount( custom ? 110 : 130 );
                QLinearGradient gradient( 0, 0, 0, r.bottom() );
                gradient.setColorAt( 0, color.lighter( lightenAmount ) );
                gradient.setColorAt( 1, color );

                p.setPen( QPen( color, 1 ) );
                p.setBrush( gradient );
                p.drawPath( path );

            }

            {
                // contrast pixel
                QPainterPath path;
                path.addRoundedRect( r.adjusted( 1, 1, -1, -1 ), rounding - 1, rounding - 1 );
                p.strokePath( path, QPen( QColor( 255, 255, 255, 64 ), 1 ) );
            }

            p.end();

            tileSet = new TileSet( pixmap, 8, 0, 32, height );
            _selectionCache.insert( key, tileSet );

        }

        return tileSet;

    }

}
