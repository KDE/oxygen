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
        p.drawRoundRect(QRectF(0,0, 9*0.8, 9),90,90);
        p.resetTransform();

        // draw white edge at bottom
        p.setClipRect(0,7,9,2);
        p.setBrush(Qt::NoBrush);
        p.setPen( KColorUtils::shade(surroundColor, 0.3));
        p.drawRoundRect(QRectF(0.5, 0.5, 8, 8),90,90);
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
        p.drawRoundRect(QRectF(0,0, 9*0.8, 9),90,90);
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
        p.drawRoundRect(QRectF(0,0, 9, 9),90,90);

        // draw white edge at bottom
        p.setClipRect(0,7,9,2);
        p.setBrush(Qt::NoBrush);
        p.setPen( KColorUtils::shade(surroundColor, 0.3));
        p.drawRoundRect(QRectF(0.5, 0.5, 8, 8),90,90);
        p.setPen(Qt::NoPen);
        p.end();

        tileSet = new TileSet(QPixmap::fromImage(tmpImg), 4, 4, 1, 1);

        m_setCache.insert(key, tileSet);
    }
    return tileSet;
}

TileSet *OxygenStyleHelper::verticalScrollBar(const QColor &color, const QRect &orgR)
{
    int width = orgR.width();
    int height = orgR.height();
    int offsetSize = int(1.5 * width);
    int offset = orgR.top() % (3*width);

    quint64 key = (quint64(color.rgba()) << 32) | (width<<20) | (height<<10) | offset;
    TileSet *tileSet = m_verticalScrollBarCache.object(key);
    if (!tileSet)
    {
        QPixmap tmpPixmap(width, height);
        tmpPixmap.fill(Qt::transparent);

        QPainter p(&tmpPixmap);
        QRect r(0, 0, width, height);
        p.setPen(Qt::NoPen);
        p.setRenderHint(QPainter::Antialiasing);

        QLinearGradient lg(QPointF(0, 0),QPointF(width*0.6, 0));
        lg.setColorAt(0, color.lighter(140));
        lg.setColorAt(1, color);
        p.setBrush(lg);
        p.drawRoundRect(r, 90*9/width,90*9/height);

        lg = QLinearGradient (QPointF(0, 0),QPointF(width, 0));
        QColor tmpColor(28,255,28);
        tmpColor.setAlpha(0);
        lg.setColorAt(0, tmpColor);
        tmpColor.setAlpha(128);
        lg.setColorAt(1, tmpColor);
        p.setBrush(lg);
        p.drawRoundRect(QRectF(0.48*width, 0, 0.52*width, height), int(90*9/(width*0.52)),90*9/height);
//Not ported below

        lg = QLinearGradient(QPointF(width/2, -offset),QPointF(width, -offset+offsetSize));
        lg.setSpread(QGradient::ReflectSpread);
        tmpColor = color.darker(130);
        tmpColor.setAlpha(110);
        lg.setColorAt(0.0, tmpColor);
        tmpColor.setAlpha(30);
        lg.setColorAt(0.6, tmpColor);
        tmpColor.setAlpha(0);
        lg.setColorAt(1.0, tmpColor);
        p.setBrush(lg);
        p.drawRoundRect(r, 90*9/width, 90*9/height);

        tileSet = new TileSet(tmpPixmap, 1, 1, width-2, height-2);

        m_verticalScrollBarCache.insert(key, tileSet);
    }
    return tileSet;
}
