// krazy:excludeall=qclasses

//////////////////////////////////////////////////////////////////////////////
// oxygenstyle.cpp
// Oxygen widget style for KDE 4
// -------------------
//
// Copyright ( C ) 2009-2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright ( C ) 2008 Long Huynh Huu <long.upcase@googlemail.com>
// Copyright ( C ) 2007-2008 Casper Boemann <cbr@boemann.dk>
// Copyright ( C ) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
// Copyright ( C ) 2003-2005 Sandro Giessl <sandro@giessl.com>
//
// based on the KDE style "dotNET":
// Copyright ( C ) 2001-2002, Chris Lee <clee@kde.org>
// Carsten Pfeiffer <pfeiffer@kde.org>
// Karol Szwed <gallium@kde.org>
// Drawing routines completely reimplemented from KDE3 +HighColor, which was
// originally based on some stuff from the KDE2 HighColor.
//
// based on drawing routines of the style "Keramik":
// Copyright ( c ) 2002 Malte Starostik <malte@kde.org>
// ( c ) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
// based on the KDE3 HighColor Style
// Copyright ( C ) 2001-2002 Karol Szwed <gallium@kde.org>
// ( C ) 2001-2002 Fredrik H?glund <fredrik@kde.org>
// Drawing routines adapted from the KDE2 HCStyle,
// Copyright ( C ) 2000 Daniel M. Duley <mosfet@kde.org>
// ( C ) 2000 Dirk Mueller <mueller@kde.org>
// ( C ) 2001 Martijn Klingens <klingens@kde.org>
// Progressbar code based on KStyle,
// Copyright ( C ) 2001-2002 Karol Szwed <gallium@kde.org>
//
// SPDX-License-Identifier: LGPL-2.0-only
//////////////////////////////////////////////////////////////////////////////

#include "oxygenstyle.h"

#include "oxygenanimations.h"
#include "oxygenblurhelper.h"
#include "oxygenframeshadow.h"
#include "oxygenmdiwindowshadow.h"
#include "oxygenmnemonics.h"
#include "oxygenpropertynames.h"
#include "oxygenshadowhelper.h"
#include "oxygensplitterproxy.h"
#include "oxygenstyleconfigdata.h"
#include "oxygentransitions.h"
#include "oxygenwidgetexplorer.h"
#include "oxygenwindowmanager.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDBusConnection>
#include <QDial>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsView>
#include <QGroupBox>
#include <QItemDelegate>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QSplitterHandle>
#include <QTextEdit>
#include <QToolButton>

#include <KColorUtils>

#include <cmath>

namespace OxygenPrivate
{

    /*!
    tabBar data class needed for
    the rendering of tabbars when
    one tab is being drawn
    */
    class TabBarData: public QObject
    {

        public:

        //* constructor
        explicit TabBarData( Oxygen::Style* parent ):
            QObject( parent ),
            _style( parent )
        {}

        //* assign target tabBar
        void lock( const QWidget* widget )
        { _tabBar = widget; }

        //* true if tabbar is locked
        bool locks( const QWidget* widget ) const
        { return _tabBar && _tabBar.data() == widget; }

        //* set dirty
        void setDirty( const bool& value = true )
        { _dirty = value; }

        //* release
        void release( void )
        { _tabBar.clear(); }

        //* draw tabBarBase
        virtual void drawTabBarBaseControl( const QStyleOptionTab*, QPainter*, const QWidget* );

        private:

        //* pointer to parent style object
        Oxygen::WeakPointer<const Oxygen::Style> _style;

        //* pointer to target tabBar
        Oxygen::WeakPointer<const QWidget> _tabBar;

        //* if true, will paint on next TabBarTabShapeControl call
        bool _dirty = false;

    };

    //* needed to have spacing added to items in combobox
    class ComboBoxItemDelegate: public QItemDelegate
    {

        public:

        //* constructor
        ComboBoxItemDelegate( QAbstractItemView* parent ):
            QItemDelegate( parent ),
            _proxy( parent->itemDelegate() ),
            _itemMargin( Oxygen::Metrics::ItemView_ItemMarginWidth )
        {}

        //* paint
        void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            // call either proxy or parent class
            if( _proxy ) _proxy.data()->paint( painter, option, index );
            else QItemDelegate::paint( painter, option, index );
        }

        //* size hint for index
        QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const override
        {

            // get size from either proxy or parent class
            QSize size( _proxy ?
                _proxy.data()->sizeHint( option, index ):
                QItemDelegate::sizeHint( option, index ) );

            // adjust and return
            if( size.isValid() ) { size.rheight() += _itemMargin*2; }
            return size;

        }

        private:

        //* proxy
        Oxygen::WeakPointer<QAbstractItemDelegate> _proxy;

        //* margin
        int _itemMargin;

    };

}

namespace Oxygen
{

    //* toplevel manager
    class TopLevelManager: public QObject
    {
        public:

        //* constructor
        TopLevelManager( QObject* parent, const StyleHelper& helper ):
            QObject( parent ),
            _helper( helper )
        {}

        //* event filter
        bool eventFilter(QObject* object, QEvent* event ) override
        {

            // cast to QWidget
            QWidget *widget = static_cast<QWidget*>( object );
            if( event->type() == QEvent::Show && _helper.hasDecoration( widget ) )
            { _helper.setHasBackgroundGradient( widget->winId(), true ); }

            return false;
        }

        private:

        //* helper
        const StyleHelper& _helper;

    };

    //______________________________________________________________
    Style::Style( void ):
        _helper( new StyleHelper( StyleConfigData::self()->sharedConfig() ) )
        ,_shadowHelper( new ShadowHelper( this, *_helper ) )
        ,_animations( new Animations( this ) )
        ,_transitions( new Transitions( this ) )
        ,_windowManager( new WindowManager( this ) )
        ,_topLevelManager( new TopLevelManager( this, *_helper ) )
        ,_frameShadowFactory( new FrameShadowFactory( this ) )
        ,_mdiWindowShadowFactory( new MdiWindowShadowFactory( this, *_helper ) )
        ,_mnemonics( new Mnemonics( this ) )
        ,_blurHelper( new BlurHelper( this, *_helper ) )
        ,_widgetExplorer( new WidgetExplorer( this ) )
        ,_tabBarData( new OxygenPrivate::TabBarData( this ) )
        ,_splitterFactory( new SplitterFactory( this ) )
        ,SH_ArgbDndWindow( newStyleHint( QStringLiteral( "SH_ArgbDndWindow" ) ) )
        ,CE_CapacityBar( newControlElement( QStringLiteral( "CE_CapacityBar" ) ) )
    {

        // use DBus connection to update on oxygen configuration change
        QDBusConnection dbus = QDBusConnection::sessionBus();
        dbus.connect( QString(),
            QStringLiteral( "/OxygenStyle" ),
            QStringLiteral( "org.kde.Oxygen.Style" ),
            QStringLiteral( "reparseConfiguration" ), this, SLOT(configurationChanged()) );

        // call the slot directly; this initial call will set up things that also
        // need to be reset when the system palette changes
        loadConfiguration();

    }

    //______________________________________________________________
    Style::~Style( void )
    {
        // _shadowHelper is a child of us, but its destructor uses _helper so we
        // delete it manually to ensure it is deleted *before* _helper is
        // deleted
        delete _shadowHelper;
        delete _helper;
    }

    //______________________________________________________________
    void Style::polish( QWidget* widget )
    {
        if( !widget ) return;

        // register widget to animations
        _animations->registerWidget( widget );
        _transitions->registerWidget( widget );
        _windowManager->registerWidget( widget );
        _frameShadowFactory->registerWidget( widget, *_helper );
        _mdiWindowShadowFactory->registerWidget( widget );
        _shadowHelper->registerWidget( widget );
        _splitterFactory->registerWidget( widget );

        // scroll areas
        if( QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>( widget ) )
        { polishScrollArea( scrollArea ); }

        // several widgets set autofill background to false, which effectively breaks the background
        // gradient rendering. Instead of patching all concerned applications,
        // we change the background here
        if( widget->inherits( "MessageList::Core::Widget" ) )
        { widget->setAutoFillBackground( false ); }

        // adjust layout for K3B themed headers
        // FIXME: to be removed when fixed upstream
        if( widget->inherits( "K3b::ThemedHeader" ) && widget->layout() )
        {
            widget->layout()->setContentsMargins( 0, 0, 0, 0 );
            _frameShadowFactory->setHasContrast( widget, true );
        }

        // adjust flags for windows and dialogs
        switch( widget->windowFlags() & Qt::WindowType_Mask )
        {

            case Qt::Window:
            case Qt::Dialog:

            // set background as styled
            widget->setAttribute( Qt::WA_StyledBackground );
            widget->installEventFilter( _topLevelManager );

            break;

            default: break;

        }

        // enforce translucency for drag and drop window
        if( widget->testAttribute( Qt::WA_X11NetWmWindowTypeDND ) && _helper->compositingActive() )
        {
            widget->setAttribute( Qt::WA_TranslucentBackground );
            widget->clearMask();
        }

        if(
            qobject_cast<QAbstractItemView*>( widget )
            || qobject_cast<QAbstractSpinBox*>( widget )
            || qobject_cast<QCheckBox*>( widget )
            || qobject_cast<QComboBox*>( widget )
            || qobject_cast<QDial*>( widget )
            || qobject_cast<QLineEdit*>( widget )
            || qobject_cast<QPushButton*>( widget )
            || qobject_cast<QRadioButton*>( widget )
            || qobject_cast<QScrollBar*>( widget )
            || qobject_cast<QSlider*>( widget )
            || qobject_cast<QSplitterHandle*>( widget )
            || qobject_cast<QTabBar*>( widget )
            || qobject_cast<QTextEdit*>( widget )
            || qobject_cast<QToolButton*>( widget )
            || widget->inherits( "KTextEditor::View" )
            )
        { widget->setAttribute( Qt::WA_Hover ); }

        // transparent tooltips
        if( widget->inherits( "QTipLabel" ) )
        {
            widget->setAttribute( Qt::WA_TranslucentBackground );

            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif
        }

        if( QAbstractItemView *itemView = qobject_cast<QAbstractItemView*>( widget ) )
        {

            // enable hover effects in itemviews' viewport
            itemView->viewport()->setAttribute( Qt::WA_Hover );


        } else if( QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>( widget ) ) {

            // enable hover effect in sunken scrollareas that support focus
            if( scrollArea->frameShadow() == QFrame::Sunken && widget->focusPolicy()&Qt::StrongFocus )
            { widget->setAttribute( Qt::WA_Hover ); }

        } else if( QGroupBox* groupBox = qobject_cast<QGroupBox*>( widget ) )  {

            // checkable group boxes
            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover ); }

        } else if( qobject_cast<QAbstractButton*>( widget ) && qobject_cast<QDockWidget*>( widget->parent() ) ) {

            widget->setAttribute( Qt::WA_Hover );

        } else if( qobject_cast<QAbstractButton*>( widget ) && qobject_cast<QToolBox*>( widget->parent() ) ) {

            widget->setAttribute( Qt::WA_Hover );

        }

        if( qobject_cast<QToolButton*>( widget ) )
        {
            if( qobject_cast<QToolBar*>( widget->parent() ) )
            {
                // this hack is needed to have correct text color
                // rendered in toolbars. This does not really update nicely when changing styles
                // but is the best I can do for now since setting the palette color at painting
                // time is not doable
                QPalette palette( widget->palette() );
                palette.setColor( QPalette::Disabled, QPalette::ButtonText, palette.color( QPalette::Disabled, QPalette::WindowText ) );
                palette.setColor( QPalette::Active, QPalette::ButtonText, palette.color( QPalette::Active, QPalette::WindowText ) );
                palette.setColor( QPalette::Inactive, QPalette::ButtonText, palette.color( QPalette::Inactive, QPalette::WindowText ) );
                widget->setPalette( palette );
            }

            widget->setBackgroundRole( QPalette::NoRole );

            if( widget->parentWidget() &&
                widget->parentWidget()->parentWidget() &&
                widget->parentWidget()->parentWidget()->inherits( "Gwenview::SideBarGroup" ) )
            { widget->setProperty( PropertyNames::toolButtonAlignment, Qt::AlignLeft ); }

        } else if( qobject_cast<QMenuBar*>( widget ) ) {

            widget->setBackgroundRole( QPalette::NoRole );

        } else if( widget->inherits( "KMultiTabBar" ) ) {

            // kMultiTabBar margins are set to unity for alignment
            // with ( usually sunken ) neighbor frames
            widget->setContentsMargins( 1, 1, 1, 1 );

        } else if( qobject_cast<QToolBar*>( widget ) ) {

            widget->setBackgroundRole( QPalette::NoRole );
            addEventFilter( widget );

        } else if( qobject_cast<QTabBar*>( widget ) ) {

            addEventFilter( widget );

        } else if( widget->inherits( "QTipLabel" ) ) {

            widget->setBackgroundRole( QPalette::NoRole );
            widget->setAttribute( Qt::WA_TranslucentBackground );

            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif

        } else if( qobject_cast<QScrollBar*>( widget ) ) {

            widget->setAttribute( Qt::WA_OpaquePaintEvent, false );

            // when painted in konsole, one needs to paint the window background below
            // the scrollarea, otherwise an ugly flat background is used
            if( widget->parent() && widget->parent()->inherits( "Konsole::TerminalDisplay" ) )
            { addEventFilter( widget ); }

        } else if( qobject_cast<QDockWidget*>( widget ) ) {

            widget->setBackgroundRole( QPalette::NoRole );
            widget->setContentsMargins( 3,3,3,3 );
            addEventFilter( widget );

        } else if( qobject_cast<QMdiSubWindow*>( widget ) ) {

            widget->setAutoFillBackground( false );
            addEventFilter( widget );

        } else if( qobject_cast<QToolBox*>( widget ) ) {

            widget->setBackgroundRole( QPalette::NoRole );
            widget->setAutoFillBackground( false );
            widget->setContentsMargins( 5,5,5,5 );
            addEventFilter( widget );

        } else if( widget->parentWidget() && widget->parentWidget()->parentWidget() && qobject_cast<QToolBox*>( widget->parentWidget()->parentWidget()->parentWidget() ) ) {

            widget->setBackgroundRole( QPalette::NoRole );
            widget->setAutoFillBackground( false );
            widget->parentWidget()->setAutoFillBackground( false );

        } else if( qobject_cast<QMenu*>( widget ) ) {

            widget->setAttribute( Qt::WA_TranslucentBackground );
            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif

        } else if( qobject_cast<QCommandLinkButton*>( widget ) ) {

            addEventFilter( widget );

        } else if( QComboBox *comboBox = qobject_cast<QComboBox*>( widget ) ) {

            if( !hasParent( widget, "QWebView" ) )
            {
                QAbstractItemView *itemView( comboBox->view() );
                if( itemView && itemView->itemDelegate() && itemView->itemDelegate()->inherits( "QComboBoxDelegate" ) )
                { itemView->setItemDelegate( new OxygenPrivate::ComboBoxItemDelegate( itemView ) ); }
            }

        } else if( widget->inherits( "QComboBoxPrivateContainer" ) ) {

            addEventFilter( widget );
            widget->setAttribute( Qt::WA_TranslucentBackground );
            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif

        } else if( qobject_cast<QFrame*>( widget ) && widget->parent() && widget->parent()->inherits( "KTitleWidget" ) ) {

            widget->setAutoFillBackground( false );
            widget->setBackgroundRole( QPalette::Window );

        }

        // base class polishing
        KStyle::polish( widget );

    }

    //_______________________________________________________________
    void Style::unpolish( QWidget* widget )
    {

        // register widget to animations
        _animations->unregisterWidget( widget );
        _transitions->unregisterWidget( widget );
        _windowManager->unregisterWidget( widget );
        _frameShadowFactory->unregisterWidget( widget );
        _mdiWindowShadowFactory->unregisterWidget( widget );
        _shadowHelper->unregisterWidget( widget );
        _splitterFactory->unregisterWidget( widget );
        _blurHelper->unregisterWidget( widget );

        // event filters
        switch( widget->windowFlags() & Qt::WindowType_Mask )
        {

            case Qt::Window:
            case Qt::Dialog:
            widget->removeEventFilter( this );
            widget->setAttribute( Qt::WA_StyledBackground, false );
            break;

            default:
            break;

        }

        // checkable group boxes
        if( QGroupBox* groupBox = qobject_cast<QGroupBox*>( widget ) )
        {
            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover, false ); }
        }

        // hover flags
        if(
            qobject_cast<QAbstractItemView*>( widget )
            || qobject_cast<QAbstractSpinBox*>( widget )
            || qobject_cast<QCheckBox*>( widget )
            || qobject_cast<QComboBox*>( widget )
            || qobject_cast<QDial*>( widget )
            || qobject_cast<QLineEdit*>( widget )
            || qobject_cast<QPushButton*>( widget )
            || qobject_cast<QRadioButton*>( widget )
            || qobject_cast<QScrollBar*>( widget )
            || qobject_cast<QSlider*>( widget )
            || qobject_cast<QSplitterHandle*>( widget )
            || qobject_cast<QTabBar*>( widget )
            || qobject_cast<QTextEdit*>( widget )
            || qobject_cast<QToolButton*>( widget )
            )
        { widget->setAttribute( Qt::WA_Hover, false ); }

        // checkable group boxes
        if( QGroupBox* groupBox = qobject_cast<QGroupBox*>( widget ) )
        {
            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover, false ); }
        }

        if( qobject_cast<QMenuBar*>( widget )
            || qobject_cast<QToolBar*>( widget )
            || ( widget && qobject_cast<QToolBar *>( widget->parent() ) )
            || qobject_cast<QToolBox*>( widget ) )
        {
            widget->setBackgroundRole( QPalette::Button );
            widget->removeEventFilter( this );
            widget->clearMask();
        }

        if( qobject_cast<QTabBar*>( widget ) )
        {

            widget->removeEventFilter( this );

        } else if( widget->inherits( "QTipLabel" ) ) {

            widget->setAttribute( Qt::WA_PaintOnScreen, false );
            widget->setAttribute( Qt::WA_NoSystemBackground, false );
            widget->clearMask();

        } else if( qobject_cast<QScrollBar*>( widget ) ) {

            widget->setAttribute( Qt::WA_OpaquePaintEvent );

        } else if( qobject_cast<QDockWidget*>( widget ) ) {

            widget->setContentsMargins( 0,0,0,0 );
            widget->clearMask();

        } else if( qobject_cast<QToolBox*>( widget ) ) {

            widget->setBackgroundRole( QPalette::Button );
            widget->setContentsMargins( 0,0,0,0 );
            widget->removeEventFilter( this );

        } else if( qobject_cast<QMenu*>( widget ) ) {

            widget->setAttribute( Qt::WA_PaintOnScreen, false );
            widget->setAttribute( Qt::WA_NoSystemBackground, false );
            widget->clearMask();

        } else if( widget->inherits( "QComboBoxPrivateContainer" ) ) widget->removeEventFilter( this );

        KStyle::unpolish( widget );

    }

    //______________________________________________________________
    int Style::pixelMetric( PixelMetric metric, const QStyleOption* option, const QWidget* widget ) const
    {

        // handle special cases
        switch( metric )
        {

            case PM_DefaultFrameWidth:
            if( qobject_cast<const QLineEdit*>( widget ) ) return Metrics::LineEdit_FrameWidth;
            else if( isQtQuickControl( option, widget ) )
            {
                const QString &elementType = option->styleObject->property( "elementType" ).toString();
                if( elementType == QLatin1String( "edit" ) || elementType == QLatin1String( "spinbox" ) )
                {

                    return Metrics::LineEdit_FrameWidth;

                } else if( elementType == QLatin1String( "combobox" ) ) {

                    return Metrics::ComboBox_FrameWidth;

                }

            }

            // fallback
            return Metrics::Frame_FrameWidth;

            case PM_ComboBoxFrameWidth:
            {
                const QStyleOptionComboBox* comboBoxOption( qstyleoption_cast< const QStyleOptionComboBox*>( option ) );
                return comboBoxOption && comboBoxOption->editable ? Metrics::LineEdit_FrameWidth : Metrics::ComboBox_FrameWidth;
            }

            case PM_SpinBoxFrameWidth: return Metrics::SpinBox_FrameWidth;
            case PM_ToolBarFrameWidth: return Metrics::ToolBar_FrameWidth;
            case PM_ToolTipLabelFrameWidth: return Metrics::ToolTip_FrameWidth;

            // layout
            case PM_LayoutLeftMargin:
            case PM_LayoutTopMargin:
            case PM_LayoutRightMargin:
            case PM_LayoutBottomMargin:
            {
                /*
                use either Child margin or TopLevel margin,
                depending on  widget type
                */
                if( ( option && ( option->state & QStyle::State_Window ) ) || ( widget && widget->isWindow() ) )
                {

                    return Metrics::Layout_TopLevelMarginWidth;

                } else {

                    return Metrics::Layout_ChildMarginWidth;

                }

            }

            case PM_LayoutHorizontalSpacing: return Metrics::Layout_DefaultSpacing;
            case PM_LayoutVerticalSpacing: return Metrics::Layout_DefaultSpacing;

            // buttons
            case PM_ButtonMargin:
            {
                /* HACK: needs special case for kcalc buttons, to prevent the application to set too small margins */
                if( widget && widget->inherits( "KCalcButton" ) ) return Metrics::Button_MarginWidth + 4;
                else return Metrics::Button_MarginWidth;
            }

            // buttons
            case PM_ButtonDefaultIndicator: return 0;
            case PM_ButtonShiftHorizontal: return 0;
            case PM_ButtonShiftVertical: return 0;

            // menubars
            case PM_MenuBarPanelWidth: return 0;
            case PM_MenuBarHMargin: return 0;
            case PM_MenuBarVMargin: return 0;
            case PM_MenuBarItemSpacing: return 0;
            case PM_MenuDesktopFrameWidth: return 0;

            // menu buttons
            case PM_MenuButtonIndicator: return Metrics::MenuButton_IndicatorWidth;

            // toolbars
            case PM_ToolBarHandleExtent: return Metrics::ToolBar_HandleExtent;
            case PM_ToolBarSeparatorExtent: return Metrics::ToolBar_SeparatorWidth;
            case PM_ToolBarExtensionExtent:
            return pixelMetric( PM_SmallIconSize, option, widget ) + 2*Metrics::ToolButton_MarginWidth;

            case PM_ToolBarItemMargin: return 0;
            case PM_ToolBarItemSpacing: return Metrics::ToolBar_ItemSpacing;

            // tabbars
            case PM_TabBarTabShiftVertical: return 0;
            case PM_TabBarTabShiftHorizontal: return 0;
            case PM_TabBarTabOverlap: return Metrics::TabBar_TabOverlap;
            case PM_TabBarBaseOverlap: return Metrics::TabBar_BaseOverlap;
            case PM_TabBarTabHSpace: return 2*Metrics::TabBar_TabMarginWidth;
            case PM_TabBarTabVSpace: return 2*Metrics::TabBar_TabMarginHeight;
            case PM_TabCloseIndicatorWidth:
            case PM_TabCloseIndicatorHeight:
            return pixelMetric( PM_SmallIconSize, option, widget );

            // scrollbars
            case PM_ScrollBarExtent: return StyleConfigData::scrollBarWidth() + 2;
            case PM_ScrollBarSliderMin: return Metrics::ScrollBar_MinSliderHeight;

            // title bar
            case PM_TitleBarHeight: return 2*Metrics::TitleBar_MarginWidth + pixelMetric( PM_SmallIconSize, option, widget );

            // sliders
            case PM_SliderThickness: return Metrics::Slider_ControlThickness;
            case PM_SliderControlThickness: return Metrics::Slider_ControlThickness;
            case PM_SliderLength: return Metrics::Slider_ControlThickness;

            // checkboxes and radio buttons
            case PM_IndicatorWidth: return Metrics::CheckBox_Size;
            case PM_IndicatorHeight: return Metrics::CheckBox_Size;
            case PM_ExclusiveIndicatorWidth: return Metrics::CheckBox_Size;
            case PM_ExclusiveIndicatorHeight: return Metrics::CheckBox_Size;

            // list heaaders
            case PM_HeaderMarkSize: return Metrics::Header_ArrowSize;
            case PM_HeaderMargin: return Metrics::Header_MarginWidth;

            // dock widget
            // return 0 here, since frame is handled directly in polish
            case PM_DockWidgetFrameWidth: return 0;
            case PM_DockWidgetTitleMargin: return Metrics::Frame_FrameWidth;
            case PM_DockWidgetTitleBarButtonMargin: return Metrics::ToolButton_MarginWidth;

            case PM_SplitterWidth: return Metrics::Splitter_SplitterWidth;
            case PM_DockWidgetSeparatorExtent: return Metrics::Splitter_SplitterWidth;

            // spacing between widget and scrollbars
            case PM_ScrollView_ScrollBarSpacing:
            if( const QFrame* frame = qobject_cast<const QFrame*>( widget ) )
            {

                const bool framed( frame->frameShape() != QFrame::NoFrame );
                return framed ? -1:0;

            } else return -1;

            // fallback
            default: return KStyle::pixelMetric( metric, option, widget );

        }

    }

    //______________________________________________________________
    int Style::styleHint( StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData ) const
    {

        /*
        special cases, that cannot be registered in styleHint map,
        because of conditional statements
        */
        switch( hint )
        {

            case SH_RubberBand_Mask:
            {

                if( QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask*>( returnData ) )
                {

                    mask->region = option->rect;

                    // need to check on widget before removing inner region
                    // in order to still preserve rubberband in MainWindow and QGraphicsView
                    // in QMainWindow because it looks better
                    // in QGraphicsView because the painting fails completely otherwise
                    if( widget && (
                        qobject_cast<const QAbstractItemView*>( widget->parent() ) ||
                        qobject_cast<const QGraphicsView*>( widget->parent() ) ||
                        qobject_cast<const QMainWindow*>( widget->parent() ) ) )
                    { return true; }

                    // also check if widget's parent is some itemView viewport
                    if( widget && widget->parent() &&
                        qobject_cast<const QAbstractItemView*>( widget->parent()->parent() ) &&
                        static_cast<const QAbstractItemView*>( widget->parent()->parent() )->viewport() == widget->parent() )
                    { return true; }

                    // mask out center
                    mask->region -= insideMargin( option->rect, 1 );

                    return true;
                }
                return false;
            }

            case SH_ToolTip_Mask:
            case SH_Menu_Mask:
            {

                if( !_helper->hasAlphaChannel( widget ) && ( !widget || widget->isWindow() ) )
                {

                    // mask should be set only if compositing is disabled
                    if( QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>( returnData ) )
                    { mask->region = _helper->roundedMask( option->rect ); }

                }

                return true;

            }

            // mouse tracking
            case SH_ComboBox_ListMouseTracking: return true;
            case SH_MenuBar_MouseTracking: return true;
            case SH_Menu_MouseTracking: return true;
            case SH_Menu_SubMenuPopupDelay: return 150;
            case SH_Menu_SloppySubMenus: return true;
            case SH_ToolBox_SelectedPageTitleBold: return false;

            case SH_Widget_Animate: return StyleConfigData::animationsEnabled();
            case SH_Menu_SupportsSections: return true;

            case SH_TitleBar_NoBorder: return false;
            case SH_GroupBox_TextLabelVerticalAlignment: return Qt::AlignVCenter;
            case SH_ScrollBar_MiddleClickAbsolutePosition: return true;
            case SH_ScrollView_FrameOnlyAroundContents: return true;
            case SH_FormLayoutFormAlignment: return Qt::AlignLeft | Qt::AlignTop;
            case SH_FormLayoutLabelAlignment: return Qt::AlignRight;
            case SH_FormLayoutFieldGrowthPolicy: return QFormLayout::ExpandingFieldsGrow;
            case SH_FormLayoutWrapPolicy: return QFormLayout::DontWrapRows;
            case SH_MessageBox_TextInteractionFlags: return Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse;
            case SH_RequestSoftwareInputPanel: return RSIP_OnMouseClick;
            case SH_ProgressDialog_CenterCancelButton:
            case SH_MessageBox_CenterButtons:
            return false;

            default: return KStyle::styleHint( hint, option, widget, returnData );
        }

    }

    //______________________________________________________________
    QRect Style::subElementRect( SubElement element, const QStyleOption* option, const QWidget* widget ) const
    {

        switch( element )
        {

            case SE_PushButtonContents: return pushButtonContentsRect( option, widget );
            case SE_CheckBoxContents: return checkBoxContentsRect( option, widget );
            case SE_RadioButtonContents: return checkBoxContentsRect( option, widget );
            case SE_LineEditContents: return lineEditContentsRect( option, widget );
            case SE_ProgressBarGroove: return progressBarGrooveRect( option, widget );
            case SE_ProgressBarContents: return progressBarContentsRect( option, widget );
            case SE_ProgressBarLabel: return defaultSubElementRect( option, widget );
            case SE_HeaderArrow: return headerArrowRect( option, widget );
            case SE_HeaderLabel: return headerLabelRect( option, widget );
            case SE_TabBarTabLeftButton: return tabBarTabLeftButtonRect( option, widget );
            case SE_TabBarTabRightButton: return tabBarTabRightButtonRect( option, widget );
            case SE_TabWidgetTabBar: return tabWidgetTabBarRect( option, widget );
            case SE_TabWidgetTabContents: return tabWidgetTabContentsRect( option, widget );
            case SE_TabWidgetTabPane: return tabWidgetTabPaneRect( option, widget );
            case SE_TabWidgetLeftCorner: return tabWidgetCornerRect( SE_TabWidgetLeftCorner, option, widget );
            case SE_TabWidgetRightCorner: return tabWidgetCornerRect( SE_TabWidgetRightCorner, option, widget );
            case SE_ToolBoxTabContents: return toolBoxTabContentsRect( option, widget );

            default: return KStyle::subElementRect( element, option, widget );

        }

    }

    //______________________________________________________________
    QRect Style::subControlRect( ComplexControl element, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        switch( element )
        {

            case CC_GroupBox: return groupBoxSubControlRect( option, subControl, widget );
            case CC_ToolButton: return toolButtonSubControlRect( option, subControl, widget );
            case CC_ComboBox: return comboBoxSubControlRect( option, subControl, widget );
            case CC_SpinBox: return spinBoxSubControlRect( option, subControl, widget );
            case CC_ScrollBar: return scrollBarSubControlRect( option, subControl, widget );
            case CC_Slider: return sliderSubControlRect( option, subControl, widget );

            // fallback
            default: return KStyle::subControlRect( element, option, subControl, widget );

        }

    }

    //______________________________________________________________
    QSize Style::sizeFromContents( ContentsType element, const QStyleOption* option, const QSize& size, const QWidget* widget ) const
    {

        switch( element )
        {
            case CT_CheckBox: return checkBoxSizeFromContents( option, size, widget );
            case CT_RadioButton: return checkBoxSizeFromContents( option, size, widget );
            case CT_LineEdit: return lineEditSizeFromContents( option, size, widget );
            case CT_ComboBox: return comboBoxSizeFromContents( option, size, widget );
            case CT_SpinBox: return spinBoxSizeFromContents( option, size, widget );
            case CT_Slider: return sliderSizeFromContents( option, size, widget );
            case CT_PushButton: return pushButtonSizeFromContents( option, size, widget );
            case CT_ToolButton: return toolButtonSizeFromContents( option, size, widget );
            case CT_MenuBar: return defaultSizeFromContents( option, size, widget );
            case CT_MenuBarItem: return menuBarItemSizeFromContents( option, size, widget );
            case CT_MenuItem: return menuItemSizeFromContents( option, size, widget );
            case CT_TabWidget: return tabWidgetSizeFromContents( option, size, widget );
            case CT_TabBarTab: return tabBarTabSizeFromContents( option, size, widget );
            case CT_HeaderSection: return headerSectionSizeFromContents( option, size, widget );
            case CT_ItemViewItem: return itemViewItemSizeFromContents( option, size, widget );

            default: return KStyle::sizeFromContents( element, option, size, widget );
        }

    }

    //______________________________________________________________
    QStyle::SubControl Style::hitTestComplexControl( ComplexControl control, const QStyleOptionComplex* option, const QPoint& point, const QWidget* widget ) const
    {
        switch( control )
        {
            case CC_ScrollBar:
            {

                QRect grooveRect = subControlRect( CC_ScrollBar, option, SC_ScrollBarGroove, widget );
                if( grooveRect.contains( point ) )
                {
                    //Must be either page up/page down, or just click on the slider.
                    //Grab the slider to compare
                    QRect sliderRect = subControlRect( CC_ScrollBar, option, SC_ScrollBarSlider, widget );

                    if( sliderRect.contains( point ) ) return SC_ScrollBarSlider;
                    else if( preceeds( point, sliderRect, option ) ) return SC_ScrollBarSubPage;
                    else return SC_ScrollBarAddPage;

                }

                //This is one of the up/down buttons. First, decide which one it is.
                if( preceeds( point, grooveRect, option ) )
                {

                    if( _subLineButtons == DoubleButton )
                    {
                        QRect buttonRect = scrollBarInternalSubControlRect( option, SC_ScrollBarSubLine );
                        return scrollBarHitTest( buttonRect, point, option );
                    } else return SC_ScrollBarSubLine;

                }

                if( _addLineButtons == DoubleButton )
                {

                    QRect buttonRect = scrollBarInternalSubControlRect( option, SC_ScrollBarAddLine );
                    return scrollBarHitTest( buttonRect, point, option );

                } else return SC_ScrollBarAddLine;
            }

            // fallback
            default: return KStyle::hitTestComplexControl( control, option, point, widget );

        }

    }

    //______________________________________________________________
    void Style::drawPrimitive( PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        StylePrimitive fcn( nullptr );
        switch( element )
        {

            case PE_PanelButtonCommand: fcn = &Style::drawPanelButtonCommandPrimitive; break;
            case PE_PanelButtonTool: fcn = &Style::drawPanelButtonToolPrimitive; break;
            case PE_PanelScrollAreaCorner: fcn = &Style::drawPanelScrollAreaCornerPrimitive; break;
            case PE_PanelMenu: fcn = &Style::drawPanelMenuPrimitive; break;
            case PE_PanelTipLabel: fcn = &Style::drawPanelTipLabelPrimitive; break;
            case PE_PanelItemViewItem: fcn = &Style::drawPanelItemViewItemPrimitive; break;
            case PE_IndicatorCheckBox: fcn = &Style::drawIndicatorCheckBoxPrimitive; break;
            case PE_IndicatorRadioButton: fcn = &Style::drawIndicatorRadioButtonPrimitive; break;
            case PE_IndicatorButtonDropDown: fcn = &Style::drawIndicatorButtonDropDownPrimitive; break;
            case PE_IndicatorTabClose: fcn = &Style::drawIndicatorTabClosePrimitive; break;
            case PE_IndicatorTabTear: fcn = &Style::drawIndicatorTabTearPrimitive; break;
            case PE_IndicatorArrowUp: fcn = &Style::drawIndicatorArrowUpPrimitive; break;
            case PE_IndicatorArrowDown: fcn = &Style::drawIndicatorArrowDownPrimitive; break;
            case PE_IndicatorArrowLeft: fcn = &Style::drawIndicatorArrowLeftPrimitive; break;
            case PE_IndicatorArrowRight: fcn = &Style::drawIndicatorArrowRightPrimitive; break;
            case PE_IndicatorMenuCheckMark: fcn = &Style::drawIndicatorMenuCheckMarkPrimitive; break;
            case PE_IndicatorHeaderArrow: fcn = &Style::drawIndicatorHeaderArrowPrimitive; break;
            case PE_IndicatorToolBarHandle: fcn = &Style::drawIndicatorToolBarHandlePrimitive; break;
            case PE_IndicatorToolBarSeparator: fcn = &Style::drawIndicatorToolBarSeparatorPrimitive; break;
            case PE_IndicatorBranch: fcn = &Style::drawIndicatorBranchPrimitive; break;
            case PE_IndicatorDockWidgetResizeHandle: fcn = &Style::drawIndicatorDockWidgetResizeHandlePrimitive; break;
            case PE_FrameStatusBar: fcn = &Style::emptyPrimitive; break;
            case PE_Frame: fcn = &Style::drawFramePrimitive; break;
            case PE_FrameLineEdit: fcn = &Style::drawFrameLineEditPrimitive; break;
            case PE_FrameMenu: fcn = &Style::drawFrameMenuPrimitive; break;
            case PE_FrameGroupBox: fcn = &Style::drawFrameGroupBoxPrimitive; break;
            case PE_FrameTabWidget: fcn = &Style::drawFrameTabWidgetPrimitive; break;
            case PE_FrameTabBarBase: fcn = &Style::drawFrameTabBarBasePrimitive; break;
            case PE_FrameWindow: fcn = &Style::drawFrameWindowPrimitive; break;
            case PE_FrameFocusRect: fcn = _frameFocusPrimitive; break;
            case PE_Widget: fcn = &Style::drawWidgetPrimitive; break;

            // fallback
            default: break;

        }

        painter->save();

        // call function if implemented
        if( !( fcn && ( this->*fcn )( option, painter, widget ) ) )
        { KStyle::drawPrimitive( element, option, painter, widget ); }

        painter->restore();

    }

    //______________________________________________________________
    void Style::drawControl( ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        StyleControl fcn( nullptr );
        if( element == CE_CapacityBar )
        {

            fcn = &Style::drawProgressBarControl;

        } else
        switch( element ) {

            case CE_ComboBoxLabel: break;
            case CE_DockWidgetTitle: fcn = &Style::drawDockWidgetTitleControl; break;
            case CE_HeaderEmptyArea: fcn = &Style::drawHeaderEmptyAreaControl; break;
            case CE_HeaderLabel: break;
            case CE_HeaderSection: fcn = &Style::drawHeaderSectionControl; break;
            case CE_MenuBarEmptyArea: fcn = &Style::emptyControl; break;
            case CE_MenuBarItem: fcn = &Style::drawMenuBarItemControl; break;
            case CE_MenuItem: fcn = &Style::drawMenuItemControl; break;
            case CE_ProgressBar: fcn = &Style::drawProgressBarControl; break;
            case CE_ProgressBarContents: fcn = &Style::drawProgressBarContentsControl; break;
            case CE_ProgressBarGroove: fcn = &Style::drawProgressBarGrooveControl; break;
            case CE_ProgressBarLabel: fcn = &Style::drawProgressBarLabelControl; break;
            case CE_PushButtonBevel: fcn = &Style::drawPanelButtonCommandPrimitive; break;
            case CE_PushButtonLabel: fcn = &Style::drawPushButtonLabelControl; break;
            case CE_RubberBand: fcn = &Style::drawRubberBandControl; break;
            case CE_ScrollBarSlider: fcn = &Style::drawScrollBarSliderControl; break;
            case CE_ScrollBarAddLine: fcn = &Style::drawScrollBarAddLineControl; break;
            case CE_ScrollBarSubLine: fcn = &Style::drawScrollBarSubLineControl; break;
            case CE_ScrollBarAddPage: fcn = &Style::emptyControl; break;
            case CE_ScrollBarSubPage: fcn = &Style::emptyControl; break;
            case CE_ShapedFrame: fcn = &Style::drawShapedFrameControl; break;
            case CE_SizeGrip: fcn = &Style::emptyControl; break;
            case CE_Splitter: fcn = &Style::drawSplitterControl; break;
            case CE_TabBarTabLabel: fcn = &Style::drawTabBarTabLabelControl; break;
            case CE_TabBarTabShape: fcn = &Style::drawTabBarTabShapeControl; break;
            case CE_ToolBar: fcn = &Style::drawToolBarControl; break;
            case CE_ToolBoxTabLabel: fcn = &Style::drawToolBoxTabLabelControl; break;
            case CE_ToolBoxTabShape: fcn = &Style::drawToolBoxTabShapeControl; break;
            case CE_ToolButtonLabel: fcn = &Style::drawToolButtonLabelControl; break;

            default: break;

        }

        painter->save();

        // call function if implemented
        if( !( fcn && ( this->*fcn )( option, painter, widget ) ) )
        { KStyle::drawControl( element, option, painter, widget ); }

        painter->restore();

    }

    //______________________________________________________________
    void Style::drawComplexControl( ComplexControl element, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        StyleComplexControl fcn( nullptr );
        switch( element )
        {
            case CC_GroupBox: break;
            case CC_ToolButton: fcn = &Style::drawToolButtonComplexControl; break;
            case CC_ComboBox: fcn = &Style::drawComboBoxComplexControl; break;
            case CC_SpinBox: fcn = &Style::drawSpinBoxComplexControl; break;
            case CC_Slider: fcn = &Style::drawSliderComplexControl; break;
            case CC_Dial: fcn = &Style::drawDialComplexControl; break;
            case CC_ScrollBar: fcn = &Style::drawScrollBarComplexControl; break;
            case CC_TitleBar: fcn = &Style::drawTitleBarComplexControl; break;

            // fallback
            default: break;
        }

        painter->save();

        // call function if implemented
        if( !( fcn && ( this->*fcn )( option, painter, widget ) ) )
        { KStyle::drawComplexControl( element, option, painter, widget ); }

        painter->restore();

    }


    //___________________________________________________________________________________
    void Style::drawItemText(
        QPainter* painter, const QRect& rect, int flags, const QPalette& palette, bool enabled,
        const QString &text, QPalette::ColorRole textRole ) const
    {

        // hide mnemonics if requested
        if( !_mnemonics->enabled() && ( flags&Qt::TextShowMnemonic ) && !( flags&Qt::TextHideMnemonic ) )
        {
            flags &= ~Qt::TextShowMnemonic;
            flags |= Qt::TextHideMnemonic;
        }

        // make sure vertical alignment is defined
        // fallback on Align::VCenter if not
        if( !(flags&Qt::AlignVertical_Mask) ) flags |= Qt::AlignVCenter;

        if( _animations->widgetEnableStateEngine().enabled() )
        {

            /*
            check if painter engine is registered to widgetEnableStateEngine, and animated
            if yes, merge the palettes. Note: a static_cast is safe here, since only the address
            of the pointer is used, not the actual content.
            */
            const QWidget* widget( static_cast<const QWidget*>( painter->device() ) );
            if( _animations->widgetEnableStateEngine().isAnimated( widget, AnimationEnable ) )
            {

                const QPalette copy( _helper->disabledPalette( palette, _animations->widgetEnableStateEngine().opacity( widget, AnimationEnable ) ) );
                return KStyle::drawItemText( painter, rect, flags, copy, enabled, text, textRole );

            }

        }

        // fallback
        return KStyle::drawItemText( painter, rect, flags, palette, enabled, text, textRole );

    }


    //_____________________________________________________________________
    bool Style::eventFilter( QObject *object, QEvent *event )
    {

        if( QTabBar* tabBar = qobject_cast<QTabBar*>( object ) ) { return eventFilterTabBar( tabBar, event ); }
        else if( QToolBar* toolBar = qobject_cast<QToolBar*>( object ) ) { return eventFilterToolBar( toolBar, event ); }
        else if( QDockWidget* dockWidget = qobject_cast<QDockWidget*>( object ) ) { return eventFilterDockWidget( dockWidget, event ); }
        else if( QToolBox* toolBox = qobject_cast<QToolBox*>( object ) ) { return eventFilterToolBox( toolBox, event ); }
        else if( QMdiSubWindow* subWindow = qobject_cast<QMdiSubWindow*>( object ) ) { return eventFilterMdiSubWindow( subWindow, event ); }
        else if( QScrollBar* scrollBar = qobject_cast<QScrollBar*>( object ) ) { return eventFilterScrollBar( scrollBar, event ); }
        else if( QCommandLinkButton* commandLinkButton = qobject_cast<QCommandLinkButton*>( object ) ) { return eventFilterCommandLinkButton( commandLinkButton, event ); }

        // cast to QWidget
        QWidget *widget = static_cast<QWidget*>( object );
        if( widget->inherits( "QComboBoxPrivateContainer" ) ) { return eventFilterComboBoxContainer( widget, event ); }

        // fallback
        return KStyle::eventFilter( object, event );

    }

    //_________________________________________________________
    bool Style::eventFilterComboBoxContainer( QWidget* widget, QEvent* event )
    {
        switch( event->type() )
        {

            case QEvent::Show:
            case QEvent::Resize:
            {
                if( !_helper->hasAlphaChannel( widget ) ) widget->setMask( _helper->roundedMask( widget->size() ) );
                else widget->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                QPainter painter( widget );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const QRect rect( widget->rect() );
                const QColor color( widget->palette().color( widget->window()->backgroundRole() ) );
                const bool hasAlpha( _helper->hasAlphaChannel( widget ) );

                if( hasAlpha )
                {

                    _helper->roundCorner( color ).render( rect, &painter );
                    painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                    painter.setClipPath( _helper->roundedPath( insideMargin( rect, 1 ) ), Qt::IntersectClip );

                }

                // background
                _helper->renderMenuBackground( &painter, paintEvent->rect(), widget, widget->palette() );

                // frame
                if( hasAlpha ) painter.setClipping( false );

                _helper->drawFloatFrame( &painter, rect, color, !hasAlpha );
                return false;

            }
            default: return false;
        }
    }

    //____________________________________________________________________________
    bool Style::eventFilterDockWidget( QDockWidget* dockWidget, QEvent* event )
    {
        switch( event->type() )
        {
            case QEvent::Show:
            case QEvent::Resize:
            {
                // make sure mask is appropriate
                if( dockWidget->isFloating() ) dockWidget->setMask( _helper->roundedMask( dockWidget->size() ) );
                else dockWidget->clearMask();
                return false;
            }

            case QEvent::Paint:
            {
                QPainter painter( dockWidget );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const QColor color( dockWidget->palette().color( QPalette::Window ) );
                const QRect rect( dockWidget->rect() );
                if( dockWidget->isWindow() )
                {

                    _helper->renderWindowBackground( &painter, rect, dockWidget, color, 0 );

                    #ifndef Q_WS_WIN
                    _helper->drawFloatFrame( &painter, rect, color, !_helper->compositingActive() );
                    #endif

                } else {

                    // need to draw window background for autoFilled dockWidgets for better rendering
                    if( dockWidget->autoFillBackground() )
                    { _helper->renderWindowBackground( &painter, rect, dockWidget, color ); }

                    // adjust color
                    QColor top( _helper->backgroundColor( color, dockWidget, rect.topLeft() ) );
                    QColor bottom( _helper->backgroundColor( color, dockWidget, rect.bottomLeft() ) );
                    _helper->dockFrame( top, bottom ).render( rect, &painter );

                }

                return false;
            }

            default: return false;

        }

    }

    //____________________________________________________________________________
    bool Style::eventFilterMdiSubWindow( QMdiSubWindow* subWindow, QEvent* event )
    {

        if( event->type() == QEvent::Paint )
        {

            QPainter painter( subWindow );
            QRect clip( static_cast<QPaintEvent*>( event )->rect() );
            if( subWindow->isMaximized() ) _helper->renderWindowBackground( &painter, clip, subWindow, subWindow->palette() );
            else {

                painter.setClipRect( clip );

                const QRect rect( subWindow->rect() );
                _helper->roundCorner( subWindow->palette().color( subWindow->backgroundRole() ) ).render( rect, &painter );

                painter.setClipPath( _helper->roundedPath( insideMargin( rect, 1 ) ), Qt::IntersectClip );
                _helper->renderWindowBackground( &painter, clip, subWindow, subWindow, subWindow->palette(), 0 );

            }

        }

        // continue with normal painting
        return false;

    }

    //____________________________________________________________________________
    bool Style::eventFilterCommandLinkButton( QCommandLinkButton* button, QEvent* event )
    {

        if( event->type() == QEvent::Paint )
        {

            // painter
            QPainter painter( button );
            painter.setClipRegion( static_cast<QPaintEvent*>( event )->region() );

            const bool isFlat = false;

            // option
            QStyleOptionButton option;
            option.initFrom( button );
            option.features |= QStyleOptionButton::CommandLinkButton;
            if( isFlat ) option.features |= QStyleOptionButton::Flat;
            option.text = QString();
            option.icon = QIcon();

            if( button->isChecked() ) option.state|=State_On;
            if( button->isDown() ) option.state|=State_Sunken;

            // frame
            drawControl(QStyle::CE_PushButton, &option, &painter, button );

            // offset
            const int margin( Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth );
            QPoint offset( margin, margin );

            // state
            const State& state( option.state );
            const bool enabled( state & State_Enabled );

            // icon
            if( !button->icon().isNull() )
            {

                const QSize pixmapSize( button->icon().actualSize( button->iconSize() ) );
                const QRect pixmapRect( QPoint( offset.x(), button->description().isEmpty() ? (button->height() - pixmapSize.height())/2:offset.y() ), pixmapSize );
                const QPixmap pixmap( button->icon().pixmap(pixmapSize,
                    enabled ? QIcon::Normal : QIcon::Disabled,
                    button->isChecked() ? QIcon::On : QIcon::Off) );
                drawItemPixmap( &painter, pixmapRect, Qt::AlignCenter, pixmap );

                offset.rx() += pixmapSize.width() + 4;

            }

            // text rect
            QRect textRect( offset, QSize( button->size().width() - offset.x() - margin, button->size().height() - 2*margin ) );
            const QPalette::ColorRole textRole = QPalette::ButtonText;
            if( !button->text().isEmpty() )
            {

                QFont font( button->font() );
                font.setBold( true );
                painter.setFont( font );
                if( button->description().isEmpty() )
                {

                    drawItemText( &painter, textRect, Qt::AlignLeft|Qt::AlignVCenter|Qt::TextHideMnemonic, button->palette(), enabled, button->text(), textRole );

                } else {

                    drawItemText( &painter, textRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextHideMnemonic, button->palette(), enabled, button->text(), textRole );
                    textRect.setTop( textRect.top() + QFontMetrics( font ).height() );

                }

                painter.setFont( button->font() );

            }

            if( !button->description().isEmpty() )
            { drawItemText( &painter, textRect, Qt::AlignLeft|Qt::AlignVCenter|Qt::TextWordWrap, button->palette(), enabled, button->description(), textRole ); }

            return true;
        }

        // continue with normal painting
        return false;

    }

    //_________________________________________________________
    bool Style::eventFilterScrollBar( QWidget* widget, QEvent* event )
    {

        if( event->type() == QEvent::Paint )
        {
            QPainter painter( widget );
            painter.setClipRegion( static_cast<QPaintEvent*>( event )->region() );
            _helper->renderWindowBackground( &painter, widget->rect(), widget,widget->palette() );
        }

        return false;
    }

    //_____________________________________________________________________
    bool Style::eventFilterTabBar( QWidget* widget, QEvent* event )
    {
        if( event->type() == QEvent::Paint && _tabBarData->locks( widget ) )
        {
            /*
            this makes sure that tabBar base is drawn ( and drawn only once )
            every time a replaint is triggered by dragging a tab around
            */
            _tabBarData->setDirty();
        }

        return false;
    }

    //_____________________________________________________________________
    bool Style::eventFilterToolBar( QToolBar* toolBar, QEvent* event )
    {
        switch( event->type() )
        {
            case QEvent::Show:
            case QEvent::Resize:
            {

                // make sure mask is appropriate
                if( toolBar->isFloating() )
                {

                    // TODO: should not be needed
                    toolBar->setMask( _helper->roundedMask( toolBar->size() ) );

                } else toolBar->clearMask();
                return false;

            }

            case QEvent::Paint:
            {

                QPainter painter( toolBar );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const QRect rect( toolBar->rect() );
                const QColor color( toolBar->palette().window().color() );

                // default painting when not qrealing
                if( !toolBar->isFloating() )
                {

                    // background has to be rendered explicitly
                    // when one of the parent has autofillBackground set to true
                    if( _helper->checkAutoFillBackground( toolBar ) )
                    { _helper->renderWindowBackground( &painter, rect, toolBar, color, 0 ); }

                    return false;

                } else {

                    // background
                    _helper->renderWindowBackground( &painter, rect, toolBar, color );

                    if( toolBar->isMovable() )
                    {
                        // remaining painting: need to add handle
                        // this is copied from QToolBar::paintEvent
                        QStyleOptionToolBar opt;
                        opt.initFrom( toolBar );
                        if( toolBar->orientation() == Qt::Horizontal )
                        {

                            opt.rect = visualRect( &opt, QRect( rect.topLeft(), QSize( 8, rect.height() ) ) );
                            opt.state |= QStyle::State_Horizontal;

                        } else {

                            opt.rect = visualRect( &opt, QRect( rect.topLeft(), QSize( rect.width(), 8 ) ) );

                        }

                        drawPrimitive( PE_IndicatorToolBarHandle, &opt, &painter, toolBar );

                    }

                    #ifndef Q_WS_WIN
                    if( _helper->compositingActive() ) _helper->drawFloatFrame( &painter, insideMargin( rect, -1 ), color, false );
                    else _helper->drawFloatFrame( &painter, rect, color, true );
                    #endif

                    // do not propagate
                    return true;

                }

            }
            default: return false;
        }

    }

    //____________________________________________________________________________
    bool Style::eventFilterToolBox( QToolBox* toolBox, QEvent* event )
    {

        if( event->type() == QEvent::Paint )
        {
            if( toolBox->frameShape() != QFrame::NoFrame )
            {

                const QRect rect( toolBox->rect() );
                const StyleOptions styleOptions( NoFill );

                QPainter painter( toolBox );
                painter.setClipRegion( static_cast<QPaintEvent*>( event )->region() );
                renderSlab( &painter, rect, toolBox->palette().color( QPalette::Button ), styleOptions );

            }
        }

        return false;
    }

    //_____________________________________________________________________
    void Style::configurationChanged()
    {

        // reload
        StyleConfigData::self()->load();

        _shadowHelper->reparseCacheConfig();

        _helper->invalidateCaches();

        loadConfiguration();
    }

    //____________________________________________________________________
    QIcon Style::standardIconImplementation(
        StandardPixmap standardPixmap,
        const QStyleOption *option,
        const QWidget *widget ) const
    {

        // MDI windows buttons
        // get button color ( unfortunately option and widget might not be set )
        QColor buttonColor;
        QColor iconColor;
        if( option )
        {

            buttonColor = option->palette.window().color();
            iconColor   = option->palette.windowText().color();

        } else if( widget ) {

            buttonColor = widget->palette().window().color();
            iconColor   = widget->palette().windowText().color();

        } else if( qApp ) {

            // might not have a QApplication
            buttonColor = QPalette().window().color();
            iconColor   = QPalette().windowText().color();

        } else {

            // KCS is always safe
            buttonColor = KColorScheme( QPalette::Active, KColorScheme::Window, _helper->config() ).background().color();
            iconColor   = KColorScheme( QPalette::Active, KColorScheme::Window, _helper->config() ).foreground().color();

        }

        // contrast
        const QColor contrast( _helper->calcLightColor( buttonColor ) );
        const int iconSize( pixelMetric( QStyle::PM_SmallIconSize ) );
        const QRect rect( 0, 0, iconSize, iconSize );

        switch( standardPixmap )
        {

            case SP_TitleBarNormalButton:
            {
                QPixmap pixmap( _helper->highDpiPixmap( rect.size() ) );

                pixmap.fill( Qt::transparent );

                QPainter painter( &pixmap );
                renderTitleBarButton( &painter, rect, buttonColor, iconColor, SC_TitleBarNormalButton );

                return QIcon( pixmap );

            }

            case SP_TitleBarShadeButton:
            {
                QPixmap pixmap( _helper->highDpiPixmap( rect.size() ) );

                pixmap.fill( Qt::transparent );
                QPainter painter( &pixmap );
                renderTitleBarButton( &painter, rect, buttonColor, iconColor, SC_TitleBarShadeButton );

                return QIcon( pixmap );
            }

            case SP_TitleBarUnshadeButton:
            {
                QPixmap pixmap( _helper->highDpiPixmap( rect.size() ) );

                pixmap.fill( Qt::transparent );
                QPainter painter( &pixmap );
                renderTitleBarButton( &painter, rect, buttonColor, iconColor, SC_TitleBarUnshadeButton );

                return QIcon( pixmap );
            }

            case SP_TitleBarCloseButton:
            case SP_DockWidgetCloseButton:
            {
                QPixmap pixmap( _helper->highDpiPixmap( rect.size() ) );

                pixmap.fill( Qt::transparent );
                QPainter painter( &pixmap );
                renderTitleBarButton( &painter, rect, buttonColor, iconColor, SC_TitleBarCloseButton );

                return QIcon( pixmap );

            }

            case SP_ToolBarHorizontalExtensionButton:
            {
                QIcon icon;

                // default icon sizes
                static const QList<int> iconSizes = { 8, 16, 22, 32, 48 };
                foreach( const int& iconSize, iconSizes )
                {

                    QPixmap pixmap(  iconSize, iconSize );
                    pixmap.fill( Qt::transparent );
                    QPainter painter( &pixmap );
                    painter.setRenderHints( QPainter::Antialiasing );
                    painter.setWindow( rect );
                    painter.setBrush( Qt::NoBrush );

                    painter.translate( QRectF( rect ).center() );

                    const bool reverseLayout( option && option->direction == Qt::RightToLeft );
                    QPolygonF arrow = genericArrow( reverseLayout ? ArrowLeft:ArrowRight, ArrowTiny );

                    const qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( _helper->calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( arrow );

                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( arrow );
                    painter.end();
                    icon.addPixmap( pixmap );

                }

                return icon;

            }

            case SP_ToolBarVerticalExtensionButton:
            {
                QIcon icon;

                // default icon sizes
                static const QList<int> iconSizes = { 8, 16, 22, 32, 48 };
                foreach( const int& iconSize, iconSizes )
                {
                    QPixmap pixmap( iconSize, iconSize );
                    pixmap.fill( Qt::transparent );
                    QPainter painter( &pixmap );
                    painter.setRenderHints( QPainter::Antialiasing );
                    painter.setWindow( rect );
                    painter.setBrush( Qt::NoBrush );

                    painter.translate( QRectF( rect ).center() );

                    QPolygonF arrow = genericArrow( ArrowDown, ArrowTiny );

                    const qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( _helper->calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( arrow );

                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( arrow );
                    painter.end();

                    icon.addPixmap( pixmap );

                }

                return icon;

            }

            default:
            // do not cache parent style icon, since it may change at runtime
            return  KStyle::standardIcon( standardPixmap, option, widget );

        }
    }


    //_____________________________________________________________________
    void Style::loadConfiguration()
    {
        // set helper configuration
        _helper->loadConfig();

        // background gradient
        _helper->setUseBackgroundGradient( StyleConfigData::useBackgroundGradient() );

        // update top level window hints
        foreach( QWidget* widget, qApp->topLevelWidgets() )
        {
            // make sure widget has a valid WId
            if( !(widget->testAttribute(Qt::WA_WState_Created) || widget->internalWinId() ) ) continue;

            // make sure widget has a decoration
            if( !_helper->hasDecoration( widget ) ) continue;

            // update flags
            _helper->setHasBackgroundGradient( widget->winId(), true );
        }

        // update caches size
        int cacheSize( StyleConfigData::cacheEnabled() ?
            StyleConfigData::maxCacheSize():0 );

        _helper->setMaxCacheSize( cacheSize );

        // always enable blur helper
        _blurHelper->setEnabled( true );

        // reinitialize engines
        _animations->setupEngines();
        _transitions->setupEngines();
        _windowManager->initialize();
        _shadowHelper->loadConfig();

        // mnemonics
        _mnemonics->setMode( StyleConfigData::mnemonicsMode() );

        // widget explorer
        _widgetExplorer->setEnabled( StyleConfigData::widgetExplorerEnabled() );
        _widgetExplorer->setDrawWidgetRects( StyleConfigData::drawWidgetRects() );

        // splitter proxy
        _splitterFactory->setEnabled( StyleConfigData::splitterProxyEnabled() );

        // scrollbar button dimentions.
        /* it has to be reinitialized here because scrollbar width might have changed */
        _noButtonHeight = 0;
        _singleButtonHeight = qMax( StyleConfigData::scrollBarWidth() * 7 / 10, 14 );
        _doubleButtonHeight = 2*_singleButtonHeight;

        // scrollbar buttons
        switch( StyleConfigData::scrollBarAddLineButtons() )
        {
            case 0: _addLineButtons = NoButton; break;
            case 1: _addLineButtons = SingleButton; break;

            default:
            case 2: _addLineButtons = DoubleButton; break;
        }

        switch( StyleConfigData::scrollBarSubLineButtons() )
        {
            case 0: _subLineButtons = NoButton; break;
            case 1: _subLineButtons = SingleButton; break;

            default:
            case 2: _subLineButtons = DoubleButton; break;
        }

        // frame focus
        if( StyleConfigData::viewDrawFocusIndicator() ) _frameFocusPrimitive = &Style::drawFrameFocusRectPrimitive;
        else _frameFocusPrimitive = &Style::emptyPrimitive;

    }

    //___________________________________________________________________________________________________________________
    QRect Style::pushButtonContentsRect( const QStyleOption* option, const QWidget* ) const
    { return insideMargin( option->rect, Metrics::Frame_FrameWidth ); }

    //___________________________________________________________________________________________________________________
    QRect Style::checkBoxContentsRect( const QStyleOption* option, const QWidget* ) const
    { return visualRect( option, option->rect.adjusted( Metrics::CheckBox_Size + Metrics::CheckBox_ItemSpacing, 0, 0, 0 ) ); }

    //___________________________________________________________________________________________________________________
    QRect Style::lineEditContentsRect( const QStyleOption* option, const QWidget* widget ) const
    {
        // cast option and check
        const QStyleOptionFrame* frameOption( qstyleoption_cast<const QStyleOptionFrame*>( option ) );
        if( !frameOption ) return option->rect;

        // check flatness
        const bool flat( frameOption->lineWidth == 0 );
        if( flat ) return option->rect;

        // copy rect and take out margins
        QRect rect( option->rect );

        // take out margins if there is enough room
        const int frameWidth( pixelMetric( PM_DefaultFrameWidth, option, widget ) );
        if( rect.height() > option->fontMetrics.height() + 2*frameWidth ) return insideMargin( rect, frameWidth );
        else return rect;
    }

    //____________________________________________________________________
    QRect Style::progressBarGrooveRect( const QStyleOption* option, const QWidget* ) const
    {
        const QRect rect( option->rect );

        const auto progressBarOption2( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
        const bool horizontal( !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal );
        if( horizontal ) return insideMargin( rect, 1, 0 );
        else return insideMargin( rect, 0, 1 );
    }

    //____________________________________________________________________
    QRect Style::progressBarContentsRect( const QStyleOption* option, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
        if( !progressBarOption ) return QRect();

        // get orientation
        const auto progressBarOption2( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
        const bool horizontal( !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal );

        // check inverted appearance
        const bool inverted( progressBarOption2 ? progressBarOption2->invertedAppearance : false );

        // get groove rect
        const QRect rect( progressBarGrooveRect( option, widget ) );

        // get progress
        qreal progress = progressBarOption->progress - progressBarOption->minimum;
        const bool busy = ( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );
        if( busy ) progress = _animations->busyIndicatorEngine().value();
        if( !( progress || busy ) ) return QRect();

        const int steps = qMax( progressBarOption->maximum  - progressBarOption->minimum, 1 );

        //Calculate width fraction
        qreal widthFrac( busy ?  qreal(Metrics::ProgressBar_BusyIndicatorSize)/100 : progress/steps );
        widthFrac = qMin( (qreal)1, widthFrac );

        // And now the pixel width
        const int indicatorSize( widthFrac*( horizontal ? rect.width():rect.height() ) );

        // do nothing if indicator size is too small
        if( indicatorSize < 4 ) return QRect();
        QRect indicatorRect;
        if( busy )
        {

            // The space around which we move around...
            int remSize = ( ( 1 - widthFrac )*( horizontal ? rect.width():rect.height() ) );
            remSize = qMax( remSize, 1 );

            int pstep =  remSize*2*progress;
            if( pstep > remSize )
            { pstep = -( pstep - 2*remSize ); }

            if( horizontal ) {

                indicatorRect = QRect( inverted ? (rect.right() - pstep - indicatorSize + 1) : (rect.left() + pstep), rect.top(), indicatorSize, rect.height() );
                indicatorRect = visualRect( option->direction, rect, indicatorRect );

            } else {

                indicatorRect = QRect( rect.left(), inverted ? (rect.bottom() - pstep - indicatorSize + 1) : (rect.top() + pstep), rect.width(), indicatorSize );

            }

        } else {

            if( horizontal )
            {

                indicatorRect = QRect( inverted ? (rect.right() - indicatorSize + 1) : rect.left(), rect.top(), indicatorSize, rect.height() );
                indicatorRect = visualRect( option->direction, rect, indicatorRect );

            } else {

                indicatorRect = QRect( rect.left(), inverted ? rect.top() : (rect.bottom()- indicatorSize + 1), rect.width(), indicatorSize );

            }
        }

        // adjust
        return insideMargin( indicatorRect, 1 );

    }

    //___________________________________________________________________________________________________________________
    QRect Style::headerArrowRect( const QStyleOption* option, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionHeader* headerOption( qstyleoption_cast<const QStyleOptionHeader*>( option ) );
        if( !headerOption ) return option->rect;

        // check if arrow is necessary
        if( headerOption->sortIndicator == QStyleOptionHeader::None ) return QRect();

        QRect arrowRect( insideMargin( option->rect, Metrics::Header_MarginWidth ) );
        arrowRect.setLeft( arrowRect.right() - Metrics::Header_ArrowSize + 1 );

        return visualRect( option, arrowRect );

    }

    //___________________________________________________________________________________________________________________
    QRect Style::headerLabelRect( const QStyleOption* option, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionHeader* headerOption( qstyleoption_cast<const QStyleOptionHeader*>( option ) );
        if( !headerOption ) return option->rect;

        // check if arrow is necessary
        // QRect labelRect( insideMargin( option->rect, Metrics::Header_MarginWidth ) );
        QRect labelRect( insideMargin( option->rect, Metrics::Header_MarginWidth, 0 ) );
        if( headerOption->sortIndicator == QStyleOptionHeader::None ) return labelRect;

        labelRect.adjust( 0, 0, -Metrics::Header_ArrowSize-Metrics::Header_ItemSpacing, 0 );
        return visualRect( option, labelRect );

    }

    //____________________________________________________________________
    QRect Style::tabBarTabLeftButtonRect( const QStyleOption* option, const QWidget* ) const
    {

        // cast option and check
        const auto tabOptionV3( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOptionV3 || tabOptionV3->leftButtonSize.isEmpty() ) return QRect();

        const QRect rect( option->rect );
        const QSize size( tabOptionV3->leftButtonSize );
        QRect buttonRect( QPoint(0,0), size );

        // vertical positioning
        switch( tabOptionV3->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            buttonRect.moveLeft( rect.left() + Metrics::TabBar_TabMarginWidth );
            buttonRect.moveTop( ( rect.height() - buttonRect.height() )/2 );
            buttonRect = visualRect( option, buttonRect );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            buttonRect.moveBottom( rect.bottom() - Metrics::TabBar_TabMarginWidth );
            buttonRect.moveLeft( ( rect.width() - buttonRect.width() )/2 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            buttonRect.moveTop( rect.top() + Metrics::TabBar_TabMarginWidth );
            buttonRect.moveLeft( ( rect.width() - buttonRect.width() )/2 );
            break;

            default: break;
        }

        return buttonRect;

    }

    //____________________________________________________________________
    QRect Style::tabBarTabRightButtonRect( const QStyleOption* option, const QWidget* ) const
    {

        // cast option and check
        const auto tabOptionV3( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOptionV3 || tabOptionV3->rightButtonSize.isEmpty() ) return QRect();

        const QRect rect( option->rect );
        const QSize size( tabOptionV3->rightButtonSize );
        QRect buttonRect( QPoint(0,0), size );

        // vertical positioning
        switch( tabOptionV3->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            buttonRect.moveRight( rect.right() - Metrics::TabBar_TabMarginWidth );
            buttonRect.moveTop( ( rect.height() - buttonRect.height() )/2 );
            buttonRect = visualRect( option, buttonRect );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            buttonRect.moveTop( rect.top() + Metrics::TabBar_TabMarginWidth );
            buttonRect.moveLeft( ( rect.width() - buttonRect.width() )/2 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            buttonRect.moveBottom( rect.bottom() - Metrics::TabBar_TabMarginWidth );
            buttonRect.moveLeft( ( rect.width() - buttonRect.width() )/2 );
            break;

            default: break;
        }

        return buttonRect;

    }

    //____________________________________________________________________
    QRect Style::tabWidgetTabBarRect( const QStyleOption* option, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionTabWidgetFrame* tabOption = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option );
        if( !tabOption ) return KStyle::subElementRect( SE_TabWidgetTabBar, option, widget );

        // do nothing if tabbar is hidden
        const QSize tabBarSize( tabOption->tabBarSize );

        QRect rect( option->rect );
        QRect tabBarRect( QPoint(0, 0), tabBarSize );

        // horizontal positioning
        const bool verticalTabs( isVerticalTab( tabOption->shape ) );
        if( verticalTabs )
        {

            tabBarRect.setHeight( qMin( tabBarRect.height(), rect.height() - 2 ) );
            tabBarRect.moveTop( rect.top()+1 );

        } else {

            // account for corner rects
            // need to re-run visualRect to remove right-to-left handling, since it is re-added on tabBarRect at the end
            const QRect leftButtonRect( visualRect( option, subElementRect( SE_TabWidgetLeftCorner, option, widget ) ) );
            const QRect rightButtonRect( visualRect( option, subElementRect( SE_TabWidgetRightCorner, option, widget ) ) );

            rect.setLeft( leftButtonRect.width() );
            rect.setRight( rightButtonRect.left() - 1 );

            tabBarRect.setWidth( qMin( tabBarRect.width(), rect.width() - 2 ) );
            tabBarRect.moveLeft( rect.left() + 1 );

            tabBarRect = visualRect( option, tabBarRect );

        }

        // vertical positioning
        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            tabBarRect.moveTop( rect.top()+1 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            tabBarRect.moveBottom( rect.bottom()-1 );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            tabBarRect.moveLeft( rect.left()+1 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            tabBarRect.moveRight( rect.right()-1 );
            break;

            default: break;

        }

        return tabBarRect;

    }

    //____________________________________________________________________
    QRect Style::tabWidgetTabContentsRect( const QStyleOption* option, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionTabWidgetFrame* tabOption = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option );
        if( !tabOption ) return option->rect;

        // do nothing if tabbar is hidden
        if( tabOption->tabBarSize.isEmpty() ) return option->rect;
        const QRect rect = tabWidgetTabPaneRect( option, widget );

        const bool documentMode( tabOption->lineWidth == 0 );
        if( documentMode )
        {

            // add margin only to the relevant side
            switch( tabOption->shape )
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                return rect.adjusted( 0, Metrics::TabWidget_MarginWidth, 0, 0 );

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                return rect.adjusted( 0, 0, 0, -Metrics::TabWidget_MarginWidth );

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                return rect.adjusted( Metrics::TabWidget_MarginWidth, 0, 0, 0 );

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                return rect.adjusted( 0, 0, -Metrics::TabWidget_MarginWidth, 0 );

                default: return rect;
            }

        } else return insideMargin( rect, Metrics::TabWidget_MarginWidth );

    }

    //____________________________________________________________________
    QRect Style::tabWidgetTabPaneRect( const QStyleOption* option, const QWidget* ) const
    {

        const QStyleOptionTabWidgetFrame* tabOption = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option );
        if( !tabOption || tabOption->tabBarSize.isEmpty() ) return option->rect;

        const int overlap = Metrics::TabBar_BaseOverlap - 1;
        const QSize tabBarSize( tabOption->tabBarSize - QSize( overlap, overlap ) );

        QRect rect( option->rect );
        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            rect.adjust( 0, tabBarSize.height(), 0, 0 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            rect.adjust( 0, 0, 0, -tabBarSize.height() );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            rect.adjust( tabBarSize.width(), 0, 0, 0 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            rect.adjust( 0, 0, -tabBarSize.width(), 0 );
            break;

            default: break;
        }

        return rect;

    }

    //____________________________________________________________________
    QRect Style::tabWidgetCornerRect( SubElement element, const QStyleOption* option, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionTabWidgetFrame *tabOption = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>( option );
        if( !tabOption ) return QRect();

        // copy rect
        const QRect paneRect( subElementRect( SE_TabWidgetTabPane, option, widget ) );
        QRect rect;

        switch( element )
        {
            case SE_TabWidgetLeftCorner:
            rect = QRect( QPoint(0,0), tabOption->leftCornerWidgetSize );
            break;

            case SE_TabWidgetRightCorner:
            rect = QRect( QPoint(0,0), tabOption->rightCornerWidgetSize );
            break;

            default: break;

        }


        if( element == SE_TabWidgetRightCorner ) rect.moveRight( paneRect.right() );
        else rect.moveLeft( paneRect.left() );

        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            rect.moveBottom( paneRect.top() - 1 );
            rect.translate( 0, 3 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            rect.moveTop( paneRect.bottom() + 1 );
            rect.translate( 0, -3 );
            break;

            default:
            return QRect();

        }

        rect = visualRect( tabOption, rect );
        return rect;

    }

    //____________________________________________________________________
    QRect Style::toolBoxTabContentsRect( const QStyleOption* option, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionToolBox* toolBoxOption( qstyleoption_cast<const QStyleOptionToolBox *>( option ) );
        if( !toolBoxOption ) return option->rect;

        // copy rect
        const QRect& rect( option->rect );

        int contentsWidth(0);
        if( !toolBoxOption->icon.isNull() )
        {
            const int iconSize( pixelMetric( QStyle::PM_SmallIconSize, option, widget ) );
            contentsWidth += iconSize;

            if( !toolBoxOption->text.isEmpty() ) contentsWidth += Metrics::ToolBox_TabItemSpacing;
        }

        if( !toolBoxOption->text.isEmpty() )
        {

            const int textWidth = toolBoxOption->fontMetrics.size( _mnemonics->textFlags(), toolBoxOption->text ).width();
            contentsWidth += textWidth;

        }

        contentsWidth = qMin( contentsWidth, rect.width() );
        contentsWidth = qMax( contentsWidth, int(Metrics::ToolBox_TabMinWidth) );
        return centerRect( rect, contentsWidth, rect.height() );

    }

    //______________________________________________________________
    QRect Style::groupBoxSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        QRect rect = option->rect;
        switch( subControl )
        {

            case SC_GroupBoxFrame: return rect;

            case SC_GroupBoxContents:
            {

                // cast option and check
                const QStyleOptionGroupBox *groupBoxOption = qstyleoption_cast<const QStyleOptionGroupBox*>( option );
                if( !groupBoxOption ) break;

                // take out frame width
                rect = insideMargin( rect, Metrics::Frame_FrameWidth );

                // get state
                const bool checkable( groupBoxOption->subControls & QStyle::SC_GroupBoxCheckBox );
                const bool emptyText( groupBoxOption->text.isEmpty() );

                // calculate title height
                int titleHeight( 0 );
                if( !emptyText ) titleHeight = groupBoxOption->fontMetrics.height();
                if( checkable ) titleHeight = qMax( titleHeight, int(Metrics::CheckBox_Size) );

                // add margin
                if( titleHeight > 0 ) titleHeight += 2*Metrics::GroupBox_TitleMarginWidth;

                rect.adjust( 0, titleHeight, 0, 0 );
                return rect;

            }

            case SC_GroupBoxCheckBox:
            case SC_GroupBoxLabel:
            {

                // cast option and check
                const QStyleOptionGroupBox *groupBoxOption = qstyleoption_cast<const QStyleOptionGroupBox*>( option );
                if( !groupBoxOption ) break;

                // take out frame width
                rect = insideMargin( rect, Metrics::Frame_FrameWidth );

                const bool emptyText( groupBoxOption->text.isEmpty() );
                const bool checkable( groupBoxOption->subControls & QStyle::SC_GroupBoxCheckBox );

                // calculate title height
                int titleHeight( 0 );
                int titleWidth( 0 );
                if( !emptyText )
                {
                    const QFontMetrics fontMetrics = option->fontMetrics;
                    titleHeight = qMax( titleHeight, fontMetrics.height() );
                    titleWidth += fontMetrics.size( _mnemonics->textFlags(), groupBoxOption->text ).width();
                }

                if( checkable )
                {
                    titleHeight = qMax( titleHeight, int(Metrics::CheckBox_Size) );
                    titleWidth += Metrics::CheckBox_Size;
                    if( !emptyText ) titleWidth += Metrics::CheckBox_ItemSpacing;
                }

                // adjust height
                QRect titleRect( rect );
                titleRect.setHeight( titleHeight );
                titleRect.translate( 0, Metrics::GroupBox_TitleMarginWidth );

                // center
                titleRect = centerRect( titleRect, titleWidth, titleHeight );

                if( subControl == SC_GroupBoxCheckBox )
                {

                    // vertical centering
                    titleRect = centerRect( titleRect, titleWidth, Metrics::CheckBox_Size );

                    // horizontal positioning
                    const QRect subRect( titleRect.topLeft(), QSize( Metrics::CheckBox_Size, titleRect.height() ) );
                    return visualRect( option->direction, titleRect, subRect );

                } else {

                    // vertical centering
                    QFontMetrics fontMetrics = option->fontMetrics;
                    titleRect = centerRect( titleRect, titleWidth, fontMetrics.height() );

                    // horizontal positioning
                    QRect subRect( titleRect );
                    if( checkable ) subRect.adjust( Metrics::CheckBox_Size + Metrics::CheckBox_ItemSpacing, 0, 0, 0 );
                    return visualRect( option->direction, titleRect, subRect );

                }

            }

            default: break;

        }

        return KStyle::subControlRect( CC_GroupBox, option, subControl, widget );
    }

    //___________________________________________________________________________________________________________________
    QRect Style::toolButtonSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionToolButton* toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton*>( option );
        if( !toolButtonOption ) return KStyle::subControlRect( CC_ToolButton, option, subControl, widget );

        const bool hasPopupMenu( toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup );
        const bool hasInlineIndicator(
            toolButtonOption->features&QStyleOptionToolButton::HasMenu
            && toolButtonOption->features&QStyleOptionToolButton::PopupDelay
            && !hasPopupMenu );

        // store rect
        const QRect& rect( option->rect );
        const int menuButtonWidth( Metrics::MenuButton_IndicatorWidth );
        switch( subControl )
        {
            case SC_ToolButtonMenu:
            {

                // check fratures
                if( !(hasPopupMenu || hasInlineIndicator ) ) return QRect();

                // check features
                QRect menuRect( rect );
                menuRect.setLeft( rect.right() - menuButtonWidth + 1 );
                if( hasInlineIndicator )
                { menuRect.setTop( menuRect.bottom() - menuButtonWidth + 1 ); }

                return visualRect( option, menuRect );
            }

            case SC_ToolButton:
            {

                if( hasPopupMenu )
                {

                    QRect contentsRect( rect );
                    contentsRect.setRight( rect.right() - menuButtonWidth );
                    return visualRect( option, contentsRect );

                } else return rect;

            }

            default: return QRect();

        }

    }

    //___________________________________________________________________________________________________________________
    QRect Style::comboBoxSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {
        // cast option and check
        const QStyleOptionComboBox *comboBoxOption( qstyleoption_cast<const QStyleOptionComboBox*>( option ) );
        if( !comboBoxOption ) return KStyle::subControlRect( CC_ComboBox, option, subControl, widget );

        const bool editable( comboBoxOption->editable );
        const bool flat( editable && !comboBoxOption->frame );

        // copy rect
        QRect rect( option->rect );

        switch( subControl )
        {
            case SC_ComboBoxFrame: return flat ? rect : QRect();
            case SC_ComboBoxListBoxPopup: return rect;

            case SC_ComboBoxArrow:
            {

                // take out frame width
                if( !flat ) rect = insideMargin( rect, Metrics::Frame_FrameWidth );

                QRect arrowRect(
                    rect.right() - Metrics::MenuButton_IndicatorWidth + 1,
                    rect.top(),
                    Metrics::MenuButton_IndicatorWidth,
                    rect.height() );

                arrowRect = centerRect( arrowRect, Metrics::MenuButton_IndicatorWidth, Metrics::MenuButton_IndicatorWidth );
                return visualRect( option, arrowRect );

            }

            case SC_ComboBoxEditField:
            {

                QRect labelRect;
                const int frameWidth( pixelMetric( PM_ComboBoxFrameWidth, option, widget ) );
                labelRect = QRect(
                    rect.left(), rect.top(),
                    rect.width() - Metrics::MenuButton_IndicatorWidth,
                    rect.height() );

                // remove margins
                if( !flat && rect.height() > option->fontMetrics.height() + 2*frameWidth )
                { labelRect.adjust( frameWidth, frameWidth, 0, -frameWidth ); }

                return visualRect( option, labelRect );

            }

            default: break;

        }

        return KStyle::subControlRect( CC_ComboBox, option, subControl, widget );

    }

    //___________________________________________________________________________________________________________________
    QRect Style::spinBoxSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSpinBox *spinBoxOption( qstyleoption_cast<const QStyleOptionSpinBox*>( option ) );
        if( !spinBoxOption ) return KStyle::subControlRect( CC_SpinBox, option, subControl, widget );
        const bool flat( !spinBoxOption->frame );

        // copy rect
        QRect rect( option->rect );

        switch( subControl )
        {
            case SC_SpinBoxFrame: return flat ? QRect():rect;

            case SC_SpinBoxUp:
            case SC_SpinBoxDown:
            {

                // take out frame width
                if( !flat && rect.height() >= 2*Metrics::Frame_FrameWidth + Metrics::SpinBox_ArrowButtonWidth ) rect = insideMargin( rect, Metrics::Frame_FrameWidth );

                QRect arrowRect;
                arrowRect = QRect(
                    rect.right() - Metrics::SpinBox_ArrowButtonWidth + 1,
                    rect.top(),
                    Metrics::SpinBox_ArrowButtonWidth,
                    rect.height() );

                const int arrowHeight( qMin( rect.height(), int(Metrics::SpinBox_ArrowButtonWidth) ) );
                arrowRect = centerRect( arrowRect, Metrics::SpinBox_ArrowButtonWidth, arrowHeight );
                arrowRect.setHeight( arrowHeight/2 );
                if( subControl == SC_SpinBoxDown ) arrowRect.translate( 0, arrowHeight/2 );

                return visualRect( option, arrowRect );

            }

            case SC_SpinBoxEditField:
            {

                QRect labelRect;
                labelRect = QRect(
                    rect.left(), rect.top(),
                    rect.width() - Metrics::SpinBox_ArrowButtonWidth,
                    rect.height() );

                // remove right side line editor margins
                const int frameWidth( pixelMetric( PM_SpinBoxFrameWidth, option, widget ) );
                if( !flat && labelRect.height() > option->fontMetrics.height() + 2*frameWidth )
                { labelRect.adjust( frameWidth, frameWidth, 0, -frameWidth ); }

                return visualRect( option, labelRect );

            }

            default: break;

        }

        return KStyle::subControlRect( CC_SpinBox, option, subControl, widget );

    }

    //___________________________________________________________________________________________________________________
    QRect Style::scrollBarInternalSubControlRect( const QStyleOptionComplex* option, SubControl subControl ) const
    {

        const QRect& rect = option->rect;
        const State& state( option->state );
        const bool horizontal( state & State_Horizontal );

        switch( subControl )
        {

            case SC_ScrollBarSubLine:
            {
                int majorSize( scrollBarButtonHeight( _subLineButtons ) );
                if( horizontal ) return visualRect( option, QRect( rect.left(), rect.top(), majorSize, rect.height() ) );
                else return visualRect( option, QRect( rect.left(), rect.top(), rect.width(), majorSize ) );
            }

            case SC_ScrollBarAddLine:
            {
                int majorSize( scrollBarButtonHeight( _addLineButtons ) );
                if( horizontal ) return visualRect( option, QRect( rect.right() - majorSize + 1, rect.top(), majorSize, rect.height() ) );
                else return visualRect( option, QRect( rect.left(), rect.bottom() - majorSize + 1, rect.width(), majorSize ) );
            }

            default: return QRect();

        }
    }

    //___________________________________________________________________________________________________________________
    QRect Style::scrollBarSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {
        // cast option and check
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return KStyle::subControlRect( CC_ScrollBar, option, subControl, widget );

        // get relevant state
        const State& state( option->state );
        const bool horizontal( state & State_Horizontal );

        switch( subControl )
        {

            case SC_ScrollBarSubLine:
            case SC_ScrollBarAddLine:
            return scrollBarInternalSubControlRect( option, subControl );

            case SC_ScrollBarGroove:
            {
                QRect topRect = visualRect( option, scrollBarInternalSubControlRect( option, SC_ScrollBarSubLine ) );
                QRect bottomRect = visualRect( option, scrollBarInternalSubControlRect( option, SC_ScrollBarAddLine ) );

                QPoint topLeftCorner;
                QPoint botRightCorner;

                if( horizontal )
                {

                    topLeftCorner  = QPoint( topRect.right() + 1, topRect.top() );
                    botRightCorner = QPoint( bottomRect.left()  - 1, topRect.bottom() );

                } else {

                    topLeftCorner  = QPoint( topRect.left(),  topRect.bottom() + 1 );
                    botRightCorner = QPoint( topRect.right(), bottomRect.top() - 1 );

                }

                // define rect
                return visualRect( option, QRect( topLeftCorner, botRightCorner )  );

            }

            case SC_ScrollBarSlider:
            {

                // We handle RTL here to unreflect things if need be
                QRect groove = visualRect( option, subControlRect( CC_ScrollBar, option, SC_ScrollBarGroove, widget ) );

                if( sliderOption->minimum == sliderOption->maximum ) return groove;

                //Figure out how much room we have..
                int space( horizontal ? groove.width() : groove.height() );

                //Calculate the portion of this space that the slider should take up.
                int sliderSize = space * qreal( sliderOption->pageStep ) / ( sliderOption->maximum - sliderOption->minimum + sliderOption->pageStep );
                sliderSize = qMax( sliderSize, static_cast<int>(Metrics::ScrollBar_MinSliderHeight ) );
                sliderSize = qMin( sliderSize, space );

                space -= sliderSize;
                if( space <= 0 ) return groove;

                int pos = qRound( qreal( sliderOption->sliderPosition - sliderOption->minimum )/ ( sliderOption->maximum - sliderOption->minimum )*space );
                if( sliderOption->upsideDown ) pos = space - pos;
                if( horizontal ) return visualRect( option, QRect( groove.left() + pos, groove.top(), sliderSize, groove.height() ) );
                else return visualRect( option, QRect( groove.left(), groove.top() + pos, groove.width(), sliderSize ) );
            }

            case SC_ScrollBarSubPage:
            {

                //We do visualRect here to unreflect things if need be
                QRect slider = visualRect( option, subControlRect( CC_ScrollBar, option, SC_ScrollBarSlider, widget ) );
                QRect groove = visualRect( option, subControlRect( CC_ScrollBar, option, SC_ScrollBarGroove, widget ) );

                if( horizontal ) return visualRect( option, QRect( groove.left(), groove.top(), slider.left() - groove.left(), groove.height() ) );
                else return visualRect( option, QRect( groove.left(), groove.top(), groove.width(), slider.top() - groove.top() ) );
            }

            case SC_ScrollBarAddPage:
            {

                //We do visualRect here to unreflect things if need be
                QRect slider = visualRect( option, subControlRect( CC_ScrollBar, option, SC_ScrollBarSlider, widget ) );
                QRect groove = visualRect( option, subControlRect( CC_ScrollBar, option, SC_ScrollBarGroove, widget ) );

                if( horizontal ) return visualRect( option, QRect( slider.right() + 1, groove.top(), groove.right() - slider.right(), groove.height() ) );
                else return visualRect( option, QRect( groove.left(), slider.bottom() + 1, groove.width(), groove.bottom() - slider.bottom() ) );

            }

            default: return KStyle::subControlRect( CC_ScrollBar, option, subControl, widget );;
        }
    }


    //___________________________________________________________________________________________________________________
    QRect Style::sliderSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return KStyle::subControlRect( CC_Slider, option, subControl, widget );

        switch( subControl )
        {
            case SC_SliderGroove:
            {

                // direction
                const bool horizontal( sliderOption->orientation == Qt::Horizontal );

                // get base class rect
                QRect grooveRect( KStyle::subControlRect( CC_Slider, option, subControl, widget ) );
                grooveRect = insideMargin( grooveRect, pixelMetric( PM_DefaultFrameWidth, option, widget ) );

                // centering
                if( horizontal )
                {

                    grooveRect = centerRect( grooveRect, grooveRect.width(), Metrics::Slider_GrooveThickness );
                    grooveRect = insideMargin( grooveRect, 3, 0 );

                } else {

                    grooveRect = centerRect( grooveRect, Metrics::Slider_GrooveThickness, grooveRect.height() );
                    grooveRect = insideMargin( grooveRect, 0, 3 );

                }

                return grooveRect;

            }

            case SC_SliderHandle:
            {

                QRect handleRect( KStyle::subControlRect( CC_Slider, option, subControl, widget ) );
                handleRect = centerRect( handleRect, Metrics::Slider_ControlThickness, Metrics::Slider_ControlThickness );
                return handleRect;

            }

            default: return KStyle::subControlRect( CC_Slider, option, subControl, widget );
        }

    }

    //______________________________________________________________
    QSize Style::checkBoxSizeFromContents( const QStyleOption*, const QSize& contentsSize, const QWidget* ) const
    {
        // get contents size
        QSize size( contentsSize );

        // add focus height
        size = expandSize( size, 0, Metrics::CheckBox_FocusMarginWidth );

        // make sure there is enough height for indicator
        size.setHeight( qMax( size.height(), int(Metrics::CheckBox_Size) ) );

        // Add space for the indicator and the icon
        size.rwidth() += Metrics::CheckBox_Size + Metrics::CheckBox_ItemSpacing;

        // also add extra space, to leave room to the right of the label
        size.rwidth() += Metrics::CheckBox_ItemSpacing;

        return size;

    }

    //______________________________________________________________
    QSize Style::lineEditSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {
        // cast option and check
        const QStyleOptionFrame* frameOption( qstyleoption_cast<const QStyleOptionFrame*>( option ) );
        if( !frameOption ) return contentsSize;

        const bool flat( frameOption->lineWidth == 0 );
        const int frameWidth( pixelMetric( PM_DefaultFrameWidth, option, widget ) );
        return flat ? contentsSize : expandSize( contentsSize, frameWidth );
    }

    //______________________________________________________________
    QSize Style::comboBoxSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionComboBox* comboBoxOption( qstyleoption_cast<const QStyleOptionComboBox*>( option ) );
        if( !comboBoxOption ) return contentsSize;

        // copy size
        QSize size( contentsSize );

        // add relevant margin
        const bool flat( !comboBoxOption->frame );
        const int frameWidth( pixelMetric( PM_ComboBoxFrameWidth, option, widget ) );
        if( !flat ) size = expandSize( size, frameWidth );

        // make sure there is enough height for the button
        size.setHeight( qMax( size.height(), int(Metrics::MenuButton_IndicatorWidth) ) );

        // add button width and spacing
        size.rwidth() += Metrics::MenuButton_IndicatorWidth;

        return size;

    }

    //______________________________________________________________
    QSize Style::spinBoxSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSpinBox *spinBoxOption( qstyleoption_cast<const QStyleOptionSpinBox*>( option ) );
        if( !spinBoxOption ) return contentsSize;

        const bool flat( !spinBoxOption->frame );

        // copy size
        QSize size( contentsSize );

        // add editor margins
        const int frameWidth( pixelMetric( PM_SpinBoxFrameWidth, option, widget ) );
        if( !flat ) size = expandSize( size, frameWidth );

        // make sure there is enough height for the button
        size.setHeight( qMax( size.height(), int(Metrics::SpinBox_ArrowButtonWidth) ) );

        // add button width and spacing
        size.rwidth() += Metrics::SpinBox_ArrowButtonWidth;

        return size;

    }

    //______________________________________________________________
    QSize Style::sliderSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionSlider *sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return contentsSize;

        // store tick position and orientation
        const QSlider::TickPosition& tickPosition( sliderOption->tickPosition );
        const bool horizontal( sliderOption->orientation == Qt::Horizontal );
        const bool disableTicks( !StyleConfigData::sliderDrawTickMarks() );

        /*
        Qt adds its own tick length directly inside QSlider.
        Take it out and replace by ours, if needed
        */
        const int tickLength( disableTicks ? 0 : (
            Metrics::Slider_TickLength + Metrics::Slider_TickMarginWidth +
            (Metrics::Slider_GrooveThickness - Metrics::Slider_ControlThickness)/2 ) );

        const int builtInTickLength( 5 );
        if( tickPosition == QSlider::NoTicks ) return contentsSize;

        QSize size( contentsSize );
        if( horizontal )
        {

            if(tickPosition & QSlider::TicksAbove) size.rheight() += tickLength - builtInTickLength;
            if(tickPosition & QSlider::TicksBelow) size.rheight() += tickLength - builtInTickLength;

        } else {

            if(tickPosition & QSlider::TicksAbove) size.rwidth() += tickLength - builtInTickLength;
            if(tickPosition & QSlider::TicksBelow) size.rwidth() += tickLength - builtInTickLength;

        }

        return size;

    }

    //______________________________________________________________
    QSize Style::pushButtonSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionButton* buttonOption( qstyleoption_cast<const QStyleOptionButton*>( option ) );
        if( !buttonOption ) return contentsSize;

        QSize size( contentsSize );

        // add space for arrow
        if( buttonOption->features & QStyleOptionButton::HasMenu )
        {
            size.rheight() += 2*Metrics::Button_MarginWidth;
            size.setHeight( qMax( size.height(), int( Metrics::MenuButton_IndicatorWidth ) ) );
            size.rwidth() += Metrics::Button_MarginWidth;

            if( !( buttonOption->icon.isNull() && buttonOption->text.isEmpty() ) )
            { size.rwidth() += Metrics::Button_ItemSpacing; }

        }  else size = expandSize( size, Metrics::Button_MarginWidth );

        // add space for icon
        if( !buttonOption->icon.isNull() )
        {

            QSize iconSize = buttonOption->iconSize;
            if( !iconSize.isValid() ) iconSize = QSize( pixelMetric( PM_SmallIconSize, option, widget ), pixelMetric( PM_SmallIconSize, option, widget ) );

            size.setHeight( qMax( size.height(), iconSize.height() ) );

            if( !buttonOption->text.isEmpty() )
            { size.rwidth() += Metrics::Button_ItemSpacing; }

        }

        // make sure buttons have a minimum width
        if( !buttonOption->text.isEmpty() )
        { size.rwidth() = qMax( size.rwidth(), int( Metrics::Button_MinWidth ) ); }

        // finally add margins
        return expandSize( size, Metrics::Frame_FrameWidth );

    }

    //______________________________________________________________
    QSize Style::toolButtonSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionToolButton* toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton*>( option );
        if( !toolButtonOption ) return contentsSize;

        // copy size
        QSize size = contentsSize;

        // get relevant state flags
        const State& state( option->state );
        const bool autoRaise( state & State_AutoRaise );
        const bool hasPopupMenu( toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup );
        const bool hasInlineIndicator(
            toolButtonOption->features&QStyleOptionToolButton::HasMenu
            && toolButtonOption->features&QStyleOptionToolButton::PopupDelay
            && !hasPopupMenu );

        const int marginWidth( autoRaise ? Metrics::ToolButton_MarginWidth : Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth );

        if( hasInlineIndicator ) size.rwidth() += Metrics::ToolButton_InlineIndicatorWidth;
        size = expandSize( size, marginWidth );

        return size;

    }

    //______________________________________________________________
    QSize Style::menuBarItemSizeFromContents( const QStyleOption*, const QSize& contentsSize, const QWidget* ) const
    { return expandSize( contentsSize, Metrics::MenuBarItem_MarginWidth, Metrics::MenuBarItem_MarginHeight ); }

    //______________________________________________________________
    QSize Style::menuItemSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if( !menuItemOption ) return contentsSize;

        // First, we calculate the intrinsic size of the item.
        // this must be kept consistent with what's in drawMenuItemControl
        QSize size( contentsSize );
        switch( menuItemOption->menuItemType )
        {

            case QStyleOptionMenuItem::Normal:
            case QStyleOptionMenuItem::DefaultItem:
            case QStyleOptionMenuItem::SubMenu:
            {

                const int iconWidth( isQtQuickControl( option, widget ) ? qMax( pixelMetric(PM_SmallIconSize, option, widget ), menuItemOption->maxIconWidth ) : menuItemOption->maxIconWidth );

                int leftColumnWidth( iconWidth );

                // add space with respect to text
                leftColumnWidth += Metrics::MenuItem_ItemSpacing;

                // add checkbox indicator width
                if( menuItemOption->menuHasCheckableItems )
                { leftColumnWidth += Metrics::CheckBox_Size + Metrics::MenuItem_ItemSpacing; }

                // add spacing for accelerator
                /*
                Note:
                The width of the accelerator itself is not included here since
                Qt will add that on separately after obtaining the
                sizeFromContents() for each menu item in the menu to be shown
                ( see QMenuPrivate::calcActionRects() )
                */
                const bool hasAccelerator( menuItemOption->text.indexOf( QLatin1Char( '\t' ) ) >= 0 );
                if( hasAccelerator ) size.rwidth() += Metrics::MenuItem_AcceleratorSpace;

                // right column
                const int rightColumnWidth = Metrics::MenuButton_IndicatorWidth + Metrics::MenuItem_ItemSpacing;
                size.rwidth() += leftColumnWidth + rightColumnWidth;

                // make sure height is large enough for icon and arrow
                size.setHeight( qMax( size.height(), int(Metrics::MenuButton_IndicatorWidth) ) );
                size.setHeight( qMax( size.height(), int(Metrics::CheckBox_Size) ) );
                size.setHeight( qMax( size.height(), iconWidth ) );
                return expandSize( size, Metrics::MenuItem_MarginWidth );

            }

            case QStyleOptionMenuItem::Separator:
            {

                if( menuItemOption->text.isEmpty() && menuItemOption->icon.isNull() )
                {

                    return expandSize( QSize(0,1), Metrics::MenuItem_MarginWidth );


                } else {

                    // build toolbutton option
                    const QStyleOptionToolButton toolButtonOption( separatorMenuItemOption( menuItemOption, widget ) );

                    // make sure height is large enough for icon and text
                    const int iconWidth( qMax( pixelMetric(PM_SmallIconSize, option, widget ), menuItemOption->maxIconWidth ) );
                    const int textHeight( menuItemOption->fontMetrics.height() );
                    if( !menuItemOption->icon.isNull() ) size.setHeight( qMax( size.height(), iconWidth ) );
                    if( !menuItemOption->text.isEmpty() )
                    {
                        size.setHeight( qMax( size.height(), textHeight ) );
                        size.setWidth( qMax( size.width(), menuItemOption->fontMetrics.width( menuItemOption->text ) ) );
                    }

                    return sizeFromContents( CT_ToolButton, &toolButtonOption, size, widget );

                }

            }

            // for all other cases, return input
            default: return contentsSize;
        }

    }

    //______________________________________________________________
    QSize Style::tabWidgetSizeFromContents( const QStyleOption*, const QSize& contentsSize, const QWidget* ) const
    { return expandSize( contentsSize, Metrics::TabWidget_MarginWidth ); }

    //______________________________________________________________
    QSize Style::tabBarTabSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        const auto tabOption( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        const auto tabOptionV3( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        const bool hasText( tabOption && !tabOption->text.isEmpty() );
        const bool hasIcon( tabOption && !tabOption->icon.isNull() );
        const bool hasLeftButton( tabOptionV3 && !tabOptionV3->leftButtonSize.isEmpty() );
        const bool hasRightButton( tabOptionV3 && !tabOptionV3->leftButtonSize.isEmpty() );

        // calculate width increment for horizontal tabs
        int widthIncrement = 0;
        if( hasIcon && !( hasText || hasLeftButton || hasRightButton ) ) widthIncrement -= 4;
        if( hasText && hasIcon ) widthIncrement += Metrics::TabBar_TabItemSpacing;
        if( hasLeftButton && ( hasText || hasIcon ) )  widthIncrement += Metrics::TabBar_TabItemSpacing;
        if( hasRightButton && ( hasText || hasIcon || hasLeftButton ) )  widthIncrement += Metrics::TabBar_TabItemSpacing;

        // add margins
        QSize size( contentsSize );

        // compare to minimum size
        const bool verticalTabs( tabOption && isVerticalTab( tabOption ) );
        if( verticalTabs )
        {

            size.rwidth() += 2*Metrics::TabBar_TabOffset;
            size.rheight() += widthIncrement;
            if( hasIcon && !hasText ) size = size.expandedTo( QSize( Metrics::TabBar_TabMinHeight, 0 ) );
            else size = size.expandedTo( QSize( Metrics::TabBar_TabMinHeight, Metrics::TabBar_TabMinWidth ) );

        } else {

            size.rheight() += 2*Metrics::TabBar_TabOffset;
            size.rwidth() += widthIncrement;
            if( hasIcon && !hasText ) size = size.expandedTo( QSize( 0, Metrics::TabBar_TabMinHeight ) );
            else size = size.expandedTo( QSize( Metrics::TabBar_TabMinWidth, Metrics::TabBar_TabMinHeight ) );
        }

        return size;

    }

    //______________________________________________________________
    QSize Style::headerSectionSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionHeader* headerOption( qstyleoption_cast<const QStyleOptionHeader*>( option ) );
        if( !headerOption ) return contentsSize;

        // get text size
        const bool horizontal( headerOption->orientation == Qt::Horizontal );
        const bool hasText( !headerOption->text.isEmpty() );
        const bool hasIcon( !headerOption->icon.isNull() );

        const QSize textSize( hasText ? headerOption->fontMetrics.size( 0, headerOption->text ) : QSize() );
        const QSize iconSize( hasIcon ? QSize( 22,22 ) : QSize() );

        // contents width
        int contentsWidth( 0 );
        if( hasText ) contentsWidth += textSize.width();
        if( hasIcon )
        {
            contentsWidth += iconSize.width();
            if( hasText ) contentsWidth += Metrics::Header_ItemSpacing;
        }

        // contents height
        int contentsHeight( headerOption->fontMetrics.height() );
        if( hasIcon ) contentsHeight = qMax( contentsHeight, iconSize.height() );

        if( horizontal && headerOption->sortIndicator != QStyleOptionHeader::None )
        {
            // also add space for sort indicator
            contentsWidth += Metrics::Header_ArrowSize + Metrics::Header_ItemSpacing;
            contentsHeight = qMax( contentsHeight, int(Metrics::Header_ArrowSize) );
        }

        // update contents size, add margins and return
        const QSize size( contentsSize.expandedTo( QSize( contentsWidth, contentsHeight ) ) );
        return expandSize( size, Metrics::Header_MarginWidth );

    }

    //______________________________________________________________
    QSize Style::itemViewItemSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {
        // call base class
        QSize size( KStyle::sizeFromContents( CT_ItemViewItem, option, contentsSize, widget ) );

        // add margins
        return expandSize( size, Metrics::ItemView_ItemMarginWidth );

    }

    //___________________________________________________________________________________
    bool Style::drawFramePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );

        const bool isInputWidget( ( widget && widget->testAttribute( Qt::WA_Hover ) ) ||
            ( isQtQuickControl( option, widget ) && option->styleObject->property( "elementType" ).toString() == QStringLiteral( "edit") ) );

        const bool mouseOver( enabled && isInputWidget && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && isInputWidget && ( state & State_HasFocus ) );

        // assume focus takes precedence over hover
        _animations->inputWidgetEngine().updateState( widget, AnimationFocus, hasFocus );
        _animations->inputWidgetEngine().updateState( widget, AnimationHover, mouseOver && !hasFocus );

        if( state & State_Sunken )
        {

            // retrieve animation mode and opacity
            const AnimationMode mode( _animations->inputWidgetEngine().frameAnimationMode( widget ) );
            const qreal opacity( _animations->inputWidgetEngine().frameOpacity( widget ) );

            if( _frameShadowFactory->isRegistered( widget ) )
            {

                _frameShadowFactory->updateShadowsGeometry( widget, rect );
                _frameShadowFactory->updateState( widget, hasFocus, mouseOver, opacity, mode );

            } else {

                StyleOptions options;
                if( hasFocus ) options |= Focus;
                if( mouseOver ) options |= Hover;

                _helper->renderHole(
                    painter, palette.color( QPalette::Window ), rect, options,
                    opacity, mode, TileSet::Ring );

            }

        } else if( state & State_Raised ) {

            const QRect local( rect );
            renderSlab( painter, rect, palette.color( QPalette::Window ), NoFill );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawFrameLineEditPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // make sure there is enough room to render frame
        if( rect.height() < 2*Metrics::LineEdit_FrameWidth + option->fontMetrics.height() )
        {

            const QColor background( palette.color( QPalette::Base ) );

            painter->setPen( Qt::NoPen );
            painter->setBrush( background );
            painter->drawRect( rect );
            return true;

        } else {

            // store state
            const State& state( option->state );
            const bool enabled( state & State_Enabled );
            const bool mouseOver( enabled && ( state & State_MouseOver ) );
            const bool hasFocus( enabled && ( state & State_HasFocus ) );

            // assume focus takes precedence over hover
            _animations->inputWidgetEngine().updateState( widget, AnimationFocus, hasFocus );
            _animations->inputWidgetEngine().updateState( widget, AnimationHover, mouseOver && !hasFocus );

            // retrieve animation mode and opacity
            const AnimationMode mode( _animations->inputWidgetEngine().frameAnimationMode( widget ) );
            const qreal opacity( _animations->inputWidgetEngine().frameOpacity( widget ) );

            // fill
            painter->setPen( Qt::NoPen );
            painter->setBrush( palette.color( QPalette::Base ) );
            _helper->fillHole( *painter, rect );

            // render hole
            StyleOptions options;
            if( hasFocus ) options |= Focus;
            if( mouseOver ) options |= Hover;
            _helper->renderHole( painter, palette.color( QPalette::Window ), rect, options, opacity, mode, TileSet::Ring );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawFrameFocusRectPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        if( !widget ) return true;

        // no focus indicator on buttons, since it is rendered elsewhere
        if( qobject_cast< const QAbstractButton*>( widget ) )
        { return true; }

        const State& state( option->state );
        const QRect rect( option->rect.adjusted( 0, 0, 0, 1 ) );
        const QPalette& palette( option->palette );

        if( rect.width() < 10 ) return true;

        const QColor outlineColor( state & State_Selected ? palette.color( QPalette::HighlightedText ):palette.color( QPalette::Highlight ) );
        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->setPen( outlineColor );
        painter->drawLine( rect.bottomLeft(), rect.bottomRight() );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawFrameMenuPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        // only draw frame for ( expanded ) toolbars
        // do nothing for other cases
        if( qobject_cast<const QToolBar*>( widget ) )
        {

            _helper->renderWindowBackground( painter, option->rect, widget, option->palette );
            _helper->drawFloatFrame( painter, option->rect, option->palette.window().color(), true );

        } else if( isQtQuickControl( option, widget ) ) {

            // QtQuick Control case
            painter->fillRect( option->rect, option->palette.window() );
            _helper->drawFloatFrame( painter, option->rect, option->palette.window().color(), true );
        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawFrameGroupBoxPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const auto frameOption( qstyleoption_cast<const QStyleOptionFrame*>( option ) );
        if( !frameOption ) return true;

        // no frame for flat groupboxes
        if( frameOption->features & QStyleOptionFrame::Flat ) return true;
        // normal frame
        const QPalette& palette( option->palette );
        const QRect& rect( option->rect );
        const QColor base( _helper->backgroundColor( palette.color( QPalette::Window ), widget, rect.center() ) );

        painter->save();
        painter->setRenderHint( QPainter::Antialiasing );
        painter->setPen( Qt::NoPen );

        QLinearGradient innerGradient( 0, rect.top()-rect.height()+12, 0, rect.bottom()+rect.height()-19 );
        QColor light( _helper->calcLightColor( base ) );
        light.setAlphaF( 0.4 ); innerGradient.setColorAt( 0, light );
        light.setAlphaF( 0 ); innerGradient.setColorAt( 1, light );
        painter->setBrush( innerGradient );
        painter->setClipRect( rect.adjusted( 0, 0, 0, -19 ) );
        _helper->fillSlab( *painter, rect );

        painter->setClipping( false );
        _helper->slope( base, 0 ).render( rect, painter );

        painter->restore();
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawFrameTabWidgetPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionTabWidgetFrame* tabOption( qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option ) );
        if( !tabOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        const bool reverseLayout( option->direction == Qt::RightToLeft );

        /*
        no frame is drawn when tabbar is empty.
        this is consistent with the tabWidgetTabContents subelementRect
        */
        if( tabOption->tabBarSize.isEmpty() ) return true;

        // get tabbar dimensions
        const int width( tabOption->tabBarSize.width() );
        const int height( tabOption->tabBarSize.height() );

        // left corner widget
        const int leftWidth( tabOption->leftCornerWidgetSize.width() );
        const int rightWidth( tabOption->rightCornerWidgetSize.width() );

        // list of slabs to be drawn
        SlabRect::List slabs;

        QRect baseSlabRect( rect );

        // render the three free sides
        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                // main slab
                slabs << SlabRect( baseSlabRect, ( TileSet::Ring & ~TileSet::Top ) );

                // top
                if( reverseLayout )
                {

                    // left side
                    QRect slabRect( baseSlabRect );
                    slabRect.setRight( qMax( slabRect.right() - width - leftWidth, slabRect.left() + rightWidth ) );
                    slabRect.setHeight( 1 );
                    slabs << SlabRect( slabRect, TileSet::TopLeft ).adjusted( TileSet::DefaultSize );

                    // right side
                    if( rightWidth > 0 )
                    {
                        QRect slabRect( baseSlabRect );
                        slabRect.setLeft( slabRect.right() - rightWidth );
                        slabRect.setHeight( 1 );
                        slabs << SlabRect( slabRect, TileSet::TopRight ).adjusted( TileSet::DefaultSize );
                    }

                } else {

                    // left side
                    if( leftWidth > 0 )
                    {

                        QRect slabRect( baseSlabRect );
                        slabRect.setRight( baseSlabRect.left() + leftWidth );
                        slabRect.setHeight( 1 );
                        slabs << SlabRect( slabRect, TileSet::TopLeft ).adjusted( TileSet::DefaultSize );

                    }

                    // right side
                    QRect slabRect( baseSlabRect );
                    slabRect.setLeft( qMin( slabRect.left() + width + leftWidth + 1, slabRect.right() - rightWidth ) );
                    slabRect.setHeight( 1 );
                    slabs << SlabRect( slabRect, TileSet::TopRight ).adjusted( TileSet::DefaultSize );

                }
                break;
            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {

                slabs << SlabRect( baseSlabRect, TileSet::Ring & ~TileSet::Bottom );

                if( reverseLayout )
                {

                    // left side
                    QRect slabRect( baseSlabRect );
                    slabRect.setRight( qMax( slabRect.right() -width - leftWidth, slabRect.left() + rightWidth ) );
                    slabRect.setTop( slabRect.bottom() );
                    slabs << SlabRect( slabRect, TileSet::BottomLeft ).adjusted( TileSet::DefaultSize );

                    // right side
                    if( rightWidth > 0 )
                    {
                        QRect slabRect( baseSlabRect );
                        slabRect.setLeft( slabRect.right() - rightWidth - TileSet::DefaultSize );
                        slabRect.setTop( slabRect.bottom() );
                        slabs << SlabRect( slabRect, TileSet::BottomRight ).adjusted( TileSet::DefaultSize );
                    }

                } else {

                    // left side
                    if( leftWidth > 0 )
                    {

                        QRect slabRect( baseSlabRect );
                        slabRect.setRight( baseSlabRect.left() + leftWidth );
                        slabRect.setTop( slabRect.bottom() );
                        slabs << SlabRect( slabRect, TileSet::BottomLeft ).adjusted( TileSet::DefaultSize );

                    }

                    // right side
                    QRect slabRect( baseSlabRect );
                    slabRect.setLeft( qMin( slabRect.left() + width + leftWidth + 1, slabRect.right() - rightWidth ) );
                    slabRect.setTop( slabRect.bottom() );
                    slabs << SlabRect( slabRect, TileSet::BottomRight ).adjusted( TileSet::DefaultSize );

                }

                break;
            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                slabs << SlabRect( baseSlabRect, TileSet::Ring & ~TileSet::Left );

                // bottom side
                QRect slabRect( baseSlabRect );
                slabRect.setTop( qMin( slabRect.top() + height, slabRect.bottom() ) );
                slabRect.setWidth( 1 );
                slabs << SlabRect( slabRect, TileSet::BottomLeft ).adjusted( TileSet::DefaultSize );

                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                slabs << SlabRect( baseSlabRect, TileSet::Ring & ~TileSet::Right );

                // bottom side
                QRect slabRect( baseSlabRect );
                slabRect.setTop( qMin( slabRect.top() + height, slabRect.bottom() ) );
                slabRect.setLeft( slabRect.right() );
                slabs << SlabRect( slabRect, TileSet::BottomRight ).adjusted( TileSet::DefaultSize );
                break;
            }

            break;

            default: break;
        }

        // render registered slabs
        foreach( const SlabRect& slab, slabs )
        { renderSlab( painter, slab, palette.color( QPalette::Window ), NoFill ); }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawFrameTabBarBasePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // tabbar frame used either for 'separate' tabbar, or in 'document mode'

        // cast option and check
        const QStyleOptionTabBarBase* tabOption( qstyleoption_cast<const QStyleOptionTabBarBase*>( option ) );
        if( !tabOption ) return true;

        if( tabOption->tabBarRect.isValid() )
        {

            // if tabBar rect is valid, all the frame is handled in tabBarTabShapeControl
            // nothing to be done here.
            // on the other hand, invalid tabBarRect corresponds to buttons in tabbars ( e.g. corner buttons ),
            // and the appropriate piece of frame needs to be drawn
            return true;

        }

        // store palette and rect
        const QPalette& palette( option->palette );
        const QRect& rect( option->rect );

        if( !rect.isValid() ) return true;

        QRect frameRect( rect );
        SlabRect slab;
        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                frameRect = insideMargin( frameRect, -8, 0 );
                frameRect.translate( 0, 4 );
                slab = SlabRect( frameRect, TileSet::Top );
                break;
            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                frameRect = insideMargin( frameRect, -8, 0 );
                frameRect.translate( 0, -4 );
                slab = SlabRect( frameRect, TileSet::Bottom );
                break;
            }

            default: return true;
        }

        // render registered slabs
        renderSlab( painter, slab, palette.color( QPalette::Window ), NoFill );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawFrameWindowPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // render
        _helper->drawFloatFrame( painter, rect, palette.window().color(), false );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorArrowPrimitive( ArrowOrientation orientation, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        // store rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        bool mouseOver( enabled && ( state & State_MouseOver ) );
        bool hasFocus( enabled && ( state & State_HasFocus ) );

        // detect special buttons
        const bool inTabBar( widget && qobject_cast<const QTabBar*>( widget->parentWidget() ) );
        const bool inToolButton( qstyleoption_cast<const QStyleOptionToolButton *>( option ) );

                // color
        QColor color;
        if( inTabBar ) {

            // for tabbar arrows one uses animations to get the arrow color
            // get animation state
            /* there is no need to update the engine since this was already done when rendering the frame */
            const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
            const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );

            StyleOptions styleOptions;
            if( mouseOver ) styleOptions |= Hover;
            if( hasFocus ) styleOptions |= Focus;

            color = _helper->arrowColor( palette, styleOptions, opacity, mode );

        } else if( mouseOver && !inToolButton ) {

            color = _helper->hoverColor( palette );

        } else if( inToolButton ) {

            const bool flat( state & State_AutoRaise );

            // cast option
            const QStyleOptionToolButton* toolButtonOption( static_cast<const QStyleOptionToolButton*>( option ) );
            const bool hasPopupMenu( toolButtonOption->subControls & SC_ToolButtonMenu );
            if( flat && hasPopupMenu )
            {

                // for menu arrows in flat toolbutton one uses animations to get the arrow color
                // handle arrow over animation
                const bool arrowHover( mouseOver && ( toolButtonOption->activeSubControls & SC_ToolButtonMenu ) );
                _animations->toolButtonEngine().updateState( widget, AnimationHover, arrowHover );

                const bool animated( _animations->toolButtonEngine().isAnimated( widget, AnimationHover ) );
                const qreal opacity( _animations->toolButtonEngine().opacity( widget, AnimationHover ) );

                StyleOptions styleOptions;
                if( arrowHover ) styleOptions |= Hover;

                color = _helper->arrowColor( palette, styleOptions, opacity, animated ? AnimationHover:AnimationNone );

            } else {

                color = palette.color( flat ? QPalette::WindowText : QPalette::ButtonText );

            }

        } else color = palette.color( QPalette::WindowText );

        // get arrow polygon
        const QPolygonF arrow = genericArrow( orientation, ArrowNormal );

        const qreal penThickness = 1.6;
        const qreal offset( qMin( penThickness, qreal( 1 ) ) );

        painter->translate( QRectF( rect ).center() );
        painter->setRenderHint( QPainter::Antialiasing );

        painter->translate( 0,offset );
        const QColor background = palette.color( QPalette::Window );
        painter->setPen( QPen( _helper->calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );
        painter->translate( 0,-offset );

        painter->setPen( QPen( _helper->decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorHeaderArrowPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionHeader *headerOption = qstyleoption_cast<const QStyleOptionHeader *>( option );
        const State& state( option->state );

        // arrow orientation
        ArrowOrientation orientation( ArrowNone );
        if( state & State_UpArrow || ( headerOption && headerOption->sortIndicator==QStyleOptionHeader::SortUp ) ) orientation = ArrowUp;
        else if( state & State_DownArrow || ( headerOption && headerOption->sortIndicator==QStyleOptionHeader::SortDown ) ) orientation = ArrowDown;
        if( orientation == ArrowNone ) return true;

        // invert arrows if requested by (hidden) options
        if( StyleConfigData::viewInvertSortIndicator() ) orientation = (orientation == ArrowUp) ? ArrowDown:ArrowUp;

        // flags, rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );

        _animations->headerViewEngine().updateState( widget, rect.topLeft(), mouseOver );
        const bool animated( enabled && _animations->headerViewEngine().isAnimated( widget, rect.topLeft() ) );

        // define gradient and polygon for drawing arrow
        const QPolygonF arrow = genericArrow( orientation, ArrowNormal );
        QColor color = palette.color( QPalette::WindowText );
        const QColor background = palette.color( QPalette::Window );
        const QColor highlight( _helper->hoverColor( palette ) );
        const qreal penThickness = 1.6;
        const qreal offset( qMin( penThickness, qreal( 1 ) ) );

        if( animated )
        {

            const qreal opacity( _animations->headerViewEngine().opacity( widget, rect.topLeft() ) );
            color = KColorUtils::mix( color, highlight, opacity );

        } else if( mouseOver ) color = highlight;

        painter->translate( QRectF(rect).center() );
        painter->translate( 0, 1 );
        painter->setRenderHint( QPainter::Antialiasing );

        painter->translate( 0,offset );
        painter->setPen( QPen( _helper->calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );
        painter->translate( 0,-offset );

        painter->setPen( QPen( _helper->decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );

        return true;
    }

    //______________________________________________________________
    bool Style::drawPanelButtonCommandPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) && !( widget && widget->focusProxy() ) );
        const bool sunken( state & ( State_On|State_Sunken ) );

        StyleOptions styleOptions;
        if( sunken ) styleOptions |= Sunken;
        if( hasFocus ) styleOptions |= Focus;
        if( mouseOver ) styleOptions |= Hover;

        // update animation state
        _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        // store animation state
        AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
        const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );

        // decide if widget must be rendered flat.
        /*
        The decision is made depending on
        - whether the "flat" flag is set in the option
        - whether the widget is hight enough to render both icons and normal margins
        Note: in principle one should also check for the button text height
        */

        const QStyleOptionButton* buttonOption( qstyleoption_cast< const QStyleOptionButton* >( option ) );
        bool flat = ( buttonOption && (
            buttonOption->features.testFlag( QStyleOptionButton::Flat ) ||
            ( ( !buttonOption->icon.isNull() ) && sizeFromContents( CT_PushButton, option, buttonOption->iconSize, widget ).height() > rect.height() ) ) );

        if( flat )
        {

            if( !sunken )
            {

                const QColor glow( _helper->buttonGlowColor( palette, styleOptions, opacity, mode ) );
                if( glow.isValid() ) _helper->slitFocused( glow ).render( rect, painter );

            } else {

                _helper->renderHole( painter, palette.color( QPalette::Window ), rect, styleOptions, opacity, mode, TileSet::Ring );

            }

        } else {

            // match color to the window background
            QColor buttonColor( _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );

            // merge button color with highlight in case of default button
            if( enabled && buttonOption && (buttonOption->features&QStyleOptionButton::DefaultButton) )
            {
                const QColor tintColor( _helper->calcLightColor( buttonColor ) );
                buttonColor = KColorUtils::mix( buttonColor, tintColor, 0.5 );
            }

            // disable animation for sunken buttons
            if( sunken ) mode = AnimationNone;
            renderButtonSlab( painter, rect, buttonColor, styleOptions, opacity, mode, TileSet::Ring );

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawPanelButtonToolPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );
        const bool autoRaised( state & State_AutoRaise );
        const bool sunken( state & ( State_On|State_Sunken ) );

        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // toolbar animation
        const bool isInToolBar( widget && qobject_cast<const QToolBar*>( widget->parent() ) );
        const bool toolBarAnimated( isInToolBar && widget && ( _animations->toolBarEngine().isAnimated( widget->parentWidget() ) || _animations->toolBarEngine().isFollowMouseAnimated( widget->parentWidget() ) ) );
        const QRect animatedRect( ( isInToolBar && widget ) ? _animations->toolBarEngine().animatedRect( widget->parentWidget() ):QRect() );
        const QRect childRect( ( widget && widget->parentWidget() ) ? rect.translated( widget->mapToParent( QPoint( 0,0 ) ) ):QRect() );
        const QRect currentRect(  widget ? _animations->toolBarEngine().currentRect( widget->parentWidget() ):QRect() );
        const bool current( isInToolBar && widget && widget->parentWidget() && currentRect.intersects( rect.translated( widget->mapToParent( QPoint( 0,0 ) ) ) ) );

        const bool toolBarTimerActive( isInToolBar && widget && _animations->toolBarEngine().isTimerActive( widget->parentWidget() ) );
        const qreal toolBarOpacity( ( isInToolBar && widget ) ? _animations->toolBarEngine().opacity( widget->parentWidget() ):0 );

        // normal animation state
        // no need to update, this was already done in drawToolButtonComplexControl
        const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
        const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );

        // style options
        StyleOptions styleOptions;
        if( sunken ) styleOptions |= Sunken;
        if( hasFocus ) styleOptions |= Focus;
        if( mouseOver ) styleOptions |= Hover;

        // non autoraised tool buttons get same slab as regular buttons
        if( !autoRaised )
        {

            TileSet::Tiles tiles( TileSet::Ring );

            // adjust tiles and rect in case of menubutton
            const QToolButton* toolButton = qobject_cast<const QToolButton*>( widget );
            const bool hasPopupMenu( toolButton && toolButton->popupMode() == QToolButton::MenuButtonPopup );

            if( hasPopupMenu )
            {
                if( reverseLayout ) tiles &= ~TileSet::Left;
                else tiles &= ~TileSet::Right;
            }

            // match button color to window background
            const QColor buttonColor( _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );

            // render slab
            renderButtonSlab( painter, rect, buttonColor, styleOptions, opacity, mode, tiles );

            return true;

        }

        // normal ( auto-raised ) toolbuttons
        if( sunken )
        {

            // fill hole
            qreal holeOpacity = 1;
            const qreal bias = 0.75;
            if( mode == AnimationHover ) holeOpacity = 1 - bias*opacity;
            else if( toolBarAnimated && enabled && animatedRect.isNull() && current  ) holeOpacity = 1 - bias*toolBarOpacity;
            else if( enabled && (( toolBarTimerActive && current ) || mouseOver ) ) holeOpacity = 1 - bias;

            if( holeOpacity > 0 )
            {
                QColor color( _helper->backgroundColor( _helper->calcMidColor( palette.color( QPalette::Window ) ), widget, rect.center() ) );
                color = _helper->alphaColor( color, holeOpacity );
                painter->setRenderHint( QPainter::Antialiasing );
                painter->setPen( Qt::NoPen );
                painter->setBrush( color );
                painter->drawRoundedRect( insideMargin( rect, 1 ), 3.5, 3.5 );
            }


            styleOptions |= HoleContrast;
            if( mode != AnimationNone )
            {

                _helper->renderHole( painter, palette.color( QPalette::Window ), rect, styleOptions, opacity, mode, TileSet::Ring );

            } else if( toolBarAnimated ) {

                if( enabled && animatedRect.isNull() && current  )
                {

                    _helper->renderHole( painter, palette.color( QPalette::Window ), rect, styleOptions, toolBarOpacity, AnimationHover, TileSet::Ring );

                } else {

                    _helper->renderHole( painter, palette.color( QPalette::Window ), rect, HoleContrast );

                }

            } else if( toolBarTimerActive && current ) {

                _helper->renderHole( painter, palette.color( QPalette::Window ), rect, styleOptions | Hover );

            } else {

                _helper->renderHole( painter, palette.color( QPalette::Window ), rect, styleOptions );

            }

        } else {

            const QColor glow( _helper->buttonGlowColor( palette, styleOptions, opacity, mode ) );
            if( mode != AnimationNone ) _helper->slitFocused( glow ).render( rect, painter );
            else if( toolBarAnimated ) {

                if( enabled && animatedRect.isNull() && current )
                {
                    QColor glow( _helper->alphaColor( _helper->hoverColor( palette ), toolBarOpacity ) );
                    _helper->slitFocused( glow ).render( rect, painter );
                }

            } else if( hasFocus || mouseOver ) {

                _helper->slitFocused( glow ).render( rect, painter );

            } else if( toolBarTimerActive && current ) {

                _helper->slitFocused( _helper->hoverColor( palette ) ).render( rect, painter );

            }

        }

        return true;
    }

    //______________________________________________________________
    bool Style::drawTabBarPanelButtonToolPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QPalette& palette( option->palette );
        QRect rect( option->rect );

        // adjust rect depending on shape
        const QTabBar* tabBar( static_cast<const QTabBar*>( widget->parent() ) );
        switch( tabBar->shape() )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            rect.adjust( 0, 0, 0, -6 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            rect.adjust( 0, 6, 0, 0 );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            rect.adjust( 0, 0, -6, 0 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            rect.adjust( 6, 0, 0, 0 );
            break;

            default: break;

        }

        const QPalette local( widget->parentWidget() ? widget->parentWidget()->palette() : palette );

        // check whether parent has autofill background flag
        const QWidget* parent = _helper->checkAutoFillBackground( widget );
        if( parent && !qobject_cast<const QDockWidget*>( parent ) ) painter->fillRect( rect, parent->palette().color( parent->backgroundRole() ) );
        else _helper->renderWindowBackground( painter, rect, widget, local );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawPanelScrollAreaCornerPrimitive( const QStyleOption*, QPainter*, const QWidget* widget ) const
    {
        // disable painting of PE_PanelScrollAreaCorner
        // the default implementation fills the rect with the window background color
        // which does not work for windows that have gradients.
        // unfortunately, this does not work when scrollbars are children of QWebView,
        // in which case, false is returned, in order to fall back to the parent style implementation
        return !( widget && widget->inherits( "QWebView" ) );
    }

    //___________________________________________________________________________________
    bool Style::drawPanelMenuPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // do nothing if menu is embedded in another widget
        // this corresponds to having a transparent background
        if( widget && !widget->isWindow() ) return true;

        // cast option and check
        const QStyleOptionMenuItem* menuItemOption( qstyleoption_cast<const QStyleOptionMenuItem*>( option ) );
        if( !( menuItemOption && widget ) ) return true;

        // copy rect and get color
        const QRect& rect = menuItemOption->rect;
        const QColor color = menuItemOption->palette.color( widget->window()->backgroundRole() );
        const bool hasAlpha( _helper->hasAlphaChannel( widget ) );
        if( hasAlpha )
        {

            painter->setCompositionMode( QPainter::CompositionMode_Source );
            _helper->roundCorner( color ).render( rect, painter );

            painter->setCompositionMode( QPainter::CompositionMode_SourceOver );
            painter->setClipPath( _helper->roundedPath( insideMargin( rect, 1 ) ), Qt::IntersectClip );

        }

        _helper->renderMenuBackground( painter, rect, widget, menuItemOption->palette );

        if( hasAlpha ) painter->setClipping( false );
        _helper->drawFloatFrame( painter, rect, color, !hasAlpha );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawPanelTipLabelPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // force registration of widget
        if( widget && widget->window() )
        { _shadowHelper->registerWidget( widget->window(), true ); }

        const QRect& rect( option->rect );
        const QColor color( option->palette.brush( QPalette::ToolTipBase ).color() );
        QColor topColor( _helper->backgroundTopColor( color ) );
        QColor bottomColor( _helper->backgroundBottomColor( color ) );

        // make tooltip semi transparents when possible
        // alpha is copied from "kdebase/apps/dolphin/tooltips/filemetadatatooltip.cpp"
        const bool hasAlpha( _helper->hasAlphaChannel( widget ) );
        if( hasAlpha && StyleConfigData::toolTipTransparent() )
        {
            if( widget && widget->window() )
            { _blurHelper->registerWidget( widget->window() ); }
            topColor.setAlpha( 220 );
            bottomColor.setAlpha( 220 );
        }

        QLinearGradient gradient( 0, rect.top(), 0, rect.bottom() );
        gradient.setColorAt( 0, topColor );
        gradient.setColorAt( 1, bottomColor );

        // contrast pixmap
        QLinearGradient gradient2( 0, rect.top(), 0, rect.bottom() );
        gradient2.setColorAt( 0.5, _helper->calcLightColor( bottomColor ) );
        gradient2.setColorAt( 0.9, bottomColor );

        painter->save();

        if( hasAlpha )
        {
            painter->setRenderHint( QPainter::Antialiasing );

            QRectF local( rect );
            local.adjust( 0.5, 0.5, -0.5, -0.5 );

            painter->setPen( Qt::NoPen );
            painter->setBrush( gradient );
            painter->drawRoundedRect( local, 4, 4 );

            painter->setBrush( Qt::NoBrush );
            painter->setPen( QPen( gradient2, 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawRoundedRect( local, 3.5, 3.5 );

        } else {

            painter->setPen( Qt::NoPen );
            painter->setBrush( gradient );
            painter->drawRect( rect );

            painter->setBrush( Qt::NoBrush );
            painter->setPen( QPen( gradient2, 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawRect( rect );

        }

        painter->restore();

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawPanelItemViewItemPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const auto viewItemOption = qstyleoption_cast<const QStyleOptionViewItem*>( option );

        if( !viewItemOption ) return false;

        // try cast widget
        const auto view = qobject_cast<const QAbstractItemView *>( widget );

        // store palette and rect
        const auto& palette( option->palette );
        QRect rect( option->rect );

        // store flags
        const State& state( option->state );
        const bool mouseOver( ( state & State_MouseOver ) && (!view || view->selectionMode() != QAbstractItemView::NoSelection) );
        const bool selected( state & State_Selected );
        const bool enabled( state & State_Enabled );
        const bool active( state & State_Active );
        const bool hasCustomBackground( viewItemOption->backgroundBrush.style() != Qt::NoBrush && !selected );
        const bool hasSolidBackground( !hasCustomBackground || viewItemOption->backgroundBrush.style() == Qt::SolidPattern );

        if( !mouseOver && !selected && !hasCustomBackground && !( viewItemOption->features & QStyleOptionViewItemV2::Alternate ) )
        { return true; }

        QPalette::ColorGroup colorGroup;
        if( enabled ) colorGroup = active ? QPalette::Normal : QPalette::Inactive;
        else colorGroup = QPalette::Disabled;

        QColor color;
        if( hasCustomBackground && hasSolidBackground ) color = viewItemOption->backgroundBrush.color();
        else color = palette.color( colorGroup, QPalette::Highlight );

        if( mouseOver && !hasCustomBackground )
        {
            if( !selected ) color.setAlphaF( 0.2 );
            else color = color.lighter( 110 );
        }

        if( viewItemOption && ( viewItemOption->features & QStyleOptionViewItemV2::Alternate ) )
        { painter->fillRect( option->rect, palette.brush( colorGroup, QPalette::AlternateBase ) ); }

        if( !mouseOver && !selected && !hasCustomBackground )
        { return true; }

        if( hasCustomBackground && !hasSolidBackground )
        {

            const QPointF oldBrushOrigin = painter->brushOrigin();
            painter->setBrushOrigin( viewItemOption->rect.topLeft() );
            painter->setBrush( viewItemOption->backgroundBrush );
            painter->setPen( Qt::NoPen );
            painter->drawRect( viewItemOption->rect );
            painter->setBrushOrigin( oldBrushOrigin );

        } else {

            // get selection tileset
            TileSet tileSet( _helper->selection( color, rect.height(), hasCustomBackground ) );

            bool roundedLeft  = false;
            bool roundedRight = false;
            if( viewItemOption )
            {

                roundedLeft  = ( viewItemOption->viewItemPosition == QStyleOptionViewItem::Beginning );
                roundedRight = ( viewItemOption->viewItemPosition == QStyleOptionViewItem::End );

                if( viewItemOption->viewItemPosition == QStyleOptionViewItem::OnlyOne ||
                    viewItemOption->viewItemPosition == QStyleOptionViewItem::Invalid ||
                    ( view && view->selectionBehavior() != QAbstractItemView::SelectRows ) )
                {
                    roundedLeft  = true;
                    roundedRight = true;
                }

            }

            const bool reverseLayout( option->direction == Qt::RightToLeft );

            // define tiles
            TileSet::Tiles tiles( TileSet::Center );
            if( !reverseLayout ? roundedLeft : roundedRight ) tiles |= TileSet::Left;
            if( !reverseLayout ? roundedRight : roundedLeft ) tiles |= TileSet::Right;

            // adjust rect and render
            rect = tileSet.adjust( rect, tiles );
            if( rect.isValid() ) tileSet.render( rect, painter, tiles );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorMenuCheckMarkPrimitive( const QStyleOption *option, QPainter *painter, const QWidget * ) const
    {
        const QRect& rect( option->rect );
        const State& state( option->state );
        const QPalette& palette( option->palette );
        const bool enabled( state & State_Enabled );

        StyleOptions styleOptions( NoFill );
        if( !enabled ) styleOptions |= Disabled;

        renderCheckBox( painter, rect, palette, styleOptions, CheckOn );
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorCheckBoxPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // get rect
        const QRect& rect( option->rect );
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );

        StyleOptions styleOptions;
        if( !enabled ) styleOptions |= Disabled;
        if( mouseOver ) styleOptions |= Hover;
        if( hasFocus ) styleOptions |= Focus;

        // get checkbox state
        CheckBoxState checkBoxState;
        if( state & State_NoChange ) checkBoxState = CheckTriState;
        else if( state & State_Sunken ) checkBoxState = CheckSunken;
        else if( state & State_On ) checkBoxState = CheckOn;
        else checkBoxState = CheckOff;

        // match button color to window background
        QPalette palette( option->palette );
        palette.setColor( QPalette::Button, _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );

        // animation state: mouseOver has precedence over focus
        _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

        const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
        const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );
        renderCheckBox( painter, rect, palette, styleOptions, checkBoxState, opacity, mode );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorRadioButtonPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // copy rect
        const QRect& rect( option->rect );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( state & State_HasFocus );

        StyleOptions styleOptions;
        if( !enabled ) styleOptions |= Disabled;
        if( mouseOver ) styleOptions |= Hover;
        if( hasFocus ) styleOptions |= Focus;

        CheckBoxState checkBoxState;
        if( state & State_Sunken ) checkBoxState = CheckSunken;
        else if( state & State_On ) checkBoxState = CheckOn;
        else checkBoxState = CheckOff;

        // match button color to window background
        QPalette palette( option->palette );
        palette.setColor( QPalette::Button, _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );

         // animation state: mouseOver has precedence over focus
        _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

        const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
        const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );
        renderRadioButton( painter, rect, palette, styleOptions, checkBoxState, opacity, mode );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorButtonDropDownPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionToolButton* toolButtonOption( qstyleoption_cast<const QStyleOptionToolButton*>( option ) );
        if( !toolButtonOption ) return true;

        // copy palette and rect
        const QPalette& palette( option->palette );
        const QRect& rect( option->rect );

        // store state
        const State& state( option->state );
        const bool autoRaise( state & State_AutoRaise );

        // do nothing for autoraise buttons
        if( autoRaise || !(toolButtonOption->subControls & SC_ToolButtonMenu) ) return true;

        // store state
        const bool enabled( state & State_Enabled );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool sunken( enabled && ( state & State_Sunken ) );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // match button color to window background
        const QColor highlight( _helper->hoverColor( palette ) );
        const QColor background( _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );
        StyleOptions styleOptions;
        if( hasFocus ) styleOptions |= Focus;
        if( mouseOver ) styleOptions |= Hover;

        // update animation state
        // mouse over takes precedence over focus
        _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
        const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );

        // paint frame
        TileSet::Tiles tiles( TileSet::Ring );
        if( state & ( State_On|State_Sunken ) ) styleOptions |= Sunken;
        if( reverseLayout ) tiles &= ~TileSet::Right;
        else tiles &= ~TileSet::Left;

        painter->setClipRect( rect, Qt::IntersectClip );
        renderButtonSlab( painter, rect, background, styleOptions, opacity, mode, tiles );

        // draw separating vertical line
        const QColor color( palette.color( QPalette::Button ) );
        const QColor light =_helper->alphaColor( _helper->calcLightColor( color ), 0.6 );
        QColor dark = _helper->calcDarkColor( color );
        dark.setAlpha( 200 );

        const int top( rect.top()+ (sunken ? 3:2) );
        const int bottom( rect.bottom()-4 );

        painter->setPen( QPen( light,1 ) );

        if( reverseLayout )
        {

            painter->drawLine( rect.right()+1, top+1, rect.right()+1, bottom );
            painter->drawLine( rect.right()-1, top+2, rect.right()-1, bottom );
            painter->setPen( dark );
            painter->drawLine( rect.right(), top, rect.right(), bottom );

        } else {

            painter->drawLine( rect.left()-1, top+1, rect.left()-1, bottom-1 );
            painter->drawLine( rect.left()+1, top+1, rect.left()+1, bottom-1 );
            painter->setPen( dark );
            painter->drawLine( rect.left(), top, rect.left(), bottom );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorTabClosePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {
        if( _tabCloseIcon.isNull() )
        {
            // load the icon on-demand: in the constructor, KDE is not yet ready to find it!
            _tabCloseIcon = QIcon::fromTheme( QStringLiteral( "dialog-close" ) );
            if( _tabCloseIcon.isNull() ) return false;
        }

        const int size( pixelMetric(QStyle::PM_SmallIconSize) );
        QIcon::Mode mode;
        if( option->state & State_Enabled )
        {
            if( option->state & State_Raised ) mode = QIcon::Active;
            else mode = QIcon::Normal;
        } else mode = QIcon::Disabled;

        if( !(option->state & State_Raised)
            && !(option->state & State_Sunken)
            && !(option->state & QStyle::State_Selected))
            mode = QIcon::Disabled;

        QIcon::State state = option->state & State_Sunken ? QIcon::On:QIcon::Off;
        QPixmap pixmap( _tabCloseIcon.pixmap(size, mode, state) );
        drawItemPixmap( painter, option->rect, Qt::AlignCenter, pixmap );
        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorTabTearPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionTab* tabOption( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOption ) return true;

        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // in fact with current version of Qt ( 4.6.0 ) the cast fails and document mode is always false
        // this will hopefully be fixed in later versions
        const auto tabOptionV3( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        bool documentMode( tabOptionV3 ? tabOptionV3->documentMode : false );

        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : nullptr;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        QRect gradientRect( rect );
        switch( tabOption->shape )
        {

            case QTabBar::TriangularNorth:
            case QTabBar::RoundedNorth:
            gradientRect.adjust( 0, 0, 0, -5 );
            break;

            case QTabBar::TriangularSouth:
            case QTabBar::RoundedSouth:
            gradientRect.adjust( 0, 5, 0, 0 );
            break;

            case QTabBar::TriangularWest:
            case QTabBar::RoundedWest:
            gradientRect.adjust( 0, 0, -5, 0 );
            break;

            case QTabBar::TriangularEast:
            case QTabBar::RoundedEast:
            gradientRect.adjust( 5, 0, 0, 0 );
            break;

            default: return true;
        }

        // fade tabbar
        QPixmap pixmap( gradientRect.size() );
        {
            pixmap.fill( Qt::transparent );
            QPainter painter( &pixmap );

            const bool verticalTabs( isVerticalTab( tabOption ) );

            int width = 0;
            int height = 0;
            if( verticalTabs ) height = gradientRect.height();
            else width = gradientRect.width();

            QLinearGradient grad;
            if( reverseLayout && !verticalTabs ) grad = QLinearGradient( 0, 0, width, height );
            else grad = QLinearGradient( width, height, 0, 0 );

            grad.setColorAt( 0, Qt::transparent );
            grad.setColorAt( 0.6, Qt::black );

            if( widget )
            { _helper->renderWindowBackground( &painter, pixmap.rect(), widget, palette ); }
            painter.setCompositionMode( QPainter::CompositionMode_DestinationAtop );
            painter.fillRect( pixmap.rect(), QBrush( grad ) );
            painter.end();
        }

        // draw pixmap
        painter->drawPixmap( gradientRect.topLeft() + QPoint( 0,-1 ), pixmap );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorToolBarHandlePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // do nothing if disabled from options
        if( !StyleConfigData::toolBarDrawItemSeparator() )
        { return true; }

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool horizontal( state & State_Horizontal );
        int counter( 1 );

        if( horizontal )
        {

            const int center( rect.left()+ rect.width()/2 );
            for( int j = rect.top()+2; j <= rect.bottom()-3; j+=3, ++counter )
            {
                if( counter%2 == 0 ) _helper->renderDot( painter, QPoint( center+1, j ), palette.color( QPalette::Window ) );
                else _helper->renderDot( painter, QPoint( center-2, j ), palette.color( QPalette::Window ) );
            }

        } else {

            const int center( rect.top()+ rect.height()/2 );
            for( int j = rect.left()+2; j <= rect.right()-3; j+=3, ++counter )
            {
                if( counter%2 == 0 ) _helper->renderDot( painter, QPoint( j, center+1 ), palette.color( QPalette::Window ) );
                else _helper->renderDot( painter, QPoint( j, center-2 ), palette.color( QPalette::Window ) );
            }
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorToolBarSeparatorPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        /*
        do nothing if disabled from options
        also need to check if widget is a combobox, because of Qt hack using 'toolbar' separator primitive
        for rendering separators in comboboxes
        */
        if( !( StyleConfigData::toolBarDrawItemSeparator() || qobject_cast<const QComboBox*>( widget ) ) )
        { return true; }

        // store rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool separatorIsVertical( state & State_Horizontal );

        // define color and render
        const QColor color( palette.color( QPalette::Window ) );
        if( separatorIsVertical ) _helper->drawSeparator( painter, rect, color, Qt::Vertical );
        else _helper->drawSeparator( painter, rect, color, Qt::Horizontal );

        return true;
    }

   //___________________________________________________________________________________
    bool Style::drawIndicatorBranchPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // state
        const State& state( option->state );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        //draw expander
        int expanderAdjust = 0;
        if( state & State_Children )
        {

            int sizeLimit = qMin( rect.width(), rect.height() );
            const bool expanderOpen( state & State_Open );

            // make sure size limit is odd
            expanderAdjust = sizeLimit/2 + 1;

            // flags
            const bool enabled( state & State_Enabled );
            const bool mouseOver( enabled && ( state & State_MouseOver ) );

            // color
            const QColor expanderColor( mouseOver ? _helper->hoverColor( palette ):palette.color( QPalette::Text ) );

            // get arrow size from option
            ArrowSize size = ArrowSmall;
            qreal penThickness( 1.2 );
            switch( StyleConfigData::viewTriangularExpanderSize() )
            {
                case StyleConfigData::TE_TINY:
                size = ArrowTiny;
                break;

                default:
                case StyleConfigData::TE_SMALL:
                size = ArrowSmall;
                break;

                case StyleConfigData::TE_NORMAL:
                penThickness = 1.6;
                size = ArrowNormal;
                break;

            }

            // get arrows polygon
            QPolygonF arrow;
            if( expanderOpen ) arrow = genericArrow( ArrowDown, size );
            else arrow = genericArrow( reverseLayout ? ArrowLeft:ArrowRight, size );

            // render
            painter->save();
            painter->translate( QRectF( rect ).center() );
            painter->setPen( QPen( expanderColor, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->setRenderHint( QPainter::Antialiasing );
            painter->drawPolyline( arrow );
            painter->restore();

        }


        // tree branches
        if( !StyleConfigData::viewDrawTreeBranchLines() ) return true;

        const QPoint center( rect.center() );
        const QColor lineColor( KColorUtils::mix( palette.color( QPalette::Text ), palette.color( QPalette::Window ), 0.8 ) );
        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->setPen( lineColor );
        if( state & ( State_Item | State_Children | State_Sibling ) )
        {
            const QLine line( QPoint( center.x(), rect.top() ), QPoint( center.x(), center.y() - expanderAdjust ) );
            painter->drawLine( line );
        }

        //The right/left ( depending on dir ) line gets drawn if we have an item
        if( state & State_Item )
        {
            const QLine line = reverseLayout ?
                QLine( QPoint( rect.left(), center.y() ), QPoint( center.x() - expanderAdjust, center.y() ) ):
                QLine( QPoint( center.x() + expanderAdjust, center.y() ), QPoint( rect.right(), center.y() ) );
            painter->drawLine( line );

        }

        //The bottom if we have a sibling
        if( state & State_Sibling )
        {
            const QLine line( QPoint( center.x(), center.y() + expanderAdjust ), QPoint( center.x(), rect.bottom() ) );
            painter->drawLine( line );
        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawWidgetPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // check widget and attributes
        if( !widget || !widget->testAttribute( Qt::WA_StyledBackground ) || widget->testAttribute( Qt::WA_NoSystemBackground ) ) return false;
        if( !( ( widget->windowFlags() & Qt::WindowType_Mask ) & ( Qt::Window|Qt::Dialog ) ) ) return false;
        if( !widget->isWindow() ) return false;

        // normal "window" background
        const QPalette& palette( option->palette );

        // do not render background if palette brush has a texture (pixmap or image)
        const QBrush brush( palette.brush( widget->backgroundRole() ) );
        if( !( brush.texture().isNull() && brush.textureImage().isNull() ) )
        { return false; }

        _helper->renderWindowBackground( painter, option->rect, widget, palette );
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawPushButtonLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionButton* buttonOption( qstyleoption_cast<const QStyleOptionButton*>( option ) );
        if( !buttonOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool sunken( ( state & State_On ) || ( state & State_Sunken ) );
        const bool mouseOver( enabled && (option->state & State_MouseOver) );
        const bool flat( buttonOption->features & QStyleOptionButton::Flat );

        // content
        const bool hasIcon( !buttonOption->icon.isNull() );
        const bool hasText( !buttonOption->text.isEmpty() );

        // contents
        QRect contentsRect( rect );

        // color role
        const QPalette::ColorRole textRole( flat ? QPalette::WindowText:QPalette::ButtonText );

        // menu arrow
        if( buttonOption->features & QStyleOptionButton::HasMenu )
        {

            // define rect
            QRect arrowRect( contentsRect );
            arrowRect.setLeft( contentsRect.right() - Metrics::MenuButton_IndicatorWidth + 1 );
            arrowRect = centerRect( arrowRect, Metrics::MenuButton_IndicatorWidth, Metrics::MenuButton_IndicatorWidth );

            contentsRect.setRight( arrowRect.left() - Metrics::Button_ItemSpacing - 1  );
            contentsRect.adjust( Metrics::Button_MarginWidth, 0, 0, 0 );

            arrowRect = visualRect( option, arrowRect );

            // arrow
            const qreal penThickness = 1.6;
            QPolygonF arrow = genericArrow( ArrowDown, ArrowNormal );

            const QColor color = palette.color( flat ? QPalette::WindowText:QPalette::ButtonText );
            const QColor background = palette.color( flat ? QPalette::Window:QPalette::Button );

            painter->save();
            painter->translate( QRectF( arrowRect ).center() );
            painter->setRenderHint( QPainter::Antialiasing );

            const qreal offset( qMin( penThickness, qreal( 1 ) ) );
            painter->translate( 0,offset );
            painter->setPen( QPen( _helper->calcLightColor(  background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( arrow );
            painter->translate( 0,-offset );

            painter->setPen( QPen( _helper->decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( arrow );
            painter->restore();

        } else contentsRect = insideMargin( contentsRect, Metrics::Button_MarginWidth, 0 );

        // icon size
        QSize iconSize( buttonOption->iconSize );
        if( !iconSize.isValid() )
        {
            const int metric( pixelMetric( PM_SmallIconSize, option, widget ) );
            iconSize = QSize( metric, metric );
        }

        // text size
        const int textFlags( _mnemonics->textFlags() | Qt::AlignCenter );
        const QSize textSize( option->fontMetrics.size( textFlags, buttonOption->text ) );

        // adjust text and icon rect based on options
        QRect iconRect;
        QRect textRect;

        if( hasText && !hasIcon ) textRect = contentsRect;
        else if( hasIcon && !hasText ) iconRect = contentsRect;
        else {

            const int contentsWidth( iconSize.width() + textSize.width() + Metrics::Button_ItemSpacing );
            iconRect = QRect( QPoint( contentsRect.left() + (contentsRect.width() - contentsWidth )/2, contentsRect.top() + (contentsRect.height() - iconSize.height())/2 ), iconSize );
            textRect = QRect( QPoint( iconRect.right() + Metrics::ToolButton_ItemSpacing + 1, contentsRect.top() + (contentsRect.height() - textSize.height())/2 ), textSize );

        }

        // handle right to left
        if( iconRect.isValid() ) iconRect = visualRect( option, iconRect );
        if( textRect.isValid() ) textRect = visualRect( option, textRect );

        // make sure there is enough room for icon
        if( iconRect.isValid() ) iconRect = centerRect( iconRect, iconSize );

        // render icon
        if( hasIcon && iconRect.isValid() ) {

            // icon state and mode
            const QIcon::State iconState( sunken ? QIcon::On : QIcon::Off );
            QIcon::Mode iconMode;
            if( !enabled ) iconMode = QIcon::Disabled;
            else if( mouseOver && flat ) iconMode = QIcon::Active;
            else iconMode = QIcon::Normal;

            const QPixmap pixmap = buttonOption->icon.pixmap( iconSize, iconMode, iconState );
            drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );

        }

        // render text
        if( hasText && textRect.isValid() )
        { drawItemText( painter, textRect, textFlags, palette, enabled, buttonOption->text, textRole ); }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawToolButtonLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionToolButton* toolButtonOption( qstyleoption_cast<const QStyleOptionToolButton*>(option) );

        // copy rect and palette
        const QRect& rect = option->rect;
        const QPalette& palette = option->palette;

        // state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool sunken( ( state & State_On ) || ( state & State_Sunken ) );
        const bool mouseOver( enabled && (option->state & State_MouseOver) );
        const bool flat( state & State_AutoRaise );

        const bool hasArrow( toolButtonOption->features & QStyleOptionToolButton::Arrow );
        const bool hasIcon( !( hasArrow || toolButtonOption->icon.isNull() ) );
        const bool hasText( !toolButtonOption->text.isEmpty() );

        // icon size
        const QSize iconSize( toolButtonOption->iconSize );

        // text size
        int textFlags( _mnemonics->textFlags() );
        const QSize textSize( option->fontMetrics.size( textFlags, toolButtonOption->text ) );

        // adjust text and icon rect based on options
        QRect iconRect;
        QRect textRect;

        if( hasText && ( !(hasArrow||hasIcon) || toolButtonOption->toolButtonStyle == Qt::ToolButtonTextOnly ) )
        {

            // text only
            textRect = rect;
            textFlags |= Qt::AlignCenter;

        } else if( (hasArrow||hasIcon) && (!hasText || toolButtonOption->toolButtonStyle == Qt::ToolButtonIconOnly ) ) {

            // icon only
            iconRect = rect;

        } else if( toolButtonOption->toolButtonStyle == Qt::ToolButtonTextUnderIcon ) {

            const int contentsHeight( iconSize.height() + textSize.height() + Metrics::ToolButton_ItemSpacing );
            iconRect = QRect( QPoint( rect.left() + (rect.width() - iconSize.width())/2, rect.top() + (rect.height() - contentsHeight)/2 ), iconSize );
            textRect = QRect( QPoint( rect.left() + (rect.width() - textSize.width())/2, iconRect.bottom() + Metrics::ToolButton_ItemSpacing + 1 ), textSize );
            textFlags |= Qt::AlignCenter;

        } else {

            const bool leftAlign( widget && widget->property( PropertyNames::toolButtonAlignment ).toInt() == Qt::AlignLeft );

            if( leftAlign ) {
                  const int marginWidth( Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth + 1 );
                  iconRect = QRect( QPoint( rect.left() + marginWidth, rect.top() + (rect.height() - iconSize.height())/2 ), iconSize );
            }
            else {

                const int contentsWidth( iconSize.width() + textSize.width() + Metrics::ToolButton_ItemSpacing );
                iconRect = QRect( QPoint( rect.left() + (rect.width() - contentsWidth )/2, rect.top() + (rect.height() - iconSize.height())/2 ), iconSize );

            }

            textRect = QRect( QPoint( iconRect.right() + Metrics::ToolButton_ItemSpacing + 1, rect.top() + (rect.height() - textSize.height())/2 ), textSize );

            // handle right to left layouts
            iconRect = visualRect( option, iconRect );
            textRect = visualRect( option, textRect );

            textFlags |= Qt::AlignLeft | Qt::AlignVCenter;

        }

        // make sure there is enough room for icon
        if( iconRect.isValid() ) iconRect = centerRect( iconRect, iconSize );

        // render arrow or icon
        if( hasArrow && iconRect.isValid() )
        {

            QStyleOptionToolButton copy( *toolButtonOption );
            copy.rect = iconRect;
            switch( toolButtonOption->arrowType )
            {
                case Qt::LeftArrow: drawPrimitive( PE_IndicatorArrowLeft, &copy, painter, widget ); break;
                case Qt::RightArrow: drawPrimitive( PE_IndicatorArrowRight, &copy, painter, widget ); break;
                case Qt::UpArrow: drawPrimitive( PE_IndicatorArrowUp, &copy, painter, widget ); break;
                case Qt::DownArrow: drawPrimitive( PE_IndicatorArrowDown, &copy, painter, widget ); break;
                default: break;
            }

        } else if( hasIcon && iconRect.isValid() ) {

            // icon state and mode
            const QIcon::State iconState( sunken ? QIcon::On : QIcon::Off );
            QIcon::Mode iconMode;
            if( !enabled ) iconMode = QIcon::Disabled;
            else if( mouseOver && flat ) iconMode = QIcon::Active;
            else iconMode = QIcon::Normal;

            const QPixmap pixmap = toolButtonOption->icon.pixmap( iconSize, iconMode, iconState );
            drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );

        }

        // render text
        if( hasText && textRect.isValid() )
        {

            const QPalette::ColorRole textRole( flat ? QPalette::WindowText: QPalette::ButtonText );

            painter->setFont(toolButtonOption->font);
            drawItemText( painter, textRect, textFlags, palette, enabled, toolButtonOption->text, textRole );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawMenuBarItemControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if( !menuItemOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool selected( enabled && (state & State_Selected) );
        const bool sunken( state & State_Sunken );
        const bool useStrongFocus = StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG;

        if( enabled )
        {
            const bool active( state & State_Selected );
            const bool animated( _animations->menuBarEngine().isAnimated( widget, rect.topLeft() ) );
            const qreal opacity( _animations->menuBarEngine().opacity( widget, rect.topLeft() ) );
            const QRect currentRect( _animations->menuBarEngine().currentRect( widget, rect.topLeft() ) );
            const QRect animatedRect( _animations->menuBarEngine().animatedRect( widget ) );

            const bool intersected( animatedRect.intersects( rect ) );
            const bool current( currentRect.contains( rect.topLeft() ) );
            const bool timerIsActive( _animations->menuBarEngine().isTimerActive( widget ) );

            // do nothing in case of empty intersection between animated rect and current
            if( ( intersected || !animated || animatedRect.isNull() ) && ( active || animated || timerIsActive ) )
            {

                QColor color( _helper->calcMidColor( palette.color( QPalette::Window ) ) );
                if( StyleConfigData::menuHighlightMode() != StyleConfigData::MM_DARK )
                {

                    if( state & State_Sunken )
                    {

                        if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG ) color = palette.color( QPalette::Highlight );
                        else color = KColorUtils::mix( color, KColorUtils::tint( color, palette.color( QPalette::Highlight ), 0.6 ) );

                    } else {

                        if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG ) color = KColorUtils::tint( color, _helper->hoverColor( palette ) );
                        else color = KColorUtils::mix( color, KColorUtils::tint( color, _helper->hoverColor( palette ) ) );
                    }

                } else color = _helper->backgroundColor( color, widget, rect.center() );

                // drawing
                if( animated && intersected )
                {

                    _helper->holeFlat( color, 0 ).render( insideMargin( animatedRect, 1 ), painter, TileSet::Full );

                } else if( timerIsActive && current ) {

                    _helper->holeFlat( color, 0 ).render( insideMargin( rect, 1 ), painter, TileSet::Full );

                } else if( animated && current ) {

                    color.setAlphaF( opacity );
                    _helper->holeFlat( color, 0 ).render( insideMargin( rect, 1 ), painter, TileSet::Full );

                } else if( active ) {

                    _helper->holeFlat( color, 0 ).render( insideMargin( rect, 1 ), painter, TileSet::Full );

                }

            }

        }
        /*
        check if item as an icon, in which case only the icon should be rendered
        consistently with comment in QMenuBarPrivate::calcActionRects
        */
        if( !menuItemOption->icon.isNull() )
        {

            // icon size is forced to SmallIconSize
            const auto iconSize = pixelMetric(QStyle::PM_SmallIconSize, nullptr, widget);
            const auto iconRect = centerRect( rect, iconSize, iconSize );

            // decide icon mode and state
            QIcon::Mode iconMode;
            QIcon::State iconState;
            if( !enabled )
            {
                iconMode = QIcon::Disabled;
                iconState = QIcon::Off;

            } else {

                if( useStrongFocus && sunken ) iconMode = QIcon::Selected;
                else if( selected ) iconMode = QIcon::Active;
                else iconMode = QIcon::Normal;

                iconState = sunken ? QIcon::On : QIcon::Off;

            }

            const auto pixmap = menuItemOption->icon.pixmap( iconSize, iconMode, iconState );
            drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );


        } else {

            // text role
            QPalette::ColorRole role( QPalette::WindowText );
            if( useStrongFocus && sunken && enabled )
            { role = QPalette::HighlightedText; }

            // text flags
            const int textFlags( Qt::AlignCenter|_mnemonics->textFlags() );
            drawItemText( painter, rect, textFlags, palette, enabled, menuItemOption->text, role );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawMenuItemControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if( !menuItemOption ) return true;
        if( menuItemOption->menuItemType == QStyleOptionMenuItem::EmptyArea ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // render background
        renderMenuItemBackground( option, painter, widget );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool selected( enabled && (state & State_Selected) );
        const bool sunken( enabled && (state & (State_On|State_Sunken) ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // Active indicator
        if( selected )
        {

            // check if there is a 'sliding' animation in progress, in which case, do nothing
            const QRect animatedRect( _animations->menuEngine().animatedRect( widget ) );
            if( animatedRect.isNull() )
            {

                const bool animated( _animations->menuEngine().isAnimated( widget, Current ) );
                const QRect currentRect( _animations->menuEngine().currentRect( widget, Current ) );
                const bool intersected( currentRect.contains( rect.topLeft() ) );

                const QColor color( _helper->menuBackgroundColor( _helper->calcMidColor( palette.color( QPalette::Window ) ), widget, rect.center() ) );

                if( animated && intersected ) renderMenuItemRect( option, rect, color, palette, painter, _animations->menuEngine().opacity( widget, Current ) );
                else renderMenuItemRect( option, rect, color, palette, painter );

            }

        }

        // get rect available for contents
        QRect contentsRect( insideMargin( rect,  Metrics::MenuItem_MarginWidth ) );

        // deal with separators
        if( menuItemOption->menuItemType == QStyleOptionMenuItem::Separator )
        {

            // normal separator
            if( menuItemOption->text.isEmpty() && menuItemOption->icon.isNull() )
            {

                // in all other cases draw regular separator
                const QColor color( _helper->menuBackgroundColor( palette.color( QPalette::Window ), widget, rect.center() ) );
                _helper->drawSeparator( painter, rect, color, Qt::Horizontal );
                return true;

            } else {

                // separator can have a title and an icon
                // in that case they are rendered as sunken flat toolbuttons
                QStyleOptionToolButton toolButtonOption( separatorMenuItemOption( menuItemOption, widget ) );
                toolButtonOption.state = State_On|State_Sunken|State_Enabled;
                drawComplexControl( CC_ToolButton, &toolButtonOption, painter, widget );
                return true;

            }

        }

        // define relevant rectangles
        // checkbox
        QRect checkBoxRect;
        if( menuItemOption->menuHasCheckableItems )
        {
            checkBoxRect = QRect( contentsRect.left(), contentsRect.top() + (contentsRect.height()-Metrics::CheckBox_Size)/2 - 1, Metrics::CheckBox_Size, Metrics::CheckBox_Size );
            contentsRect.setLeft( checkBoxRect.right() + Metrics::MenuItem_ItemSpacing + 1 );
        }

        // render checkbox indicator
        const CheckBoxState checkBoxState( menuItemOption->checked ? CheckOn:CheckOff );
        if( menuItemOption->checkType == QStyleOptionMenuItem::NonExclusive )
        {

            checkBoxRect = visualRect( option, checkBoxRect );

            StyleOptions styleOptions;
            styleOptions |= Sunken;
            if( !enabled ) styleOptions |= Disabled;
            if( mouseOver ) styleOptions |= Hover;
            if( hasFocus ) styleOptions |= Focus;

            QPalette localPalette( palette );
            localPalette.setColor( QPalette::Window, _helper->menuBackgroundColor( palette.color( QPalette::Window ), widget, rect.topLeft() ) );
            renderCheckBox( painter, checkBoxRect, localPalette, styleOptions, checkBoxState );

        } else if( menuItemOption->checkType == QStyleOptionMenuItem::Exclusive ) {

            checkBoxRect = visualRect( option, checkBoxRect );

            StyleOptions styleOptions;
            if( !enabled ) styleOptions |= Disabled;
            if( mouseOver ) styleOptions |= Hover;
            if( hasFocus ) styleOptions |= Focus;

            QPalette localPalette( palette );
            localPalette.setColor( QPalette::Window, _helper->menuBackgroundColor( palette.color( QPalette::Window ), widget, rect.topLeft() ) );
            renderRadioButton( painter, checkBoxRect, localPalette, styleOptions, checkBoxState );

        }

        // icon
        const int iconWidth( isQtQuickControl( option, widget ) ? qMax( pixelMetric(PM_SmallIconSize, option, widget ), menuItemOption->maxIconWidth ) : menuItemOption->maxIconWidth );

        QRect iconRect( contentsRect.left(), contentsRect.top() + (contentsRect.height()-iconWidth)/2, iconWidth, iconWidth );
        contentsRect.setLeft( iconRect.right() + Metrics::MenuItem_ItemSpacing + 1 );

        if( !menuItemOption->icon.isNull() )
        {

            const QSize iconSize( pixelMetric( PM_SmallIconSize, option, widget ), pixelMetric( PM_SmallIconSize, option, widget ) );
            iconRect = centerRect( iconRect, iconSize );
            iconRect = visualRect( option, iconRect );

            // icon mode
            QIcon::Mode mode;
            if( (StyleConfigData::menuHighlightMode() != StyleConfigData::MM_DARK) && selected ) mode = QIcon::Selected;
            else if( selected ) mode = QIcon::Active;
            else if( enabled ) mode = QIcon::Normal;
            else mode = QIcon::Disabled;

            // icon state
            const QIcon::State iconState( sunken ? QIcon::On:QIcon::Off );
            const QPixmap icon = menuItemOption->icon.pixmap( iconRect.size(), mode, iconState );
            painter->drawPixmap( iconRect, icon );

        }

        // text role
        const QPalette::ColorRole textRole( ( selected && StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG ) ?
            QPalette::HighlightedText:
            QPalette::WindowText );

        QRect arrowRect( contentsRect.right() - Metrics::MenuButton_IndicatorWidth + 1, contentsRect.top() + (contentsRect.height()-Metrics::MenuButton_IndicatorWidth)/2, Metrics::MenuButton_IndicatorWidth, Metrics::MenuButton_IndicatorWidth );
        contentsRect.setRight( arrowRect.left() -  Metrics::MenuItem_ItemSpacing - 1 );

        // arrow
        if( menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu )
        {

            const qreal penThickness = 1.6;
            const QColor color = palette.color( textRole );
            const QColor background = palette.color( QPalette::Window );

            // get arrow shape
            QPolygonF arrow = genericArrow( option->direction == Qt::LeftToRight ? ArrowRight : ArrowLeft, ArrowNormal );

            painter->save();
            painter->translate( QRectF( arrowRect ).center() );
            painter->setRenderHint( QPainter::Antialiasing );

            // white outline
            const qreal offset( qMin( penThickness, qreal( 1 ) ) );
            painter->translate( 0,offset );
            painter->setPen( QPen( _helper->calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( arrow );
            painter->translate( 0,-offset );

            painter->setPen( QPen( _helper->decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( arrow );
            painter->restore();

        }

        // text
        QRect textRect = contentsRect;
        if( !menuItemOption->text.isEmpty() )
        {

            // adjust textRect
            QString text = menuItemOption->text;
            textRect = centerRect( textRect, textRect.width(), option->fontMetrics.size( _mnemonics->textFlags(), text ).height() );
            textRect = visualRect( option, textRect );

            // set font
            painter->setFont( menuItemOption->font );

            // locate accelerator and render
            const int tabPosition( text.indexOf( QLatin1Char( '\t' ) ) );
            if( tabPosition >= 0 )
            {
                QString accelerator( text.mid( tabPosition + 1 ) );
                text = text.left( tabPosition );
                drawItemText( painter, textRect, Qt::AlignRight | Qt::AlignVCenter | _mnemonics->textFlags(), palette, enabled, accelerator, textRole );
            }

            // render text
            const int textFlags( Qt::AlignVCenter | (reverseLayout ? Qt::AlignRight : Qt::AlignLeft ) | _mnemonics->textFlags() );
            drawItemText( painter, textRect, textFlags, palette, enabled, text, textRole );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionProgressBar* progressBarOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
        if( !progressBarOption ) return true;

        QStyleOptionProgressBar progressBarOption2 = *progressBarOption;

        progressBarOption2.rect = subElementRect( SE_ProgressBarGroove, progressBarOption, widget );
        drawControl( CE_ProgressBarGroove, &progressBarOption2, painter, widget );

        // enable busy animations
        const QObject* styleObject( widget ? widget:progressBarOption->styleObject );

        if( styleObject && _animations->busyIndicatorEngine().enabled() )
        {

            // register QML object if defined
            if( !widget && progressBarOption->styleObject )
            { _animations->busyIndicatorEngine().registerWidget( progressBarOption->styleObject ); }

            _animations->busyIndicatorEngine().setAnimated( styleObject, progressBarOption->maximum == 0 && progressBarOption->minimum == 0 );

        }

        if( _animations->busyIndicatorEngine().isAnimated( styleObject ) )
        { progressBarOption2.progress = _animations->busyIndicatorEngine().value(); }

        // render contents
        progressBarOption2.rect = subElementRect( SE_ProgressBarContents, progressBarOption, widget );
        drawControl( CE_ProgressBarContents, &progressBarOption2, painter, widget );

        // render text
        const bool textVisible( progressBarOption->textVisible );
        const bool busy( progressBarOption->minimum == 0 && progressBarOption->maximum == 0 );
        if( textVisible && !busy )
        {
            progressBarOption2.rect = subElementRect( SE_ProgressBarLabel, progressBarOption, widget );
            drawControl( CE_ProgressBarLabel, &progressBarOption2, painter, widget );
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarContentsControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionProgressBar* progressBarOption = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        if( !progressBarOption ) return true;

        // get orientation
        const auto progressBarOption2 = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        const bool horizontal( !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal );

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // make sure rect is large enough
        if( rect.isValid() )
        {
            // calculate dimension
            int dimension( 20 );
            if( progressBarOption2 ) dimension = qMax( 5, horizontal ? rect.height() : rect.width() );
            _helper->progressBarIndicator( palette, dimension ).render( rect, painter, TileSet::Full );
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarGrooveControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const auto progressBarOption = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        const Qt::Orientation orientation( progressBarOption? progressBarOption->orientation : Qt::Horizontal );
        renderScrollBarHole( painter, option->rect, option->palette.color( QPalette::Window ), orientation );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionProgressBar* progressBarOption = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        if( !progressBarOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool reverseLayout = ( option->direction == Qt::RightToLeft );

        // get orientation
        const auto progressBarOption2 = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        const bool horizontal = !progressBarOption2 || progressBarOption2->orientation == Qt::Horizontal;

        // check inverted appearance
        const bool inverted( progressBarOption2 ? progressBarOption2->invertedAppearance : false );

        // rotate label for vertical layout
        QTransform transform;
        if( !horizontal )
        {
            if( reverseLayout ) transform.rotate( -90 );
            else transform.rotate( 90 );
        }

        painter->setTransform( transform );
        const QRect progressRect( transform.inverted().mapRect( subElementRect( SE_ProgressBarContents, progressBarOption, widget ) ) );
        QRect textRect( transform.inverted().mapRect( rect ) );

        Qt::Alignment hAlign( ( progressBarOption->textAlignment == Qt::AlignLeft ) ? Qt::AlignHCenter : progressBarOption->textAlignment );

        /*
        Figure out the geometry of the indicator.
        This is copied from drawProgressBarContentsControl
        */
        if( progressRect.isValid() )
        {
            // first pass ( normal )
            QRect textClipRect( textRect );

            if( horizontal )
            {

                if( (reverseLayout && !inverted) || (inverted && !reverseLayout) ) textClipRect.setRight( progressRect.left() );
                else textClipRect.setLeft( progressRect.right() + 1 );

            } else if( (reverseLayout && !inverted) || (inverted && !reverseLayout) ) textClipRect.setLeft( progressRect.right() + 1 );
            else textClipRect.setRight( progressRect.left() );

            painter->setClipRect( textClipRect );
            drawItemText( painter, textRect, Qt::AlignVCenter | hAlign, palette, enabled, progressBarOption->text, QPalette::WindowText );

            // second pass ( highlighted )
            painter->setClipRect( progressRect );
            drawItemText( painter, textRect, Qt::AlignVCenter | hAlign, palette, enabled, progressBarOption->text, QPalette::HighlightedText );

        } else {

            drawItemText( painter, textRect, Qt::AlignVCenter | hAlign, palette, enabled, progressBarOption->text, QPalette::WindowText );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarSliderControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSlider *sliderOption = qstyleoption_cast<const QStyleOptionSlider *>( option );
        if( !sliderOption ) return true;

        // store rect and palette
        QRect rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const Qt::Orientation orientation( (state & State_Horizontal) ? Qt::Horizontal : Qt::Vertical );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );

        // update animations
        _animations->scrollBarEngine().updateState( widget, enabled && ( sliderOption->activeSubControls & SC_ScrollBarSlider ) );
        const bool animated( enabled && _animations->scrollBarEngine().isAnimated( widget, SC_ScrollBarSlider ) );

        if( orientation == Qt::Horizontal ) rect = insideMargin( rect, 0, 1 );
        else rect = insideMargin( rect, 1, 0 );

        // render
        if( animated ) renderScrollBarHandle( painter, rect, palette, orientation, mouseOver, _animations->scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
        else renderScrollBarHandle( painter, rect, palette, orientation, mouseOver );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarAddLineControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // do nothing if no buttons are defined
        if( _addLineButtons == NoButton ) return true;

        // cast option and check
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return true;

        const State& state( option->state );
        const bool horizontal( state & State_Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // colors
        const QPalette& palette( option->palette );
        const QColor background( palette.color( QPalette::Window ) );

        // adjust rect, based on number of buttons to be drawn
        const QRect rect( scrollBarInternalSubControlRect( sliderOption, SC_ScrollBarAddLine ) );

        QColor color;
        QStyleOptionSlider copy( *sliderOption );
        if( _addLineButtons == DoubleButton )
        {

            if( horizontal )
            {

                //Draw the arrows
                const QSize halfSize( rect.width()/2, rect.height() );
                const QRect leftSubButton( rect.topLeft(), halfSize );
                const QRect rightSubButton( leftSubButton.topRight() + QPoint( 1, 0 ), halfSize );

                copy.rect = leftSubButton;
                color = scrollBarArrowColor( &copy,  reverseLayout ? SC_ScrollBarAddLine:SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, leftSubButton, color, background, ArrowLeft );

                copy.rect = rightSubButton;
                color = scrollBarArrowColor( &copy,  reverseLayout ? SC_ScrollBarSubLine:SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, rightSubButton, color, background, ArrowRight );

            } else {

                const QSize halfSize( rect.width(), rect.height()/2 );
                const QRect topSubButton( rect.topLeft(), halfSize );
                const QRect botSubButton( topSubButton.bottomLeft() + QPoint( 0, 1 ), halfSize );

                copy.rect = topSubButton;
                color = scrollBarArrowColor( &copy, SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, topSubButton, color, background, ArrowUp );

                copy.rect = botSubButton;
                color = scrollBarArrowColor( &copy, SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, botSubButton, color, background, ArrowDown );

            }

        } else if( _addLineButtons == SingleButton ) {

            copy.rect = rect;
            color = scrollBarArrowColor( &copy,  SC_ScrollBarAddLine, widget );
            if( horizontal ) renderScrollBarArrow( painter, rect, color, background, reverseLayout ? ArrowLeft : ArrowRight );
            else renderScrollBarArrow( painter, rect, color, background, ArrowDown );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarSubLineControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // do nothing if no buttons are set
        if( _subLineButtons == NoButton ) return true;

        // cast option and check
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return true;

        const State& state( option->state );
        const bool horizontal( state & State_Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // colors
        const QPalette& palette( option->palette );
        const QColor background( palette.color( QPalette::Window ) );


        // adjust rect, based on number of buttons to be drawn
        QRect rect( scrollBarInternalSubControlRect( sliderOption, SC_ScrollBarSubLine ) );

        QColor color;
        QStyleOptionSlider copy( *sliderOption );
        if( _subLineButtons == DoubleButton )
        {

            if( horizontal )
            {

                //Draw the arrows
                const QSize halfSize( rect.width()/2, rect.height() );
                const QRect leftSubButton( rect.topLeft(), halfSize );
                const QRect rightSubButton( leftSubButton.topRight() + QPoint( 1, 0 ), halfSize );

                copy.rect = leftSubButton;
                color = scrollBarArrowColor( &copy,  reverseLayout ? SC_ScrollBarAddLine:SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, leftSubButton, color, background, ArrowLeft );

                copy.rect = rightSubButton;
                color = scrollBarArrowColor( &copy,  reverseLayout ? SC_ScrollBarSubLine:SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, rightSubButton, color, background, ArrowRight );

            } else {

                const QSize halfSize( rect.width(), rect.height()/2 );
                const QRect topSubButton( rect.topLeft(), halfSize );
                const QRect botSubButton( topSubButton.bottomLeft() + QPoint( 0, 1 ), halfSize );

                copy.rect = topSubButton;
                color = scrollBarArrowColor( &copy, SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, topSubButton, color, background, ArrowUp );

                copy.rect = botSubButton;
                color = scrollBarArrowColor( &copy, SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, botSubButton, color, background, ArrowDown );

            }

        } else if( _subLineButtons == SingleButton ) {

            copy.rect = rect;
            color = scrollBarArrowColor( &copy,  SC_ScrollBarSubLine, widget );
            if( horizontal ) renderScrollBarArrow( painter, rect, color, background, reverseLayout ? ArrowRight : ArrowLeft );
            else renderScrollBarArrow( painter, rect, color, background, ArrowUp );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawShapedFrameControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const auto frameOption = qstyleoption_cast<const QStyleOptionFrame*>( option );
        if( !frameOption ) return false;

        switch( frameOption->frameShape )
        {

            case QFrame::Box:
            {
                if( option->state & State_Sunken ) return true;
                else break;
            }

            case QFrame::HLine:
            {
                const QColor color( _helper->backgroundColor( option->palette.color( QPalette::Window ), widget, option->rect.center() ) );
                _helper->drawSeparator( painter, option->rect, color, Qt::Horizontal );
                return true;
            }

            case QFrame::VLine:
            {
                const QColor color( _helper->backgroundColor( option->palette.color( QPalette::Window ), widget, option->rect.center() ) );
                _helper->drawSeparator( painter, option->rect, color, Qt::Vertical );
                return true;
            }

            case QFrame::StyledPanel:
            {
                if( isQtQuickControl( option, widget ) )
                {

                    // ComboBox popup frame
                    drawFrameMenuPrimitive( option, painter, widget );
                    return true;

                } else break;
            }

            default: break;

        }

        return false;

    }

    //___________________________________________________________________________________
    bool Style::drawRubberBandControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const QPalette& palette( option->palette );
        const QRect rect( option->rect );

        QColor color = palette.color( QPalette::Highlight );
        painter->setPen( KColorUtils::mix( color, palette.color( QPalette::Active, QPalette::WindowText ) ) );
        color.setAlpha( 50 );
        painter->setBrush( color );
        painter->setClipRegion( rect );
        painter->drawRect( rect.adjusted( 0, 0, -1, -1 ) );
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawHeaderSectionControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionHeader* headerOption( qstyleoption_cast<const QStyleOptionHeader *>( option ) );
        if( !headerOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const bool horizontal( headerOption->orientation == Qt::Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );
        const bool isFirst( horizontal && ( headerOption->position == QStyleOptionHeader::Beginning ) );
        const bool isCorner( widget && widget->inherits( "QTableCornerButton" ) );

        // corner header lines
        if( isCorner )
        {

            if( widget ) _helper->renderWindowBackground( painter, rect, widget, palette );
            else painter->fillRect( rect, palette.color( QPalette::Window ) );
            if( reverseLayout ) renderHeaderLines( rect, palette, painter, TileSet::BottomLeft );
            else renderHeaderLines( rect, palette, painter, TileSet::BottomRight );

        } else renderHeaderBackground( rect, palette, painter, widget, horizontal, reverseLayout );

        // dots
        const QColor color( palette.color( QPalette::Window ) );
        if( horizontal )
        {

            if( headerOption->section != 0 || isFirst )
            {
                const int center( rect.center().y() );
                const int pos( reverseLayout ? rect.left()+1 : rect.right()-1 );
                _helper->renderDot( painter, QPoint( pos, center-3 ), color );
                _helper->renderDot( painter, QPoint( pos, center ), color );
                _helper->renderDot( painter, QPoint( pos, center+3 ), color );
            }

        } else {

            const int center( rect.center().x() );
            const int pos( rect.bottom()-1 );
            _helper->renderDot( painter, QPoint( center-3, pos ), color );
            _helper->renderDot( painter, QPoint( center, pos ), color );
            _helper->renderDot( painter, QPoint( center+3, pos ), color );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawHeaderEmptyAreaControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // use the same background as in drawHeaderPrimitive
        QPalette palette( option->palette );

        if( widget && _animations->widgetEnableStateEngine().isAnimated( widget, AnimationEnable ) )
        { palette = _helper->disabledPalette( palette, _animations->widgetEnableStateEngine().opacity( widget, AnimationEnable )  ); }

        const bool horizontal( option->state & QStyle::State_Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );
        renderHeaderBackground( option->rect, palette, painter, widget, horizontal, reverseLayout );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // call parent style method
        KStyle::drawControl( CE_TabBarTabLabel, option, painter, widget );

        // store rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // check focus
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool selected( state & State_Selected );
        const bool hasFocus( enabled && selected && (state & State_HasFocus) );

        // update mouse over animation state
        _animations->tabBarEngine().updateState( widget, rect.topLeft(), AnimationFocus, hasFocus );
        const bool animated( enabled && selected && _animations->tabBarEngine().isAnimated( widget, rect.topLeft(), AnimationFocus ) );
        const qreal opacity( _animations->tabBarEngine().opacity( widget, rect.topLeft(), AnimationFocus ) );

        if( !( hasFocus || animated ) ) return true;

        // code is copied from QCommonStyle, but adds focus
        // cast option and check
        const QStyleOptionTab *tabOption( qstyleoption_cast<const QStyleOptionTab*>(option) );
        if( !tabOption || tabOption->text.isEmpty() ) return true;

        // tab option rect
        const bool verticalTabs( isVerticalTab( tabOption ) );
        const int textFlags( Qt::AlignCenter | _mnemonics->textFlags() );

        // text rect
        QRect textRect( subElementRect(SE_TabBarTabText, option, widget) );

        if( verticalTabs )
        {

            // properly rotate painter
            painter->save();
            int newX, newY, newRot;
            if( tabOption->shape == QTabBar::RoundedEast || tabOption->shape == QTabBar::TriangularEast)
            {

                newX = rect.width() + rect.x();
                newY = rect.y();
                newRot = 90;

            } else {

                newX = rect.x();
                newY = rect.y() + rect.height();
                newRot = -90;

            }

            QTransform transform;
            transform.translate( newX, newY );
            transform.rotate(newRot);
            painter->setTransform( transform, true );

        }

        // adjust text rect based on font metrics
        textRect = option->fontMetrics.boundingRect( textRect, textFlags, tabOption->text );

        // focus color
        QColor focusColor;
        if( animated ) focusColor = _helper->alphaColor( _helper->focusColor( palette ), opacity );
        else if( hasFocus ) focusColor =  _helper->focusColor( palette );

        // render focus line
        if( focusColor.isValid() )
        {
            painter->save();
            painter->setRenderHint( QPainter::Antialiasing, false );
            painter->setBrush( Qt::NoBrush );
            painter->setPen( focusColor );

            painter->translate( 0, 2 );
            painter->drawLine( textRect.bottomLeft(), textRect.bottomRight() );
            painter->restore();
        }

        if( verticalTabs ) painter->restore();

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabShapeControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // check if tabwidget is selected and redirect
        const State& state( option->state );
        const bool selected( state & State_Selected );
        if( selected ) return drawTabBarTabShapeControl_selected( option, painter, widget );
        else return drawTabBarTabShapeControl_unselected( option, painter, widget );
    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabShapeControl_selected( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionTab* tabOption( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // tab position and state
        const QStyleOptionTab::TabPosition& position = tabOption->position;
        bool isFirst( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::Beginning );
        bool isLast( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::End );

        // document mode
        const  auto tabOptionV3 = qstyleoption_cast<const QStyleOptionTab*>( option );
        bool documentMode = tabOptionV3 ? tabOptionV3->documentMode : false;
        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : nullptr;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        // this is needed to complete the base frame when there are widgets in tabbar
        const QTabBar* tabBar( qobject_cast<const QTabBar*>( widget ) );
        const QRect tabBarRect( tabBar ? tabBar->rect():QRect() );

        // check if tab is being dragged
        const bool isDragged( painter->device() != tabBar );

        // hover and animation state
        /* all are disabled when tabBar is locked ( drag in progress ) */
        const bool tabBarLocked( _tabBarData->locks( tabBar ) );
        const bool mouseOver( enabled && !tabBarLocked && ( state & State_MouseOver ) );

        // animation state
        _animations->tabBarEngine().updateState( widget, rect.topLeft(), AnimationHover, mouseOver );

        // handle base frame painting, for tabbars in which tab is being dragged
        _tabBarData->drawTabBarBaseControl( tabOption, painter, widget );
        if( tabBar && isDragged ) _tabBarData->lock( tabBar );
        else if( _tabBarData->locks( tabBar ) ) _tabBarData->release();

        // corner widgets
        const bool verticalTabs( isVerticalTab( tabOption ) );
        const bool hasLeftCornerWidget( (tabOption->cornerWidgets & QStyleOptionTab::LeftCornerWidget) && !verticalTabs );

        // true if widget is aligned to the frame
        const bool isFrameAligned( !documentMode && isFirst && !hasLeftCornerWidget && !isDragged );

        // swap state based on reverse layout, so that they become layout independent
        if( reverseLayout && !verticalTabs ) qSwap( isFirst, isLast );

        // part of the tab in which the text is drawn
        QRect tabRect( insideMargin( rect, -1 ) );
        SlabRect tabSlab( insideMargin( rect, -1 ), TileSet::Ring );

        // connection to the frame
        SlabRect::List slabs;

        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {

                tabSlab.tiles &= ~TileSet::Bottom;
                tabSlab.rect.adjust( 0, 0, 0, 2 );
                if( isDragged ) break;

                // left side
                if( isFrameAligned && !reverseLayout )
                {

                    QRect frameRect( rect );
                    frameRect.setLeft( frameRect.left() - 1 );
                    frameRect.setRight( tabRect.left() );
                    frameRect.setTop( tabRect.bottom() - 4 );
                    slabs << SlabRect( frameRect, TileSet::Left ).adjusted( TileSet::DefaultSize );

                } else {

                    QRect frameRect( rect );
                    frameRect.setRight( tabRect.left() + 3 );
                    frameRect.setTop( rect.bottom() - TileSet::DefaultSize + 1 );
                    slabs << SlabRect( frameRect, TileSet::Top ).adjusted( TileSet::DefaultSize );

                }

                // right side
                if( isFrameAligned && reverseLayout )
                {

                    QRect frameRect( rect );
                    frameRect.setLeft( tabRect.right() );
                    frameRect.setRight( frameRect.right() + 1 );
                    frameRect.setTop( tabRect.bottom() - 4 );
                    slabs << SlabRect( frameRect, TileSet::Right ).adjusted( TileSet::DefaultSize );

                } else {

                    QRect frameRect( rect );
                    frameRect.setLeft( tabRect.right() - 3 );
                    frameRect.setTop( rect.bottom() - TileSet::DefaultSize + 1 );
                    slabs << SlabRect( frameRect, TileSet::Top ).adjusted( TileSet::DefaultSize );

                }

                // extra base, to extend below inactive tabs and buttons
                if( tabBar )
                {
                    QRect frameRect( rect );
                    frameRect.setTop( rect.bottom() - Metrics::TabBar_BaseOverlap + 1 );

                    if( rect.left() > tabBarRect.left() )
                    {

                        frameRect.setLeft( tabBarRect.left() - Metrics::TabBar_BaseOverlap );
                        frameRect.setRight( rect.left() + Metrics::TabBar_BaseOverlap - 1 );
                        if( documentMode || reverseLayout ) slabs << SlabRect( frameRect, TileSet::Top );
                        else slabs << SlabRect( frameRect, TileSet::TopLeft );

                    }

                    if( rect.right() < tabBarRect.right() )
                    {

                        frameRect.setLeft( rect.right() - Metrics::TabBar_BaseOverlap + 1 );
                        frameRect.setRight( tabBarRect.right() + Metrics::TabBar_BaseOverlap );
                        if( documentMode || !reverseLayout ) slabs << SlabRect( frameRect, TileSet::Top );
                        else slabs << SlabRect( frameRect, TileSet::TopRight );

                    }

                }

                break;

            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {

                tabSlab.tiles &= ~TileSet::Top;
                tabSlab.rect.adjust( 0, -2, 0, 0 );
                if( isDragged ) break;

                // left side
                if( isFrameAligned && !reverseLayout )
                {

                    QRect frameRect( rect );
                    frameRect.setLeft( frameRect.left() - 1 );
                    frameRect.setRight( tabRect.left() );
                    frameRect.setBottom( tabRect.top() + 4 );
                    slabs << SlabRect( frameRect, TileSet::Left ).adjusted( TileSet::DefaultSize );

                } else {

                    QRect frameRect( rect );
                    frameRect.setRight( tabRect.left() + 3 );
                    frameRect.setBottom( rect.top() + TileSet::DefaultSize - 1 );
                    slabs << SlabRect( frameRect, TileSet::Bottom ).adjusted( TileSet::DefaultSize );
                }

                // right side
                if( isFrameAligned && reverseLayout )
                {

                    QRect frameRect( rect );
                    frameRect.setLeft( tabRect.right() );
                    frameRect.setRight( frameRect.right() + 1 );
                    frameRect.setBottom( tabRect.top() + 4 );
                    slabs << SlabRect( frameRect, TileSet::Right ).adjusted( TileSet::DefaultSize );

                } else {

                    QRect frameRect( rect );
                    frameRect.setLeft( tabRect.right() - 3 );
                    frameRect.setBottom( rect.top() + TileSet::DefaultSize - 1 );
                    slabs << SlabRect( frameRect, TileSet::Bottom ).adjusted( TileSet::DefaultSize );

                }

                // extra base, to extend below tabbar buttons
                if( tabBar )
                {

                    QRect frameRect( rect );
                    frameRect.setBottom( rect.top() + Metrics::TabBar_BaseOverlap - 1 );

                    if( rect.left() > tabBarRect.left() )
                    {
                        frameRect.setLeft( tabBarRect.left() - Metrics::TabBar_BaseOverlap );
                        frameRect.setRight( rect.left() + Metrics::TabBar_BaseOverlap - 1 );
                        if( documentMode || reverseLayout ) slabs << SlabRect( frameRect, TileSet::Bottom );
                        else slabs << SlabRect( frameRect, TileSet::BottomLeft );
                    }

                    if( rect.right() < tabBarRect.right() )
                    {

                        frameRect.setLeft( rect.right() - Metrics::TabBar_BaseOverlap + 1 );
                        frameRect.setRight( tabBarRect.right() + Metrics::TabBar_BaseOverlap );
                        if( documentMode || !reverseLayout ) slabs << SlabRect( frameRect, TileSet::Bottom );
                        else slabs << SlabRect( frameRect, TileSet::BottomRight );
                    }

                }

                break;

            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {

                tabSlab.tiles &= ~TileSet::Right;
                tabSlab.rect.adjust( 0, 0, 2, 0 );
                if( isDragged ) break;

                // top side
                if( isFrameAligned )
                {

                    QRect frameRect( rect );
                    frameRect.setLeft( tabRect.right() - 4 );
                    frameRect.setTop( frameRect.top() - 1 );
                    frameRect.setBottom( tabRect.top() );
                    slabs << SlabRect( frameRect, TileSet::Top ).adjusted( TileSet::DefaultSize );

                } else {

                    QRect frameRect( rect );
                    frameRect.setLeft( rect.right() - TileSet::DefaultSize + 1 );
                    frameRect.setBottom( tabRect.top() + 3 );
                    slabs << SlabRect( frameRect, TileSet::Left ).adjusted( TileSet::DefaultSize );
                }

                // bottom side
                QRect frameRect( rect );
                frameRect.setLeft( rect.right() - TileSet::DefaultSize + 1 );
                frameRect.setTop( tabRect.bottom() - 3 );
                slabs << SlabRect( frameRect, TileSet::Left ).adjusted( TileSet::DefaultSize );

                // extra base, to extend below tabbar buttons
                if( tabBar )
                {

                    QRect frameRect( rect );
                    frameRect.setLeft( rect.right() - Metrics::TabBar_BaseOverlap + 1 );
                    if( rect.top() > tabBarRect.top() )
                    {

                        frameRect.setTop( tabBarRect.top() - Metrics::TabBar_BaseOverlap );
                        frameRect.setBottom( rect.top() + Metrics::TabBar_BaseOverlap - 1 );
                        if( documentMode ) slabs << SlabRect( frameRect, TileSet::Left );
                        else slabs << SlabRect( frameRect, TileSet::TopLeft );
                    }

                    if( rect.bottom() < tabBarRect.bottom() )
                    {

                        frameRect.setTop( rect.bottom() - Metrics::TabBar_BaseOverlap + 1 );
                        frameRect.setBottom( tabBarRect.bottom() + Metrics::TabBar_BaseOverlap - 1 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    }
                }

                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {

                tabSlab.tiles &= ~TileSet::Left;
                tabSlab.rect.adjust( -2, 0, 0, 0 );
                if( isDragged ) break;

                // top side
                if( isFrameAligned )
                {

                    QRect frameRect( rect );
                    frameRect.setRight( tabRect.left() + 4 );
                    frameRect.setTop( frameRect.top() - 1 );
                    frameRect.setBottom( tabRect.top() );
                    slabs << SlabRect( frameRect, TileSet::Top ).adjusted( TileSet::DefaultSize );

                } else {

                    QRect frameRect( rect );
                    frameRect.setRight( rect.left() + TileSet::DefaultSize - 1 );
                    frameRect.setBottom( tabRect.top() + 3 );
                    slabs << SlabRect( frameRect, TileSet::Right ).adjusted( TileSet::DefaultSize );
                }

                // bottom side
                QRect frameRect( rect );
                frameRect.setRight( rect.left() + TileSet::DefaultSize - 1 );
                frameRect.setTop( tabRect.bottom() - 3 );
                slabs << SlabRect( frameRect, TileSet::Right ).adjusted( TileSet::DefaultSize );

                // extra base, to extend below tabbar buttons
                if( tabBar )
                {

                    QRect frameRect( rect );
                    frameRect.setRight( rect.left() + Metrics::TabBar_BaseOverlap - 1 );

                    if( rect.top() > tabBarRect.top() )
                    {

                        frameRect.setTop( tabBarRect.top() - Metrics::TabBar_BaseOverlap );
                        frameRect.setBottom( rect.top() + Metrics::TabBar_BaseOverlap - 1 );
                        if( documentMode ) slabs << SlabRect( frameRect, TileSet::Right );
                        else slabs << SlabRect( frameRect, TileSet::TopRight );
                    }

                    if( rect.bottom() < tabBarRect.bottom() )
                    {

                        frameRect.setTop( rect.bottom() - Metrics::TabBar_BaseOverlap + 1 );
                        frameRect.setBottom( tabBarRect.bottom() + Metrics::TabBar_BaseOverlap - 1 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    }
                }

                break;
            }

            default: break;
        }

        // store default color
        const QColor color( palette.color( QPalette::Window ) );

        // render connections to frame
        // extra care must be taken care of so that no slab
        // extends beyond tabWidget frame, if any
        const QRect tabWidgetRect( tabWidget ? tabWidget->rect().translated( -widget->geometry().topLeft() ) : QRect() );

        foreach( SlabRect slab, slabs ) // krazy:exclude=foreach
        {
            adjustSlabRect( slab, tabWidgetRect, documentMode, verticalTabs );
            renderSlab( painter, slab, color, NoFill );
        }

        //  adjust clip rect and render tab
        if( tabBar )
        {
            painter->save();
            painter->setClipRegion( tabBarClipRegion( tabBar ) );
        }

        // fill tab
        if( isDragged ) fillTabBackground( painter, tabRect, color, widget );

        // slab options
        renderSlab( painter, tabSlab, color, NoFill );
        fillTab( painter, tabRect, color, tabOption->shape );

        // restore clip region
        if( tabBar ) painter->restore();

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabShapeControl_unselected( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionTab* tabOption( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // tab position and flags
        const QStyleOptionTab::TabPosition& position = tabOption->position;
        bool isFirst( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::Beginning );
        bool isLast( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::End );
        bool isLeftOfSelected( tabOption->selectedPosition == QStyleOptionTab::NextIsSelected );
        bool isRightOfSelected( tabOption->selectedPosition == QStyleOptionTab::PreviousIsSelected );

        // document mode
        const auto tabOptV3 = qstyleoption_cast<const QStyleOptionTab*>( option );
        bool documentMode = tabOptV3 ? tabOptV3->documentMode : false;
        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : nullptr;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        // this is needed to complete the base frame when there are widgets in tabbar
        const QTabBar* tabBar( qobject_cast<const QTabBar*>( widget ) );
        const QRect tabBarRect( tabBar ? tabBar->rect():QRect() );

        // hover and animation flags
        /* all are disabled when tabBar is locked ( drag in progress ) */
        const bool tabBarLocked( _tabBarData->locks( tabBar ) );
        const bool mouseOver( enabled && !tabBarLocked && ( state & State_MouseOver ) );

        // animation state
        _animations->tabBarEngine().updateState( widget, rect.topLeft(), AnimationHover, mouseOver );
        const bool animated( enabled && !tabBarLocked && _animations->tabBarEngine().isAnimated( widget, rect.topLeft(), AnimationHover ) );

        // corner widgets
        const bool verticalTabs( isVerticalTab( tabOption ) );
        const bool hasLeftCornerWidget( ( tabOption->cornerWidgets & QStyleOptionTab::LeftCornerWidget ) && !verticalTabs );

        // true if widget is aligned to the frame
        /* need to check for 'isRightOfSelected' because for some reason the isFirst flag is set when active tab is being moved */
        const bool isFrameAligned( !documentMode && isFirst && !hasLeftCornerWidget && !isRightOfSelected );
        isFirst &= !isRightOfSelected;
        isLast &= !isLeftOfSelected;

        // swap flags based on reverse layout, so that they become layout independent
        if( reverseLayout && !verticalTabs )
        {
            qSwap( isFirst, isLast );
            qSwap( isLeftOfSelected, isRightOfSelected );
        }

        const qreal radius = 4;

        // part of the tab in which the text is drawn
        QRect tabRect( rect );
        QPainterPath path;

        // highlighted slab (if any)
        SlabRect highlightSlab;

        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {

                tabRect.adjust( 0, 3, 1, -4 );

                // adjust sides when slab is adjacent to selected slab
                if( isLeftOfSelected ) tabRect.adjust( 0, 0, 2, 0 );
                else if( isRightOfSelected ) tabRect.adjust( -2, 0, 0, 0 );

                if( isFirst )
                {

                    tabRect.adjust( 1, 0, 0, 0 );
                    if( isFrameAligned ) path.moveTo( tabRect.bottomLeft() + QPoint( 0, 2 ) );
                    else path.moveTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.topLeft() + QPointF( 0, radius ) );
                    path.quadTo( tabRect.topLeft(), tabRect.topLeft() + QPoint( radius, 0 ) );
                    path.lineTo( tabRect.topRight() );
                    path.lineTo( tabRect.bottomRight() );

                } else if( isLast ) {

                    tabRect.adjust( 0, 0, -2, 0 );
                    path.moveTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.topLeft() );
                    path.lineTo( tabRect.topRight() - QPointF( radius, 0 ) );
                    path.quadTo( tabRect.topRight(), tabRect.topRight() + QPointF( 0, radius ) );
                    if( isFrameAligned ) path.lineTo( tabRect.bottomRight() + QPointF( 0, 2 ) );
                    else path.lineTo( tabRect.bottomRight() );

                } else {

                    path.moveTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.topLeft() );
                    path.lineTo( tabRect.topRight() );
                    path.lineTo( tabRect.bottomRight() );

                }

                // highlight
                QRect highlightRect( tabRect.left(), tabRect.bottom() - Metrics::TabBar_BaseOverlap + Metrics::TabBar_TabOffset, tabRect.width(), 1 );
                if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( -2, 0, 0, 0 ), TileSet::TopLeft );
                else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( 0, 0, 2, 0 ), TileSet::TopRight );
                else highlightSlab = SlabRect( highlightRect, TileSet::Top );
                break;

            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {

                tabRect.adjust( 0, 4, 1, -3 );

                // adjust sides when slab is adjacent to selected slab
                if( isLeftOfSelected ) tabRect.adjust( 0, 0, 2, 0 );
                else if( isRightOfSelected ) tabRect.adjust( -2, 0, 0, 0 );

                if( isFirst )
                {

                    tabRect.adjust( 1, 0, 0, 0 );
                    if( isFrameAligned ) path.moveTo( tabRect.topLeft() - QPoint( 0, 2 ) );
                    else path.moveTo( tabRect.topLeft() );
                    path.lineTo( tabRect.bottomLeft() - QPointF( 0, radius ) );
                    path.quadTo( tabRect.bottomLeft(), tabRect.bottomLeft() + QPoint( radius, 0 ) );
                    path.lineTo( tabRect.bottomRight() );
                    path.lineTo( tabRect.topRight() );

                } else if( isLast ) {

                    tabRect.adjust( 0, 0, -2, 0 );
                    path.moveTo( tabRect.topLeft() );
                    path.lineTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.bottomRight() - QPointF( radius, 0 ) );
                    path.quadTo( tabRect.bottomRight(), tabRect.bottomRight() - QPointF( 0, radius ) );
                    if( isFrameAligned ) path.lineTo( tabRect.topRight() - QPointF( 0, 2 ) );
                    else path.lineTo( tabRect.topRight() );

                } else {

                    path.moveTo( tabRect.topLeft() );
                    path.lineTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.bottomRight() );
                    path.lineTo( tabRect.topRight() );

                }

                // highlight
                QRect highlightRect( tabRect.left(), tabRect.top() - Metrics::TabBar_BaseOverlap + Metrics::TabBar_TabOffset, tabRect.width(), TileSet::DefaultSize );
                if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( -2, 0, 0, 0 ), TileSet::BottomLeft );
                else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( 0, 0, 2, 0 ), TileSet::BottomRight );
                else highlightSlab = SlabRect( highlightRect, TileSet::Bottom );

                break;

            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {

                tabRect.adjust( 3, 0, -4, 1 );

                // adjust sides when slab is adjacent to selected slab
                if( isLeftOfSelected ) tabRect.adjust( 0, 0, 0, 2 );
                else if( isRightOfSelected ) tabRect.adjust( 0, -2, 0, 0 );

                if( isFirst )
                {

                    tabRect.adjust( 0, 1, 0, 0 );
                    if( isFrameAligned ) path.moveTo( tabRect.topRight() + QPoint( 2, 0 ) );
                    else path.moveTo( tabRect.topRight() );
                    path.lineTo( tabRect.topLeft() + QPointF( radius, 0 ) );
                    path.quadTo( tabRect.topLeft(), tabRect.topLeft() + QPoint( 0, radius ) );
                    path.lineTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.bottomRight() );

                } else if( isLast ) {

                    tabRect.adjust( 0, 0, 0, -1 );
                    path.moveTo( tabRect.topRight() );
                    path.lineTo( tabRect.topLeft() );
                    path.lineTo( tabRect.bottomLeft() - QPointF( 0, radius ) );
                    path.quadTo( tabRect.bottomLeft(), tabRect.bottomLeft() + QPointF( radius, 0 ) );
                    path.lineTo( tabRect.bottomRight() );

                } else {

                    path.moveTo( tabRect.topRight() );
                    path.lineTo( tabRect.topLeft() );
                    path.lineTo( tabRect.bottomLeft() );
                    path.lineTo( tabRect.bottomRight() );

                }

                // highlight
                QRect highlightRect( tabRect.right() - Metrics::TabBar_BaseOverlap + Metrics::TabBar_TabOffset, tabRect.top(), 1, tabRect.height() );
                if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( 0, -2, 0, 0 ), TileSet::TopLeft );
                else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( 0, 0, 0, 2 ), TileSet::BottomLeft );
                else highlightSlab = SlabRect( highlightRect, TileSet::Left );

                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {

                tabRect.adjust( 4, 0, -3, 1 );

                // adjust sides when slab is adjacent to selected slab
                if( isLeftOfSelected ) tabRect.adjust( 0, 0, 0, 2 );
                else if( isRightOfSelected ) tabRect.adjust( 0, -2, 0, 0 );

                if( isFirst )
                {

                    tabRect.adjust( 0, 1, 0, 0 );
                    if( isFrameAligned ) path.moveTo( tabRect.topLeft() - QPoint( 2, 0 ) );
                    else path.moveTo( tabRect.topLeft() );
                    path.lineTo( tabRect.topRight() - QPointF( radius, 0 ) );
                    path.quadTo( tabRect.topRight(), tabRect.topRight() + QPoint( 0, radius ) );
                    path.lineTo( tabRect.bottomRight() );
                    path.lineTo( tabRect.bottomLeft() );

                } else if( isLast ) {

                    tabRect.adjust( 0, 0, 0, -1 );
                    path.moveTo( tabRect.topLeft() );
                    path.lineTo( tabRect.topRight() );
                    path.lineTo( tabRect.bottomRight() - QPointF( 0, radius ) );
                    path.quadTo( tabRect.bottomRight(), tabRect.bottomRight() - QPointF( radius, 0 ) );
                    path.lineTo( tabRect.bottomLeft() );

                } else {

                    path.moveTo( tabRect.topLeft() );
                    path.lineTo( tabRect.topRight() );
                    path.lineTo( tabRect.bottomRight() );
                    path.lineTo( tabRect.bottomLeft() );

                }

                // highlight
                QRect highlightRect( tabRect.left() - Metrics::TabBar_BaseOverlap + Metrics::TabBar_TabOffset, tabRect.top(), TileSet::DefaultSize, tabRect.height() );
                if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( 0, -2, 0, 0 ), TileSet::TopRight );
                else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( 0, 0, 0, 2 ), TileSet::BottomRight );
                else highlightSlab = SlabRect( highlightRect, TileSet::Right );

                break;
            }

            default: break;
        }

        const QColor color( palette.color( QPalette::Window ) );

        //  adjust clip rect and render tab
        if( tabBar )
        {
            painter->save();
            painter->setClipRegion( tabBarClipRegion( tabBar ) );
        }

        // fill tab
        const QColor backgroundColor = _helper->backgroundColor( color, widget, rect.center() );
        const QColor midColor = _helper->alphaColor( _helper->calcDarkColor( backgroundColor ), 0.4 );
        const QColor darkColor = _helper->alphaColor( _helper->calcDarkColor( backgroundColor ), 0.6 );

        painter->save();
        painter->translate( 0.5, 0.5 );
        painter->setRenderHints( QPainter::Antialiasing );
        painter->setPen( darkColor );
        painter->setBrush( midColor );
        painter->drawPath( path );
        painter->restore();

        // restore clip region
        if( tabBar ) painter->restore();

        // handle base frame painting, for tabbars in which tab is being dragged
        _tabBarData->drawTabBarBaseControl( tabOption, painter, widget );

        // hovered highlight
        if( ( animated || mouseOver ) && highlightSlab.isValid() )
        {

            highlightSlab.adjust( TileSet::DefaultSize );
            const QRect tabWidgetRect( tabWidget ? tabWidget->rect().translated( -widget->geometry().topLeft() ) : QRect() );
            adjustSlabRect( highlightSlab, tabWidgetRect, documentMode, verticalTabs );

            const qreal opacity( _animations->tabBarEngine().opacity( widget, rect.topLeft(), AnimationHover ) );
            const StyleOptions hoverTabOpts( NoFill | Hover );

            // pass an invalid color to have only the glow painted
            if( animated ) renderSlab( painter, highlightSlab, QColor(), hoverTabOpts, opacity, AnimationHover );
            else renderSlab( painter, highlightSlab, QColor(), hoverTabOpts );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawToolBoxTabLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionToolBox* toolBoxOption( qstyleoption_cast<const QStyleOptionToolBox *>( option ) );
        if( !toolBoxOption ) return true;

        // copy palette
        const QPalette& palette( option->palette );

        const State& state( option->state );
        const bool enabled( state & State_Enabled );

        // text alignment
        const int textFlags( _mnemonics->textFlags() | Qt::AlignCenter );

        // contents rect
        const QRect rect( subElementRect( SE_ToolBoxTabContents, option, widget ) );

        // store icon size
        const int iconSize( pixelMetric( QStyle::PM_SmallIconSize, option, widget ) );

        // find contents size and rect
        QRect contentsRect( rect );
        QSize contentsSize;
        if( !toolBoxOption->text.isEmpty() )
        {
            contentsSize = option->fontMetrics.size( _mnemonics->textFlags(), toolBoxOption->text );
            if( !toolBoxOption->icon.isNull() ) contentsSize.rwidth() += Metrics::ToolBox_TabItemSpacing;
        }

        // icon size
        if( !toolBoxOption->icon.isNull() )
        {

            contentsSize.setHeight( qMax( contentsSize.height(), iconSize ) );
            contentsSize.rwidth() += iconSize;

        }

        // adjust contents rect
        contentsRect = centerRect( contentsRect, contentsSize );

        // render icon
        if( !toolBoxOption->icon.isNull() )
        {

            // icon rect
            QRect iconRect;
            if( toolBoxOption->text.isEmpty() ) iconRect = centerRect( contentsRect, iconSize, iconSize );
            else {

                iconRect = contentsRect;
                iconRect.setWidth( iconSize );
                iconRect = centerRect( iconRect, iconSize, iconSize );
                contentsRect.setLeft( iconRect.right() + Metrics::ToolBox_TabItemSpacing + 1 );

            }

            iconRect = visualRect( option, iconRect );
            const QIcon::Mode mode( enabled ? QIcon::Normal : QIcon::Disabled );
            const QPixmap pixmap( toolBoxOption->icon.pixmap( iconSize, mode ) );
            drawItemPixmap( painter, iconRect, textFlags, pixmap );

        }

        // render text
        if( !toolBoxOption->text.isEmpty() )
        {
            contentsRect = visualRect( option, contentsRect );
            drawItemText( painter, contentsRect, textFlags, palette, enabled, toolBoxOption->text, QPalette::WindowText );
        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawToolBoxTabShapeControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // copy state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool selected( state & State_Selected );
        const bool mouseOver( enabled && !selected && ( state & State_MouseOver ) );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // cast option and check
        const auto v2 = qstyleoption_cast<const QStyleOptionToolBox*>( option );
        if( v2 && v2->position == QStyleOptionToolBox::Beginning && selected ) return true;

        /*
        the proper widget ( the toolbox tab ) is not passed as argument by Qt.
        What is passed is the toolbox directly. To implement animations properly,
        the painter->device() is used instead
        */
        bool animated( false );
        qreal opacity( AnimationData::OpacityInvalid );
        if( enabled )
        {
            // try retrieve button from painter device.
            if( QPaintDevice* device = painter->device() )
            {
                _animations->toolBoxEngine().updateState( device, mouseOver );
                animated = _animations->toolBoxEngine().isAnimated( device );
                opacity = _animations->toolBoxEngine().opacity( device );
            }

        }

        // save colors for shadow
        /* important: option returns a wrong color. We use the widget's palette when widget is set */
        const QColor color( widget ? widget->palette().color( widget->backgroundRole() ) : palette.color( QPalette::Window ) );
        const QColor dark( _helper->calcDarkColor( color ) );
        QList<QColor> colors;
        colors.append( _helper->calcLightColor( color ) );

        if( mouseOver || animated )
        {

            QColor highlight = _helper->hoverColor( palette );
            if( animated )
            {

                colors.append( KColorUtils::mix( dark, highlight, opacity ) );
                colors.append( _helper->alphaColor( highlight, 0.2*opacity ) );

            } else {

                colors.append( highlight );
                colors.append( _helper->alphaColor( highlight, 0.2 ) );

            }

        } else colors.append( dark );

        // create path
        const qreal radius( 8 );
        const qreal offset( radius * std::tan( M_PI/8 ) );
        const QRectF rectf( rect );

        QPainterPath path;
        if( reverseLayout )
        {

            path.moveTo( rectf.right()-1, rectf.top() );
            path.lineTo( rectf.left() + 50 + radius + offset, rectf.top() );
            path.arcTo( QRectF( rectf.left() + 50 + radius + offset - 2*radius, rectf.top(), radius*2, radius*2 ), 90, 45 );
            path.lineTo( rectf.left() + 50 - rectf.height() + 3 + offset/std::sqrt(2), rectf.bottom() - 3 - offset/std::sqrt(2) );
            path.arcTo( QRectF( rectf.left() + 50 - rectf.height() + 3 + radius - offset - 2*radius, rectf.bottom() - 3 - 2*radius, 2*radius, 2*radius ), 315, -45 );
            path.lineTo( rectf.left(), rectf.bottom() - 3 );

        } else {

            path.moveTo( rectf.left(), rectf.top() );
            path.lineTo( rectf.right()- 50 - radius - offset, rectf.top() );
            path.arcTo( QRectF( rectf.right() - 50 - radius - offset, rectf.top(), radius*2, radius*2 ), 90, -45 );
            path.lineTo( rectf.right() - 50 + rectf.height() - 3 - offset/std::sqrt(2), rectf.bottom() - 3 - offset/std::sqrt(2) );
            path.arcTo( QRectF( rectf.right() - 50 + rectf.height() - 3 - radius + offset, rectf.bottom() - 3 - 2*radius, 2*radius, 2*radius ), 225, 45 );
            path.lineTo( rectf.right() - 1, rectf.bottom() - 3 );

        }


        // paint
        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->translate(0.5, 0.5 );
        painter->translate( 0, 2 );
        foreach( const QColor& color, colors )
        {
            painter->setPen( color );
            painter->drawPath( path );
            painter->translate( 0,-1 );
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawDockWidgetTitleControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionDockWidget* dockWidgetOption = qstyleoption_cast<const QStyleOptionDockWidget*>( option );
        if( !dockWidgetOption ) return true;

        // copy palette
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // cast to v2 to check vertical bar
        const auto v2 = qstyleoption_cast<const QStyleOptionDockWidget*>( option );
        const bool verticalTitleBar( v2 ? v2->verticalTitleBar : false );

        const QRect buttonRect( subElementRect( dockWidgetOption->floatable ? SE_DockWidgetFloatButton : SE_DockWidgetCloseButton, option, widget ) );

        // get rectangle and adjust to properly accounts for buttons
        QRect rect( insideMargin( dockWidgetOption->rect, Metrics::Frame_FrameWidth ) );
        if( verticalTitleBar )
        {

            if( buttonRect.isValid() ) rect.setTop( buttonRect.bottom()+1 );

        } else if( reverseLayout ) {

            if( buttonRect.isValid() ) rect.setLeft( buttonRect.right()+1 );
            rect.adjust( 0, 0, -4, 0 );

        } else {

            if( buttonRect.isValid() ) rect.setRight( buttonRect.left()-1 );
            rect.adjust( 4, 0, 0, 0 );

        }

        QString title( dockWidgetOption->title );
        int titleWidth = dockWidgetOption->fontMetrics.size( _mnemonics->textFlags(), title ).width();
        int width = verticalTitleBar ? rect.height() : rect.width();
        if( width < titleWidth ) title = dockWidgetOption->fontMetrics.elidedText( title, Qt::ElideRight, width, Qt::TextShowMnemonic );

        if( verticalTitleBar )
        {

            QSize s = rect.size();
            s.transpose();
            rect.setSize( s );

            painter->save();
            painter->translate( rect.left(), rect.top() + rect.width() );
            painter->rotate( -90 );
            painter->translate( - rect.left(), - rect.top() );
            drawItemText( painter, rect, Qt::AlignLeft | Qt::AlignVCenter | _mnemonics->textFlags(), palette, enabled, title, QPalette::WindowText );
            painter->restore();


        } else {

            drawItemText( painter, rect, Qt::AlignLeft | Qt::AlignVCenter | _mnemonics->textFlags(), palette, enabled, title, QPalette::WindowText );

        }

        return true;


    }

    //___________________________________________________________________________________
    bool Style::drawToolBarControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // when timeLine is running draw border event if not hovered
        const bool toolBarAnimated( _animations->toolBarEngine().isFollowMouseAnimated( widget ) );
        const QRect animatedRect( _animations->toolBarEngine().animatedRect( widget ) );
        const bool toolBarIntersected( toolBarAnimated && animatedRect.intersects( rect ) );
        if( toolBarIntersected )
        { _helper->slitFocused( _helper->focusColor( palette ) ).render( animatedRect, painter ); }

        // draw nothing otherwise ( toolbars are transparent )

        return true;

    }

    //______________________________________________________________
    bool Style::drawToolButtonComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        // check autoRaise state
        const State& state( option->state );
        const bool isInToolBar( widget && qobject_cast<QToolBar*>( widget->parent() ) );

        // get rect and palette
        const QRect& rect( option->rect );
        const QStyleOptionToolButton *toolButtonOption( qstyleoption_cast<const QStyleOptionToolButton *>( option ) );
        if( !toolButtonOption ) return true;

        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );
        const bool sunken( state & ( State_Sunken|State_On ) );
        const bool autoRaise( state & State_AutoRaise );

        if( isInToolBar )
        {

            _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );

        } else {

            // mouseOver has precedence over focus
            _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
            _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

        }

        // toolbar animation
        QWidget* parent( widget ? widget->parentWidget():0 );
        const bool toolBarAnimated( isInToolBar && _animations->toolBarEngine().isAnimated( parent ) );
        const QRect animatedRect( _animations->toolBarEngine().animatedRect( parent ) );
        const QRect currentRect( _animations->toolBarEngine().currentRect( parent ) );
        const bool current( isInToolBar && currentRect.intersects( rect.translated( widget->mapToParent( QPoint( 0,0 ) ) ) ) );
        const bool toolBarTimerActive( isInToolBar && _animations->toolBarEngine().isTimerActive( widget->parentWidget() ) );

        // normal toolbutton animation
        const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );

        // detect buttons in tabbar, for which special rendering is needed
        const bool inTabBar( widget && qobject_cast<const QTabBar*>( widget->parentWidget() ) );

        // local copy of option
        QStyleOptionToolButton copy( *toolButtonOption );

        const bool hasPopupMenu( toolButtonOption->features & QStyleOptionToolButton::MenuButtonPopup );
        const bool hasInlineIndicator(
            toolButtonOption->features&QStyleOptionToolButton::HasMenu
            && toolButtonOption->features&QStyleOptionToolButton::PopupDelay
            && !hasPopupMenu );

        const QRect buttonRect( subControlRect( CC_ToolButton, option, SC_ToolButton, widget ) );
        const QRect menuRect( subControlRect( CC_ToolButton, option, SC_ToolButtonMenu, widget ) );

        // frame
        const bool drawFrame(
            (enabled && !( mouseOver || hasFocus || sunken ) &&
            ((mode != AnimationNone) || ( ( ( toolBarAnimated && animatedRect.isNull() )||toolBarTimerActive ) && current )) ) ||
            (toolButtonOption->subControls & SC_ToolButton) );
        if( drawFrame )
        {
            copy.rect = buttonRect;
            if( inTabBar ) drawTabBarPanelButtonToolPrimitive( &copy, painter, widget );
            else drawPrimitive( PE_PanelButtonTool, &copy, painter, widget);
        }

        if( hasPopupMenu )
        {

            copy.rect = menuRect;
            if( !autoRaise )
            {
                drawPrimitive( PE_IndicatorButtonDropDown, &copy, painter, widget );
                copy.state &= ~(State_MouseOver|State_HasFocus);
            }

            drawPrimitive( PE_IndicatorArrowDown, &copy, painter, widget );

        } else if( hasInlineIndicator ) {

            copy.rect = menuRect;
            copy.state &= ~(State_MouseOver|State_HasFocus);
            drawPrimitive( PE_IndicatorArrowDown, &copy, painter, widget );

        }

        // contents
        {

            // restore state
            copy.state = state;

            // define contents rect
            QRect contentsRect( buttonRect );

            // detect dock widget title button
            /* for dockwidget title buttons, do not take out margins, so that icon do not get scaled down */
            const bool isDockWidgetTitleButton( widget && widget->inherits( "QDockWidgetTitleButton" ) );
            if( isDockWidgetTitleButton )
            {

                // cast to abstract button
                // adjust state to have correct icon rendered
                const QAbstractButton* button( qobject_cast<const QAbstractButton*>( widget ) );
                if( button->isChecked() || button->isDown() ) copy.state |= State_On;

            } else if( !inTabBar && hasInlineIndicator ) {

                const int marginWidth( autoRaise ? Metrics::ToolButton_MarginWidth : Metrics::Button_MarginWidth + Metrics::Frame_FrameWidth );
                contentsRect = insideMargin( contentsRect, marginWidth, 0 );
                contentsRect.setRight( contentsRect.right() - Metrics::ToolButton_InlineIndicatorWidth );
                contentsRect = visualRect( option, contentsRect );

            }

            copy.rect = contentsRect;

            // render
            drawControl( CE_ToolButtonLabel, &copy, painter, widget);

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawComboBoxComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {


        // cast option and check
        const QStyleOptionComboBox* comboBoxOption( qstyleoption_cast<const QStyleOptionComboBox*>( option ) );
        if( !comboBoxOption ) return true;

        // rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );
        const bool editable( comboBoxOption->editable );
        const bool sunken( state & (State_On|State_Sunken) );
        bool flat( !comboBoxOption->frame );

        // style options
        StyleOptions styleOptions;
        if( mouseOver ) styleOptions |= Hover;
        if( hasFocus ) styleOptions |= Focus;
        if( sunken && !editable ) styleOptions |= Sunken;

        // frame
        if( comboBoxOption->subControls & SC_ComboBoxFrame )
        {

            if( editable )
            {

                // editable combobox. Make it look like a LineEdit
                // focus takes precedence over hover
                _animations->inputWidgetEngine().updateState( widget, AnimationFocus, hasFocus );
                _animations->inputWidgetEngine().updateState( widget, AnimationHover, mouseOver && !hasFocus );

                // input area
                painter->setRenderHint( QPainter::Antialiasing );
                flat |= ( rect.height() <= 2*Metrics::Frame_FrameWidth + Metrics::MenuButton_IndicatorWidth );
                if( flat )
                {

                    const QColor background( palette.color( QPalette::Base ) );

                    painter->setPen( Qt::NoPen );
                    painter->setBrush( background );
                    painter->drawRect( rect );

                } else {

                    drawPrimitive( PE_FrameLineEdit, option, painter, widget );

                }

            } else {

                // non editable combobox. Make it look like a PushButton
                // hover takes precedence over focus
                _animations->inputWidgetEngine().updateState( widget, AnimationHover, mouseOver );
                _animations->inputWidgetEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

                const AnimationMode mode( _animations->inputWidgetEngine().buttonAnimationMode( widget ) );
                const qreal opacity( _animations->inputWidgetEngine().buttonOpacity( widget ) );

                // blend button color to the background
                const QColor buttonColor( _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );
                const QRect slabRect( rect );

                if( flat )
                {

                    if( !sunken )
                    {
                        // hover rect
                        const QColor glow( _helper->buttonGlowColor( palette, styleOptions, opacity, mode ) );
                        if( glow.isValid() ) _helper->slitFocused( glow ).render( rect, painter );

                    } else {

                        styleOptions |= HoleContrast;
                        _helper->renderHole( painter, palette.color( QPalette::Window ), rect, styleOptions, opacity, mode );

                    }

                } else {

                    renderButtonSlab( painter, slabRect, buttonColor, styleOptions, opacity, mode, TileSet::Ring );

                }

            }

        }

        if( comboBoxOption->subControls & SC_ComboBoxArrow )
        {

            const QComboBox* comboBox = qobject_cast<const QComboBox*>( widget );
            const bool empty( comboBox && !comboBox->count() );

            QColor color;
            QColor background;
            bool drawContrast( true );

            if( comboBoxOption->editable )
            {

                if( enabled && empty ) color = palette.color( QPalette::Disabled,  QPalette::Text );
                else {

                    // check animation state
                    const bool subControlHover( enabled && mouseOver && comboBoxOption->activeSubControls&SC_ComboBoxArrow );
                    _animations->comboBoxEngine().updateState( widget, AnimationHover, subControlHover  );

                    const bool animated( enabled && _animations->comboBoxEngine().isAnimated( widget, AnimationHover ) );
                    const qreal opacity( _animations->comboBoxEngine().opacity( widget, AnimationHover ) );

                    if( animated )
                    {

                        QColor highlight = _helper->hoverColor( palette );
                        color = KColorUtils::mix( palette.color( QPalette::Text ), highlight, opacity );

                    } else if( subControlHover ) {

                        color = _helper->hoverColor( palette );

                    } else {

                        color = palette.color( QPalette::Text );

                    }

                }

                background = palette.color( QPalette::Window );

                if( enabled ) drawContrast = false;

            } else {

                // foreground color
                const QPalette::ColorRole role( flat ? QPalette::WindowText : QPalette::ButtonText );
                if( enabled && empty ) color = palette.color( QPalette::Disabled,  role );
                else color  = palette.color( role );

                // background color
                background = palette.color( flat ? QPalette::Window : QPalette::Button );

            }

            // draw the arrow
            QRect arrowRect = subControlRect( CC_ComboBox, option, SC_ComboBoxArrow, widget );
            const QPolygonF arrow( genericArrow( ArrowDown, ArrowNormal ) );
            const qreal penThickness = 1.6;

            painter->save();
            painter->translate( QRectF( arrowRect ).center() );
            painter->setRenderHint( QPainter::Antialiasing );

            if( drawContrast )
            {

                const qreal offset( qMin( penThickness, qreal( 1 ) ) );
                painter->translate( 0,offset );
                painter->setPen( QPen( _helper->calcLightColor( palette.color( QPalette::Window ) ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                painter->drawPolyline( arrow );
                painter->translate( 0,-offset );

            }

            painter->setPen( QPen( _helper->decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( arrow );
            painter->restore();

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawSpinBoxComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSpinBox *spinBoxOption = qstyleoption_cast<const QStyleOptionSpinBox *>( option );
        if( !spinBoxOption ) return true;

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        if( spinBoxOption->subControls & SC_SpinBoxFrame )
        {

            // detect flat spinboxes
            bool flat( !spinBoxOption->frame );
            flat |= ( rect.height() <= 2*Metrics::Frame_FrameWidth + Metrics::MenuButton_IndicatorWidth );
            if( flat )
            {

                const QColor background( palette.color( QPalette::Base ) );

                painter->setRenderHint( QPainter::Antialiasing );
                painter->setPen( Qt::NoPen );
                painter->setBrush( background );
                painter->drawRect( rect );

            } else {

                drawPrimitive( PE_FrameLineEdit, option, painter, widget );

            }
        }

        if( spinBoxOption->subControls & SC_SpinBoxUp ) renderSpinBoxArrow( painter, spinBoxOption, widget, SC_SpinBoxUp );
        if( spinBoxOption->subControls & SC_SpinBoxDown ) renderSpinBoxArrow( painter, spinBoxOption, widget, SC_SpinBoxDown );

        return true;

    }

    //______________________________________________________________
    bool Style::drawSliderComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSlider *sliderOption( qstyleoption_cast<const QStyleOptionSlider *>( option ) );
        if( !sliderOption ) return true;


        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // copy state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( state & State_HasFocus );

        // direction
        const bool horizontal( sliderOption->orientation == Qt::Horizontal );

        if( sliderOption->subControls & SC_SliderTickmarks )
        {
            const bool upsideDown( sliderOption->upsideDown );
            const int tickPosition( sliderOption->tickPosition );
            const int available( pixelMetric( PM_SliderSpaceAvailable, option, widget ) );
            int interval = sliderOption->tickInterval;
            if( interval < 1 ) interval = sliderOption->pageStep;
            if( interval >= 1 )
            {
                const int fudge( pixelMetric( PM_SliderLength, option, widget ) / 2 );
                int current( sliderOption->minimum );

                // store tick lines
                const QRect grooveRect( subControlRect( CC_Slider, sliderOption, SC_SliderGroove, widget ) );
                QList<QLine> tickLines;
                if( horizontal )
                {

                    if( tickPosition & QSlider::TicksAbove ) tickLines.append( QLine( rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.top() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength ) );
                    if( tickPosition & QSlider::TicksBelow ) tickLines.append( QLine( rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth, rect.left(), grooveRect.bottom() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength ) );

                } else {

                    if( tickPosition & QSlider::TicksAbove ) tickLines.append( QLine( grooveRect.left() - Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.left() - Metrics::Slider_TickMarginWidth - Metrics::Slider_TickLength, rect.top() ) );
                    if( tickPosition & QSlider::TicksBelow ) tickLines.append( QLine( grooveRect.right() + Metrics::Slider_TickMarginWidth, rect.top(), grooveRect.right() + Metrics::Slider_TickMarginWidth + Metrics::Slider_TickLength, rect.top() ) );

                }

                // colors
                QColor base( _helper->backgroundColor( palette.color( QPalette::Window ), widget, rect.center() ) );
                base = _helper->calcDarkColor( base );
                painter->setPen( base );

                while( current <= sliderOption->maximum )
                {

                    // calculate positions and draw lines
                    int position( sliderPositionFromValue( sliderOption->minimum, sliderOption->maximum, current, available ) + fudge );
                    foreach( const QLine& tickLine, tickLines )
                    {
                        if( horizontal ) painter->drawLine( tickLine.translated( upsideDown ? (rect.width() - position) : position, 0 ) );
                        else painter->drawLine( tickLine.translated( 0, upsideDown ? (rect.height() - position):position ) );
                    }

                    // go to next position
                    current += interval;

                }
            }
        }

        // groove
        if( sliderOption->subControls & SC_SliderGroove )
        {
            // get rect
            QRect grooveRect( subControlRect( CC_Slider, sliderOption, SC_SliderGroove, widget ) );

            // render
            _helper->scrollHole( palette.color( QPalette::Window ), sliderOption->orientation, true ).render( grooveRect, painter, TileSet::Full );

        }

        // handle
        if( sliderOption->subControls & SC_SliderHandle )
        {

            // get rect and center
            QRect handleRect( subControlRect( CC_Slider, sliderOption, SC_SliderHandle, widget ) );

            // handle state
            const bool handleActive( sliderOption->activeSubControls & SC_SliderHandle );
            const bool sunken( state & (State_On|State_Sunken) );
            StyleOptions styleOptions;
            if( hasFocus ) styleOptions |= Focus;
            if( handleActive && mouseOver ) styleOptions |= Hover;

            // animation state
            _animations->widgetStateEngine().updateState( widget, AnimationHover, handleActive && mouseOver );
            _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus );
            const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
            const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );

            // define colors
            const QColor color( _helper->backgroundColor( palette.color( QPalette::Button ), widget, handleRect.center() ) );
            const QColor glow( _helper->buttonGlowColor( palette, styleOptions, opacity, mode ) );

            // render
            painter->drawPixmap( handleRect.topLeft(), _helper->sliderSlab( color, glow, sunken, 0 ) );

        }

        return true;
    }

    //______________________________________________________________
    bool Style::drawDialComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        const State& state( option->state );
        const bool enabled = state & State_Enabled;
        const bool mouseOver( enabled && ( state & State_MouseOver ) );
        const bool hasFocus( enabled && ( state & State_HasFocus ) );
        const bool sunken( state & ( State_On|State_Sunken ) );

        StyleOptions styleOptions;
        if( sunken ) styleOptions |= Sunken;
        if( hasFocus ) styleOptions |= Focus;
        if( mouseOver ) styleOptions |= Hover;

        // mouseOver has precedence over focus
        _animations->widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        _animations->widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        const AnimationMode mode( _animations->widgetStateEngine().buttonAnimationMode( widget ) );
        const qreal opacity( _animations->widgetStateEngine().buttonOpacity( widget ) );

        const QRect rect( option->rect );
        const QPalette &palette( option->palette );
        const QColor buttonColor( _helper->backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );
        renderDialSlab( painter, rect, buttonColor, option, styleOptions, opacity, mode );

        return true;

    }

    //______________________________________________________________
    bool Style::drawScrollBarComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        // render full groove directly, rather than using the addPage and subPage control element methods
        if( option->subControls & SC_ScrollBarGroove )
        {
            // retrieve groove rectangle
            QRect grooveRect( subControlRect( CC_ScrollBar, option, SC_ScrollBarGroove, widget ) );

            const QPalette& palette( option->palette );
            const QColor color( palette.color( QPalette::Window ) );
            const State& state( option->state );
            const bool horizontal( state & State_Horizontal );

            if( horizontal ) grooveRect = centerRect( grooveRect, grooveRect.width(), StyleConfigData::scrollBarWidth() );
            else grooveRect = centerRect( grooveRect, StyleConfigData::scrollBarWidth(), grooveRect.height() );

            // render
            renderScrollBarHole( painter, grooveRect, color, Qt::Horizontal );


        }

        // call base class primitive
        KStyle::drawComplexControl( CC_ScrollBar, option, painter, widget );
        return true;
    }

    //______________________________________________________________
    bool Style::drawTitleBarComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionTitleBar *titleBarOption( qstyleoption_cast<const QStyleOptionTitleBar *>( option ) );
        if( !titleBarOption ) return true;

        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool active( enabled && ( titleBarOption->titleBarState & Qt::WindowActive ) );

        // draw title text
        {
            QRect textRect = subControlRect( CC_TitleBar, titleBarOption, SC_TitleBarLabel, widget );

            // enable state transition
            _animations->widgetEnableStateEngine().updateState( widget, AnimationEnable, active );

            // make sure palette has the correct color group
            QPalette palette( option->palette );

            if( _animations->widgetEnableStateEngine().isAnimated( widget, AnimationEnable ) )
            { palette = _helper->disabledPalette( palette, _animations->widgetEnableStateEngine().opacity( widget, AnimationEnable )  ); }

            palette.setCurrentColorGroup( active ? QPalette::Active: QPalette::Disabled );
            KStyle::drawItemText( painter, textRect, Qt::AlignCenter, palette, active, titleBarOption->text, QPalette::WindowText );

        }


        // menu button
        if( ( titleBarOption->subControls & SC_TitleBarSysMenu ) && ( titleBarOption->titleBarFlags & Qt::WindowSystemMenuHint ) && !titleBarOption->icon.isNull() )
        {

            QRect iconRect = subControlRect( CC_TitleBar, titleBarOption, SC_TitleBarSysMenu, widget );
            const int iconWidth( pixelMetric( PM_SmallIconSize, option, widget ) );
            const QSize iconSize( iconWidth, iconWidth );
            iconRect = centerRect( iconRect, iconSize );
            const QPixmap pixmap = titleBarOption->icon.pixmap( iconSize, QIcon::Normal, QIcon::On );
            painter->drawPixmap( iconRect, pixmap );

        }

        if( ( titleBarOption->subControls & SC_TitleBarMinButton ) && ( titleBarOption->titleBarFlags & Qt::WindowMinimizeButtonHint ) )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarMinButton ); }

        if( ( titleBarOption->subControls & SC_TitleBarMaxButton ) && ( titleBarOption->titleBarFlags & Qt::WindowMaximizeButtonHint ) )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarMaxButton ); }

        if( ( titleBarOption->subControls & SC_TitleBarCloseButton ) )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarCloseButton ); }

        if( ( titleBarOption->subControls & SC_TitleBarNormalButton ) &&
            ( ( ( titleBarOption->titleBarFlags & Qt::WindowMinimizeButtonHint ) &&
            ( titleBarOption->titleBarState & Qt::WindowMinimized ) ) ||
            ( ( titleBarOption->titleBarFlags & Qt::WindowMaximizeButtonHint ) &&
            ( titleBarOption->titleBarState & Qt::WindowMaximized ) ) ) )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarNormalButton ); }

        if( titleBarOption->subControls & SC_TitleBarShadeButton )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarShadeButton ); }

        if( titleBarOption->subControls & SC_TitleBarUnshadeButton )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarUnshadeButton ); }

        if( ( titleBarOption->subControls & SC_TitleBarContextHelpButton ) && ( titleBarOption->titleBarFlags & Qt::WindowContextHelpButtonHint ) )
        { renderTitleBarButton( painter, titleBarOption, widget, SC_TitleBarContextHelpButton ); }

        return true;
    }

    //_________________________________________________________________________________
    void Style::renderDialSlab( QPainter *painter, const QRect& constRect, const QColor &color, const QStyleOption *option, StyleOptions styleOptions, qreal opacity, AnimationMode mode ) const
    {

        // cast option and check
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return;

        // copy palette
        const QPalette& palette( option->palette );

        // adjust rect to be square, and centered
        const int dimension( qMin( constRect.width(), constRect.height() ) );
        const QRect rect( centerRect( constRect, dimension, dimension ) );

        // calculate glow color
        const QColor glow( _helper->buttonGlowColor( palette, styleOptions, opacity, mode ) );

        // get main slab
        QPixmap pixmap( _helper->dialSlab( color, glow, 0, dimension ) );
        {
            const QColor light( _helper->calcLightColor( color ) );
            const QColor shadow( _helper->calcShadowColor( color ) );

            QPainter painter( &pixmap );
            painter.setPen( Qt::NoPen );
            painter.setRenderHints( QPainter::Antialiasing );

            // indicator
            const qreal angle( dialAngle( sliderOption, sliderOption->sliderPosition ) );
            QPointF center( pixmap.rect().center()/_helper->devicePixelRatio( pixmap ) );
            const int sliderWidth( dimension/6 );
            const qreal radius( 0.5*( dimension - 2*sliderWidth ) );
            center += QPointF( radius*cos( angle ), -radius*sin( angle ) );

            QRectF sliderRect( 0, 0, sliderWidth, sliderWidth );
            sliderRect.moveCenter( center );

            // outline circle
            const qreal offset( 0.3 );
            painter.setBrush( light );
            painter.setPen( Qt::NoPen );
            painter.drawEllipse( sliderRect.translated( 0, offset ) );

            // mask
            painter.setPen( Qt::NoPen );
            painter.save();
            painter.setCompositionMode( QPainter::CompositionMode_DestinationOut );
            painter.setBrush( QBrush( Qt::black ) );
            painter.drawEllipse( sliderRect );
            painter.restore();

            // shadow
            painter.translate( sliderRect.topLeft() );
            _helper->drawInverseShadow( painter, shadow.darker( 200 ), 0, sliderRect.width(), 0 );

            // glow
            if( glow.isValid() ) _helper->drawInverseGlow( painter, glow, 0, sliderRect.width(),  sliderRect.width() );

            painter.end();
        }

        painter->drawPixmap( rect.topLeft(), pixmap );

        return;

    }

    //____________________________________________________________________________________
    void Style::renderButtonSlab( QPainter *painter, QRect rect, const QColor &color, StyleOptions options, qreal opacity,
        AnimationMode mode,
        TileSet::Tiles tiles ) const
    {

        // check rect
        if( !rect.isValid() ) return;

        // edges
        // for slabs, hover takes precedence over focus ( other way around for holes )
        // but in any case if the button is sunken we don't show focus nor hover
        TileSet tileSet;
        if( options & Sunken )
        {
            tileSet = _helper->slabSunken( color );

        } else {

            QColor glow = _helper->buttonGlowColor( QPalette::Active, options, opacity, mode );
            tileSet = _helper->slab( color, glow, 0 );

        }

        // adjust rect to account for missing tiles
        if( tileSet.isValid() ) rect = tileSet.adjust( rect, tiles );

        // fill
        if( !( options & NoFill ) ) _helper->fillButtonSlab( *painter, rect, color, options&Sunken );

        // render slab
        if( tileSet.isValid() ) tileSet.render( rect, painter, tiles );

    }

    //____________________________________________________________________________________
    void Style::renderSlab(
        QPainter *painter, QRect rect,
        const QColor &color,
        StyleOptions options, qreal opacity,
        AnimationMode mode,
        TileSet::Tiles tiles ) const
    {

        // check rect
        if( !rect.isValid() ) return;

        // fill
        if( !( options & NoFill ) )
        {
            painter->save();
            painter->setRenderHint( QPainter::Antialiasing );
            painter->setPen( Qt::NoPen );

            if( _helper->calcShadowColor( color ).value() > color.value() && ( options & Sunken ) )
            {

                QLinearGradient innerGradient( 0, rect.top(), 0, rect.bottom() + rect.height() );
                innerGradient.setColorAt( 0, color );
                innerGradient.setColorAt( 1, _helper->calcLightColor( color ) );
                painter->setBrush( innerGradient );

            } else {

                QLinearGradient innerGradient( 0, rect.top() - rect.height(), 0, rect.bottom() );
                innerGradient.setColorAt( 0, _helper->calcLightColor( color ) );
                innerGradient.setColorAt( 1, color );
                painter->setBrush( innerGradient );

            }

            _helper->fillSlab( *painter, rect );

            painter->restore();
        }

        // edges
        // for slabs, hover takes precedence over focus ( other way around for holes )
        // but in any case if the button is sunken we don't show focus nor hover
        TileSet tileSet;
        if( ( options & Sunken ) && color.isValid() )
        {
            tileSet = _helper->slabSunken( color );

        } else {

            // calculate proper glow color based on current settings and opacity
            const QColor glow( _helper->buttonGlowColor( QPalette::Active, options, opacity, mode ) );
            if( color.isValid() || glow.isValid() ) tileSet = _helper->slab( color, glow , 0 );
            else return;

        }

        // render tileset
        if( tileSet.isValid() ) tileSet.render( rect, painter, tiles );

    }

    //______________________________________________________________________________________________________________________________
    void Style::fillTabBackground( QPainter* painter, const QRect& rect, const QColor &color, const QWidget* widget ) const
    {

        // filling
        const QRect fillRect( insideMargin( rect, Metrics::TabBar_TabOffset ) );
        if( widget ) _helper->renderWindowBackground( painter, fillRect, widget, color );
        else painter->fillRect( fillRect, color );

    }

    //______________________________________________________________________________________________________________________________
    void Style::fillTab( QPainter* painter, const QRect &rect, const QColor &color, QTabBar::Shape shape ) const
    {

        const QColor dark( _helper->calcDarkColor( color ) );
        const QColor shadow( _helper->calcShadowColor( color ) );
        const QColor light( _helper->calcLightColor( color ) );
        const QRect fillRect( insideMargin( rect, Metrics::TabBar_TabOffset ) );

        QLinearGradient gradient;
        switch( shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            gradient = QLinearGradient( fillRect.topLeft(), fillRect.bottomLeft() );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            gradient = QLinearGradient( fillRect.bottomLeft(), fillRect.topLeft() );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            gradient = QLinearGradient( fillRect.topRight(), fillRect.topLeft() );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            gradient = QLinearGradient( fillRect.topLeft(), fillRect.topRight() );
            break;

            default: return;

        }

        gradient.setColorAt( 0, _helper->alphaColor( light, 0.5 ) );
        gradient.setColorAt( 0.1, _helper->alphaColor( light, 0.5 ) );
        gradient.setColorAt( 0.25, _helper->alphaColor( light, 0.3 ) );
        gradient.setColorAt( 0.5, _helper->alphaColor( light, 0.2 ) );
        gradient.setColorAt( 0.75, _helper->alphaColor( light, 0.1 ) );
        gradient.setColorAt( 0.9, Qt::transparent );

        painter->setRenderHints( QPainter::Antialiasing );
        painter->setPen( Qt::NoPen );

        painter->setBrush( gradient );
        painter->drawRoundedRect( fillRect, 2, 2 );

    }

    //____________________________________________________________________________________________________
    void Style::renderSpinBoxArrow( QPainter* painter, const QStyleOptionSpinBox* option, const QWidget* widget, const SubControl& subControl ) const
    {

        const QPalette& palette( option->palette );
        const State& state( option->state );

        // enable state
        bool enabled( state & State_Enabled );

        // check steps enable step
        const bool atLimit(
            (subControl == SC_SpinBoxUp && !(option->stepEnabled & QAbstractSpinBox::StepUpEnabled )) ||
            (subControl == SC_SpinBoxDown && !(option->stepEnabled & QAbstractSpinBox::StepDownEnabled ) ) );

        // update enabled state accordingly
        enabled &= !atLimit;

        // update mouse-over effect
        const bool mouseOver( enabled && ( state & State_MouseOver ) );

        // check animation state
        const bool subControlHover( enabled && mouseOver && ( option->activeSubControls & subControl ) );
        _animations->spinBoxEngine().updateState( widget, subControl, subControlHover );

        const bool animated( enabled && _animations->spinBoxEngine().isAnimated( widget, subControl ) );
        const qreal opacity( _animations->spinBoxEngine().opacity( widget, subControl ) );

        QColor color;
        if( animated )
        {

            QColor highlight = _helper->hoverColor( palette );
            color = KColorUtils::mix( palette.color( QPalette::Text ), highlight, opacity );

        } else if( subControlHover ) {

            color = _helper->hoverColor( palette );

        } else if( atLimit ) {

            color = palette.color( QPalette::Disabled, QPalette::Text );

        } else {

            color = palette.color( QPalette::Text );

        }

        const qreal penThickness = 1.6;
        const QColor background = palette.color( QPalette::Window );

        const QPolygonF arrow( genericArrow( ( subControl == SC_SpinBoxUp ) ? ArrowUp:ArrowDown, ArrowNormal ) );
        const QRect arrowRect( subControlRect( CC_SpinBox, option, subControl, widget ) );

        painter->save();
        painter->translate( QRectF( arrowRect ).center() );
        painter->setRenderHint( QPainter::Antialiasing );

        painter->setPen( QPen( _helper->decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );
        painter->restore();

        return;

    }

    //___________________________________________________________________________________
    void Style::renderSplitter( const QStyleOption* option, QPainter* painter, const QWidget* widget, bool horizontal ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool mouseOver( enabled && ( state & ( State_MouseOver|State_Sunken ) ) );

        // get orientation
        const Qt::Orientation orientation( horizontal ? Qt::Horizontal : Qt::Vertical );

        bool animated( false );
        qreal opacity( AnimationData::OpacityInvalid );

        if( enabled )
        {
            if( qobject_cast<const QMainWindow*>( widget ) )
            {

                _animations->dockSeparatorEngine().updateRect( widget, rect, orientation, mouseOver );
                animated = _animations->dockSeparatorEngine().isAnimated( widget, rect, orientation );
                opacity = animated ? _animations->dockSeparatorEngine().opacity( widget, orientation ) : AnimationData::OpacityInvalid;

            } else if( QPaintDevice* device = painter->device() ) {

                /*
                try update QSplitterHandle using painter device, because Qt passes
                QSplitter as the widget to the QStyle primitive.
                */
                _animations->splitterEngine().updateState( device, mouseOver );
                animated = _animations->splitterEngine().isAnimated( device );
                opacity = _animations->splitterEngine().opacity( device );

            }
        }

        // get base color
        const QColor color = palette.color( QPalette::Window );

        if( horizontal )
        {
            const int hCenter = rect.center().x();
            const int h = rect.height();

            if( animated || mouseOver )
            {
                const QColor highlight = _helper->alphaColor( _helper->calcLightColor( color ),0.5*( animated ? opacity:1 ) );
                const qreal fraction( rect.height() > 30 ? 10/rect.height():0.1 );
                QLinearGradient gradient( rect.topLeft(), rect.bottomLeft() );
                gradient.setColorAt( 0, Qt::transparent );
                gradient.setColorAt( fraction, highlight );
                gradient.setColorAt( 1-fraction, highlight );
                gradient.setColorAt( 1, Qt::transparent );
                painter->fillRect( rect, gradient );
            }

            const int ngroups( qMax( 1,h / 250 ) );
            int center( ( h - ( ngroups-1 ) * 250 ) /2 + rect.top() );
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                _helper->renderDot( painter, QPoint( hCenter, center-3 ), color );
                _helper->renderDot( painter, QPoint( hCenter, center ), color );
                _helper->renderDot( painter, QPoint( hCenter, center+3 ), color );
            }

        } else {

            const int vCenter( rect.center().y() );
            const int w( rect.width() );
            if( animated || mouseOver )
            {
                const QColor highlight( _helper->alphaColor( _helper->calcLightColor( color ),0.5*( animated ? opacity:1 ) ) );
                const qreal fraction( rect.width() > 30 ? 10 / rect.width():0.1 );
                QLinearGradient gradient( rect.topLeft(), rect.topRight() );
                gradient.setColorAt( 0, Qt::transparent );
                gradient.setColorAt( fraction, highlight );
                gradient.setColorAt( 1 - fraction, highlight );
                gradient.setColorAt( 1, Qt::transparent );
                painter->fillRect( rect, gradient );

            }

            const int ngroups( qMax( 1, w / 250 ) );
            int center = ( w - ( ngroups-1 ) * 250 ) /2 + rect.left();
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                _helper->renderDot( painter, QPoint( center-3, vCenter ), color );
                _helper->renderDot( painter, QPoint( center, vCenter ), color );
                _helper->renderDot( painter, QPoint( center+3, vCenter ), color );
            }

        }

    }

    //____________________________________________________________________________________________________
    void Style::renderTitleBarButton( QPainter* painter, const QStyleOptionTitleBar* option, const QWidget* widget, const SubControl& subControl ) const
    {

        // get relevant rect
        const QRect rect = subControlRect( CC_TitleBar, option, subControl, widget );
        if( !rect.isValid() ) return;

        // copy palette
        QPalette palette = option->palette;

        // store state
        const State& state( option->state );
        const bool enabled( state & State_Enabled );
        const bool active( enabled && ( option->titleBarState & Qt::WindowActive ) );

        // enable state transition
        _animations->widgetEnableStateEngine().updateState( widget, AnimationEnable, active );
        if( _animations->widgetEnableStateEngine().isAnimated( widget, AnimationEnable ) )
        { palette = _helper->disabledPalette( palette, _animations->widgetEnableStateEngine().opacity( widget, AnimationEnable )  ); }

        const bool sunken( state & State_Sunken );
        const bool mouseOver( ( !sunken ) && widget && rect.translated( widget->mapToGlobal( QPoint( 0,0 ) ) ).contains( QCursor::pos() ) );

        _animations->mdiWindowEngine().updateState( widget, subControl, enabled && mouseOver );
        const bool animated( enabled && _animations->mdiWindowEngine().isAnimated( widget, subControl ) );
        const qreal opacity( _animations->mdiWindowEngine().opacity( widget, subControl ) );

        // contrast color
        const QColor base =option->palette.color( QPalette::Active, QPalette::Window );

        // icon color
        QColor color;
        if( animated )
        {

            const QColor base( palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText ) );
            const QColor glow( subControl == SC_TitleBarCloseButton ?
                _helper->negativeTextColor( palette ):
                _helper->hoverColor( palette ) );

            color = KColorUtils::mix( base, glow, opacity );

        } else if( mouseOver ) {

            color = ( subControl == SC_TitleBarCloseButton ) ?
                _helper->negativeTextColor( palette ):
                _helper->hoverColor( palette );

        } else {

            color = palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText );

        }

        // rendering
        renderTitleBarButton( painter, rect, base, color, subControl );

    }

    //____________________________________________________________________________________________________
    void Style::renderTitleBarButton( QPainter* painter, const QRect& rect, const QColor& base, const QColor& color, const SubControl& subControl ) const
    {

        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );
        painter->setBrush( Qt::NoBrush );

        painter->drawPixmap( rect, _helper->dockWidgetButton( base, true, rect.width() ) );

        const qreal width( 1.1 );

        // contrast
        painter->translate( 0, 0.5 );
        painter->setPen( QPen( _helper->calcLightColor( base ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        renderTitleBarIcon( painter, rect, subControl );

        // main icon painting
        painter->translate( 0,-1 );
        painter->setPen( QPen( color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        renderTitleBarIcon( painter, rect, subControl );

        painter->restore();

    }

    //____________________________________________________________________________________
    void Style::renderTitleBarIcon( QPainter *painter, const QRect& rect, const SubControl& subControl ) const
    {

        painter->save();

        painter->translate( rect.topLeft() );
        painter->scale( qreal( rect.width() )/16, qreal( rect.height() )/16 );

        switch( subControl )
        {
            case SC_TitleBarContextHelpButton:
            {
                painter->drawArc( 6, 4, 3, 3, 135*16, -180*16 );
                painter->drawArc( 8, 7, 3, 3, 135*16, 45*16 );
                painter->drawPoint( 8, 11 );
                break;
            }

            case SC_TitleBarMinButton:
            {
                painter->drawPolyline( QPolygon() <<  QPoint( 5, 7 ) << QPoint( 8, 10 ) << QPoint( 11, 7 ) );
                break;
            }

            case SC_TitleBarNormalButton:
            {
                painter->drawPolygon( QPolygon() << QPoint( 8, 5 ) << QPoint( 11, 8 ) << QPoint( 8, 11 ) << QPoint( 5, 8 ) );
                break;
            }

            case SC_TitleBarMaxButton:
            {
                painter->drawPolyline( QPolygon() << QPoint( 5, 9 ) << QPoint( 8, 6 ) << QPoint( 11, 9 ) );
                break;
            }

            case SC_TitleBarCloseButton:
            {

                painter->drawLine( QPointF( 5.5, 5.5 ), QPointF( 10.5, 10.5 ) );
                painter->drawLine( QPointF( 10.5, 5.5 ), QPointF( 5.5, 10.5 ) );
                break;

            }

            case SC_TitleBarShadeButton:
            {
                painter->drawLine( QPoint( 5, 11 ), QPoint( 11, 11 ) );
                painter->drawPolyline( QPolygon() << QPoint( 5, 5 ) << QPoint( 8, 8 ) << QPoint( 11, 5 ) );
                break;
            }

            case SC_TitleBarUnshadeButton:
            {
                painter->drawPolyline( QPolygon() << QPoint( 5, 8 ) << QPoint( 8, 5 ) << QPoint( 11, 8 ) );
                painter->drawLine( QPoint( 5, 11 ), QPoint( 11, 11 ) );
                break;
            }

            default:
            break;
        }
        painter->restore();
    }

    //__________________________________________________________________________
    void Style::renderHeaderBackground( const QRect& rect, const QPalette& palette, QPainter* painter, const QWidget* widget, bool horizontal, bool reverse ) const
    {

        // use window background for the background
        if( widget ) _helper->renderWindowBackground( painter, rect, widget, palette );
        else painter->fillRect( rect, palette.color( QPalette::Window ) );

        if( horizontal ) renderHeaderLines( rect, palette, painter, TileSet::Bottom );
        else if( reverse ) renderHeaderLines( rect, palette, painter, TileSet::Left );
        else renderHeaderLines( rect, palette, painter, TileSet::Right );

    }

    //__________________________________________________________________________
    void Style::renderHeaderLines( const QRect& constRect, const QPalette& palette, QPainter* painter, TileSet::Tiles tiles ) const
    {

        // add horizontal lines
        const QColor color( palette.color( QPalette::Window ) );
        const QColor dark( _helper->calcDarkColor( color ) );
        const QColor light( _helper->calcLightColor( color ) );

        painter->save();
        QRect rect( constRect );
        if( tiles & TileSet::Bottom  )
        {

            painter->setPen( dark );
            if( tiles & TileSet::Left ) painter->drawPoint( rect.bottomLeft() );
            else if( tiles& TileSet::Right ) painter->drawPoint( rect.bottomRight() );
            else painter->drawLine( rect.bottomLeft(), rect.bottomRight() );

            rect.adjust( 0, 0, 0, -1 );
            painter->setPen( light );
            if( tiles & TileSet::Left )
            {
                painter->drawLine( rect.bottomLeft(), rect.bottomLeft()+QPoint( 1, 0 ) );
                painter->drawLine( rect.bottomLeft()+ QPoint( 1, 0 ), rect.bottomLeft()+QPoint( 1, 1 ) );

            } else if( tiles & TileSet::Right ) {

                painter->drawLine( rect.bottomRight(), rect.bottomRight() - QPoint( 1, 0 ) );
                painter->drawLine( rect.bottomRight() - QPoint( 1, 0 ), rect.bottomRight() - QPoint( 1, -1 ) );

            } else {

                painter->drawLine( rect.bottomLeft(), rect.bottomRight() );
            }
        } else if( tiles & TileSet::Left ) {

            painter->setPen( dark );
            painter->drawLine( rect.topLeft(), rect.bottomLeft() );

            rect.adjust( 1, 0, 0, 0 );
            painter->setPen( light );
            painter->drawLine( rect.topLeft(), rect.bottomLeft() );

        } else if( tiles & TileSet::Right ) {

            painter->setPen( dark );
            painter->drawLine( rect.topRight(), rect.bottomRight() );

            rect.adjust( 0, 0, -1, 0 );
            painter->setPen( light );
            painter->drawLine( rect.topRight(), rect.bottomRight() );

        }

        painter->restore();

        return;

    }

    //__________________________________________________________________________
    void Style::renderMenuItemBackground( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );
        const QRect animatedRect( _animations->menuEngine().animatedRect( widget ) );
        if( !animatedRect.isNull() )
        {

            if( animatedRect.intersects( rect ) )
            {
                const QColor color( _helper->menuBackgroundColor( _helper->calcMidColor( palette.color( QPalette::Window ) ), widget, animatedRect.center() ) );
                renderMenuItemRect( option, animatedRect, color, palette, painter );
            }

        } else if( _animations->menuEngine().isTimerActive( widget ) ) {

            const QRect previousRect( _animations->menuEngine().currentRect( widget, Previous ) );
            if( previousRect.intersects( rect ) )
            {

                const QColor color( _helper->menuBackgroundColor( _helper->calcMidColor( palette.color( QPalette::Window ) ), widget, previousRect.center() ) );
                renderMenuItemRect( option, previousRect, color, palette, painter );
            }

        } else if( _animations->menuEngine().isAnimated( widget, Previous ) ) {

            QRect previousRect( _animations->menuEngine().currentRect( widget, Previous ) );
            if( previousRect.intersects( rect ) )
            {
                const qreal opacity(  _animations->menuEngine().opacity( widget, Previous ) );
                const QColor color( _helper->menuBackgroundColor( _helper->calcMidColor( palette.color( QPalette::Window ) ), widget, previousRect.center() ) );
                renderMenuItemRect( option, previousRect, color, palette, painter, opacity );
            }

        }

        return;
    }

    //__________________________________________________________________________
    void Style::renderMenuItemRect( const QStyleOption* option, const QRect& rect, const QColor& base, const QPalette& palette, QPainter* painter, qreal opacity ) const
    {

        if( opacity == 0 ) return;

        // get relevant color
        // TODO: this is inconsistent with MenuBar color.
        // this should change to properly account for 'sunken' state
        QColor color( base );
        if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG )
        {

            color = palette.color( QPalette::Highlight );

        } else if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_SUBTLE ) {

            color = KColorUtils::mix( color, KColorUtils::tint( color, palette.color( QPalette::Highlight ), 0.6 ) );

        }

        // special painting for items with submenus
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if( menuItemOption && menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu )
        {

            QPixmap pixmap( rect.size() );
            {
                pixmap.fill( Qt::transparent );
                QPainter painter( &pixmap );
                const QRect pixmapRect( pixmap.rect() );

                painter.setRenderHint( QPainter::Antialiasing );
                painter.setPen( Qt::NoPen );

                painter.setBrush( color );
                _helper->fillHole( painter, pixmapRect );

                _helper->holeFlat( color, 0 ).render( pixmapRect.adjusted( 1, 2, -2, -1 ), &painter );

                QRect maskRect( visualRect( option->direction, pixmapRect, QRect( pixmapRect.width()-40, 0, 40, pixmapRect.height() ) ) );
                QLinearGradient gradient(
                    visualPos( option->direction, maskRect, QPoint( maskRect.left(), 0 ) ),
                    visualPos( option->direction, maskRect, QPoint( maskRect.right()-4, 0 ) ) );
                gradient.setColorAt( 0, Qt::black );
                gradient.setColorAt( 1, Qt::transparent );
                painter.setBrush( gradient );
                painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
                painter.drawRect( maskRect );

                if( opacity >= 0 && opacity < 1 )
                {
                    painter.setCompositionMode( QPainter::CompositionMode_DestinationIn );
                    painter.fillRect( pixmapRect, _helper->alphaColor( Qt::black, opacity ) );
                }

                painter.end();

            }

            painter->drawPixmap( visualRect( option, rect ), pixmap );

        } else {

            if( opacity >= 0 && opacity < 1 )
            { color.setAlphaF( opacity ); }

            _helper->holeFlat( color, 0 ).render( rect.adjusted( 1, 2, -2, -1 ), painter, TileSet::Full );

        }

    }

    //________________________________________________________________________
    void Style::renderCheckBox(
        QPainter *painter, const QRect &constRect, const QPalette &palette,
        StyleOptions options, CheckBoxState state,
        qreal opacity,
        AnimationMode mode ) const
    {

        const int size( qMin( constRect.width(), constRect.height() ) );
        const QRect rect( centerRect( constRect, size, size ) );

        if( !( options & NoFill ) )
        {
            if( options & Sunken ) _helper->holeFlat( palette.color( QPalette::Window ), 0, false ).render( insideMargin( rect, 1 ), painter, TileSet::Full );
            else renderSlab( painter, rect, palette.color( QPalette::Button ), options, opacity, mode, TileSet::Ring );
        }

        if( state == CheckOff ) return;

        // check mark
        qreal penThickness( 2 );
        const QColor color( palette.color( ( options&Sunken ) ? QPalette::WindowText:QPalette::ButtonText ) );
        const QColor background( palette.color( ( options&Sunken ) ? QPalette::Window:QPalette::Button ) );
        QPen pen( _helper->decoColor( background, color ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        QPen contrastPen( _helper->calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
        if( state == CheckTriState )
        {

            QVector<qreal> dashes;
            dashes << 1 << 2;
            penThickness = 1.3;
            pen.setWidthF( penThickness );
            contrastPen.setWidthF( penThickness );
            pen.setDashPattern( dashes );
            contrastPen.setDashPattern( dashes );

        } else if( state == CheckSunken ) {

            pen.setColor( _helper->alphaColor( pen.color(), 0.3 ) );
            contrastPen.setColor( _helper->alphaColor( contrastPen.color(), 0.3 ) );

        }

        painter->save();
        painter->translate( QRectF( rect ).center() );

        if( !( options&Sunken ) ) painter->translate( 0, -1 );
        painter->setRenderHint( QPainter::Antialiasing );

        QPolygonF checkMark;
        checkMark << QPointF( 5, -2 ) << QPointF( -1, 5 ) << QPointF( -4, 2 );

        const qreal offset( qMin( penThickness, qreal( 1 ) ) );
        painter->setPen( contrastPen );
        painter->translate( 0, offset );
        painter->drawPolyline( checkMark );

        painter->setPen( pen );
        painter->translate( 0, -offset );
        painter->drawPolyline( checkMark );

        painter->restore();

        return;

    }

    //___________________________________________________________________
    void Style::renderRadioButton(
        QPainter* painter, const QRect& constRect,
        const QPalette& palette,
        StyleOptions options,
        CheckBoxState state,
        qreal opacity,
        AnimationMode mode ) const
    {

        // get pixmap
        const QColor color( palette.color( QPalette::Button ) );
        const QColor glow( _helper->buttonGlowColor( palette, options, opacity, mode ) );
        QPixmap pixmap( _helper->roundSlab( color, glow, 0 ) );

        // center rect
        const QRect rect( centerRect( constRect, pixmap.size()/_helper->devicePixelRatio( pixmap ) ) );

        // render
        painter->drawPixmap( rect.topLeft(), pixmap );

        // draw the radio mark
        if( state != CheckOff )
        {
            const qreal radius( 2.6 );
            const qreal dx( 0.5*rect.width() - radius );
            const qreal dy( 0.5*rect.height() - radius );
            const QRectF symbolRect( QRectF( rect ).adjusted( dx, dy, -dx, -dy ) );

            painter->save();
            painter->setRenderHints( QPainter::Antialiasing );
            painter->setPen( Qt::NoPen );

            const QColor background( palette.color( QPalette::Button ) );
            const QColor color( palette.color( QPalette::ButtonText ) );

            // contrast
            if( state == CheckOn ) painter->setBrush( _helper->calcLightColor( background ) );
            else painter->setBrush( _helper->alphaColor( _helper->calcLightColor( background ), 0.3 ) );
            painter->translate( 0, radius/2 );
            painter->drawEllipse( symbolRect );

            // symbol
            if( state == CheckOn ) painter->setBrush( _helper->decoColor( background, color ) );
            else painter->setBrush( _helper->alphaColor( _helper->decoColor( background, color ), 0.3 ) );
            painter->translate( 0, -radius/2 );
            painter->drawEllipse( symbolRect );
            painter->restore();

        }

        return;
    }

    //______________________________________________________________________________
    void Style::renderScrollBarHole(
        QPainter *painter, const QRect &rect, const QColor &color,
        const Qt::Orientation& orientation, const TileSet::Tiles& tiles ) const
    {

        if( !rect.isValid() ) return;

        // one need to make smaller shadow
        // notably on the size when rect height is too high
        const bool smallShadow( orientation == Qt::Horizontal ? rect.height() < 10 : rect.width() < 10 );
        _helper->scrollHole( color, orientation, smallShadow ).render( rect, painter, tiles );

    }

    //______________________________________________________________________________
    void Style::renderScrollBarHandle(
        QPainter* painter, const QRect& constRect, const QPalette& palette,
        const Qt::Orientation& orientation, const bool& hover, const qreal& opacity ) const
    {

        if( !constRect.isValid() ) return;

        // define rect and check
        QRect rect( insideMargin( constRect, 3 ) );

        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );
        const QColor color( palette.color( QPalette::Button ) );

        // draw the slider
        const qreal radius = 3.5;

        // glow / shadow
        QColor glow;
        const QColor shadow( _helper->alphaColor( _helper->calcShadowColor( color ), 0.4 ) );
        const QColor hovered( _helper->hoverColor( palette ) );

        if( opacity >= 0 ) glow = KColorUtils::mix( shadow, hovered, opacity );
        else if( hover ) glow = hovered;
        else glow = shadow;

        _helper->scrollHandle( color, glow ).render( constRect, painter, TileSet::Full );

        // contents
        const QColor mid( _helper->calcMidColor( color ) );
        QLinearGradient gradient( rect.topLeft(), rect.bottomLeft() );
        gradient.setColorAt(0, color );
        gradient.setColorAt(1, mid );
        painter->setPen( Qt::NoPen );
        painter->setBrush( gradient );
        painter->drawRoundedRect( insideMargin( rect, 1 ), radius - 2, radius - 2 );

        // bevel pattern
        const QColor light( _helper->calcLightColor( color ) );

        const bool horizontal( orientation == Qt::Horizontal );
        QLinearGradient patternGradient( 0, 0, horizontal ? 30:0, horizontal? 0:30 );
        patternGradient.setSpread( QGradient::ReflectSpread );
        patternGradient.setColorAt( 0, Qt::transparent );
        patternGradient.setColorAt( 1, _helper->alphaColor( light, 0.1 ) );

        QRect bevelRect( rect );
        if( horizontal ) bevelRect = insideMargin( bevelRect, 0, 3 );
        else bevelRect = insideMargin( bevelRect, 3, 0 );

        if( bevelRect.isValid() )
        {
            painter->setBrush( patternGradient );
            painter->drawRect( bevelRect );
        }

        painter->restore();
        return;

    }

    //______________________________________________________________________________
    void Style::renderScrollBarArrow(
        QPainter* painter, const QRect& rect, const QColor& color, const QColor& background,
        ArrowOrientation orientation ) const
    {

        const qreal penThickness = 1.6;
        QPolygonF arrow( genericArrow( orientation, ArrowNormal ) );

        const QColor contrast( _helper->calcLightColor( background ) );
        const QColor base( _helper->decoColor( background, color ) );

        painter->save();
        painter->translate( QRectF(rect).center() );
        painter->setRenderHint( QPainter::Antialiasing );

        const qreal offset( qMin( penThickness, qreal( 1 ) ) );
        painter->translate( 0,offset );
        painter->setPen( QPen( contrast, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );
        painter->translate( 0,-offset );

        painter->setPen( QPen( base, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( arrow );
        painter->restore();

        return;

    }

    //______________________________________________________________________________
    QColor Style::scrollBarArrowColor( const QStyleOptionSlider* option, const SubControl& control, const QWidget* widget ) const
    {

        // copy rect and palette
        const QRect& rect( option->rect );
        const QPalette& palette( option->palette );

        // color
        QColor color( palette.color( QPalette::WindowText ) );

        // check enabled state
        const bool enabled( option->state & State_Enabled );
        if( !enabled ) return color;

        if(
            ( control == SC_ScrollBarSubLine && option->sliderValue == option->minimum ) ||
            ( control == SC_ScrollBarAddLine && option->sliderValue == option->maximum ) )
        {

            // manually disable arrow, to indicate that scrollbar is at limit
            return palette.color( QPalette::Disabled, QPalette::WindowText );

        }

        const bool mouseOver( _animations->scrollBarEngine().isHovered( widget, control ) );
        const bool animated( _animations->scrollBarEngine().isAnimated( widget, control ) );
        const qreal opacity( _animations->scrollBarEngine().opacity( widget, control ) );

        // retrieve mouse position from engine
        QPoint position( mouseOver ? _animations->scrollBarEngine().position( widget ) : QPoint( -1, -1 ) );
        if( mouseOver && rect.contains( position ) )
        {
            // need to update the arrow controlRect on fly because there is no
            // way to get it from the styles directly, outside of repaint events
            _animations->scrollBarEngine().setSubControlRect( widget, control, rect );
        }


        if( rect.intersects(  _animations->scrollBarEngine().subControlRect( widget, control ) ) )
        {

            QColor highlight = _helper->hoverColor( palette );
            if( animated )
            {
                color = KColorUtils::mix( color, highlight, opacity );

            } else if( mouseOver ) {

                color = highlight;

            }

        }

        return color;

    }

    //______________________________________________________________________________
    qreal Style::dialAngle( const QStyleOptionSlider* sliderOption, int value ) const
    {

        // calculate angle at which handle needs to be drawn
        qreal angle( 0 );
        if( sliderOption->maximum == sliderOption->minimum ) angle = M_PI / 2;
        else {

            qreal fraction( qreal( value - sliderOption->minimum )/qreal( sliderOption->maximum - sliderOption->minimum ) );
            if( !sliderOption->upsideDown ) fraction = 1 - fraction;

            if( sliderOption->dialWrapping ) angle = 1.5*M_PI - fraction*2*M_PI;
            else  angle = ( M_PI*8 - fraction*10*M_PI )/6;

        }

        return angle;

    }

    //______________________________________________________________
    void Style::polishScrollArea( QAbstractScrollArea* scrollArea ) const
    {

        if( !scrollArea ) return;

        // HACK: add exception for KPIM transactionItemView, which is an overlay widget
        // and must have filled background. This is a temporary workaround until a more
        // robust solution is found.
        if( scrollArea->inherits( "KPIM::TransactionItemView" ) )
        {
            // also need to make the scrollarea background plain ( using autofill background )
            // so that optional vertical scrollbar background is not transparent either.
            // TODO: possibly add an event filter to use the "normal" window background
            // instead of something flat.
            scrollArea->setAutoFillBackground( true );
            return;
        }

        // check frame style and background role
        if( !(scrollArea->frameShape() == QFrame::NoFrame || scrollArea->backgroundRole() == QPalette::Window ) )
        { return; }

        // get viewport and check background role
        QWidget* viewport( scrollArea->viewport() );
        if( !( viewport && viewport->backgroundRole() == QPalette::Window ) ) return;

        // change viewport autoFill background.
        // do the same for children if the background role is QPalette::Window
        viewport->setAutoFillBackground( false );
        QList<QWidget*> children( viewport->findChildren<QWidget*>() );
        foreach( QWidget* child, children )
        {
            if( child->parent() == viewport && child->backgroundRole() == QPalette::Window )
            { child->setAutoFillBackground( false ); }
        }

    }

    //_______________________________________________________________
    QRegion Style::tabBarClipRegion( const QTabBar* tabBar ) const
    {
        // need to mask-out arrow buttons, if visible.
        QRegion mask( tabBar->rect() );
        foreach( const QObject* child, tabBar->children() )
        {
            const QToolButton* toolButton( qobject_cast<const QToolButton*>( child ) );
            if( toolButton && toolButton->isVisible() ) mask -= toolButton->geometry();
        }

        return mask;

    }

    //____________________________________________________________________________________
    QPolygonF Style::genericArrow( ArrowOrientation orientation, ArrowSize size ) const
    {

        QPolygonF arrow;
        switch( orientation )
        {
            case ArrowUp:
            {
                if( size == ArrowTiny ) arrow << QPointF( -2.25, 1.125 ) << QPointF( 0, -1.125 ) << QPointF( 2.25, 1.125 );
                else if( size == ArrowSmall ) arrow << QPointF( -2.5, 1.5 ) << QPointF( 0, -1.5 ) << QPointF( 2.5, 1.5 );
                else arrow << QPointF( -3.5, 2 ) << QPointF( 0, -2 ) << QPointF( 3.5, 2 );
                break;
            }

            case ArrowDown:
            {
                if( size == ArrowTiny ) arrow << QPointF( -2.25, -1.125 ) << QPointF( 0, 1.125 ) << QPointF( 2.25, -1.125 );
                else if( size == ArrowSmall ) arrow << QPointF( -2.5, -1.5 ) << QPointF( 0, 1.5 ) << QPointF( 2.5, -1.5 );
                else arrow << QPointF( -3.5, -2 ) << QPointF( 0, 2 ) << QPointF( 3.5, -2 );
                break;
            }

            case ArrowLeft:
            {
                if( size == ArrowTiny ) arrow << QPointF( 1.125, -2.25 ) << QPointF( -1.125, 0 ) << QPointF( 1.125, 2.25 );
                else if( size == ArrowSmall ) arrow << QPointF( 1.5, -2.5 ) << QPointF( -1.5, 0 ) << QPointF( 1.5, 2.5 );
                else arrow << QPointF( 2, -3.5 ) << QPointF( -2, 0 ) << QPointF( 2, 3.5 );

                break;
            }

            case ArrowRight:
            {
                if( size == ArrowTiny ) arrow << QPointF( -1.125, -2.25 ) << QPointF( 1.125, 0 ) << QPointF( -1.125, 2.25 );
                else if( size == ArrowSmall ) arrow << QPointF( -1.5, -2.5 ) << QPointF( 1.5, 0 ) << QPointF( -1.5, 2.5 );
                else arrow << QPointF( -2, -3.5 ) << QPointF( 2, 0 ) << QPointF( -2, 3.5 );
                break;
            }

            default: break;

        }

        return arrow;

    }

    //____________________________________________________________________________________
    QStyleOptionToolButton Style::separatorMenuItemOption( const QStyleOptionMenuItem* menuItemOption, const QWidget* widget ) const
    {

        // separator can have a title and an icon
        // in that case they are rendered as sunken flat toolbuttons
        QStyleOptionToolButton toolButtonOption;
        toolButtonOption.initFrom( widget );
        toolButtonOption.rect = menuItemOption->rect;
        toolButtonOption.features = QStyleOptionToolButton::None;
        toolButtonOption.state = State_On|State_Sunken|State_Enabled;
        toolButtonOption.subControls = SC_ToolButton;
        toolButtonOption.icon =  menuItemOption->icon;

        const int iconWidth( pixelMetric( PM_SmallIconSize, menuItemOption, widget ) );
        toolButtonOption.iconSize = QSize( iconWidth, iconWidth );
        toolButtonOption.text = menuItemOption->text;

        toolButtonOption.toolButtonStyle = Qt::ToolButtonTextBesideIcon;

        return toolButtonOption;

    }

    //____________________________________________________________________
    bool Style::isQtQuickControl( const QStyleOption* option, const QWidget* widget ) const
    {
        const bool is = (widget == nullptr) && option && option->styleObject && option->styleObject->inherits( "QQuickItem" );
        if ( is ) _windowManager->registerQuickItem( static_cast<QQuickItem*>( option->styleObject ) );
        return is;
    }

    //_____________________________________________________________
    Style::SlabRect::SlabRect(void):
        tiles( TileSet::Ring )
    {}

    //_____________________________________________________________
    Style::SlabRect::SlabRect( const QRect& rect, int tiles ):
        rect( rect ),
        tiles( TileSet::Tiles(tiles) )
    {}

    //_____________________________________________________________
    bool Style::SlabRect::isValid( void ) const
    { return rect.isValid() && tiles; }

    //_____________________________________________________________
    void Style::SlabRect::adjust( int tileSize )
    {
        if( !( tiles & TileSet::Left ) ) rect.adjust( -tileSize, 0, 0, 0 );
        if( !( tiles & TileSet::Right ) ) rect.adjust( 0, 0, tileSize, 0 );
        if( !( tiles & TileSet::Top ) ) rect.adjust( 0, -tileSize, 0, 0 );
        if( !( tiles & TileSet::Bottom ) ) rect.adjust( 0, 0, 0, tileSize );
    }

    //_____________________________________________________________
    Style::SlabRect Style::SlabRect::adjusted( int tileSize ) const
    {
        SlabRect copy( *this );
        copy.adjust( tileSize );
        return copy;
    }

}

namespace OxygenPrivate
{

    //_________________________________________________________________________________________________________
    void TabBarData::drawTabBarBaseControl( const QStyleOptionTab* tabOption, QPainter* painter, const QWidget* widget )
    {


        // check parent
        if( !_style ) return;

        // make sure widget is locked
        if( !locks( widget ) ) return;

        // make sure dirty flag is set
        if( !_dirty ) return;

        // cast to TabBar and check
        const QTabBar* tabBar( qobject_cast<const QTabBar*>( widget ) );
        if( !tabBar ) return;

        // get reverseLayout flag
        const bool reverseLayout( tabOption->direction == Qt::RightToLeft );

        // get documentMode flag
        const auto tabOptionV3 = qstyleoption_cast<const QStyleOptionTab*>( tabOption );

        bool documentMode = tabOptionV3 ? tabOptionV3->documentMode : false;
        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : nullptr;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        const QRect tabBarRect( tabBar->rect() );

        // define slab
        Oxygen::Style::SlabRect slab;

        // switch on tab shape
        switch( tabOption->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                Oxygen::TileSet::Tiles tiles( Oxygen::TileSet::Top );
                QRect frameRect;
                frameRect.setLeft( tabBarRect.left() - Oxygen::TileSet::DefaultSize );
                frameRect.setRight( tabBarRect.right() + Oxygen::TileSet::DefaultSize );
                frameRect.setTop( tabBarRect.bottom() - Oxygen::TileSet::DefaultSize + 1 );
                frameRect.setHeight( 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= Oxygen::TileSet::Left;
                if( !documentMode && reverseLayout ) tiles |= Oxygen::TileSet::Right;
                slab = Oxygen::Style::SlabRect( frameRect, tiles );
                break;
            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                Oxygen::TileSet::Tiles tiles( Oxygen::TileSet::Bottom );
                QRect frameRect;
                frameRect.setLeft( tabBarRect.left() - Oxygen::TileSet::DefaultSize );
                frameRect.setRight( tabBarRect.right() + Oxygen::TileSet::DefaultSize );
                frameRect.setBottom( tabBarRect.top() + Oxygen::TileSet::DefaultSize - 1 );
                frameRect.setTop( frameRect.bottom() - 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= Oxygen::TileSet::Left;
                if( !documentMode && reverseLayout ) tiles |= Oxygen::TileSet::Right;
                slab = Oxygen::Style::SlabRect( frameRect, tiles );
                break;
            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                Oxygen::TileSet::Tiles tiles( Oxygen::TileSet::Left );
                QRect frameRect;
                frameRect.setTop( tabBarRect.top() - Oxygen::TileSet::DefaultSize );
                frameRect.setBottom( tabBarRect.bottom() + Oxygen::TileSet::DefaultSize );
                frameRect.setLeft( tabBarRect.right() - Oxygen::TileSet::DefaultSize + 1 );
                frameRect.setWidth( 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= Oxygen::TileSet::Top;
                if( !documentMode && reverseLayout ) tiles |= Oxygen::TileSet::Bottom;
                slab = Oxygen::Style::SlabRect( frameRect, tiles );
                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                Oxygen::TileSet::Tiles tiles( Oxygen::TileSet::Right );
                QRect frameRect;
                frameRect.setTop( tabBarRect.top() - Oxygen::TileSet::DefaultSize );
                frameRect.setBottom( tabBarRect.bottom() + Oxygen::TileSet::DefaultSize );
                frameRect.setRight( tabBarRect.left() + Oxygen::TileSet::DefaultSize - 1 );
                frameRect.setLeft( frameRect.right() - 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= Oxygen::TileSet::Top;
                if( !documentMode && reverseLayout ) tiles |= Oxygen::TileSet::Bottom;
                slab = Oxygen::Style::SlabRect( frameRect, tiles );
                break;
            }

            default:
            break;
        }

        const bool verticalTabs( _style.data()->isVerticalTab( tabOption ) );
        const QRect tabWidgetRect( tabWidget ? tabWidget->rect().translated( -widget->geometry().topLeft() ) : QRect() );

        const QPalette& palette( tabOption->palette );
        const QColor color( palette.color( QPalette::Window ) );
        _style.data()->adjustSlabRect( slab, tabWidgetRect, documentMode, verticalTabs );
        _style.data()->renderSlab( painter, slab, color, Oxygen::NoFill );

        setDirty( false );
        return;

    }

}
