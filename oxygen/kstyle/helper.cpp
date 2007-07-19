/*
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
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

#include <QtGui/QPainter>
#include <QtGui/QLinearGradient>

OxygenStyleHelper::OxygenStyleHelper(const QByteArray &componentName)
    : OxygenHelper(componentName)
{
}

TileSet *OxygenStyleHelper::hole(const QColor &surroundColor)
{
    quint64 key = (quint64(surroundColor.rgba()) << 32) | 0x1;
    TileSet *tileSet = m_setCache.object(key);

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
        p.drawRoundRect(QRectF(0,0, 9*0.8, 9),80,80);
        p.resetTransform();

        // draw white edge at bottom
        p.setClipRect(0,7,9,2);
        p.setBrush(Qt::NoBrush);
        p.setPen( KColorUtils::shade(surroundColor, 0.3));
        p.drawRoundRect(QRectF(0.5, 0.5, 8, 8),80,80);
        p.setPen(Qt::NoPen);
        p.end();

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 4, 4, 1, 1);

        m_setCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::holeFocused(const QColor &surroundColor, QColor glowColor)
{
    // FIXME need to figure out to what extent we need to care about glowColor
    // for the key as well, might be enough just to stash the color set in the
    // key since glow color should only change when the system scheme is
    // changed by the user
    quint64 key = (quint64(surroundColor.rgba()) << 32) | 0x2;
    TileSet *tileSet = m_setCache.object(key);

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
        p.drawRoundRect(QRectF(0,0, 9*0.8, 9),80,80);
        p.resetTransform();

        rg = QRadialGradient(4.5, 4.5, 5.0, 4.5, 4.5);
        stops.clear();
        glowColor.setAlpha(0);
        stops << QGradientStop(0, glowColor);
        glowColor.setAlpha(30);
        stops  << QGradientStop(0.40, glowColor);
        glowColor.setAlpha(110);
        stops  << QGradientStop(0.65, glowColor);
        glowColor.setAlpha(170);
        stops  << QGradientStop(0.75, glowColor);
        glowColor.setAlpha(0);
        stops  << QGradientStop(0.78, glowColor);
        rg.setStops(stops);
        p.setBrush(rg);
        p.setClipRect(0,0,9,9);
        p.drawRoundRect(QRectF(0,0, 9, 9),80,80);

        // draw white edge at bottom
        p.setClipRect(0,7,9,2);
        p.setBrush(Qt::NoBrush);
        p.setPen( KColorUtils::shade(surroundColor, 0.3));
        p.drawRoundRect(QRectF(0.5, 0.5, 8, 8),80,80);
        p.setPen(Qt::NoPen);
        p.end();

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 4, 4, 1, 1);

        m_setCache.insert(key, tileSet);
    }
    return tileSet;
}
