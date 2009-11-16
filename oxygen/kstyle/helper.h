#ifndef oxygen_style_helper_h
#define oxygen_style_helper_h

/*
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
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

#include "lib/helper.h"

//! helper class
/*! contains utility functions used at multiple places in oxygen style */
class OxygenStyleHelper : public OxygenHelper
{
public:
    explicit OxygenStyleHelper(const QByteArray &componentName);
    virtual ~OxygenStyleHelper() {}

    virtual void invalidateCaches();

    //! overloaded
    virtual QPixmap windecoButton(const QColor &color, bool pressed, int size = 21);

    QColor calcMidColor(const QColor &color) const;

    static void fillSlab(QPainter&, const QRect&, int size = 7);
    static void fillHole(QPainter&, const QRect&, int size = 7);

    QPixmap dialSlab(const QColor&, qreal shade, int size = 7);
    QPixmap dialSlabFocused(const QColor&, const QColor&, qreal shade, int size = 7);
    QPixmap roundSlab(const QColor&, qreal shade, int size = 7);
    QPixmap roundSlabFocused(const QColor&, const QColor &glowColor, qreal shade, int size = 7);

    // progressbar
    QPixmap progressBarIndicator( const QPalette&, const QRect& );

    // TODO - need to rebase scrollbars to size=7
    TileSet *roundCorner(const QColor&, int size = 5);
    TileSet *slabFocused(const QColor&, const QColor &glowColor, qreal shade, int size = 7);
    TileSet *slabSunken(const QColor&, qreal shade, int size = 7);
    TileSet *slabInverted(const QColor&, qreal shade, int size = 7);

    TileSet *slope(const QColor&, qreal shade, int size = 7);

    TileSet *hole(const QColor&, qreal shade, int size = 7);
    TileSet *holeFlat(const QColor&, qreal shade, int size = 7);
    TileSet *holeFocused(const QColor&, const QColor &glowColor, qreal shade, int size = 7);

    TileSet *groove(const QColor&, qreal shade, int size = 7);

    TileSet *slitFocused(const QColor&);

    TileSet *dockFrame(const QColor&, int size);
    TileSet *scrollHole(const QColor&, Qt::Orientation orientation, bool smallShadow = false );

    QPalette mergePalettes( const QPalette&, qreal ) const;

    // these two methods must be public because they are used directly by OxygenStyle to draw dials
    void drawInverseShadow(QPainter&, const QColor&, int pad, int size, qreal fuzz) const;
    void drawInverseGlow(QPainter&, const QColor&, int pad, int size, int rsize) const;

    protected:

    void drawHole(QPainter&, const QColor&, qreal shade, int r = 7) const;
    void drawRoundSlab( QPainter&, const QColor&, qreal ) const;

    //! progressbar cache
    QCache<quint64, QPixmap> m_progressBarCache;

    QCache<quint64, TileSet> m_cornerCache;
    QCache<quint64, TileSet> m_slabSunkenCache;
    QCache<quint64, TileSet> m_slabInvertedCache;
    QCache<quint64, TileSet> m_holeCache;
    QCache<quint64, TileSet> m_holeFlatCache;
    QCache<quint64, TileSet> m_slopeCache;
    QCache<quint64, TileSet> m_grooveCache;
    QCache<quint64, TileSet> m_slitCache;
    QCache<quint64, TileSet> m_dockFrameCache;
    QCache<quint64, TileSet> m_scrollHoleCache;

};

#endif // __OXYGEN_STYLE_HELPER_H
