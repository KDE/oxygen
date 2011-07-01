#ifndef oxygenstyle_h
#define oxygenstyle_h

//////////////////////////////////////////////////////////////////////////////
// oxygenstyle.h
// Oxygen widget style for KDE 4
// -------------------
//
// Copyright (C) 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenanimationmodes.h"
#include "oxygenmetrics.h"
#include "oxygentileset.h"

#include <QtCore/QMap>
#include <QtGui/QAbstractScrollArea>
#include <QtGui/QCommonStyle>
#include <QtGui/QDockWidget>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QStyleOption>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>
#include <QtGui/QWidget>

namespace Oxygen
{

    class Animations;
    class FrameShadowFactory;
    class MdiWindowShadowFactory;
    class ShadowHelper;
    class SplitterFactory;
    class StyleHelper;
    class Transitions;
    class WindowManager;
    class WidgetExplorer;

    //! toplevel manager
    class TopLevelManager: public QObject
    {
        public:

        //! constructor
        TopLevelManager( QObject* parent, const StyleHelper& helper ):
            QObject( parent ),
            _helper( helper )
        {}

        //! event filter
        virtual bool eventFilter(QObject *, QEvent *);

        private:

        //! helper
        const StyleHelper& _helper;

    };


    //! base class for oxygen style
    /*! it is responsible to draw all the primitives to be displayed on screen, on request from Qt paint engine */
    class Style: public QCommonStyle
    {
        Q_OBJECT

        /* this tells kde applications that custom style elements are supported, using the kstyle mechanism */
        Q_CLASSINFO ("X-KDE-CustomElements", "true")

        public:

        //! constructor
        explicit Style( void );

        //! destructor
        virtual ~Style( void );

        //! widget polishing
        virtual void polish( QWidget* );

        //! widget unpolishing
        virtual void unpolish( QWidget* );

        //! needed to avoid warnings at compilation time
        using  QCommonStyle::polish;
        using  QCommonStyle::unpolish;

        //! pixel metrics
        virtual int pixelMetric(PixelMetric, const QStyleOption* = 0, const QWidget* = 0) const;

        //! style hints
        virtual int styleHint(StyleHint, const QStyleOption* = 0, const QWidget* = 0, QStyleHintReturn* = 0) const;

        //! returns rect corresponding to one widget's subelement
        virtual QRect subElementRect( SubElement subRect, const QStyleOption*, const QWidget* ) const;

        //! returns rect corresponding to one widget's subcontrol
        virtual QRect subControlRect( ComplexControl, const QStyleOptionComplex*, SubControl, const QWidget* ) const;

        //! returns size matching contents
        QSize sizeFromContents( ContentsType, const QStyleOption*, const QSize&, const QWidget* ) const;

        //! returns which subcontrol given QPoint corresponds to
        SubControl hitTestComplexControl( ComplexControl, const QStyleOptionComplex*, const QPoint&, const QWidget* ) const;

        //! primitives
        void drawPrimitive( PrimitiveElement, const QStyleOption*, QPainter*, const QWidget* ) const;

        //! controls
        void drawControl( ControlElement, const QStyleOption*, QPainter*, const QWidget* ) const;

        //! complex controls
        void drawComplexControl( ComplexControl, const QStyleOptionComplex*, QPainter*, const QWidget* ) const;

        //! generic text rendering
        virtual void drawItemText(
            QPainter*, const QRect&, int alignment, const QPalette&, bool enabled,
            const QString&, QPalette::ColorRole = QPalette::NoRole) const;

        //! event filters
        virtual bool eventFilter(QObject *, QEvent *);

        //!@name specialized event filters
        /*!
        Note: one could use separate objects to install these event filters
        This would have the advantage to avoid the big 'if' statement in the
        Style::eventFilter method
        */

        //@{
        bool eventFilterComboBoxContainer( QWidget*, QEvent* );
        bool eventFilterDockWidget( QDockWidget*, QEvent* );
        bool eventFilterGeometryTip( QWidget*, QEvent* );
        bool eventFilterMdiSubWindow( QMdiSubWindow*, QEvent* );
        bool eventFilterQ3ListView( QWidget*, QEvent* );
        bool eventFilterScrollBar( QWidget*, QEvent* );
        bool eventFilterTabBar( QWidget*, QEvent* );
        bool eventFilterToolBar( QToolBar*, QEvent* );
        bool eventFilterToolBox( QToolBox*, QEvent* );

        //! install event filter to object, in a unique way
        void addEventFilter( QObject* object )
        {
            object->removeEventFilter( this );
            object->installEventFilter( this );
        }

        //@}

        protected slots:

        //! update oxygen configuration
        void oxygenConfigurationChanged( void );

        //! needed to update style when configuration is changed
        void globalPaletteChanged( void );

        //! copied from kstyle
        int layoutSpacingImplementation(
            QSizePolicy::ControlType, QSizePolicy::ControlType, Qt::Orientation,
            const QStyleOption* option, const QWidget* widget ) const
        { return pixelMetric(PM_DefaultLayoutSpacing, option, widget); }

        //! standard icons
        virtual QIcon standardIconImplementation(
            StandardPixmap standardIcon,
            const QStyleOption *option,
            const QWidget *widget) const;

        protected:

        //! initialize kGlobalSettings conections
        void initializeKGlobalSettings( void );

        //! helper
        StyleHelper& helper( void ) const
        { return *_helper; }

        //! shadow Helper
        ShadowHelper& shadowHelper( void ) const
        { return *_shadowHelper; }

        //!@name enumerations and convenience classes
        //@{

        //! arrow orientation
        enum ArrowOrientation
        {
            ArrowNone,
            ArrowUp,
            ArrowDown,
            ArrowLeft,
            ArrowRight
        };


        //! get polygon corresponding to generic arrow
        enum ArrowSize
        {
            ArrowNormal,
            ArrowSmall,
            ArrowTiny
        };

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

        //! used to store slab characteristics
        class SlabRect
        {
            public:

            //! constructor
            SlabRect(void):
                _tiles( TileSet::Ring )
            {}

            //! constructor
            SlabRect( const QRect& r, const int& tiles ):
                _r( r ),
                _tiles( TileSet::Tiles( tiles ) )
            {}

            QRect _r;
            TileSet::Tiles _tiles;

        };

        //! list of slabs
        typedef QList<SlabRect> SlabRectList;

        /*!
        tabBar data class needed for
        the rendering of tabbars when
        one tab is being drawn
        */
        class TabBarData: public QObject
        {

            public:

            //! constructor
            TabBarData( Style* parent ):
                QObject( parent ),
                _style( parent ),
                _dirty( false )
            {}

            //! destructor
            virtual ~TabBarData( void )
            {}

            //! assign target tabBar
            void lock( const QWidget* widget )
            { _tabBar = widget; }

            //! true if tabbar is locked
            bool locks( const QWidget* widget ) const
            { return _tabBar && _tabBar.data() == widget; }

            //! set dirty
            void setDirty( const bool& value = true )
            { _dirty = value; }

            //! release
            void release( void )
            { _tabBar.clear(); }

            //! draw tabBarBase
            virtual void drawTabBarBaseControl( const QStyleOptionTab*, QPainter*, const QWidget* );

            private:

            //! pointer to parent style object
            QWeakPointer<const Style> _style;

            //! pointer to target tabBar
            QWeakPointer<const QWidget> _tabBar;

            //! if true, will paint on next TabBarTabShapeControlCall
            bool _dirty;

        };

        //@}

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

        //! mdi window shadows
        MdiWindowShadowFactory& mdiWindowShadowFactory( void ) const
        { return *_mdiWindowShadowFactory; }

        //! widget explorer
        /*!
        this is used for debugging. Provides information about
        widgets, widgets' geometry, and ancestry tree
        */
        WidgetExplorer& widgetExplorer( void ) const
        { return *_widgetExplorer; }

        //! splitter factory
        SplitterFactory& splitterFactory( void ) const
        { return *_splitterFactory; }

        //! tabBar data
        TabBarData& tabBarData( void ) const
        { return *_tabBarData; }

        //!@name subelementRect specialized functions
        //@{

        //! default implementation. Does not change anything
        QRect defaultSubElementRect( const QStyleOption* option, const QWidget* ) const
        { return option->rect; }

        //! pushbutton contents
        QRect pushButtonContentsRect( const QStyleOption* option, const QWidget* ) const
        {
            return insideMargin( option->rect,
                PushButton_ContentsMargin,
                PushButton_ContentsMargin_Left,
                PushButton_ContentsMargin_Top,
                PushButton_ContentsMargin_Right,
                PushButton_ContentsMargin_Bottom );
        }

        //! toolbox tab
        QRect toolBoxTabContentsRect( const QStyleOption* option, const QWidget* ) const
        { return insideMargin( option->rect, 0, 5, 0, 5, 0 ); }

        //! checkbox contents
        QRect checkBoxContentsRect( const QStyleOption* option, const QWidget* ) const
        { return handleRTL( option, option->rect.adjusted( CheckBox_Size + CheckBox_BoxTextSpace, 0, 0, 0 ) ); }

        //! progressbar contents
        QRect progressBarContentsRect( const QStyleOption* option, const QWidget* ) const;

        //! tabBar buttons
        QRect tabBarTabLeftButtonRect( const QStyleOption* option, const QWidget* widget ) const
        { return tabBarTabButtonRect( SE_TabBarTabLeftButton, option, widget ); }

        QRect tabBarTabRightButtonRect( const QStyleOption* option, const QWidget* widget ) const
        { return tabBarTabButtonRect( SE_TabBarTabRightButton, option, widget ); }

        QRect tabBarTabButtonRect( SubElement, const QStyleOption*, const QWidget* ) const;

        // tabbar tab text
        QRect tabBarTabTextRect( const QStyleOption* option, const QWidget* widget ) const
        { return QCommonStyle::subElementRect( SE_TabBarTabText, option, widget ).adjusted( 6, 0, -6, 0 ); }

        // tab widgets
        QRect tabWidgetTabContentsRect( const QStyleOption*, const QWidget* ) const;
        QRect tabWidgetTabPaneRect( const QStyleOption*, const QWidget* ) const;

        QRect tabWidgetLeftCornerRect( const QStyleOption* option, const QWidget* widget ) const;
        QRect tabWidgetRightCornerRect( const QStyleOption* option, const QWidget* widget ) const;

        //@}

        //!@name subcontrol Rect specialized functions
        //@{

        QRect groupBoxSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect comboBoxSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect scrollBarSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect sliderSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;
        QRect spinBoxSubControlRect( const QStyleOptionComplex*, SubControl, const QWidget* ) const;

        //! this properly handles single/double or no scrollBar buttons
        QRect scrollBarInternalSubControlRect( const QStyleOptionComplex*, SubControl ) const;

        //@}

        //!@name sizeFromContents
        //@{

        QSize checkBoxSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize comboBoxSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize headerSectionSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;

        QSize menuBarSizeFromContents( const QStyleOption*, const QSize& size, const QWidget* ) const
        { return size; }

        QSize menuBarItemSizeFromContents( const QStyleOption*, const QSize& size, const QWidget* ) const
        { return expandSize( size, MenuBarItem_Margin, MenuBarItem_Margin_Left, 0, MenuBarItem_Margin_Right, 0 ); }

        QSize menuItemSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;
        QSize pushButtonSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;

        QSize tabWidgetSizeFromContents( const QStyleOption*, const QSize& size, const QWidget* ) const
        { return expandSize( size, TabWidget_ContentsMargin - 2 ); }

        QSize tabBarTabSizeFromContents( const QStyleOption*, const QSize& size, const QWidget* ) const;
        QSize toolButtonSizeFromContents( const QStyleOption*, const QSize&, const QWidget* ) const;

        //@}

        //!@name primitives specialized functions
        //@{

        bool emptyPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const
        { return true; }

        bool drawFramePrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameFocusRectPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameGroupBoxPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameMenuPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameTabBarBasePrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameTabWidgetPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawFrameWindowPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;

        bool drawIndicatorArrowUpPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowUp, option, painter, widget ); }

        bool drawIndicatorArrowDownPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowDown, option, painter, widget ); }

        bool drawIndicatorArrowLeftPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowLeft, option, painter, widget ); }

        bool drawIndicatorArrowRightPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        { return drawIndicatorArrowPrimitive( ArrowRight, option, painter, widget ); }

        bool drawIndicatorArrowPrimitive( ArrowOrientation, const QStyleOption*, QPainter*, const QWidget* ) const;

        //! dock widget separators
        /*! it uses the same painting as QSplitter, but due to Qt, the horizontal/vertical convention is inverted */
        bool drawIndicatorDockWidgetResizeHandlePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget) const
        {

            renderSplitter( option, painter, widget, !(option->state & State_Horizontal ) );
            return true;

        }

        bool drawIndicatorHeaderArrowPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelButtonCommandPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelMenuPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelButtonToolPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelScrollAreaCornerPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelTipLabelPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelItemViewItemPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawPanelLineEditPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorMenuCheckMarkPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawQ3CheckListIndicatorPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawQ3CheckListExclusiveIndicatorPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorBranchPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorButtonDropDownPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorCheckBoxPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorRadioButtonPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorTabTearPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorToolBarHandlePrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;
        bool drawIndicatorToolBarSeparatorPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;

        bool drawWidgetPrimitive( const QStyleOption*, QPainter*, const QWidget* ) const;

        //@}

        //!@name controls specialized functions
        //@{

        bool emptyControl( const QStyleOption*, QPainter*, const QWidget* ) const
        { return true; }

        virtual bool drawCapacityBarControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawComboBoxLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawDockWidgetTitleControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawHeaderEmptyAreaControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawHeaderLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawHeaderSectionControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawMenuBarItemControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawMenuItemControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarContentsControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarGrooveControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawProgressBarLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawPushButtonLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawRubberBandControl( const QStyleOption*, QPainter*, const QWidget* ) const;

        //! scrollbar
        virtual bool drawScrollBarSliderControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarAddLineControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarSubLineControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarAddPageControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawScrollBarSubPageControl( const QStyleOption*, QPainter*, const QWidget* ) const;

        virtual bool drawShapedFrameControl( const QStyleOption*, QPainter*, const QWidget* ) const;

        // size grip
        virtual bool drawSizeGripControl( const QStyleOption*, QPainter*, const QWidget* ) const
        {
            // size grips, whose usage is discouraged in KDE, are not rendered at all by oxygen
            return true;
        }

        // splitters
        virtual bool drawSplitterControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
        {
            renderSplitter( option, painter, widget, option->state & State_Horizontal );
            return true;
        }

        virtual bool drawTabBarTabLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;

        //! tabbar tabs.
        /*! there are two methods (_Single and _Plain) implemented, to deal with tabbar appearance selected from options */
        virtual bool drawTabBarTabShapeControl_Single( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawTabBarTabShapeControl_Plain( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolBarControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolBoxTabLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolBoxTabShapeControl( const QStyleOption*, QPainter*, const QWidget* ) const;
        virtual bool drawToolButtonLabelControl( const QStyleOption*, QPainter*, const QWidget* ) const;

        //!@}

        //!@name complex ontrols specialized functions
        //@{
        bool drawComboBoxComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawDialComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawGroupBoxComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawQ3ListViewComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawSliderComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawSpinBoxComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawTitleBarComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        bool drawToolButtonComplexControl( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;
        //@}

        //! true if widget is child of KTextEdit
        bool isKTextEditFrame( const QWidget* widget ) const
        { return ( widget && widget->parentWidget() && widget->parentWidget()->inherits( "KTextEditor::View" ) ); }

        //! adjust rect based on provided margins
        QRect insideMargin( const QRect& r, int main, int left = 0, int top = 0, int right = 0, int bottom = 0 ) const
        { return r.adjusted( main+left, main+top, -main-right, -main-bottom ); }

        //! expand size based on margins
        QSize expandSize( const QSize& size, int main, int left = 0, int top = 0, int right = 0, int bottom = 0 ) const
        { return size + QSize( 2*main+left+right, 2*main+top+bottom ); }

        //! returns true for vertical tabs
        bool isVerticalTab( const QStyleOptionTab* option ) const
        { return isVerticalTab( option->shape ); }

        bool isVerticalTab( const QTabBar::Shape& shape ) const
        {
            return shape == QTabBar::RoundedEast
                || shape == QTabBar::RoundedWest
                || shape == QTabBar::TriangularEast
                || shape == QTabBar::TriangularWest;

        }

        //! returns true for reflected tabs
        bool isReflected( const QStyleOptionTab* option ) const
        { return isReflected( option->shape ); }

        bool isReflected( const QTabBar::Shape& shape ) const
        {
            return shape == QTabBar::RoundedEast
                || shape == QTabBar::RoundedSouth
                || shape == QTabBar::TriangularEast
                || shape == QTabBar::TriangularSouth;

        }

        //! right to left alignment handling
        QRect handleRTL(const QStyleOption* opt, const QRect& subRect) const
        { return visualRect(opt->direction, opt->rect, subRect); }

        //! right to left alignment handling
        QPoint handleRTL(const QStyleOption* opt, const QPoint& pos) const
        { return visualPos(opt->direction, opt->rect, pos); }

        QRect centerRect(const QRect &in, const QSize& s ) const
        { return centerRect( in, s.width(), s.height() ); }

        QRect centerRect(const QRect &in, int w, int h) const
        { return QRect(in.x() + (in.width() - w)/2, in.y() + (in.height() - h)/2, w, h); }

        /*
        Checks whether the point is before the bound rect for
        bound of given orientation
        */
        inline bool preceeds( const QPoint&, const QRect&, const QStyleOption* ) const;

        //! return which arrow button is hit by point for scrollbar double buttons
        inline QStyle::SubControl scrollBarHitTest( const QRect&, const QPoint&, const QStyleOption* ) const;

        //! polish scrollarea
        void polishScrollArea( QAbstractScrollArea* ) const;

        //! tiles from tab orientation
        inline TileSet::Tiles tilesByShape( const QTabBar::Shape& shape) const;

        //! toolbar mask
        /*! this masks out toolbar expander buttons, when visible, from painting */
        QRegion tabBarClipRegion( const QTabBar* ) const;

        //! adjusted slabRect
        inline void adjustSlabRect( SlabRect& slab, const QRect&, bool documentMode, bool vertical ) const;

        //!@name internal rendering methods
        /*! here mostly to avoid code duplication */
        //@{

        //! qdial slab
        void renderDialSlab( QPainter* p, const QRect& r, const QColor& c, const QStyleOption* option, StyleOptions opts = 0 ) const
        { renderDialSlab( p, r, c, option, opts, -1,  AnimationNone ); }

        //! qdial slab
        void renderDialSlab( QPainter*, const QRect&, const QColor&, const QStyleOption*, StyleOptions, qreal, AnimationMode ) const;

        //! generic button slab
        void renderButtonSlab( QPainter* p, QRect r, const QColor& c, StyleOptions opts = 0, TileSet::Tiles tiles = TileSet::Ring) const
        { renderButtonSlab( p, r, c, opts, -1,  AnimationNone, tiles ); }

        //! generic button slab
        void renderButtonSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, AnimationMode, TileSet::Tiles ) const;

        //! generic slab
        void renderSlab( QPainter* painter, const SlabRect& slab, const QColor& color, StyleOptions options = 0 ) const
        { renderSlab( painter, slab._r, color, options, slab._tiles ); }

        //! generic slab
        void renderSlab( QPainter* painter, QRect rect, const QColor& color, StyleOptions options = 0, TileSet::Tiles tiles = TileSet::Ring) const
        { renderSlab( painter, rect, color, options, -1, AnimationNone, tiles ); }

        //! generic slab
        void renderSlab( QPainter* painter, const SlabRect& slab, const QColor& color, StyleOptions options, qreal opacity, AnimationMode mode ) const
        { renderSlab( painter, slab._r, color, options, opacity, mode, slab._tiles ); }

        //! generic slab
        void renderSlab( QPainter*, QRect, const QColor&, StyleOptions, qreal, AnimationMode, TileSet::Tiles ) const;

        // render tab background
        void renderTabBackground( QPainter*, const QRect&, const QPalette&, const QTabBar::Shape, const QWidget* ) const;

        //! tab background
        /*! this paints window background behind tab when tab is being dragged */
        void fillTabBackground( QPainter*, const QRect&, const QColor&, const QTabBar::Shape, const QWidget* ) const;

        //! tab filling
        void fillTab( QPainter*, const QRect&, const QColor&, const QTabBar::Shape, bool active ) const;

        //! spinbox arrows
        void renderSpinBoxArrow( QPainter*, const QStyleOptionSpinBox*, const QWidget*, const SubControl& ) const;

        //! splitter
        void renderSplitter( const QStyleOption*, QPainter*, const QWidget*, bool ) const;

        //! mdi subwindow titlebar button
        void renderTitleBarButton( QPainter*, const QStyleOptionTitleBar*, const QWidget*, const SubControl& ) const;
        void renderTitleBarIcon( QPainter*, const QRectF&, const SubControl& ) const;

        //! header background
        void renderHeaderBackground( const QRect&, const QPalette&, QPainter*, const QWidget*, bool horizontal, bool reverse ) const;
        void renderHeaderLines( const QRect&, const QPalette&, QPainter*, TileSet::Tiles ) const;

        //! menu item background
        void renderMenuItemBackground( const QStyleOption*, QPainter*, const QWidget* ) const;

        void renderMenuItemRect( const QStyleOption* opt, const QRect& rect, const QPalette& pal, QPainter* p, qreal opacity = -1 ) const
        { renderMenuItemRect( opt, rect, pal.color(QPalette::Window), p, opacity ); }

        void renderMenuItemRect( const QStyleOption*, const QRect&, const QColor&, const QPalette&, QPainter* p, qreal opacity = -1 ) const;

        //! checkbox state (used for checkboxes _and_ radio buttons)
        enum CheckBoxState
        {
            CheckOn,
            CheckOff,
            CheckTriState
        };

        //! checkbox
        void renderCheckBox( QPainter*, const QRect&, const QPalette&, StyleOptions, CheckBoxState, qreal opacity = -1, AnimationMode mode = AnimationNone ) const;

        //! radio button
        void renderRadioButton( QPainter*, const QRect&, const QPalette&, StyleOptions, CheckBoxState, qreal opacity = -1, AnimationMode mode = AnimationNone ) const;

        //! scrollbar hole
        void renderScrollBarHole( QPainter*, const QRect&, const QColor&, const Qt::Orientation&, const TileSet::Tiles& = TileSet::Full ) const;

        //! scrollbar handle (non animated)
        void renderScrollBarHandle(
            QPainter* painter, const QRect& r, const QPalette& palette,
            const Qt::Orientation& orientation, const bool& hover) const
        { renderScrollBarHandle( painter, r, palette, orientation, hover, -1 ); }

        //! scrollbar handle (animated)
        void renderScrollBarHandle( QPainter*, const QRect&, const QPalette&, const Qt::Orientation&, const bool&, const qreal& ) const;

        //! scrollbar arrow
        void renderScrollBarArrow( QPainter*, const QRect&, const QColor&, const QColor&, ArrowOrientation ) const;

        //! returns true if given scrollbar arrow is animated
        QColor scrollBarArrowColor( const QStyleOption*, const SubControl&, const QWidget* ) const;

        //! slider tickmarks
        void renderSliderTickmarks( QPainter*, const QStyleOptionSlider*, const QWidget* ) const;

        //@}

        //! slab glowing color
        QColor slabShadowColor( QColor, StyleOptions, qreal, AnimationMode ) const;

        //! returns point position for generic arrows
        QPolygonF genericArrow( ArrowOrientation, ArrowSize = ArrowNormal ) const;

        //! scrollbar buttons
        enum ScrollBarButtonType
        {
            NoButton,
            SingleButton,
            DoubleButton
        };

        //! returns height for scrollbar buttons depending of button types
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

        //!@name custom elemenents
        //@{

        //! generic element
        int newStyleElement( const QString &element, const char *check, int &counter )
        {

            if( !element.contains(check) ) return 0;
            int id = _styleElements.value(element, 0);
            if( !id )
            {
                ++counter;
                id = counter;
                _styleElements.insert(element, id);
            }
            return id;
        }

        //! style hint
        QStyle::StyleHint newStyleHint( const QString &element )
        { return (StyleHint) newStyleElement( element, "SH_", _hintCounter ); }

        //! control element
        QStyle::ControlElement newControlElement( const QString &element )
        { return (ControlElement)newStyleElement( element, "CE_", _controlCounter ); }

        //! subElement
        QStyle::SubElement newSubElement(const QString &element )
        { return (SubElement)newStyleElement( element, "SE_", _subElementCounter ); }

        //@}

        private:

        //! true if KGlobalSettings signals are initialized
        bool _kGlobalSettingsInitialized;

        //!@name scrollbar button types (for addLine and subLine )
        //@{
        ScrollBarButtonType _addLineButtons;
        ScrollBarButtonType _subLineButtons;
        //@}

        //!@name metrics for scrollbar buttons
        //@{
        int _noButtonHeight;
        int _singleButtonHeight;
        int _doubleButtonHeight;
        //@}

        //! true if keyboard accelerators must be drawn
        bool _showMnemonics;

        //! helper
        StyleHelper* _helper;

        //! shadow helper
        ShadowHelper* _shadowHelper;

        //! animations
        Animations* _animations;

        //! transitions
        Transitions* _transitions;

        //! window manager
        WindowManager* _windowManager;

        //! toplevel manager
        TopLevelManager* _topLevelManager;

        //! frame shadows
        FrameShadowFactory* _frameShadowFactory;

        //! mdi window shadows
        MdiWindowShadowFactory* _mdiWindowShadowFactory;

        //! widget explorer
        WidgetExplorer* _widgetExplorer;

        //! tabBar data
        TabBarData* _tabBarData;

        //! splitter Factory, to extend splitters hit area
        SplitterFactory* _splitterFactory;

        //! pointer to primitive specialized function
        typedef bool (Style::*StylePrimitive)( const QStyleOption*, QPainter*, const QWidget* ) const;
        StylePrimitive _frameFocusPrimitive;

        //! pointer to control specialized function
        typedef bool (Style::*StyleControl)( const QStyleOption*, QPainter*, const QWidget* ) const;
        StyleControl _tabBarTabShapeControl;

        //! pointer to control specialized function
        typedef bool (Style::*StyleComplexControl)( const QStyleOptionComplex*, QPainter*, const QWidget* ) const;

        //!@name custom elements
        //@{

        int _hintCounter;
        int _controlCounter;
        int _subElementCounter;
        QHash<QString, int> _styleElements;

        QStyle::ControlElement CE_CapacityBar;

        //@}

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

    //_____________________________________________________________
    TileSet::Tiles Style::tilesByShape( const QTabBar::Shape& shape ) const
    {
        switch( shape )
        {

            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            return TileSet::Top | TileSet::Left | TileSet::Right;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            return TileSet::Bottom | TileSet::Left | TileSet::Right;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            return TileSet::Right | TileSet::Top | TileSet::Bottom;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            return TileSet::Left | TileSet::Top | TileSet::Bottom;

            default:
            return TileSet::Ring;

        }

    }

    //___________________________________________________________________________________
    void Style::adjustSlabRect( SlabRect& slab, const QRect& tabWidgetRect, bool documentMode, bool vertical ) const
    {

        // no tabWidget found, do nothing
        if( documentMode || !tabWidgetRect.isValid() ) return;
        else if( vertical )
        {

            slab._r.setTop( qMax( slab._r.top(), tabWidgetRect.top() ) );
            slab._r.setBottom( qMin( slab._r.bottom(), tabWidgetRect.bottom() ) );

        } else {

            slab._r.setLeft( qMax( slab._r.left(), tabWidgetRect.left() ) );
            slab._r.setRight( qMin( slab._r.right(), tabWidgetRect.right() ) );

        }

        return;
    }

}

#endif
