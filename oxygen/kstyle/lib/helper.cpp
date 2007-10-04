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

#include "helper.h"

#include <KGlobalSettings>
#include <KColorUtils>
#include <KColorScheme>

#include <QtGui/QPainter>

#include <math.h>

// alphaBlendColors Copyright 2003 Sandro Giessl <ceebx@users.sourceforge.net>
// DEPRECATED (use KColorUtils::mix to the extent we still need such a critter)
QColor alphaBlendColors(const QColor &bgColor, const QColor &fgColor, const int a)
{

    // normal button...
    QRgb rgb = bgColor.rgb();
    QRgb rgb_b = fgColor.rgb();
    int alpha = a;
    if(alpha>255) alpha = 255;
    if(alpha<0) alpha = 0;
    int inv_alpha = 255 - alpha;

    QColor result  = QColor( qRgb(qRed(rgb_b)*inv_alpha/255 + qRed(rgb)*alpha/255,
                                  qGreen(rgb_b)*inv_alpha/255 + qGreen(rgb)*alpha/255,
                                  qBlue(rgb_b)*inv_alpha/255 + qBlue(rgb)*alpha/255) );

    return result;
}

// NOTE: OxygenStyleHelper needs to use a KConfig from its own KComponentData
// Since the ctor order causes a SEGV if we try to pass in a KConfig here from
// a KComponentData constructed in the OxygenStyleHelper ctor, we'll just keep
// one here, even though the window decoration doesn't really need it.
OxygenHelper::OxygenHelper(const QByteArray &componentName)
    : _componentData(componentName, 0, KComponentData::SkipMainComponentRegistration)
{
    _config = _componentData.config();
    _contrast = KGlobalSettings::contrastF(_config);
    _bgcontrast = 0.3;// // shouldn't use contrast for this _contrast; // TODO get style setting

    m_backgroundCache.setMaxCost(64);
    m_windecoButtonCache.setMaxCost(64);
}

KSharedConfigPtr OxygenHelper::config() const
{
    return _config;
}

void OxygenHelper::reloadConfig()
{
    double old_contrast = _contrast;

    _config->reparseConfiguration();
    _contrast = KGlobalSettings::contrastF(_config);

    if (_contrast != old_contrast)
        invalidateCaches(); // contrast changed, invalidate our caches
}

void OxygenHelper::invalidateCaches()
{
    m_backgroundCache.clear();
    m_windecoButtonCache.clear();
}

bool OxygenHelper::lowThreshold(const QColor &color)
{
    QColor darker = KColorScheme::shade(color, KColorScheme::MidShade, 0.5);
    return KColorUtils::luma(darker) > KColorUtils::luma(color);
}

QColor OxygenHelper::alphaColor(QColor color, double alpha)
{
    color.setAlphaF(alpha);
    return color;
}

QColor OxygenHelper::backgroundRadialColor(const QColor &color) const
{
    if (lowThreshold(color))
        return KColorScheme::shade(color, KColorScheme::LightShade, 0.0);
    else
        return KColorScheme::shade(color, KColorScheme::LightShade, _bgcontrast);
}

QColor OxygenHelper::backgroundTopColor(const QColor &color) const
{
    if (lowThreshold(color))
        return KColorScheme::shade(color, KColorScheme::MidlightShade, 0.0);
    else
        return KColorScheme::shade(color, KColorScheme::MidlightShade, _bgcontrast);
}

QColor OxygenHelper::backgroundBottomColor(const QColor &color) const
{
    QColor midColor = KColorScheme::shade(color, KColorScheme::MidShade, 0.0);
    if (lowThreshold(color))
        return midColor;

    double by = KColorUtils::luma(color), my = KColorUtils::luma(midColor);
    return KColorUtils::shade(color, (my - by) * _bgcontrast);
}

QColor OxygenHelper::calcLightColor(const QColor &color) const
{
    return KColorScheme::shade(color, KColorScheme::LightShade, _contrast);
}

QColor OxygenHelper::calcDarkColor(const QColor &color) const
{
    if (lowThreshold(color))
        return KColorUtils::mix(calcLightColor(color), color, 0.2 + 0.8 * _contrast);
    else
        return KColorScheme::shade(color, KColorScheme::MidShade, _contrast);
}

QColor OxygenHelper::calcShadowColor(const QColor &color) const
{
    return KColorScheme::shade(color, KColorScheme::ShadowShade, _contrast);
}

QColor OxygenHelper::backgroundColor(const QColor &color, int height, int y)
{
    double h = height * 0.5;
    if (y > height>>1) {
        double a = double(y) / h;
        return KColorUtils::mix(backgroundTopColor(color), color, a);
    }
    else {
        double a = (double(y) - h) / h;
        return KColorUtils::mix(color, backgroundBottomColor(color), a);
    }
}

QPixmap OxygenHelper::verticalGradient(const QColor &color, int height)
{
    quint64 key = (quint64(color.rgba()) << 32) | height | 0x8000;
    QPixmap *pixmap = m_backgroundCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(32, height);

        QLinearGradient gradient(0, 0, 0, height);
        gradient.setColorAt(0.0, backgroundTopColor(color));
        gradient.setColorAt(0.5, color);
        gradient.setColorAt(1.0, backgroundBottomColor(color));

        QPainter p(pixmap);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(pixmap->rect(), gradient);

        m_backgroundCache.insert(key, pixmap);
    }

    return *pixmap;
}

QPixmap OxygenHelper::radialGradient(const QColor &color, int width)
{
    quint64 key = (quint64(color.rgba()) << 32) | width | 0xb000;
    QPixmap *pixmap = m_backgroundCache.object(key);

    if (!pixmap)
    {
        width /= 2;
        pixmap = new QPixmap(width, 64);
        pixmap->fill(QColor(0,0,0,0));
        QColor radialColor = backgroundRadialColor(color);
        radialColor.setAlpha(255);
        QRadialGradient gradient(64, 0, 64);
        gradient.setColorAt(0, radialColor);
        radialColor.setAlpha(101);
        gradient.setColorAt(0.5, radialColor);
        radialColor.setAlpha(37);
        gradient.setColorAt(0.75, radialColor);
        radialColor.setAlpha(0);
        gradient.setColorAt(1, radialColor);

        QPainter p(pixmap);
        p.scale(width/128.0,1);
        p.fillRect(pixmap->rect(), gradient);

        m_backgroundCache.insert(key, pixmap);
    }

    return *pixmap;
}

void OxygenHelper::drawShadow(QPainter &p, const QColor &color, int size) const
{
    double m = double(size-2)*0.5;

    const double offset = 0.8;
    double k0 = (m-4.0) / m;
    QRadialGradient shadowGradient(m+1.0, m+offset+1.0, m);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        double k1 = (k0 * double(8 - i) + double(i)) * 0.125;
        double a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a));
    }
    shadowGradient.setColorAt(1.0, alphaColor(color, 0.0));
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(0, 0, size, size));
}

QLinearGradient OxygenHelper::decoGradient(const QRect &r, const QColor &color)
{
    QColor light = KColorUtils::lighten(color, _contrast * 0.4);
    QColor dark = KColorUtils::darken(color, _contrast * 0.4);

    QLinearGradient gradient(r.topLeft(), r.bottomLeft());
    gradient.setColorAt(0.15, dark);
    gradient.setColorAt(0.50, color);
    gradient.setColorAt(0.85, light);

    return gradient;
}

QPixmap OxygenHelper::windecoButton(const QColor &color, int size)
{
    quint64 key = (quint64(color.rgba()) << 32) | size;
    QPixmap *pixmap = m_windecoButtonCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(size, (int)ceil(double(size)*10.0/9.0));
        pixmap->fill(QColor(0,0,0,0));

        QPainter p(pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,18,20);

        QColor light = calcLightColor(color);
        QColor dark = calcDarkColor(color);

        // shadow
        drawShadow(p, calcShadowColor(color), 18);

        // bevel
        qreal y = KColorUtils::luma(color);
        qreal yl = KColorUtils::luma(light);
        qreal yd = KColorUtils::luma(light);
        QLinearGradient bevelGradient(0, 0, 0, 18);
        bevelGradient.setColorAt(0.45, light);
        bevelGradient.setColorAt(0.80, dark);
        if (y < yl && y > yd) // no middle when color is very light/dark
            bevelGradient.setColorAt(0.55, color);
        p.setBrush(QBrush(bevelGradient));
        p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

        // inside mask
        QRadialGradient maskGradient(9,9,7,9,9);
        maskGradient.setColorAt(0.70, QColor(0,0,0,0));
        maskGradient.setColorAt(0.85, QColor(0,0,0,140));
        maskGradient.setColorAt(0.95, QColor(0,0,0,255));
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.setBrush(maskGradient);
        p.drawRect(0,0,20,20);

        // inside
        QLinearGradient innerGradient(0, 3, 0, 15);
        innerGradient.setColorAt(0.0, color);
        innerGradient.setColorAt(1.0, light);
        p.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        p.setBrush(innerGradient);
        p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

        // anti-shadow
        QRadialGradient highlightGradient(9,8.5,8,9,8.5);
        highlightGradient.setColorAt(0.85, alphaColor(light, 0.0));
        highlightGradient.setColorAt(1.00, light);
        p.setCompositionMode(QPainter::CompositionMode_SourceOver);
        p.setBrush(highlightGradient);
        p.drawEllipse(QRectF(2.0,2.0,14.0,14.0));

        m_windecoButtonCache.insert(key, pixmap);
    }

    return *pixmap;
}
