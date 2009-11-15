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

#include "helper.h"

#include <KColorUtils>
#include <KColorScheme>

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

#include <math.h>

//______________________________________________________________________________
OxygenStyleHelper::OxygenStyleHelper(const QByteArray &componentName)
    : OxygenHelper(componentName)
{
    m_dockFrameCache.setMaxCost(1);
    m_scrollHoleCache.setMaxCost(10);
}

//______________________________________________________________________________
void OxygenStyleHelper::invalidateCaches()
{
    m_progressBarCache.clear();
    m_cornerCache.clear();
    m_slabSunkenCache.clear();
    m_slabInvertedCache.clear();
    m_holeCache.clear();
    m_holeFlatCache.clear();
    m_slopeCache.clear();
    m_slitCache.clear();
    m_dockFrameCache.clear();
    m_scrollHoleCache.clear();
    OxygenHelper::invalidateCaches();
}

//______________________________________________________________________________
QPalette OxygenStyleHelper::mergePalettes( const QPalette& source, qreal ratio ) const
{

    QPalette out( source );
    out.setColor( QPalette::Background, KColorUtils::mix( source.color( QPalette::Active, QPalette::Background ), source.color( QPalette::Disabled, QPalette::Background ), 1.0-ratio ) );
    out.setColor( QPalette::WindowText, KColorUtils::mix( source.color( QPalette::Active, QPalette::WindowText ), source.color( QPalette::Disabled, QPalette::WindowText ), 1.0-ratio ) );
    out.setColor( QPalette::ButtonText, KColorUtils::mix( source.color( QPalette::Active, QPalette::ButtonText ), source.color( QPalette::Disabled, QPalette::ButtonText ), 1.0-ratio ) );
    out.setColor( QPalette::Text, KColorUtils::mix( source.color( QPalette::Active, QPalette::Text ), source.color( QPalette::Disabled, QPalette::Text ), 1.0-ratio ) );
    out.setColor( QPalette::Button, KColorUtils::mix( source.color( QPalette::Active, QPalette::Button ), source.color( QPalette::Disabled, QPalette::Button ), 1.0-ratio ) );
    return out;
}

//______________________________________________________________________________
QPixmap OxygenStyleHelper::windecoButton(const QColor &color, bool pressed, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (size << 1) | (int)pressed;
    QPixmap *pixmap = m_windecoButtonCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(size, size);
        pixmap->fill(Qt::transparent);

        QColor light  = calcLightColor(color);
        QColor dark   = calcDarkColor(color);

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        qreal u = size/18.0;
        p.translate( 0.5*u, (0.5-0.668)*u );

        {
            // outline circle
            qreal penWidth = 1.2;
            QLinearGradient lg( 0, u*(1.665-penWidth), 0, u*(12.33+1.665-penWidth) );
            lg.setColorAt( 0, dark );
            lg.setColorAt( 1, light );
            QRectF r( u*0.5*(17-12.33+penWidth), u*(1.665+penWidth), u*(12.33-penWidth), u*(12.33-penWidth) );
            p.setPen( QPen( lg, penWidth*u ) );
            p.drawEllipse( r );
            p.end();
        }

        m_windecoButtonCache.insert(key, pixmap);
    }

    return *pixmap;
}

//______________________________________________________________________________
QColor OxygenStyleHelper::calcMidColor(const QColor &color) const
{
    return KColorScheme::shade(color, KColorScheme::MidShade, _contrast - 1.0);
}

//______________________________________________________________________________
QPixmap OxygenStyleHelper::roundSlab(const QColor &color, qreal shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (int)(256.0 * shade) << 24 | size;
    QPixmap *pixmap = cache->m_roundSlabCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(size*3, size*3);
        pixmap->fill(Qt::transparent);

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,21,21);

        // shadow
        drawShadow(p, calcShadowColor(color), 21);
        drawRoundSlab( p, color, shade );
        p.end();

        cache->m_roundSlabCache.insert(key, pixmap);
    }

    return *pixmap;
}

//__________________________________________________________________________________________________________
QPixmap OxygenStyleHelper::roundSlabFocused(const QColor &color, const QColor &glowColor, qreal shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (quint64(glowColor.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    QPixmap *pixmap = cache->m_roundSlabCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(size*3, size*3);
        pixmap->fill(Qt::transparent);

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,21,21);

        drawShadow(p, calcShadowColor(color), 21);
        drawOuterGlow( p, glowColor, 21 );
        drawRoundSlab( p, color, shade );

        p.end();
        cache->m_roundSlabCache.insert(key, pixmap);

    }
    return *pixmap;
}

//__________________________________________________________________________________________________________
QPixmap OxygenStyleHelper::progressBarIndicator(const QPalette& pal, const QRect& rect )
{

    QColor highlight = pal.color(QPalette::Active, QPalette::Highlight);
    quint64 key = (quint64(highlight.rgba()) << 32) | (rect.width() << 16 ) | (rect.height() );

    QPixmap *pixmap = m_progressBarCache.object(key);
    if (!pixmap)
    {

        QRect local( rect );
        local.adjust( -1, -1, 1, 1 );
        local.adjust( 0, -1, 0, 0 );

        // set topLeft corner to 0.0
        local.translate( -local.topLeft() );

        pixmap = new QPixmap(local.size());
        pixmap->fill( Qt::transparent );

        QPainter p( pixmap );
        p.setRenderHints( QPainter::Antialiasing );
        p.setBrush(Qt::NoBrush);

        local.adjust( 1, 1, -1, -1 );
        QColor lhighlight = calcLightColor(highlight);
        QColor color = pal.color(QPalette::Active, QPalette::Window);
        QColor light = calcLightColor(color);
        QColor dark = calcDarkColor(color);
        QColor shadow = calcShadowColor(color);

        // shadow
        p.setPen(alphaColor(shadow, 0.6));
        p.drawRoundedRect(local,2,1);

        // fill
        p.setPen(Qt::NoPen);
        p.setBrush(KColorUtils::mix(highlight, dark, 0.2));
        p.drawRect(local.adjusted(1, 0, -1, 0 ) );

        // fake radial gradient
        local.adjust( 0, 0, -1, 0 );
        QPixmap pm(local.size());
        pm.fill(Qt::transparent);
        {
            QRectF pmRect = pm.rect();
            QLinearGradient mask(pmRect.topLeft(), pmRect.topRight());
            mask.setColorAt(0.0, Qt::transparent);
            mask.setColorAt(0.4, Qt::black);
            mask.setColorAt(0.6, Qt::black);
            mask.setColorAt(1.0, Qt::transparent);

            QLinearGradient radial(pmRect.topLeft(), pmRect.bottomLeft());
            radial.setColorAt(0.0, KColorUtils::mix(lhighlight, light, 0.3));
            radial.setColorAt(0.5, Qt::transparent);
            radial.setColorAt(0.6, Qt::transparent);
            radial.setColorAt(1.0, KColorUtils::mix(lhighlight, light, 0.3));

            QPainter pp(&pm);
            pp.fillRect(pm.rect(), mask);
            pp.setCompositionMode(QPainter::CompositionMode_SourceIn);
            pp.fillRect(pm.rect(), radial);
            pp.end();

        }

        p.drawPixmap( QPoint(1,1), pm);

        // bevel
        p.setRenderHint(QPainter::Antialiasing, false);
        QLinearGradient bevel(local.topLeft(), local.bottomLeft());
        bevel.setColorAt(0, lhighlight);
        bevel.setColorAt(0.5, highlight);
        bevel.setColorAt(1, calcDarkColor(highlight));
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(bevel, 1));
        p.drawRoundedRect(local,2,2);

        // bright top edge
        QLinearGradient lightHl(local.topLeft(),local.topRight());
        lightHl.setColorAt(0, Qt::transparent);
        lightHl.setColorAt(0.5, KColorUtils::mix(highlight, light, 0.8));
        lightHl.setColorAt(1, Qt::transparent);

        p.setPen(QPen(lightHl, 1));
        p.drawLine(local.topLeft(), local.topRight());
        p.end();

        m_progressBarCache.insert(key, pixmap);
    }

    return *pixmap;

}

//__________________________________________________________________________________________________________
void OxygenStyleHelper::drawHole(QPainter &p, const QColor &color, qreal shade, int r) const
{
    const int r2 = 2*r;
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);
    QColor mid = KColorUtils::shade(calcMidColor(color), shade);

    // bevel
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 2, 0, r2-2);
    bevelGradient1.setColorAt(0.2, dark);
    bevelGradient1.setColorAt(0.5, mid);
    bevelGradient1.setColorAt(1.0, light);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.6, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(3,3,r2-5,r2-5);

    // mask
    QRadialGradient maskGradient(r,r,r-2);
    maskGradient.setColorAt(0.80, QColor(0,0,0,255));
    maskGradient.setColorAt(0.90, QColor(0,0,0,140));
    maskGradient.setColorAt(1.00, QColor(0,0,0,0));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,r2,r2);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

//__________________________________________________________________________________________________________
void OxygenStyleHelper::drawRoundSlab(QPainter &p, const QColor &color, qreal shade ) const
{

    p.save();

    // colors
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 10, 0, 18);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(3.0,3.0,15.0,15.0));

    // bevel, part 2
    if (_slabThickness > 0.0) {
        QLinearGradient bevelGradient2(0, 7, 0, 28);
        bevelGradient2.setColorAt(0.0, light);
        bevelGradient2.setColorAt(0.9, base);
        p.setBrush(bevelGradient2);
        p.drawEllipse(QRectF(3.6,3.6,13.8,13.8));
    }

    // inside
    QLinearGradient innerGradient(0, -17, 0, 20);
    innerGradient.setColorAt(0.0, light);
    innerGradient.setColorAt(1.0, base);
    p.setBrush(innerGradient);
    qreal ic = 3.6 + _slabThickness;
    qreal is = 13.8 - (2.0*_slabThickness);
    p.drawEllipse(QRectF(ic, ic, is, is));

    p.restore();

}

//________________________________________________________________________________________________________
void OxygenStyleHelper::drawInverseShadow(
    QPainter &p, const QColor &color,
    int pad, int size, qreal fuzz) const
{
    qreal m = qreal(size)*0.5;

    const qreal offset = 0.8;
    qreal k0 = (m-2.0) / qreal(m+2.0);
    QRadialGradient shadowGradient(pad+m, pad+m+offset, m+2.0);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        qreal k1 = (qreal(8 - i) + k0 * qreal(i)) * 0.125;
        qreal a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a * _shadowGain));
    }
    shadowGradient.setColorAt(k0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0));
}

//________________________________________________________________________________________________________
void OxygenStyleHelper::drawInverseGlow(
    QPainter &p, const QColor &color,
    int pad, int size, int rsize) const
{
    QRectF r(pad, pad, size, size);
    qreal m = qreal(size)*0.5;

    const qreal width = 3.0;
    const qreal bias = _glowBias * 7.0 / qreal(rsize);
    qreal k0 = (m-width) / (m-bias);
    QRadialGradient glowGradient(pad+m, pad+m, m-bias);
    for (int i = 0; i < 8; i++)
    {
      // inverse parabolic gradient
      qreal k1 = (k0 * qreal(i) + qreal(8 - i)) * 0.125;
      qreal a = 1.0 - sqrt(i * 0.125);
      glowGradient.setColorAt(k1, alphaColor(color, a));

    }

    glowGradient.setColorAt(k0, alphaColor(color, 0.0));
    p.setBrush(glowGradient);
    p.drawEllipse(r);
}

//________________________________________________________________________________________________________
void OxygenStyleHelper::fillSlab(QPainter &p, const QRect &rect, int size)
{
    const qreal s = qreal(size) * (3.6 + (0.5 * _slabThickness)) / 7.0;
    QRectF r = rect;
    r.adjust(s, s, -s, -s);
    qreal w = r.width(), h = r.height();
    if (w <= 0 || h <= 0) return;
    const qreal ra = 200.0 * (7.0 - (3.6 + (0.5 * _slabThickness))) / 7.0;
    qreal rx = floor((ra*size) / w);
    qreal ry = floor((ra*size) / h);

    p.drawRoundRect(r, rx, ry);
}

//________________________________________________________________________________________________________
void OxygenStyleHelper::fillHole(QPainter &p, const QRect &rect, int size)
{
    const qreal s = qreal(size) * 3.0 / 7.0;
    p.drawRoundedRect(rect.adjusted(s,s,-s,-s), 4, 4);
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::slabFocused(const QColor &color, const QColor &glowColor, qreal shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (quint64(glowColor.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = cache->m_slabCache.object(key);

    const qreal hScale( 1 );
    const int hSize( size*hScale );
    const int vSize( size );

    if (!tileSet)
    {
        QPixmap pixmap(hSize*2,vSize*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);

        int fixedSize = 14;
        p.setWindow(0,0,fixedSize*hScale, fixedSize);

        // draw all components
        drawShadow(p, calcShadowColor(color), 14);
        drawOuterGlow( p, glowColor, 14 );
        drawSlab(p, color, shade);

        //slab(color, shade, size)->render(QRect( 0, 0, fixedSize*hScale, fixedSize), &p);
        //outerGlow( glowColor, size)->render( QRect( 0, 0, fixedSize*hScale, fixedSize), &p);

        p.end();

        tileSet = new TileSet(pixmap, hSize, vSize, hSize, vSize, hSize-1, vSize, 2, 1);

        cache->m_slabCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::roundCorner(const QColor &color, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slabSunkenCache.object(key);

    if (!tileSet)
    {

        QPixmap pixmap = QPixmap( size*2, size*2 );
        pixmap.fill( Qt::transparent );

        QPainter p( &pixmap );
        p.setRenderHint( QPainter::Antialiasing );
        p.setPen( Qt::NoPen );

        QLinearGradient lg = QLinearGradient(0.0, size-4.5, 0.0, size+4.5);
        lg.setColorAt(0.0, calcLightColor( backgroundTopColor(color) ));
        lg.setColorAt(0.52, backgroundTopColor(color) );
        lg.setColorAt(1.0, backgroundBottomColor(color) );

        // draw ellipse.
        p.setBrush( lg );
        p.drawEllipse( QRectF( size-4, size-4, 8, 8 ) );

        tileSet = new TileSet(pixmap, size, size, 1, 1);
        m_cornerCache.insert(key, tileSet);

    }

    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::slabSunken(const QColor &color, qreal shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slabSunkenCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, size*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,14,14);

        // slab
        drawSlab(p, color, shade);

        // shadow
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        drawInverseShadow(p, calcShadowColor(color), 3, 8, 0.0);

        p.end();

        tileSet = new TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);

        m_slabSunkenCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::slabInverted(const QColor &color, qreal shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slabInvertedCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, size*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,14,14);

        QColor base = KColorUtils::shade(color, shade);
        QColor light = KColorUtils::shade(calcLightColor(color), shade);
        QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

        // bevel, part 2
        QLinearGradient bevelGradient2(0, 8, 0, -8);
        bevelGradient2.setColorAt(0.0, light);
        bevelGradient2.setColorAt(0.9, base);
        p.setBrush(bevelGradient2);
        p.drawEllipse(QRectF(2.6,2.6,8.8,8.8));

        // bevel, part 1
        qreal y = KColorUtils::luma(base);
        qreal yl = KColorUtils::luma(light);
        qreal yd = KColorUtils::luma(dark);
        QLinearGradient bevelGradient1(0, 7, 0, 4);
        bevelGradient1.setColorAt(0.0, light);
        bevelGradient1.setColorAt(0.9, dark);

        if (y < yl && y > yd)
        {
            // no middle when color is very light/dark
            bevelGradient1.setColorAt(0.5, base);
        }

        p.setBrush(bevelGradient1);
        p.drawEllipse(QRectF(3.4,3.4,7.2,7.2));

        // inside mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush(QBrush(Qt::black));
        p.drawEllipse(QRectF(4.0,4.0,6.0,6.0));

        // shadow
        p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        drawInverseShadow(p, calcShadowColor(color), 4, 6, 0.5);

        p.end();

        tileSet = new TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);

        m_slabInvertedCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::slope(const QColor &color, qreal shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slopeCache.object(key);

    if (!tileSet)
    {
        // TODO - rebase??
        QPixmap pixmap(size*4, size*4);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setPen(Qt::NoPen);

        // edges
        TileSet *slabTileSet = slab(color, shade, size);
        slabTileSet->render(QRect(0, 0, size*4, size*5), &p,
                            TileSet::Left | TileSet::Right | TileSet::Top);

        p.setWindow(0,0,28,28);

        // bottom
        QColor base = color;
        QColor light = KColorUtils::shade(calcLightColor(color), shade);
        QLinearGradient fillGradient(0, -28, 0, 28);
        light.setAlphaF(0.4);
        fillGradient.setColorAt(0.0, light);
        base.setAlphaF(0.4);
        fillGradient.setColorAt(1.0, base);
        p.setBrush(fillGradient);
        p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        p.drawRect(3, 9, 22, 17);

        // fade bottom
        QLinearGradient maskGradient(0, 7, 0, 28);
        maskGradient.setColorAt(0.0, QColor(0, 0, 0, 255));
        maskGradient.setColorAt(1.0, QColor(0, 0, 0, 0));

        p.setBrush(maskGradient);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawRect(0, 9, 28, 19);

        p.end();

        tileSet = new TileSet(pixmap, size, size, size*2, 2);

        m_slopeCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::hole(const QColor &color, qreal shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = m_holeCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(qreal(size) * 5.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(2,2,10,10);

        // hole mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush(Qt::black);
        p.drawEllipse(3,3,8,8);

        // shadow
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        drawInverseShadow(p, calcShadowColor(color), 3, 8, 0.0);

        p.end();

        tileSet = new TileSet(pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1);

        m_holeCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::holeFlat(const QColor &color, qreal shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = m_holeFlatCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(qreal(size) * 5.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(2,2,10,10);

        // hole
        drawHole(p, color, shade, 7);

        // hole inside
        p.setBrush(color);
        p.drawEllipse(QRectF(3.2,3.2,7.6,7.6));

        p.end();

        tileSet = new TileSet(pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1);

        m_holeFlatCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::holeFocused(const QColor &color, const QColor &glowColor, qreal shade, int size)
{
    // FIXME must move to s/slabcache/cache/ b/c key is wrong
    quint64 key = (quint64(color.rgba()) << 32) | quint64(glowColor.rgba());
    TileSet *tileSet = m_holeCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(qreal(size) * 5.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);

        TileSet *holeTileSet = hole(color, shade, size);

        // hole
        holeTileSet->render(QRect(0,0,10,10), &p);

        p.setWindow(2,2,10,10);

        // glow
        drawInverseGlow(p, glowColor, 3, 8, size);

        p.end();

        tileSet = new TileSet(pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1);

        m_holeCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::groove(const QColor &color, qreal shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = m_grooveCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(qreal(size) * 3.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(2,2,6,6);

        // hole mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush(Qt::black);
        p.drawEllipse(4,4,2,2);

        // shadow
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        drawInverseShadow(p, calcShadowColor(color), 3, 4, 0.0);

        p.end();

        tileSet = new TileSet(pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1);

        m_grooveCache.insert(key, tileSet);
    }
    return tileSet;
}

//________________________________________________________________________________________________________
TileSet *OxygenStyleHelper::slitFocused(const QColor &glowColor)
{
    quint64 key = (quint64(glowColor.rgba()) << 32);
    TileSet *tileSet = m_slitCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(9,9);
        QPainter p;

        pixmap.fill(Qt::transparent);

        p.begin(&pixmap);
        p.setPen(Qt::NoPen);
        p.setRenderHint(QPainter::Antialiasing);
        QRadialGradient rg = QRadialGradient(4.5, 4.5, 4.5, 4.5, 4.5);
        QColor tmpColor = glowColor;
        tmpColor.setAlpha(180*glowColor.alphaF());
        rg.setColorAt(0.75, tmpColor);
        tmpColor.setAlpha(0);
        rg.setColorAt(0.90, tmpColor);
        rg.setColorAt(0.4, tmpColor);
        p.setBrush(rg);
        p.drawEllipse(QRectF(0, 0, 9, 9));

        p.end();

        tileSet = new TileSet(pixmap, 4, 4, 1, 1);

        m_slitCache.insert(key, tileSet);
    }
    return tileSet;
}

//____________________________________________________________________
TileSet *OxygenStyleHelper::dockFrame(const QColor &color, int width)
{
    quint64 key = quint64(color.rgba()) << 32 | width;
    TileSet *tileSet = m_dockFrameCache.object(key);
    if (!tileSet)
    {
        if (!width&1) // width should be uneven
            --width;

        int w = width;
        int h = 9;

        QPixmap pm(w, h);
        pm.fill(Qt::transparent);

        QPainter p(&pm);
        p.setRenderHints(QPainter::Antialiasing);
        p.setBrush(Qt::NoBrush);
        p.translate(0.5, 0.5);
        QRect rect(0.5,0.5,w-0.5,h-0.);

        QColor light = calcLightColor(color);
        QColor dark = calcDarkColor(color);

        //dark.setAlpha(200);
        light.setAlpha(150);

        // left and right border
        QLinearGradient lg(QPoint(0,0), QPoint(w,0));
        lg.setColorAt(0.0, light);
        lg.setColorAt(0.1, QColor(0,0,0,0));
        lg.setColorAt(0.9, QColor(0,0,0,0));
        lg.setColorAt(1.0, light);
        p.setPen(QPen(lg,1));
        p.drawRoundedRect(rect.adjusted(0,-1,0,-1),4,5);
        p.drawRoundedRect(rect.adjusted(2,1,-2,-2),4,5);

        lg.setColorAt(0.0, dark);
        lg.setColorAt(0.1, QColor(0,0,0,0));
        lg.setColorAt(0.9, QColor(0,0,0,0));
        lg.setColorAt(1.0, dark);
        p.setPen(QPen(lg, 1));
        p.drawRoundedRect(rect.adjusted(1,0,-1,-2),4,5);

        // top and bottom border
        drawSeparator(&p, QRect(0,0,w,2), color, Qt::Horizontal);
        drawSeparator(&p, QRect(0,h-2,w,2), color, Qt::Horizontal);

        p.end();

        tileSet = new TileSet(pm, 4, 4, w-8, h-8);

        m_dockFrameCache.insert(key, tileSet);
    }
    return tileSet;
}

//______________________________________________________________________________
TileSet *OxygenStyleHelper::scrollHole(const QColor &color, Qt::Orientation orientation, bool smallShadow )
{
    const quint64 key = quint64(color.rgba()) << 32 | (orientation == Qt::Horizontal ? 2 : 0) | (smallShadow ? 1 : 0);
    TileSet *tileSet = m_scrollHoleCache.object(key);
    if (!tileSet)
    {
        QPixmap pm(15, 15);
        pm.fill(Qt::transparent);

        QPainter p(&pm);

        QColor dark = calcDarkColor(color);
        QColor light = calcLightColor(color);
        QColor shadow = calcShadowColor(color);

        // use space for white border
        QRect r = QRect(0,0,15,15);
        QRect rect = r.adjusted(1, 0, -1, -1);
        int shadowWidth(0);
        if( smallShadow ) shadowWidth = (orientation == Qt::Horizontal) ? 2 : 1;
        else shadowWidth = (orientation == Qt::Horizontal) ? 3 : 2;

        p.setRenderHints(QPainter::Antialiasing);
        p.setBrush(dark);
        p.setPen(Qt::NoPen);

        // base
        p.drawRoundedRect(rect, 4.5, 4.5);

        // slight shadow across the whole hole
        QLinearGradient shadowGradient(rect.topLeft(),
            orientation == Qt::Horizontal ?
            rect.bottomLeft():rect.topRight());

        shadowGradient.setColorAt(0.0, alphaColor(shadow, 0.1));
        shadowGradient.setColorAt(0.6, Qt::transparent);
        p.setBrush(shadowGradient);
        p.drawRoundedRect(rect, 4.5, 4.5);

        // strong shadow

        // left
        QLinearGradient l1 = QLinearGradient(rect.topLeft(), rect.topLeft()+QPoint(shadowWidth,0));
        l1.setColorAt(0.0, alphaColor(shadow, orientation == Qt::Horizontal ? 0.3 : 0.2));
        l1.setColorAt(0.5, alphaColor(shadow, orientation == Qt::Horizontal ? 0.1 : 0.1));
        l1.setColorAt(1.0, Qt::transparent);
        p.setBrush(l1);
        p.drawRoundedRect(QRect(rect.topLeft(), rect.bottomLeft()+QPoint(shadowWidth,0)), 4.5, 4.5);

        // right
        l1 = QLinearGradient(rect.topRight(), rect.topRight()-QPoint(shadowWidth,0));
        l1.setColorAt(0.0, alphaColor(shadow, orientation == Qt::Horizontal ? 0.3 : 0.2));
        l1.setColorAt(0.5, alphaColor(shadow, orientation == Qt::Horizontal ? 0.1 : 0.1));
        l1.setColorAt(1.0, Qt::transparent);
        p.setBrush(l1);
        p.drawRoundedRect(QRect(rect.topRight()-QPoint(shadowWidth,0), rect.bottomRight()), 4.5, 4.5);
        //top
        l1 = QLinearGradient(rect.topLeft(), rect.topLeft()+QPoint(0,3));
        l1.setColorAt(0.0, alphaColor(shadow, 0.3));
        l1.setColorAt(1.0, Qt::transparent);
        p.setBrush(l1);
        p.drawRoundedRect(QRect(rect.topLeft(),rect.topRight()+QPoint(0,3)), 4.5, 4.5);

        // light border
        QLinearGradient borderGradient(r.topLeft()+QPoint(0,r.height()/2-1), r.bottomLeft());
        borderGradient.setColorAt(0.0, Qt::transparent);
        borderGradient.setColorAt(1.0, alphaColor(light, 0.8));
        p.setPen( QPen(borderGradient, 1.0) );
        p.setBrush(Qt::NoBrush);
        p.drawRoundedRect(r.adjusted(0.5,0,-0.5,0), 5.0, 5.0);

        p.end();

        tileSet = new TileSet(pm, 7, 7, 1, 1);

        m_scrollHoleCache.insert(key, tileSet);
    }
    return tileSet;
}
