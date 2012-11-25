#ifndef oxygentileset_h
#define oxygentileset_h

/*
 * Copyright 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
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

#include "oxygen_export.h"

#include <QtGui/QPixmap>
#include <QtCore/QRect>
#include <QtCore/QVector>

//! handles proper scaling of pixmap to match widget rect.
/*!
tilesets are collections of stretchable pixmaps corresponding to a given widget corners, sides, and center.
corner pixmaps are never stretched. center pixmaps are
*/
namespace Oxygen
{
    class OXYGEN_EXPORT TileSet
    {
        public:
        /*!
        Create a TileSet from a pixmap. The size of the bottom/right chunks is
        whatever is left over from the other chunks, whose size is specified
        in the required parameters.

        @param w1 width of the left chunks
        @param h1 height of the top chunks
        @param w2 width of the not-left-or-right chunks
        @param h2 height of the not-top-or-bottom chunks
        */
        TileSet(const QPixmap&, int w1, int h1, int w2, int h2, bool stretch = false );

        /*!
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
        TileSet(const QPixmap &pix, int w1, int h1, int w3, int h3, int x2, int y2, int w2, int h2, bool stretch = false );

        //! empty constructor
        TileSet();

        //! destructor
        virtual ~TileSet()
        {}

        /*!
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
            TopLeft = Top|Left,
            TopRight = Top|Right,
            BottomLeft = Bottom|Left,
            BottomRight = Bottom|Right,
            Ring = Top|Left|Bottom|Right,
            Horizontal = Left|Right|Center,
            Vertical = Top|Bottom|Center,
            Full = Ring|Center
        };
        Q_DECLARE_FLAGS(Tiles, Tile)

        /*!
        Fills the specified rect with tiled chunks. Corners are never tiled,
        edges are tiled in one direction, and the center chunk is tiled in both
        directions. Partial tiles are used as needed so that the entire rect is
        perfectly filled. Filling is performed as if all chunks are being drawn.
        */
        void render(const QRect&, QPainter*, Tiles = Ring) const;

        //! return size associated to this tileset
        QSize size( void ) const
        { return QSize( _w1 + _w3, _h1 + _h3 ); }

        //! is valid
        bool isValid( void ) const
        { return size().isValid() && _pixmaps.size() == 9; }

        //! save all pixmaps
        /*! pixmap names will be \p basename-position.suffix. Other arguments are the same as for QPixmap::save */
        void save( const QString& basename, const QString& suffix = "png", const char* format = 0, int quality = -1 ) const;

        //! side extend
        /*!
        it is used to (pre) tile the side pixmaps, in order to make further tiling faster when rendering, at the cost of
        using more memory for the cache. Changes to this member only affects tilesets that are created afterwards.
        */
        void setSideExtent( int value )
        { _sideExtent = value; }

        //! returns pixmap for given index
        QPixmap pixmap( int index ) const
        { return _pixmaps[index]; }

        protected:

        //! shortcut to pixmap list
        typedef QVector<QPixmap> PixmapList;

        //! initialize pixmap
        void initPixmap( PixmapList&, const QPixmap&, int w, int h, const QRect& );

        private:

        //! side extend
        /*!
        it is used to (pre) tile the side pixmaps, in order to make further tiling faster when rendering, at the cost of
        using more memory for the cache.
        */
        static int _sideExtent;

        //! pixmap arry
        PixmapList _pixmaps;

        // stretch pixmaps
        bool _stretch;

        // dimensions
        int _w1;
        int _h1;
        int _w3;
        int _h3;

    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Oxygen::TileSet::Tiles)

#endif //TILESET_H
