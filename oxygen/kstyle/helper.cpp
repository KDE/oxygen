/*
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
#include "elements/scrollbar.h"

#include <KColorUtils>

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

#include <math.h>

OxygenStyleHelper::OxygenStyleHelper(const QByteArray &componentName)
    : OxygenHelper(componentName)
{
}

SlabCache* OxygenStyleHelper::slabCache(const QColor &color)
{
    quint64 key = (quint64(color.rgba()) << 32);
    SlabCache *cache = m_slabCache.object(key);

    if (!cache)
    {
        cache = new SlabCache;
        m_slabCache.insert(key, cache);
    }

    return cache;
}

QPixmap OxygenStyleHelper::roundSlab(const QColor &color, double shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (int)(256.0 * shade) << 24 | size;
    QPixmap *pixmap = cache->m_roundSlabCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(size*3, int(double(size*3)*10.0/9.0));
        pixmap->fill(QColor(0,0,0,0));

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,18,20);

        QColor base = KColorUtils::shade(color, shade);
        QColor light = KColorUtils::shade(calcLightColor(color), shade);
        QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

        // shadow
        drawShadow(p, calcShadowColor(color), 18);

        // bevel, part 1
        qreal y = KColorUtils::luma(base);
        qreal yl = KColorUtils::luma(light);
        qreal yd = KColorUtils::luma(light);
        QLinearGradient bevelGradient1(0, 9, 0, 16);
        bevelGradient1.setColorAt(0.0, light);
        bevelGradient1.setColorAt(0.9, dark);
        if (y < yl && y > yd) // no middle when color is very light/dark
            bevelGradient1.setColorAt(0.5, base);
        p.setBrush(bevelGradient1);
        p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

        // bevel, part 2
        QLinearGradient bevelGradient2(0, 6, 0, 26);
        bevelGradient2.setColorAt(0.0, light);
        bevelGradient2.setColorAt(0.9, base);
        p.setBrush(bevelGradient2);
        p.drawEllipse(QRectF(2.6,2.6,12.8,12.8));

        // inside
        QLinearGradient innerGradient(-12, 0, 0, 18);
        innerGradient.setColorAt(0.0, light);
        innerGradient.setColorAt(1.0, base);
        p.setBrush(innerGradient);
        p.drawEllipse(QRectF(3.4,3.4,11.2,11.2));

        p.end();

        cache->m_roundSlabCache.insert(key, pixmap);
    }

    return *pixmap;
}

TileSet *OxygenStyleHelper::slab(const QColor &color, double shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = cache->m_slabCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, (int)ceil(double(size*2)*14.0/12.0));
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,12,14);

        QColor base = KColorUtils::shade(color, shade);
        QColor light = KColorUtils::shade(calcLightColor(color), shade);
        QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

        // shadow
        drawShadow(p, calcShadowColor(color), 12);

        // bevel, part 1
        qreal y = KColorUtils::luma(base);
        qreal yl = KColorUtils::luma(light);
        qreal yd = KColorUtils::luma(light);
        QLinearGradient bevelGradient1(0, 6, 0, 10);
        bevelGradient1.setColorAt(0.0, light);
        bevelGradient1.setColorAt(0.9, dark);
        if (y < yl && y > yd) // no middle when color is very light/dark
            bevelGradient1.setColorAt(0.5, base);
        p.setBrush(bevelGradient1);
        p.drawEllipse(QRectF(2.0,2.0,8.0,8.0));

        // bevel, part 2
        QLinearGradient bevelGradient2(0, 5, 0, 18);
        bevelGradient2.setColorAt(0.0, light);
        bevelGradient2.setColorAt(0.9, base);
        p.setBrush(bevelGradient2);
        p.drawEllipse(QRectF(2.6,2.6,6.8,6.8));

        // inside mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush(QBrush(Qt::black));
        p.drawEllipse(QRectF(3.4,3.4,5.2,5.2));

        p.end();

        tileSet = new TileSet(pixmap, size-1, size, 2, 1);

        cache->m_slabCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::slabFocused(const QColor &color, QColor glow, double shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (quint64(glow.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = cache->m_slabCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, (int)ceil(double(size*2)*14.0/12.0));
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,12,14);

        TileSet *slabTileSet = slab(color, shade, size);

        // slab
        slabTileSet->render(QRect(0,0,12,14), &p);

        // glow
        QRadialGradient rg = QRadialGradient(8.5, 8.5, 8.5, 8.5, 8.5);
        glow.setAlpha(0);
        rg.setColorAt(4.5/8.5 - 0.01, glow);
        glow.setAlpha(180);
        rg.setColorAt(4.5/8.5, glow);
        glow.setAlpha(70);
        rg.setColorAt(6.5/8.5, glow);
        glow.setAlpha(0);
        rg.setColorAt(1.0, glow);
        p.setBrush(rg);
        p.drawEllipse(QRectF(0, 0, 17, 17));

        p.end();

        tileSet = new TileSet(pixmap, size-1, size, 2, 1);

        cache->m_slabCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::slabSunken(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slabSunkenCache.object(key);

    if (!tileSet)
    {
        QImage tmpImg(17, 17, QImage::Format_ARGB32);
        QGradientStops stops;
        QPainter p;

        tmpImg.fill(Qt::transparent);

        // TODO
        p.begin(&tmpImg);
        p.setPen(Qt::NoPen);
        p.setBrush(Qt::black);
        p.setRenderHint(QPainter::Antialiasing);
        p.drawEllipse(QRectF(4.5, 4.5, 8, 8));
        p.end();

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 8, 8, 1, 1);

        m_slabSunkenCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::slope(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slopeCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*4, size*4);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setPen(Qt::NoPen);

        // edges
        TileSet *slabTileSet = slab(color, shade, size);
        slabTileSet->render(QRect(0, 0, size*4, size*5), &p,
                            TileSet::Left | TileSet::Right | TileSet::Top);

        p.setWindow(0,0,24,24);

        // bottom
        QColor base = color;
        QColor light = calcLightColor(color); //KColorUtils::shade(calcLightColor(color), shade));
        QLinearGradient fillGradient(0, -24, 0, 24);
        light.setAlphaF(0.4);
        fillGradient.setColorAt(0.0, light);
        base.setAlphaF(0.4);
        fillGradient.setColorAt(1.0, base);
        p.setBrush(fillGradient);
        p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        p.drawRect(2, 8, 20, 16);

        // fade bottom
        QLinearGradient maskGradient(0, 6, 0, 24);
        maskGradient.setColorAt(0.0, QColor(0, 0, 0, 255));
        maskGradient.setColorAt(1.0, QColor(0, 0, 0, 0));

        p.setBrush(maskGradient);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawRect(0, 8, 24, 16);

        tileSet = new TileSet(pixmap, size, size, size*2, 2);
        m_slopeCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::hole(const QColor &surroundColor)
{
    quint64 key = (quint64(surroundColor.rgba()) << 32);
    TileSet *tileSet = m_holeCache.object(key);

    if (!tileSet)
    {
        QImage tmpImg(9, 9, QImage::Format_ARGB32);
        QLinearGradient lg; QGradientStops stops;
        QPainter p;

        tmpImg.fill(Qt::transparent);

        p.begin(&tmpImg);
        p.setPen(Qt::NoPen);
        p.setRenderHint(QPainter::Antialiasing);
        p.scale(1.25, 1.0);
        QRadialGradient rg = QRadialGradient(4.5*0.8, 4.5, 5.0, 4.5*0.8, 4.5+1.3);
        stops.clear();
        stops << QGradientStop( 0.4, QColor(0,0,0, 0) )
           << QGradientStop( 0.58, QColor(0,0,0, 20) )
           << QGradientStop( 0.75, QColor(0,0,0, 53) )
           << QGradientStop( 0.88, QColor(0,0,0, 100) )
           << QGradientStop( 1, QColor(0,0,0, 150 ) );
        rg.setStops(stops);
        p.setBrush(rg);
        p.setClipRect(0,0,9,8);
        p.drawEllipse(QRectF(0,0, 9*0.8, 9));
        p.resetTransform();

        // draw white edge at bottom
        p.setClipRect(0,7,9,2);
        p.setBrush(Qt::NoBrush);
        p.setPen( KColorUtils::shade(surroundColor, 0.3));
        p.drawEllipse(QRectF(0.5, 0.5, 8, 8));
        p.setPen(Qt::NoPen);
        p.end();

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 4, 4, 1, 1);

        m_holeCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::holeFocused(const QColor &surroundColor, QColor glowColor)
{
    quint64 key = (quint64(surroundColor.rgba()) << 32) | quint64(glowColor.rgba());
    TileSet *tileSet = m_holeCache.object(key);

    if (!tileSet)
    {
        QImage tmpImg(9, 9, QImage::Format_ARGB32);
        QLinearGradient lg; QGradientStops stops;
        QPainter p;

        tmpImg.fill(Qt::transparent);

        p.begin(&tmpImg);
        p.setPen(Qt::NoPen);
        p.setRenderHint(QPainter::Antialiasing);
        p.scale(1.25, 1.0);
        QRadialGradient rg = QRadialGradient(4.5*0.8, 4.5, 5.0, 4.5*0.8, 4.5+1.3);
        stops.clear();
        stops << QGradientStop( 0.4, QColor(0,0,0, 0) )
           << QGradientStop( 0.58, QColor(0,0,0, 20) )
           << QGradientStop( 0.75, QColor(0,0,0, 53) )
           << QGradientStop( 0.88, QColor(0,0,0, 100) )
           << QGradientStop( 1, QColor(0,0,0, 150 ) );
        rg.setStops(stops);
        p.setBrush(rg);
        p.setClipRect(0,0,9,8);
        p.drawEllipse(QRectF(0,0, 9*0.8, 9));
        p.resetTransform();

        rg = QRadialGradient(4.5, 4.5, 5.0, 4.5, 4.5);
        stops.clear();
        glowColor.setAlpha(0);
        stops << QGradientStop(0, glowColor);
        glowColor.setAlpha(30);
        stops  << QGradientStop(0.30, glowColor);
        glowColor.setAlpha(110);
        stops  << QGradientStop(0.55, glowColor);
        glowColor.setAlpha(170);
        stops  << QGradientStop(0.65, glowColor);
        glowColor.setAlpha(0);
        stops  << QGradientStop(0.70, glowColor);
        rg.setStops(stops);
        p.setBrush(rg);
        p.setClipRect(0,0,9,9);
        p.drawEllipse(QRectF(0,0, 9, 9));

        // draw white edge at bottom
        p.setClipRect(0,7,9,2);
        p.setBrush(Qt::NoBrush);
        p.setPen( KColorUtils::shade(surroundColor, 0.3));
        p.drawRoundRect(QRectF(0.5, 0.5, 8, 8),90,90);
        p.setPen(Qt::NoPen);
        p.end();

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 4, 4, 1, 1);

        m_holeCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::slitFocused(const QColor &glowColor)
{
    quint64 key = (quint64(glowColor.rgba()) << 32);
    TileSet *tileSet = m_slitCache.object(key);

    if (!tileSet)
    {
        QImage tmpImg(9, 9, QImage::Format_ARGB32);
        QPainter p;

        tmpImg.fill(Qt::transparent);

        p.begin(&tmpImg);
        p.setPen(Qt::NoPen);
        p.setRenderHint(QPainter::Antialiasing);
        QRadialGradient rg = QRadialGradient(4.5, 4.5, 4.5, 4.5, 4.5);
        QColor tmpColor = glowColor;
        rg.setColorAt(0.75, tmpColor);
        tmpColor.setAlpha(0);
        rg.setColorAt(0.9, tmpColor);
        rg.setColorAt(0.6, tmpColor);
        p.setBrush(rg);
        p.drawEllipse(QRectF(0,0, 9, 9));

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 4, 4, 1, 1);

        m_slitCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::verticalScrollBar(const QColor &color, int width, int offset, int size)
{
    size = (size*4)/3; // this code is writetn wrong :-), with base size == 8, not 6
    offset %= (size * 4);

    quint64 key = (quint64(color.rgba()) << 32) | (width<<22) | (offset<<10) | size;
    TileSet *tileSet = m_verticalScrollBarCache.object(key);
    if (!tileSet)
    {
        tileSet = OxygenScrollbar(color, _contrast).vertical(size, width, offset);
        m_verticalScrollBarCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::horizontalScrollBar(const QColor &color, int width, int offset, int size)
{
    size = (size*4)/3; // this code is writetn wrong :-), with base size == 8, not 6
    offset %= (size * 4);

    quint64 key = (quint64(color.rgba()) << 32) | (width<<12) | offset;
    TileSet *tileSet = m_horizontalScrollBarCache.object(key);
    if (!tileSet)
    {
        tileSet = OxygenScrollbar(color, _contrast).horizontal(size, width, offset);
        m_horizontalScrollBarCache.insert(key, tileSet);
    }
    return tileSet;
}
