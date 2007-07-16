/* Oxygen widget style for KDE 4
   Copyright (C) 2006-2007 Thomas Luebking <thomas.luebking@web.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#include <QPainter>
#include <cmath>

#include "tileset.h"

static bool isEmpty(const QPixmap &pix)
{
   if (!pix.hasAlpha()) return false;
   QImage img =  pix.toImage();
   uint *data = ( uint * ) img.bits();
   int total = img.width() * img.height();
   for ( int current = 0 ; current < total ; ++current )
      if (qAlpha(data[ current ]))
         return false;
   return true;
}

TileSet::TileSet(const QPixmap &pix, int xOff, int yOff, int width, int height, int rx, int ry)
{
    if (pix.isNull())
    {
        _isBitmap = false;
        return;
    }
    _isBitmap = pix.isQBitmap();
    rxf = pix.width()*rx;
    ryf = pix.height()*ry;
    
    int rOff = pix.width() - xOff - width;
    int bOff = pix.height() - yOff - height;
    int amount = 32/width+1;
    int amount2 = 32/height+1;
    int i;
    
    QPainter p;
   
#define initPixmap(_SECTION_,_WIDTH_,_HEIGHT_)\
   pixmap[_SECTION_] = QPixmap(_WIDTH_, _HEIGHT_);\
   pixmap[_SECTION_].fill(Qt::transparent); p.begin(&pixmap[_SECTION_])
      
#define finishPixmap(_SECTION_)\
   p.end();\
   if (isEmpty(pixmap[_SECTION_]))\
      pixmap[_SECTION_] = QPixmap()

    initPixmap(TopLeft, xOff, yOff);
    p.drawPixmap(0, 0, pix, 0, 0, xOff, yOff);
    finishPixmap(TopLeft);

    initPixmap(TopMid, amount*width, yOff);
    for (i = 0; i < amount; i++)
         p.drawPixmap(i*width, 0, pix, xOff, 0, width, yOff);
    finishPixmap(TopMid);

    initPixmap(TopRight, rOff, yOff);
    p.drawPixmap(0, 0, pix, xOff+width, 0, rOff, yOff);
    finishPixmap(TopRight);

    //----------------------------------
    initPixmap(MidLeft, xOff, amount2*height);
    for (i = 0; i < amount2; i++)
         p.drawPixmap(0, i*height, pix, 0, yOff, xOff, height);
    finishPixmap(MidLeft);

    initPixmap(MidMid, amount*width, amount2*height);
    for (i = 0; i < amount; i++)
         for (int j = 0; j < amount2; j++)
              p.drawPixmap(i*width, j*height, pix, xOff, yOff, width, height);
    finishPixmap(MidMid);

    initPixmap(MidRight, rOff, amount2*height);
    for (i = 0; i < amount2; i++)
         p.drawPixmap(0, i*height, pix, xOff+width, yOff, rOff, height);
    finishPixmap(MidRight);

    //----------------------------------
    initPixmap(BtmLeft, xOff, bOff);
    p.drawPixmap(0, 0, pix, 0, yOff+height, xOff, bOff);
    finishPixmap(BtmLeft);

    initPixmap(BtmMid, amount*width, bOff);
    for (i = 0; i < amount; i++)
         p.drawPixmap(i*width, 0, pix, xOff, yOff+height, width, bOff);
    finishPixmap(BtmMid);

    initPixmap(BtmRight, rOff, bOff);
    p.drawPixmap(0, 0, pix, xOff+width, yOff+height, rOff, bOff);
    finishPixmap(BtmRight);
}

void TileSet::render(const QRect &r, QPainter *p, PosFlags pf) const
{
#define PIXMAP(_TILE_) pixmap[_TILE_]
#define DRAW_PIXMAP p->drawPixmap
#define DRAW_TILED_PIXMAP p->drawTiledPixmap

    int rOff = 0, xOff, yOff, w, h;
    
    r.getRect(&xOff, &yOff, &w, &h);
    int tlh = height(TopLeft), blh = height(BtmLeft),
        trh = height(TopRight), brh = height(BtmLeft),
        tlw = width(TopLeft), blw = width(BtmLeft),
        trw = width(TopRight), brw = width(BtmRight);
    
    if (pf & Left)
    {
        w -= width(TopLeft);
        xOff += width(TopLeft);
        if (pf & (Top | Bottom) && tlh + blh > r.height()) // vertical edge overlap
        {
            tlh = (tlh*r.height())/(tlh+blh);
            blh = r.height() - tlh;
        }
    }
    if (pf & Right)
    {
        w -= width(TopRight);
        if (matches(Top | Bottom, pf) && trh + brh > r.height()) // vertical edge overlap
        {
            trh = (trh*r.height())/(trh+brh);
            brh = r.height() - trh;
        }
    }
    
    if (pf & Top)
    {
        if (matches(Left | Right, pf) && w < 0) // horizontal edge overlap
        {
            tlw = tlw*r.width()/(tlw+trw);
            trw = r.width() - tlw;
        }
        rOff = r.right()-trw+1;
        yOff += tlh;
        h -= tlh;
        if (pf & Left) // upper left
            DRAW_PIXMAP(r.x(),r.y(),PIXMAP(TopLeft), 0, 0, tlw, tlh);
        if (pf & Right) // upper right
            DRAW_PIXMAP(rOff, r.y(), PIXMAP(TopRight), width(TopRight)-trw, 0,
                        trw, trh);
        
        // upper line
        if (w > 0 && !pixmap[TopMid].isNull())
            DRAW_TILED_PIXMAP(xOff, r.y(), w, tlh/*height(TopMid)*/, PIXMAP(TopMid));
    }
    if (pf & Bottom)
    {
        if (matches(Left | Right, pf) && w < 0) // horizontal edge overlap
        {
            blw = (blw*r.width())/(blw+brw);
            brw = r.width() - blw;
        }
        rOff = r.right()-brw+1;
        int bOff = r.bottom()-blh+1;
        h -= blh;
        if (pf & Left) // lower left
            DRAW_PIXMAP(r.x(), bOff, PIXMAP(BtmLeft), 0, height(BtmLeft)-blh,
                        blw, blh);
        if (pf & Right) // lower right
            DRAW_PIXMAP(rOff, bOff, PIXMAP(BtmRight), width(BtmRight)-brw,
                        height(BtmRight)-brh, brw, brh);
        
        // lower line
        if (w > 0 && !pixmap[BtmMid].isNull())
            DRAW_TILED_PIXMAP(xOff, bOff, w, height(BtmMid), PIXMAP(BtmMid));
    }
    
    if (h > 0)
    {
        if ((pf & Center) && (w > 0)) // center part
            DRAW_TILED_PIXMAP(xOff, yOff, w, h, pixmap[MidMid]);
        if (pf & Left && !pixmap[MidLeft].isNull()) // left line
            DRAW_TILED_PIXMAP(r.x(), yOff, width(MidLeft), h, PIXMAP(MidLeft));
        rOff = r.right()-width(MidRight)+1;
        if (pf & Right && !pixmap[MidRight].isNull()) // right line
            DRAW_TILED_PIXMAP(rOff, yOff, width(MidRight), h, PIXMAP(MidRight));
    }

#undef PIXMAP
#undef DRAW_PIXMAP
#undef DRAW_TILED_PIXMAP
}
