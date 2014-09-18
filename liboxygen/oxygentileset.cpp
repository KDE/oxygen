/*
 * Copyright 2009-2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygentileset.h"

#include <QPainter>
#include <QTextStream>

namespace Oxygen
{

    //______________________________________________________________
    int TileSet::_sideExtent = 32;

    //______________________________________________________________
    void TileSet::initPixmap( PixmapList& pixmaps, const QPixmap &source, int w, int h, const QRect &rect)
    {
        QSize size( w, h );
        if( !( size.isValid() && rect.isValid() ) )
        {
            pixmaps.append( QPixmap() );

        } else if( size != rect.size() ) {

            const qreal dpiRatio( source.devicePixelRatio() );
            const QRect scaledRect( rect.topLeft()*dpiRatio, rect.size()*dpiRatio );
            const QSize scaledSize( size*dpiRatio );
            const QPixmap tile( source.copy(scaledRect) );
            QPixmap pixmap( scaledSize );

            pixmap.fill(Qt::transparent);
            QPainter p(&pixmap);
            p.drawTiledPixmap(0, 0, scaledSize.width(), scaledSize.height(), tile);
            pixmap.setDevicePixelRatio( dpiRatio );
            pixmaps.append( pixmap );

        } else {

            const qreal dpiRatio( source.devicePixelRatio() );
            const QRect scaledRect( rect.topLeft()*dpiRatio, rect.size()*dpiRatio );
            QPixmap pixmap( source.copy( scaledRect ) );
            pixmap.setDevicePixelRatio( dpiRatio );
            pixmaps.append( pixmap );

        }

    }

    //______________________________________________________________
    TileSet::TileSet( void ):
        _w1(0),
        _h1(0),
        _w3(0),
        _h3(0)
    { _pixmaps.reserve(9); }

    //______________________________________________________________
    TileSet::TileSet(const QPixmap &source, int w1, int h1, int w2, int h2 ):
        _w1(w1),
        _h1(h1),
        _w3(0),
        _h3(0)
    {
        _pixmaps.reserve(9);
        if( source.isNull() ) return;

        _w3 = source.width()/source.devicePixelRatio() - (w1 + w2);
        _h3 = source.height()/source.devicePixelRatio() - (h1 + h2);
        int w = w2;
        int h = h2;

        // initialise pixmap array
        initPixmap( _pixmaps, source, _w1, _h1, QRect(0, 0, _w1, _h1) );
        initPixmap( _pixmaps, source, w, _h1, QRect(_w1, 0, w2, _h1) );
        initPixmap( _pixmaps, source, _w3, _h1, QRect(_w1+w2, 0, _w3, _h1) );
        initPixmap( _pixmaps, source, _w1, h, QRect(0, _h1, _w1, h2) );
        initPixmap( _pixmaps, source, w, h, QRect(_w1, _h1, w2, h2) );
        initPixmap( _pixmaps, source, _w3, h, QRect(_w1+w2, _h1, _w3, h2) );
        initPixmap( _pixmaps, source, _w1, _h3, QRect(0, _h1+h2, _w1, _h3) );
        initPixmap( _pixmaps, source, w, _h3, QRect(_w1, _h1+h2, w2, _h3) );
        initPixmap( _pixmaps, source, _w3, _h3, QRect(_w1+w2, _h1+h2, _w3, _h3) );
    }

    //______________________________________________________________
    TileSet::TileSet(const QPixmap &source, int w1, int h1, int w3, int h3, int x1, int y1, int w2, int h2 ):
        _w1(w1),
        _h1(h1),
        _w3(w3),
        _h3(h3)
    {
        _pixmaps.reserve(9);
        if( source.isNull() ) return;

        int x2 = source.width()/source.devicePixelRatio() - _w3;
        int y2 = source.height()/source.devicePixelRatio() - _h3;
        int w = w2;
        int h = h2;

        // initialise pixmap array
        initPixmap( _pixmaps, source, _w1, _h1, QRect(0, 0, _w1, _h1) );
        initPixmap( _pixmaps, source, w, _h1, QRect(x1, 0, w2, _h1) );
        initPixmap( _pixmaps, source, _w3, _h1, QRect(x2, 0, _w3, _h1) );
        initPixmap( _pixmaps, source, _w1, h, QRect(0, y1, _w1, h2) );
        initPixmap( _pixmaps, source, w, h, QRect(x1, y1, w2, h2) );
        initPixmap( _pixmaps, source, _w3, h, QRect(x2, y1, _w3, h2) );
        initPixmap( _pixmaps, source, _w1, _h3, QRect(0, y2, _w1, _h3) );
        initPixmap( _pixmaps, source, w, _h3, QRect(x1, y2, w2, _h3) );
        initPixmap( _pixmaps, source, _w3, _h3, QRect(x2, y2, _w3, _h3) );

    }

    //___________________________________________________________
    inline bool bits(TileSet::Tiles flags, TileSet::Tiles testFlags)
    { return (flags & testFlags) == testFlags; }

    //___________________________________________________________
    QRect TileSet::adjust(const QRect &constRect, Tiles tiles ) const
    {

        // adjust rect to deal with missing edges
        QRect rect( constRect );
        if( !(tiles&Left) ) rect.adjust( -_w1, 0, 0, 0 );
        if( !(tiles&Right) ) rect.adjust( 0, 0, _w3, 0 );
        if( !(tiles&Top) ) rect.adjust( 0, -_h1, 0, 0 );
        if( !(tiles&Bottom) ) rect.adjust( 0, 0, 0, _h3 );

        return rect;

    }

    //___________________________________________________________
    void TileSet::render(const QRect &constRect, QPainter *painter, Tiles tiles) const
    {

        const bool oldHint( painter->testRenderHint( QPainter::SmoothPixmapTransform ) );
        painter->setRenderHint( QPainter::SmoothPixmapTransform, true );

        // check initialization
        if( _pixmaps.size() < 9 ) return;

        // copy source rect
        QRect rect( constRect );

        // get rect dimensions
        int x0, y0, w, h;
        rect.getRect(&x0, &y0, &w, &h);

        // calculate pixmaps widths
        int wLeft(0);
        int wRight(0);
        if( _w1+_w3 > 0 )
        {
            qreal wRatio( qreal( _w1 )/qreal( _w1 + _w3 ) );
            wLeft = (tiles&Right) ? qMin( _w1, int(w*wRatio) ):_w1;
            wRight = (tiles&Left) ? qMin( _w3, int(w*(1.0-wRatio)) ):_w3;
        }

        // calculate pixmap heights
        int hTop(0);
        int hBottom(0);
        if( _h1+_h3 > 0 )
        {
            qreal hRatio( qreal( _h1 )/qreal( _h1 + _h3 ) );
            hTop = (tiles&Bottom) ? qMin( _h1, int(h*hRatio) ):_h1;
            hBottom = (tiles&Top) ? qMin( _h3, int(h*(1.0-hRatio)) ):_h3;
        }

        // calculate corner locations
        w -= wLeft + wRight;
        h -= hTop + hBottom;
        const int x1 = x0 + wLeft;
        const int x2 = x1 + w;
        const int y1 = y0 + hTop;
        const int y2 = y1 + h;

        const int w2 = _pixmaps.at(7).width()/_pixmaps.at(7).devicePixelRatio();
        const int h2 = _pixmaps.at(5).height()/_pixmaps.at(5).devicePixelRatio();

        // corner
        if( bits( tiles, Top|Left) )  painter->drawPixmap(x0, y0, _pixmaps.at(0), 0, 0, wLeft*_pixmaps.at(0).devicePixelRatio(), hTop*_pixmaps.at(0).devicePixelRatio());
        if( bits( tiles, Top|Right) ) painter->drawPixmap(x2, y0, _pixmaps.at(2), _w3-wRight, 0, wRight*_pixmaps.at(2).devicePixelRatio(), hTop*_pixmaps.at(2).devicePixelRatio());
        if( bits( tiles, Bottom|Left) )  painter->drawPixmap(x0, y2, _pixmaps.at(6), 0, _h3-hBottom, wLeft*_pixmaps.at(6).devicePixelRatio(),  hBottom*_pixmaps.at(6).devicePixelRatio());
        if( bits( tiles, Bottom|Right) ) painter->drawPixmap(x2, y2, _pixmaps.at(8), _w3-wRight, _h3-hBottom, wRight*_pixmaps.at(8).devicePixelRatio(), hBottom*_pixmaps.at(8).devicePixelRatio() );

        // top and bottom
        if( w > 0 )
        {
            if( tiles&Top ) painter->drawPixmap(x1, y0, w, hTop, _pixmaps.at(1) );
            if( tiles&Bottom ) painter->drawPixmap(x1, y2, w, hBottom, _pixmaps.at(7), 0, (_h3-hBottom)*_pixmaps.at(7).devicePixelRatio(), w2*_pixmaps.at(7).devicePixelRatio(), hBottom*_pixmaps.at(7).devicePixelRatio() );
        }

        // left and right
        if( h > 0 )
        {
            if( tiles&Left ) painter->drawPixmap(x0, y1, wLeft, h, _pixmaps.at(3));
            if( tiles&Right ) painter->drawPixmap(x2, y1, wRight, h, _pixmaps.at(5), (_w3-wRight)*_pixmaps.at(5).devicePixelRatio(), 0, wRight*_pixmaps.at(5).devicePixelRatio(), h2*_pixmaps.at(5).devicePixelRatio() );
        }

        // center
        if( (tiles&Center) && h > 0 && w > 0 ) painter->drawPixmap(x1, y1, w, h, _pixmaps.at(4));

        // restore
        painter->setRenderHint( QPainter::SmoothPixmapTransform, oldHint );

    }

}
