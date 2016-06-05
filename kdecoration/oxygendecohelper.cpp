/*
 * Copyright 2013 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright 2007 Casper Boemann <cbr@boemann.dk>
 * Copyright 2015  David Edmundson <davidedmundson@kde.org>
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

#include "oxygendecohelper.h"

#include <QPainter>
#include <KColorUtils>

#include <cmath>

namespace Oxygen
{

    //______________________________________________________________________________
    DecoHelper::DecoHelper():
        Helper(KSharedConfig::openConfig("oxygenrc"))
    {}

    //______________________________________________________________________________
    void DecoHelper::invalidateCaches( void )
    {
        // base class call
        Helper::invalidateCaches();

        // local caches
        _windecoButtonCache.clear();

    }

    //______________________________________________________________________________
    QPixmap DecoHelper::windecoButton(const QColor &color, const QColor& glow, bool sunken, int size)
    {

        Oxygen::Cache<QPixmap>::Value cache( _windecoButtonCache.get( color ) );

        const quint64 key( ( colorKey(glow) << 32 ) | (sunken << 23 ) | size );

        if( QPixmap *cachedPixmap = cache->object( key ) )
        { return *cachedPixmap; }

        QPixmap pixmap( size, size );
        pixmap.fill(Qt::transparent);

        QPainter p( &pixmap );
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow( 0, 0, 21, 21 );

        // button shadow
        if( color.isValid() )
        {
            p.save();
            p.translate( 0, -0.2 );
            drawShadow( p, calcShadowColor( color ), 21 );
            p.restore();
        }

        // button glow
        if( glow.isValid() )
        {
            p.save();
            p.translate( 0, -0.2 );
            drawOuterGlow( p, glow, 21 );
            p.restore();
        }

        // button slab
        p.translate( 0, 1 );
        p.setWindow( 0, 0, 18, 18 );
        if( color.isValid() )
        {
            p.translate( 0, (0.5-0.668) );

            const QColor light( calcLightColor(color) );
            const QColor dark( calcDarkColor(color) );

            {
                //plain background
                QLinearGradient lg( 0, 1.665, 0, (12.33+1.665) );
                if( sunken )
                {
                    lg.setColorAt( 1, light );
                    lg.setColorAt( 0, dark );
                } else {
                    lg.setColorAt( 0, light );
                    lg.setColorAt( 1, dark );
                }

                const QRectF r( 0.5*(18-12.33), 1.665, 12.33, 12.33 );
                p.setBrush( lg );
                p.drawEllipse( r );
            }

            {
                // outline circle
                const qreal penWidth( 0.7 );
                QLinearGradient lg( 0, 1.665, 0, (2.0*12.33+1.665) );
                lg.setColorAt( 0, light );
                lg.setColorAt( 1, dark );
                const QRectF r( 0.5*(18-12.33+penWidth), (1.665+penWidth), (12.33-penWidth), (12.33-penWidth) );
                p.setPen( QPen( lg, penWidth ) );
                p.setBrush( Qt::NoBrush );
                p.drawEllipse( r );
            }

        }

        p.end();
        cache->insert( key, new QPixmap(pixmap) );

        return pixmap;
    }

}
