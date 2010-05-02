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

#include "oxygenhelper.h"

#include <KColorUtils>
#include <KColorScheme>
#include <KDebug>
#include <KGlobalSettings>

#include <QtGui/QWidget>
#include <QtGui/QPainter>

#include <math.h>

const qreal OxygenHelper::_slabThickness = 0.45;
const qreal OxygenHelper::_shadowGain = 1.5;
const qreal OxygenHelper::_glowBias = 0.6;

// NOTE: OxygenStyleHelper needs to use a KConfig from its own KComponentData
// Since the ctor order causes a SEGV if we try to pass in a KConfig here from
// a KComponentData constructed in the OxygenStyleHelper ctor, we'll just keep
// one here, even though the window decoration doesn't really need it.
OxygenHelper::OxygenHelper(const QByteArray &componentName):
    _componentData(componentName, 0, KComponentData::SkipMainComponentRegistration)
{
    _config = _componentData.config();
    _contrast = KGlobalSettings::contrastF(_config);

    // background contrast is calculated so that it is 0.9
    // when KGlobalSettings contrast value of 0.7
    _bgcontrast = qMin(1.0, 0.9*_contrast/0.7);

    m_backgroundCache.setMaxCost(64);
    m_windecoButtonCache.setMaxCost(64);
    m_windecoButtonGlowCache.setMaxCost(64);
}

//____________________________________________________________________
KSharedConfigPtr OxygenHelper::config() const
{ return _config; }

//____________________________________________________________________
void OxygenHelper::reloadConfig()
{

    qreal old_contrast = _contrast;

    _config->reparseConfiguration();
    _contrast = KGlobalSettings::contrastF(_config);
    _bgcontrast = qMin(1.0, 0.9*_contrast/0.7);

    // contrast changed, invalidate our caches
    if( _contrast != old_contrast ) invalidateCaches();

}

//____________________________________________________________________
void OxygenHelper::renderWindowBackground(QPainter *p, const QRect &clipRect, const QWidget *widget, const QWidget* window, const QPalette & pal, int y_shift, int gradientHeight)
{

    // get coordinates relative to the client area
    // this is stupid. One could use mapTo if this was taking const QWidget* and not
    // const QWidget* as argument.
    const QWidget* w = widget;
    int x = 0, y = -y_shift;
    while ( w != window && !w->isWindow() && w != w->parentWidget() ) {
        x += w->geometry().x();
        y += w->geometry().y();
        w = w->parentWidget();
    }

    if (clipRect.isValid()) {
        p->save();
        p->setClipRegion(clipRect,Qt::IntersectClip);
    }

    // calculate upper part height
    // special tricks are needed
    // to handle both window contents and window decoration
    QRect r = window->rect();
    QColor color = pal.color(window->backgroundRole());
    int height = window->frameGeometry().height();
    int width = window->frameGeometry().width();
    if( y_shift > 0 )
    {
      height -= 2*y_shift;
      width -= 2*y_shift;
    }

    int splitY = qMin(300, (3*height)/4);

    // draw upper linear gradient
    QRect upperRect = QRect(-x, -y, r.width(), splitY);
    QPixmap tile = verticalGradient(color, splitY, gradientHeight-64);
    p->drawTiledPixmap(upperRect, tile);

    // draw lower flat part
    QRect lowerRect = QRect(-x, splitY-y, r.width(), r.height() - splitY-y_shift);
    p->fillRect(lowerRect, backgroundBottomColor(color));

    // draw upper radial gradient
    int radialW = qMin(600, width);
    QRect radialRect = QRect((r.width() - radialW) / 2-x, -y, radialW, gradientHeight);
    if (clipRect.intersects(radialRect))
    {
        tile = radialGradient(color, radialW, gradientHeight);
        p->drawPixmap(radialRect, tile);
    }

    if (clipRect.isValid())
    { p->restore(); }
}

//____________________________________________________________________
void OxygenHelper::invalidateCaches()
{
    m_slabCache.clear();
    m_backgroundColorCache.clear();
    m_backgroundCache.clear();
    m_windecoButtonCache.clear();
    m_windecoButtonGlowCache.clear();
}

//____________________________________________________________________
bool OxygenHelper::lowThreshold(const QColor &color)
{
    QColor darker = KColorScheme::shade(color, KColorScheme::MidShade, 0.5);
    return KColorUtils::luma(darker) > KColorUtils::luma(color);
}

//____________________________________________________________________
QColor OxygenHelper::alphaColor(QColor color, qreal alpha)
{
    if( alpha >= 0 && alpha < 1.0 )
    { color.setAlphaF(qMax(qreal(0.0), alpha) * color.alphaF()); }
    return color;
}

//____________________________________________________________________
QColor OxygenHelper::backgroundRadialColor(const QColor &color) const
{
    return (lowThreshold(color)) ?
      KColorScheme::shade(color, KColorScheme::LightShade, 0.0):
      KColorScheme::shade(color, KColorScheme::LightShade, _bgcontrast);
}

//_________________________________________________________________________
QColor OxygenHelper::backgroundTopColor(const QColor &color) const
{

    if( lowThreshold(color) ) return KColorScheme::shade(color, KColorScheme::MidlightShade, 0.0);
    qreal my = KColorUtils::luma( KColorScheme::shade(color, KColorScheme::LightShade, 0.0) );
    qreal by = KColorUtils::luma(color);
    return KColorUtils::shade(color, (my - by) * _bgcontrast);

}

//_________________________________________________________________________
QColor OxygenHelper::backgroundBottomColor(const QColor &color) const
{
    QColor midColor = KColorScheme::shade(color, KColorScheme::MidShade, 0.0);
    if( lowThreshold(color) ) return midColor;

    qreal by = KColorUtils::luma(color);
    qreal my = KColorUtils::luma(midColor);
    return KColorUtils::shade(color, (my - by) * _bgcontrast);

}

//____________________________________________________________________
QColor OxygenHelper::calcLightColor(const QColor &color) const
{ return KColorScheme::shade(color, KColorScheme::LightShade, _contrast); }

//____________________________________________________________________
QColor OxygenHelper::calcDarkColor(const QColor &color) const
{
    return (lowThreshold(color)) ?
        KColorUtils::mix(calcLightColor(color), color, 0.3 + 0.7 * _contrast):
        KColorScheme::shade(color, KColorScheme::MidShade, _contrast);
}

//____________________________________________________________________
QColor OxygenHelper::calcShadowColor(const QColor &color) const
{

    return KColorScheme::shade(
        KColorUtils::mix(QColor(255,255,255),color, color.alpha()*(1/255.0)),
        KColorScheme::ShadowShade,
        _contrast);

}

//____________________________________________________________________
QColor OxygenHelper::cachedBackgroundColor(const QColor &color, qreal ratio)
{

    quint64 key = (quint64(color.rgba()) << 32) | int(ratio*512);
    QColor *out = m_backgroundColorCache.object(key);
    if( !out )
    {
        if( ratio < 0.5 )
        {

            qreal a = 2.0*ratio;
            out = new QColor( KColorUtils::mix(backgroundTopColor(color), color, a) );

        } else {

            qreal a = 2.0*ratio-1;
            out = new QColor( KColorUtils::mix(color, backgroundBottomColor(color), a) );

        }

        m_backgroundColorCache.insert(key, out );

    }

    return *out;

}


//____________________________________________________________________
QPixmap OxygenHelper::verticalGradient(const QColor &color, int height, int offset)
{
    quint64 key = (quint64(color.rgba()) << 32) | height | 0x8000;
    QPixmap *pixmap = m_backgroundCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(32, height);
        pixmap->fill( Qt::transparent );

        QLinearGradient gradient(0, offset, 0, height+offset);
        gradient.setColorAt(0.0, backgroundTopColor(color));
        gradient.setColorAt(0.5, color);
        gradient.setColorAt(1.0, backgroundBottomColor(color));

        QPainter p(pixmap);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(pixmap->rect(), gradient);

        p.end();

        m_backgroundCache.insert(key, pixmap);
    }

    return *pixmap;
}

//____________________________________________________________________
QPixmap OxygenHelper::radialGradient(const QColor &color, int width, int height)
{
    quint64 key = (quint64(color.rgba()) << 32) | width | 0xb000;
    QPixmap *pixmap = m_backgroundCache.object(key);

    if (!pixmap)
    {
        pixmap = new QPixmap(width, height);
        pixmap->fill(Qt::transparent);

        QColor radialColor = backgroundRadialColor(color);
        radialColor.setAlpha(255);
        QRadialGradient gradient(64, height-64, 64);
        gradient.setColorAt(0, radialColor);
        radialColor.setAlpha(101);
        gradient.setColorAt(0.5, radialColor);
        radialColor.setAlpha(37);
        gradient.setColorAt(0.75, radialColor);
        radialColor.setAlpha(0);
        gradient.setColorAt(1, radialColor);

        QPainter p(pixmap);
        p.scale(width/128.0,1);
        p.fillRect(QRect(0,0,128,height), gradient);

        p.end();

        m_backgroundCache.insert(key, pixmap);
    }

    return *pixmap;
}

//____________________________________________________________________________________
QColor OxygenHelper::decoColor(const QColor& background, const QColor &color) const
{ return KColorUtils::mix( background, color, 0.4 + 0.8*_contrast ); }

//_______________________________________________________________________
QRegion OxygenHelper::roundedRegion( const QRect& r, int left, int right, int top, int bottom ) const
{
    // get rect geometry
    int x, y, w, h;
    r.getRect(&x, &y, &w, &h);

    // disable bottom corners when border frame is too small and window is not shaded
    QRegion mask( x+5*left,   y+0*top, w-5*(left+right), h-0*(top+bottom));
    mask += QRegion(x+0*left, y+5*top, w-0*(left+right), h-5*(top+bottom));
    mask += QRegion(x+2*left, y+2*top, w-2*(left+right), h-2*(top+bottom));
    mask += QRegion(x+3*left, y+1*top, w-3*(left+right), h-1*(top+bottom));
    mask += QRegion(x+1*left, y+3*top, w-1*(left+right), h-3*(top+bottom));
    return mask;

}

//_______________________________________________________________________
QRegion OxygenHelper::roundedMask( const QRect& r, int left, int right, int top, int bottom ) const
{
    // get rect geometry
    int x, y, w, h;
    r.getRect(&x, &y, &w, &h);

    QRegion mask(x + 4*left, y + 0*top, w-4*(left+right), h-0*(top+bottom));
    mask += QRegion(x + 0*left, y + 4*top, w-0*(left+right), h-4*(top+bottom));
    mask += QRegion(x + 2*left, y + 1*top, w-2*(left+right), h-1*(top+bottom));
    mask += QRegion(x + 1*left, y + 2*top, w-1*(left+right), h-2*(top+bottom));

    return mask;
}

//______________________________________________________________________
void OxygenHelper::drawFloatFrame(
    QPainter *p, const QRect r,
    const QColor &color,
    bool drawUglyShadow, bool isActive, const QColor &frameColor, TileSet::Tiles tiles ) const
{

    p->save();
    p->setRenderHint(QPainter::Antialiasing);
    QRect frame = r;
    frame.adjust(1,1,-1,-1);
    int x,y,w,h;
    frame.getRect(&x, &y, &w, &h);

    QColor light = calcLightColor(backgroundTopColor(color));
    QColor dark = calcLightColor(backgroundBottomColor(color));
    p->setBrush(Qt::NoBrush);

    if (drawUglyShadow)
    {

        if(isActive)
        {
            //window active - it's a glow - not a shadow
            QColor glow = KColorUtils::mix(QColor(128,128,128),frameColor,0.7);
            p->setPen(glow);

            if( tiles & TileSet::Top )
            {
                p->drawLine(QPointF(x+4, y-0.5), QPointF(x+w-4, y-0.5));
                p->drawArc(QRectF(x-0.5, y-0.5, 11, 11),90*16, 90*16);
                p->drawArc(QRectF(x+w-11+0.5, y-0.5, 11, 11), 0, 90*16);
            }

            if( tiles & TileSet::Left ) p->drawLine(QPointF(x-0.5, y+4), QPointF(x-0.5, y+h-4));
            if( tiles & TileSet::Right ) p->drawLine(QPointF(x+w+0.5, y+4), QPointF(x+w+0.5, y+h-4));

            if( tiles & TileSet::Bottom )
            {
                if( tiles & TileSet::Left ) p->drawArc(QRectF(x-0.5, y+h-11+0.5, 11, 11),180*16, 90*16);
                if( tiles & TileSet::Right ) p->drawArc(QRectF(x+w-11+0.5, y+h-11+0.5, 11, 11),270*16, 90*16);
                p->drawLine(QPointF(x+4, y+h+0.5), QPointF(x+w-4, y+h+0.5));
            }

            light = KColorUtils::mix(light, frameColor);
            dark = KColorUtils::mix(dark, frameColor);

        } else {

            // window inactive - draw something resembling shadow
            // fully desaturate
            QColor shadow = KColorUtils::darken(color, 0.0, 0.0);

            if( tiles & TileSet::Top )
            {
                p->setPen(KColorUtils::darken(shadow, 0.2));
                p->drawLine(QPointF(x+4, y-0.5), QPointF(x+w-4, y-0.5));
                if( tiles & TileSet::Left ) p->drawArc(QRectF(x-0.5, y-0.5, 11, 11),90*16, 90*16);
                if( tiles & TileSet::Right ) p->drawArc(QRectF(x+w-11+0.5, y-0.5, 11, 11), 0, 90*16);
            }

            p->setPen(KColorUtils::darken(shadow, 0.35));
            if( tiles & TileSet::Left ) p->drawLine(QPointF(x-0.5, y+4), QPointF(x-0.5, y+h-4));
            if( tiles & TileSet::Right ) p->drawLine(QPointF(x+w+0.5, y+4), QPointF(x+w+0.5, y+h-4));

            if( tiles & TileSet::Bottom )
            {

                p->setPen(KColorUtils::darken(shadow, 0.45));
                if( tiles & TileSet::Left ) p->drawArc(QRectF(x-0.5, y+h-11+0.5, 11, 11),180*16, 90*16);
                if( tiles & TileSet::Right ) p->drawArc(QRectF(x+w-11+0.5, y+h-11+0.5, 11, 11),270*16, 90*16);
                p->setPen(KColorUtils::darken(shadow, 0.6));
                p->drawLine(QPointF(x+4, y+h+0.5), QPointF(x+w-4, y+h+0.5));

            }

        }
    }

    // top frame
    if( tiles & TileSet::Top )
    {
        p->setPen(QPen(light, 0.8));
        p->drawLine(QPointF(x+4, y+0.6), QPointF(x+w-4, y+0.6));
    }

    // corner and side frames
    // sides are drawn even if Top only is selected, but with a different gradient
    if( h >= 4+1.5 )
    {
      QLinearGradient lg(0.0, y+1.5, 0.0, y+h-4);
      lg.setColorAt(0, light);
      lg.setColorAt(1, alphaColor(dark, 0) );

      if( h > 8.5 ) lg.setColorAt(qMax( 0.0, 3.0/(h-5.5) ), dark);
      if( h > 20.5 ) lg.setColorAt(qMax( 0.0, 1.0 - 12.0/(h-5.5) ), dark);

      p->setPen(QPen(lg, 0.8));
      if( tiles & TileSet::Left ) p->drawLine(QPointF(x+0.6, y+4), QPointF(x+0.6, y+h-4));
      if( tiles & TileSet::Right ) p->drawLine(QPointF(x+w-0.6, y+4), QPointF(x+w-0.6, y+h-4));
    }

    if( tiles & TileSet::Top )
    {
      p->drawArc(QRectF(x+0.6, y+0.6, 9, 9),90*16, 90*16);
      p->drawArc(QRectF(x+w-9-0.6, y+0.6, 9, 9), 0, 90*16);
    }

    p->restore();

}

//______________________________________________________________________________________
void OxygenHelper::drawSeparator(QPainter *p, const QRect &rect, const QColor &color, Qt::Orientation orientation) const
{
    QColor light = calcLightColor(color);
    QColor dark = calcDarkColor(color);

    p->save();
    p->setRenderHint(QPainter::Antialiasing,false);

    QPoint start,end,offset;

    if (orientation == Qt::Horizontal) {
        start = QPoint(rect.x(),rect.y()+rect.height()/2-1);
        end = QPoint(rect.right(),rect.y()+rect.height()/2-1);
        offset = QPoint(0,1);
    } else {
        start = QPoint(rect.x()+rect.width()/2-1,rect.y());
        end = QPoint(rect.x()+rect.width()/2-1,rect.bottom());
        offset = QPoint(1,0);
        light.setAlpha(150);
    }

    QLinearGradient lg(start,end);
    lg.setColorAt(0.3, dark);
    lg.setColorAt(0.7, dark);
    dark.setAlpha(0);
    lg.setColorAt(0.0, dark);
    lg.setColorAt(1.0, dark);
    p->setPen(QPen(lg,1));

    if (orientation == Qt::Horizontal)
        p->drawLine(start,end);
    else
        p->drawLine(start+offset,end+offset);

    lg = QLinearGradient(start,end);
    lg.setColorAt(0.3, light);
    lg.setColorAt(0.7, light);
    light.setAlpha(0);
    lg.setColorAt(0.0, light);
    lg.setColorAt(1.0, light);
    p->setPen(QPen(lg,1));


    if (orientation == Qt::Horizontal)
        p->drawLine(start+offset,end+offset);
    else
    {
        p->drawLine(start,end);
        p->drawLine(start+offset*2,end+offset*2);
    }

    p->restore();
}

//________________________________________________________________________________________________________
TileSet *OxygenHelper::slab(const QColor &color, qreal shade, int size )
{
    Oxygen::Cache<TileSet>::Value *cache = m_slabCache.get(color);
    quint64 key = (int)(256.0 * shade) << 24 | size;
    TileSet *tileSet = cache->object(key);

    if (!tileSet)
    {
        QPixmap pixmap(size*2, size*2);
        pixmap.fill(Qt::transparent);

        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setWindow(0,0,14,14);

        // shadow
        drawShadow(p, calcShadowColor(color), 14);
        drawSlab(p, color, shade);

        p.end();

        tileSet = new TileSet(pixmap, size, size, size, size, size-1, size, 2, 1);

        cache->insert(key, tileSet);
    }

    return tileSet;
}

//______________________________________________________________________________________
void OxygenHelper::drawSlab(QPainter &p, const QColor &color, qreal shade) const
{
    const QColor base = KColorUtils::shade(color, shade);
    const QColor light = KColorUtils::shade(calcLightColor(color), shade);
    const QColor dark = KColorUtils::shade(calcDarkColor(color), shade);

    // bevel, part 1
    p.save();
    qreal y = KColorUtils::luma(base);
    qreal yl = KColorUtils::luma(light);
    qreal yd = KColorUtils::luma(dark);
    QLinearGradient bevelGradient1(0, 7, 0, 11);
    bevelGradient1.setColorAt(0.0, light);
    if (y < yl && y > yd) // no middle when color is very light/dark
        bevelGradient1.setColorAt(0.5, base);
    bevelGradient1.setColorAt(0.9, base);
    p.setBrush(bevelGradient1);
    p.drawEllipse(QRectF(3.0,3.0,8.0,8.0));

    // bevel, part 2
    if (_slabThickness > 0.0) {
        QLinearGradient bevelGradient2(0, 6, 0, 19);
        bevelGradient2.setColorAt(0.0, light);
        bevelGradient2.setColorAt(0.9, base);
        p.setBrush(bevelGradient2);
        p.drawEllipse(QRectF(3.6,3.6,6.8,6.8));
    }

    // inside mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);

    const qreal ic = 3.6 + 0.5*_slabThickness;
    const qreal is = 6.8 - (2.0*0.5*_slabThickness);
    p.drawEllipse(QRectF(ic, ic, is, is));
    p.restore();
}

//___________________________________________________________________________________________
void OxygenHelper::drawShadow(QPainter &p, const QColor &color, int size) const
{
    const qreal m = qreal(size-2)*0.5;
    const qreal offset = 0.8;
    const qreal k0 = (m-4.0) / m;
    QRadialGradient shadowGradient(m+1.0, m+offset+1.0, m);
    for (int i = 0; i < 8; i++) { // sinusoidal gradient
        qreal k1 = (k0 * qreal(8 - i) + qreal(i)) * 0.125;
        qreal a = (cos(3.14159 * i * 0.125) + 1.0) * 0.25;
        shadowGradient.setColorAt(k1, alphaColor(color, a * _shadowGain));
    }
    shadowGradient.setColorAt(1.0, alphaColor(color, 0.0));
    p.save();
    p.setBrush(shadowGradient);
    p.drawEllipse(QRectF(0, 0, size, size));
    p.restore();

}

//_______________________________________________________________________
void OxygenHelper::drawOuterGlow( QPainter &p, const QColor &color, int size) const
{

    const QRectF r(0, 0, size, size);
    const qreal m = qreal(size)*0.5;
    const qreal width( 3 );

    const qreal bias = _glowBias * qreal(14)/size;

    // k0 is located at width - bias from the outer edge
    qreal gm = m + bias - 0.9;
    qreal k0 = (m-width+bias) / gm ;
    QRadialGradient glowGradient(m, m, gm );
    for (int i = 0; i < 8; i++)
    {

        // k1 grows linearly from k0 to 1.0
        qreal k1 =  k0 + qreal(i)*(1.0-k0)/8.0;

        // a folows sqrt curve
        qreal a = 1.0 - sqrt(qreal(i)/8);
        glowGradient.setColorAt(k1, alphaColor(color, a));
    }

    // glow
    p.save();
    p.setBrush(glowGradient);
    p.drawEllipse(r);

    // inside mask
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    p.drawEllipse(r.adjusted(width, width, -width, -width));
    p.restore();

}
