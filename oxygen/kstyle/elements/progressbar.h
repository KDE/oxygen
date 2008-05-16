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

#ifndef __OXYGEN_STYLE_PROGRESSBAR_H
#define __OXYGEN_STYLE_PROGRESSBAR_H

#include "tileset.h"

class QPainter;
class QLinearGradient;

class OxygenProgressBar {
public:
    OxygenProgressBar(const QColor&, double contrast);

    TileSet* horizontal(int size, int width) const;
    TileSet* vertical(int size, int width) const;

private:
    void mask(QPainter &p, const QRectF &rect) const;

    QLinearGradient baseGradient(double width, Qt::Orientation orient) const;
    QLinearGradient shineGradient(double width, Qt::Orientation orient) const;

    QColor color;
    QColor light;
    QColor mid;
    QColor dark;
    QColor shadow;
    QColor highlight;
};

#endif // __OXYGEN_STYLE_SCROLLBAR_H
