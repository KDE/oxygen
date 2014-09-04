#ifndef oxygenstyle_h
#define oxygenstyle_h

//////////////////////////////////////////////////////////////////////////////
// oxygenstyle.h
// Oxygen widget style for KDE 4
// -------------------
//
// Copyright (C) 2009-2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright (C) 2008 Long Huynh Huu <long.upcase@googlemail.com>
// Copyright (C) 2007-2008 Casper Boemann <cbr@boemann.dk>
// Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
// Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>
//
// based on the KDE style "dotNET":
// Copyright (C) 2001-2002, Chris Lee <clee@kde.org>
// Carsten Pfeiffer <pfeiffer@kde.org>
// Karol Szwed <gallium@kde.org>
// Drawing routines completely reimplemented from KDE3 HighColor, which was
// originally based on some stuff from the KDE2 HighColor.
//
// based on drawing routines of the style "Keramik":
// Copyright (c) 2002 Malte Starostik <malte@kde.org>
// (c) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
// based on the KDE3 HighColor Style
// Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
// (C) 2001-2002 Fredrik Höglund  <fredrik@kde.org>
// Drawing routines adapted from the KDE2 HCStyle,
// Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
// (C) 2000 Dirk Mueller          <mueller@kde.org>
// (C) 2001 Martijn Klingens      <klingens@kde.org>
// Progressbar code based on KStyle,
// Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
//////////////////////////////////////////////////////////////////////////////

#include "oxygen.h"
#include "oxygentileset.h"

#include <KStyle>

#include <QAbstractScrollArea>
#include <QDockWidget>
#include <QMdiSubWindow>
#include <QStyleOption>
#include <QStyleOptionSlider>
#include <QStylePlugin>
#include <QToolBar>
#include <QToolBox>
#include <QWidget>

#include <QIcon>

namespace OxygenPrivate
{
    class TabBarData;
}

namespace Oxygen
{

    class Animations;
    class FrameShadowFactory;
    class MdiWindowShadowFactory;
    class Mnemonics;
    class ShadowHelper;
    class SplitterFactory;
    class StyleHelper;
    class Transitions;
    class TopLevelManager;
    class WindowManager;
    class WidgetExplorer;
    class BlurHelper;

    //* convenience typedef for base class
    #if USE_KDE4
    using ParentStyleClass = QCommonStyle;
    #else
    using ParentStyleClass = KStyle;
    #endif

    //* base class for oxygen style
    /** it is responsible to draw all the primitives to be displayed on screen, on request from Qt paint engine */
    class Style: public ParentStyleClass
    {
        Q_OBJECT

        /* this tells kde applications that custom style elements are supported, using the kstyle mechanism */
        Q_CLASSINFO ("X-KDE-CustomElements", "true")

        public:

        //* constructor
        explicit Style( void );

        //* destructor
        virtual ~Style( void );

        //* needed to avoid warnings at compilation time
        using  ParentStyleClass::polish;
        using  ParentStyleClass::unpolish;

        //* widget polishing
        virtual void polish( QWidget* );

        //* widget unpolishing
        virtual void unpolish( QWidget* );

        //* pixel metrics
        virtual int pixelMetric(PixelMetric, const QStyleOption* = nullptr, const QWidget* = nullptr) const;

        //* style hints
        virtual int styleHint(StyleHint, const QStyleOption* = nullptr, const QWidget* = nullptr, QStyleHintReturn* = nullptr) const;

        //* returns rect corresponding to one widget's subelement
        virtual QRect subElementRect( SubElement, const QStyleOption*, const QWidget* ) const;

        //* returns rect corresponding to one widget's subcontrol
        virtual QRect subControlRect( ComplexControl, const QStyleOptionComplex*, SubControl, const QWidget* ) const;

        //* returns size matching contents
        QSize sizeFromContents( ContentsType, const QStyleOption*, const QSize&, const QWidget* ) const;

        //* returns which subcontrol given QPoint corresponds to
        SubControl hitTestComplexControl( ComplexControl, const QStyleOptionComplex*, const QPoint&, const QWidget* ) const;

        //* primitives
        void drawPrimitive( PrimitiveElement, const QStyleOption*, QPainter*, const QWidget* ) const;

        //* controls
        void drawControl( ControlElement, const QStyleOption*, QPainter*, const QWidget* ) const;

        //* complex controls
        void drawComplexControl( ComplexControl, const QStyleOptionComplex*, QPainter*, const QWidget* ) const;

        //* generic text rendering
        virtual void drawItemText(
            QPainter*, const QRect&, int alignment, const QPalette&, bool enabled,
            const QString&, QPalette::ColorRole = QPalette::NoRole) const;

        //*@name event filters
        //@{

        virtual bool eventFilter(QObject *, QEvent *);
        bool eventFilterComboBoxContainer( QWidget*, QEvent* );
        bool eventFilterDockWidget( QDockWidget*, QEvent* );
        bool eventFilterMdiSubWindow( QMdiSubWindow*, QEvent* );
        bool eventFilterScrollBar( QWidget*, QEvent* );
        bool eventFilterTabBar( QWidget*, QEvent* );
        bool eventFilterToolBar( QToolBar*, QEvent* );
        bool eventFilterToolBox( QToolBox*, QEvent* );

        //* install event filter to object, in a unique way
        void addEventFilter( QObject* object )
        {
            object->removeEventFilter( this );
            object->installEventFilter( this );
        }

        //@}

        protected Q_SLOTS:

        //* update configuration
        void configurationChanged( void );

        //* standard icons
        virtual QIcon standardIconImplementation( StandardPixmap, const QStyleOption*, const QWidget* ) const;

        protected:

        //* standard icons
        virtual QIcon standardIcon( StandardPixmap pixmap, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const
        { return standardIconImplementation( pixmap, option, widget ); }

        //* load configuration
        void loadConfiguration();

        //*@name enumerations and convenience classes
        //@{

        //* used to store slab characteristics
        class SlabRect
        {
            public:

            //* constructor
            explicit SlabRect(void);

            //* constructor
            SlabRect( const QRect& rect, int tiles );

            //* validity
            bool isValid( void ) const;

            /**
            adjust rectangle depending on tiles and tileSize
            so that it is rendered properly
            **/
            void adjust( int );

            /**
            adjust rectangle depending on tiles and tileSize
            so that it is rendered properly
            **/
            SlabRect adjusted( int ) const;

            QRect rect;
            TileSet::Tiles tiles;

            //* list of slabs
            using List = QList<SlabRect>;

        };

        //@}

        //*@name subelementRect specialized functions
        //@{

        //* default implementation. Does not change anything
        QRect defaultSubElementRect( const QStyleOption* option, const QWidget* ) const
        { return option->rect; }

        QRect pushButtonContentsRect( const QStyleOption* option, const QWidget* ) const;
        QRect checkBoxContentsRect( const QStyleOption* option, const QWidget* ) const;
        QRect lineEditContentsRect( const QStyleOption*, const QWidget* ) const;
        QRect progressBarGrooveRect( const QStyleOption* option, const QWidget* ) const;
        QRect progressBarContentsRect( const QStyleOption* option, const QWidget* ) const;
        QRect headerArrowRect( const QStyleOption* option, const QWidget* ) const;
        QRect headerLabelRect( const QStyleOption* option, const QWidget* ) const;
        QRect tabWidgetTabBarRect( const QStyleOption*, const QWidget* ) const;
        QRect tabWidgetTabContentsRect( const QStyleOption*, const QWidget* ) const;
        QRect tabWidgetTabPaneRect( const QStyleOption*, const QWidget* ) const;
        QRect tabWidgetCornerRect( SubElement, const QStyleOption* option, const QWidget* widget ) const;
        QRect toolBoxTabContentsRect( const QStyleOption* option, const QWidget* ) const;

        //@}

        //*@name subcontrol Rect specialized functions
        //@{

        QRect groupBoxSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect toolButtonSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect comboBoxSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect spinBoxSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect scrollBarInternalSubControlRect( const QStyleOptionComplex*, SubControl ) const;
        QRect scrollBarSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect sliderSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;

        //@}

        //*@name sizeFromContents
        //@{
        QSize defaultSizeFromContents( const QStyleOption*, const QSize& size, const QWidget* ) const
        { return size; }

        QSize checkBoxSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize lineEditSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize comboBoxSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize spinBoxSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize sliderSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize pushButtonSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize toolButtonSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize menuBarItemSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize menuItemSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize tabWidgetSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize tabBarTabSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize headerSectionSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize itemViewItemSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;

        //@}

        //*@name primitives specialized functions
        //@{

        bool emptyPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const
        { return true; }

        bool drawFramePrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameLineEditPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameMenuPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameGroupBoxPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameTabWidgetPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameTabBarBasePrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameWindowPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;

        bool drawIndicatorArrowUpPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowUp, option, painter, widget ); }

        bool drawIndicatorArrowDownPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowDown, option, painter, widget ); }

        bool drawIndicatorArrowLeftPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowLeft, option, painter, widget ); }

        bool drawIndicatorArrowRightPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowRight, option, painter, widget ); }

        //* dock widget separators
        /** it uses the same painting as QSplitter, but due to Qt, the horizontal/vertical convention is inverted */
        bool drawIndicatorDockWidgetResizeHandlePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget) const
        {
            renderSplitter( option, painter, widget, !(option->state & State_Horizontal ) );
            return true;
        }

        bool drawIndicatorArrowPrimitive( ArrowOrientation, const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorHeaderArrowPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelButtonCommandPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelButtonToolPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawTabBarPanelButtonToolPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelScrollAreaCornerPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelMenuPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelTipLabelPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelItemViewItemPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorMenuCheckMarkPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorCheckBoxPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorRadioButtonPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorButtonDropDownPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorTabClosePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const;
        bool drawIndicatorTabTearPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorToolBarHandlePrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorToolBarSeparatorPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorBranchPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawWidgetPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;

        //@}

        //*@name controls specialized functions
        //@{

        bool emptyControl( const QStyleOption*, QPainter*, const QWidget* ) const
        { return true; }

        virtual bool drawPushButtonLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolButtonLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawMenuBarItemControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawMenuItemControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarContentsControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarGrooveControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarSliderControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarAddLineControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarSubLineControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawShapedFrameControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawRubberBandControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawHeaderSectionControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawHeaderEmptyAreaControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawTabBarTabLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawTabBarTabShapeControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawTabBarTabShapeControl_selected( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawTabBarTabShapeControl_unselected( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolBoxTabLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolBoxTabShapeControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawDockWidgetTitleControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolBarControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawSplitterControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        {
            renderSplitter( option, painter, widget, option->state & State_Horizontal );
            return true;
        }

        //*@}

        //*@name complex ontrols specialized functions
        //@{
        bool drawToolButtonComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawComboBoxComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawSpinBoxComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawSliderComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawDialComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawScrollBarComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawTitleBarComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        //@}

        //*@name internal rendering methods
        /** here mostly to avoid code duplication */
        //@{

        //* qdial slab
        void renderDialSlab( QPainter* p, const QRect& r, const QColor& c, const QStyleOption* option, StyleOptions opts = 0 ) const
        { renderDialSlab( p, r, c, option, opts, -1,  AnimationNone ); }

        //* qdial slab
        void renderDialSlab( QPainter*, const QRect&, const QColor&, const QStyleOption*, StyleOptions, qreal, AnimationMode ) const;

        //* generic button slab
        void renderButtonSlab( QPainter* p, QRect r, const QColor& c, StyleOptions opts = 0, TileSet::Tiles tiles = TileSet::Ring) const
        { renderButtonSlab( p, r, c, opts, -1,  AnimationNone, tiles ); }

        //* generic button slab
        void renderButtonSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, AnimationMode, TileSet::Tiles ) const;

        //* generic slab
        void renderSlab( QPainter* painter, const SlabRect& slab, const QColor& color, StyleOptions options = 0 ) const
        { renderSlab( painter, slab.rect, color, options, slab.tiles ); }

        //* generic slab
        void renderSlab( QPainter* painter, QRect rect, const QColor& color, StyleOptions options = 0, TileSet::Tiles tiles = TileSet::Ring) const
        { renderSlab( painter, rect, color, options, -1, AnimationNone, tiles ); }

        //* generic slab
        void renderSlab( QPainter* painter, const SlabRect& slab, const QColor& color, StyleOptions options, qreal opacity, AnimationMode mode ) const
        { renderSlab( painter, slab.rect, color, options, opacity, mode, slab.tiles ); }

        //* generic slab
        void renderSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, AnimationMode, TileSet::Tiles ) const;

        //* tab background
        /** this paints window background behind tab when tab is being dragged */
        void fillTabBackground( QPainter*, const QRect&, const QColor&, const QWidget* ) const;

        //* tab filling
        void fillTab( QPainter*, const QRect&, const QColor&, const QTabBar::Shape ) const;

        //* spinbox arrows
        void renderSpinBoxArrow( QPainter*, const QStyleOptionSpinBox*, const QWidget*, const SubControl& ) const;

        //* splitter
        void renderSplitter( const QStyleOption*, QPainter*, const QWidget*, bool ) const;

        //* mdi subwindow titlebar button
        void renderTitleBarButton( QPainter*, const QStyleOptionTitleBar*, const QWidget*, const SubControl& ) const;
        void renderTitleBarButton( QPainter*, const QRect& r, const QColor&, const QColor&, const SubControl& ) const;
        void renderTitleBarIcon( QPainter*, const QRect&, const SubControl& ) const;

        //* header background
        void renderHeaderBackground( const QRect&, const QPalette&, QPainter*, const QWidget*, bool horizontal, bool reverse ) const;
        void renderHeaderLines( const QRect&, const QPalette&, QPainter*, TileSet::Tiles ) const;

        //* menu item background
        void renderMenuItemBackground( const QStyleOption*, QPainter*, const QWidget* ) const;

        void renderMenuItemRect( const QStyleOption* opt, const QRect& rect, const QPalette& pal, QPainter* p, qreal opacity = -1 ) const
        { renderMenuItemRect( opt, rect, pal.color(QPalette::Window), p, opacity ); }

        void renderMenuItemRect( const QStyleOption*, const QRect&, const QColor&, const QPalette&, QPainter* p, qreal opacity = -1 ) const;

        //* checkbox state (used for checkboxes _and_ radio buttons)
        enum CheckBoxState
        {
            CheckOn,
            CheckOff,
            CheckTriState,
            CheckSunken
        };

        //* checkbox
        void renderCheckBox( QPainter*, const QRect&, const QPalette&, StyleOptions, CheckBoxState, qreal opacity = -1, AnimationMode mode = AnimationNone ) const;

        //* radio button
        void renderRadioButton( QPainter*, const QRect&, const QPalette&, StyleOptions, CheckBoxState, qreal opacity = -1, AnimationMode mode = AnimationNone ) const;

        //* scrollbar hole
        void renderScrollBarHole( QPainter*, const QRect&, const QColor&, const Qt::Orientation&, const TileSet::Tiles& = TileSet::Full ) const;

        //* scrollbar handle (non animated)
        void renderScrollBarHandle(
            QPainter* painter, const QRect& r, const QPalette& palette,
            const Qt::Orientation& orientation, const bool& hover) const
        { renderScrollBarHandle( painter, r, palette, orientation, hover, -1 ); }

        //* scrollbar handle (animated)
        void renderScrollBarHandle( QPainter*, const QRect&, const QPalette&, const Qt::Orientation&, const bool&, const qreal& ) const;

        //* scrollbar arrow
        void renderScrollBarArrow( QPainter*, const QRect&, const QColor&, const QColor&, ArrowOrientation ) const;

        //* returns true if given scrollbar arrow is animated
        QColor scrollBarArrowColor( const QStyleOptionSlider*, const SubControl&, const QWidget* ) const;

        //* debug frame
        void renderDebugFrame( QPainter*, const QRect& ) const;

        //@}

        //!*@name various utilty functions
        //@{

        //* return dial angle based on option and value
        qreal dialAngle( const QStyleOptionSlider*, int ) const;

        //* polish scrollarea
        void polishScrollArea( QAbstractScrollArea* ) const;

        //* toolbar mask
        /** this masks out toolbar expander buttons, when visible, from painting */
        QRegion tabBarClipRegion( const QTabBar* ) const;

        //* returns point position for generic arrows
        QPolygonF genericArrow( ArrowOrientation, ArrowSize = ArrowNormal ) const;

        //* scrollbar buttons
        enum ScrollBarButtonType
        {
            NoButton,
            SingleButton,
            DoubleButton
        };

        //* returns height for scrollbar buttons depending of button types
        int scrollBarButtonHeight( const ScrollBarButtonType& type ) const
        {
            switch( type )
            {
                case NoButton: return _noButtonHeight;
                case SingleButton: return _singleButtonHeight;
                case DoubleButton: return _doubleButtonHeight;
                default: return 0;
            }
        }

        /**
        separator can have a title and an icon
        in that case they are rendered as sunken flat toolbuttons
        return toolbutton option that matches named separator menu items
        */
        QStyleOptionToolButton separatorMenuItemOption( const QStyleOptionMenuItem*, const QWidget* ) const;

        //@}

        //* adjust rect based on provided margins
        QRect insideMargin( const QRect& r, int margin ) const
        { return insideMargin( r, margin, margin ); }

        //* adjust rect based on provided margins
        QRect insideMargin( const QRect& r, int marginWidth, int marginHeight ) const
        { return r.adjusted( marginWidth, marginHeight, -marginWidth, -marginHeight ); }

        //* expand size based on margins
        QSize expandSize( const QSize& size, int margin ) const
        { return expandSize( size, margin, margin ); }

        //* expand size based on margins
        QSize expandSize( const QSize& size, int marginWidth, int marginHeight ) const
        { return size + 2*QSize( marginWidth, marginHeight ); }

        //* returns true for vertical tabs
        bool isVerticalTab( const QStyleOptionTab* option ) const
        { return isVerticalTab( option->shape ); }

        bool isVerticalTab( const QTabBar::Shape& shape ) const
        {
            return shape == QTabBar::RoundedEast
                || shape == QTabBar::RoundedWest
                || shape == QTabBar::TriangularEast
                || shape == QTabBar::TriangularWest;

        }

        //* right to left alignment handling
        using ParentStyleClass::visualRect;
        QRect visualRect(const QStyleOption* opt, const QRect& subRect) const
        { return ParentStyleClass::visualRect(opt->direction, opt->rect, subRect); }

        //* centering
        QRect centerRect(const QRect &rect, const QSize& size ) const
        { return centerRect( rect, size.width(), size.height() ); }

        QRect centerRect(const QRect &rect, int width, int height) const
        { return QRect(rect.left() + (rect.width() - width)/2, rect.top() + (rect.height() - height)/2, width, height); }

        /*
        Checks whether the point is before the bound rect for bound of given orientation.
        This is needed to implement custom number of buttons in scrollbars,
        as well as proper mouse-hover
        */
        inline bool preceeds( const QPoint&, const QRect&, const QStyleOption* ) const;

        //* return which arrow button is hit by point for scrollbar double buttons
        inline QStyle::SubControl scrollBarHitTest( const QRect&, const QPoint&, const QStyleOption* ) const;

        //* adjusted slabRect
        inline void adjustSlabRect( SlabRect& slab, const QRect&, bool documentMode, bool vertical ) const;

        private:

        //*@name scrollbar button types (for addLine and subLine )
        //@{
        ScrollBarButtonType _addLineButtons;
        ScrollBarButtonType _subLineButtons;
        //@}

        //*@name metrics for scrollbar buttons
        //@{
        int _noButtonHeight;
        int _singleButtonHeight;
        int _doubleButtonHeight;
        //@}

        //* helper
        StyleHelper* _helper;

        //* shadow helper
        ShadowHelper* _shadowHelper;

        //* animations
        Animations* _animations;

        //* transitions
        Transitions* _transitions;

        //* window manager
        WindowManager* _windowManager;

        //* toplevel manager
        TopLevelManager* _topLevelManager;

        //* frame shadows
        FrameShadowFactory* _frameShadowFactory;

        //* mdi window shadows
        MdiWindowShadowFactory* _mdiWindowShadowFactory;

        //* keyboard accelerators
        Mnemonics* _mnemonics;

        //* blur helper
        BlurHelper* _blurHelper;

        //* widget explorer
        WidgetExplorer* _widgetExplorer;

        //* tabBar data
        OxygenPrivate::TabBarData* _tabBarData;

        //* splitter Factory, to extend splitters hit area
        SplitterFactory* _splitterFactory;

        //* pointer to primitive specialized function
        typedef bool (Style::*StylePrimitive)( const QStyleOption*, QPainter*, const QWidget* ) const;
        StylePrimitive _frameFocusPrimitive;

        //* pointer to control specialized function
        typedef bool (Style::*StyleControl)( const QStyleOption*, QPainter*, const QWidget* ) const;

        //* pointer to control specialized function
        typedef bool (Style::*StyleComplexControl)( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;

        //*@name custom elements
        //@{

        //* use Argb Drag and Drop Window
        QStyle::StyleHint SH_ArgbDndWindow;

        //* styled painting for KCapacityBar
        QStyle::ControlElement CE_CapacityBar;

        //@}

        //* tab close button icon (cached)
        mutable QIcon _tabCloseIcon;

        friend class OxygenPrivate::TabBarData;

    };

    //_________________________________________________________________________
    bool Style::preceeds( const QPoint& point, const QRect& bound, const QStyleOption* option ) const
    {
        if( option->state&QStyle::State_Horizontal)
        {

            if( option->direction == Qt::LeftToRight) return point.x() < bound.right();
            else return point.x() > bound.x();

        } else return point.y() < bound.y();

    }

    //_________________________________________________________________________
    QStyle::SubControl Style::scrollBarHitTest( const QRect& rect, const QPoint& point, const QStyleOption* option ) const
    {
        if( option->state & QStyle::State_Horizontal)
        {

            if( option->direction == Qt::LeftToRight ) return point.x() < rect.center().x() ? QStyle::SC_ScrollBarSubLine : QStyle::SC_ScrollBarAddLine;
            else return point.x() > rect.center().x() ? QStyle::SC_ScrollBarSubLine : QStyle::SC_ScrollBarAddLine;
        } else return point.y() < rect.center().y() ? QStyle::SC_ScrollBarSubLine : QStyle::SC_ScrollBarAddLine;
    }

    //___________________________________________________________________________________
    void Style::adjustSlabRect( SlabRect& slab, const QRect& tabWidgetRect, bool documentMode, bool vertical ) const
    {

        // no tabWidget found, do nothing
        if( documentMode || !tabWidgetRect.isValid() ) return;
        else if( vertical )
        {

            slab.rect.setTop( qMax( slab.rect.top(), tabWidgetRect.top() ) );
            slab.rect.setBottom( qMin( slab.rect.bottom(), tabWidgetRect.bottom() ) );

        } else {

            slab.rect.setLeft( qMax( slab.rect.left(), tabWidgetRect.left() ) );
            slab.rect.setRight( qMin( slab.rect.right(), tabWidgetRect.right() ) );

        }

        return;
    }

}

#endif
