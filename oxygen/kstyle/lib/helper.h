/*
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

#ifndef __OXYGEN_HELPER_H
#define __OXYGEN_HELPER_H

#include <ksharedconfig.h>
#include <kcomponentdata.h>

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QLinearGradient>
#include <QtCore/QCache>

// alphaBlendColors Copyright 2003 Sandro Giessl <ceebx@users.sourceforge.net>
// DEPRECATED
QColor alphaBlendColors(const QColor &backgroundColor, const QColor &foregroundColor, const int alpha);

// WARNING - OxygenHelper must be a K_GLOBAL_STATIC!
class OxygenHelper
{
public:
    explicit OxygenHelper(const QByteArray &componentName);
    virtual ~OxygenHelper() {}

    KSharedConfigPtr config() const;

    static bool lowThreshold(const QColor &color);
    QColor alphaColor(QColor color, double alpha);

    QColor calcLightColor(const QColor &color);
    QColor calcDarkColor(const QColor &color);
    QColor calcShadowColor(const QColor &color);

    QColor backgroundColor(const QColor &color, int height, int y);

    QColor backgroundRadialColor(const QColor &color) const;
    QColor backgroundTopColor(const QColor &color) const;
    QColor backgroundBottomColor(const QColor &color) const;

    QPixmap verticalGradient(const QColor &color, int height);
    QPixmap radialGradient(const QColor &color, int width);

    void drawShadow(QPainter &p, const QColor &color, int size);

    QLinearGradient decoGradient(const QRect &r, const QColor &color);

    QPixmap windecoButton(const QColor &color, int size);

protected:
    KComponentData _componentData;
    KSharedConfigPtr _config;
    qreal _contrast;
    qreal _bgcontrast;

    QCache<quint64, QPixmap> m_backgroundCache;
    QCache<quint64, QPixmap> m_windecoButtonCache;
};

#endif // __OXYGEN_HELPER_H
