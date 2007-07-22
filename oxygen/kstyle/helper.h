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

#ifndef __OXYGEN_STYLE_HELPER_H
#define __OXYGEN_STYLE_HELPER_H

#include "lib/helper.h"
#include "tileset.h"

class OxygenStyleHelper : public OxygenHelper
{
public:
    explicit OxygenStyleHelper(const QByteArray &componentName);
    virtual ~OxygenStyleHelper() {}

    TileSet *hole(const QColor &color);
    TileSet *holeFocused(const QColor &color, QColor glowColor);
    TileSet *verticalScrollBar(const QColor &color, int width, int height, int offset);

protected:
    QCache<quint64, TileSet> m_setCache;
    QCache<quint64, TileSet> m_verticalScrollBarCache;
};

#endif // __OXYGEN_STYLE_HELPER_H
