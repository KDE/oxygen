#ifndef oxygen_h
#define oxygen_h

/* Oxygen widget style for KDE 4
   Copyright (C) 2008 Long Huynh Huu <long.upcase@googlemail.com>
   Copyright (C) 2007-2008 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
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

#include <KStyle>
#include <KColorScheme>
#include <KSharedConfig>

#include <QtGui/QAbstractScrollArea>
#include <QtGui/QBitmap>
#include <QtGui/QStyleOption>
#include <QtGui/QTabBar>

#include "helper.h"
#include "lib/tileset.h"
#include "animations/oxygenwidgetstateengine.h"

#define u_arrow -4,1, 2,1, -3,0, 1,0, -2,-1, 0,-1, -1,-2
#define d_arrow -4,-2, 2,-2, -3,-1, 1,-1, -2,0, 0,0, -1,1
#define l_arrow 0,-3, 0,3,-1,-2,-1,2,-2,-1,-2,1,-3,0
#define r_arrow -2,-3,-2,3,-1,-2, -1,2,0,-1,0,1,1,0

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

namespace Oxygen
{
  class Animations;
  class Transitions;
}

//! main oxygen style class.
/*! it is responsible to draw all the primitives to be displayed on screen, on request from Qt paint engine */
class OxygenStyle : public KStyle
{
    Q_OBJECT
    Q_CLASSINFO ("X-KDE-CustomElements", "true")

    public:

    //! constructor
    OxygenStyle();

    //! destructor
    virtual ~OxygenStyle()
    {}

    //! reimp from QCommonStyle
    virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
    virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const;
    virtual void drawComplexControl(ComplexControl control,const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;

    //! generic primitive drawing
    virtual void drawKStylePrimitive(
        WidgetType, int,
        const QStyleOption*,
        const QRect &, const QPalette &,
        State,
        QPainter*,
        const QWidget* = 0,
        Option* = 0) const;

    virtual QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const;

    virtual void polish(QWidget* widget);
    virtual void unpolish(QWidget* widget);
    using  KStyle::polish;
    using  KStyle::unpolish;


    int styleHint(
        StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0,
        QStyleHintReturn * returnData = 0 ) const;

    //virtual int styleHint(StyleHint hint, const QStyleOption * option = 0,
    // const QWidget * widget = 0, QStyleHintReturn * returnData = 0) const;
    virtual int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const;
    virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const;
    virtual QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const;

    //! internal option flags to pass arguments around
    enum StyleOption
    {
        Sunken = 0x1,
        Focus = 0x2,
        Hover = 0x4,
        Disabled = 0x8,
        NoFill = 0x10,
        SubtleShadow = 0x20
    };
    Q_DECLARE_FLAGS(StyleOptions, StyleOption)

    protected:

    //!@name dedicated complexcontrol drawing
    //@{
    virtual bool drawGroupBoxComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;
    virtual bool drawDialComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;
    virtual bool drawToolButtonComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;
    virtual void drawSliderTickmarks( const QStyleOptionSlider* option, QPainter *painter, const QWidget *widget) const;
    //@}

    //!@name dedicated primitive drawing
    //@{
    // should use a macro to declare these functions
    virtual bool drawPushButtonPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawToolBoxTabPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawProgressBarPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawMenuBarPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawMenuBarItemPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawMenuPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawMenuItemPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawDockWidgetPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawStatusBarPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawCheckBoxPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawRadioButtonPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawScrollBarPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawTabBarPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawTabWidgetPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawWindowPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawSplitterPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawSliderPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawSpinBoxPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawComboBoxPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawHeaderPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawTreePrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawLineEditPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawGroupBoxPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawToolBarPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawToolButtonPrimitive( int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    virtual bool drawGenericPrimitive(WidgetType,  int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
    //@}

    //! capacity bar
    virtual void drawCapacityBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const;

    //! animations
    Oxygen::Animations& animations( void ) const
    { return *_animations; }

    Oxygen::Transitions& transitions( void ) const
    { return *_transitions; }

    //! register scrollarea
    void registerScrollArea( QAbstractScrollArea* ) const;

    private:

    //! header background
    void renderHeaderBackground( const QRect&, const QPalette&, QPainter*, const QWidget*, bool horizontal, bool reverse ) const;

    //! menu item
    void renderMenuItemRect( const QStyleOption* opt, const QRect& rect, const QPalette& pal, QPainter* p, qreal opacity = -1 ) const
    { renderMenuItemRect( opt, rect, pal.color(QPalette::Window), p, opacity ); }

    //! menu item
    void renderMenuItemRect( const QStyleOption*, const QRect&, const QColor&, const QPalette&, QPainter* p, qreal opacity = -1 ) const;

    //! slab glowing color
    QColor slabShadowColor( QColor color, StyleOptions opts, qreal opacity, Oxygen::AnimationMode mode ) const;

    //! qdial slab
    void renderDialSlab( QPainter* p, QRect r, const QColor& c, const QStyleOption* option, StyleOptions opts = 0 ) const
    { renderDialSlab( p, r, c, option, opts, -1,  Oxygen::AnimationNone ); }

    //! qdial slab
    void renderDialSlab( QPainter*, QRect, const QColor&, const QStyleOption*, StyleOptions, qreal, Oxygen::AnimationMode ) const;

    //! generic button slab
    void renderButtonSlab( QPainter* p, QRect r, const QColor& c, StyleOptions opts = 0, TileSet::Tiles tiles = TileSet::Ring) const
    { renderButtonSlab( p, r, c, opts, -1,  Oxygen::AnimationNone, tiles ); }

    //! generic button slab
    void renderButtonSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, Oxygen::AnimationMode, TileSet::Tiles ) const;

    //! generic slab
    void renderSlab( QPainter* p, QRect r, const QColor& c, StyleOptions opts = 0, TileSet::Tiles tiles = TileSet::Ring) const
    { renderSlab( p, r, c, opts, -1, Oxygen::AnimationNone, tiles ); }

    //! generic slab
    void renderSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, Oxygen::AnimationMode, TileSet::Tiles ) const;

    //! generic hole
    void renderHole(QPainter *p, const QColor& color, const QRect &r,
        bool focus=false, bool hover=false,
        TileSet::Tiles posFlags = TileSet::Ring) const
    { renderHole( p, color, r, focus, hover, -1, Oxygen::AnimationNone, posFlags ); }

    //! generic hole (with animated glow)
    void renderHole(QPainter *p, const QColor&, const QRect &r,
        bool focus, bool hover,
        qreal opacity, Oxygen::AnimationMode animationMode,
        TileSet::Tiles posFlags = TileSet::Ring) const;

    //! checkbox
    void renderCheckBox(QPainter *p, const QRect &r, const QPalette &pal,
        bool enabled, bool hasFocus, bool mouseOver,
        int checkPrimitive, bool sunken=false, qreal opacity = -1, Oxygen::AnimationMode mode = Oxygen::AnimationNone ) const;

    //! radio button
    void renderRadioButton(
        QPainter *p, const QRect &r, const QPalette &pal,
        bool enabled, bool hasFocus, bool mouseOver,
        int radioPrimitive, bool drawButton=true, qreal opacity = -1, Oxygen::AnimationMode mode = Oxygen::AnimationNone ) const;

    void renderDot(QPainter *p, const QPointF &point, const QColor &baseColor) const;

    //! tabs
    void renderTab(
        QPainter*p, const QRect& r,
        const QPalette& pal,
        State flags,
        const QStyleOptionTabV2 *tabOpt,
        const bool reverseLayout,
        const QWidget *widget=NULL) const;

    //! tab filling
    void fillTab(
        QPainter *p, const QRect &r, const QColor &color, Qt::Orientation orientation,
        bool active, bool inverted) const;

    //! window icon (for MDI)
    void renderWindowIcon(QPainter *p, const QRectF &r, int &type) const;

    //! scrollbar hole
    void renderScrollBarHole(
        QPainter *p, const QRect &r, const QColor &color,
        Qt::Orientation orientation, TileSet::Tiles = TileSet::Full) const;

    //! scrollbar handle (non animated)
    void renderScrollBarHandle(
        QPainter *p, const QRect &r, const QPalette &pal,
        Qt::Orientation orientation, bool hover) const
    { renderScrollBarHandle( p, r, pal, orientation, hover, -1 ); }

    //! scrollbar handle (animated)
    void renderScrollBarHandle(
        QPainter *p, const QRect &r, const QPalette &pal,
        Qt::Orientation orientation, bool hover, qreal opacity) const;

    //! event filter
    /*! for some widgets special painting has to be done in event method */
    bool eventFilter(QObject *, QEvent *);

    //! returns true if compositing is active
    bool compositingActive( void ) const;

    /*!
    returns first widget in parent chain that sets autoFillBackground to true,
    or NULL if none
    */
    const QWidget* checkAutoFillBackground( const QWidget* ) const;

    protected Q_SLOTS:

    //! standard icons
    virtual QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const;

    //! needed to update style when configuration is changed
    void globalSettingsChange(int type, int arg);

    private:

    //! right to left languages
    QPoint handleRTL(const QStyleOption* opt, const QPoint& pos) const;
    QRect handleRTL(const QStyleOption* opt, const QRect& subRect) const;

    //! configuration
    void loadConfiguration();

    //! custom Control element to implement re-painting of dolphin CapacityBar
    const QStyle::ControlElement CE_CapacityBar;

    //! helper
    OxygenStyleHelper &_helper;

    KSharedConfigPtr _sharedConfig;
    KStatefulBrush _viewFocusBrush;
    KStatefulBrush _viewHoverBrush;

    TileSet *m_holeTileSet;

    //! animations
    Oxygen::Animations* _animations;

    //! transitions
    Oxygen::Transitions* _transitions;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(OxygenStyle::StyleOptions)

#endif // __OXYGEN_H

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
