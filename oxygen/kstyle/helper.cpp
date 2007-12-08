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
#include <KColorScheme>

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

#include <math.h>

const double OxygenStyleHelper::_slabThickness = 0.45; //TODO: configurable?

OxygenStyleHelper::OxygenStyleHelper(const QByteArray &componentName)
    : OxygenHelper(componentName)
{
}

QColor OxygenStyleHelper::calcMidColor(const QColor &color) const
{
    return KColorScheme::shade(color, KColorScheme::MidShade, _contrast - 1.0);
}

void OxygenStyleHelper::invalidateCaches()
{
    m_slabCache.clear();
    m_slabSunkenCache.clear();
    m_slabInvertedCache.clear();
    m_holeCache.clear();
    m_holeFlatCache.clear();
    m_slopeCache.clear();
    m_slitCache.clear();
    m_verticalScrollBarCache.clear();
    m_horizontalScrollBarCache.clear();
    OxygenHelper::invalidateCaches();
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
        pixmap = new QPixmap(size*3, size*3);
        pixmap->fill(QColor(0,0,0,0));

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,21,21);

        QColor base = KColorUtils::shade(color, shade);
        QColor light = KColorUtils::shade(calcLightColor(color), shade);
        QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

        // shadow
        drawShadow(p, calcShadowColor(color), 21);

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
        double ic = 3.6 + _slabThickness;
        double is = 13.8 - (2.0*_slabThickness);
        p.drawEllipse(QRectF(ic, ic, is, is));

        p.end();

        cache->m_roundSlabCache.insert(key, pixmap);
    }

    return *pixmap;
}

QPixmap OxygenStyleHelper::roundSlabFocused(const QColor &color, const QColor &glowColor, double shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (quint64(glowColor.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    QPixmap *pixmap = cache->m_roundSlabCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(size*3, size*3);
        pixmap->fill(QColor(0,0,0,0));

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,21,21);

        // slab
        QPixmap slabPixmap = roundSlab(color, shade, size);
        p.drawPixmap(0, 0, slabPixmap);

        // glow
        QPixmap gp = glow(glowColor, 21, size*3);
        p.drawPixmap(0, 0, gp);

        p.end();

        cache->m_roundSlabCache.insert(key, pixmap);
    }
    return *pixmap;
}

void OxygenStyleHelper::drawHole(QPainter &p, const QColor &color, double shade) const
{
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);
    QColor mid = KColorUtils::shade(calcMidColor(color), shade);

    // bevel
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 2, 0, 12);
    bevelGradient1.setColorAt(0.2, dark);
    bevelGradient1.setColorAt(0.5, mid);
    bevelGradient1.setColorAt(1.0, light);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.6, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(2,2,10,10);

    // mask
    QRadialGradient maskGradient(7,7,5);
    maskGradient.setColorAt(0.80, QColor(0,0,0,255));
    maskGradient.setColorAt(0.90, QColor(0,0,0,140));
    maskGradient.setColorAt(1.00, QColor(0,0,0,0));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,14,14);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

void OxygenStyleHelper::drawGroove(QPainter &p, const QColor &color, double shade) const
{
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);
    QColor mid = KColorUtils::shade(calcMidColor(color), shade);

    // bevel
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 2, 0, 8);
    bevelGradient1.setColorAt(0.2, dark);
    bevelGradient1.setColorAt(0.5, mid);
    bevelGradient1.setColorAt(1.0, light);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.7, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(2,2,6,6);

    // mask
    QRadialGradient maskGradient(5,5,3);
    maskGradient.setColorAt(0.8, QColor(0,0,0,255));
    maskGradient.setColorAt(0.9, QColor(0,0,0,140));
    maskGradient.setColorAt(1.00, QColor(0,0,0,0));
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.setBrush(maskGradient);
    p.drawRect(0,0,10,10);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

void OxygenStyleHelper::drawSlab(QPainter &p, const QColor &color, double shade) const
{
    QColor base = KColorUtils::shade(color, shade);
    QColor light = KColorUtils::shade(calcLightColor(color), shade);
    QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // bevel, part 1
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 7, 0, 11);
    bevelGradient1.setColorAt(0.0, light);
    bevelGradient1.setColorAt(0.9, dark);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(3.0,3.0,8.0,8.0));

    // bevel, part 2
    if (_slabThickness > 0.0) {
        QLinearGradient bevelGradient2(0, 6, 0, 19);
        bevelGradient2.setColorAt(0.0, light);
        bevelGradient2.setColorAt(0.9, base);
        p.setBrush(bevelGradient2);
        p.drawEllipse(QRectF(3.6,3.6,6.8,6.8));
    }

    // inside mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(QBrush(Qt::black));
    double ic = 3.6 + _slabThickness;
    double is = 6.8 - (2.0*_slabThickness);
    p.drawEllipse(QRectF(ic, ic, is, is));
}

void OxygenStyleHelper::drawInverseShadow(QPainter &p, const QColor &color,
                                          int pad, int size, double fuzz) const
{
    double m = double(size)*0.5;

    const double offset = 0.8;
    double k0 = (m-2.0) / double(m+2.0);
    QRadialGradient shadowGradient(pad+m, pad+m+offset, m+2.0);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = (double(8 - i) + k0 * double(i)) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a * _shadowGain));
    }
    shadowGradient.setColorAt(k0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(pad-fuzz, pad-fuzz, size+fuzz*2.0, size+fuzz*2.0));
}

void OxygenStyleHelper::drawInverseGlow(QPainter &p, const QColor &color,
                                        int pad, int size, int rsize) const
{
    QRectF r(pad, pad, size, size);
    double m = double(size)*0.5;

    const double width = 3.0;
    const double bias = _glowBias * 7.0 / double(rsize);
    double k0 = (m-width) / (m-bias);
    QRadialGradient glowGradient(pad+m, pad+m, m-bias);
    for (int i = 0; i < 8; i++) { // inverse parabolic gradient
        double k1 = (k0 * double(i) + double(8 - i)) * 0.125;
        double a = 1.0 - sqrt(i * 0.125);
        glowGradient.setColorAt(k1, alphaColor(color, a));
    }
    glowGradient.setColorAt(k0, alphaColor(color, 0.0));
    p.setBrush(glowGradient);
    p.drawEllipse(r);
}

void OxygenStyleHelper::fillSlab(QPainter &p, const QRect &rect, int size)
{
    const double s = double(size) * (3.6 + (0.5 * _slabThickness)) / 7.0;
    QRectF r = rect;
    r.adjust(s, s, -s, -s);
    double w = r.width(), h = r.height();
    if (w <= 0 || h <= 0)
        return;
    const double ra = 200.0 * (7.0 - (3.6 + (0.5 * _slabThickness))) / 7.0;
    int rx = (int)floor((ra*size) / w);
    int ry = (int)floor((ra*size) / h);

    p.drawRoundRect(r, rx, ry);
}

void OxygenStyleHelper::fillHole(QPainter &p, const QRect &rect, int size)
{
    const double s = double(size) * 3.0 / 7.0;
    QRectF r = rect;
    r.adjust(s, s, -s, -s);
    double w = r.width(), h = r.height();
    if (w <= 0 || h <= 0)
        return;
    const double ra = 200.0 * 4.0 / 7.0;
    int rx = (int)floor((ra*size) / w);
    int ry = (int)floor((ra*size) / h);

    p.drawRoundRect(r, rx, ry);
}

TileSet *OxygenStyleHelper::slab(const QColor &color, double shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = cache->m_slabCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, size*2);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,14,14);

        // shadow
        drawShadow(p, calcShadowColor(color), 14);

        // slab
        drawSlab(p, color, shade);

        p.end();

        tileSet = new TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);

        cache->m_slabCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::slabFocused(const QColor &color, const QColor &glowColor, double shade, int size)
{
    SlabCache *cache = slabCache(color);
    quint64 key = (quint64(glowColor.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = cache->m_slabCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2,size*2);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,14,14);

        TileSet *slabTileSet = slab(color, shade, size);

        // slab
        slabTileSet->render(QRect(0,0,14,14), &p);

        // glow
        QPixmap gp = glow(glowColor, 14, size*2);
        p.drawPixmap(0, 0, gp);

        p.end();

        tileSet = new TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);

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
        QPixmap pixmap(size*2, size*2);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,14,14);

        // hole
        drawHole(p, color, shade);

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

TileSet *OxygenStyleHelper::slabInverted(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slabInvertedCache.object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, size*2);
        pixmap.fill(QColor(0,0,0,0));

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
        if (y < yl && y > yd) // no middle when color is very light/dark
            bevelGradient1.setColorAt(0.5, base);
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

TileSet *OxygenStyleHelper::slope(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32);
    TileSet *tileSet = m_slopeCache.object(key);

    if (!tileSet)
    {
        // TODO - rebase??
        QPixmap pixmap(size*4, size*4);
        pixmap.fill(QColor(0,0,0,0));

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

        tileSet = new TileSet(pixmap, size, size, size*2, 2);
        m_slopeCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::hole(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = m_holeCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(double(size) * 5.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(2,2,10,10);

        // hole
        drawHole(p, color, shade);

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

TileSet *OxygenStyleHelper::holeFlat(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = m_holeFlatCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(double(size) * 5.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(2,2,10,10);

        // hole
        drawHole(p, color, shade);

        // hole mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush(Qt::black);
        p.drawEllipse(3,3,8,8);

        p.end();

        tileSet = new TileSet(pixmap, rsize, rsize, rsize, rsize, rsize-1, rsize, 2, 1);

        m_holeFlatCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::holeFocused(const QColor &color, const QColor &glowColor, double shade, int size)
{
    // FIXME must move to s/slabcache/cache/ b/c key is wrong
    quint64 key = (quint64(color.rgba()) << 32) | quint64(glowColor.rgba());
    TileSet *tileSet = m_holeCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(double(size) * 5.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(QColor(0,0,0,0));

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

TileSet *OxygenStyleHelper::groove(const QColor &color, double shade, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = m_grooveCache.object(key);

    if (!tileSet)
    {
        int rsize = (int)ceil(double(size) * 3.0/7.0);
        QPixmap pixmap(rsize*2, rsize*2);
        pixmap.fill(QColor(0,0,0,0));

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(2,2,6,6);

        // hole
        drawGroove(p, color, shade);

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
