#ifndef oxygen_style_helper_h
#define oxygen_style_helper_h

/*
 * SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * SPDX-FileCopyrightText: 2007 Casper Boemann <cbr@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#include "oxygen.h"
#include "oxygenhelper.h"

#include <KWindowSystem>

#if OXYGEN_HAVE_X11
#include <xcb/xcb.h>
#endif

//* helper class
/** contains utility functions used at multiple places in oxygen style */
namespace Oxygen
{

class StyleHelper : public Helper
{
public:
    //* constructor
    explicit StyleHelper(KSharedConfigPtr config);

    //* destructor
    //* clear cache
    void invalidateCaches() override;

    //* update maximum cache size
    void setMaxCacheSize(int) override;

    //* background gradient
    void setUseBackgroundGradient(bool value)
    {
        _useBackgroundGradient = value;
    }

    //* render window background using a given color as a reference
    /**
    For the widget style, both the gradient and the background pixmap are rendered in the same method.
    All the actual rendering is performed by the base class
    */
    using Helper::renderWindowBackground;
    void renderWindowBackground(QPainter *, const QRect &, const QWidget *, const QColor &, int y_shift = -23) override;

    //* set background gradient hint to widget
    void setHasBackgroundGradient(WId, bool) const override;

    // render menu background
    void renderMenuBackground(QPainter *p, const QRect &clipRect, const QWidget *widget, const QPalette &pal)
    {
        renderMenuBackground(p, clipRect, widget, pal.color(widget->window()->backgroundRole()));
    }

    // render menu background
    void renderMenuBackground(QPainter *, const QRect &, const QWidget *, const QColor &);

    //*@name color utilities
    //@{

    //* glow color for buttons (mouse-over takes precedence over focus)
    QColor buttonGlowColor(const QPalette &palette, StyleOptions options, qreal opacity, AnimationMode mode) const
    {
        return buttonGlowColor(palette.currentColorGroup(), options, opacity, mode);
    }

    //* glow color for frames (focus takes precedence over mouse-over)
    QColor frameGlowColor(const QPalette &palette, StyleOptions options, qreal opacity, AnimationMode mode) const
    {
        return frameGlowColor(palette.currentColorGroup(), options, opacity, mode);
    }

    //* glow color for arrows (mouse-over takes precedence over focus)
    QColor arrowColor(const QPalette &palette, StyleOptions options, qreal opacity, AnimationMode mode) const;

    //* glow color for buttons (mouse-over takes precedence over focus)
    QColor buttonGlowColor(QPalette::ColorGroup, StyleOptions, qreal, AnimationMode) const;

    //* glow color for frames (focus takes precedence over mouse-over)
    QColor frameGlowColor(QPalette::ColorGroup, StyleOptions, qreal, AnimationMode) const;

    //* returns menu background color matching position in a given menu widget
    QColor menuBackgroundColor(const QColor &color, const QWidget *w, const QPoint &point)
    {
        if (!(w && w->window()) || checkAutoFillBackground(w))
            return color;
        else
            return menuBackgroundColor(color, w->window()->height(), w->mapTo(w->window(), point).y());
    }

    //* returns menu background color matching position in a menu widget of given height
    QColor menuBackgroundColor(const QColor &color, int height, int y)
    {
        return backgroundColor(color, qMin(qreal(1.0), qreal(y) / qMin(200, 3 * height / 4)));
    }

    //* color
    inline QColor calcMidColor(const QColor &color);

    //* merge active and inactive palettes based on ratio, for smooth enable state change transition
    QPalette disabledPalette(const QPalette &, qreal ratio) const;

    //@}

    //* overloaded window decoration buttons for MDI windows
    QPixmap dockWidgetButton(const QColor &color, bool pressed, int size = 21);

    //* round corners( used for Menus, combobox drop-down, detached toolbars and dockwidgets
    TileSet roundCorner(const QColor &, int size = 5);

    //* groupbox background
    TileSet slope(const QColor &, qreal shade, int size = TileSet::DefaultSize);

    //*@name slabs
    //@{

    //* inverse (inner-hole) shadow
    /** this method must be public because it is used directly by OxygenStyle to draw dials */
    void drawInverseShadow(QPainter &, const QColor &, int pad, int size, qreal fuzz) const;

    //* fill a slab of given size with brush set on painter
    void fillSlab(QPainter &, const QRect &, int size = TileSet::DefaultSize) const;

    //* linear gradient used to fill buttons
    void fillButtonSlab(QPainter &, const QRect &, const QColor &, bool sunken);

    //* default slab
    TileSet slab(const QColor &color, qreal shade, int size = TileSet::DefaultSize)
    {
        return slab(color, QColor(), shade, size);
    }

    //* default slab (with glow)
    TileSet slab(const QColor &, const QColor &glow, qreal shade, int size = TileSet::DefaultSize);

    //* sunken slab
    TileSet slabSunken(const QColor &, int size = TileSet::DefaultSize);

    //* progressbar
    TileSet progressBarIndicator(const QPalette &, int);

    //* dial
    QPixmap dialSlab(const QColor &color, qreal shade, int size = TileSet::DefaultSize)
    {
        return dialSlab(color, QColor(), shade, size);
    }

    //* dial
    QPixmap dialSlab(const QColor &, const QColor &, qreal shade, int size = TileSet::DefaultSize);

    // round slabs
    QPixmap roundSlab(const QColor &color, qreal shade, int size = TileSet::DefaultSize)
    {
        return roundSlab(color, QColor(), shade, size);
    }

    // round slab
    QPixmap roundSlab(const QColor &, const QColor &glow, qreal shade, int size = TileSet::DefaultSize);

    //* slider slab
    QPixmap sliderSlab(const QColor &, const QColor &glow, bool sunken, qreal shade, int size = TileSet::DefaultSize);

    //@}

    //* debug frame
    void renderDebugFrame(QPainter *, const QRect &) const;

    //*@name holes
    //@{

    void fillHole(QPainter &, const QRect &, int offset = 2) const;

    //* generic hole
    void renderHole(QPainter *painter, const QColor &color, const QRect &rect, StyleOptions options = {}, TileSet::Tiles tiles = TileSet::Ring)
    {
        renderHole(painter, color, rect, options, -1, Oxygen::AnimationNone, tiles);
    }

    //* generic hole (with animated glow)
    void renderHole(QPainter *, const QColor &, const QRect &, StyleOptions, qreal, Oxygen::AnimationMode, TileSet::Tiles = TileSet::Ring);

    TileSet holeFlat(const QColor &, qreal shade, bool fill = true, int size = TileSet::DefaultSize);

    //* scrollbar hole
    TileSet scrollHole(const QColor &, Qt::Orientation orientation, bool smallShadow = false);

    //* scrollbar handle
    TileSet scrollHandle(const QColor &, const QColor &, int size = TileSet::DefaultSize);

    //@}

    //* focus rect for flat toolbuttons
    TileSet slitFocused(const QColor &);

    //* dock frame
    TileSet dockFrame(const QColor &, const QColor &);

    //* selection
    TileSet selection(const QColor &, int height, bool custom);

    //* inverse glow
    /** this method must be public because it is used directly by OxygenStyle to draw dials */
    void drawInverseGlow(QPainter &, const QColor &, int pad, int size, int rsize) const;

    //*@name utility functions

    //* returns true if compositing is active
    bool compositingActive(void) const;

    //* returns true if a given widget supports alpha channel
    inline bool hasAlphaChannel(const QWidget *) const;

    //* returns true if given widget will get a decoration
    bool hasDecoration(const QWidget *) const;

    //@}

private:
    //*@name holes
    //@{

    //* holes
    TileSet hole(const QColor &color, int size = TileSet::DefaultSize, StyleOptions options = {})
    {
        return hole(color, QColor(), size, options);
    }

    //* holes
    TileSet hole(const QColor &, const QColor &glow, int size = TileSet::DefaultSize, StyleOptions = {});

    //@}

    //* generic slab painting (to be stored in tilesets)
    void drawSlab(QPainter &, const QColor &, qreal shade);

    // round slabs
    void drawRoundSlab(QPainter &, const QColor &, qreal);

    // slider slabs
    void drawSliderSlab(QPainter &, const QColor &, bool sunken, qreal);

    //* initialize
    void init(void);

    //* background grandient
    bool _useBackgroundGradient;

    Cache<QPixmap> _dialSlabCache;
    Cache<QPixmap> _roundSlabCache;
    Cache<QPixmap> _sliderSlabCache;
    Cache<TileSet> _holeCache;
    Cache<TileSet> _scrollHandleCache;
    Cache<TileSet> _slabCache;

    //* mid color cache
    ColorCache _midColorCache;

    //* dock button cache
    PixmapCache _dockWidgetButtonCache;

    using TileSetCache = BaseCache<TileSet>;
    TileSetCache _slabSunkenCache;
    TileSetCache _cornerCache;
    TileSetCache _holeFlatCache;
    TileSetCache _slopeCache;
    TileSetCache _slitCache;
    TileSetCache _dockFrameCache;
    TileSetCache _scrollHoleCache;
    TileSetCache _selectionCache;
    TileSetCache _progressBarCache;

#if OXYGEN_HAVE_X11

    //* atom used for compositing manager
    xcb_atom_t _compositingManagerAtom;

#endif
};

//____________________________________________________________________
QColor StyleHelper::calcMidColor(const QColor &color)
{
    const quint64 key(color.rgba());
    if (QColor *cachedColor = _midColorCache.object(key)) {
        return *cachedColor;
    }

    QColor out = KColorScheme::shade(color, KColorScheme::MidShade, _contrast - 1.0);
    _midColorCache.insert(key, new QColor(out));

    return out;
}

//____________________________________________________________________
bool StyleHelper::hasAlphaChannel(const QWidget *widget) const
{
    return compositingActive() && widget && widget->testAttribute(Qt::WA_TranslucentBackground);
}
}
#endif
