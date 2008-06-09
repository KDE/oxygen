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

#include "scrollbar.h"

#include <KColorUtils>
#include <KColorScheme>

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

inline QColor alphaColor(QColor color, double alpha)
{
    color.setAlphaF(alpha);
    return color;
}

OxygenScrollbar::OxygenScrollbar(const QColor &c, double contrast) : color(c),
    light(KColorScheme::shade(c, KColorScheme::LightShade, contrast - 0.5)),
    mid(KColorScheme::shade(c, KColorScheme::MidShade, contrast)),
    dark(KColorScheme::shade(c, KColorScheme::DarkShade, contrast - 0.5)),
    shadow(KColorScheme::shade(c, KColorScheme::ShadowShade, contrast)),
    highlight(Qt::white)
{
    double y = KColorUtils::luma(color);
    if (y > KColorUtils::luma(light)) {
        light = Qt::white;
        dark = KColorScheme::shade(c, KColorScheme::DarkShade, contrast);
    }
}

void OxygenScrollbar::mask(QPainter &p, const QRectF &rect) const
{
    double w = rect.width();
    double h = rect.height();

    // drawRoundRect is too bloody hard to control to get the corners perfectly
    // square (i.e. circles not ellipses), so draw the mask in parts with real
    // circles
    p.setBrush(Qt::black); // color doesn't matter
    p.drawRect(rect.adjusted(7,0,-7,0));
    p.drawRect(rect.adjusted(0,7,0,-7));
    p.drawEllipse(QRectF(0,0,14,14));
    p.drawEllipse(QRectF(w-14,0,14,14));
    p.drawEllipse(QRectF(0,h-14,14,14));
    p.drawEllipse(QRectF(w-14,h-14,14,14));

    // never draw outside the mask
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
}

QLinearGradient OxygenScrollbar::baseGradient(double width, Qt::Orientation orient) const
{
    double x = 0.0, y1 = width, y2 = width;
    if (orient == Qt::Vertical)
        x = width * 0.6;
    else
        y2 = width * 0.4;

    QLinearGradient gradient(0, y1, x, y2);
    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, mid);

    return gradient;
}

QLinearGradient OxygenScrollbar::shineGradient(double width, Qt::Orientation orient) const
{
    QLinearGradient gradient(0, width, 0, -width);
    gradient.setColorAt(0.0, light);
    gradient.setColorAt(0.5, alphaColor(color, 0.5));
    gradient.setColorAt(1.0, color);

    return gradient;
}

QLinearGradient OxygenScrollbar::shimmerGradient(double offset, Qt::Orientation orient) const
{
    double x = 0.0, y = 0.0, xo = 0.0, yo = 0.0;
    if (orient == Qt::Vertical) {
        yo = offset;
        x = 14.4/2.0;
        y = 43.2/2.0;
    } else {
        xo = offset;
        x = 43.2/2.0;
        y = -14.4/2.0;
    }

    // should tile every 48 units, with 1:3 slope
    QLinearGradient gradient(xo, yo, x+xo, y+yo);
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0.0, alphaColor(dark, 0.40));
    gradient.setColorAt(0.6, alphaColor(dark, 0.10));
    gradient.setColorAt(1.0, alphaColor(dark, 0.00));

    return gradient;
}

QLinearGradient OxygenScrollbar::dimGradient(Qt::Orientation orient) const
{
    int x = 0, y = 0;
    if (orient == Qt::Vertical)
        y = 3*22;
    else
        x = 3*22;

    QLinearGradient gradient(0, 0, x, y);
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0.00, alphaColor(dark, 1.0));
    gradient.setColorAt(0.19, alphaColor(dark, 0.3));
    gradient.setColorAt(0.27, alphaColor(dark, 0.0));

    return gradient;
}

QPixmap OxygenScrollbar::bevel(int width, int height, double w, double h, int rx, int ry) const
{
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0, 0, int(w), int(h));

    QRectF rect(0, 0, w, h);

    // anti-highlight
    QLinearGradient ahGradient(0, 0, 0, 8);
    ahGradient.setColorAt(0.0, dark);
    ahGradient.setColorAt(1.0, alphaColor(shadow,0.8));
    p.setBrush(ahGradient);
    p.drawRect(rect);

    // anti-highlight mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    p.drawRoundRect(rect.adjusted(1, 1, -1, -1.5), rx, ry);

    // bevel
    QLinearGradient bevelGradient(0, 0, 0, 8);
    bevelGradient.setColorAt(0.0, alphaColor(highlight, 0.5));
    bevelGradient.setColorAt(0.7, alphaColor(highlight, 0.3));
    bevelGradient.setColorAt(1.0, alphaColor(highlight, 0));
    p.setBrush(bevelGradient);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    p.drawRect(rect.adjusted(1.5,0,-1.5,0));

    // mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    p.drawRoundRect(rect.adjusted(2, 2, -2, -2.5), rx, ry);

    p.end();
    return pixmap;
}

TileSet* OxygenScrollbar::vertical(int size, int width, int offset) const
{
    int s = size/2;
    int length = s*22;
    double w = 12.0 * double(width)/double(s*2);
    double o = -12.0 * double(offset) / double(size);
    const int h = 6*22;

    QPixmap pixmap(width, length);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0, 0, int(w), h);
    QRectF rect(0, 0, w, h);

    // mask; never draw outside this, hence mask() sets SourceAtop
    mask(p, rect);

    // base
    p.setBrush(baseGradient(w, Qt::Vertical));
    p.drawRect(rect);

    // shine
    p.setBrush(shineGradient(w, Qt::Vertical));
    p.drawRoundRect(QRectF(0, 0, int(w*0.45), h), 2000.0 / w, 12);
    p.setClipping(false);

    // shimmer
    p.setBrush(shimmerGradient(o, Qt::Vertical));
    p.drawRect(rect);

    // dim edges
    p.setBrush(dimGradient(Qt::Vertical));
    p.drawRect(rect);

    // highlight
    p.setBrush(alphaColor(highlight, 0.2));
    p.drawRoundRect(QRectF(w-3, 5.5, 1.5, h-12), 100, 5);
    p.drawRoundRect(QRectF(1.5, 5.5, 1.5, h-12), 100, 5);

    // bevel
    p.setWindow(0, 0, width, length);
    p.drawPixmap(0, 0, bevel(width, length, w, h, int(1400.0/w), 9));

    return new TileSet(pixmap, 1, s*3, width-2, s*16);
}

TileSet* OxygenScrollbar::horizontal(int size, int width, int offset) const
{
    int s = size/2;
    int length = s*22;
    double h = 12.0 * double(width)/double(s*2);
    double o = -12.0 * double(offset) / double(size);
    const int w = 6*22;

    QPixmap pixmap(length, width);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setWindow(0, 0, w, int(h));
    QRectF rect(0, 0, w, h);

    // mask; never draw outside this, hence mask() sets SourceAtop
    mask(p, rect);

    // base
    p.setBrush(baseGradient(h, Qt::Horizontal));
    p.drawRect(rect);

    // shine
    p.setBrush(shineGradient(h, Qt::Horizontal));
    p.drawRoundRect(QRectF(0, 0.5, w, int(h*0.45)), 12, 2000.0 / h);
    p.setClipping(false);

    // shimmer
    p.setBrush(shimmerGradient(o, Qt::Horizontal));
    p.drawRect(rect);

    // dim edges
    p.setBrush(dimGradient(Qt::Horizontal));
    p.drawRect(rect);

    // bevel
    p.setWindow(0, 0, length, width);
    p.drawPixmap(0, 0, bevel(length, width, w, h, 9, int(1400.0/h)));

    return new TileSet(pixmap, s*3, 1, s*16, width-2);
}
