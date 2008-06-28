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

#include "progressbar.h"

#include <KColorUtils>
#include <KColorScheme>
#include <QDebug>

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

#include <math.h>

inline QColor alphaColor(QColor color, double alpha)
{
    color.setAlphaF(alpha);
    return color;
}

OxygenProgressBar::OxygenProgressBar(const QColor &c, double contrast) : color(c),
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

void OxygenProgressBar::mask(QPainter &p, const QRectF &rect) const
{
    p.setBrush(alphaColor(mid,0.8)); // color does matter
    p.drawRoundedRect(rect,8,8);

    // never draw outside the mask
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
}

QLinearGradient OxygenProgressBar::baseGradient(double width, Qt::Orientation orient) const
{
    int x = width, y = width;
    if (orient == Qt::Horizontal)
        x = 0;
    else
        y = 0;

    QLinearGradient gradient(0, 0, x, y);
    gradient.setColorAt(0.0, mid);
    gradient.setColorAt(1.0, alphaColor(light,0.8));

    return gradient;
}

QLinearGradient OxygenProgressBar::shineGradient(double width, Qt::Orientation orient) const
{
    int x = width, y = width;
    if (orient == Qt::Horizontal)
        x = 0;
    else
        y = 0;

    QLinearGradient gradient(0, 0, x, y);
    gradient.setColorAt(0.0, light);
    gradient.setColorAt(0.45, alphaColor(light,0.5));
    gradient.setColorAt(0.5, Qt::transparent);

    return gradient;
}

TileSet* OxygenProgressBar::horizontal(int size, int width) const
{
    int s = size/2;
    int length = s*22;
    double h = 12.0 * double(width)/double(s*2);
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

    // shadow
    p.setBrush(alphaColor(dark,0.2));
    p.drawRoundedRect(rect,6,6);
    rect.adjust(0,0,0,-1);

    // base gradient
    p.setBrush(baseGradient(h, Qt::Horizontal));
    p.drawRoundedRect(rect,6,6);

    // shine gradient
    p.setBrush(shineGradient(h, Qt::Horizontal));
    p.drawRoundedRect(rect.adjusted(1,1,-1,-2),6,6);

    // dim layer
    p.fillRect(rect.adjusted(2,2,-2,-2),alphaColor(dark,0.3));

    // shine and border
    p.setBrush(Qt::NoBrush);
    p.setPen( QPen(dark, 1.2)  );
    p.drawRoundedRect(rect.adjusted(0,0,0,-1),6,6);

    p.setPen( QPen(alphaColor(light,0.4), 1.0) );
    p.drawRoundedRect(rect.adjusted(2,2,-2,-3),6,6);
    p.drawRoundedRect(rect.adjusted(2,2,-2,-2.5),6,6);


    return new TileSet(pixmap, 5, 2, pixmap.width()-10, pixmap.height()-5);
}


TileSet* OxygenProgressBar::vertical(int size, int width) const
{
    int s = size/2;
    int length = s*22;
    double w = 12.0 * double(width)/double(s*2);
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

    // shadow
    p.setBrush(alphaColor(dark,0.2));
    p.drawRoundedRect(rect,6,6);
    rect.adjust(0,0,0,-1);

    // base gradient
    p.setBrush(baseGradient(w, Qt::Vertical));
    p.drawRoundedRect(rect,6,6);

    // shine gradient
    p.setBrush(shineGradient(w, Qt::Vertical));
    p.drawRoundedRect(rect.adjusted(1,1,-1,-2),6,6);

    // dim layer
    p.fillRect(rect.adjusted(2,2,-2,-2),alphaColor(dark,0.3));

    // shine and border
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(dark,1));
    p.drawRoundedRect(rect.adjusted(0,0,0,-1),6,6);

    p.setPen(alphaColor(light,0.4));
    p.drawRoundedRect(rect.adjusted(2,2,-2,-2),6,6);
    p.drawRoundedRect(rect.adjusted(2,2,-2,-2.5),6,6);

    return new TileSet(pixmap, 1, s*3, width-2, s*16);
}
