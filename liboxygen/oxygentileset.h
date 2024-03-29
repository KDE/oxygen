#ifndef oxygentileset_h
#define oxygentileset_h

/*
 * SPDX-FileCopyrightText: 2009-2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#include "oxygen_export.h"

#include <QPixmap>
#include <QRect>
#include <QVector>

//* handles proper scaling of pixmap to match widget rect.
/**
tilesets are collections of stretchable pixmaps corresponding to a given widget corners, sides, and center.
corner pixmaps are never stretched. center pixmaps are
*/
namespace Oxygen
{
class OXYGEN_EXPORT TileSet final
{
public:
    //* default size for tileset tiles
    enum { DefaultSize = 7 };

    /**
    Create a TileSet from a pixmap. The size of the bottom/right chunks is
    whatever is left over from the other chunks, whose size is specified
    in the required parameters.

    @param w1 width of the left chunks
    @param h1 height of the top chunks
    @param w2 width of the not-left-or-right chunks
    @param h2 height of the not-top-or-bottom chunks
    */
    TileSet(const QPixmap &, int w1, int h1, int w2, int h2);

    /**
    Create a TileSet from a pixmap. The size of the top/left and bottom/right
    chunks is specified, with the middle chunks created from the specified
    portion of the pixmap. This allows the middle chunks to overlap the outer
    chunks (or to not use all pixels). The top/left and bottom/right chunks
    are carved out of the corners of the pixmap.

    @param w1 width of the left chunks
    @param h1 height of the top chunks
    @param w3 width of the right chunks
    @param h3 height of bottom chunks
    @param x2 x-coordinate of the top of the not-left-or-right chunks
    @param y2 y-coordinate of the left of the not-top-or-bottom chunks
    @param w2 width of the not-left-or-right chunks
    @param h2 height of the not-top-or-bottom chunks
    */
    TileSet(const QPixmap &pix, int w1, int h1, int w3, int h3, int x2, int y2, int w2, int h2);

    //* empty constructor
    TileSet();

    /**
    Flags specifying what sides to draw in ::render. Corners are drawn when
    the sides forming that corner are drawn, e.g. Top|Left draws the
    top-center, center-left, and top-left chunks. The center-center chunk is
    only drawn when Center is requested.
    */
    enum Tile {
        Top = 0x1,
        Left = 0x2,
        Bottom = 0x4,
        Right = 0x8,
        Center = 0x10,
        TopLeft = Top | Left,
        TopRight = Top | Right,
        BottomLeft = Bottom | Left,
        BottomRight = Bottom | Right,
        Ring = Top | Left | Bottom | Right,
        Horizontal = Left | Right | Center,
        Vertical = Top | Bottom | Center,
        Full = Ring | Center
    };
    Q_DECLARE_FLAGS(Tiles, Tile)

    /**
    Adjust rect to deal with missing tiles
    This will extend the relevant side so that the missing tiles extends beyond the
    rect passed as argument
    */

    QRect adjust(const QRect &, Tiles) const;

    /**
    Fills the specified rect with tiled chunks. Corners are never tiled,
    edges are tiled in one direction, and the center chunk is tiled in both
    directions. Partial tiles are used as needed so that the entire rect is
    perfectly filled. Filling is performed as if all chunks are being drawn.
    */
    void render(const QRect &, QPainter *, Tiles = Ring) const;

    //* return size associated to this tileset
    QSize size(void) const
    {
        return QSize(_w1 + _w3, _h1 + _h3);
    }

    //* is valid
    bool isValid(void) const
    {
        return _pixmaps.size() == 9;
    }

    //* side extend
    /**
    it is used to (pre) tile the side pixmaps, in order to make further tiling faster when rendering, at the cost of
    using more memory for the cache. Changes to this member only affects tilesets that are created afterwards.
    */
    void setSideExtent(int value)
    {
        _sideExtent = value;
    }

    //* returns pixmap for given index
    QPixmap pixmap(int index) const
    {
        return _pixmaps[index];
    }

private:
    //* shortcut to pixmap list
    using PixmapList = QVector<QPixmap>;

    //* initialize pixmap
    void initPixmap(PixmapList &, const QPixmap &, int w, int h, const QRect &);

    //* side extend
    /**
    it is used to (pre) tile the side pixmaps, in order to make further tiling faster when rendering, at the cost of
    using more memory for the cache.
    */
    static int _sideExtent;

    //* pixmap arry
    PixmapList _pixmaps;

    // dimensions
    int _w1 = 0;
    int _h1 = 0;
    int _w3 = 0;
    int _h3 = 0;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Oxygen::TileSet::Tiles)

#endif // TILESET_H
