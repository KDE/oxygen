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
    {}

    //______________________________________________________________________________
    void StyleHelper::invalidateCaches( void )
    {

        _dialSlabCache.clear();
        _roundSlabCache.clear();
        _sliderSlabCache.clear();
        _holeCache.clear();

        _midColorCache.clear();

        _progressBarCache.clear();
        _cornerCache.clear();
        _selectionCache.clear();
        _holeFlatCache.clear();
        _slopeCache.clear();
        _grooveCache.clear();
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

        _progressBarCache.setMaxCost( value );
        _cornerCache.setMaxCost( value );
        _selectionCache.setMaxCost( value );
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

        if ( clipRect.isValid() )
        {
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
            lg.setColorAt( 0.50, calcLightColor( backgroundTopColor( color ) ) );
            lg.setColorAt( 0.51, backgroundBottomColor( color ) );

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

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::progressBarIndicator( const QPalette& pal, const QRect& rect )
    {

        const QColor highlight( pal.color( QPalette::Highlight ) );
        const quint64 key( ( quint64( highlight.rgba() ) << 32 ) | ( rect.width() << 16 ) | ( rect.height() ) );

        QPixmap *pixmap = _progressBarCache.object( key );
        if ( !pixmap )
        {

            QRect local( rect );

            // set topLeft corner to 0.0
            local.translate( -local.topLeft() );

            pixmap = new QPixmap( local.size() );
            pixmap->fill( Qt::transparent );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setBrush( Qt::NoBrush );

            const QColor lhighlight( calcLightColor( highlight ) );
            const QColor color( pal.color( QPalette::Active, QPalette::Window ) );
            const QColor light( calcLightColor( color ) );
            const QColor dark( calcDarkColor( color ) );
            const QColor shadow( calcShadowColor( color ) );

            // shadow
            {
                p.setPen( QPen( alphaColor( shadow, 0.4 ),0.6 ) );
                p.drawRoundedRect( QRectF( local ).adjusted( 0.5, 0.5, -0.5, 0.5 ), 3.0, 3.0 );
            }

            // fill
            local.adjust( 1, 1, -1, 0 );
            {
                p.setPen( Qt::NoPen );
                p.setBrush( KColorUtils::mix( highlight, dark, 0.2 ) );
                p.drawRoundedRect( local, 2.5, 2.5 );
            }

            // fake radial gradient
            {
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

            }

            // bevel
            {
                QLinearGradient bevel( QPointF( 0, 0.5 ) + local.topLeft(), QPointF( 0, -0.5 ) + local.bottomLeft() );
                bevel.setColorAt( 0, lhighlight );
                bevel.setColorAt( 0.5, highlight );
                bevel.setColorAt( 1, calcDarkColor( highlight ) );
                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( bevel, 1 ) );
                p.drawRoundedRect( QRectF(local).adjusted( 0.5, 0.5, -0.5, -0.5 ), 2.5, 2.5 );
            }

            // bright top edge
            {
                QLinearGradient lightHl( local.topLeft(),local.topRight() );
                lightHl.setColorAt( 0, Qt::transparent );
                lightHl.setColorAt( 0.5, KColorUtils::mix( highlight, light, 0.8 ) );
                lightHl.setColorAt( 1, Qt::transparent );

                p.setPen( QPen( lightHl, 1 ) );
                p.drawLine( QPointF( 0.5, 0.5 ) + local.topLeft(), QPointF( 0.5, 0.5 ) + local.topRight() );
            }

            p.end();

            _progressBarCache.insert( key, pixmap );
        }

        return *pixmap;

    }

    //______________________________________________________________________________
    QPixmap StyleHelper::dialSlab( const QColor& color, const QColor& glow, qreal shade, int size )
    {
        Oxygen::Cache<QPixmap>::Value* cache =  _dialSlabCache.get( color );

        const quint64 key( ( quint64( glow.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
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

            if( glow.isValid() )
            { drawOuterGlow( p, glow, rect.width() ); }

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

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::roundSlab( const QColor& color, const QColor& glow, qreal shade, int size )
    {

        Oxygen::Cache<QPixmap>::Value* cache( _roundSlabCache.get( color ) );

        const quint64 key( ( quint64( glow.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | size );
        QPixmap *pixmap = cache->object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( size*3, size*3 );
            pixmap->fill( Qt::transparent );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0,0,21,21 );

            // draw normal shadow
            drawShadow( p, calcShadowColor( color ), 21 );

            // draw glow.
            if( glow.isValid() )
            { drawOuterGlow( p, glow, 21 ); }

            drawRoundSlab( p, color, shade );

            p.end();
            cache->insert( key, pixmap );

        }
        return *pixmap;
    }

    //__________________________________________________________________________________________________________
    QPixmap StyleHelper::sliderSlab( const QColor& color, const QColor& glow, bool sunken, qreal shade, int size )
    {

        Oxygen::Cache<QPixmap>::Value* cache( _sliderSlabCache.get( color ) );

        const quint64 key( ( quint64( glow.rgba() ) << 32 ) | ( quint64( 256.0 * shade ) << 24 ) | (sunken << 23 ) | size );
        QPixmap *pixmap = cache->object( key );

        if ( !pixmap )
        {
            pixmap = new QPixmap( size*3, size*3 );
            pixmap->fill( Qt::transparent );

            QPainter p( pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );

            p.setWindow( -1, -1, 23, 23 );

            if( color.isValid() ) drawShadow( p, alphaColor( calcShadowColor( color ), 0.8 ), 21 );
            if( glow.isValid() ) drawOuterGlow( p, glow, 21 );

            // draw slab
            p.setWindow( -2, -2, 25, 25 );
            drawSliderSlab( p, color, sunken, shade );

            p.end();
            cache->insert( key, pixmap );

        }
        return *pixmap;
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

    //__________________________________________________________________________________________________________
    void StyleHelper::drawSliderSlab( QPainter& p, const QColor& color, bool sunken, qreal shade )
    {

        p.save();

        const QColor light( KColorUtils::shade( calcLightColor(color), shade ) );
        const QColor dark( KColorUtils::shade( calcDarkColor(color), shade ) );

        p.setPen(Qt::NoPen);

        {
            //plain background
            QLinearGradient lg( 0, 3, 0, 21 );
            lg.setColorAt( 0, light );
            lg.setColorAt( 1, dark );

            const QRectF r( 3, 3, 15, 15 );
            p.setBrush( lg );
            p.drawEllipse( r );

        }

        if( sunken )
        {
            //plain background
            QLinearGradient lg( 0, 3, 0, 21 );
            lg.setColorAt( 0, dark );
            lg.setColorAt( 1, light );

            const QRectF r( 5, 5, 11, 11 );
            p.setBrush( lg );
            p.drawEllipse( r );

        }

        {
            // outline circle
            const qreal penWidth( 1 );
            QLinearGradient lg( 0, 3, 0, 30 );
            lg.setColorAt( 0, light );
            lg.setColorAt( 1, dark );

            const QRectF r( 3.5, 3.5, 14, 14 );
            p.setPen( QPen( lg, penWidth ) );
            p.setBrush( Qt::NoBrush );
            p.drawEllipse( r );
        }

        p.restore();

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
    void StyleHelper::renderHole( QPainter* p, const QColor& base, const QRect& r, HoleOptions options, qreal opacity, Oxygen::AnimationMode animationMode,  TileSet::Tiles tiles )
    {
        if( !r.isValid() ) return;
        if( opacity >= 0 && ( animationMode & Oxygen::AnimationFocus ) )
        {

            // calculate proper glow color based on current settings and opacity
            const QColor glow( (options&HoleHover) ?
                KColorUtils::mix( viewHoverBrush().brush( QPalette::Active ).color(), viewFocusBrush().brush( QPalette::Active ).color(), opacity ):
                alphaColor(  viewFocusBrush().brush( QPalette::Active ).color(), opacity ) );

            hole( base, glow, 7, options )->render( r, p, tiles );

        } else if( options & HoleFocus ) {

            const QColor glow( viewFocusBrush().brush( QPalette::Active ).color() );
            hole( base, glow, 7, options )->render( r, p, tiles );

        } else if( opacity >= 0 && ( animationMode & Oxygen::AnimationHover ) ) {

            // calculate proper glow color based on current settings and opacity
            const QColor glow( alphaColor(  viewHoverBrush().brush( QPalette::Active ).color(), opacity ) );
            hole( base, glow, 7, options )->render( r, p, tiles );

        } else if( options & HoleHover ) {

            const QColor glow( viewHoverBrush().brush( QPalette::Active ).color() );
            hole( base, glow, 7, options )->render( r, p, tiles );

        } else {

            hole( base, 7, options )->render( r, p, tiles );

        }

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

            if( fill )
            {

                // hole inside
                p.setBrush( color );
                p.drawRoundedRect( QRectF( 1, 0, 12, 13 ), 3.0, 3.0 );
                p.setBrush( Qt::NoBrush );

                {
                    // shadow (top)
                    const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
                    QLinearGradient gradient( 0, -2, 0, 14 );
                    gradient.setColorAt( 0.0, dark );
                    gradient.setColorAt( 0.5, Qt::transparent );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 1.5, 0.5, 11, 12 ), 2.5, 2.5 );
                }

                {

                    // contrast (bottom)
                    const QColor light( KColorUtils::shade( calcLightColor( color ), shade ) );
                    QLinearGradient gradient( 0, 0, 0, 18 );
                    gradient.setColorAt( 0.5, Qt::transparent );
                    gradient.setColorAt( 1.0, light );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 0.5, 0.5, 13, 13 ), 3.5, 3.5 );

                }

            } else {

                // hole inside
                p.setBrush( color );
                p.drawRoundedRect( QRectF( 1, 1, 12, 12 ), 3.0, 3.0 );
                p.setBrush( Qt::NoBrush );

                {
                    // shadow (top)
                    const QColor dark( KColorUtils::shade( calcDarkColor( color ), shade ) );
                    QLinearGradient gradient( 0, 1, 0, 12 );
                    gradient.setColorAt( 0.0, dark );
                    gradient.setColorAt( 0.5, Qt::transparent );

                    p.setPen( QPen( gradient, 1 ) );
                    p.drawRoundedRect( QRectF( 1.5, 1.5, 11, 11 ), 2.5, 2.5 );
                }

                {
                    // contrast (bottom)
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
    TileSet *StyleHelper::hole( const QColor& color, const QColor& glow, int size, HoleOptions options )
    {

        // get key
        Oxygen::Cache<TileSet>::Value* cache( _holeCache.get( glow ) );

        const quint64 key( ( quint64( color.rgba() ) << 32 ) | (size << 4) | options );
        TileSet *tileSet = cache->object( key );

        if ( !tileSet )
        {

            // first create shadow
            int shadowSize( (size*5)/7 );
            QPixmap shadowPixmap( shadowSize*2, shadowSize*2 );

            // calc alpha channel and fade
            const int alpha( glow.isValid() ? glow.alpha():0 );

            {
                shadowPixmap.fill( Qt::transparent );

                QPainter p( &shadowPixmap );
                p.setRenderHints( QPainter::Antialiasing );
                p.setPen( Qt::NoPen );
                p.setWindow( 0, 0, 10, 10 );

                // fade-in shadow
                if( alpha < 255 )
                {
                    QColor shadowColor( calcShadowColor( color ) );
                    shadowColor.setAlpha( 255-alpha );
                    drawInverseShadow( p, shadowColor, 1, 8, 0.0 );
                }

                // fade-out glow
                if( alpha > 0 )
                { drawInverseGlow( p, glow, 1, 8, shadowSize ); }

                p.end();

            }

            // create pixmap
            QPixmap pixmap( size*2, size*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0, 0, 14, 14 );

            // hole mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( Qt::black );

            p.drawRoundedRect( QRectF( 1, 1, 12, 12 ), 2.5, 2.5 );
            p.setCompositionMode( QPainter::CompositionMode_SourceOver );

            // render shadow
            TileSet(
                shadowPixmap, shadowSize, shadowSize, shadowSize,
                shadowSize, shadowSize-1, shadowSize, 2, 1 ).
                render( pixmap.rect(), &p );

            if( (options&HoleOutline) && alpha < 255 )
            {
                QColor dark( calcDarkColor( color ) );
                dark.setAlpha( 255 - alpha );
                QLinearGradient blend( 0, 0, 0, 14 );
                blend.setColorAt( 0, Qt::transparent );
                blend.setColorAt( 0.8, dark );

                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( blend, 1 ) );
                p.drawRoundedRect( QRectF( 1.5, 1.5, 11, 11 ), 3.0, 3.0 );
                p.setPen( Qt::NoPen );
            }

            if( options&HoleContrast )
            {
                QColor light( calcLightColor( color ) );
                QLinearGradient blend( 0, 0, 0, 18 );
                blend.setColorAt( 0.5, Qt::transparent );
                blend.setColorAt( 1.0, light );

                p.setBrush( Qt::NoBrush );
                p.setPen( QPen( blend, 1 ) );
                p.drawRoundedRect( QRectF( 0.5, 0.5, 13, 13 ), 4.0, 4.0 );
                p.setPen( Qt::NoPen );
            }

            p.end();

            // create tileset and return
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

            p.setRenderHints( QPainter::Antialiasing );
            p.setBrush( dark );
            p.setPen( Qt::NoPen );

            // base
            const qreal radius( smallShadow ? 2.5:3.0 );
            p.drawRoundedRect( rect, radius, radius );

            // slight shadow across the whole hole
            if( true )
            {
                QLinearGradient shadowGradient( rect.topLeft(),
                    orientation == Qt::Horizontal ?
                    rect.bottomLeft():rect.topRight() );

                shadowGradient.setColorAt( 0.0, alphaColor( shadow, 0.1 ) );
                shadowGradient.setColorAt( 0.6, Qt::transparent );
                p.setBrush( shadowGradient );
                p.drawRoundedRect( rect, radius, radius );

            }

            // first create shadow
            int shadowSize( 5 );
            QPixmap shadowPixmap( shadowSize*2, shadowSize*2 );

            {
                shadowPixmap.fill( Qt::transparent );

                QPainter p( &shadowPixmap );
                p.setRenderHints( QPainter::Antialiasing );
                p.setPen( Qt::NoPen );

                // fade-in shadow
                QColor shadowColor( calcShadowColor( color ) );
                if( smallShadow ) shadowColor = alphaColor( shadowColor, 0.6 );
                drawInverseShadow( p, shadowColor, 1, 8, 0.0 );

                p.end();

            }

            // render shadow
            TileSet(
                shadowPixmap, shadowSize, shadowSize, shadowSize,
                shadowSize, shadowSize-1, shadowSize, 2, 1 ).
                render( rect.adjusted( -1, -1, 1, 1 ), &p, TileSet::Full );

            // light border
            QLinearGradient borderGradient( 0, r.top(), 0, r.bottom() );
            if( smallShadow && orientation == Qt::Vertical )
            {

                borderGradient.setColorAt( 0.8, Qt::transparent );
                borderGradient.setColorAt( 1.0, alphaColor( light, 0.5 ) );

            } else {

                borderGradient.setColorAt( 0.5, Qt::transparent );
                borderGradient.setColorAt( 1.0, alphaColor( light, 0.6 ) );

            }

            p.setPen( QPen( borderGradient, 1.0 ) );
            p.setBrush( Qt::NoBrush );
            p.drawRoundedRect( QRectF(r).adjusted( 0.5,0.5,-0.5,-0.5 ), radius+0.5, radius+0.5 );

            p.end();

            tileSet = new TileSet( pm, 7, 7, 1, 1 );

            _scrollHoleCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::scrollHandle( const QColor& color, const QColor& glow, int size)
    {

        // get key
        Oxygen::Cache<TileSet>::Value* cache( _scrollHandleCache.get( glow ) );

        const quint64 key( ( quint64( color.rgba() ) << 32 ) | size );
        TileSet *tileSet = cache->object( key );

        if ( !tileSet )
        {
            QPixmap pm( 2*size, 2*size );
            pm.fill( Qt::transparent );

            QPainter p( &pm );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0, 0, 14, 14 );

            QPixmap shadowPixmap( 10, 10 );
            {

                shadowPixmap.fill( Qt::transparent );

                QPainter p( &shadowPixmap );
                p.setRenderHints( QPainter::Antialiasing );
                p.setPen( Qt::NoPen );

                // shadow/glow
                drawOuterGlow( p, glow, 10 );

                p.end();
            }

            TileSet( shadowPixmap, 4, 4, 1, 1 ).render( QRect( 0, 0, 14, 14 ), &p, TileSet::Full );

            // outline
            {
                const QColor mid( calcMidColor( color ) );
                QLinearGradient lg( 0, 3, 0, 11 );
                lg.setColorAt( 0, color );
                lg.setColorAt( 1, mid );
                p.setPen( Qt::NoPen );
                p.setBrush( lg );
                p.drawRoundedRect( QRectF( 3, 3, 8, 8 ), 2.5, 2.5 );
            }

            // contrast
            {
                const QColor light( calcLightColor( color ) );
                QLinearGradient lg( 0, 3, 0, 11 );
                lg.setColorAt( 0., alphaColor( light, 0.9 ) );
                lg.setColorAt( 0.5, alphaColor( light, 0.44 ) );
                p.setBrush( lg );
                p.drawRoundedRect( QRectF( 3, 3, 8, 8 ), 2.5, 2.5 );
            }

            p.end();

            // create tileset and return
            tileSet = new TileSet( pm, size-1, size, 1, 1 );
            cache->insert( key, tileSet );

        }

        return tileSet;
    }

    //______________________________________________________________________________
    TileSet *StyleHelper::groove( const QColor& color, int size )
    {
        const quint64 key( ( quint64( color.rgba() ) << 32 ) | size );
        TileSet *tileSet = _grooveCache.object( key );

        if ( !tileSet )
        {
            const int rsize( ( int )ceil( qreal( size ) * 3.0/7.0 ) );
            QPixmap pixmap( rsize*2, rsize*2 );
            pixmap.fill( Qt::transparent );

            QPainter p( &pixmap );
            p.setRenderHints( QPainter::Antialiasing );
            p.setPen( Qt::NoPen );
            p.setWindow( 0, 0, 6, 6 );

            // hole mask
            p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            p.setBrush( Qt::black );
            p.drawEllipse( 2, 2, 2, 2 );

            // shadow
            p.setCompositionMode( QPainter::CompositionMode_SourceOver );
            drawInverseShadow( p, calcShadowColor( color ), 1, 4, 0.0 );

            p.end();

            tileSet = new TileSet( pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1 );

            _grooveCache.insert( key, tileSet );

        }

        return tileSet;
    }

    //________________________________________________________________________________________________________
    TileSet *StyleHelper::slitFocused( const QColor& glow )
    {
        const quint64 key( ( quint64( glow.rgba() ) << 32 ) );
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

            rg.setColorAt( 1.0, alphaColor( glow, 180.0/255 ) );
            rg.setColorAt( 0.5, alphaColor( glow, 0 ) );
            p.setBrush( rg );

            p.drawEllipse( QRectF( 1, 1, 7, 7 ) );

            p.end();

            tileSet = new TileSet( pixmap, 4, 4, 1, 1 );

            _slitCache.insert( key, tileSet );
        }
        return tileSet;
    }

    //____________________________________________________________________
    TileSet *StyleHelper::dockFrame( const QColor& top, const QColor& bottom )
    {
        const quint64 key( quint64( top.rgba() ) << 32 | quint64( bottom.rgba() ) );
        TileSet *tileSet = _dockFrameCache.object( key );
        if ( !tileSet )
        {

            int size( 13 );
            QPixmap pm( size, size );
            pm.fill( Qt::transparent );

            QPainter p( &pm );
            p.setRenderHints( QPainter::Antialiasing );
            p.setBrush( Qt::NoBrush );

            const QColor lightTop = alphaColor( calcLightColor( top ), 0.5 );
            const QColor lightBottom = alphaColor( calcLightColor( bottom ), 0.5 );
            const QColor darkTop = alphaColor( calcDarkColor( top ), 0.6 );
            const QColor darkBottom = alphaColor( calcDarkColor( bottom ), 0.6 );

            // dark frame
            {
                QLinearGradient lg( 0, 0.5, 0, size-1.5 );
                lg.setColorAt( 0.0, darkTop );
                lg.setColorAt( 1.0, darkBottom );

                p.setPen( QPen( lg, 1 ) );
                p.drawRoundedRect( QRectF( 1.5, 0.5, size-3, size-2 ), 4, 4 );
            }

            // bottom contrast
            {
                QLinearGradient lg( 0, 0.5, 0, size-0.5 );
                lg.setColorAt( 0.0, Qt::transparent );
                lg.setColorAt( 1.0, lightBottom );
                p.setPen( QPen( lg, 1.0 ) );
                p.drawRoundedRect( QRectF( 0.5, 0.5, size-1, size-1 ), 4.5, 4.5 );
            }

            // top contrast
            {
                QLinearGradient lg( 0, 1.5, 0, size-2.5 );
                lg.setColorAt( 0.0, lightTop );
                lg.setColorAt( 1.0, Qt::transparent );
                p.setPen( QPen( lg, 1.0 ) );
                p.drawRoundedRect( QRectF( 2.5, 1.5, size-5, size-4 ), 3.5, 3.5 );
            }

            p.end();
            tileSet = new TileSet( pm, (size-1)/2, (size-1)/2, 1, 1 );

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
