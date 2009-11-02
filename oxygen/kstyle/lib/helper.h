/*
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright 2007 Casper Boemann <cbr@boemann.dk>
 * Copyright 2007 Fredrik Höglund <fredrik@kde.org>
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

#ifndef OXYGEN_HELPER_H
#define OXYGEN_HELPER_H

#include <ksharedconfig.h>
#include <kcomponentdata.h>

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtGui/QLinearGradient>
#include <QtCore/QCache>

#include "tileset.h"

#define _glowBias 0.9 // not likely to be configurable


class SlabCache {
public:
    SlabCache() {}
    ~SlabCache() {}

    QCache<quint64, QPixmap> m_roundSlabCache;
    QCache<quint64, TileSet> m_slabCache;
    QCache<quint64, TileSet> m_shadowCache;
    QCache<quint64, TileSet> m_outerGlowCache;
};

// WARNING - OxygenHelper must be a K_GLOBAL_STATIC!
class OxygenHelper
{
public:
    explicit OxygenHelper(const QByteArray &componentName);
    virtual ~OxygenHelper() {}

    KSharedConfigPtr config() const;
    void reloadConfig();

    // y_shift: shift the background gradient upwards, to fit with the windec
    // gradientHeight: the height of the generated gradient.
    // for different heights, the gradient is translated so that it is always at the same position from the bottom
    void renderWindowBackground(QPainter *p, const QRect &clipRect, const QWidget *widget, const QPalette & pal, int y_shift=-23, int gradientHeight = 64)
    { renderWindowBackground( p, clipRect, widget, widget->window(), pal, y_shift, gradientHeight ); }

    // y_shift: shift the background gradient upwards, to fit with the windec
    // gradientHeight: the height of the generated gradient.
    // for different heights, the gradient is translated so that it is always at the same position from the bottom
    void renderWindowBackground(QPainter *p, const QRect &clipRect, const QWidget *widget, const QWidget* window, const QPalette & pal, int y_shift=-23, int gradientHeight = 64);

    virtual void invalidateCaches();

    static bool lowThreshold(const QColor &color);

    static QColor alphaColor(QColor color, qreal alpha);

    virtual QColor calcLightColor(const QColor &color) const;
    virtual QColor calcDarkColor(const QColor &color) const;
    virtual QColor calcShadowColor(const QColor &color) const;

    virtual QColor backgroundColor(const QColor &color, int height, int y);

    virtual QColor backgroundRadialColor(const QColor &color) const;
    virtual QColor backgroundTopColor(const QColor &color) const;
    virtual QColor backgroundBottomColor(const QColor &color) const;

    virtual QPixmap verticalGradient(const QColor &color, int height);
    virtual QPixmap radialGradient(const QColor &color, int width, int height = 64);

    //! merge background and front color for check marks, arrows, etc. using _contrast
    virtual QColor decoColor(const QColor &background, const QColor &color) const;

    virtual QPixmap windecoButton(const QColor &color, bool pressed, int size = 21);
    virtual QPixmap windecoButtonGlow(const QColor &color, int size = 21);

    //! returns a region matching given rect, with rounded corners, based on the multipliers
    /*! setting any of the multipliers to zero will result in no corners shown on the corresponding side */
    virtual QRegion roundedRegion( const QRect&, int left = 1, int right = 1, int top = 1, int bottom = 1 ) const;

    //! returns a region matching given rect, with rounded corners, based on the multipliers
    /*! setting any of the multipliers to zero will result in no corners shown on the corresponding side */
    virtual QRegion roundedMask( const QRect&, int left = 1, int right = 1, int top = 1, int bottom = 1 ) const;

    virtual void drawFloatFrame(
      QPainter *p, const QRect r, const QColor &color,
      bool drawUglyShadow=true, bool isActive=false,
      const QColor &frameColor=QColor(),
      TileSet::Tiles tiles = TileSet::Ring
      ) const;

    virtual void drawSeparator(QPainter *p, const QRect &r, const QColor &color, Qt::Orientation orientation) const;

    virtual TileSet *slab(const QColor&, qreal shade, int size = 7);
    virtual TileSet *shadow(const QColor&, int size = 7);
    virtual TileSet *outerGlow(const QColor&, int size = 7);

    protected:
    virtual void drawSlab(QPainter&, const QColor&, qreal shade) const;
    virtual void drawShadow(QPainter&, const QColor&, int size) const;
    virtual void drawOuterGlow(QPainter&, const QColor&, int size) const;

    virtual SlabCache* slabCache(const QColor&);

    static const qreal _slabThickness;
    static const qreal _shadowGain;

    KComponentData _componentData;
    KSharedConfigPtr _config;
    qreal _contrast;
    qreal _bgcontrast;

    QCache<quint64, SlabCache> m_slabCache;

    QCache<quint64, QPixmap> m_backgroundCache;
    QCache<quint64, QPixmap> m_windecoButtonCache;
    QCache<quint64, QPixmap> m_windecoButtonGlowCache;
};

#endif // __OXYGEN_HELPER_H
