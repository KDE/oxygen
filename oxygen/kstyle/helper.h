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

class SlabCache {
public:
    SlabCache() {}
    ~SlabCache() {}

    QCache<quint64, QPixmap> m_roundSlabCache;
    QCache<quint64, TileSet> m_slabCache;
};

class OxygenStyleHelper : public OxygenHelper
{
public:
    explicit OxygenStyleHelper(const QByteArray &componentName);
    virtual ~OxygenStyleHelper() {}

    QPixmap  roundSlab(const QColor&, double shade, int size = 6);

    TileSet *slab(const QColor&, double shade, int size = 6);
    TileSet *slabFocused(const QColor&, QColor glow, double shade, int size = 6);
    TileSet *slabSunken(const QColor&, double shade, int size = 6);

    TileSet *slope(const QColor&, double shade, int size = 6);

    TileSet *hole(const QColor&);
    TileSet *holeFocused(const QColor&, QColor glow);

    TileSet *slitFocused(const QColor&);

    TileSet *verticalScrollBar(const QColor&, int width, int offset);
    TileSet *horizontalScrollBar(const QColor&, int width, int offset);

protected:
    SlabCache* slabCache(const QColor&);

    QCache<quint64, SlabCache> m_slabCache;
    QCache<quint64, TileSet> m_slabSunkenCache;
    QCache<quint64, TileSet> m_holeCache;
    QCache<quint64, TileSet> m_slopeCache;
    QCache<quint64, TileSet> m_slitCache;
    QCache<quint64, TileSet> m_verticalScrollBarCache;
    QCache<quint64, TileSet> m_horizontalScrollBarCache;
};

#endif // __OXYGEN_STYLE_HELPER_H
