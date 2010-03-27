#ifndef oxygen_helper_h
#define oxygen_helper_h

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

#include <ksharedconfig.h>
#include <kcomponentdata.h>

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QWidget>
#include <QtGui/QLinearGradient>
#include <QtCore/QCache>

#include "tileset.h"

namespace Oxygen
{

    template<typename T> class OXYGEN_EXPORT Cache
    {

        public:

        //! constructor
        Cache()
        {}

        //! destructor
        ~Cache()
        {}

        //! return cache matching a given key
        typedef QCache<quint64, T> Value;
        Value* get( const QColor& color )
        {
            quint64 key = (quint64(color.rgba()) << 32);
            Value *cache = data_.object(key);

            if (!cache)
            {
                cache = new Value();
                data_.insert(key, cache);
            }

            return cache;
        }

        //! clear
        void clear( void )
        { data_.clear(); }

        private:

        QCache<quint64, Value> data_;

    };

}

//! oxygen style helper class.
/*! contains utility functions used at multiple places in both oxygen style and oxygen window decoration */
class OXYGEN_EXPORT OxygenHelper
{
public:
    explicit OxygenHelper(const QByteArray &componentName);
    virtual ~OxygenHelper() {}

    KSharedConfigPtr config() const;
    void reloadConfig();

    //! render window background gradients
    /*!
    \par y_shift: shift the background gradient upwards, to fit with the windec
    \par gradientHeight: the height of the generated gradient.
    for different heights, the gradient is translated so that it is always at the same position from the bottom
    */
    void renderWindowBackground(QPainter *p, const QRect &clipRect, const QWidget *widget, const QPalette & pal, int y_shift=-23, int gradientHeight = 64)
    { renderWindowBackground( p, clipRect, widget, widget->window(), pal, y_shift, gradientHeight ); }

    // y_shift: shift the background gradient upwards, to fit with the windec
    // gradientHeight: the height of the generated gradient.
    // for different heights, the gradient is translated so that it is always at the same position from the bottom
    void renderWindowBackground(QPainter *p, const QRect &clipRect, const QWidget *widget, const QWidget* window, const QPalette & pal, int y_shift=-23, int gradientHeight = 64);

    //! reset all caches
    virtual void invalidateCaches();

    static bool lowThreshold(const QColor &color);

    static QColor alphaColor(QColor color, qreal alpha);

    virtual QColor calcLightColor(const QColor &color) const;
    virtual QColor calcDarkColor(const QColor &color) const;
    virtual QColor calcShadowColor(const QColor &color) const;

    //! returns menu background color matching position in a given top level widget
    virtual QColor backgroundColor(const QColor &color, const QWidget* w, const QPoint& point )
    {
        if( !( w && w->window() ) ) return color;
        else return backgroundColor( color, w->window()->height(), w->mapTo( w->window(), point ).y() );
    }

    //! returns menu background color matching position in a top level widget of given height
    virtual QColor backgroundColor(const QColor &color, int height, int y)
    { return cachedBackgroundColor( color, qMin(qreal(1.0), qreal(y)/qMin(300, 3*height/4) ) ); }

    virtual QColor backgroundRadialColor(const QColor &color) const;
    virtual QColor backgroundTopColor(const QColor &color) const;
    virtual QColor backgroundBottomColor(const QColor &color) const;

    virtual QPixmap verticalGradient(const QColor &color, int height, int offset = 0 );
    virtual QPixmap radialGradient(const QColor &color, int width, int height = 64);

    //! merge background and front color for check marks, arrows, etc. using _contrast
    virtual QColor decoColor(const QColor &background, const QColor &color) const;

    //! returns a region matching given rect, with rounded corners, based on the multipliers
    /*! setting any of the multipliers to zero will result in no corners shown on the corresponding side */
    virtual QRegion roundedRegion( const QRect&, int left = 1, int right = 1, int top = 1, int bottom = 1 ) const;

    //! returns a region matching given rect, with rounded corners, based on the multipliers
    /*! setting any of the multipliers to zero will result in no corners shown on the corresponding side */
    virtual QRegion roundedMask( const QRect&, int left = 1, int right = 1, int top = 1, int bottom = 1 ) const;

    //! draw frame that mimics some sort of shadows around a panel
    /*! it is used for menus, detached dock panels and toolbar, as well as window decoration when compositing is disabled */
    virtual void drawFloatFrame(
      QPainter *p, const QRect r, const QColor &color,
      bool drawUglyShadow=true, bool isActive=false,
      const QColor &frameColor=QColor(),
      TileSet::Tiles tiles = TileSet::Ring
      ) const;

    virtual void drawSeparator(QPainter *p, const QRect &r, const QColor &color, Qt::Orientation orientation) const;

    virtual TileSet *slab(const QColor&, qreal shade, int size = 7);

    protected:

    virtual void drawSlab(QPainter&, const QColor&, qreal shade) const;
    virtual void drawShadow(QPainter&, const QColor&, int size) const;
    virtual void drawOuterGlow(QPainter&, const QColor&, int size) const;

    //! return background adjusted color matching relative vertical position in window
    QColor cachedBackgroundColor( const QColor&, qreal ratio );

    static const qreal _glowBias;
    static const qreal _slabThickness;
    static const qreal _shadowGain;

    KComponentData _componentData;
    KSharedConfigPtr _config;
    qreal _contrast;
    qreal _bgcontrast;

    Oxygen::Cache<TileSet> m_slabCache;

    QCache<quint64, QColor> m_backgroundColorCache;
    QCache<quint64, QPixmap> m_backgroundCache;
    QCache<quint64, QPixmap> m_windecoButtonCache;
    QCache<quint64, QPixmap> m_windecoButtonGlowCache;
};

#endif
