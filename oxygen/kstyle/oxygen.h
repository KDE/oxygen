/* Oxygen widget style for KDE 4
   Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>

   based on the KDE style "dotNET":
   Copyright (C) 2001-2002, Chris Lee <clee@kde.org>
                            Carsten Pfeiffer <pfeiffer@kde.org>
                            Karol Szwed <gallium@kde.org>
   Drawing routines completely reimplemented from KDE3 HighColor, which was
   originally based on some stuff from the KDE2 HighColor.

   based on drawing routines of the style "Keramik":
   Copyright (c) 2002 Malte Starostik <malte@kde.org>
             (c) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
   based on the KDE3 HighColor Style
   Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
             (C) 2001-2002 Fredrik Höglund  <fredrik@kde.org>
   Drawing routines adapted from the KDE2 HCStyle,
   Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
             (C) 2000 Dirk Mueller          <mueller@kde.org>
             (C) 2001 Martijn Klingens      <klingens@kde.org>
   Progressbar code based on KStyle,
   Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>

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

#ifndef __OXYGEN_H
#define __OXYGEN_H

#include <KStyle>
#include <KColorScheme>
#include <KSharedConfig>

#include <QtGui/QBitmap>
#include <QtGui/QStyleOption>

#include "helper.h"
#include "tileset.h"

#define u_arrow -4,1, 2,1, -3,0, 1,0, -2,-1, 0,-1, -1,-2
#define d_arrow -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1
#define l_arrow 0,-3, 0,3,-1,-2,-1,2,-2,-1,-2,1,-3,0
#define r_arrow -2,-3,-2,3,-1,-2, -1,2,0,-1,0,1,1,0

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

class QTimer;

class OxygenStyle : public KStyle
{
    Q_OBJECT

public:
    OxygenStyle();
    virtual ~OxygenStyle();

    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                               QPainter *painter, const QWidget *widget) const;

    virtual void drawKStylePrimitive(WidgetType widgetType, int primitive,
                                     const QStyleOption* opt,
                                     const QRect &r, const QPalette &pal, State flags,
                                     QPainter* p,
                                     const QWidget* widget = 0,
                                     Option* kOpt = 0) const;

    virtual void polish(QWidget* widget);
    virtual void unpolish(QWidget* widget);

    virtual int styleHint(StyleHint hint, const QStyleOption * option = 0,
                          const QWidget * widget = 0, QStyleHintReturn * returnData = 0) const;
    virtual int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const;
    virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                                SubControl subControl, const QWidget* widget) const;
    virtual QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const;
public:
    enum StyleOption
    {
        Sunken = 0x1,
        Focus = 0x2,
        Hover = 0x4,
        Disabled = 0x8,
        NoFill = 0x10
    };
    Q_DECLARE_FLAGS(StyleOptions, StyleOption)

protected:
    enum TabPosition
    {
        First = 0,
        Middle,
        Last,
        Single // only one tab!
    };

    enum ColorType
    {
        ButtonContour,
        DragButtonContour,
        DragButtonSurface,
        PanelContour,
        PanelLight,
        PanelLight2,
        PanelDark,
        PanelDark2,
        MouseOverHighlight,
        FocusHighlight,
        CheckMark
    };

    void renderSlab(QPainter*, const QRect&, const QColor&, StyleOptions = 0,
                    TileSet::Tiles tiles = TileSet::Ring) const;

    void renderHole(QPainter *p,
                        const QRect &r,
                        bool focus=false,
                        bool hover=false,
                        TileSet::Tiles posFlags = TileSet::Ring) const;

    // TODO: cleanup helper methods...
    void renderCheckBox(QPainter *p, const QRect &r, const QPalette &pal,
                        bool enabled, bool hasFocus, bool mouseOver, int checkPrimitive) const;
    void renderRadioButton(QPainter *p, const QRect &r, const QPalette &pal,
                           bool enabled, bool mouseOver, int radioPrimitive) const;

    void renderDot(QPainter *p, const QPointF &point, const QColor &baseColor) const;

    void renderTab(QPainter *p,
                   const QRect &r,
                   const QPalette &pal,
                   bool mouseOver,
                   const bool selected,
                   const bool bottom,
                   const QStyleOptionTabV2 *tabOpt,
                   const bool reverseLayout) const;

    bool eventFilter(QObject *, QEvent *);

protected Q_SLOTS:
    virtual QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const;
    //Animation slots.
    void updateProgressPos();
    void progressBarDestroyed(QObject* bar);
    //For KGlobalSettings notifications
    void globalSettingsChange(int type, int arg);

    inline QColor getColor(const QPalette &pal, const ColorType t, const bool enabled = true)const;
    inline QColor getColor(const QPalette &pal, const ColorType t, const StyleOptions s)const;
private:
    mutable bool flatMode;

    int _contrast; // FIXME should be qreal, from KGlobalSettings::contrastF()
    bool _scrollBarLines;
    bool _animateProgressBar;
    bool _drawToolBarItemSeparator;
    bool _drawFocusRect;
    bool _drawTriangularExpander;
    bool _inputFocusHighlight;
    bool _customOverHighlightColor; // FIXME REMOVE ME
    bool _customFocusHighlightColor; // FIXME REMOVE ME
    QColor _overHighlightColor; // FIXME REMOVE ME
    QColor _focusHighlightColor; // FIXME REMOVE ME

    // global colors
    OxygenStyleHelper &_helper;
    KSharedConfigPtr _config;
    KStatefulBrush _viewFocusBrush;
    KStatefulBrush _viewHoverBrush;

    //Animation support.
    QMap<QWidget*, int> progAnimWidgets;
    // For progress bar animation
    QTimer *animationTimer;

    TileSet *m_holeTileSet;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(OxygenStyle::StyleOptions)

#endif // __OXYGEN_H

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
