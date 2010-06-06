#ifndef oxygen_h
#define oxygen_h

/* Oxygen widget style for KDE 4
   Copyright (C) 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
#include <KWindowSystem>

#include <QtGui/QAbstractScrollArea>
#include <QtGui/QBitmap>
#include <QtGui/QDockWidget>
#include <QtGui/QFrame>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QStyleOption>
#include <QtGui/QTabBar>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>

#include "oxygenstylehelper.h"
#include "animations/oxygenwidgetstateengine.h"

namespace Oxygen
{
    class Animations;
    class Transitions;
    class WindowManager;
    class FrameShadowFactory;
    class WidgetExplorer;

    //! main oxygen style class.
    /*! it is responsible to draw all the primitives to be displayed on screen, on request from Qt paint engine */
    class Style : public KStyle
    {
        Q_OBJECT
        Q_CLASSINFO ("X-KDE-CustomElements", "true")

        public:

        //! constructor
        Style();

        //! destructor
        virtual ~Style()
        {}

        //! reimp from QCommonStyle
        virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *widget) const;
        virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const;
        virtual void drawComplexControl(ComplexControl control,const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const;
        virtual void drawItemText(QPainter*, const QRect&, int alignment, const QPalette&, bool enabled,
            const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const;

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

        //!@name polishing
        //@{

        virtual void polish( QWidget* );
        virtual void unpolish( QWidget* );
        using  KStyle::polish;
        using  KStyle::unpolish;

        //@}

        int styleHint(
            StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0,
            QStyleHintReturn * returnData = 0 ) const;

        virtual int pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const;
        virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const;
        virtual QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const;

        //! internal option flags to pass arguments around
        enum StyleOption {
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
        virtual bool drawGenericArrow(WidgetType,  int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
        virtual bool drawGenericFrame(WidgetType,  int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
        virtual bool drawFocusIndicator(WidgetType,  int, const QStyleOption*, const QRect &, const QPalette &, State, QPainter*, const QWidget*, Option*) const;
        //@}

        //! capacity bar
        virtual void drawCapacityBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const;

        //! animations
        Animations& animations( void ) const
        { return *_animations; }

        Transitions& transitions( void ) const
        { return *_transitions; }

        //! window manager
        WindowManager& windowManager( void ) const
        { return *_windowManager; }

        //! frame shadows
        FrameShadowFactory& frameShadowFactory( void ) const
        { return *_frameShadowFactory; }

        //! widget explorer
        /*!
        this is used for debugging. Provides information about
        widgets, widgets' geometry, and ancestry tree
        */
        WidgetExplorer& widgetExplorer( void ) const
        { return *_widgetExplorer; }

        //! polish scrollarea
        void polishScrollArea( QAbstractScrollArea* ) const;

        private:

        //!@name rendering methods
        //@{

        //! header background
        void renderHeaderBackground( const QRect&, const QPalette&, QPainter*, const QWidget*, bool horizontal, bool reverse ) const;
        void renderHeaderLines( const QRect&, const QPalette&, QPainter*, TileSet::Tiles ) const;

        //! menu item
        void renderMenuItemRect( const QStyleOption* opt, const QRect& rect, const QPalette& pal, QPainter* p, qreal opacity = -1 ) const
        { renderMenuItemRect( opt, rect, pal.color(QPalette::Window), p, opacity ); }

        //! menu item
        void renderMenuItemRect( const QStyleOption*, const QRect&, const QColor&, const QPalette&, QPainter* p, qreal opacity = -1 ) const;

        //! slab glowing color
        QColor slabShadowColor( QColor color, StyleOptions opts, qreal opacity, AnimationMode mode ) const;

        //! qdial slab
        void renderDialSlab( QPainter* p, QRect r, const QColor& c, const QStyleOption* option, StyleOptions opts = 0 ) const
        { renderDialSlab( p, r, c, option, opts, -1,  AnimationNone ); }

        //! qdial slab
        void renderDialSlab( QPainter*, QRect, const QColor&, const QStyleOption*, StyleOptions, qreal, AnimationMode ) const;

        //! generic button slab
        void renderButtonSlab( QPainter* p, QRect r, const QColor& c, StyleOptions opts = 0, TileSet::Tiles tiles = TileSet::Ring) const
        { renderButtonSlab( p, r, c, opts, -1,  AnimationNone, tiles ); }

        //! generic button slab
        void renderButtonSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, AnimationMode, TileSet::Tiles ) const;

        //! generic slab
        void renderSlab( QPainter* p, QRect r, const QColor& c, StyleOptions opts = 0, TileSet::Tiles tiles = TileSet::Ring) const
        { renderSlab( p, r, c, opts, -1, AnimationNone, tiles ); }

        //! generic slab
        void renderSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, AnimationMode, TileSet::Tiles ) const;

        //! checkbox
        void renderCheckBox(QPainter *p, const QRect &r, const QPalette &pal,
            bool enabled, bool hasFocus, bool mouseOver,
            int checkPrimitive, bool sunken=false, qreal opacity = -1, AnimationMode mode = AnimationNone ) const;

        //! radio button
        void renderRadioButton(
            QPainter *p, const QRect &r, const QPalette &pal,
            bool enabled, bool hasFocus, bool mouseOver,
            int radioPrimitive, bool drawButton=true, qreal opacity = -1, AnimationMode mode = AnimationNone ) const;

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

        //! get polygon corresponding to generic arrow
        enum ArrowSize {
            ArrowNormal,
            ArrowSmall,
            ArrowTiny
        };

        // tab orientation
        enum TabOrientation {
            TabSouth,
            TabNorth,
            TabEast,
            TabWest
        };

        //! tab orientation
        TabOrientation tabOrientation( const QTabBar::Shape& shape ) const
        {
            switch( shape )
            {

                default: case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                return TabNorth;

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                return TabSouth;

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                return TabEast;

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                return TabWest;

            }

        }

        //! returns point position for generic arrows
        QPolygonF genericArrow( int primitive, ArrowSize size = ArrowNormal ) const;

        //@}

        //!@name utilities
        bool isKTextEditFrame( const QWidget* widget ) const
        { return ( widget && widget->parentWidget() && widget->parentWidget()->inherits( "KTextEditor::View" ) ); }

        //!@name event filters
        //@{

        bool eventFilter(QObject *, QEvent *);
        bool eventFilterToolBar( QToolBar*, QEvent* );
        bool eventFilterDockWidget( QDockWidget*, QEvent* );
        bool eventFilterToolBox( QToolBox*, QEvent* );
        bool eventFilterQ3ListView( QWidget*, QEvent* );
        bool eventFilterComboBoxContainer( QWidget*, QEvent* );
        bool eventFilterScrollBar( QWidget*, QEvent* );
        bool eventFilterMdiSubWindow( QMdiSubWindow*, QEvent* );
        bool eventFilterGeometryTip( QWidget*, QEvent* );

        //@}

        //! returns true if compositing is active
        bool compositingActive( void ) const
        { return KWindowSystem::compositingActive(); }

        //! returns true if a given widget supports alpha channel
        bool hasAlphaChannel( const QWidget* ) const;

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

        //! tiles from tab orientation
        TileSet::Tiles tilesByShape(QTabBar::Shape shape) const;

        //! configuration
        void loadConfiguration();

        //! custom Control element to implement re-painting of dolphin CapacityBar
        const QStyle::ControlElement CE_CapacityBar;

        //! helper
        StyleHelper &_helper;

        //! animations
        Animations* _animations;

        //! transitions
        Transitions* _transitions;

        //! window manager
        WindowManager* _windowManager;

        //! frame shadows
        FrameShadowFactory* _frameShadowFactory;

        //! widget explorer
        /*!
        this is used for debugging. Provides information about
        widgets, widgets' geometry, and ancestry tree
        */
        WidgetExplorer* _widgetExplorer;

    };
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Oxygen::Style::StyleOptions)

#endif // __OXYGEN_H

