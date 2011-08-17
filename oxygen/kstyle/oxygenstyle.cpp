// krazy:excludeall=qclasses

//////////////////////////////////////////////////////////////////////////////
// oxygenstyle.cpp
// Oxygen widget style for KDE 4
// -------------------
//
// Copyright ( C ) 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
// Copyright ( C ) 2008 Long Huynh Huu <long.upcase@googlemail.com>
// Copyright ( C ) 2007-2008 Casper Boemann <cbr@boemann.dk>
// Copyright ( C ) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
// Copyright ( C ) 2003-2005 Sandro Giessl <sandro@giessl.com>
//
// based on the KDE style "dotNET":
// Copyright ( C ) 2001-2002, Chris Lee <clee@kde.org>
// Carsten Pfeiffer <pfeiffer@kde.org>
// Karol Szwed <gallium@kde.org>
// Drawing routines completely reimplemented from KDE3 HighColor, which was
// originally based on some stuff from the KDE2 HighColor.
//
// based on drawing routines of the style "Keramik":
// Copyright ( c ) 2002 Malte Starostik <malte@kde.org>
// ( c ) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
// based on the KDE3 HighColor Style
// Copyright ( C ) 2001-2002 Karol Szwed <gallium@kde.org>
// ( C ) 2001-2002 Fredrik Höglund <fredrik@kde.org>
// Drawing routines adapted from the KDE2 HCStyle,
// Copyright ( C ) 2000 Daniel M. Duley <mosfet@kde.org>
// ( C ) 2000 Dirk Mueller <mueller@kde.org>
// ( C ) 2001 Martijn Klingens <klingens@kde.org>
// Progressbar code based on KStyle,
// Copyright ( C ) 2001-2002 Karol Szwed <gallium@kde.org>
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

#include "oxygenstyle.h"
#include "oxygenstyle.moc"

#include "oxygenanimations.h"
#include "oxygenframeshadow.h"
#include "oxygenmdiwindowshadow.h"
#include "oxygenshadowhelper.h"
#include "oxygensplitterproxy.h"
#include "oxygenstyleconfigdata.h"
#include "oxygentransitions.h"
#include "oxygenwidgetexplorer.h"
#include "oxygenwindowmanager.h"

#include <QtCore/QDebug>
#include <QtGui/QAbstractButton>
#include <QtGui/QAbstractItemView>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDial>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QGroupBox>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitterHandle>
#include <QtGui/QStylePlugin>
#include <QtGui/QStyleOption>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>
#include <QtGui/QToolButton>

#include <QtDBus/QDBusConnection>

#include <KColorUtils>
#include <KGlobal>
#include <KGlobalSettings>
#include <KIconLoader>
#include <KIcon>
#include <kdeversion.h>

#include <cmath>

/* These are to link libkio even if 'smart' linker is used */
#include <kio/authinfo.h>
extern "C" KIO::AuthInfo* _oxygen_init_kio() { return new KIO::AuthInfo(); }


//_____________________________________________
// style plugin
namespace Oxygen
{
    //! style plugin
    class StylePlugin : public QStylePlugin
    {
        public:

        //! returns list of valid keys
        QStringList keys() const
        { return QStringList( QLatin1String( "Oxygen" ) ); }

        //! create style
        QStyle *create( const QString &key )
        {

            if( key.toLower() == QLatin1String( "oxygen" ) ) return new Style;
            else return NULL;
        }
    };

}

Q_EXPORT_PLUGIN2( oxygen-qt, Oxygen::StylePlugin )

namespace Oxygen
{

    // hardcoded index offsets for custom widgets
    // copied from e.g. kstyle.cxx
    static const QStyle::StyleHint SH_KCustomStyleElement = ( QStyle::StyleHint )0xff000001;
    static const int X_KdeBase = 0xff000000;

    //_____________________________________________________________________
    bool TopLevelManager::eventFilter( QObject *object, QEvent *event )
    {

        // cast to QWidget
        QWidget *widget = static_cast<QWidget*>( object );
        if( event->type() == QEvent::Show && _helper.hasDecoration( widget ) )
        {
            _helper.setHasBackgroundGradient( widget->winId(), true );
            _helper.setHasBackgroundPixmap( widget->winId(), _helper.hasBackgroundPixmap() );
        }

        return false;
    }

    //______________________________________________________________
    Style::Style( void ):
        _kGlobalSettingsInitialized( false ),
        _addLineButtons( DoubleButton ),
        _subLineButtons( SingleButton ),
        _singleButtonHeight( 14 ),
        _doubleButtonHeight( 28 ),
        _showMnemonics( true ),
        _helper( new StyleHelper( "oxygen" ) ),
        _shadowHelper( new ShadowHelper( this, *_helper ) ),
        _animations( new Animations( this ) ),
        _transitions( new Transitions( this ) ),
        _windowManager( new WindowManager( this ) ),
        _topLevelManager( new TopLevelManager( this, *_helper ) ),
        _frameShadowFactory( new FrameShadowFactory( this ) ),
        _mdiWindowShadowFactory( new MdiWindowShadowFactory( this, *_helper ) ),
        _widgetExplorer( new WidgetExplorer( this ) ),
        _tabBarData( new TabBarData( this ) ),
        _splitterFactory( new SplitterFactory( this ) ),
        _frameFocusPrimitive( 0 ),
        _tabBarTabShapeControl( 0 ),
        _hintCounter( X_KdeBase+1 ),
        _controlCounter( X_KdeBase ),
        _subElementCounter( X_KdeBase ),
        CE_CapacityBar( newControlElement( "CE_CapacityBar" ) )

    {

        // use DBus connection to update on oxygen configuration change
        QDBusConnection dbus = QDBusConnection::sessionBus();
        dbus.connect( QString(), "/OxygenStyle", "org.kde.Oxygen.Style", "reparseConfiguration", this, SLOT(oxygenConfigurationChanged()) );

        // call the slot directly; this initial call will set up things that also
        // need to be reset when the system palette changes
        oxygenConfigurationChanged();

    }

    //______________________________________________________________
    Style::~Style( void )
    { delete _helper; }

    //______________________________________________________________
    void Style::polish( QWidget* widget )
    {
        if( !widget ) return;

        // register widget to animations
        animations().registerWidget( widget );
        transitions().registerWidget( widget );
        windowManager().registerWidget( widget );
        frameShadowFactory().registerWidget( widget, helper() );
        mdiWindowShadowFactory().registerWidget( widget );
        shadowHelper().registerWidget( widget );
        splitterFactory().registerWidget( widget );

        // scroll areas
        if( QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>( widget ) )
        {

            polishScrollArea( scrollArea );

        } else if( widget->inherits( "Q3ListView" ) ) {

            addEventFilter( widget );
            widget->setAttribute( Qt::WA_Hover );

        }

        // several widgets set autofill background to false, which effectively breaks the background
        // gradient rendering. Instead of patching all concerned applications,
        // we change the background here
        if( widget->inherits( "MessageList::Core::Widget" ) )
        { widget->setAutoFillBackground( false ); }

        // KTextEdit frames
        // static cast is safe here, since isKTextEdit already checks that widget inherits from QFrame
        if( isKTextEditFrame( widget ) && static_cast<QFrame*>( widget )->frameStyle() == ( QFrame::StyledPanel | QFrame::Sunken ) )
        {

            widget->setAttribute( Qt::WA_Hover );
            animations().lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        }

        // adjust layout for K3B themed headers
        // FIXME: to be removed when fixed upstream
        if( widget->inherits( "K3b::ThemedHeader" ) && widget->layout() )
        {
            widget->layout()->setMargin( 0 );
            frameShadowFactory().setHasContrast( widget, true );
        }

        // adjust flags for windows and dialogs
        switch( widget->windowFlags() & Qt::WindowType_Mask )
        {

            case Qt::Window:
            case Qt::Dialog:

            // set background as styled
            widget->setAttribute( Qt::WA_StyledBackground );
            widget->installEventFilter( _topLevelManager );

            // initialize connections to kGlobalSettings
            /*
            this musts be done in ::polish and not before,
            in order to be able to detect Qt-KDE vs Qt-only applications
            */
            if( !_kGlobalSettingsInitialized ) initializeKGlobalSettings();

            break;

            default: break;

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

        // also enable hover effects in itemviews' viewport
        if( QAbstractItemView *itemView = qobject_cast<QAbstractItemView*>( widget ) )
        { itemView->viewport()->setAttribute( Qt::WA_Hover ); }

        // checkable group boxes
        if( QGroupBox* groupBox = qobject_cast<QGroupBox*>( widget ) )
        {

            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover ); }

        } else if( qobject_cast<QAbstractButton*>( widget ) && qobject_cast<QDockWidget*>( widget->parent() ) ) {

            widget->setAttribute( Qt::WA_Hover );

        } else if( qobject_cast<QAbstractButton*>( widget ) && qobject_cast<QToolBox*>( widget->parent() ) ) {

            widget->setAttribute( Qt::WA_Hover );

        }

        /*
        add extra margins for widgets in toolbars
        this allows to preserve alignment with respect to actions
        */
        if( qobject_cast<QToolBar*>( widget->parent() ) )
        { widget->setContentsMargins( 0,0,0,1 ); }

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

        } else if( qobject_cast<QMenuBar*>( widget ) ) {

            widget->setBackgroundRole( QPalette::NoRole );

        } else if( widget->inherits( "KMultiTabBar" ) ) {

            // kMultiTabBar margins are set to unity for alignment
            // with ( usually sunken ) neighbor frames
            widget->setContentsMargins( 1, 1, 1, 1 );

        } else if( widget->inherits( "Q3ToolBar" ) || qobject_cast<QToolBar*>( widget ) ) {

            widget->setBackgroundRole( QPalette::NoRole );
            widget->setAttribute( Qt::WA_TranslucentBackground );
            addEventFilter( widget );

            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif

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

        } else if( widget->inherits( "QComboBoxPrivateContainer" ) ) {

            addEventFilter( widget );
            widget->setAttribute( Qt::WA_TranslucentBackground );
            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif

        } else if( widget->inherits( "KWin::GeometryTip" ) ) {

            // special handling of kwin geometry tip widget
            addEventFilter( widget );
            widget->setAttribute( Qt::WA_NoSystemBackground );
            widget->setAttribute( Qt::WA_TranslucentBackground );
            if( QLabel* label = qobject_cast<QLabel*>( widget ) )
            {
                label->setFrameStyle( QFrame::NoFrame );
                label->setMargin( 5 );
            }

            #ifdef Q_WS_WIN
            widget->setWindowFlags( widget->windowFlags() | Qt::FramelessWindowHint );
            #endif

        } else if( qobject_cast<QFrame*>( widget ) && widget->parent() && widget->parent()->inherits( "KTitleWidget" ) ) {

            widget->setAutoFillBackground( false );
            widget->setBackgroundRole( QPalette::Window );

        }

        // base class polishing
        QCommonStyle::polish( widget );

    }

    //_______________________________________________________________
    void Style::unpolish( QWidget* widget )
    {

        // register widget to animations
        animations().unregisterWidget( widget );
        transitions().unregisterWidget( widget );
        windowManager().unregisterWidget( widget );
        frameShadowFactory().unregisterWidget( widget );
        mdiWindowShadowFactory().unregisterWidget( widget );
        shadowHelper().unregisterWidget( widget );
        splitterFactory().unregisterWidget( widget );

        if( isKTextEditFrame( widget ) )
        { widget->setAttribute( Qt::WA_Hover, false  ); }

        if( widget && widget->inherits( "Q3ListView" ) ) {

            widget->removeEventFilter( this );
            widget->setAttribute( Qt::WA_Hover, false );

        }

        // event filters
        switch ( widget->windowFlags() & Qt::WindowType_Mask )
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
            || ( widget && widget->inherits( "Q3ToolBar" ) )
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

        QCommonStyle::unpolish( widget );

    }

    //______________________________________________________________
    int Style::pixelMetric( PixelMetric metric, const QStyleOption* option, const QWidget* widget ) const
    {

        // handle special cases
        switch( metric )
        {

            // rely on QCommonStyle here
            case PM_SmallIconSize:
            case PM_ButtonIconSize:
            return KIconLoader::global()->currentSize( KIconLoader::Small );

            case PM_ToolBarIconSize:
            return KIconLoader::global()->currentSize( KIconLoader::Toolbar );

            case PM_LargeIconSize:
            return KIconLoader::global()->currentSize( KIconLoader::Dialog );

            case PM_MessageBoxIconSize:
            return KIconLoader::SizeHuge;

            case PM_DefaultFrameWidth:
            {

                if( qobject_cast<const QLineEdit*>( widget ) ) return LineEdit_FrameWidth;
                else if( qobject_cast<const QComboBox*>( widget ) ) return ComboBox_FrameWidth;
                else if( qobject_cast<const QFrame*>( widget ) )
                {

                    // special case for KTitleWidget: frameWidth is set to zero, since
                    // no frame, nor background is painted for these
                    if( widget->parent() && widget->parent()->inherits( "KTitleWidget" ) ) return 0;
                    else return Frame_FrameWidth;

                }
                else if( qstyleoption_cast<const QStyleOptionGroupBox *>( option ) ) return GroupBox_FrameWidth;
                else return 1;

            }

            case PM_LayoutLeftMargin:
            case PM_LayoutTopMargin:
            case PM_LayoutRightMargin:
            case PM_LayoutBottomMargin:
            {
                // use either Child margin or TopLevel margin, depending on
                // widget type
                if( ( option && ( option->state & QStyle::State_Window ) ) || ( widget && widget->isWindow() ) )
                {

                    return pixelMetric( PM_DefaultTopLevelMargin, option, widget );

                } else {

                    return pixelMetric( PM_DefaultChildMargin, option, widget );

                }

            }

            // push buttons
            /* HACK: needs special case for kcalc buttons, to prevent the application to set too small margins */
            case PM_ButtonMargin:
            { return ( widget && widget->inherits( "KCalcButton" ) ) ? 8:5; }

            case PM_MenuButtonIndicator:
            {
                if( qstyleoption_cast<const QStyleOptionToolButton*>( option ) ) return ToolButton_MenuIndicatorSize;
                else return PushButton_MenuIndicatorSize;
            }

            case PM_ScrollBarExtent:
            return StyleConfigData::scrollBarWidth() + 2;

            case PM_ScrollBarSliderMin: return ScrollBar_MinimumSliderHeight;

            // tooltip label
            case PM_ToolTipLabelFrameWidth:
            {
                if( StyleConfigData::toolTipDrawStyledFrames() ) return 3;
                else break;
            }

            case PM_DefaultChildMargin: return 4;
            case PM_DefaultTopLevelMargin: return 11;
            case PM_DefaultLayoutSpacing: return 4;
            case PM_LayoutHorizontalSpacing: return -1;
            case PM_LayoutVerticalSpacing: return -1;

            // buttons
            case PM_ButtonDefaultIndicator: return 0;
            case PM_ButtonShiftHorizontal: return 0;
            case PM_ButtonShiftVertical: return 0;

            // checkboxes: return radiobutton sizes
            case PM_IndicatorWidth: return CheckBox_Size;
            case PM_IndicatorHeight: return CheckBox_Size;
            case PM_ExclusiveIndicatorWidth: return CheckBox_Size;
            case PM_ExclusiveIndicatorHeight: return CheckBox_Size;
            case PM_CheckListControllerSize: return CheckBox_Size;
            case PM_CheckListButtonSize: return CheckBox_Size;

            // splitters and dock widgets
            case PM_SplitterWidth: return Splitter_Width;
            case PM_DockWidgetFrameWidth: return DockWidget_FrameWidth;
            case PM_DockWidgetSeparatorExtent: return DockWidget_SeparatorExtend;
            case PM_DockWidgetTitleMargin: return DockWidget_TitleMargin;

            // progress bar
            case PM_ProgressBarChunkWidth: return 1;

            // menu bars
            case PM_MenuBarPanelWidth: return 0;
            case PM_MenuBarHMargin: return 0;
            case PM_MenuBarVMargin: return 0;
            case PM_MenuBarItemSpacing: return 0;
            case PM_MenuDesktopFrameWidth: return 0;
            case PM_MenuPanelWidth: return 5;

            case PM_MenuScrollerHeight: return 10;
            case PM_MenuTearoffHeight: return 10;

            // tabbars
            case PM_TabBarTabHSpace: return 0;
            case PM_TabBarTabVSpace: return 0;
            case PM_TabBarBaseHeight: return TabBar_BaseHeight;
            case PM_TabBarBaseOverlap: return TabBar_BaseOverlap;
            case PM_TabBarTabOverlap: return 0;
            case PM_TabBarScrollButtonWidth: return TabBar_ScrollButtonWidth;

            /*
            disable shifts: return because last time I checked it did not work well
            for South and East tabs. Instead the shifts are added directly in
            drawTabBarTabLabel. ( Hugo )
            */
            case PM_TabBarTabShiftVertical: return 0;
            case PM_TabBarTabShiftHorizontal: return 0;

            // sliders
            case PM_SliderThickness: return 23;
            case PM_SliderControlThickness: return 23;
            case PM_SliderLength: return 21;

            // spinboxes
            case PM_SpinBoxFrameWidth: return SpinBox_FrameWidth;

            // comboboxes
            case PM_ComboBoxFrameWidth: return ComboBox_FrameWidth;

            // tree view header
            case PM_HeaderMarkSize: return 9;
            case PM_HeaderMargin: return 3;

            // toolbars
            case PM_ToolBarFrameWidth: return 0;
            case PM_ToolBarHandleExtent: return 6;
            case PM_ToolBarSeparatorExtent: return 6;

            case PM_ToolBarExtensionExtent: return 16;
            case PM_ToolBarItemMargin: return 1;
            case PM_ToolBarItemSpacing: return 1;

            // MDI windows titlebars
            case PM_TitleBarHeight: return 20;

            // spacing between widget and scrollbars
            case PM_ScrollView_ScrollBarSpacing:
            if( const QFrame* frame = qobject_cast<const QFrame*>( widget ) )
            {

                const bool framed( frame->frameShape() != QFrame::NoFrame );
                return framed ? -2:0;

            } else return -2;

            default: break;
        }

        // fallback
        return QCommonStyle::pixelMetric( metric, option, widget );

    }

    //______________________________________________________________
    int Style::styleHint( StyleHint hint, const QStyleOption* option, const QWidget* widget, QStyleHintReturn* returnData ) const
    {

        // handles SH_KCustomStyleElement out of switch statement,
        // to avoid warning at compilation
        if( hint == SH_KCustomStyleElement )
        {
            if( widget ) return _styleElements.value( widget->objectName(), 0 );
            else return 0;
        }

        /*
        special cases, that cannot be registered in styleHint map,
        because of conditional statements
        */
        switch( hint )
        {

            case SH_DialogButtonBox_ButtonsHaveIcons:
            return KGlobalSettings::showIconsOnPushButtons();

            case SH_GroupBox_TextLabelColor:
            if( option ) return option->palette.color( QPalette::WindowText ).rgba();
            else return qApp->palette().color( QPalette::WindowText ).rgba();

            case SH_ItemView_ActivateItemOnSingleClick:
            return helper().config()->group( "KDE" ).readEntry( "SingleClick", KDE_DEFAULT_SINGLECLICK );
            return false;

            case SH_RubberBand_Mask:
            {

                const QStyleOptionRubberBand *opt = qstyleoption_cast<const QStyleOptionRubberBand *>( option );
                if( !opt ) return false;

                if( QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask*>( returnData ) )
                {

                    mask->region = option->rect;

                    // need to check on widget before removing inner region
                    // in order to still preserve rubberband in MainWindow and QGraphicsView
                    // in QMainWindow because it looks better
                    // in QGraphicsView because the painting fails completely otherwise
                    if( !( widget && (
                        qobject_cast<const QGraphicsView*>( widget->parent() ) ||
                        qobject_cast<const QMainWindow*>( widget->parent() ) ) ) )
                        { mask->region -= option->rect.adjusted( 1,1,-1,-1 ); }

                        return true;
                }
                return false;
            }

            case SH_ToolTip_Mask:
            case SH_Menu_Mask:
            {

                if( !helper().hasAlphaChannel( widget ) && ( !widget || widget->isWindow() ) )
                {

                    // mask should be set only if compositing is disabled
                    if( QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>( returnData ) )
                    { mask->region = helper().roundedMask( option->rect ); }

                }

                return true;

            }

            // mouse tracking
            case SH_ComboBox_ListMouseTracking: return true;
            case SH_MenuBar_MouseTracking: return true;
            case SH_Menu_MouseTracking: return true;

            case SH_Menu_SubMenuPopupDelay: return 150;

            case SH_TitleBar_NoBorder: return 0;
            case SH_GroupBox_TextLabelVerticalAlignment: return Qt::AlignVCenter;
            case SH_DialogButtonLayout: return QDialogButtonBox::KdeLayout;
            case SH_ScrollBar_MiddleClickAbsolutePosition: return true;
            case SH_ItemView_ShowDecorationSelected: return false;
            case SH_ItemView_ArrowKeysNavigateIntoChildren: return true;
            case SH_ScrollView_FrameOnlyAroundContents: return true;
            case SH_FormLayoutFormAlignment: return Qt::AlignLeft | Qt::AlignTop;
            case SH_FormLayoutLabelAlignment: return Qt::AlignRight;
            case SH_FormLayoutFieldGrowthPolicy: return QFormLayout::ExpandingFieldsGrow;
            case SH_FormLayoutWrapPolicy: return QFormLayout::DontWrapRows;
            case SH_MessageBox_TextInteractionFlags: return true;
            case SH_WindowFrame_Mask: return false;

            default: return QCommonStyle::styleHint( hint, option, widget, returnData );
        }

    }

    //______________________________________________________________
    QRect Style::subElementRect( SubElement element, const QStyleOption* option, const QWidget* widget ) const
    {


        switch( element )
        {

            // push buttons
            case SE_PushButtonContents: return pushButtonContentsRect( option, widget );
            case SE_PushButtonFocusRect: return defaultSubElementRect( option, widget );

            // checkboxes
            case SE_CheckBoxContents: return checkBoxContentsRect( option, widget );
            case SE_CheckBoxFocusRect: return defaultSubElementRect( option, widget );

            // progress bars
            case SE_ProgressBarGroove: return defaultSubElementRect( option, widget );
            case SE_ProgressBarContents: return progressBarContentsRect( option, widget );
            case SE_ProgressBarLabel: return defaultSubElementRect( option, widget );

            // radio buttons
            case SE_RadioButtonContents: return checkBoxContentsRect( option, widget );
            case SE_RadioButtonFocusRect: return defaultSubElementRect( option, widget );

            // tab widget
            case SE_TabBarTabLeftButton: return tabBarTabLeftButtonRect( option, widget );
            case SE_TabBarTabRightButton: return tabBarTabRightButtonRect( option, widget );
            case SE_TabBarTabText: return tabBarTabTextRect( option, widget );
            case SE_TabWidgetTabContents: return tabWidgetTabContentsRect( option, widget );
            case SE_TabWidgetTabPane: return tabWidgetTabPaneRect( option, widget );
            case SE_TabWidgetLeftCorner: return tabWidgetLeftCornerRect( option, widget );
            case SE_TabWidgetRightCorner: return tabWidgetRightCornerRect( option, widget );

            // toolboxes
            case SE_ToolBoxTabContents: return toolBoxTabContentsRect( option, widget );

            default: return QCommonStyle::subElementRect( element, option, widget );

        }

    }

    //______________________________________________________________
    QRect Style::subControlRect( ComplexControl element, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        switch( element )
        {

            case CC_GroupBox: return groupBoxSubControlRect( option, subControl, widget );
            case CC_ComboBox: return comboBoxSubControlRect( option, subControl, widget );
            case CC_Slider: return sliderSubControlRect( option, subControl, widget );
            case CC_ScrollBar: return scrollBarSubControlRect( option, subControl, widget );
            case CC_SpinBox: return spinBoxSubControlRect( option, subControl, widget );
            default: return QCommonStyle::subControlRect( element, option, subControl, widget );
        }

    }

    //______________________________________________________________
    QSize Style::sizeFromContents( ContentsType element, const QStyleOption* option, const QSize& size, const QWidget* widget ) const
    {

        switch( element )
        {
            case CT_CheckBox: return checkBoxSizeFromContents( option, size, widget );
            case CT_ComboBox: return comboBoxSizeFromContents( option, size, widget );
            case CT_HeaderSection: return headerSectionSizeFromContents( option, size, widget );
            case CT_PushButton: return pushButtonSizeFromContents( option, size, widget );
            case CT_MenuBar: return menuBarSizeFromContents( option, size, widget );
            case CT_MenuBarItem: return menuBarItemSizeFromContents( option, size, widget );
            case CT_MenuItem: return menuItemSizeFromContents( option, size, widget );
            case CT_RadioButton: return checkBoxSizeFromContents( option, size, widget );
            case CT_TabBarTab: return tabBarTabSizeFromContents( option, size, widget );
            case CT_TabWidget: return tabWidgetSizeFromContents( option, size, widget );
            case CT_ToolButton: return toolButtonSizeFromContents( option, size, widget );
            default: return QCommonStyle::sizeFromContents( element, option, size, widget );
        }

    }

    //______________________________________________________________
    QStyle::SubControl Style::hitTestComplexControl( ComplexControl control, const QStyleOptionComplex* option, const QPoint& point, const QWidget* widget ) const
    {
        switch( control )
        {
            case CC_ScrollBar:
            {

                QRect groove = scrollBarSubControlRect( option, SC_ScrollBarGroove, widget );
                if ( groove.contains( point ) )
                {
                    //Must be either page up/page down, or just click on the slider.
                    //Grab the slider to compare
                    QRect slider = scrollBarSubControlRect( option, SC_ScrollBarSlider, widget );

                    if( slider.contains( point ) ) return SC_ScrollBarSlider;
                    else if( preceeds( point, slider, option ) ) return SC_ScrollBarSubPage;
                    else return SC_ScrollBarAddPage;

                }

                //This is one of the up/down buttons. First, decide which one it is.
                if( preceeds( point, groove, option ) )
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

            default: return QCommonStyle::hitTestComplexControl( control, option, point, widget );
        }

    }

    //______________________________________________________________
    void Style::drawPrimitive( PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        painter->save();

        StylePrimitive fcn( 0 );
        switch( element )
        {

            // register primitives for which nothing is done
            case PE_FrameStatusBar: fcn = &Style::emptyPrimitive; break;

            case PE_Frame: fcn = &Style::drawFramePrimitive; break;

            // frame focus primitive is set at run time, in oxygenConfigurationChanged
            case PE_FrameFocusRect: fcn = _frameFocusPrimitive; break;

            case PE_FrameGroupBox: fcn = &Style::drawFrameGroupBoxPrimitive; break;
            case PE_FrameLineEdit: fcn = &Style::drawFramePrimitive; break;
            case PE_FrameMenu: fcn = &Style::drawFrameMenuPrimitive; break;

            // TabBar
            case PE_FrameTabBarBase: fcn = &Style::drawFrameTabBarBasePrimitive; break;
            case PE_FrameTabWidget: fcn = &Style::drawFrameTabWidgetPrimitive; break;

            case PE_FrameWindow: fcn = &Style::drawFrameWindowPrimitive; break;

            // arrows
            case PE_IndicatorArrowUp: fcn = &Style::drawIndicatorArrowUpPrimitive; break;
            case PE_IndicatorArrowDown: fcn = &Style::drawIndicatorArrowDownPrimitive; break;
            case PE_IndicatorArrowLeft: fcn = &Style::drawIndicatorArrowLeftPrimitive; break;
            case PE_IndicatorArrowRight: fcn = &Style::drawIndicatorArrowRightPrimitive; break;

            case PE_IndicatorDockWidgetResizeHandle: fcn = &Style::drawIndicatorDockWidgetResizeHandlePrimitive; break;
            case PE_IndicatorHeaderArrow: fcn = &Style::drawIndicatorHeaderArrowPrimitive; break;

            case PE_PanelButtonCommand: fcn = &Style::drawPanelButtonCommandPrimitive; break;
            case PE_PanelButtonTool: fcn = &Style::drawPanelButtonToolPrimitive; break;

            case PE_PanelItemViewItem: fcn = &Style::drawPanelItemViewItemPrimitive; break;
            case PE_PanelLineEdit: fcn = &Style::drawPanelLineEditPrimitive; break;
            case PE_PanelMenu: fcn = &Style::drawPanelMenuPrimitive; break;
            case PE_PanelScrollAreaCorner: fcn = &Style::drawPanelScrollAreaCornerPrimitive; break;
            case PE_PanelTipLabel: fcn = &Style::drawPanelTipLabelPrimitive; break;

            case PE_IndicatorMenuCheckMark: fcn = &Style::drawIndicatorMenuCheckMarkPrimitive; break;
            case PE_Q3CheckListIndicator: fcn = &Style::drawQ3CheckListIndicatorPrimitive; break;
            case PE_Q3CheckListExclusiveIndicator: fcn = &Style::drawQ3CheckListExclusiveIndicatorPrimitive; break;
            case PE_IndicatorBranch: fcn = &Style::drawIndicatorBranchPrimitive; break;
            case PE_IndicatorButtonDropDown: fcn = &Style::drawIndicatorButtonDropDownPrimitive; break;
            case PE_IndicatorCheckBox: fcn = &Style::drawIndicatorCheckBoxPrimitive; break;
            case PE_IndicatorRadioButton: fcn = &Style::drawIndicatorRadioButtonPrimitive; break;
            case PE_IndicatorTabTear: fcn = &Style::drawIndicatorTabTearPrimitive; break;
            case PE_IndicatorToolBarHandle: fcn = &Style::drawIndicatorToolBarHandlePrimitive; break;
            case PE_IndicatorToolBarSeparator: fcn = &Style::drawIndicatorToolBarSeparatorPrimitive; break;

            case PE_Widget: fcn = &Style::drawWidgetPrimitive; break;

            default: break;

        }

        // try find primitive in map, and run.
        // exit if result is true, otherwise fallback to generic case
        if( !( fcn && ( this->*fcn )( option, painter, widget ) ) )
        { QCommonStyle::drawPrimitive( element, option, painter, widget ); }

        painter->restore();

    }

    //______________________________________________________________
    void Style::drawControl( ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        painter->save();

        StyleControl fcn( 0 );
        if( element == CE_CapacityBar )
        {

            fcn = &Style::drawCapacityBarControl;

        } else switch( element ) {

            case CE_ComboBoxLabel: fcn = &Style::drawComboBoxLabelControl; break;
            case CE_DockWidgetTitle: fcn = &Style::drawDockWidgetTitleControl; break;
            case CE_HeaderEmptyArea: fcn = &Style::drawHeaderEmptyAreaControl; break;
            case CE_HeaderLabel: fcn = &Style::drawHeaderLabelControl; break;
            case CE_HeaderSection: fcn = &Style::drawHeaderSectionControl; break;
            case CE_MenuBarEmptyArea: fcn = &Style::emptyControl; break;
            case CE_MenuBarItem: fcn = &Style::drawMenuBarItemControl; break;
            case CE_MenuItem: fcn = &Style::drawMenuItemControl; break;
            case CE_ProgressBar: fcn = &Style::drawProgressBarControl; break;
            case CE_ProgressBarContents: fcn = &Style::drawProgressBarContentsControl; break;
            case CE_ProgressBarGroove: fcn = &Style::drawProgressBarGrooveControl; break;
            case CE_ProgressBarLabel: fcn = &Style::drawProgressBarLabelControl; break;

            /*
            for CE_PushButtonBevel the only thing that is done is draw the PanelButtonCommand primitive
            since the prototypes are identical we register the second directly in the control map: fcn = without
            using an intermediate function
            */
            case CE_PushButtonBevel: fcn = &Style::drawPanelButtonCommandPrimitive; break;
            case CE_PushButtonLabel: fcn = &Style::drawPushButtonLabelControl; break;

            case CE_RubberBand: fcn = &Style::drawRubberBandControl; break;
            case CE_ScrollBarSlider: fcn = &Style::drawScrollBarSliderControl; break;
            case CE_ScrollBarAddLine: fcn = &Style::drawScrollBarAddLineControl; break;
            case CE_ScrollBarAddPage: fcn = &Style::drawScrollBarAddPageControl; break;
            case CE_ScrollBarSubLine: fcn = &Style::drawScrollBarSubLineControl; break;
            case CE_ScrollBarSubPage: fcn = &Style::drawScrollBarSubPageControl; break;

            case CE_ShapedFrame: fcn = &Style::drawShapedFrameControl; break;
            case CE_SizeGrip: fcn = &Style::drawSizeGripControl; break;
            case CE_Splitter: fcn = &Style::drawSplitterControl; break;
            case CE_TabBarTabLabel: fcn = &Style::drawTabBarTabLabelControl; break;

            // default tab style is 'SINGLE'
            case CE_TabBarTabShape: fcn = _tabBarTabShapeControl; break;

            case CE_ToolBar: fcn = &Style::drawToolBarControl; break;
            case CE_ToolBoxTabLabel: fcn = &Style::drawToolBoxTabLabelControl; break;
            case CE_ToolBoxTabShape: fcn = &Style::drawToolBoxTabShapeControl; break;
            case CE_ToolButtonLabel: fcn = &Style::drawToolButtonLabelControl; break;

            default: break;

        }

        if( !( fcn && ( this->*fcn )( option, painter, widget ) ) )
        { QCommonStyle::drawControl( element, option, painter, widget ); }

        painter->restore();

    }

    //______________________________________________________________
    void Style::drawComplexControl( ComplexControl element, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        painter->save();

        StyleComplexControl fcn( 0 );
        switch( element )
        {

            case CC_ComboBox: fcn = &Style::drawComboBoxComplexControl; break;
            case CC_Dial: fcn = &Style::drawDialComplexControl; break;
            case CC_GroupBox: fcn = &Style::drawGroupBoxComplexControl; break;
            case CC_Q3ListView: fcn = &Style::drawQ3ListViewComplexControl; break;
            case CC_Slider: fcn = &Style::drawSliderComplexControl; break;
            case CC_SpinBox: fcn = &Style::drawSpinBoxComplexControl; break;
            case CC_TitleBar: fcn = &Style::drawTitleBarComplexControl; break;
            case CC_ToolButton: fcn = &Style::drawToolButtonComplexControl; break;
            default: break;

        }

        if( !( fcn && ( this->*fcn )( option, painter, widget ) ) )
        { QCommonStyle::drawComplexControl( element, option, painter, widget ); }

        painter->restore();

    }


    //___________________________________________________________________________________
    void Style::drawItemText(
        QPainter* painter, const QRect& r, int flags, const QPalette& palette, bool enabled,
        const QString &text, QPalette::ColorRole textRole ) const
    {

        // hide mnemonics if requested
        if( (!_showMnemonics) && ( flags & Qt::TextShowMnemonic ) && !( flags&Qt::TextHideMnemonic ) )
        {
            flags &= ~Qt::TextShowMnemonic;
            flags |= Qt::TextHideMnemonic;
        }

        if( animations().widgetEnabilityEngine().enabled() )
        {

            /*
            check if painter engine is registered to WidgetEnabilityEngine, and animated
            if yes, merge the palettes. Note: a static_cast is safe here, since only the address
            of the pointer is used, not the actual content.
            */
            const QWidget* widget( static_cast<const QWidget*>( painter->device() ) );
            if( animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
            {

                const QPalette pal = helper().mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  );
                return QCommonStyle::drawItemText( painter, r, flags, pal, enabled, text, textRole );

            }

        }

        return QCommonStyle::drawItemText( painter, r, flags, palette, enabled, text, textRole );

    }


    //_____________________________________________________________________
    bool Style::eventFilter( QObject *object, QEvent *event )
    {

        if( QTabBar* tabBar = qobject_cast<QTabBar*>( object ) ) { return eventFilterTabBar( tabBar, event ); }
        if( QToolBar* toolBar = qobject_cast<QToolBar*>( object ) ) { return eventFilterToolBar( toolBar, event ); }
        if( QDockWidget* dockWidget = qobject_cast<QDockWidget*>( object ) ) { return eventFilterDockWidget( dockWidget, event ); }
        if( QToolBox* toolBox = qobject_cast<QToolBox*>( object ) ) { return eventFilterToolBox( toolBox, event ); }
        if( QMdiSubWindow* subWindow = qobject_cast<QMdiSubWindow*>( object ) ) { return eventFilterMdiSubWindow( subWindow, event ); }
        if( QScrollBar* scrollBar = qobject_cast<QScrollBar*>( object ) ) { return eventFilterScrollBar( scrollBar, event ); }

        // cast to QWidget
        QWidget *widget = static_cast<QWidget*>( object );

        if( widget->inherits( "Q3ListView" ) ) { return eventFilterQ3ListView( widget, event ); }
        if( widget->inherits( "QComboBoxPrivateContainer" ) ) { return eventFilterComboBoxContainer( widget, event ); }
        if( widget->inherits( "KWin::GeometryTip" ) ) { return eventFilterGeometryTip( widget, event ); }

        return QCommonStyle::eventFilter( object, event );

    }

    //_________________________________________________________
    bool Style::eventFilterComboBoxContainer( QWidget* widget, QEvent* event )
    {
        switch( event->type() )
        {

            case QEvent::Show:
            case QEvent::Resize:
            {
                if( !helper().hasAlphaChannel( widget ) ) widget->setMask( helper().roundedMask( widget->rect() ) );
                else widget->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                QPainter painter( widget );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const QRect r( widget->rect() );
                const QColor color( widget->palette().color( widget->window()->backgroundRole() ) );
                const bool hasAlpha( helper().hasAlphaChannel( widget ) );

                if( hasAlpha )
                {

                    TileSet *tileSet( helper().roundCorner( color ) );
                    tileSet->render( r, &painter );
                    painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                    painter.setClipRegion( helper().roundedMask( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

                }

                // background
                helper().renderMenuBackground( &painter, paintEvent->rect(), widget, widget->palette() );

                // frame
                if( hasAlpha ) painter.setClipping( false );

                helper().drawFloatFrame( &painter, r, color, !hasAlpha );
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
                if( dockWidget->isFloating() )
                {
                    if( helper().compositingActive() )
                    {

                        // TODO: should not be needed
                        dockWidget->setMask( helper().roundedMask( dockWidget->rect().adjusted( 1, 1, -1, -1 ) ) );

                    } else {

                        dockWidget->setMask( helper().roundedMask( dockWidget->rect() ) );

                    }

                } else dockWidget->clearMask();

                return false;
            }

            case QEvent::Paint:
            {
                QPainter painter( dockWidget );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const QColor color( dockWidget->palette().color( QPalette::Window ) );
                const QRect r( dockWidget->rect() );
                if( dockWidget->isWindow() )
                {

                    helper().renderWindowBackground( &painter, r, dockWidget, color );

                    #ifndef Q_WS_WIN
                    helper().drawFloatFrame( &painter, r, color, !helper().compositingActive() );
                    #endif

                } else {

                    // need to draw window background for autoFilled dockWidgets for better rendering
                    if( dockWidget->autoFillBackground() )
                    { helper().renderWindowBackground( &painter, r, dockWidget, color ); }

                    // adjust color
                    QColor top( helper().backgroundColor( color, dockWidget, r.topLeft() ) );
                    QColor bottom( helper().backgroundColor( color, dockWidget, r.bottomLeft() ) );
                    TileSet *tileSet = helper().dockFrame( top, bottom );
                    tileSet->render( r, &painter );

                }

                return false;
            }

            default: return false;

        }

    }

    //____________________________________________________________________________
    bool Style::eventFilterGeometryTip( QWidget* widget, QEvent* event )
    {
        switch( event->type() )
        {

            case QEvent::Show:
            case QEvent::Resize:
            {

                // make sure mask is appropriate
                if( !helper().hasAlphaChannel( widget ) ) widget->setMask( helper().roundedMask( widget->rect() ) );
                else widget->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                const QColor color( widget->palette().window().color() );
                const QRect r( widget->rect() );

                QPainter painter( widget );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const bool hasAlpha( helper().hasAlphaChannel( widget ) );
                if( hasAlpha )
                {

                    painter.setCompositionMode( QPainter::CompositionMode_Source );
                    TileSet *tileSet( helper().roundCorner( color ) );
                    tileSet->render( r, &painter );

                    painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                    painter.setClipRegion( helper().roundedMask( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

                }

                helper().renderMenuBackground( &painter, r, widget,color );

                // frame
                if( hasAlpha ) painter.setClipping( false );
                helper().drawFloatFrame( &painter, r, color, !hasAlpha );

            }

            return false;

            default: return false;

        }

        // continue with normal painting
        return false;

    }

    //____________________________________________________________________________
    bool Style::eventFilterMdiSubWindow( QMdiSubWindow* subWindow, QEvent* event )
    {

        if( event->type() == QEvent::Paint )
        {

            QPainter painter( subWindow );
            QRect clip( static_cast<QPaintEvent*>( event )->rect() );
            if( subWindow->isMaximized() ) helper().renderWindowBackground( &painter, clip, subWindow, subWindow->palette() );
            else {

                painter.setClipRect( clip );

                const QRect r( subWindow->rect() );
                TileSet *tileSet( helper().roundCorner( subWindow->palette().color( subWindow->backgroundRole() ) ) );
                tileSet->render( r, &painter );

                painter.setClipRegion( helper().roundedMask( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );
                helper().renderWindowBackground( &painter, clip, subWindow, subWindow, subWindow->palette(), 0, 58 );

            }

        }

        // continue with normal painting
        return false;

    }

    //__________________________________________________________________________________
    bool Style::eventFilterQ3ListView( QWidget* widget, QEvent* event )
    {
        // this apparently fixes a Qt bug with Q3ListView, consisting in
        // the fact that Focus events do not trigger repaint of these
        switch( event->type() )
        {
            case QEvent::FocusIn: widget->update(); return false;
            case QEvent::FocusOut: widget->update(); return false;
            default: return false;
        }

    }

    //_________________________________________________________
    bool Style::eventFilterScrollBar( QWidget* widget, QEvent* event )
    {

        if( event->type() == QEvent::Paint )
        {
            QPainter painter( widget );
            painter.setClipRegion( static_cast<QPaintEvent*>( event )->region() );
            helper().renderWindowBackground( &painter, widget->rect(), widget,widget->palette() );
        }

        return false;
    }

    //_____________________________________________________________________
    bool Style::eventFilterTabBar( QWidget* widget, QEvent* event )
    {
        if( event->type() == QEvent::Paint && tabBarData().locks( widget ) )
        {
            /*
            this makes sure that tabBar base is drawn ( and drawn only once )
            every time a replaint is triggered by dragging a tab around
            */
            tabBarData().setDirty();
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
                if( toolBar->isFloating() && !helper().hasAlphaChannel( toolBar ) )
                {

                    toolBar->setMask( helper().roundedMask( toolBar->rect() ) );

                } else  toolBar->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                QPainter painter( toolBar );
                QPaintEvent *paintEvent = static_cast<QPaintEvent*>( event );
                painter.setClipRegion( paintEvent->region() );

                const QRect r( toolBar->rect() );
                const QColor color( toolBar->palette().window().color() );

                // default painting when not qrealing
                if( !toolBar->isFloating() )
                {

                    // background has to be rendered explicitly
                    // when one of the parent has autofillBackground set to true
                    if( helper().checkAutoFillBackground( toolBar ) )
                    { helper().renderWindowBackground( &painter, r, toolBar, color ); }

                    return false;

                }

                const bool hasAlpha( helper().hasAlphaChannel( toolBar ) );
                if( hasAlpha )
                {
                    painter.setCompositionMode( QPainter::CompositionMode_Source );
                    TileSet *tileSet( helper().roundCorner( color ) );
                    tileSet->render( r, &painter );

                    painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
                    painter.setClipRegion( helper().roundedMask( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );
                }

                // background
                helper().renderWindowBackground( &painter, r, toolBar, color );

                if( toolBar->isMovable() )
                {
                    // remaining painting: need to add handle
                    // this is copied from QToolBar::paintEvent
                    QStyleOptionToolBar opt;
                    opt.initFrom( toolBar );
                    if( toolBar->orientation() == Qt::Horizontal )
                    {

                        opt.rect = handleRTL( &opt, QRect( r.topLeft(), QSize( 8, r.height() ) ) );
                        opt.state |= QStyle::State_Horizontal;

                    } else {

                        opt.rect = handleRTL( &opt, QRect( r.topLeft(), QSize( r.width(), 8 ) ) );

                    }

                    drawIndicatorToolBarHandlePrimitive( &opt, &painter, toolBar );

                }

                // frame
                if( hasAlpha ) painter.setClipping( false );
                helper().drawFloatFrame( &painter, r, color, !hasAlpha );

                return true;

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

                const QRect r( toolBox->rect() );
                const StyleOptions opts( NoFill );

                QPainter painter( toolBox );
                painter.setClipRegion( static_cast<QPaintEvent*>( event )->region() );
                renderSlab( &painter, r, toolBox->palette().color( QPalette::Button ), opts );

            }
        }

        return false;
    }

    //____________________________________________________________________
    QRect Style::progressBarContentsRect( const QStyleOption* option, const QWidget* ) const
    {
        const QRect out( insideMargin( option->rect, ProgressBar_GrooveMargin ) );
        const QStyleOptionProgressBarV2 *pbOpt( qstyleoption_cast<const QStyleOptionProgressBarV2 *>( option ) );
        if( pbOpt && pbOpt->orientation == Qt::Vertical ) return out.adjusted( 0, 1, 0, -1 );
        else return out.adjusted( 1, 0, -1, 0 );
    }

    //____________________________________________________________________
    QRect Style::tabBarTabButtonRect( SubElement element, const QStyleOption* option, const QWidget* widget ) const
    {

        const QStyleOptionTab* tabOpt( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if ( !tabOpt ) return QRect();

        QRect r( QCommonStyle::subElementRect( element, option, widget ) );
        const bool selected( option->state&State_Selected );

        switch( tabOpt->shape )
        {

            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            r.translate( 0, -1 );
            if( selected ) r.translate( 0, -1 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            r.translate( 0, -1 );
            if( selected ) r.translate( 0, 1 );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            r.translate( 0, 1 );
            if( selected )  r.translate( -1, 0 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            r.translate( 0, -2 );
            if( selected ) r.translate( 1, 0 );
            break;

            default: break;

        }
        return r;
    }

    //____________________________________________________________________
    QRect Style::tabWidgetTabContentsRect( const QStyleOption* option, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionTabWidgetFrame* tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option );
        if( !tabOpt ) return option->rect;

        // do nothing if tabbar is hidden
        if( tabOpt->tabBarSize.isEmpty() ) return option->rect;

        QRect r( option->rect );


        // include margins
        r = tabWidgetTabPaneRect( option, widget );

        // document mode
        const bool documentMode( tabOpt->lineWidth == 0 );

        if( !documentMode )
        {
            r = insideMargin( r, TabWidget_ContentsMargin );
            r.translate( 0, -1 );
        }

        return r;

    }

    //____________________________________________________________________
    QRect Style::tabWidgetTabPaneRect( const QStyleOption* option, const QWidget* ) const
    {


        const QStyleOptionTabWidgetFrame* tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option );
        if( !tabOpt ) return option->rect;

        QRect r( option->rect );
        const bool documentMode( tabOpt->lineWidth == 0 );
        int overlap( TabBar_BaseOverlap );
        if( documentMode ) overlap -= TabWidget_ContentsMargin;

        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                if( documentMode ) overlap++;
                r.setTop( r.top() + qMax( tabOpt->tabBarSize.height() - overlap, 0 ) );
                break;
            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                if( documentMode ) overlap--;
                r.setBottom( r.bottom() - qMax( tabOpt->tabBarSize.height() - overlap, 0 ) );
                break;
            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                r.setLeft( r.left() + qMax( tabOpt->tabBarSize.width() - overlap, 0 ) );
                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                r.setRight( r.right() - qMax( tabOpt->tabBarSize.width() - overlap, 0 ) );
                break;
            }

        }

        return r;

    }

    //____________________________________________________________________
    QRect Style::tabWidgetLeftCornerRect( const QStyleOption* option, const QWidget* widget ) const
    {

        const QStyleOptionTabWidgetFrame *tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>( option );
        if( !tabOpt ) return QRect();

        QRect r( option->rect );
        const QRect paneRect( subElementRect( SE_TabWidgetTabPane, option, widget ) );

        const QTabWidget* tabWidget( qobject_cast<const QTabWidget*>( widget ) );
        const bool documentMode( tabWidget ? tabWidget->documentMode() : false );

        const QSize& size( tabOpt->leftCornerWidgetSize );
        const int h( size.height() );
        const int w( size.width() );

        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            r = QRect( QPoint( paneRect.x(), paneRect.y() - h ), size );
            r = visualRect( tabOpt->direction, tabOpt->rect, r );
            if( !documentMode ) r.translate( 0, 3 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            r = QRect( QPoint( paneRect.x(), paneRect.height() ), size );
            r = visualRect( tabOpt->direction, tabOpt->rect, r );
            if( !documentMode ) r.translate( 0, -3 );
            else r.translate( 0, 2 );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            r = QRect( QPoint( paneRect.x() - w, paneRect.y() ), size );
            if( !documentMode ) r.translate( 2, 0 );
            else r.translate( -2, 0 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            r = QRect( QPoint( paneRect.x() + paneRect.width(), paneRect.y() ), size );
            if( !documentMode ) r.translate( -2, 0 );
            else r.translate( 2, 0 );
            break;

            default:
            break;
        }

        return r;

    }

    //____________________________________________________________________
    QRect Style::tabWidgetRightCornerRect( const QStyleOption* option, const QWidget* widget ) const
    {

        const QStyleOptionTabWidgetFrame *tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>( option );
        if( !tabOpt ) return QRect();

        QRect r( option->rect );
        const QRect paneRect( subElementRect( SE_TabWidgetTabPane, option, widget ) );

        const QTabWidget* tabWidget( qobject_cast<const QTabWidget*>( widget ) );
        const bool documentMode( tabWidget ? tabWidget->documentMode() : false );

        const QSize& size( tabOpt->rightCornerWidgetSize );
        const int h( size.height() );
        const int w( size.width() );

        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            r = QRect( QPoint( paneRect.right() - w + 1, paneRect.y() - h ), size );
            r = visualRect( tabOpt->direction, tabOpt->rect, r );
            if( !documentMode ) r.translate( 0, 3 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            r = QRect( QPoint( paneRect.right() - w + 1, paneRect.height() ), size );
            r = visualRect( tabOpt->direction, tabOpt->rect, r );
            if( !documentMode ) r.translate( 0, -3 );
            else r.translate( 0, 2 );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            r = QRect( QPoint( paneRect.x() - w, paneRect.bottom() - h + 1 ), size );
            if( !documentMode ) r.translate( 2, 0 );
            else r.translate( -2, 0 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            r = QRect( QPoint( paneRect.x() + paneRect.width(), paneRect.bottom() - h + 1 ), size );
            if( !documentMode ) r.translate( -2, 0 );
            else r.translate( 2, 0 );
            break;

            default:
            break;
        }

        return r;

    }

    //______________________________________________________________
    QRect Style::groupBoxSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        QRect r = option->rect;

        //
        switch ( subControl )
        {

            case SC_GroupBoxFrame: return r.adjusted( -1, -2, 1, 0 );

            case SC_GroupBoxContents:
            {

                // cast option and check
                const QStyleOptionGroupBox *gbOpt = qstyleoption_cast<const QStyleOptionGroupBox *>( option );
                if( !gbOpt ) break;

                const bool isFlat( gbOpt->features & QStyleOptionFrameV2::Flat );
                const int th( gbOpt->fontMetrics.height() + 8 );
                const QRect cr( subElementRect( SE_CheckBoxIndicator, option, widget ) );
                const int fw( pixelMetric( PM_DefaultFrameWidth, option, widget ) );
                const bool checkable( gbOpt->subControls & QStyle::SC_GroupBoxCheckBox );
                const bool emptyText( gbOpt->text.isEmpty() );

                r.adjust( fw, fw, -fw, -fw );
                if( checkable && !emptyText ) r.adjust( 0, qMax( th, cr.height() ), 0, 0 );
                else if( checkable ) r.adjust( 0, cr.height(), 0, 0 );
                else if( !emptyText ) r.adjust( 0, th, 0, 0 );

                // add additional indentation to flat group boxes
                if( isFlat )
                {
                    const int leftMarginExtension( 16 );
                    r = visualRect( option->direction, r, r.adjusted( leftMarginExtension,0,0,0 ) );
                }

                return r;
            }

            case SC_GroupBoxCheckBox:
            case SC_GroupBoxLabel:
            {
                // cast option and check
                const QStyleOptionGroupBox *gbOpt = qstyleoption_cast<const QStyleOptionGroupBox *>( option );
                if( !gbOpt ) break;

                const bool isFlat( gbOpt->features & QStyleOptionFrameV2::Flat );
                QFont font = widget->font();

                // calculate text width assuming bold text in flat group boxes
                if( isFlat ) font.setBold( true );

                QFontMetrics fontMetrics = QFontMetrics( font );
                const int h( fontMetrics.height() );
                const int tw( fontMetrics.size( Qt::TextShowMnemonic, gbOpt->text + QLatin1String( "  " ) ).width() );
                r.setHeight( h );

                // translate down by 6 pixels in non flat mode,
                // to avoid collision with groupbox frame
                if( !isFlat ) r.moveTop( 6 );

                QRect cr;
                if( gbOpt->subControls & QStyle::SC_GroupBoxCheckBox )
                {
                    cr = subElementRect( SE_CheckBoxIndicator, option, widget );
                    QRect gcr( ( gbOpt->rect.width() - tw -cr.width() )/2 , ( h-cr.height() )/2+r.y(), cr.width(), cr.height() );
                    if( subControl == SC_GroupBoxCheckBox )
                    {
                        if( !isFlat ) return visualRect( option->direction, option->rect, gcr );
                        else return visualRect( option->direction, option->rect, QRect( 0,0,cr.width(),cr.height() ) );
                    }
                }

                // left align labels in flat group boxes, center align labels in framed group boxes
                if( isFlat ) r = QRect( cr.width(),r.y(),tw,r.height() );
                else r = QRect( ( gbOpt->rect.width() - tw - cr.width() )/2 + cr.width(), r.y(), tw, r.height() );

                return visualRect( option->direction, option->rect, r );
            }

            default: break;

        }

        return QCommonStyle::subControlRect( CC_GroupBox, option, subControl, widget );
    }

    //___________________________________________________________________________________________________________________
    QRect Style::comboBoxSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        const QRect& r( option->rect );
        const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>( option );
        if( !cb ) return QCommonStyle::subControlRect( CC_ComboBox, option, subControl, widget );

        switch( subControl )
        {
            case SC_ComboBoxFrame:
            { return cb->frame ? r : QRect(); }

            case SC_ComboBoxListBoxPopup:
            { return r.adjusted( 1,0,-1,0 ); }

            case SC_ComboBoxArrow:
            case SC_ComboBoxEditField:
            {
                // frame width
                int fw = ComboBox_FrameWidth;

                // button width
                int bw = ComboBox_ButtonWidth;

                // button margin
                int bm = ComboBox_ButtonMargin;
                int bml = bm + ComboBox_ButtonMargin_Left;
                int bmr = bm + ComboBox_ButtonMargin_Right;
                int bmt = bm + ComboBox_ButtonMargin_Top;
                int bmb = bm + ComboBox_ButtonMargin_Bottom;

                // ComboBox without a frame, set the corresponding layout values to 0, reduce button width.
                if( !cb->frame )
                {
                    bw = bw - bmr; // reduce button with as the right button margin will be ignored.
                    fw = 0;

                    // TODO: why is bml not also set to 0
                    bmt = bmb = 0;
                }

                if( subControl == SC_ComboBoxArrow )
                {

                    return
                        handleRTL( option,
                        QRect( r.right()-bw+bml+1, r.top()+bmt, bw-bml-bmr, r.height()-bmt-bmb ) );

                } else {

                    QRect labelRect( r.left()+fw, r.top()+fw, r.width()-fw-bw, r.height()-2*fw );
                    labelRect = insideMargin( labelRect,
                        ComboBox_ContentsMargin,
                        ComboBox_ContentsMargin_Left,
                        ComboBox_ContentsMargin_Top,
                        ComboBox_ContentsMargin_Right,
                        ComboBox_ContentsMargin_Bottom );
                    return handleRTL( option, labelRect );

                }

            }

            default: break;

        }

        return QCommonStyle::subControlRect( CC_ComboBox, option, subControl, widget );

    }

    //___________________________________________________________________________________________________________________
    QRect Style::scrollBarInternalSubControlRect( const QStyleOptionComplex* option, SubControl subControl ) const
    {

        const QRect& r = option->rect;
        const State& flags( option->state );
        const bool horizontal( flags&State_Horizontal );

        switch ( subControl )
        {

            case SC_ScrollBarSubLine:
            {
                int majorSize( scrollBarButtonHeight( _subLineButtons ) );
                if( horizontal ) return handleRTL( option, QRect( r.x(), r.y(), majorSize, r.height() ) );
                else return handleRTL( option, QRect( r.x(), r.y(), r.width(), majorSize ) );

            }

            case SC_ScrollBarAddLine:
            {
                int majorSize( scrollBarButtonHeight( _addLineButtons ) );
                if( horizontal ) return handleRTL( option, QRect( r.right() - majorSize, r.y(), majorSize, r.height() ) );
                else return handleRTL( option, QRect( r.x(), r.bottom() - majorSize, r.width(), majorSize ) );
            }

            default: return QRect();

        }
    }

    //___________________________________________________________________________________________________________________
    QRect Style::sliderSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {
        switch( subControl )
        {
            case SC_SliderHandle:
            return QCommonStyle::subControlRect( CC_Slider, option, subControl, widget );

            case SC_SliderGroove:
            {
                QRect groove( QCommonStyle::subControlRect( CC_Slider, option, subControl, widget ) );
                if( const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>( option ) )
                {
                    const bool horizontal( slider->orientation == Qt::Horizontal );
                    if( horizontal )
                    {

                        const int center( groove.center().y() );
                        groove = QRect( groove.left(), center-Slider_GrooveWidth/2, groove.width(), Slider_GrooveWidth  ).adjusted( 3, 0, -3, 0 );
                        groove.adjust( 2, 1, -2, 0 );

                    } else {

                        const int center( groove.center().x() );
                        groove = QRect( center-Slider_GrooveWidth/2, groove.top(), Slider_GrooveWidth, groove.height() ).adjusted( 0, 3, 0, -3 );
                        groove.adjust( 0, 2, 0, -2 );

                    }

                }

                return groove;

            }

            default:
            return QCommonStyle::subControlRect( CC_Slider, option, subControl, widget );

        }

    }

    //___________________________________________________________________________________________________________________
    QRect Style::scrollBarSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {
        const QRect& r = option->rect;
        const State& flags( option->state );
        const bool horizontal( flags&State_Horizontal );

        switch ( subControl )
        {
            //For both arrows, we return -everything-,
            //to get stuff to repaint right. See scrollBarInternalSubControlRect
            //for the real thing
            case SC_ScrollBarSubLine:
            case SC_ScrollBarAddLine:
            if( horizontal ) return r.adjusted( 0, 1, 0, -1 );
            else return r.adjusted( 1, 0, -1, 0 );

            //The main groove area. This is used to compute the others...
            case SC_ScrollBarGroove:
            {
                QRect top = handleRTL( option, scrollBarInternalSubControlRect( option, SC_ScrollBarSubLine ) );
                QRect bot = handleRTL( option, scrollBarInternalSubControlRect( option, SC_ScrollBarAddLine ) );

                QPoint topLeftCorner;
                QPoint botRightCorner;

                if( horizontal )
                {

                    topLeftCorner  = QPoint( top.right() + 1, top.top() );
                    botRightCorner = QPoint( bot.left()  - 1, top.bottom() );

                } else {
                    topLeftCorner  = QPoint( top.left(),  top.bottom() + 1 );
                    botRightCorner = QPoint( top.right(), bot.top()    - 1 );
                }

                // define rect, and reduce margins
                QRect r( topLeftCorner, botRightCorner );
                if( horizontal ) r.adjust( 0, 1, 0, -1 );
                else r.adjust( 1, 0, -1, 0 );

                return handleRTL( option, r );
            }

            case SC_ScrollBarSlider:
            {
                const QStyleOptionSlider* slOpt = qstyleoption_cast<const QStyleOptionSlider*>( option );
                if( !slOpt ) return QRect();

                //We do handleRTL here to unreflect things if need be
                QRect groove = handleRTL( option, scrollBarSubControlRect( option, SC_ScrollBarGroove, widget ) );

                if ( slOpt->minimum == slOpt->maximum ) return groove;

                //Figure out how much room we have..
                int space( horizontal ? groove.width() : groove.height() );

                //Calculate the portion of this space that the slider should take up.
                int sliderSize = space * qreal( slOpt->pageStep ) / ( slOpt->maximum - slOpt->minimum + slOpt->pageStep );
                sliderSize = qMax( sliderSize, ( int )ScrollBar_MinimumSliderHeight );
                sliderSize = qMin( sliderSize, space );

                space -= sliderSize;
                if( space <= 0 ) return groove;

                int pos = qRound( qreal( slOpt->sliderPosition - slOpt->minimum )/ ( slOpt->maximum - slOpt->minimum )*space );
                if( slOpt->upsideDown ) pos = space - pos;
                if( horizontal ) return handleRTL( option, QRect( groove.x() + pos, groove.y(), sliderSize, groove.height() ) );
                else return handleRTL( option, QRect( groove.x(), groove.y() + pos, groove.width(), sliderSize ) );
            }

            case SC_ScrollBarSubPage:
            {

                //We do handleRTL here to unreflect things if need be
                QRect slider = handleRTL( option, scrollBarSubControlRect( option, SC_ScrollBarSlider, widget ) );
                QRect groove = handleRTL( option, scrollBarSubControlRect( option, SC_ScrollBarGroove, widget ) );

                if( horizontal ) return handleRTL( option, QRect( groove.x(), groove.y(), slider.x() - groove.x(), groove.height() ) );
                else return handleRTL( option, QRect( groove.x(), groove.y(), groove.width(), slider.y() - groove.y() ) );
            }

            case SC_ScrollBarAddPage:
            {

                //We do handleRTL here to unreflect things if need be
                QRect slider = handleRTL( option, scrollBarSubControlRect( option, SC_ScrollBarSlider, widget ) );
                QRect groove = handleRTL( option, scrollBarSubControlRect( option, SC_ScrollBarGroove, widget ) );

                if( horizontal ) return handleRTL( option, QRect( slider.right() + 1, groove.y(), groove.right() - slider.right(), groove.height() ) );
                else return handleRTL( option, QRect( groove.x(), slider.bottom() + 1, groove.width(), groove.bottom() - slider.bottom() ) );

            }

            default: return QRect();
        }
    }

    //___________________________________________________________________________________________________________________
    QRect Style::spinBoxSubControlRect( const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget ) const
    {

        const QRect& r( option->rect );
        const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>( option );
        if( !sb ) return r;

        int fw = SpinBox_FrameWidth;
        int bw = SpinBox_ButtonWidth;
        int bm = SpinBox_ButtonMargin;
        int bml = bm + SpinBox_ButtonMargin_Left;
        int bmt = bm + SpinBox_ButtonMargin_Top;
        int bmr = bm + SpinBox_ButtonMargin_Right;
        int bmb = bm + SpinBox_ButtonMargin_Bottom;
        int bs = 0 ;

        if( !sb->frame )
        {
            bw = bw - bmr;
            fw = 0;

            // TODO: why is bml not also set to 0
            bmt = bmb = bmr = 0;
        }

        const int buttonsWidth = bw-bml-bmr;
        const int buttonsLeft = r.right()-bw+bml+1;

        // compute the height of each button...
        const int availableButtonHeight = r.height()-bmt-bmb - bs;
        const int heightUp = availableButtonHeight / 2;
        const int heightDown = availableButtonHeight - heightUp;

        switch ( subControl )
        {

            case SC_SpinBoxUp:
            return handleRTL( option, QRect( buttonsLeft, r.top()+bmt, buttonsWidth, heightUp ) );

            case SC_SpinBoxDown:
            return handleRTL( option, QRect( buttonsLeft, r.bottom()-bmb-heightDown, buttonsWidth, heightDown ) );

            case SC_SpinBoxEditField:
            {
                const QRect labelRect( r.left()+fw, r.top()+fw, r.width()-fw-bw, r.height()-2*fw );
                return handleRTL( option, labelRect );
            }

            case SC_SpinBoxFrame:
            return sb->frame ? r : QRect();

            default:
            break;

        }

        return QCommonStyle::subControlRect( CC_SpinBox, option, subControl, widget );

   }

    //______________________________________________________________
    QSize Style::checkBoxSizeFromContents( const QStyleOption*, const QSize& contentsSize, const QWidget* ) const
    {

        //Add size for indicator
        const int indicator( CheckBox_Size );

        //Make sure we can fit the indicator
        QSize size( contentsSize );
        size.setHeight( qMax( size.height(), indicator ) );

        //Add space for the indicator and the icon
        const int spacer( CheckBox_BoxTextSpace );
        size.rwidth() += indicator + spacer;

        return size;

    }

    //______________________________________________________________
    QSize Style::comboBoxSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        QSize size = expandSize( contentsSize,
            ComboBox_ContentsMargin,
            ComboBox_ContentsMargin_Left,
            ComboBox_ContentsMargin_Top,
            ComboBox_ContentsMargin_Right,
            ComboBox_ContentsMargin_Bottom );

        // add frame width
        size = expandSize( size, ComboBox_FrameWidth );

        // Add the button width
        size.rwidth() += ComboBox_ButtonWidth;

        // TODO: For some reason the size is not right in the following configurations
        // this is still to be understood and might reveal some deeper issue.
        // notably, should compare to zhqt is done for PushButtons
        const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>( option );
        if( cb && !cb->editable && ( !cb->currentIcon.isNull() || cb->fontMetrics.height() > 13 ) ) size.rheight()+=1;

        // also expand to account for scrollbar
        size.rwidth() += StyleConfigData::scrollBarWidth() - 6;
        return size;

    }

    //______________________________________________________________
    QSize Style::headerSectionSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        const QStyleOptionHeader* headerOpt( qstyleoption_cast<const QStyleOptionHeader *>( option ) );
        if( !headerOpt ) return contentsSize;

        //TODO: check if hardcoded icon size is the right thing to do
        QSize iconSize = headerOpt->icon.isNull() ? QSize( 0,0 ) : QSize( 22,22 );
        QSize textSize = headerOpt->fontMetrics.size( 0, headerOpt->text );

        int iconSpacing = Header_TextToIconSpace;
        int w = iconSize.width() + iconSpacing + textSize.width();
        int h = qMax( iconSize.height(), textSize.height() );

        return expandSize( QSize( w, h ), Header_ContentsMargin );

    }

    //______________________________________________________________
    QSize Style::menuItemSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if( !menuItemOption ) return contentsSize;

        // First, we calculate the intrinsic size of the item.
        // this must be kept consistent with what's in drawMenuItemContol
        QSize insideSize;

        switch ( menuItemOption->menuItemType )
        {
            case QStyleOptionMenuItem::Normal:
            case QStyleOptionMenuItem::DefaultItem:
            case QStyleOptionMenuItem::SubMenu:
            {
                int iconColW = qMax( menuItemOption->maxIconWidth, ( int ) MenuItem_IconWidth );
                int leftColW = iconColW;
                if( menuItemOption->menuHasCheckableItems )
                { leftColW += MenuItem_CheckWidth + MenuItem_CheckSpace; }

                leftColW += MenuItem_IconSpace;

                int rightColW = MenuItem_ArrowSpace + MenuItem_ArrowWidth;

                QFontMetrics fm( menuItemOption->font );
                int textW;
                int tabPos = menuItemOption->text.indexOf( QLatin1Char( '\t' ) );
                if( tabPos == -1 ) textW = contentsSize.width();
                else {

                    // The width of the accelerator is not included here since
                    // Qt will add that on separately after obtaining the
                    // sizeFromContents() for each menu item in the menu to be shown
                    // ( see QMenuPrivate::calcActionRects() )
                    textW = contentsSize.width() + MenuItem_AccelSpace;

                }

                int h = qMax( contentsSize.height(), ( int ) MenuItem_MinHeight );
                insideSize = QSize( leftColW + textW + rightColW, h );
                break;
            }

            case QStyleOptionMenuItem::Separator:
            {

                // separator can have a title and an icon
                // in that case they are rendered as menubar 'title', which
                // corresponds to checked toolbuttons.
                // a rectangle identical to the one of normal items is returned.
                if( !( menuItemOption->text.isEmpty() && menuItemOption->icon.isNull() ) )
                {

                    QStyleOptionMenuItem local( *menuItemOption );
                    local.menuItemType = QStyleOptionMenuItem::Normal;
                    return menuItemSizeFromContents( &local, contentsSize, widget );

                } else {

                    insideSize = QSize( 10, 0 );
                    break;

                }

            }

            case QStyleOptionMenuItem::Scroller:
            case QStyleOptionMenuItem::TearOff:
            case QStyleOptionMenuItem::Margin:
            case QStyleOptionMenuItem::EmptyArea:
            return contentsSize;
        }

        // apply the outermost margin.
        return expandSize( insideSize, MenuItem_Margin );

    }

    //______________________________________________________________
    QSize Style::pushButtonSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* ) const
    {

        const QStyleOptionButton* bOpt = qstyleoption_cast<const QStyleOptionButton*>( option );
        if( !bOpt ) return contentsSize;

        // adjust to handle button margins
        QSize size = expandSize( contentsSize,
            PushButton_ContentsMargin,
            PushButton_ContentsMargin_Left,
            PushButton_ContentsMargin_Top,
            PushButton_ContentsMargin_Right,
            PushButton_ContentsMargin_Bottom );

        if( bOpt->features & QStyleOptionButton::HasMenu )
        { size.rwidth() += PushButton_TextToIconSpace; }

        if( !bOpt->text.isEmpty() && !bOpt->icon.isNull() )
        {

            // Incorporate the spacing between the icon and text. Qt sticks 4 there,
            // but we use PushButton::TextToIconSpace.
            size.rwidth() += PushButton_TextToIconSpace -4;
        }

        return size;

    }

    //______________________________________________________________
    QSize Style::tabBarTabSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        const QStyleOptionTab *tabOpt( qstyleoption_cast<const QStyleOptionTab*>( option ) );

        QSize size;
        const bool verticalTabs( tabOpt && isVerticalTab( tabOpt ) );
        if( verticalTabs )
        {

            size = expandSize( contentsSize,
                TabBar_TabContentsMargin,
                TabBar_TabContentsMargin_Top,
                TabBar_TabContentsMargin_Right,
                TabBar_TabContentsMargin_Bottom,
                TabBar_TabContentsMargin_Left );

        } else {

            size = expandSize( contentsSize,
                TabBar_TabContentsMargin,
                TabBar_TabContentsMargin_Left,
                TabBar_TabContentsMargin_Top,
                TabBar_TabContentsMargin_Right,
                TabBar_TabContentsMargin_Bottom );

        }

        // need to add extra size to match corner buttons
        // try cast parent for tabWidget
        const QTabWidget* tabWidget( widget ? qobject_cast<const QTabWidget*>( widget->parent() ):0 );
        if( !tabWidget ) return size;

        // try get corner widgets
        const QWidget* leftWidget( tabWidget->cornerWidget( Qt::TopLeftCorner ) );
        const QWidget* rightWidget( tabWidget->cornerWidget( Qt::TopRightCorner ) );
        QSize cornerSize;
        if( leftWidget && leftWidget->isVisible() ) cornerSize = leftWidget->minimumSizeHint();
        if( rightWidget && rightWidget->isVisible() ) cornerSize = cornerSize.expandedTo( rightWidget->minimumSizeHint() );
        if( !cornerSize.isValid() ) return size;

        // expand size
        // note: the extra pixels added to the relevant dimension are fine-tuned.
        if( verticalTabs ) size.setWidth( qMax( size.width(), cornerSize.width() + 6 ) );
        else size.setHeight( qMax( size.height(), cornerSize.height() + 4 ) );

        return size;

    }

    //______________________________________________________________
    QSize Style::toolButtonSizeFromContents( const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        QSize size = contentsSize;
        const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>( option );
        if( tbOpt && !tbOpt->icon.isNull() && !tbOpt->text.isEmpty() && tbOpt->toolButtonStyle == Qt::ToolButtonTextUnderIcon )
        { size.rheight() -= 5; }

        // We want to avoid super-skiny buttons, for things like "up" when icons + text
        // For this, we would like to make width >= height.
        // However, once we get here, QToolButton may have already put in the menu area
        // ( PM_MenuButtonIndicator ) into the width. So we may have to take it out, fix things
        // up, and add it back in. So much for class-independent rendering...
        int menuAreaWidth = 0;
        if( tbOpt )
        {

            if( tbOpt->features & QStyleOptionToolButton::MenuButtonPopup )
            {

                menuAreaWidth = pixelMetric( QStyle::PM_MenuButtonIndicator, option, widget );

            } else if( tbOpt->features & QStyleOptionToolButton::HasMenu ) {

                // TODO: something wrong here: The size is not properly accounted for
                // when drawing the slab.
                size.rwidth() += ToolButton_InlineMenuIndicatorSize;

            }

        }

        size.rwidth() -= menuAreaWidth;
        if( size.width() < size.height() ) size.setWidth( size.height() );

        size.rwidth() += menuAreaWidth;

        const QToolButton* t( qobject_cast<const QToolButton*>( widget ) );
        if( t && t->autoRaise() ) return expandSize( size, ToolButton_ContentsMargin ); // these are toolbutton margins
        else return expandSize( size,
            PushButton_ContentsMargin, 0,
            PushButton_ContentsMargin_Top, 0,
            PushButton_ContentsMargin_Bottom );

    }

    //___________________________________________________________________________________
    bool Style::drawFramePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const State& flags( option->state );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const bool enabled( flags & State_Enabled );
        const bool isInputWidget( widget && widget->testAttribute( Qt::WA_Hover ) );
        const bool hoverHighlight( enabled && isInputWidget && ( flags&State_MouseOver ) );


        //const bool focusHighlight( enabled && isInputWidget && ( flags&State_HasFocus ) );
        bool focusHighlight( false );
        if( enabled && isInputWidget && ( flags&State_HasFocus ) ) focusHighlight = true;
        else if( isKTextEditFrame( widget ) && widget->parentWidget()->hasFocus() )
        { focusHighlight = true; }

        // assume focus takes precedence over hover
        animations().lineEditEngine().updateState( widget, AnimationFocus, focusHighlight );
        animations().lineEditEngine().updateState( widget, AnimationHover, hoverHighlight && !focusHighlight );

        if( flags & State_Sunken )
        {
            const QRect local( r.adjusted( 1, 1, -1, -1 ) );
            qreal opacity( -1 );
            AnimationMode mode = AnimationNone;
            if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationFocus ) )
            {

                opacity = animations().lineEditEngine().opacity( widget, AnimationFocus  );
                mode = AnimationFocus;

            } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationHover ) ) {

                opacity = animations().lineEditEngine().opacity( widget, AnimationHover );
                mode = AnimationHover;

            }

            if( frameShadowFactory().isRegistered( widget ) )
            {

                frameShadowFactory().updateState( widget, focusHighlight, hoverHighlight, opacity, mode );

            } else {

                HoleOptions options( 0 );
                if( focusHighlight ) options |= HoleFocus;
                if( hoverHighlight ) options |= HoleHover;

                helper().renderHole(
                    painter, palette.color( QPalette::Window ), local, options,
                    opacity, mode, TileSet::Ring );

            }

        } else if( flags & State_Raised ) {

            const QRect local( r.adjusted( -1, -1, 1, 1 ) );
            renderSlab( painter, local, palette.color( QPalette::Background ), NoFill );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawFrameFocusRectPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        if( !widget ) return true;

        // focus indicators are painted only in Q3ListView and QAbstractItemView
        if( !( qobject_cast<const QAbstractItemView*>( widget ) || widget->inherits( "Q3ListView" ) ) ) return true;

        const State& flags( option->state );
        const QRect r( option->rect.adjusted( 0, 0, 0, -1 ) );
        const QPalette& palette( option->palette );

        if( r.width() < 10 ) return true;

        QLinearGradient lg( r.bottomLeft(), r.bottomRight() );

        lg.setColorAt( 0.0, Qt::transparent );
        lg.setColorAt( 1.0, Qt::transparent );
        if( flags & State_Selected )
        {

            lg.setColorAt( 0.2, palette.color( QPalette::BrightText ) );
            lg.setColorAt( 0.8, palette.color( QPalette::BrightText ) );

        } else {

            lg.setColorAt( 0.2, palette.color( QPalette::Text ) );
            lg.setColorAt( 0.8, palette.color( QPalette::Text ) );

        }

        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->setPen( QPen( lg, 1 ) );
        painter->drawLine( r.bottomLeft(), r.bottomRight() );

        return true;

    }

    //______________________________________________________________
    bool Style::drawFrameGroupBoxPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionFrame *fOpt = qstyleoption_cast<const QStyleOptionFrame *>( option );
        if( !fOpt ) return true;

        // no frame for flat groupboxes
        QStyleOptionFrameV2 fOpt2( *fOpt );
        if( fOpt2.features & QStyleOptionFrameV2::Flat ) return true;

        // normal frame
        const QPalette& palette( option->palette );
        const QRect& r( option->rect );
        const QColor base( helper().backgroundColor( palette.color( QPalette::Window ), widget, r.center() ) );

        painter->save();
        painter->setRenderHint( QPainter::Antialiasing );
        painter->setPen( Qt::NoPen );

        QLinearGradient innerGradient( 0, r.top()-r.height()+12, 0, r.bottom()+r.height()-19 );
        QColor light( helper().calcLightColor( base ) );
        light.setAlphaF( 0.4 ); innerGradient.setColorAt( 0.0, light );
        light.setAlphaF( 0.0 ); innerGradient.setColorAt( 1.0, light );
        painter->setBrush( innerGradient );
        painter->setClipRect( r.adjusted( 0, 0, 0, -19 ) );
        helper().fillSlab( *painter, r );

        painter->setClipping( false );
        helper().slope( base, 0.0 )->render( r, painter );

        painter->restore();
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawFrameMenuPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        // only draw frame for ( expanded ) toolbars
        // do nothing for other cases
        if( qobject_cast<const QToolBar*>( widget ) )
        {
            helper().renderWindowBackground( painter, option->rect, widget, option->palette );
            helper().drawFloatFrame( painter, option->rect, option->palette.window().color(), true );
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawFrameTabBarBasePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionTabBarBase* tabOpt( qstyleoption_cast<const QStyleOptionTabBarBase*>( option ) );
        if( !tabOpt ) return true;

        if( tabOpt->tabBarRect.isValid() )
        {

            // if tabBar rect is valid, all the frame is handled in tabBarTabShapeControl
            // nothing to be done here.
            // on the other hand, invalid tabBarRect corresponds to buttons in tabbars ( e.g. corner buttons ),
            // and the appropriate piece of frame needs to be drawn
            return true;

        }

        // store palette and rect
        const QPalette& palette( option->palette );
        const QRect& r( option->rect );

        QRect frameRect( r );
        SlabRect slab;
        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                frameRect.adjust( -7, -GlowWidth, 7, GlowWidth );
                frameRect.translate( 0, 4 );
                slab = SlabRect( frameRect, TileSet::Top );
                break;
            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                frameRect.adjust( -7, -GlowWidth, 7, GlowWidth );
                frameRect.translate( 0, -4 );
                slab = SlabRect( frameRect, TileSet::Bottom );
                break;
            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                frameRect.adjust( -GlowWidth, -7, GlowWidth, 7 + 1 );
                frameRect.translate( 5, 0 );
                slab = SlabRect( frameRect, TileSet::Left );
                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                frameRect.adjust( -GlowWidth, -7, GlowWidth, 7 + 1 );
                frameRect.translate( -5, 0 );
                slab = SlabRect( frameRect, TileSet::Right );
                break;
            }

            default: return true;
        }

        // render registered slabs
        renderSlab( painter, slab, palette.color( QPalette::Window ), NoFill );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawFrameTabWidgetPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionTabWidgetFrame* tabOpt( qstyleoption_cast<const QStyleOptionTabWidgetFrame*>( option ) );
        if( !tabOpt ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        /*
        no frame is drawn when tabbar is empty.
        this is consistent with the tabWidgetTabContents subelementRect
        */
        if( tabOpt->tabBarSize.isEmpty() ) return true;

        // get tabbar dimentions
        const int w( tabOpt->tabBarSize.width() );
        const int h( tabOpt->tabBarSize.height() );

        // left corner widget
        const int lw( tabOpt->leftCornerWidgetSize.width() );
        const int lh( tabOpt->leftCornerWidgetSize.height() );

        // right corner
        const int rw( tabOpt->rightCornerWidgetSize.width() );
        const int rh( tabOpt->rightCornerWidgetSize.height() );

        // list of slabs to be drawn
        SlabRectList slabs;

        // expand rect by glow width.
        QRect baseSlabRect( insideMargin( r, -GlowWidth ) );

        // render the three free sides
        switch( tabOpt->shape )
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
                    slabRect.setRight( qMax( slabRect.right() - w - lw, slabRect.left() + rw ) + 7 );
                    slabRect.setHeight( 7 );
                    slabs << SlabRect( slabRect, TileSet::TopLeft );

                    // right side
                    if( rw > 0 )
                    {
                        QRect slabRect( baseSlabRect );
                        slabRect.setLeft( slabRect.right() - rw - 7 );
                        slabRect.setHeight( 7 );
                        slabs << SlabRect( slabRect, TileSet::TopRight );
                    }

                } else {

                    // left side
                    if( lw > 0 )
                    {

                        QRect slabRect( baseSlabRect );
                        slabRect.setRight( baseSlabRect.left() + lw + 7 );
                        slabRect.setHeight( 7 );
                        slabs << SlabRect( slabRect, TileSet::TopLeft );

                    }


                    // right side
                    QRect slabRect( baseSlabRect );
                    slabRect.setLeft( qMin( slabRect.left() + w + lw + 1, slabRect.right() - rw ) -7 );
                    slabRect.setHeight( 7 );
                    slabs << SlabRect( slabRect, TileSet::TopRight );

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
                    slabRect.setRight( qMax( slabRect.right() - w - lw, slabRect.left() + rw ) + 7 );
                    slabRect.setTop( slabRect.bottom() - 7 );
                    slabs << SlabRect( slabRect, TileSet::BottomLeft );

                    // right side
                    if( rw > 0 )
                    {
                        QRect slabRect( baseSlabRect );
                        slabRect.setLeft( slabRect.right() - rw - 7 );
                        slabRect.setTop( slabRect.bottom() - 7 );
                        slabs << SlabRect( slabRect, TileSet::BottomRight );
                    }

                } else {

                    // left side
                    if( lw > 0 )
                    {

                        QRect slabRect( baseSlabRect );
                        slabRect.setRight( baseSlabRect.left() + lw + 7 );
                        slabRect.setTop( slabRect.bottom() - 7 );
                        slabs << SlabRect( slabRect, TileSet::BottomLeft );

                    }

                    // right side
                    QRect slabRect( baseSlabRect );
                    slabRect.setLeft( qMin( slabRect.left() + w + lw + 1, slabRect.right() - rw ) -7 );
                    slabRect.setTop( slabRect.bottom() - 7 );
                    slabs << SlabRect( slabRect, TileSet::BottomRight );

                }

                break;
            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                slabs << SlabRect( baseSlabRect, TileSet::Ring & ~TileSet::Left );

                // top side
                if( lh > 0 )
                {

                    QRect slabRect( baseSlabRect );
                    slabRect.setBottom( baseSlabRect.top() + lh + 7 + 1 );
                    slabRect.setWidth( 7 );
                    slabs << SlabRect( slabRect, TileSet::TopLeft );

                }

                // bottom side
                QRect slabRect( baseSlabRect );
                slabRect.setTop( qMin( slabRect.top() + h + lh, slabRect.bottom() - rh ) -7 + 1 );
                slabRect.setWidth( 7 );
                slabs << SlabRect( slabRect, TileSet::BottomLeft );

                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                slabs << SlabRect( baseSlabRect, TileSet::Ring & ~TileSet::Right );

                // top side
                if( lh > 0 )
                {

                    QRect slabRect( baseSlabRect );
                    slabRect.setBottom( baseSlabRect.top() + lh + 7 + 1 );
                    slabRect.setLeft( slabRect.right()-7 );
                    slabs << SlabRect( slabRect, TileSet::TopRight );

                }

                // bottom side
                QRect slabRect( baseSlabRect );
                slabRect.setTop( qMin( slabRect.top() + h + lh, slabRect.bottom() - rh ) -7 + 1 );
                slabRect.setLeft( slabRect.right()-7 );
                slabs << SlabRect( slabRect, TileSet::BottomRight );
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
    bool Style::drawFrameWindowPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        helper().drawFloatFrame( painter, r, palette.window().color(), false );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorArrowPrimitive( ArrowOrientation orientation, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        QRect r( option->rect );
        const QPalette& palette( option->palette );
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );

        // define gradient and polygon for drawing arrow
        const QPolygonF a = genericArrow( orientation, ArrowNormal );

        const qreal penThickness = 1.6;
        const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );

        QColor color;
        const QToolButton* toolButton( qobject_cast<const QToolButton*>( widget ) );
        if( toolButton && toolButton->arrowType() != Qt::NoArrow )
        {

            /*
            arrows painted in toolbutton need a re-centered rect,
            and have no highlight
            */

            r.translate( 1, 0 );

            // set color properly
            color = (toolButton->autoRaise() ? palette.color( QPalette::WindowText ):palette.color( QPalette::ButtonText ) );

        } else if( mouseOver ) {

            color = helper().viewHoverBrush().brush( palette ).color();

        } else {

            color = palette.color( QPalette::WindowText );

        }

        painter->translate( r.center() );
        painter->setRenderHint( QPainter::Antialiasing );

        painter->translate( 0,offset );
        const QColor background = palette.color( QPalette::Window );
        painter->setPen( QPen( helper().calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );
        painter->translate( 0,-offset );

        painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorHeaderArrowPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionHeader *headerOpt = qstyleoption_cast<const QStyleOptionHeader *>( option );
        const State& flags( option->state );

        // arrow orientation
        ArrowOrientation orientation( ArrowNone );
        if( flags&State_UpArrow || ( headerOpt && headerOpt->sortIndicator==QStyleOptionHeader::SortUp ) ) orientation = ArrowUp;
        else if( flags&State_DownArrow || ( headerOpt && headerOpt->sortIndicator==QStyleOptionHeader::SortDown ) ) orientation = ArrowDown;
        if( orientation == ArrowNone ) return true;

        // flags, rect and palette
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );

        animations().headerViewEngine().updateState( widget, r.topLeft(), mouseOver );
        const bool animated( enabled && animations().headerViewEngine().isAnimated( widget, r.topLeft() ) );

        // define gradient and polygon for drawing arrow
        const QPolygonF a = genericArrow( orientation, ArrowNormal );
        QColor color = palette.color( QPalette::WindowText );
        const QColor background = palette.color( QPalette::Window );
        const QColor highlight( helper().viewHoverBrush().brush( palette ).color() );
        const qreal penThickness = 1.6;
        const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );

        if( animated )
        {

            const qreal opacity( animations().headerViewEngine().opacity( widget, r.topLeft() ) );
            color = KColorUtils::mix( color, highlight, opacity );

        } else if( mouseOver ) color = highlight;

        painter->translate( r.center() );
        painter->translate( 0, 1 );
        painter->setRenderHint( QPainter::Antialiasing );

        painter->translate( 0,offset );
        painter->setPen( QPen( helper().calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );
        painter->translate( 0,-offset );

        painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );

        return true;
    }

    //______________________________________________________________
    bool Style::drawPanelButtonCommandPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( enabled && ( flags & State_HasFocus ) );
        const QPalette& palette( option->palette );

        StyleOptions opts = 0;
        if( flags & ( State_On|State_Sunken ) ) opts |= Sunken;
        if( flags & State_HasFocus ) opts |= Focus;
        if( enabled && ( flags & State_MouseOver ) ) opts |= Hover;

        // update animation state
        animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        // store animation state
        const bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
        const bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );
        const qreal hoverOpacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
        const qreal focusOpacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );

        // decide if widget must be rendered flat.
        /*
        The decision is made depending on
        - whether the "flat" flag is set in the option
        - whether the widget is hight enough to render both icons and normal margins
        Note: in principle one should also check for the button text height
        */

        const QRect& r( option->rect );
        const QStyleOptionButton* bOpt( qstyleoption_cast< const QStyleOptionButton* >( option ) );
        bool flat = ( bOpt && (
            bOpt->features.testFlag( QStyleOptionButton::Flat ) ||
            ( ( !bOpt->icon.isNull() ) && sizeFromContents( CT_PushButton, option, bOpt->iconSize, widget ).height() > r.height() ) ) );

        if( flat )
        {

            QRect slitRect( r );
            if( !( opts & Sunken ) )
            {
                // hover rect
                if( enabled && hoverAnimated )
                {

                    QColor glow( helper().alphaColor( helper().viewFocusBrush().brush( QPalette::Active ).color(), hoverOpacity ) );
                    helper().slitFocused( glow )->render( slitRect, painter );

                } else if( mouseOver ) {

                    helper().slitFocused( helper().viewFocusBrush().brush( QPalette::Active ).color() )->render( slitRect, painter );

                }

            } else {

                slitRect.adjust( 0, 0, 0, -1 );

                HoleOptions options( 0 );
                if( mouseOver ) options |= HoleHover;

                // flat pressed-down buttons do not get focus effect,
                // consistently with tool buttons
                if( enabled && hoverAnimated )
                {

                    helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options, hoverOpacity, AnimationHover, TileSet::Ring );

                } else {

                    helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options );

                }

            }

        } else {

            const QRect slabRect( r.adjusted( -1, 0, 1, 0 ) );

            // match color to the window background
            const QColor buttonColor( helper().backgroundColor( palette.color( QPalette::Button ), widget, r.center() ) );

            if( enabled && hoverAnimated && !( opts & Sunken ) )
            {

                renderButtonSlab( painter, slabRect, buttonColor, opts, hoverOpacity, AnimationHover, TileSet::Ring );

            } else if( enabled && !mouseOver && focusAnimated && !( opts & Sunken ) ) {

                renderButtonSlab( painter, slabRect, buttonColor, opts, focusOpacity, AnimationFocus, TileSet::Ring );

            } else {

                renderButtonSlab( painter, slabRect, buttonColor, opts );

            }

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawPanelButtonToolPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        /*
        For toolbutton in TabBars, corresponding to expanding arrows, no frame is drawn
        However one needs to draw the window background, because the button rect might
        overlap with some tab below. ( this is a Qt bug )
        */
        const bool isInTabBar( widget && qobject_cast<const QTabBar*>( widget->parent() ) );
        if( isInTabBar )
        {

            const QPalette& palette( option->palette );
            QRect r( option->rect );

            // adjust rect depending on shape
            const QTabBar* tabBar( static_cast<const QTabBar*>( widget->parent() ) );
            switch( tabBar->shape() )
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                r.setBottom( r.bottom()-6 );
                break;

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                r.setTop( r.top() + 6 );
                break;

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                r.setRight( r.right() - 6 );
                break;

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                r.setLeft( r.left() + 6 );
                break;

                default: break;

            }

            const QPalette local( widget->parentWidget() ? widget->parentWidget()->palette() : palette );

            // check whether parent has autofill background flag
            const QWidget* parent = helper().checkAutoFillBackground( widget );
            if( parent && !qobject_cast<const QDockWidget*>( parent ) ) painter->fillRect( r, parent->palette().color( parent->backgroundRole() ) );
            else helper().renderWindowBackground( painter, r, widget, local );

            return true;

        }

        const QRect& r( option->rect );
        const State& flags( option->state );
        const QPalette& palette( option->palette );

        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( enabled && ( flags & State_HasFocus ) );
        const bool reverseLayout( option->direction == Qt::RightToLeft );
        const bool autoRaised( flags & State_AutoRaise );

        // check whether toolbutton is in toolbar
        const bool isInToolBar( widget && qobject_cast<const QToolBar*>( widget->parent() ) );

        // toolbar engine
        const bool toolBarAnimated( isInToolBar && widget && ( animations().toolBarEngine().isAnimated( widget->parentWidget() ) || animations().toolBarEngine().isFollowMouseAnimated( widget->parentWidget() ) ) );
        const QRect animatedRect( ( isInToolBar && widget ) ? animations().toolBarEngine().animatedRect( widget->parentWidget() ):QRect() );
        const QRect childRect( ( widget && widget->parentWidget() ) ? r.translated( widget->mapToParent( QPoint( 0,0 ) ) ):QRect() );
        const QRect currentRect(  widget ? animations().toolBarEngine().currentRect( widget->parentWidget() ):QRect() );
        const bool current( isInToolBar && widget && widget->parentWidget() && currentRect.intersects( r.translated( widget->mapToParent( QPoint( 0,0 ) ) ) ) );
        const bool toolBarTimerActive( isInToolBar && widget && animations().toolBarEngine().isTimerActive( widget->parentWidget() ) );
        const qreal toolBarOpacity( ( isInToolBar && widget ) ? animations().toolBarEngine().opacity( widget->parentWidget() ):0 );

        // toolbutton engine
        if( isInToolBar && !toolBarAnimated )
        {

            animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );

        } else {

            // mouseOver has precedence over focus
            animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
            animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        }

        bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
        bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );

        qreal hoverOpacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
        qreal focusOpacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );

        // slit rect
        QRect slitRect( r );

        // non autoraised tool buttons get same slab as regular buttons
        if( widget && !autoRaised )
        {

            StyleOptions opts = 0;
            slitRect.adjust( -1, 0, 1, 0 );

            // "normal" parent, and non "autoraised" ( that is: always raised ) buttons
            if( flags & ( State_On|State_Sunken ) ) opts |= Sunken;
            if( flags & State_HasFocus ) opts |= Focus;
            if( enabled && ( flags & State_MouseOver ) ) opts |= Hover;

            TileSet::Tiles tiles( TileSet::Ring );

            // adjust tiles and rect in case of menubutton
            const QToolButton* t = qobject_cast<const QToolButton*>( widget );
            if( t && t->popupMode()==QToolButton::MenuButtonPopup )
            {
                if( reverseLayout )
                {

                    tiles = TileSet::Bottom | TileSet::Top | TileSet::Right;
                    slitRect.adjust( -4, 0, 0, 0 );

                } else {
                    tiles = TileSet::Bottom | TileSet::Top | TileSet::Left;
                    slitRect.adjust( 0, 0, 4, 0 );
                }
            }

            // adjust opacity and animation mode
            qreal opacity( -1 );
            AnimationMode mode( AnimationNone );
            if( enabled && hoverAnimated )
            {
                opacity = hoverOpacity;
                mode = AnimationHover;

            } else if( enabled && !hasFocus && focusAnimated ) {

                opacity = focusOpacity;
                mode = AnimationFocus;

            }

            // match button color to window background
            const QColor buttonColor( helper().backgroundColor( palette.color( QPalette::Button ), widget, r.center() ) );

            // render slab
            renderButtonSlab( painter, slitRect, buttonColor, opts, opacity, mode, tiles );

            return true;

        }

        //! fine tuning of slitRect geometry
        if( widget && widget->inherits( "QDockWidgetTitleButton" ) ) slitRect.adjust( 1, 0, 0, 0 );
        else if( widget && widget->inherits( "QToolBarExtension" ) ) slitRect.adjust( 1, 1, -1, -1 );
        else if( widget && widget->objectName() == "qt_menubar_ext_button" ) slitRect.adjust( -1, -1, 0, 0 );

        // normal ( auto-raised ) toolbuttons
        if( flags & ( State_Sunken|State_On ) )
        {

            {

                // fill hole
                qreal opacity = 1.0;
                const qreal bias = 0.75;
                if( enabled && hoverAnimated )
                {

                    opacity = 1.0 - bias*hoverOpacity;

                } else if( toolBarAnimated && enabled && animatedRect.isNull() && current  ) {

                    opacity = 1.0 - bias*toolBarOpacity;

                } else if( enabled && (( toolBarTimerActive && current ) || mouseOver ) ) {

                    opacity = 1.0 - bias;

                }

                if( opacity > 0 )
                {
                    QColor color( helper().backgroundColor( helper().calcMidColor( palette.color( QPalette::Window ) ), widget, slitRect.center() ) );
                    color = helper().alphaColor( color, opacity );
                    painter->save();
                    painter->setRenderHint( QPainter::Antialiasing );
                    painter->setPen( Qt::NoPen );
                    painter->setBrush( color );
                    painter->drawRoundedRect( slitRect.adjusted( 1, 1, -1, -1 ), 3.5, 3.5 );
                    painter->restore();
                }

            }


            HoleOptions options( HoleContrast );
            if( hasFocus && enabled ) options |= HoleFocus;
            if( mouseOver && enabled ) options |= HoleHover;

            if( enabled && hoverAnimated )
            {

                helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options, hoverOpacity, AnimationHover, TileSet::Ring );

            } else if( toolBarAnimated ) {

                if( enabled && animatedRect.isNull() && current  )
                {

                    helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options, toolBarOpacity, AnimationHover, TileSet::Ring );

                } else {

                    helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, HoleContrast );

                }

            } else if( toolBarTimerActive && current ) {

                helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options | HoleHover );

            } else {

                helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options );

            }

        } else {

            if( enabled && hoverAnimated )
            {

                QColor glow( helper().alphaColor( helper().viewFocusBrush().brush( QPalette::Active ).color(), hoverOpacity ) );
                helper().slitFocused( glow )->render( slitRect, painter );

            } else if( toolBarAnimated ) {

                if( enabled && animatedRect.isNull() && current )
                {
                    QColor glow( helper().alphaColor( helper().viewFocusBrush().brush( QPalette::Active ).color(), toolBarOpacity ) );
                    helper().slitFocused( glow )->render( slitRect, painter );
                }

            } else if( hasFocus || mouseOver || ( toolBarTimerActive && current ) ) {

                helper().slitFocused( helper().viewFocusBrush().brush( QPalette::Active ).color() )->render( slitRect, painter );

            }

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawPanelItemViewItemPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4*>( option );
        const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>( widget );
        const bool hover = ( option->state & State_MouseOver ) && ( !view || view->selectionMode() != QAbstractItemView::NoSelection );

        const bool hasCustomBackground = opt->backgroundBrush.style() != Qt::NoBrush && !( option->state & State_Selected );
        const bool hasSolidBackground = !hasCustomBackground || opt->backgroundBrush.style() == Qt::SolidPattern;

        if( !hover && !( option->state & State_Selected ) && !hasCustomBackground && !( opt->features & QStyleOptionViewItemV2::Alternate ) )
        { return true; }

        QPalette::ColorGroup cg;
        if( option->state & State_Enabled ) cg = ( option->state & State_Active ) ? QPalette::Normal : QPalette::Inactive;
        else cg = QPalette::Disabled;

        QColor color;
        if( hasCustomBackground && hasSolidBackground ) color = opt->backgroundBrush.color();
        else color = option->palette.color( cg, QPalette::Highlight );

        if( hover && !hasCustomBackground )
        {
            if( !( option->state & State_Selected ) ) color.setAlphaF( 0.2 );
            else color = color.lighter( 110 );
        }

        if( opt && ( opt->features & QStyleOptionViewItemV2::Alternate ) )
        { painter->fillRect( option->rect, option->palette.brush( cg, QPalette::AlternateBase ) ); }

        if( !hover && !( option->state & State_Selected ) && !hasCustomBackground )
        { return true; }

        if( hasCustomBackground && !hasSolidBackground )
        {

            const QPointF oldBrushOrigin = painter->brushOrigin();
            painter->setBrushOrigin( opt->rect.topLeft() );
            painter->setBrush( opt->backgroundBrush );
            painter->setPen( Qt::NoPen );
            painter->drawRect( opt->rect );
            painter->setBrushOrigin( oldBrushOrigin );

        } else {

            // get selection tileset
            QRect r = option->rect;
            TileSet *tileSet( helper().selection( color, r.height(), hasCustomBackground ) );

            bool roundedLeft  = false;
            bool roundedRight = false;
            if( opt )
            {

                roundedLeft  = ( opt->viewItemPosition == QStyleOptionViewItemV4::Beginning );
                roundedRight = ( opt->viewItemPosition == QStyleOptionViewItemV4::End );
                if( opt->viewItemPosition == QStyleOptionViewItemV4::OnlyOne ||
                    opt->viewItemPosition == QStyleOptionViewItemV4::Invalid ||
                    ( view && view->selectionBehavior() != QAbstractItemView::SelectRows ) )
                {
                    roundedLeft  = true;
                    roundedRight = true;
                }

            }

            const bool reverseLayout( option->direction == Qt::RightToLeft );

            TileSet::Tiles tiles( TileSet::Center );
            if( !reverseLayout ? roundedLeft : roundedRight ) tiles |= TileSet::Left;
            else r.adjust( -8, 0, 0, 0 );

            if( !reverseLayout ? roundedRight : roundedLeft ) tiles |= TileSet::Right;
            else r.adjust( 0, 0, 8, 0 );

            if( r.isValid() ) tileSet->render( r, painter, tiles );
        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawPanelLineEditPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        // cast option and check
        const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>( option );
        if( !panel ) return true;

        const QBrush inputBrush( palette.base() );
        const int lineWidth( panel->lineWidth );

        if( lineWidth > 0 )
        {
            painter->save();
            painter->setRenderHint( QPainter::Antialiasing );
            painter->setPen( Qt::NoPen );
            painter->setBrush( inputBrush );

            helper().fillHole( *painter, r.adjusted( 0, -1, 0, 0 ) );
            drawFramePrimitive( panel, painter, widget );

            painter->restore();

        } else  {

            painter->fillRect( r.adjusted( 2,2,-2,-2 ), inputBrush );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawPanelMenuPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // do nothing if menu is embedded in another widget
        // this corresponds to having a transparent background
        if( widget && !widget->isWindow() ) return true;

        const QStyleOptionMenuItem* mOpt( qstyleoption_cast<const QStyleOptionMenuItem*>( option ) );
        if( !( mOpt && widget ) ) return true;
        const QRect& r = mOpt->rect;
        const QColor color = mOpt->palette.color( widget->window()->backgroundRole() );

        const bool hasAlpha( helper().hasAlphaChannel( widget ) );
        if( hasAlpha )
        {

            painter->setCompositionMode( QPainter::CompositionMode_Source );
            TileSet *tileSet( helper().roundCorner( color ) );
            tileSet->render( r, painter );

            painter->setCompositionMode( QPainter::CompositionMode_SourceOver );
            painter->setClipRegion( helper().roundedMask( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

        }

        helper().renderMenuBackground( painter, r, widget, mOpt->palette );

        if( hasAlpha ) painter->setClipping( false );
        helper().drawFloatFrame( painter, r, color, !hasAlpha );

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
    bool Style::drawPanelTipLabelPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // force registration of widget
        if( widget && widget->window() )
        { shadowHelper().registerWidget( widget->window(), true ); }

        // parent style painting if frames should not be styled
        if( !StyleConfigData::toolTipDrawStyledFrames() ) return false;

        const QRect& r( option->rect );
        const QColor color( option->palette.brush( QPalette::ToolTipBase ).color() );
        QColor topColor( helper().backgroundTopColor( color ) );
        QColor bottomColor( helper().backgroundBottomColor( color ) );

        // make tooltip semi transparents when possible
        // alpha is copied from "kdebase/apps/dolphin/tooltips/filemetadatatooltip.cpp"
        const bool hasAlpha( helper().hasAlphaChannel( widget ) );
        if(  hasAlpha && StyleConfigData::toolTipTransparent() )
        {
            topColor.setAlpha( 220 );
            bottomColor.setAlpha( 220 );
        }

        QLinearGradient gr( 0, r.top(), 0, r.bottom() );
        gr.setColorAt( 0, topColor );
        gr.setColorAt( 1, bottomColor );

        // contrast pixmap
        QLinearGradient gr2( 0, r.top(), 0, r.bottom() );
        gr2.setColorAt( 0.5, helper().calcLightColor( bottomColor ) );
        gr2.setColorAt( 0.9, bottomColor );

        painter->save();

        if( hasAlpha )
        {
            painter->setRenderHint( QPainter::Antialiasing );

            QRectF local( r );
            local.adjust( 0.5, 0.5, -0.5, -0.5 );

            painter->setPen( Qt::NoPen );
            painter->setBrush( gr );
            painter->drawRoundedRect( local, 4, 4 );

            painter->setBrush( Qt::NoBrush );
            painter->setPen( QPen( gr2, 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawRoundedRect( local, 3.5, 3.5 );

        } else {

            painter->setPen( Qt::NoPen );
            painter->setBrush( gr );
            painter->drawRect( r );

            painter->setBrush( Qt::NoBrush );
            painter->setPen( QPen( gr2, 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawRect( r );

        }

        painter->restore();

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorMenuCheckMarkPrimitive( const QStyleOption *option, QPainter *painter, const QWidget * ) const
    {
        const QRect& r( option->rect );
        const State& flags( option->state );
        const QPalette& palette( option->palette );
        const bool enabled( flags & State_Enabled );

        StyleOptions opts( NoFill );
        if( !enabled ) opts |= Disabled;
        CheckBoxState state = CheckOn;

        renderCheckBox( painter, r, palette, opts, state );
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawQ3CheckListIndicatorPrimitive( const QStyleOption *option, QPainter *painter, const QWidget *widget ) const
    {
        const QStyleOptionQ3ListView* listViewOpt( qstyleoption_cast<const QStyleOptionQ3ListView*>( option ) );
        if( !listViewOpt || listViewOpt->items.isEmpty() ) return true;

        QStyleOptionButton buttonOption;
        buttonOption.QStyleOption::operator=( *option );

        QSize size( CheckBox_Size, CheckBox_Size );
        buttonOption.rect = centerRect( option->rect, size ).translated( 0, 4 );
        drawIndicatorCheckBoxPrimitive( &buttonOption, painter, widget );
        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawQ3CheckListExclusiveIndicatorPrimitive( const QStyleOption *option, QPainter *painter, const QWidget *widget ) const
    {
        const QStyleOptionQ3ListView* listViewOpt( qstyleoption_cast<const QStyleOptionQ3ListView*>( option ) );
        if( !listViewOpt || listViewOpt->items.isEmpty() ) return true;

        QStyleOptionButton buttonOption;
        buttonOption.QStyleOption::operator=( *option );

        QSize size( CheckBox_Size, CheckBox_Size );
        buttonOption.rect = centerRect( option->rect, size ).translated( 0, 4 );
        drawIndicatorRadioButtonPrimitive( &buttonOption, painter, widget );
        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorBranchPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const State& flags( option->state );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const QPoint center( r.center() );

        const bool reverseLayout( option->direction == Qt::RightToLeft );

        const int centerX = center.x();
        const int centerY = center.y();

        int expanderAdjust = 0;

        //draw expander
        if ( flags & State_Children )
        {

            int sizeLimit = qMin( qMin( r.width(), r.height() ), ( int ) Tree_MaxExpanderSize );
            const bool expanderOpen( flags & State_Open );

            // make sure size limit is odd
            if( !( sizeLimit&1 ) ) --sizeLimit;
            expanderAdjust = sizeLimit/2 + 1;

            QRect expanderRect = centerRect( r, sizeLimit, sizeLimit );
            const int radius( ( expanderRect.width() - 4 ) / 2 );

            // flags
            const bool enabled( flags & State_Enabled );
            const bool mouseOver( enabled && ( flags & State_MouseOver ) );

            // color
            const QColor expanderColor( mouseOver ? helper().viewHoverBrush().brush( palette ).color():palette.color( QPalette::Text ) );

            if( !StyleConfigData::viewDrawTriangularExpander() )
            {

                // plus or minus sign used for expanders
                painter->save();
                painter->setPen( expanderColor );
                painter->drawLine( center - QPoint( radius, 0 ), center + QPoint( radius, 0 ) );

                if( !expanderOpen )
                { painter->drawLine( center - QPoint( 0, radius ), center + QPoint( 0, radius ) ); }

                painter->restore();

            } else {

                // arrows
                painter->save();
                painter->translate( center );

                // get size from option
                QPolygonF a;
                ArrowSize size = ArrowSmall;
                qreal penThickness( 1.2 );
                qreal offset( 0.5 );

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
                    offset = 0.0;
                    size = ArrowNormal;
                    break;

                }

                if( expanderOpen )
                {

                    painter->translate( 0, offset );
                    a = genericArrow( ArrowDown, size );

                } else {

                    painter->translate( offset, 0 );
                    a = genericArrow( reverseLayout ? ArrowLeft:ArrowRight, size );

                }

                painter->setPen( QPen( expanderColor, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                painter->setRenderHint( QPainter::Antialiasing );
                painter->drawPolyline( a );
                painter->restore();
            }

        }


        // tree branches
        if( !StyleConfigData::viewDrawTreeBranchLines() ) return true;

        painter->setPen( KColorUtils::mix( palette.color( QPalette::Text ), palette.color( QPalette::Background ), 0.8 ) );
        if ( flags & ( State_Item | State_Children | State_Sibling ) )
        {
            const QLine line( QPoint( centerX, r.top() ), QPoint( centerX, centerY - expanderAdjust ) );
            painter->drawLine( line );
        }

        //The right/left ( depending on dir ) line gets drawn if we have an item
        if ( flags & State_Item )
        {
            const QLine line = reverseLayout ?
                QLine( QPoint( r.left(), centerY ), QPoint( centerX - expanderAdjust, centerY ) ):
                QLine( QPoint( centerX + expanderAdjust, centerY ), QPoint( r.right(), centerY ) );
            painter->drawLine( line );

        }

        //The bottom if we have a sibling
        if ( flags & State_Sibling )
        {
            const QLine line( QPoint( centerX, centerY + expanderAdjust ), QPoint( centerX, r.bottom() ) );
            painter->drawLine( line );
        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorButtonDropDownPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QPalette& palette( option->palette );
        const QRect& r( option->rect );
        const State& flags( option->state );

        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool reverseLayout( option->direction == Qt::RightToLeft );
        const bool autoRaise( flags & State_AutoRaise );
        const bool sunken( enabled && ( flags & State_Sunken ) );

        // match button color to window background
        const QColor highlight( helper().viewHoverBrush().brush( palette ).color() );
        QColor color = palette.color( autoRaise ? QPalette::WindowText:QPalette::ButtonText );
        QColor background = palette.color( QPalette::Window );
        StyleOptions opts = 0;

        // define gradient and polygon for drawing arrow
        QPolygonF a = genericArrow( ArrowDown, ArrowNormal );

        qreal penThickness = 1.6;

        // toolbuttons
        const QToolButton *tool( qobject_cast<const QToolButton *>( widget ) );
        if( tool && tool->popupMode()==QToolButton::MenuButtonPopup )
        {

            if( !autoRaise )
            {

                const bool hasFocus( enabled && ( flags & State_HasFocus ) );

                // handle animations
                // mouseOver has precedence over focus
                animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
                animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

                const bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
                const bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );

                const qreal hoverOpacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
                const qreal focusOpacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );

                color = palette.color( QPalette::ButtonText );
                background = helper().backgroundColor( palette.color( QPalette::Button ), widget, r.center() );

                if( hasFocus ) opts |= Focus;
                if( mouseOver ) opts |= Hover;

                // adjust opacity and animation mode
                qreal opacity( -1 );
                AnimationMode mode( AnimationNone );
                if( enabled && hoverAnimated )
                {

                    opacity = hoverOpacity;
                    mode = AnimationHover;

                } else if( enabled && !hasFocus && focusAnimated ) {

                    opacity = focusOpacity;
                    mode = AnimationFocus;

                }

                // paint frame
                painter->save();
                if( reverseLayout )
                {

                    QRect frameRect( r.adjusted( 0, 0, 10, 0 ) );
                    if( flags & ( State_On|State_Sunken ) ) opts |= Sunken;

                    painter->setClipRect( frameRect.adjusted( 0, 0, -8, 0 ), Qt::IntersectClip );
                    renderButtonSlab( painter, frameRect, background, opts, opacity, mode, TileSet::Bottom | TileSet::Top | TileSet::Left );

                } else {


                    QRect frameRect( r.adjusted( -10,0,0,0 ) );
                    if( flags & ( State_On|State_Sunken ) ) opts |= Sunken;

                    painter->setClipRect( frameRect.adjusted( 8, 0, 0, 0 ), Qt::IntersectClip );
                    renderButtonSlab( painter, frameRect, background, opts, opacity, mode, TileSet::Bottom | TileSet::Top | TileSet::Right );

                }

                painter->restore();

                // draw separating vertical line
                const QColor color( palette.color( QPalette::Button ) );
                QColor light =helper().alphaColor( helper().calcLightColor( color ), 0.6 );
                QColor dark = helper().calcDarkColor( color );
                dark.setAlpha( 200 );

                int yTop( r.top()+2 );
                if( sunken ) yTop += 1;

                const int yBottom( r.bottom()-4 );
                painter->setPen( QPen( light,1 ) );

                if( reverseLayout )
                {

                    painter->drawLine( r.right()+5, yTop+1, r.right()+5, yBottom );
                    painter->drawLine( r.right()+3, yTop+2, r.right()+3, yBottom );
                    painter->setPen( QPen( dark,1 ) );
                    painter->drawLine( r.right()+4, yTop, r.right()+4, yBottom );

                    a.translate( 3, 1 );

                } else {

                    painter->drawLine( r.x()-5, yTop+1, r.x()-5, yBottom-1 );
                    painter->drawLine( r.x()-3, yTop+1, r.x()-3, yBottom-1 );
                    painter->setPen( QPen( dark,1 ) );
                    painter->drawLine( r.x()-4, yTop, r.x()-4, yBottom );

                    a.translate( -3,1 );

                }

            } else if( const QStyleOptionToolButton *tbOption = qstyleoption_cast<const QStyleOptionToolButton *>( option ) ) {

                // handle arrow over animation
                const bool arrowHover( enabled && mouseOver && ( tbOption->activeSubControls & SC_ToolButtonMenu ) );
                animations().toolButtonEngine().updateState( widget, AnimationHover, arrowHover );

                const bool animated( enabled && animations().toolButtonEngine().isAnimated( widget, AnimationHover ) );
                const qreal opacity( animations().toolButtonEngine().opacity( widget, AnimationHover ) );

                if( animated ) color = KColorUtils::mix( color, highlight, opacity );
                else if( arrowHover ) color = highlight;
                else color = palette.color( autoRaise ? QPalette::WindowText:QPalette::ButtonText );

            }

        } else {

            color = palette.color( autoRaise ? QPalette::WindowText:QPalette::ButtonText );

            // smaller down arrow for menu indication on toolbuttons
            penThickness = 1.4;
            a = genericArrow( ArrowDown, ArrowSmall );

        }

        painter->translate( r.center() );
        painter->setRenderHint( QPainter::Antialiasing );

        // white reflection
        const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );
        painter->translate( 0,offset );
        painter->setPen( QPen( helper().calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );
        painter->translate( 0,-offset );

        painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );

        return true;

    }
    //___________________________________________________________________________________
    bool Style::drawIndicatorCheckBoxPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // get rect
        const QRect& r( option->rect );
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( flags & State_HasFocus );

        StyleOptions opts( 0 );
        if( !enabled ) opts |= Disabled;
        if( mouseOver ) opts |= Hover;
        if( hasFocus ) opts |= Focus;

        // get checkbox state
        CheckBoxState state;
        if( flags & State_NoChange ) state = CheckTriState;
        else if( flags & State_On ) state = CheckOn;
        else state = CheckOff;

        // match button color to window background
        QPalette palette( option->palette );
        palette.setColor(
            QPalette::Button,
            helper().backgroundColor(
            palette.color( QPalette::Button ), widget, r.center() ) );

        // mouseOver has precedence over focus
        animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

        if( enabled && animations().widgetStateEngine().isAnimated( widget, AnimationHover ) )
        {

            const qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
            renderCheckBox( painter, r, palette, opts, state, opacity, AnimationHover );

        } else if( enabled && !hasFocus && animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) ) {

            const qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );
            renderCheckBox( painter, r, palette, opts, state, opacity, AnimationFocus );

        } else renderCheckBox( painter, r, palette, opts, state );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorRadioButtonPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // get rect
        const QRect& r( option->rect );
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( flags & State_HasFocus );

        StyleOptions opts( 0 );
        if( !enabled ) opts |= Disabled;
        if( mouseOver ) opts |= Hover;
        if( hasFocus ) opts |= Focus;

        // match button color to window background
        QPalette palette( option->palette );
        palette.setColor( QPalette::Button, helper().backgroundColor( palette.color( QPalette::Button ), widget, r.center() ) );

        // mouseOver has precedence over focus
        animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        const CheckBoxState state( ( flags & State_On ) ? CheckOn:CheckOff );
        if( enabled && animations().widgetStateEngine().isAnimated( widget, AnimationHover ) )
        {

            const qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
            renderRadioButton( painter, r, palette, opts, state, opacity, AnimationHover );

        } else if(  enabled && animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) ) {

            const qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );
            renderRadioButton( painter, r, palette, opts, state, opacity, AnimationFocus );

        } else renderRadioButton( painter, r, palette, opts, state );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorTabTearPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionTab* tabOpt( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOpt ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // in fact with current version of Qt ( 4.6.0 ) the cast fails and document mode is always false
        // this will hopefully be fixed in later versions
        const QStyleOptionTabV3* tabOptV3( qstyleoption_cast<const QStyleOptionTabV3*>( option ) );
        bool documentMode( tabOptV3 ? tabOptV3->documentMode : false );

        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : NULL;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        QRect gradientRect( r );
        switch( tabOpt->shape )
        {

            case QTabBar::TriangularNorth:
            case QTabBar::RoundedNorth:
            gradientRect.adjust( 0, 0, 0, -5 );
            if( !reverseLayout ) gradientRect.translate( -GlowWidth,0 );
            break;

            case QTabBar::TriangularSouth:
            case QTabBar::RoundedSouth:
            gradientRect.adjust( 0, 5, 0, 0 );
            if( !reverseLayout ) gradientRect.translate( -GlowWidth,0 );
            break;

            case QTabBar::TriangularWest:
            case QTabBar::RoundedWest:
            gradientRect.adjust( 0, 0, -5, 0 );
            gradientRect.translate( 0,-GlowWidth );
            break;

            case QTabBar::TriangularEast:
            case QTabBar::RoundedEast:
            gradientRect.adjust( 5, 0, 0, 0 );
            gradientRect.translate( 0,-GlowWidth );
            break;

            default: return true;
        }

        // fade tabbar
        QPixmap pm( gradientRect.size() );
        pm.fill( Qt::transparent );
        QPainter pp( &pm );

        const bool verticalTabs( isVerticalTab( tabOpt ) );

        int w = 0, h = 0;
        if( verticalTabs ) h = gradientRect.height();
        else w = gradientRect.width();

        QLinearGradient grad;
        if( reverseLayout && !verticalTabs ) grad = QLinearGradient( 0, 0, w, h );
        else grad = QLinearGradient( w, h, 0, 0 );

        grad.setColorAt( 0, Qt::transparent );
        grad.setColorAt( 0.6, Qt::black );

        helper().renderWindowBackground( &pp, pm.rect(), widget, palette );
        pp.setCompositionMode( QPainter::CompositionMode_DestinationAtop );
        pp.fillRect( pm.rect(), QBrush( grad ) );
        pp.end();

        // draw pixmap
        painter->drawPixmap( gradientRect.topLeft()+QPoint( 0,-1 ),pm );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorToolBarHandlePrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {
        const State& flags( option->state );
        const bool horizontal( flags & State_Horizontal );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        int counter( 1 );

        if( horizontal )
        {

            const int center( r.left()+r.width()/2 );
            for( int j = r.top()+2; j <= r.bottom()-3; j+=3, ++counter )
            {
                if( counter%2 == 0 ) helper().renderDot( painter, QPoint( center+1, j ), palette.color( QPalette::Background ) );
                else helper().renderDot( painter, QPoint( center-2, j ), palette.color( QPalette::Background ) );
            }

        } else {

            const int center( r.top()+r.height()/2 );
            for( int j = r.left()+2; j <= r.right()-3; j+=3, ++counter )
            {
                if( counter%2 == 0 ) helper().renderDot( painter, QPoint( j, center+1 ), palette.color( QPalette::Background ) );
                else helper().renderDot( painter, QPoint( j, center-2 ), palette.color( QPalette::Background ) );
            }
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawIndicatorToolBarSeparatorPrimitive( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const State& flags( option->state );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        if( StyleConfigData::toolBarDrawItemSeparator() )
        {
            const QColor color( palette.color( QPalette::Window ) );
            if( flags & State_Horizontal ) helper().drawSeparator( painter, r, color, Qt::Vertical );
            else helper().drawSeparator( painter, r, color, Qt::Horizontal );
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

        helper().renderWindowBackground( painter, option->rect, widget, palette );
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawCapacityBarControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option
        const QStyleOptionProgressBar* cbOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
        if( !cbOption ) return true;

        // draw container
        QStyleOptionProgressBarV2 sub_opt( *cbOption );
        sub_opt.rect = subElementRect( QStyle::SE_ProgressBarGroove, cbOption, widget );
        drawProgressBarGrooveControl( &sub_opt, painter, widget );

        // draw bar
        sub_opt.rect = subElementRect( QStyle::SE_ProgressBarContents, cbOption, widget );
        drawProgressBarContentsControl( &sub_opt, painter, widget );

        // draw label
        sub_opt.rect = subElementRect( QStyle::SE_ProgressBarLabel, cbOption, widget );
        drawProgressBarLabelControl( &sub_opt, painter, widget );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawComboBoxLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        //same as CommonStyle, except for filling behind icon
        if( const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>( option ) )
        {

            QRect editRect( subControlRect( CC_ComboBox, cb, SC_ComboBoxEditField, widget ) );

            painter->save();
            if( !cb->currentIcon.isNull() )
            {

                QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                QPixmap pixmap( cb->currentIcon.pixmap( cb->iconSize, mode ) );
                QRect iconRect( editRect );
                iconRect.setWidth( cb->iconSize.width() + 4 );
                iconRect = alignedRect(
                    cb->direction,
                    Qt::AlignLeft | Qt::AlignVCenter,
                    iconRect.size(), editRect );

                drawItemPixmap( painter, iconRect, Qt::AlignCenter, pixmap );

                if( cb->direction == Qt::RightToLeft ) editRect.translate( -4 - cb->iconSize.width(), 0 );
                else editRect.translate( cb->iconSize.width() + 4, 0 );
            }

            if( !cb->currentText.isEmpty() && !cb->editable )
            {
                const bool& hasFrame( cb->frame );
                const QPalette::ColorRole role( hasFrame ? QPalette::ButtonText : QPalette::WindowText );
                drawItemText(
                    painter, editRect.adjusted( 1, 0, -1, 0 ),
                    visualAlignment( cb->direction, Qt::AlignLeft | Qt::AlignVCenter ),
                    cb->palette, cb->state & State_Enabled, cb->currentText, role );
            }
            painter->restore();
            return true;

        } else return false;

    }

    //___________________________________________________________________________________
    bool Style::drawDockWidgetTitleControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionDockWidget* dwOpt = ::qstyleoption_cast<const QStyleOptionDockWidget*>( option );
        if ( !dwOpt ) return true;

        const QPalette& palette( option->palette );
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // cast to v2 to check vertical bar
        const QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>( option );
        const bool verticalTitleBar( v2 ? v2->verticalTitleBar : false );

        const QRect btnr( subElementRect( dwOpt->floatable ? SE_DockWidgetFloatButton : SE_DockWidgetCloseButton, option, widget ) );

        // get rectangle and adjust to properly accounts for buttons
        QRect r( insideMargin( dwOpt->rect, DockWidget_TitleMargin ) );
        if( verticalTitleBar )
        {

            if( btnr.isValid() ) r.setTop( btnr.bottom()+1 );

        } else if( reverseLayout ) {

            if( btnr.isValid() ) r.setLeft( btnr.right()+1 );
            r.adjust( 0,0,-4,0 );

        } else {

            if( btnr.isValid() ) r.setRight( btnr.left()-1 );
            r.adjust( 4,0,0,0 );

        }

        QString title( dwOpt->title );
        QString tmpTitle = title;

        // this is quite suboptimal
        // and does not really work
        if( tmpTitle.contains( "&" ) )
        {
            int pos = tmpTitle.indexOf( "&" );
            if( !( tmpTitle.size()-1 > pos && tmpTitle.at( pos+1 ) == QChar( '&' ) ) ) tmpTitle.remove( pos, 1 );

        }

        int tw = dwOpt->fontMetrics.width( tmpTitle );
        int width = verticalTitleBar ? r.height() : r.width();
        if( width < tw ) title = dwOpt->fontMetrics.elidedText( title, Qt::ElideRight, width, Qt::TextShowMnemonic );

        if( verticalTitleBar )
        {

            QSize s = r.size();
            s.transpose();
            r.setSize( s );

            painter->save();
            painter->translate( r.left(), r.top() + r.width() );
            painter->rotate( -90 );
            painter->translate( -r.left(), -r.top() );
            drawItemText( painter, r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette, enabled, title, QPalette::WindowText );
            painter->restore();


        } else {

            drawItemText( painter, r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, palette, enabled, title, QPalette::WindowText );

        }

        return true;


    }

    //___________________________________________________________________________________
    bool Style::drawHeaderEmptyAreaControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // use the same background as in drawHeaderPrimitive
        QPalette palette( option->palette );

        if( widget && animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
        { palette = helper().mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }

        const bool horizontal( option->state & QStyle::State_Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );
        renderHeaderBackground( option->rect, palette, painter, widget, horizontal, reverseLayout );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawHeaderLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {
        const QStyleOptionHeader* headerOpt( qstyleoption_cast<const QStyleOptionHeader *>( option ) );
        if( !headerOpt ) return true;

        QRect rect( headerOpt->rect );

        if ( !headerOpt->icon.isNull() )
        {
            const QPixmap pixmap( headerOpt->icon.pixmap(
                pixelMetric( PM_SmallIconSize ),
                ( headerOpt->state & State_Enabled ) ? QIcon::Normal : QIcon::Disabled ) );

            int pixw = pixmap.width();

            QRect aligned = alignedRect( headerOpt->direction, QFlag( headerOpt->iconAlignment ), pixmap.size(), rect );
            QRect inter = aligned.intersected( rect );
            painter->drawPixmap( inter.x(), inter.y(), pixmap, inter.x() - aligned.x(), inter.y() - aligned.y(), inter.width(), inter.height() );

            if ( headerOpt->direction == Qt::LeftToRight ) rect.setLeft( rect.left() + pixw + 2 );
            else rect.setRight( rect.right() - pixw - 2 );

        }

        drawItemText(
            painter, rect, headerOpt->textAlignment, headerOpt->palette,
            ( headerOpt->state & State_Enabled ), headerOpt->text, QPalette::WindowText );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawHeaderSectionControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const QStyleOptionHeader* headerOpt( qstyleoption_cast<const QStyleOptionHeader *>( option ) );
        if( !headerOpt ) return true;

        const bool horizontal( headerOpt->orientation == Qt::Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );
        const bool isFirst( horizontal && ( headerOpt->position == QStyleOptionHeader::Beginning ) );
        const bool isCorner( widget && widget->inherits( "QTableCornerButton" ) );

        // corner header lines
        if( isCorner )
        {

            if( widget ) helper().renderWindowBackground( painter, r, widget, palette );
            else painter->fillRect( r, palette.color( QPalette::Window ) );
            if( reverseLayout ) renderHeaderLines( r, palette, painter, TileSet::BottomLeft );
            else renderHeaderLines( r, palette, painter, TileSet::BottomRight );

        } else renderHeaderBackground( r, palette, painter, widget, horizontal, reverseLayout );

        // dots
        const QColor color( palette.color( QPalette::Window ) );
        if( horizontal )
        {

            if( headerOpt->section != 0 || isFirst )
            {
                const int center( r.center().y() );
                const int pos( reverseLayout ? r.left()+1 : r.right()-1 );
                helper().renderDot( painter, QPoint( pos, center-3 ), color );
                helper().renderDot( painter, QPoint( pos, center ), color );
                helper().renderDot( painter, QPoint( pos, center+3 ), color );
            }

        } else {

            const int center( r.center().x() );
            const int pos( r.bottom()-1 );
            helper().renderDot( painter, QPoint( center-3, pos ), color );
            helper().renderDot( painter, QPoint( center, pos ), color );
            helper().renderDot( painter, QPoint( center+3, pos ), color );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawMenuBarItemControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionMenuItem* menuOpt = ::qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if ( !menuOpt ) return true;

        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        if( enabled )
        {
            const bool active( flags & State_Selected );
            const bool animated( animations().menuBarEngine().isAnimated( widget, r.topLeft() ) );
            const qreal opacity( animations().menuBarEngine().opacity( widget, r.topLeft() ) );
            const QRect currentRect( animations().menuBarEngine().currentRect( widget, r.topLeft() ) );
            const QRect animatedRect( animations().menuBarEngine().animatedRect( widget ) );

            const bool intersected( animatedRect.intersects( r ) );
            const bool current( currentRect.contains( r.topLeft() ) );
            const bool timerIsActive( animations().menuBarEngine().isTimerActive( widget ) );

            // do nothing in case of empty intersection between animated rect and current
            if( ( intersected || !animated || animatedRect.isNull() ) && ( active || animated || timerIsActive ) )
            {

                QColor color( helper().calcMidColor( palette.color( QPalette::Window ) ) );
                if( StyleConfigData::menuHighlightMode() != StyleConfigData::MM_DARK )
                {

                    if( flags & State_Sunken )
                    {

                        if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG ) color = palette.color( QPalette::Highlight );
                        else color = KColorUtils::mix( color, KColorUtils::tint( color, palette.color( QPalette::Highlight ), 0.6 ) );

                    } else {

                        if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG ) color = KColorUtils::tint( color, helper().viewHoverBrush().brush( palette ).color() );
                        else color = KColorUtils::mix( color, KColorUtils::tint( color, helper().viewHoverBrush().brush( palette ).color() ) );
                    }

                } else color = helper().backgroundColor( color, widget, r.center() );

                // drawing
                if( animated && intersected )
                {

                    helper().holeFlat( color, 0.0 )->render( animatedRect.adjusted( 1,1,-1,-1 ), painter, TileSet::Full );

                } else if( timerIsActive && current ) {

                    helper().holeFlat( color, 0.0 )->render( r.adjusted( 1,1,-1,-1 ), painter, TileSet::Full );

                } else if( animated && current ) {

                    color.setAlphaF( opacity );
                    helper().holeFlat( color, 0.0 )->render( r.adjusted( 1,1,-1,-1 ), painter, TileSet::Full );

                } else if( active ) {

                    helper().holeFlat( color, 0.0 )->render( r.adjusted( 1,1,-1,-1 ), painter, TileSet::Full );

                }

            }

        }

        // text
        QPalette::ColorRole role( QPalette::WindowText );
        if( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG && ( flags & State_Sunken ) && enabled )
        { role = QPalette::HighlightedText; }

        drawItemText( painter, r, Qt::AlignCenter | Qt::TextShowMnemonic, palette, enabled, menuOpt->text, role );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawMenuItemControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const State& flags( option->state );
        const bool active( flags & State_Selected );
        const bool enabled( flags & State_Enabled );
        const bool hasFocus( enabled && ( flags & State_HasFocus ) );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );

        //First of all,render the background.
        renderMenuItemBackground( option, painter, widget );

        // do nothing if invalid option, or empty area
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
        if( !menuItemOption || menuItemOption->menuItemType == QStyleOptionMenuItem::EmptyArea ) return true;

        //First, figure out the left column width.
        const int iconColW = qMax( menuItemOption->maxIconWidth, ( int )MenuItem_IconWidth );
        const int checkColW = MenuItem_CheckWidth;
        const int checkSpace = MenuItem_CheckSpace;

        int leftColW = iconColW;

        // only use the additional check row if the menu has checkable menuItems.
        bool hasCheckableItems = menuItemOption->menuHasCheckableItems;
        if( hasCheckableItems ) leftColW += checkColW + checkSpace;

        // right arrow column...
        int rightColW = MenuItem_ArrowSpace + MenuItem_ArrowWidth;

        //Separators: done with the bg, can paint them and bail them out.
        if( menuItemOption->menuItemType == QStyleOptionMenuItem::Separator )
        {
            // check text and icon
            // separators with non empty text are rendered as checked toolbuttons
            if( !menuItemOption->text.isEmpty() )
            {

                QStyleOptionToolButton toolButtonOpt;
                toolButtonOpt.features = QStyleOptionToolButton::None;
                toolButtonOpt.state = State_On|State_Sunken|State_Enabled;
                toolButtonOpt.rect = r.adjusted( 0, 0, 0, 1 );
                toolButtonOpt.subControls = SC_ToolButton;
                toolButtonOpt.icon =  menuItemOption->icon;

                toolButtonOpt.font = widget->font();
                toolButtonOpt.font.setBold( true );

                toolButtonOpt.iconSize = QSize(
                    pixelMetric( QStyle::PM_SmallIconSize,0,0 ),
                    pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );

                // for now menu size is not calculated properly
                // ( meaning it doesn't account for titled separators width
                // as a fallback, we elide the text to be displayed
                int width( r.width() );
                if( !menuItemOption->icon.isNull() )
                { width -= toolButtonOpt.iconSize.width() + 2; }
                width -= 2*ToolButton_ContentsMargin;
                toolButtonOpt.text = QFontMetrics( toolButtonOpt.font ).elidedText( menuItemOption->text, Qt::ElideRight, width );

                toolButtonOpt.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
                drawToolButtonComplexControl( &toolButtonOpt, painter, widget );
                return true;

            } else {

                // in all other cases draw regular separator
                const QColor color( helper().menuBackgroundColor( palette.color( QPalette::Window ), widget, r.center() ) );
                helper().drawSeparator( painter, r, color, Qt::Horizontal );
                return true;

            }

        }

        //Remove the margin ( for everything but the column background )
        const QRect ir( insideMargin( r, MenuItem_Margin ) );

        //Active indicator...
        if( active && enabled )
        {

            // check if there is a 'sliding' animation in progress, in which case, do nothing
            const QRect animatedRect( animations().menuEngine().animatedRect( widget ) );
            if( animatedRect.isNull() )
            {

                const bool animated( animations().menuEngine().isAnimated( widget, Current ) );
                const QRect currentRect( animations().menuEngine().currentRect( widget, Current ) );
                const bool intersected( currentRect.contains( r.topLeft() ) );

                const QColor color( helper().menuBackgroundColor( helper().calcMidColor( palette.color( QPalette::Window ) ), widget, r.center() ) );

                if( animated && intersected ) renderMenuItemRect( option, r, color, palette, painter, animations().menuEngine().opacity( widget, Current ) );
                else renderMenuItemRect( option, r, color, palette, painter );

            }

        }

        // color
        QPalette::ColorRole textRole( ( active && enabled && StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG ) ?
            QPalette::HighlightedText:
            QPalette::WindowText );

        //Readjust the column rectangle back to proper height
        QRect leftColRect( ir.x(), ir.y(), leftColW, ir.height() );

        // paint a normal check- resp. radiomark.
        const QRect checkColRect(
            leftColRect.x(), leftColRect.y(),
            checkColW, leftColRect.height() );

        const CheckBoxState checkBoxState( menuItemOption->checked ? CheckOn:CheckOff );
        if( menuItemOption->checkType == QStyleOptionMenuItem::NonExclusive )
        {

            StyleOptions opts( 0 );
            opts |= Sunken;
            if( !enabled ) opts |= Disabled;
            if( mouseOver ) opts |= Hover;
            if( hasFocus ) opts |= Focus;

            const QRect r( handleRTL( option, checkColRect ) );
            QPalette localPalette( palette );
            localPalette.setColor( QPalette::Window, helper().menuBackgroundColor( palette.color( QPalette::Window ), widget, r.topLeft() ) );
            renderCheckBox( painter, r.adjusted( 2,-2,2,2 ), localPalette, opts, checkBoxState );

        } else if( menuItemOption->checkType == QStyleOptionMenuItem::Exclusive ) {

            StyleOptions opts( 0 );
            if( !enabled ) opts |= Disabled;
            if( mouseOver ) opts |= Hover;
            if( hasFocus ) opts |= Focus;

            const QRect r( handleRTL( option, checkColRect ) );
            QPalette localPalette( palette );
            localPalette.setColor( QPalette::Window, helper().menuBackgroundColor( palette.color( QPalette::Window ), widget, r.topLeft() ) );
            renderRadioButton( painter, r.adjusted( 2,-2,2,2 ), localPalette, opts, checkBoxState );

        }

        // Paint the menu icon.
        if( !menuItemOption->icon.isNull() )
        {

            QRect iconColRect;

            if( hasCheckableItems )
            {

                iconColRect = QRect(
                    leftColRect.x()+checkColW+checkSpace, leftColRect.y(),
                    leftColRect.width()-( checkColW+checkSpace ), leftColRect.height() );

            } else iconColRect = leftColRect;

            // icon mode
            QIcon::Mode mode;
            if( enabled ) mode = active ? QIcon::Active: QIcon::Normal;
            else mode = QIcon::Disabled;

            // icon state
            const QIcon::State iconState(
                ( ( flags & State_On ) || ( flags & State_Sunken ) ) ?
                QIcon::On:QIcon::Off );

            // icon size
            const QSize size( pixelMetric( PM_SmallIconSize ), pixelMetric( PM_SmallIconSize ) );
            const QRect r( handleRTL( option, centerRect( iconColRect, size ) ) );
            const QPixmap icon = menuItemOption->icon.pixmap( size, mode, iconState );
            painter->drawPixmap( centerRect( r, size ), icon );

        }


        //Now include the spacing when calculating the next columns
        leftColW += MenuItem_IconSpace;

        //Render the text, including any accel.
        QString text = menuItemOption->text;
        const QRect textRect( handleRTL( option, QRect( ir.x() + leftColW, ir.y(), ir.width() - leftColW - rightColW, ir.height() ) ) );

        painter->setFont( menuItemOption->font );

        int tabPos = menuItemOption->text.indexOf( QLatin1Char( '\t' ) );
        if( tabPos != -1 )
        {

            text = menuItemOption->text.left( tabPos );
            QString accl = menuItemOption->text.mid( tabPos + 1 );

            drawItemText(
                painter, textRect, Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::AlignRight, palette,
                enabled, accl, textRole );

        }

        //Draw the text.
        drawItemText(
            painter, textRect, Qt::AlignVCenter | Qt::TextShowMnemonic, palette,
            enabled, text, textRole );

        //Render arrow, if need be.
        if( menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu )
        {

            const qreal penThickness = 1.6;
            const QColor color = palette.color( textRole );
            const QColor background = palette.color( QPalette::Window );

            const int aw = MenuItem_ArrowWidth;
            QRect arrowRect = handleRTL( option, QRect( ir.x() + ir.width() - aw, ir.y(), aw, ir.height() ) );

            // get arrow shape
            QPolygonF a = genericArrow( option->direction == Qt::LeftToRight ? ArrowRight : ArrowLeft, ArrowNormal );

            painter->translate( arrowRect.center() );
            painter->setRenderHint( QPainter::Antialiasing );

            // white reflection
            const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );
            painter->translate( 0,offset );
            painter->setPen( QPen( helper().calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( a );
            painter->translate( 0,-offset );

            painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( a );


        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        if( const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>( option ) )
        {

            // same as QCommonStyle::drawControl, except that it handles animations
            QStyleOptionProgressBarV2 subopt = *pb;
            subopt.rect = subElementRect( SE_ProgressBarGroove, pb, widget );
            drawProgressBarGrooveControl( &subopt, painter, widget );

            if( animations().progressBarEngine().busyIndicatorEnabled() && pb->maximum == 0 && pb->minimum == 0 )
            { animations().progressBarEngine().startBusyTimer(); }

            if( animations().progressBarEngine().isAnimated( widget ) )
            { subopt.progress = animations().progressBarEngine().value( widget ); }

            subopt.rect = subElementRect( SE_ProgressBarContents, &subopt, widget );
            drawProgressBarContentsControl( &subopt, painter, widget );

            if( pb->textVisible )
            {
                subopt.rect = subElementRect( SE_ProgressBarLabel, pb, widget );
                drawProgressBarLabelControl( &subopt, painter, widget );
            }

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarContentsControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionProgressBar* pbOpt = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        if ( !pbOpt ) return true;

        const QStyleOptionProgressBarV2* pbOpt2 = qstyleoption_cast<const QStyleOptionProgressBarV2*>( option );

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        // check if anything is to be drawn
        qreal progress = pbOpt->progress - pbOpt->minimum;
        const bool busyIndicator = ( pbOpt->minimum == 0 && pbOpt->maximum == 0 );
        if( busyIndicator && widget )
        {
            // load busy value from widget property
            QVariant busyValue( widget->property( ProgressBarEngine::busyValuePropertyName ) );
            if( busyValue.isValid() ) progress = busyValue.toReal();
        }

        if( !( progress || busyIndicator ) ) return true;

        const int steps = qMax( pbOpt->maximum  - pbOpt->minimum, 1 );
        const bool horizontal = !pbOpt2 || pbOpt2->orientation == Qt::Horizontal;

        //Calculate width fraction
        qreal widthFrac( busyIndicator ?  ProgressBar_BusyIndicatorSize/100.0 : progress/steps );
        widthFrac = qMin( (qreal)1.0, widthFrac );

        // And now the pixel width
        const int indicatorSize( widthFrac*( horizontal ? r.width():r.height() ) );

        // do nothing if indicator size is too small
        if( indicatorSize < 4 ) return true;

        QRect indicatorRect;
        if ( busyIndicator )
        {

            // The space around which we move around...
            int remSize = ( ( 1.0 - widthFrac )*( horizontal ? r.width():r.height() ) );
            remSize = qMax( remSize, 1 );

            int pstep =  int( progress )%( 2*remSize );
            if ( pstep > remSize )
            { pstep = -( pstep - 2*remSize ); }

            if ( horizontal ) indicatorRect = QRect( r.x() + pstep, r.y(), indicatorSize, r.height() );
            else indicatorRect = QRect( r.x(), r.y() + pstep, r.width(), indicatorSize );

        } else {

            if ( horizontal ) indicatorRect = QRect( r.x(), r.y(), indicatorSize, r.height() );
            else indicatorRect = QRect( r.x(), r.bottom()- indicatorSize + 1, r.width(), indicatorSize );

        }

        // handle right to left
        indicatorRect = handleRTL( option, indicatorRect );

        // make sure rect is large enough
        /* this account for adjustments done here and in StyleHelper::progressBarIndicator */
        if( indicatorRect.adjusted( 2, 1, -2, -1 ).isValid() )
        {
            indicatorRect.adjust( 1, 0, -1, -1 );
            QPixmap pixmap( helper().progressBarIndicator( palette, indicatorRect ) );
            painter->drawPixmap( indicatorRect.topLeft(), pixmap );
        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarGrooveControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const QStyleOptionProgressBarV2 *pbOpt = qstyleoption_cast<const QStyleOptionProgressBarV2 *>( option );
        const Qt::Orientation orientation( pbOpt? pbOpt->orientation : Qt::Horizontal );

        // ajust rect for alignment
        QRect rect( option->rect );
        if( orientation == Qt::Horizontal ) rect.adjust( 1, 0, -1, 0 );
        else rect.adjust( 0, 1, 0, -1 );

        renderScrollBarHole( painter, rect, option->palette.color( QPalette::Window ), orientation );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawProgressBarLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {
        const QStyleOptionProgressBar* pbOpt = qstyleoption_cast<const QStyleOptionProgressBar*>( option );
        if( !pbOpt ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const State& flags( option->state );
        const bool enabled( flags&State_Enabled );

        const QStyleOptionProgressBarV2* pbOpt2 = qstyleoption_cast<const QStyleOptionProgressBarV2*>( option );
        const bool horizontal = !pbOpt2 || pbOpt2->orientation == Qt::Horizontal;
        const bool reverseLayout = ( option->direction == Qt::RightToLeft );

        // rotate label for vertical layout
        if( ! ( horizontal || reverseLayout ) )
        {

            painter->translate( r.topRight() );
            painter->rotate( 90.0 );

        } else if( !horizontal ) {

            painter->translate( r.bottomLeft() );
            painter->rotate( -90.0 );

        }

        Qt::Alignment hAlign( ( pbOpt->textAlignment == Qt::AlignLeft ) ? Qt::AlignHCenter : pbOpt->textAlignment );

        /*
        Figure out the geometry of the indicator.
        This is copied from drawProgressBarContentsControl
        */
        QRect progressRect;
        const QRect textRect( horizontal? r : QRect( 0, 0, r.height(), r.width() ) );
        const qreal progress = pbOpt->progress - pbOpt->minimum;
        const int steps = qMax( pbOpt->maximum  - pbOpt->minimum, 1 );
        const bool busyIndicator = ( steps <= 1 );

        int indicatorSize( 0 );
        if( !busyIndicator )
        {
            const qreal widthFrac = qMin( (qreal)1.0, progress / steps );
            indicatorSize = widthFrac*( horizontal ? r.width() : r.height() );
        }

        if( indicatorSize )
        {
            if ( horizontal ) painter->setClipRect( handleRTL( option, QRect( r.x(), r.y(), indicatorSize, r.height() ) ) );
            else if ( !reverseLayout )  painter->setClipRect( QRect( r.height()-indicatorSize, 0, r.height(), r.width() ) );
            else painter->setClipRect( QRect( 0, 0, indicatorSize, r.width() ) );

            // first pass ( highlighted )
            drawItemText( painter, textRect, Qt::AlignVCenter | hAlign, palette, enabled, pbOpt->text, QPalette::HighlightedText );

            // second pass ( normal )
            if( horizontal ) painter->setClipRect( handleRTL( option, QRect( r.x() + indicatorSize, r.y(), r.width() - indicatorSize, r.height() ) ) );
            else if( !reverseLayout ) painter->setClipRect( QRect( 0, 0, r.height() - indicatorSize, r.width() ) );
            else painter->setClipRect( QRect( indicatorSize, 0, r.height()- indicatorSize, r.width() ) );
            drawItemText( painter, textRect, Qt::AlignVCenter | hAlign, palette, enabled, pbOpt->text, QPalette::WindowText );

        } else {

            drawItemText( painter, textRect, Qt::AlignVCenter | hAlign, palette, enabled, pbOpt->text, QPalette::WindowText );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawPushButtonLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionButton* bOpt = qstyleoption_cast<const QStyleOptionButton*>( option );
        if ( !bOpt ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const State& flags( option->state );
        const bool active( ( flags & State_On ) || ( flags & State_Sunken ) );
        const bool enabled( flags & State_Enabled );
        const bool hasFocus( flags & State_HasFocus );
        const bool flat( bOpt->features.testFlag( QStyleOptionButton::Flat ) );

        //Extract out coordinates for easier manipulation
        int x, y, w, h;
        r.getRect( &x, &y, &w, &h );

        //Layout the stuff.
        if ( bOpt->features & QStyleOptionButton::HasMenu )
        {

            const int indicatorWidth( PushButton_MenuIndicatorSize );
            const int indicatorSpacing = PushButton_TextToIconSpace;
            w -= indicatorWidth + indicatorSpacing;

            // arrow
            const QRect arrowRect( x + w + indicatorSpacing, y+1, indicatorWidth, h );
            const qreal penThickness = 1.6;
            QPolygonF a = genericArrow( ArrowDown, ArrowNormal );

            const QColor color = palette.color( flat ? QPalette::WindowText:QPalette::ButtonText );
            const QColor background = palette.color( flat ? QPalette::Window:QPalette::Button );

            painter->save();
            painter->translate( arrowRect.center() );
            painter->setRenderHint( QPainter::Antialiasing );

            const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );
            painter->translate( 0,offset );
            painter->setPen( QPen( helper().calcLightColor(  background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( a );
            painter->translate( 0,-offset );

            painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( a );
            painter->restore();

        }

        // Draw the icon if there is one
        if( !bOpt->icon.isNull() )
        {

            if ( !bOpt->text.isEmpty() )
            {

                const int margin = PushButton_TextToIconSpace;
                const int length = bOpt->iconSize.width() + margin + painter->fontMetrics().size( Qt::TextShowMnemonic, bOpt->text ).width();

                //Calculate offset.
                const int offset = ( w - length )/2;

                const QRect iconRect( handleRTL( bOpt, QRect( QPoint( x + offset, y + h/2 - bOpt->iconSize.height()/2 ), bOpt->iconSize ) ) );

                QIcon::Mode mode;
                if( enabled ) mode = ( hasFocus ) ? QIcon::Active: QIcon::Normal;
                else mode = QIcon::Disabled;

                QIcon::State iconState = active ? QIcon::On : QIcon::Off;

                QSize size = bOpt->iconSize;
                if( !size.isValid() ) size = QSize( pixelMetric( PM_SmallIconSize ), pixelMetric( PM_SmallIconSize ) );
                QPixmap icon = bOpt->icon.pixmap( size, mode, iconState );
                painter->drawPixmap( centerRect( iconRect, icon.size() ), icon );

                //new bounding rect for the text
                x += offset + bOpt->iconSize.width() + margin;
                w =  length - bOpt->iconSize.width() - margin;

            } else {

                const QRect iconRect( x, y, w, h );
                QIcon::Mode mode;
                if( enabled ) mode = ( hasFocus ) ? QIcon::Active: QIcon::Normal;
                else mode = QIcon::Disabled;

                QIcon::State iconState = active ? QIcon::On : QIcon::Off;

                QSize size = bOpt->iconSize;
                if( !size.isValid() ) size = QSize( pixelMetric( PM_SmallIconSize ), pixelMetric( PM_SmallIconSize ) );
                QPixmap icon = bOpt->icon.pixmap( size, mode, iconState );
                painter->drawPixmap( centerRect( iconRect, icon.size() ), icon );

            }

        } else {

            //Center the text
            int textW = painter->fontMetrics().size( Qt::TextShowMnemonic, bOpt->text ).width();
            x += ( w - textW )/2;
            w =  textW;

        }

        QRect textRect( handleRTL( bOpt, QRect( x, y, w, h ) ) );
        if( !bOpt->icon.isNull() ) textRect.adjust( 0, 0, 0, 1 );

        const QPalette::ColorRole role( flat ? QPalette::WindowText : QPalette::ButtonText );
        drawItemText( painter, textRect, Qt::AlignCenter | Qt::TextShowMnemonic, palette, enabled, bOpt->text, role );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawRubberBandControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        if( const QStyleOptionRubberBand *rbOpt = qstyleoption_cast<const QStyleOptionRubberBand *>( option ) )
        {

            painter->save();
            QColor color = rbOpt->palette.color( QPalette::Highlight );
            painter->setPen( KColorUtils::mix( color, rbOpt->palette.color( QPalette::Active, QPalette::WindowText ) ) );
            color.setAlpha( 50 );
            painter->setBrush( color );
            painter->setClipRegion( rbOpt->rect );
            painter->drawRect( rbOpt->rect.adjusted( 0,0,-1,-1 ) );
            painter->restore();
            return true;

        } else return false;

    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarSliderControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>( option );
        if( !slider ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const State& flags( option->state );
        const bool horizontal( flags & State_Horizontal );
        const bool enabled( flags&State_Enabled );
        const bool mouseOver( enabled && ( flags&State_MouseOver ) );

        // enable animation state
        animations().scrollBarEngine().updateState( widget, enabled && slider && ( slider->activeSubControls & SC_ScrollBarSlider ) );

        const bool animated( enabled && animations().scrollBarEngine().isAnimated( widget, SC_ScrollBarSlider ) );

        if( horizontal )
        {
            if( animated ) renderScrollBarHandle( painter, r, palette, Qt::Horizontal, mouseOver, animations().scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
            else renderScrollBarHandle( painter, r, palette, Qt::Horizontal, mouseOver );

        } else {

            if( animated ) renderScrollBarHandle( painter, r, palette, Qt::Vertical, mouseOver, animations().scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
            else renderScrollBarHandle( painter, r, palette, Qt::Vertical, mouseOver );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarAddLineControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSlider* slOpt = qstyleoption_cast<const QStyleOptionSlider*>( option );
        if ( !slOpt ) return true;

        const State& flags( option->state );
        const bool horizontal( flags & State_Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // colors
        const QPalette& palette( option->palette );
        const QColor background( palette.color( QPalette::Window ) );

        // adjust rect, based on number of buttons to be drawn
        const QRect r( scrollBarInternalSubControlRect( slOpt, SC_ScrollBarAddLine ) );

        // draw the end of the scrollbar groove
        if( horizontal )
        {

            if( reverseLayout ) renderScrollBarHole( painter, QRect( r.right()+1, r.top(), 5, r.height() ), background, Qt::Horizontal, TileSet::Vertical | TileSet::Left );
            else renderScrollBarHole( painter, QRect( r.left()-5, r.top(), 5, r.height() ), background, Qt::Horizontal, TileSet::Vertical | TileSet::Right );

        } else renderScrollBarHole( painter, QRect( r.left(), r.top()-5, r.width(), 5 ), background, Qt::Vertical, TileSet::Bottom | TileSet::Horizontal );

        // stop here if no buttons are defined
        if( _addLineButtons == NoButton ) return true;

        QColor color;
        QStyleOption localOption( *option );
        if( _addLineButtons == DoubleButton )
        {

            if( horizontal )
            {

                //Draw the arrows
                const QSize halfSize( r.width()/2, r.height() );
                const QRect leftSubButton( r.topLeft(), halfSize );
                const QRect rightSubButton( leftSubButton.topRight() + QPoint( 1, 0 ), halfSize );

                localOption.rect = leftSubButton;
                color = scrollBarArrowColor( &localOption,  reverseLayout ? SC_ScrollBarAddLine:SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, leftSubButton, color, background, ArrowLeft );

                localOption.rect = rightSubButton;
                color = scrollBarArrowColor( &localOption,  reverseLayout ? SC_ScrollBarSubLine:SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, rightSubButton, color, background, ArrowRight );

            } else {

                const QSize halfSize( r.width(), r.height()/2 );
                const QRect topSubButton( r.topLeft(), halfSize );
                const QRect botSubButton( topSubButton.bottomLeft() + QPoint( 0, 1 ), halfSize );

                localOption.rect = topSubButton;
                color = scrollBarArrowColor( &localOption, SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, topSubButton, color, background, ArrowUp );

                localOption.rect = botSubButton;
                color = scrollBarArrowColor( &localOption, SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, botSubButton, color, background, ArrowDown );

            }

        } else if( _addLineButtons == SingleButton ) {

            localOption.rect = r;
            color = scrollBarArrowColor( &localOption,  SC_ScrollBarAddLine, widget );
            if( horizontal ) renderScrollBarArrow( painter, r, color, background, reverseLayout ? ArrowLeft : ArrowRight );
            else renderScrollBarArrow( painter, r, color, background, ArrowDown );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarAddPageControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionSlider* slOpt = qstyleoption_cast<const QStyleOptionSlider*>( option );
        if ( !slOpt ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const QColor color( palette.color( QPalette::Window ) );

        const State& flags( option->state );
        const bool horizontal( flags & State_Horizontal );
        const bool reverseLayout( slOpt->direction == Qt::RightToLeft );

        if( horizontal )
        {
            if( reverseLayout ) renderScrollBarHole( painter, r.adjusted( 0,0,10,0 ), color, Qt::Horizontal, TileSet::Vertical );
            else renderScrollBarHole( painter, r.adjusted( -10, 0,0,0 ), color, Qt::Horizontal, TileSet::Vertical );
        } else renderScrollBarHole( painter, r.adjusted( 0,-10,0,0 ), color, Qt::Vertical, TileSet::Horizontal );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarSubLineControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionSlider* slOpt = qstyleoption_cast<const QStyleOptionSlider*>( option );
        if ( !slOpt ) return true;

        const State& flags( option->state );
        const bool horizontal( flags & State_Horizontal );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // colors
        const QPalette& palette( option->palette );
        const QColor background( palette.color( QPalette::Window ) );


        // adjust rect, based on number of buttons to be drawn
        QRect r( scrollBarInternalSubControlRect( slOpt, SC_ScrollBarSubLine ) );

        // draw the end of the scrollbar groove
        if( horizontal )
        {

            if( reverseLayout ) renderScrollBarHole( painter, QRect( r.left()-5, r.top(), 5, r.height() ), background, Qt::Horizontal, TileSet::Vertical | TileSet::Right );
            else renderScrollBarHole( painter, QRect( r.right()+1, r.top(), 5, r.height() ), background, Qt::Horizontal, TileSet::Vertical | TileSet::Left );

            r.translate( 1, 0 );

        } else {

            renderScrollBarHole( painter, QRect( r.left(), r.bottom()+3, r.width(), 5 ), background, Qt::Vertical, TileSet::Top | TileSet::Horizontal );
            r.translate( 0, 2 );

        }

        // stop here if no buttons are defined
        if( _subLineButtons == NoButton ) return true;

        QColor color;
        QStyleOption localOption( *option );
        if( _subLineButtons == DoubleButton )
        {

            if( horizontal )
            {

                //Draw the arrows
                const QSize halfSize( r.width()/2, r.height() );
                const QRect leftSubButton( r.topLeft(), halfSize );
                const QRect rightSubButton( leftSubButton.topRight() + QPoint( 1, 0 ), halfSize );

                localOption.rect = leftSubButton;
                color = scrollBarArrowColor( &localOption,  reverseLayout ? SC_ScrollBarAddLine:SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, leftSubButton, color, background, ArrowLeft );

                localOption.rect = rightSubButton;
                color = scrollBarArrowColor( &localOption,  reverseLayout ? SC_ScrollBarSubLine:SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, rightSubButton, color, background, ArrowRight );

            } else {

                const QSize halfSize( r.width(), r.height()/2 );
                const QRect topSubButton( r.topLeft(), halfSize );
                const QRect botSubButton( topSubButton.bottomLeft() + QPoint( 0, 1 ), halfSize );

                localOption.rect = topSubButton;
                color = scrollBarArrowColor( &localOption, SC_ScrollBarSubLine, widget );
                renderScrollBarArrow( painter, topSubButton, color, background, ArrowUp );

                localOption.rect = botSubButton;
                color = scrollBarArrowColor( &localOption, SC_ScrollBarAddLine, widget );
                renderScrollBarArrow( painter, botSubButton, color, background, ArrowDown );

            }

        } else if( _subLineButtons == SingleButton ) {

            localOption.rect = r;
            color = scrollBarArrowColor( &localOption,  SC_ScrollBarSubLine, widget );
            if( horizontal ) renderScrollBarArrow( painter, r, color, background, reverseLayout ? ArrowRight : ArrowLeft );
            else renderScrollBarArrow( painter, r, color, background, ArrowUp );

        }

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawScrollBarSubPageControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        // cast option and check
        const QStyleOptionSlider* slOpt = qstyleoption_cast<const QStyleOptionSlider*>( option );
        if ( !slOpt ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const QColor color( palette.color( QPalette::Window ) );

        const State& flags( option->state );
        const bool horizontal( flags & State_Horizontal );
        const bool reverseLayout( slOpt->direction == Qt::RightToLeft );

        if( horizontal )
        {
            if( reverseLayout ) renderScrollBarHole( painter, r.adjusted( -10, 0,0,0 ), color, Qt::Horizontal, TileSet::Vertical );
            else renderScrollBarHole( painter, r.adjusted( 0,0,10,0 ), color, Qt::Horizontal, TileSet::Vertical );
        } else renderScrollBarHole( painter, r.adjusted( 0,2,0,12 ), color, Qt::Vertical, TileSet::Horizontal );

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawShapedFrameControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionFrameV3* frameOpt = qstyleoption_cast<const QStyleOptionFrameV3*>( option );
        if( !frameOpt ) return false;

        switch( frameOpt->frameShape )
        {

            case QFrame::Box:
            {
                if( option->state & State_Sunken ) return true;
                else break;
            }

            case QFrame::HLine:
            {
                const QColor color( helper().backgroundColor( option->palette.color( QPalette::Window ), widget, option->rect.center() ) );
                helper().drawSeparator( painter, option->rect, color, Qt::Horizontal );
                return true;
            }

            case QFrame::VLine:
            {
                const QColor color( helper().backgroundColor( option->palette.color( QPalette::Window ), widget, option->rect.center() ) );
                helper().drawSeparator( painter, option->rect, color, Qt::Vertical );
                return true;
            }

            default: break;

        }

        return false;

    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* ) const
    {

        const QStyleOptionTab *tabOpt = qstyleoption_cast< const QStyleOptionTab* >( option );
        if( !tabOpt ) return true;

        // add extra offset for selected tas
        QStyleOptionTabV3 tabOptV3( *tabOpt );

        const bool selected( option->state&State_Selected );

        // get rect
        QRect r( option->rect );

        // handle selection and orientation
        /*
        painter is rotated and translated to deal with various orientations
        rect is translated to 0,0, and possibly transposed
        */
        switch( tabOptV3.shape )
        {


            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                if( selected ) r.translate( 0, -1 );
                painter->translate( r.topLeft() );
                r.moveTopLeft( QPoint( 0,0 ) );
                break;

            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                if( selected ) r.translate( 0, 1 );
                painter->translate( r.topLeft() );
                r.moveTopLeft( QPoint( 0,0 ) );
                break;

            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {

                if( selected ) r.translate( -1, 0 );
                painter->translate( r.bottomLeft() );
                painter->rotate( -90 );
                r = QRect( QPoint( 0, 0 ), QSize( r.height(), r.width() ) );
                break;

            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {

                if( selected ) r.translate( 1, 0 );
                painter->translate( r.topRight() );
                painter->rotate( 90 );
                r = QRect( QPoint( 0, 0 ), QSize( r.height(), r.width() ) );
                break;

            }

            default: break;

        }

        // make room for left and right widgets
        // left widget
        const bool verticalTabs( isVerticalTab( tabOpt ) );
        const bool hasLeftButton( !( option->direction == Qt::RightToLeft ? tabOptV3.rightButtonSize.isEmpty():tabOptV3.leftButtonSize.isEmpty() ) );
        const bool hasRightButton( !( option->direction == Qt::RightToLeft ? tabOptV3.leftButtonSize.isEmpty():tabOptV3.rightButtonSize.isEmpty() ) );

        if( hasLeftButton )
        { r.setLeft( r.left() + 4 + ( verticalTabs ? tabOptV3.leftButtonSize.height() : tabOptV3.leftButtonSize.width() ) ); }

        // make room for left and right widgets
        // left widget
        if( hasRightButton )
        { r.setRight( r.right() - 4 - ( verticalTabs ? tabOptV3.rightButtonSize.height() : tabOptV3.rightButtonSize.width() ) ); }

        // compute textRect and iconRect
        // now that orientation is properly dealt with, everything is handled as a 'north' orientation
        QRect textRect;
        QRect iconRect;

        if( tabOptV3.icon.isNull() )
        {

            textRect = r.adjusted( 6, 0, -6, 0 );

        } else {

            const QSize& iconSize( tabOptV3.iconSize );
            iconRect = centerRect( r, iconSize );
            if( !tabOptV3.text.isEmpty() )
            {

                iconRect.moveLeft( r.left() + 8 );
                textRect = r;
                textRect.setLeft( iconRect.right()+3 );
                textRect.setRight( r.right() - 6 );
            }

        }

        if( !verticalTabs )
        {
            textRect = visualRect(option->direction, r, textRect );
            iconRect = visualRect(option->direction, r, iconRect );
        }

        // render icon
        if( !iconRect.isNull() )
        {

            // not sure why this is necessary
            if( tabOptV3.shape == QTabBar::RoundedNorth || tabOptV3.shape == QTabBar::TriangularNorth )
            { iconRect.translate( 0, -1 ); }

            const QPixmap tabIcon = tabOptV3.icon.pixmap(
                tabOptV3.iconSize,
                ( tabOptV3.state & State_Enabled ) ? QIcon::Normal : QIcon::Disabled,
                ( tabOptV3.state & State_Selected ) ? QIcon::On : QIcon::Off );

            painter->drawPixmap( iconRect.x(), iconRect.y(), tabIcon );
        }

        // render text
        if( !textRect.isNull() )
        {

            const QPalette& palette( option->palette );
            const QString& text( tabOptV3.text );
            const bool enabled( option->state & State_Enabled );
            const int alignment( Qt::AlignCenter|Qt::TextShowMnemonic );
            drawItemText( painter, textRect, alignment, palette, enabled, text, QPalette::WindowText );

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabShapeControl_Single( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionTab* tabOpt( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOpt ) return true;

        const State& flags( option->state );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const bool enabled( flags & State_Enabled );
        const bool selected( flags&State_Selected );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // this is needed to complete the base frame when there are widgets in tabbar
        const QTabBar* tabBar( qobject_cast<const QTabBar*>( widget ) );
        const QRect tabBarRect( tabBar ? insideMargin( tabBar->rect(), -GlowWidth ):QRect() );

        // check if tab is being dragged
        const bool isDragged( selected && painter->device() != tabBar );

        // hover and animation flags
        /* all are disabled when tabBar is locked ( drag in progress ) */
        const bool tabBarLocked( tabBarData().locks( tabBar ) );
        const bool mouseOver( enabled && !tabBarLocked && ( flags & State_MouseOver ) );

        // animation state
        animations().tabBarEngine().updateState( widget, r.topLeft(), mouseOver );
        const bool animated( enabled && !selected && !tabBarLocked && animations().tabBarEngine().isAnimated( widget, r.topLeft() ) );

        // handle base frame painting, for tabbars in which tab is being dragged
        tabBarData().drawTabBarBaseControl( tabOpt, painter, widget );
        if( selected && tabBar && isDragged ) tabBarData().lock( tabBar );
        else if( selected  && tabBarData().locks( tabBar ) ) tabBarData().release();

        // tab position and flags
        const QStyleOptionTab::TabPosition& position = tabOpt->position;
        const bool isFirst( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::Beginning );
        const bool isRightOfSelected( tabOpt->selectedPosition == QStyleOptionTab::PreviousIsSelected );

        // document mode
        const QStyleOptionTabV3 *tabOptV3 = qstyleoption_cast<const QStyleOptionTabV3 *>( option );
        bool documentMode = tabOptV3 ? tabOptV3->documentMode : false;
        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : NULL;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        const bool verticalTabs( isVerticalTab( tabOpt ) );
        const QRect tabWidgetRect( tabWidget ?
            insideMargin( tabWidget->rect(), -GlowWidth ).translated( -widget->geometry().topLeft() ) :
            QRect() );

        // corner widgets
        const bool hasLeftCornerWidget( tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget );
        const bool hasRightCornerWidget( tabOpt->cornerWidgets & QStyleOptionTab::RightCornerWidget );

        // true if widget is aligned to the frame
        /* need to check for 'isRightOfSelected' because for some reason the isFirst flag is set when active tab is being moved */
        const bool isFrameAligned( !documentMode && isFirst && !hasLeftCornerWidget && !isRightOfSelected && !isDragged );
        bool fillBackground( selected && isDragged );

        bool isLeftFrameAligned( false );
        bool isRightFrameAligned( false );
        if( verticalTabs )
        {

            /*
            for vertical tabs:
            1/ leftFrameAligned corresponds to top side
            2/ rightFrameAligned corresponds to the bottom side
            3/ their value does not depend on reverseLayout
            */
            isLeftFrameAligned = isFrameAligned;
            isRightFrameAligned = (!documentMode) && tabWidget && ( r.bottom() >= tabWidgetRect.bottom() - 2 );;
            fillBackground |= selected && isRightFrameAligned;

        } else if( reverseLayout ) {

            isLeftFrameAligned = (!documentMode) && tabWidget && ( r.left() <= tabWidgetRect.left() + 2 );
            isRightFrameAligned = isFrameAligned;
            fillBackground |= selected && isLeftFrameAligned;

        } else {

            isLeftFrameAligned = isFrameAligned;
            isRightFrameAligned = (!documentMode) && tabWidget && ( r.right() >= tabWidgetRect.right() - 2 );
            fillBackground |= selected && isRightFrameAligned;

        }

        // part of the tab in which the text is drawn
        QRect tabRect( r );

        // connection to the frame
        SlabRectList slabs;

        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {

                // part of the tab in which the text is drawn
                // larger tabs when selected
                if( selected ) tabRect.adjust( 0, -1, 0, 2 );
                else tabRect.adjust( 0, 1, 0, 2 );

                // reduces the space between tabs
                tabRect.adjust( -GlowWidth,0,GlowWidth,0 );

                // connection to the main frame
                if( selected )
                {

                    // do nothing if dragged
                    if( isDragged ) break;

                    // left side
                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - GlowWidth );
                        frameRect.setRight( tabRect.left() + 7 );
                        frameRect.setTop( tabRect.bottom() - 13 );
                        frameRect.setBottom( frameRect.bottom() + 7 - 1 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 );
                        frameRect.setRight( tabRect.left() + 7 + 3 );
                        frameRect.setTop( r.bottom() - 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );
                    }

                    // right side
                    if( isRightFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 );
                        frameRect.setRight( frameRect.right() + GlowWidth );
                        frameRect.setTop( tabRect.bottom() - 13 );
                        frameRect.setBottom( frameRect.bottom() + 7 - 1 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 - 3 );
                        frameRect.setRight( frameRect.right() + 7 );
                        frameRect.setTop( r.bottom() - 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    }

                    // extra base, to extend below inactive tabs and buttons
                    if( tabBar )
                    {
                        if( r.left() > tabBarRect.left() + 1 )
                        {
                            QRect frameRect( r );
                            frameRect.setLeft( tabBarRect.left() - 7 + 1 );
                            frameRect.setRight( r.left() + 7 - 1 );
                            frameRect.setTop( r.bottom() - 7 );
                            if( documentMode || reverseLayout ) slabs << SlabRect( frameRect, TileSet::Top );
                            else slabs << SlabRect( frameRect, TileSet::TopLeft );

                        }

                        if( r.right() < tabBarRect.right() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setLeft( r.right() - 7 + 1 );
                            frameRect.setRight( tabBarRect.right() + 7 - 1 );
                            frameRect.setTop( r.bottom() - 7 );
                            if( documentMode || !reverseLayout ) slabs << SlabRect( frameRect, TileSet::Top );
                            else slabs << SlabRect( frameRect, TileSet::TopRight );
                        }
                    }

                } else {

                    if( isRightFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 );
                        frameRect.setRight( frameRect.right() + GlowWidth );
                        frameRect.setTop( tabRect.bottom() - 13 );
                        frameRect.setBottom( frameRect.bottom() + 7 - 2 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    }

                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - GlowWidth );
                        frameRect.setRight( tabRect.left() + 7 );
                        frameRect.setTop( tabRect.bottom() - 13 );
                        frameRect.setBottom( frameRect.bottom() + 7 - 2 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    }

                }

                break;

            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {

                // larger tabs when selected
                if( selected ) tabRect.adjust( 0, -2, 0, 1 );
                else tabRect.adjust( 0, -2, 0, -1 );

                // reduces the space between tabs
                tabRect.adjust( -GlowWidth,0,GlowWidth,0 );

                // connection to the main frame
                if( selected )
                {

                    // do nothing if dragged
                    if( isDragged ) break;

                    // left side
                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - GlowWidth );
                        frameRect.setRight( tabRect.left() + 7 );
                        frameRect.setTop( frameRect.top() - 7 + 1 );
                        frameRect.setBottom( tabRect.top() + 13 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 );
                        frameRect.setRight( tabRect.left() + 7 + 3 );
                        frameRect.setBottom( r.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Bottom );
                    }

                    // right side
                    if( isRightFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 );
                        frameRect.setRight( frameRect.right() + GlowWidth );
                        frameRect.setTop( frameRect.top() - 7 + 1 );
                        frameRect.setBottom( tabRect.top() + 13 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 - 3 );
                        frameRect.setRight( frameRect.right() + 7 );
                        frameRect.setBottom( r.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Bottom );

                    }

                    // extra base, to extend below tabbar buttons
                    if( tabBar )
                    {
                        if( r.left() > tabBarRect.left() + 1 )
                        {
                            QRect frameRect( r );
                            frameRect.setLeft( tabBarRect.left() - 7 + 1 );
                            frameRect.setRight( r.left() + 7 - 1 );
                            frameRect.setBottom( r.top() + 7 );
                            if( documentMode || reverseLayout ) slabs << SlabRect( frameRect, TileSet::Bottom );
                            else slabs << SlabRect( frameRect, TileSet::BottomLeft );
                        }

                        if( r.right() < tabBarRect.right() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setLeft( r.right() - 7 + 1 );
                            frameRect.setRight( tabBarRect.right() + 7 - 1 );
                            frameRect.setBottom( r.top() + 7 );
                            if( documentMode || !reverseLayout ) slabs << SlabRect( frameRect, TileSet::Bottom );
                            else slabs << SlabRect( frameRect, TileSet::BottomRight );
                        }

                    }

                } else {

                    if( isRightFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 );
                        frameRect.setRight( frameRect.right() + GlowWidth );
                        frameRect.setTop( frameRect.top() - 7 + 2 );
                        frameRect.setBottom( tabRect.top() + 13 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    }

                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - GlowWidth );
                        frameRect.setRight( tabRect.left() + 7 );
                        frameRect.setTop( frameRect.top() - 7 + 2 );
                        frameRect.setBottom( tabRect.top() + 13 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    }

                }

                break;

            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {

                // larger tabs when selected
                if( selected ) tabRect.adjust( -1, 0, 2, 0 );
                else tabRect.adjust( 1, 0, 2, 0 );

                // reduces the space between tabs
                tabRect.adjust( 0, -GlowWidth,0,GlowWidth );

                // connection to the main frame
                if( selected )
                {

                    // do nothing if dragged
                    if( isDragged ) break;

                    // top side
                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 13 );
                        frameRect.setRight( frameRect.right() + 7 - 1 );
                        frameRect.setTop( frameRect.top() - GlowWidth );
                        frameRect.setBottom( tabRect.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( r.right() - 7 );
                        frameRect.setTop( frameRect.top() - 7 );
                        frameRect.setBottom( tabRect.top() + 7 + 3 );
                        slabs << SlabRect( frameRect, TileSet::Left );
                    }

                    // bottom side
                    if( isRightFrameAligned )
                    {

                        // FIXME:
                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 13 );
                        frameRect.setRight( frameRect.right() + 7 - 1 );
                        frameRect.setTop( tabRect.bottom() - 7 );
                        frameRect.setBottom( frameRect.bottom() + GlowWidth );
                        slabs << SlabRect( frameRect, TileSet::Bottom );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( r.right() - 7 );
                        frameRect.setTop( tabRect.bottom() - 7 - 3 );
                        frameRect.setBottom( frameRect.bottom() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    }

                    // extra base, to extend below tabbar buttons
                    if( tabBar )
                    {
                        if( r.top() > tabBarRect.top() + 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( tabBarRect.top() - 7 + 1 );
                            frameRect.setBottom( r.top() + 7 - 1 );
                            frameRect.setLeft( r.right() - 7 );
                            if( documentMode ) slabs << SlabRect( frameRect, TileSet::Left );
                            else slabs << SlabRect( frameRect, TileSet::TopLeft );
                        }

                        if( r.bottom() < tabBarRect.bottom() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( r.bottom() - 7 + 1 );
                            if( hasRightCornerWidget && documentMode ) frameRect.setBottom( tabBarRect.bottom() + 7 - 1 );
                            else frameRect.setBottom( tabBarRect.bottom() + 7 );
                            frameRect.setLeft( r.right() - 7 );
                            slabs << SlabRect( frameRect, TileSet::Left );

                        }
                    }

                } else {

                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 13 );
                        frameRect.setRight( frameRect.right() + 7 - 2 );
                        frameRect.setTop( frameRect.top() - GlowWidth );
                        frameRect.setBottom( tabRect.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    }

                    if( isRightFrameAligned )
                    {

                        // FIXME:
                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 13 );
                        frameRect.setRight( frameRect.right() + 7 - 2 );
                        frameRect.setTop( tabRect.bottom() - 7 );
                        frameRect.setBottom( frameRect.bottom() + GlowWidth );
                        slabs << SlabRect( frameRect, TileSet::Bottom );

                    }
                }

                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {

                // larger tabs when selected
                if( selected ) tabRect.adjust( -2, 0, 1, 0 );
                else tabRect.adjust( -2, 0, -1, 0 );

                // reduces the space between tabs
                tabRect.adjust( 0, -GlowWidth,0,GlowWidth );

                // connection to the main frame
                if( selected )
                {

                    // do nothing if dragged
                    if( isDragged ) break;

                    // top side
                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 + 1 );
                        frameRect.setRight( tabRect.left() + 13 );
                        frameRect.setTop( frameRect.top() - GlowWidth );
                        frameRect.setBottom( tabRect.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    } else {

                        QRect frameRect( r );
                        frameRect.setRight( r.left() + 7 );
                        frameRect.setTop( frameRect.top() - 7 );
                        frameRect.setBottom( tabRect.top() + 7 + 3 );
                        slabs << SlabRect( frameRect, TileSet::Right );
                    }

                    // bottom side
                    if( isRightFrameAligned )
                    {

                        // FIXME:
                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 + 1 );
                        frameRect.setRight( tabRect.left() + 13 );
                        frameRect.setTop( tabRect.bottom() - 7 );
                        frameRect.setBottom( frameRect.bottom() + GlowWidth );
                        slabs << SlabRect( frameRect, TileSet::Bottom );

                    } else {

                        QRect frameRect( r );
                        frameRect.setRight( r.left() + 7 );
                        frameRect.setTop( tabRect.bottom() - 7 - 3 );
                        frameRect.setBottom( frameRect.bottom() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    }

                    // extra base, to extend below tabbar buttons
                    if( tabBar )
                    {
                        if( r.top() > tabBarRect.top() + 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( tabBarRect.top() - 7 + 1 );
                            frameRect.setBottom( r.top() + 7 - 1 );
                            frameRect.setRight( r.left() + 7 );
                            if( documentMode ) slabs << SlabRect( frameRect, TileSet::Right );
                            else slabs << SlabRect( frameRect, TileSet::TopRight );
                        }

                        if( r.bottom() < tabBarRect.bottom() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( r.bottom() - 7 + 1 );
                            if( hasRightCornerWidget && documentMode ) frameRect.setBottom( tabBarRect.bottom() + 7 - 1 );
                            else frameRect.setBottom( tabBarRect.bottom() + 7 );
                            frameRect.setRight( r.left() + 7 );
                            slabs << SlabRect( frameRect, TileSet::Right );

                        }
                    }

                } else {

                    if( isLeftFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 + 2 );
                        frameRect.setRight( tabRect.left() + 13 );
                        frameRect.setTop( frameRect.top() - GlowWidth );
                        frameRect.setBottom( tabRect.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    }

                    if( isRightFrameAligned )
                    {

                        // FIXME:
                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 + 2 );
                        frameRect.setRight( tabRect.left() + 13 );
                        frameRect.setTop( tabRect.bottom() - 7 );
                        frameRect.setBottom( frameRect.bottom() + GlowWidth );
                        slabs << SlabRect( frameRect, TileSet::Bottom );

                    }

                }

                break;
            }

            default: break;
        }


        // slab options
        StyleOptions slabOptions( NoFill );
        if( StyleConfigData::tabSubtleShadow() ) slabOptions |= SubtleShadow;
        if( ( !selected ) && ( mouseOver || animated ) ) slabOptions |= Hover;

        // color
        const QColor color( palette.color( QPalette::Window ) );

        // render connections to frame
        // extra care must be taken care of so that no slab
        // extends beyond tabWidget frame, if any
        foreach( SlabRect slab, slabs ) // krazy:exclude=foreach
        {
            adjustSlabRect( slab, tabWidgetRect, documentMode, verticalTabs );
            if( selected || !animated ) renderSlab( painter, slab, color, slabOptions );
            else {

                const qreal opacity( animations().tabBarEngine().opacity( widget, r.topLeft() ) );
                renderSlab( painter, slab, color, slabOptions, opacity, AnimationHover );

            }

        }

        //  adjust clip rect and render tabs
        if( tabBar )
        {
            painter->save();
            painter->setClipRegion( tabBarClipRegion( tabBar ) );
        }

        // draw tab
        TileSet::Tiles tiles( tilesByShape( tabOpt->shape ) );
        if( selected )
        {

            // render window background first, if needed
            if( fillBackground ) fillTabBackground( painter, tabRect, color, tabOpt->shape, widget );

            // render window background in case of dragged tabwidget
            renderSlab( painter, tabRect, color, slabOptions, tiles );

        } else if( animated ) {

            const qreal opacity( animations().tabBarEngine().opacity( widget, r.topLeft() ) );
            renderSlab( painter, tabRect, color, slabOptions, opacity, AnimationHover, tiles );

        } else renderSlab( painter, tabRect, color, slabOptions, tiles );

        // fill tab
        fillTab( painter, tabRect, color, tabOpt->shape, selected );

        // restore clip region
        if( tabBar ) painter->restore();

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawTabBarTabShapeControl_Plain( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionTab* tabOpt( qstyleoption_cast<const QStyleOptionTab*>( option ) );
        if( !tabOpt ) return true;

        const State& flags( option->state );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const bool enabled( flags & State_Enabled );
        const bool selected( flags&State_Selected );
        const bool reverseLayout( option->direction == Qt::RightToLeft );

        // tab position and flags
        const QStyleOptionTab::TabPosition& position = tabOpt->position;
        bool isFirst( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::Beginning );
        bool isLast( position == QStyleOptionTab::OnlyOneTab || position == QStyleOptionTab::End );
        bool isLeftOfSelected( tabOpt->selectedPosition == QStyleOptionTab::NextIsSelected );
        bool isRightOfSelected( tabOpt->selectedPosition == QStyleOptionTab::PreviousIsSelected );

        // document mode
        const QStyleOptionTabV3 *tabOptV3 = qstyleoption_cast<const QStyleOptionTabV3 *>( option );
        bool documentMode = tabOptV3 ? tabOptV3->documentMode : false;
        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : NULL;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        // this is needed to complete the base frame when there are widgets in tabbar
        const QTabBar* tabBar( qobject_cast<const QTabBar*>( widget ) );
        const QRect tabBarRect( tabBar ? insideMargin( tabBar->rect(), -GlowWidth ):QRect() );

        // check if tab is being dragged
        const bool isDragged( selected && painter->device() != tabBar );

        // hover and animation flags
        /* all are disabled when tabBar is locked ( drag in progress ) */
        const bool tabBarLocked( tabBarData().locks( tabBar ) );
        const bool mouseOver( enabled && !tabBarLocked && ( flags & State_MouseOver ) );

        // animation state
        animations().tabBarEngine().updateState( widget, r.topLeft(), mouseOver );
        const bool animated( enabled && !selected && !tabBarLocked && animations().tabBarEngine().isAnimated( widget, r.topLeft() ) );

        // handle base frame painting, for tabbars in which tab is being dragged
        tabBarData().drawTabBarBaseControl( tabOpt, painter, widget );
        if( selected && tabBar && isDragged ) tabBarData().lock( tabBar );
        else if( selected  && tabBarData().locks( tabBar ) ) tabBarData().release();

        // corner widgets
        const bool hasLeftCornerWidget( tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget );
        const bool hasRightCornerWidget( tabOpt->cornerWidgets & QStyleOptionTab::RightCornerWidget );

        // true if widget is aligned to the frame
        /* need to check for 'isRightOfSelected' because for some reason the isFirst flag is set when active tab is being moved */
        const bool isFrameAligned( !documentMode && isFirst && !hasLeftCornerWidget && !isRightOfSelected && !isDragged );
        isFirst &= !isRightOfSelected;
        isLast &= !isLeftOfSelected;

        // swap flags based on reverse layout, so that they become layout independent
        const bool verticalTabs( isVerticalTab( tabOpt ) );
        if( reverseLayout && !verticalTabs )
        {
            qSwap( isFirst, isLast );
            qSwap( isLeftOfSelected, isRightOfSelected );
        }

        const qreal radius = 5;

        // part of the tab in which the text is drawn
        QRect tabRect( r );
        QPainterPath path;

        // connection to the frame
        SlabRectList slabs;

        // highlighted slab ( if any )
        SlabRect highlightSlab;

        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {

                // part of the tab in which the text is drawn
                // larger tabs when selected
                if( selected ) tabRect.adjust( 0, -1, 0, 2 );
                else tabRect.adjust( 0, 3, 1, -7 + 1 );

                // connection to the main frame
                if( selected )
                {

                    // reduces the space between tabs
                    tabRect.adjust( -GlowWidth,0,GlowWidth,0 );

                    // do nothing if dragged
                    if( isDragged ) break;

                    // left side
                    if( isFrameAligned && !reverseLayout )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - GlowWidth );
                        frameRect.setRight( tabRect.left() + 7 );
                        frameRect.setTop( tabRect.bottom() - 13 );
                        frameRect.setBottom( frameRect.bottom() + 7 - 1 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 );
                        frameRect.setRight( tabRect.left() + 7 + 3 );
                        frameRect.setTop( r.bottom() - 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );
                    }

                    // right side
                    if( isFrameAligned && reverseLayout )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 );
                        frameRect.setRight( frameRect.right() + GlowWidth );
                        frameRect.setTop( tabRect.bottom() - 13 );
                        frameRect.setBottom( frameRect.bottom() + 7 - 1 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 - 3 );
                        frameRect.setRight( frameRect.right() + 7 );
                        frameRect.setTop( r.bottom() - 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    }

                    // extra base, to extend below inactive tabs and buttons
                    if( tabBar )
                    {
                        if( r.left() > tabBarRect.left() + 1 )
                        {
                            QRect frameRect( r );
                            frameRect.setLeft( tabBarRect.left() - 7 + 1 );
                            frameRect.setRight( r.left() + 7 - 1 );
                            frameRect.setTop( r.bottom() - 7 );
                            if( documentMode || reverseLayout ) slabs << SlabRect( frameRect, TileSet::Top );
                            else slabs << SlabRect( frameRect, TileSet::TopLeft );

                        }

                        if( r.right() < tabBarRect.right() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setLeft( r.right() - 7 + 1 );
                            frameRect.setRight( tabBarRect.right() + 7 - 1 );
                            frameRect.setTop( r.bottom() - 7 );
                            if( documentMode || !reverseLayout ) slabs << SlabRect( frameRect, TileSet::Top );
                            else slabs << SlabRect( frameRect, TileSet::TopRight );
                        }
                    }

                } else {

                    // adjust sides when slab is adjacent to selected slab
                    if( isLeftOfSelected ) tabRect.setRight( tabRect.right() + 2 );
                    else if( isRightOfSelected ) tabRect.setLeft( tabRect.left() - 2 );

                    if( isFirst )
                    {

                        tabRect.adjust( GlowWidth, 0, 0, 0 );
                        if( isFrameAligned ) path.moveTo( tabRect.bottomLeft() + QPoint( 0, 2 ) );
                        else path.moveTo( tabRect.bottomLeft() );
                        path.lineTo( tabRect.topLeft() + QPointF( 0, radius ) );
                        path.quadTo( tabRect.topLeft(), tabRect.topLeft() + QPoint( radius, 0 ) );
                        path.lineTo( tabRect.topRight() );
                        path.lineTo( tabRect.bottomRight() );


                    } else if( isLast ) {

                        tabRect.adjust( 0, 0, -GlowWidth-1, 0 );
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
                    QRect highlightRect( tabRect.left(), tabRect.bottom()-1, tabRect.width(), 7 );
                    if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( -2, 0, 7 + 1, 0 ), TileSet::TopLeft );
                    else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( -7, 0, 2, 0 ), TileSet::TopRight );
                    else highlightSlab = SlabRect( highlightRect.adjusted( -7, 0, 7, 0 ), TileSet::Top );

                }

                break;

            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {

                // larger tabs when selected
                if( selected ) tabRect.adjust( 0, -2, 0, 1 );
                else tabRect.adjust( 0, 7 - 1, 1, -3 );

                // connection to the main frame
                if( selected )
                {

                    // reduces the space between tabs
                    tabRect.adjust( -GlowWidth,0,GlowWidth,0 );

                    // do nothing if dragged
                    if( isDragged ) break;

                    // left side
                    if( isFrameAligned && !reverseLayout )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - GlowWidth );
                        frameRect.setRight( tabRect.left() + 7 );
                        frameRect.setTop( frameRect.top() - 7 + 1 );
                        frameRect.setBottom( tabRect.top() + 13 );
                        slabs << SlabRect( frameRect, TileSet::Left );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 );
                        frameRect.setRight( tabRect.left() + 7 + 3 );
                        frameRect.setBottom( r.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Bottom );
                    }

                    // right side
                    if( isFrameAligned && reverseLayout )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 );
                        frameRect.setRight( frameRect.right() + GlowWidth );
                        frameRect.setTop( frameRect.top() - 7 + 1 );
                        frameRect.setBottom( tabRect.top() + 13 );
                        slabs << SlabRect( frameRect, TileSet::Right );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 7 - 3 );
                        frameRect.setRight( frameRect.right() + 7 );
                        frameRect.setBottom( r.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Bottom );

                    }

                    // extra base, to extend below tabbar buttons
                    if( tabBar )
                    {
                        if( r.left() > tabBarRect.left() + 1 )
                        {
                            QRect frameRect( r );
                            frameRect.setLeft( tabBarRect.left() - 7 + 1 );
                            frameRect.setRight( r.left() + 7 - 1 );
                            frameRect.setBottom( r.top() + 7 );
                            if( documentMode || reverseLayout ) slabs << SlabRect( frameRect, TileSet::Bottom );
                            else slabs << SlabRect( frameRect, TileSet::BottomLeft );
                        }

                        if( r.right() < tabBarRect.right() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setLeft( r.right() - 7 + 1 );
                            frameRect.setRight( tabBarRect.right() + 7 - 1 );
                            frameRect.setBottom( r.top() + 7 );
                            if( documentMode || !reverseLayout ) slabs << SlabRect( frameRect, TileSet::Bottom );
                            else slabs << SlabRect( frameRect, TileSet::BottomRight );
                        }

                    }

                } else {

                    // adjust sides when slab is adjacent to selected slab
                    if( isLeftOfSelected ) tabRect.setRight( tabRect.right() + 2 );
                    else if( isRightOfSelected ) tabRect.setLeft( tabRect.left() - 2 );

                    if( isFirst )
                    {

                        tabRect.adjust( GlowWidth, 0, 0, 0 );
                        if( isFrameAligned ) path.moveTo( tabRect.topLeft() - QPoint( 0, 2 ) );
                        else path.moveTo( tabRect.topLeft() );
                        path.lineTo( tabRect.bottomLeft() - QPointF( 0, radius ) );
                        path.quadTo( tabRect.bottomLeft(), tabRect.bottomLeft() + QPoint( radius, 0 ) );
                        path.lineTo( tabRect.bottomRight() );
                        path.lineTo( tabRect.topRight() );

                    } else if( isLast ) {

                        tabRect.adjust( 0, 0, -GlowWidth-1, 0 );
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
                    QRect highlightRect( tabRect.left(), tabRect.top()-5, tabRect.width(), 7 );
                    if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( -2, 0, 7 + 1, 0 ), TileSet::BottomLeft );
                    else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( -7, 0, 2, 0 ), TileSet::BottomRight );
                    else highlightSlab = SlabRect( highlightRect.adjusted( -7, 0, 7, 0 ), TileSet::Bottom );

                }

                break;

            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {

                // larger tabs when selected
                if( selected ) tabRect.adjust( -1, 0, 2, 0 );
                else tabRect.adjust( 3, 0, -7 + 1, 1 );

                // connection to the main frame
                if( selected )
                {

                    // reduces the space between tabs
                    tabRect.adjust( 0, -GlowWidth,0,GlowWidth );

                    // do nothing if dragged
                    if( isDragged ) break;

                    // top side
                    if( isFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( tabRect.right() - 13 );
                        frameRect.setRight( frameRect.right() + 7 - 1 );
                        frameRect.setTop( frameRect.top() - GlowWidth );
                        frameRect.setBottom( tabRect.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    } else {

                        QRect frameRect( r );
                        frameRect.setLeft( r.right() - 7 );
                        frameRect.setTop( frameRect.top() - 7 );
                        frameRect.setBottom( tabRect.top() + 7 + 3 );
                        slabs << SlabRect( frameRect, TileSet::Left );
                    }

                    // bottom side
                    QRect frameRect( r );
                    frameRect.setLeft( r.right() - 7 );
                    frameRect.setTop( tabRect.bottom() - 7 - 3 );
                    frameRect.setBottom( frameRect.bottom() + 7 );
                    slabs << SlabRect( frameRect, TileSet::Left );

                    // extra base, to extend below tabbar buttons
                    if( tabBar )
                    {
                        if( r.top() > tabBarRect.top() + 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( tabBarRect.top() - 7 + 1 );
                            frameRect.setBottom( r.top() + 7 - 1 );
                            frameRect.setLeft( r.right() - 7 );
                            if( documentMode ) slabs << SlabRect( frameRect, TileSet::Left );
                            else slabs << SlabRect( frameRect, TileSet::TopLeft );
                        }

                        if( r.bottom() < tabBarRect.bottom() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( r.bottom() - 7 + 1 );
                            if( hasRightCornerWidget && documentMode ) frameRect.setBottom( tabBarRect.bottom() + 7 - 1 );
                            else frameRect.setBottom( tabBarRect.bottom() + 7 );
                            frameRect.setLeft( r.right() - 7 );
                            slabs << SlabRect( frameRect, TileSet::Left );

                        }
                    }

                } else {

                    // adjust sides when slab is adjacent to selected slab
                    if( isLeftOfSelected ) tabRect.setBottom( tabRect.bottom() + 2 );
                    else if( isRightOfSelected ) tabRect.setTop( tabRect.top() - 2 );

                    if( isFirst )
                    {

                        tabRect.adjust( 0, GlowWidth, 0, 0 );
                        if( isFrameAligned ) path.moveTo( tabRect.topRight() + QPoint( 2, 0 ) );
                        else path.moveTo( tabRect.topRight() );
                        path.lineTo( tabRect.topLeft() + QPointF( radius, 0 ) );
                        path.quadTo( tabRect.topLeft(), tabRect.topLeft() + QPoint( 0, radius ) );
                        path.lineTo( tabRect.bottomLeft() );
                        path.lineTo( tabRect.bottomRight() );

                    } else if( isLast ) {

                        tabRect.adjust( 0, 0, 0, -GlowWidth );
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
                    QRect highlightRect( tabRect.right()-1, tabRect.top(), 7, tabRect.height() );
                    if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( 0, -2, 0, 7 + 1 ), TileSet::TopLeft );
                    else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( 0, -7, 0, 2 ), TileSet::BottomLeft );
                    else highlightSlab = SlabRect( highlightRect.adjusted( 0, -7 + 1, 0, 7 + 1 ), TileSet::Left );

                }

                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {

                // larger tabs when selected
                if( selected ) tabRect.adjust( -2, 0, 1, 0 );
                else tabRect.adjust( 7 - 1, 0, -3, 1 );

                // connection to the main frame
                if( selected )
                {

                    // reduces the space between tabs
                    tabRect.adjust( 0, -GlowWidth,0,GlowWidth );

                    // do nothing if dragged
                    if( isDragged ) break;

                    // top side
                    if( isFrameAligned )
                    {

                        QRect frameRect( r );
                        frameRect.setLeft( frameRect.left() - 7 + 1 );
                        frameRect.setRight( tabRect.left() + 13 );
                        frameRect.setTop( frameRect.top() - GlowWidth );
                        frameRect.setBottom( tabRect.top() + 7 );
                        slabs << SlabRect( frameRect, TileSet::Top );

                    } else {

                        QRect frameRect( r );
                        frameRect.setRight( r.left() + 7 );
                        frameRect.setTop( frameRect.top() - 7 );
                        frameRect.setBottom( tabRect.top() + 7 + 3 );
                        slabs << SlabRect( frameRect, TileSet::Right );
                    }

                    // bottom side
                    QRect frameRect( r );
                    frameRect.setRight( r.left() + 7 );
                    frameRect.setTop( tabRect.bottom() - 7 - 3 );
                    frameRect.setBottom( frameRect.bottom() + 7 );
                    slabs << SlabRect( frameRect, TileSet::Right );

                    // extra base, to extend below tabbar buttons
                    if( tabBar )
                    {
                        if( r.top() > tabBarRect.top() + 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( tabBarRect.top() - 7 + 1 );
                            frameRect.setBottom( r.top() + 7 - 1 );
                            frameRect.setRight( r.left() + 7 );
                            if( documentMode ) slabs << SlabRect( frameRect, TileSet::Right );
                            else slabs << SlabRect( frameRect, TileSet::TopRight );
                        }

                        if( r.bottom() < tabBarRect.bottom() - 1 )
                        {

                            QRect frameRect( r );
                            frameRect.setTop( r.bottom() - 7 + 1 );
                            if( hasRightCornerWidget && documentMode ) frameRect.setBottom( tabBarRect.bottom() + 7 - 1 );
                            else frameRect.setBottom( tabBarRect.bottom() + 7 );
                            frameRect.setRight( r.left() + 7 );
                            slabs << SlabRect( frameRect, TileSet::Right );

                        }
                    }

                } else {

                    // adjust sides when slab is adjacent to selected slab
                    if( isLeftOfSelected ) tabRect.setBottom( tabRect.bottom() + 2 );
                    else if( isRightOfSelected ) tabRect.setTop( tabRect.top() - 2 );

                    if( isFirst )
                    {

                        tabRect.adjust( 0, GlowWidth, 0, 0 );
                        if( isFrameAligned ) path.moveTo( tabRect.topLeft() - QPoint( 2, 0 ) );
                        else path.moveTo( tabRect.topLeft() );
                        path.lineTo( tabRect.topRight() - QPointF( radius, 0 ) );
                        path.quadTo( tabRect.topRight(), tabRect.topRight() + QPoint( 0, radius ) );
                        path.lineTo( tabRect.bottomRight() );
                        path.lineTo( tabRect.bottomLeft() );

                    } else if( isLast ) {

                        tabRect.adjust( 0, 0, 0, -GlowWidth );
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
                    QRect highlightRect( tabRect.left()-5, tabRect.top(), 7, tabRect.height() );
                    if( isFrameAligned && isFirst ) highlightSlab = SlabRect( highlightRect.adjusted( 0, -2, 0, 7 + 1 ), TileSet::TopRight );
                    else if( isFrameAligned && isLast ) highlightSlab = SlabRect( highlightRect.adjusted( 0, -7, 0, 2 ), TileSet::BottomRight );
                    else highlightSlab = SlabRect( highlightRect.adjusted( 0, -7 + 1, 0, 7 + 1 ), TileSet::Right );

                }

                break;
            }

            default: break;
        }

        const QColor color( palette.color( QPalette::Window ) );

        // render connections to frame
        // extra care must be taken care of so that no slab
        // extends beyond tabWidget frame, if any
        const QRect tabWidgetRect( tabWidget ?
            insideMargin( tabWidget->rect(), -GlowWidth ).translated( -widget->geometry().topLeft() ) :
            QRect() );

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
        if( selected )
        {

            // render window background in case of dragged tabwidget
            if( isDragged ) fillTabBackground( painter, tabRect, color, tabOpt->shape, widget );

            // slab options
            StyleOptions selectedTabOpts( NoFill );
            if( StyleConfigData::tabSubtleShadow() ) selectedTabOpts |= SubtleShadow;

            TileSet::Tiles tiles( tilesByShape( tabOpt->shape ) );
            renderSlab( painter, tabRect, color, selectedTabOpts, tiles );
            fillTab( painter, tabRect, color, tabOpt->shape, selected );

        } else {

            const QColor backgroundColor = helper().backgroundColor( color, widget, r.center() );
            const QColor midColor = helper().alphaColor( helper().calcDarkColor( backgroundColor ), 0.4 );
            const QColor darkColor = helper().alphaColor( helper().calcDarkColor( backgroundColor ), 0.6 );

            painter->save();
            painter->translate( 0.5, 0.5 );
            painter->setRenderHints( QPainter::Antialiasing );
            painter->setPen( darkColor );
            painter->setBrush( midColor );
            painter->drawPath( path );
            painter->restore();

        }

        // restore clip region
        if( tabBar ) painter->restore();

        // hovered highlight
        if( ( animated || mouseOver ) && highlightSlab._r.isValid() )
        {

            const qreal opacity( animations().tabBarEngine().opacity( widget, r.topLeft() ) );
            const StyleOptions hoverTabOpts( NoFill | Hover );
            adjustSlabRect( highlightSlab, tabWidgetRect, documentMode, verticalTabs );

            // pass an invalid color to have only the glow painted
            if( animated ) renderSlab( painter, highlightSlab, QColor(), hoverTabOpts, opacity, AnimationHover );
            else renderSlab( painter, highlightSlab, QColor(), hoverTabOpts );

        }


        return true;

    }

    //___________________________________________________________________________________
    void Style::TabBarData::drawTabBarBaseControl( const QStyleOptionTab* tabOpt, QPainter* painter, const QWidget* widget )
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
        const bool reverseLayout( tabOpt->direction == Qt::RightToLeft );

        // get documentMode flag
        const QStyleOptionTabV3 *tabOptV3 = qstyleoption_cast<const QStyleOptionTabV3 *>( tabOpt );
        bool documentMode = tabOptV3 ? tabOptV3->documentMode : false;
        const QTabWidget *tabWidget = ( widget && widget->parentWidget() ) ? qobject_cast<const QTabWidget *>( widget->parentWidget() ) : NULL;
        documentMode |= ( tabWidget ? tabWidget->documentMode() : true );

        const QRect tabBarRect( _style.data()->insideMargin( tabBar->rect(), -GlowWidth ) );

        // define slab
        Style::SlabRect slab;

        // switch on tab shape
        switch( tabOpt->shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            {
                TileSet::Tiles tiles( TileSet::Top );
                QRect frameRect;
                frameRect.setLeft( tabBarRect.left() - 7 + 1 );
                frameRect.setRight( tabBarRect.right() + 7 - 1 );
                frameRect.setTop( tabBarRect.bottom() - 8 );
                frameRect.setHeight( 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= TileSet::Left;
                if( !documentMode && reverseLayout ) tiles |= TileSet::Right;
                slab = SlabRect( frameRect, tiles );
                break;
            }

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            {
                TileSet::Tiles tiles( TileSet::Bottom );
                QRect frameRect;
                frameRect.setLeft( tabBarRect.left() - 7 + 1 );
                frameRect.setRight( tabBarRect.right() + 7 - 1 );
                frameRect.setBottom( tabBarRect.top() + 8 );
                frameRect.setTop( frameRect.bottom() - 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= TileSet::Left;
                if( !documentMode && reverseLayout ) tiles |= TileSet::Right;
                slab = SlabRect( frameRect, tiles );
                break;
            }

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            {
                TileSet::Tiles tiles( TileSet::Left );
                QRect frameRect;
                frameRect.setTop( tabBarRect.top() - 7 + 1 );
                frameRect.setBottom( tabBarRect.bottom() + 7 - 1 );
                frameRect.setLeft( tabBarRect.right() - 8 );
                frameRect.setWidth( 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= TileSet::Top;
                if( !documentMode && reverseLayout ) tiles |= TileSet::Bottom;
                slab = SlabRect( frameRect, tiles );
                break;
            }

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            {
                TileSet::Tiles tiles( TileSet::Right );
                QRect frameRect;
                frameRect.setTop( tabBarRect.top() - 7 + 1 );
                frameRect.setBottom( tabBarRect.bottom() + 7 - 1 );
                frameRect.setRight( tabBarRect.left() + 8 );
                frameRect.setLeft( frameRect.right() - 4 );
                if( !( documentMode || reverseLayout ) ) tiles |= TileSet::Top;
                if( !documentMode && reverseLayout ) tiles |= TileSet::Bottom;
                slab = SlabRect( frameRect, tiles );
                break;
            }

            default:
            break;
        }

        const bool verticalTabs( _style.data()->isVerticalTab( tabOpt ) );
        const QRect tabWidgetRect( tabWidget ?
            _style.data()->insideMargin( tabWidget->rect(), -GlowWidth ).translated( -widget->geometry().topLeft() ) :
            QRect() );

        const QPalette& palette( tabOpt->palette );
        const QColor color( palette.color( QPalette::Window ) );
        _style.data()->adjustSlabRect( slab, tabWidgetRect, documentMode, verticalTabs );
        _style.data()->renderSlab( painter, slab, color, NoFill );

        setDirty( false );
        return;

    }

    //___________________________________________________________________________________
    bool Style::drawToolBarControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QRect& r( option->rect );

        // when timeLine is running draw border event if not hovered
        const bool toolBarAnimated( animations().toolBarEngine().isFollowMouseAnimated( widget ) );
        const QRect animatedRect( animations().toolBarEngine().animatedRect( widget ) );
        const bool toolBarIntersected( toolBarAnimated && animatedRect.intersects( r ) );
        if( toolBarIntersected )
        { helper().slitFocused( helper().viewFocusBrush().brush( QPalette::Active ).color() )->render( animatedRect, painter ); }

        // draw nothing otherwise ( toolbars are transparent )

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawToolBoxTabLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionToolBox* toolBoxOption( qstyleoption_cast<const QStyleOptionToolBox *>( option ) );
        const bool enabled( toolBoxOption->state & State_Enabled );
        const bool selected( toolBoxOption->state & State_Selected );
        QPixmap pm(
            toolBoxOption->icon.pixmap( pixelMetric( QStyle::PM_SmallIconSize, toolBoxOption, widget ),
            enabled ? QIcon::Normal : QIcon::Disabled ) );

        const QRect cr( toolBoxTabContentsRect( toolBoxOption, widget ) );
        QRect tr;
        QRect ir;
        int ih( 0 );

        if( pm.isNull() )  tr = cr.adjusted( -1, 0, -8, 0 );
        else {

            int iw = pm.width() + 4;
            ih = pm.height();
            ir = QRect( cr.left() - 1, cr.top(), iw + 2, ih );
            tr = QRect( ir.right(), cr.top(), cr.width() - ir.right() - 4, cr.height() );

        }

        if( selected )
        {
            QFont f( painter->font() );
            f.setBold( true );
            painter->setFont( f );
        }

        QString txt( toolBoxOption->fontMetrics.elidedText( toolBoxOption->text, Qt::ElideRight, tr.width() ) );

        if( ih ) painter->drawPixmap( ir.left(), ( toolBoxOption->rect.height() - ih ) / 2, pm );

        int alignment( Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic );
        drawItemText( painter, tr, alignment, toolBoxOption->palette, enabled, txt, QPalette::WindowText );

        return true;
    }

    //___________________________________________________________________________________
    bool Style::drawToolBoxTabShapeControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const State& flags( option->state );

        const bool enabled( flags&State_Enabled );
        const bool selected( flags&State_Selected );
        const bool mouseOver( enabled && !selected && ( flags&State_MouseOver ) );
        const bool reverseLayout( option->direction == Qt::RightToLeft );


        // cast to v2 and disable paint is tab is first
        const QStyleOptionToolBoxV2 *v2 = qstyleoption_cast<const QStyleOptionToolBoxV2 *>( option );
        if( v2 && v2->position == QStyleOptionToolBoxV2::Beginning && selected ) return true;

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
                animations().toolBoxEngine().updateState( device, mouseOver );
                animated = animations().toolBoxEngine().isAnimated( device );
                opacity = animations().toolBoxEngine().opacity( device );
            }

        }

        // save colors for shadow
        /* important: option returns a wrong color. We use the widget's palette when widget is set */
        const QColor color( widget ? widget->palette().color( widget->backgroundRole() ) : palette.color( QPalette::Window ) );
        const QColor dark( helper().calcDarkColor( color ) );
        QList<QColor> colors;
        colors.push_back( helper().calcLightColor( color ) );

        if( mouseOver || animated )
        {

            QColor highlight = helper().viewHoverBrush().brush( palette ).color();
            if( animated )
            {

                colors.push_back( KColorUtils::mix( dark, highlight, opacity ) );
                colors.push_back( helper().alphaColor( highlight, 0.2*opacity ) );

            } else {

                colors.push_back( highlight );
                colors.push_back( helper().alphaColor( highlight, 0.2 ) );

            }

        } else colors.push_back( dark );

        // create path
        painter->save();
        QPainterPath path;
        const int y( r.height()*15/100 );
        if( reverseLayout )
        {

            path.moveTo( r.left()+52, r.top() );
            path.cubicTo( QPointF( r.left()+50-8, r.top() ), QPointF( r.left()+50-10, r.top()+y ), QPointF( r.left()+50-10, r.top()+y ) );
            path.lineTo( r.left()+18+9, r.bottom()-y );
            path.cubicTo( QPointF( r.left()+18+9, r.bottom()-y ), QPointF( r.left()+19+6, r.bottom()-1-0.3 ), QPointF( r.left()+19, r.bottom()-1-0.3 ) );
            painter->setClipRect( QRect( r.left()+21, r.top(), 28, r.height() ) );

        } else {

            path.moveTo( r.right()-52, r.top() );
            path.cubicTo( QPointF( r.right()-50+8, r.top() ), QPointF( r.right()-50+10, r.top()+y ), QPointF( r.right()-50+10, r.top()+y ) );
            path.lineTo( r.right()-18-9, r.bottom()-y );
            path.cubicTo( QPointF( r.right()-18-9, r.bottom()-y ), QPointF( r.right()-19-6, r.bottom()-1-0.3 ), QPointF( r.right()-19, r.bottom()-1-0.3 ) );
            painter->setClipRect( QRect( r.right()-48, r.top(), 32, r.height() ) );

        }


        // paint
        painter->setRenderHint( QPainter::Antialiasing, true );
        painter->translate( 0,2 );
        foreach( const QColor& color, colors )
        {
            painter->setPen( color );
            painter->drawPath( path );
            painter->translate( 0,-1 );
        }
        painter->restore();

        painter->save();
        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->translate( 0,2 );
        foreach( const QColor& color, colors )
        {
            painter->setPen( color );
            if( reverseLayout ) {
                painter->drawLine( r.left()+50-1, r.top(), r.right(), r.top() );
                painter->drawLine( r.left()+20, r.bottom()-2, r.left(), r.bottom()-2 );
            } else {
                painter->drawLine( r.left(), r.top(), r.right()-50+1, r.top() );
                painter->drawLine( r.right()-20, r.bottom()-2, r.right(), r.bottom()-2 );
            }
            painter->translate( 0,-1 );
        }

        painter->restore();
        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawToolButtonLabelControl( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {

        // need to customize palettes to deal with autoraised buttons
        const State& flags( option->state );

        // normal processing if not autoRaised
        if( flags & State_AutoRaise )
        {

            const QStyleOptionToolButton* toolButtonOpt( qstyleoption_cast<const QStyleOptionToolButton*>( option ) );
            if( !toolButtonOpt ) return true;

            QStyleOptionToolButton localOption( *toolButtonOpt );
            localOption.palette.setColor( QPalette::ButtonText, option->palette.color( QPalette::WindowText ) );

            QCommonStyle::drawControl( CE_ToolButtonLabel, &localOption, painter, widget );

        } else {

            QCommonStyle::drawControl( CE_ToolButtonLabel, option, painter, widget );

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawComboBoxComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        // cast option and check
        const QStyleOptionComboBox* cb( qstyleoption_cast<const QStyleOptionComboBox *>( option ) );
        if( !cb ) return true;

        const State& flags( option->state );
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( flags & State_HasFocus );
        const bool& editable( cb->editable );
        const bool& hasFrame( cb->frame );

        // frame
        if( cb->subControls & SC_ComboBoxFrame )
        {


            // style options
            StyleOptions opts = 0;
            if( mouseOver ) opts |= Hover;
            if( hasFocus ) opts |= Focus;
            if( ( flags & ( State_Sunken|State_On ) ) && !editable ) opts |= Sunken;

            const QColor inputColor( palette.color( QPalette::Base ) );
            const QRect editField( subControlRect( CC_ComboBox, cb, SC_ComboBoxEditField, widget ) );

            if( editable )
            {

                // editable combobox. Make it look like a LineEdit
                // focus takes precedence over hover
                animations().lineEditEngine().updateState( widget, AnimationFocus, hasFocus );
                animations().lineEditEngine().updateState( widget, AnimationHover, mouseOver && !hasFocus );

                const QRect fr( r.adjusted( 1,1,-1,-1 ) );

                // input area
                painter->save();
                painter->setRenderHint( QPainter::Antialiasing );
                painter->setPen( Qt::NoPen );
                painter->setBrush( inputColor );

                if( !hasFrame )
                {

                    // adjust rect to match frameLess editors
                    painter->fillRect( r, inputColor );
                    painter->restore();

                } else {

                    helper().fillHole( *painter, r.adjusted( 0, -1, 0, 0 ) );
                    painter->restore();

                    HoleOptions options( 0 );
                    if( hasFocus && enabled ) options |= HoleFocus;
                    if( mouseOver && enabled ) options |= HoleHover;

                    const QColor color( palette.color( QPalette::Window ) );
                    if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationFocus ) )
                    {

                        helper().renderHole( painter, color, fr, options, animations().lineEditEngine().opacity( widget, AnimationFocus ), AnimationFocus, TileSet::Ring );

                    } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationHover ) ) {

                        helper().renderHole( painter, color, fr, options, animations().lineEditEngine().opacity( widget, AnimationHover ), AnimationHover, TileSet::Ring );

                    } else {

                        helper().renderHole( painter, color, fr, options );

                    }

                }

            } else {

                // non editable combobox. Make it look like a PushButton
                // hover takes precedence over focus
                animations().lineEditEngine().updateState( widget, AnimationHover, mouseOver );
                animations().lineEditEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

                // store animation state
                const bool hoverAnimated( animations().lineEditEngine().isAnimated( widget, AnimationHover ) );
                const bool focusAnimated( animations().lineEditEngine().isAnimated( widget, AnimationFocus ) );
                const qreal hoverOpacity( animations().lineEditEngine().opacity( widget, AnimationHover ) );
                const qreal focusOpacity( animations().lineEditEngine().opacity( widget, AnimationFocus ) );

                // blend button color to the background
                const QColor buttonColor( helper().backgroundColor( palette.color( QPalette::Button ), widget, r.center() ) );
                const QRect slabRect( r.adjusted( -1, 0, 1, 0 ) );

                if( !hasFrame )
                {

                    QRect slitRect( r );
                    if( !( opts & Sunken ) )
                    {
                        // hover rect
                        if( enabled && hoverAnimated )
                        {

                            QColor glow( helper().alphaColor( helper().viewFocusBrush().brush( QPalette::Active ).color(), hoverOpacity ) );
                            helper().slitFocused( glow )->render( slitRect, painter );

                        } else if( mouseOver ) {

                            helper().slitFocused( helper().viewFocusBrush().brush( QPalette::Active ).color() )->render( slitRect, painter );

                        }

                    } else {

                        slitRect.adjust( 0, 0, 0, -1 );

                        HoleOptions options( HoleContrast );
                        if( mouseOver && enabled ) options |= HoleHover;

                        // flat pressed-down buttons do not get focus effect,
                        // consistently with tool buttons
                        if( enabled && hoverAnimated )
                        {

                            helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options, hoverOpacity, AnimationHover, TileSet::Ring );

                        } else {

                            helper().renderHole( painter, palette.color( QPalette::Window ), slitRect, options );

                        }

                    }

                } else {

                    if( enabled && hoverAnimated )
                    {

                        renderButtonSlab( painter, slabRect, buttonColor, opts, hoverOpacity, AnimationHover, TileSet::Ring );

                    } else if( enabled && focusAnimated ) {

                        renderButtonSlab( painter, slabRect, buttonColor, opts, focusOpacity, AnimationFocus, TileSet::Ring );

                    } else {

                        renderButtonSlab( painter, slabRect, buttonColor, opts );

                    }

                }

            }

        }

        if( cb->subControls & SC_ComboBoxArrow )
        {

            const QComboBox* comboBox = qobject_cast<const QComboBox*>( widget );
            const bool empty( comboBox && !comboBox->count() );

            QColor color;
            QColor background;
            bool drawContrast( true );

            if( cb->editable )
            {

                if( enabled && empty ) color = palette.color( QPalette::Disabled,  QPalette::Text );
                else {

                    // check animation state
                    const bool subControlHover( enabled && mouseOver && cb->activeSubControls&SC_ComboBoxArrow );
                    animations().comboBoxEngine().updateState( widget, AnimationHover, subControlHover  );

                    const bool animated( enabled && animations().comboBoxEngine().isAnimated( widget, AnimationHover ) );
                    const qreal opacity( animations().comboBoxEngine().opacity( widget, AnimationHover ) );

                    if( animated )
                    {

                        QColor highlight = helper().viewHoverBrush().brush( palette ).color();
                        color = KColorUtils::mix( palette.color( QPalette::Text ), highlight, opacity );

                    } else if( subControlHover ) {

                        color = helper().viewHoverBrush().brush( palette ).color();

                    } else {

                        color = palette.color( QPalette::Text );

                    }

                }

                background = palette.color( QPalette::Background );

                if( enabled ) drawContrast = false;

            } else {

                // foreground color
                const QPalette::ColorRole role( hasFrame ? QPalette::ButtonText : QPalette::WindowText );
                if( enabled && empty ) color = palette.color( QPalette::Disabled,  role );
                else color  = palette.color( role );

                // background color
                background = palette.color( hasFrame ? QPalette::Button : QPalette::Window );

            }

            // draw the arrow
            QRect arrowRect = comboBoxSubControlRect( option, SC_ComboBoxArrow, widget );

            const QPolygonF a( genericArrow( ArrowDown, ArrowNormal ) );
            const qreal penThickness = 1.6;

            painter->save();
            painter->translate( arrowRect.center() );
            painter->setRenderHint( QPainter::Antialiasing );

            if( drawContrast )
            {

                const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );
                painter->translate( 0,offset );
                painter->setPen( QPen( helper().calcLightColor( palette.color( QPalette::Window ) ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                painter->drawPolyline( a );
                painter->translate( 0,-offset );

            }

            painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            painter->drawPolyline( a );
            painter->restore();

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawDialComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        const State& flags( option->state );
        const bool enabled = flags & State_Enabled;
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( enabled && ( flags & State_HasFocus ) );
        const bool sunken( flags & ( State_On|State_Sunken ) );

        StyleOptions opts = 0;
        if( sunken ) opts |= Sunken;
        if( hasFocus ) opts |= Focus;
        if( mouseOver ) opts |= Hover;

        // mouseOver has precedence over focus
        animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        const QRect rect( option->rect );
        const QPalette &palette( option->palette );
        const QColor buttonColor( helper().backgroundColor( palette.color( QPalette::Button ), widget, rect.center() ) );

        if( enabled && animations().widgetStateEngine().isAnimated( widget, AnimationHover ) && !( opts & Sunken ) )
        {

            qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
            renderDialSlab( painter, rect, buttonColor, option, opts, opacity, AnimationHover );

        } else if( enabled && !mouseOver && animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) && !( opts & Sunken ) ) {

            qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );
            renderDialSlab( painter, rect, buttonColor, option, opts, opacity, AnimationFocus );

        } else {

            renderDialSlab( painter, rect, buttonColor, option, opts );

        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawGroupBoxComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>( option );
        if( groupBox && groupBox->features & QStyleOptionFrameV2::Flat )
        {

            // for flat groupboxes, the groupBox title is rendered bold
            /*
            TODO: talk to pinheiro. This is not an optimal design
            I would rather
            1/ keep the font unchanged
            2/ add an horizontal separator next to the title
            ( Hugo )
            */
            const QFont oldFont = painter->font();
            QFont font = oldFont;
            font.setBold( true );
            painter->setFont( font );
            QCommonStyle::drawComplexControl( CC_GroupBox, option, painter, widget );
            painter->setFont( oldFont );
            return true;

        } else return false;
    }

    //______________________________________________________________
    bool Style::drawQ3ListViewComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        const QStyleOptionQ3ListView* optListView( qstyleoption_cast<const QStyleOptionQ3ListView*>( option ) );
        if( !optListView ) return true;

        // this is copied from skulpture code
        // Copyright ( c ) 2007-2010 Christoph Feck <christoph@maxiom.de>
        if( optListView->subControls & QStyle::SC_Q3ListView )
        {
            painter->fillRect(
                optListView->rect,
                optListView->viewportPalette.brush( optListView->viewportBGRole ) );
        }

        if( optListView->subControls & QStyle::SC_Q3ListViewBranch )
        {

            QStyleOption opt = *static_cast<const QStyleOption*>( option );
            int y = optListView->rect.y();

            for ( int i = 1; i < optListView->items.size(); ++i )
            {
                QStyleOptionQ3ListViewItem item = optListView->items.at( i );
                if( y + item.totalHeight > 0 && y < optListView->rect.height() )
                {
                    opt.state = QStyle::State_Item;
                    if ( i + 1 < optListView->items.size() )
                    { opt.state |= QStyle::State_Sibling; }

                    if(
                        item.features & QStyleOptionQ3ListViewItem::Expandable
                        || ( item.childCount > 0 && item.height > 0 ) )
                    { opt.state |= QStyle::State_Children | ( item.state & QStyle::State_Open ); }

                    opt.rect = QRect( optListView->rect.left(), y, optListView->rect.width(), item.height );
                    drawIndicatorBranchPrimitive( &opt, painter, widget );

                    if( ( opt.state & QStyle::State_Sibling ) && item.height < item.totalHeight )
                    {
                        opt.state = QStyle::State_Sibling;
                        opt.rect = QRect(
                            optListView->rect.left(), y + item.height,
                            optListView->rect.width(), item.totalHeight - item.height );
                        drawIndicatorBranchPrimitive( &opt, painter, widget );
                    }
                }

                y += item.totalHeight;
            }
        }

        return true;

    }

    //______________________________________________________________
    bool Style::drawSliderComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionSlider *slider( qstyleoption_cast<const QStyleOptionSlider *>( option ) );
        if( !slider ) return true;

        const QPalette& palette( option->palette );
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( flags & State_HasFocus );

        if( slider->subControls & SC_SliderTickmarks ) { renderSliderTickmarks( painter, slider, widget ); }

        // groove
        if( slider->subControls & SC_SliderGroove )
        {
            const QRect groove = sliderSubControlRect( slider, SC_SliderGroove, widget );
            const Qt::Orientation orientation( groove.width() > groove.height() ? Qt::Horizontal : Qt::Vertical );
            if( groove.isValid() ) helper().scrollHole( palette.color( QPalette::Window ), orientation, true )->render( groove, painter, TileSet::Full );
        }

        // handle
        if ( slider->subControls & SC_SliderHandle )
        {
            const QRect handle = sliderSubControlRect( slider, SC_SliderHandle, widget );
            const QRect r = centerRect( handle, 21, 21 );

            const bool handleActive( slider->activeSubControls & SC_SliderHandle );
            StyleOptions opts( 0 );
            if( hasFocus ) opts |= Focus;
            if( handleActive && mouseOver ) opts |= Hover;

            animations().sliderEngine().updateState( widget, enabled && handleActive );
            const qreal opacity( animations().sliderEngine().opacity( widget ) );

            const QColor color( helper().backgroundColor( palette.color( QPalette::Button ), widget, handle.center() ) );
            const QColor glow( slabShadowColor( color, opts, opacity, AnimationHover ) );

            const bool sunken( flags & (State_On|State_Sunken) );
            painter->drawPixmap( r.topLeft(), helper().sliderSlab( color, glow, sunken, 0.0 ) );

        }

        return true;
    }

    //______________________________________________________________
    bool Style::drawSpinBoxComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>( option );
        if( !sb ) return true;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( flags & State_HasFocus );
        const QColor inputColor( palette.color( QPalette::Base ) );

        if( sb->subControls & SC_SpinBoxFrame )
        {

            QRect fr( r.adjusted( 1,1,-1,-1 ) );
            painter->save();
            painter->setRenderHint( QPainter::Antialiasing );
            painter->setPen( Qt::NoPen );
            painter->setBrush( inputColor );

            if( !sb->frame )
            {
                // frameless spinbox
                // frame is adjusted to have the same dimensions as a frameless editor
                painter->fillRect( r, inputColor );
                painter->restore();

            } else {

                // normal spinbox
                helper().fillHole( *painter, r.adjusted( 0, -1, 0, 0 ) );
                painter->restore();

                HoleOptions options( 0 );
                if( hasFocus && enabled ) options |= HoleFocus;
                if( mouseOver && enabled ) options |= HoleHover;

                QColor local( palette.color( QPalette::Window ) );
                animations().lineEditEngine().updateState( widget, AnimationHover, mouseOver );
                animations().lineEditEngine().updateState( widget, AnimationFocus, hasFocus );
                if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationFocus ) )
                {

                    helper().renderHole( painter, local, fr, options, animations().lineEditEngine().opacity( widget, AnimationFocus ), AnimationFocus, TileSet::Ring );

                } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationHover ) ) {

                    helper().renderHole( painter, local, fr, options, animations().lineEditEngine().opacity( widget, AnimationHover ), AnimationHover, TileSet::Ring );

                } else {

                    helper().renderHole( painter, local, fr, options );

                }

            }
        }

        if( sb->subControls & SC_SpinBoxUp ) renderSpinBoxArrow( painter, sb, widget, SC_SpinBoxUp );
        if( sb->subControls & SC_SpinBoxDown ) renderSpinBoxArrow( painter, sb, widget, SC_SpinBoxDown );

        return true;

    }

    //______________________________________________________________
    bool Style::drawTitleBarComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {
        const QStyleOptionTitleBar *tb( qstyleoption_cast<const QStyleOptionTitleBar *>( option ) );
        if( !tb ) return true;

        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool active( enabled && ( tb->titleBarState & Qt::WindowActive ) );

        // draw title text
        {
            QRect textRect = subControlRect( CC_TitleBar, tb, SC_TitleBarLabel, widget );

            // enable state transition
            animations().widgetEnabilityEngine().updateState( widget, AnimationEnable, active );

            // make sure palette has the correct color group
            QPalette palette( option->palette );

            if( animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
            { palette = helper().mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }

            palette.setCurrentColorGroup( active ? QPalette::Active: QPalette::Disabled );
            QCommonStyle::drawItemText( painter, textRect, Qt::AlignCenter, palette, active, tb->text, QPalette::WindowText );

        }


        // menu button
        if( ( tb->subControls & SC_TitleBarSysMenu ) && ( tb->titleBarFlags & Qt::WindowSystemMenuHint ) && !tb->icon.isNull() )
        {

            const QRect br = subControlRect( CC_TitleBar, tb, SC_TitleBarSysMenu, widget );
            tb->icon.paint( painter, br );

        }

        if( ( tb->subControls & SC_TitleBarMinButton ) && ( tb->titleBarFlags & Qt::WindowMinimizeButtonHint ) )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarMinButton ); }

        if( ( tb->subControls & SC_TitleBarMaxButton ) && ( tb->titleBarFlags & Qt::WindowMaximizeButtonHint ) )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarMaxButton ); }

        if( ( tb->subControls & SC_TitleBarCloseButton ) )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarCloseButton ); }

        if( ( tb->subControls & SC_TitleBarNormalButton ) &&
            ( ( ( tb->titleBarFlags & Qt::WindowMinimizeButtonHint ) &&
            ( tb->titleBarState & Qt::WindowMinimized ) ) ||
            ( ( tb->titleBarFlags & Qt::WindowMaximizeButtonHint ) &&
            ( tb->titleBarState & Qt::WindowMaximized ) ) ) )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarNormalButton ); }

        if( tb->subControls & SC_TitleBarShadeButton )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarShadeButton ); }

        if( tb->subControls & SC_TitleBarUnshadeButton )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarUnshadeButton ); }

        if( ( tb->subControls & SC_TitleBarContextHelpButton ) && ( tb->titleBarFlags & Qt::WindowContextHelpButtonHint ) )
        { renderTitleBarButton( painter, tb, widget, SC_TitleBarContextHelpButton ); }

        return true;
    }


    //______________________________________________________________
    bool Style::drawToolButtonComplexControl( const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget ) const
    {

        // check autoRaise state
        const State flags( option->state );
        const bool isInToolBar( widget && qobject_cast<QToolBar*>( widget->parent() ) );

        // get rect and palette
        const QRect& rect( option->rect );
        const QStyleOptionToolButton *tool( qstyleoption_cast<const QStyleOptionToolButton *>( option ) );
        if( !tool ) return true;

        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );
        const bool hasFocus( enabled && ( flags&State_HasFocus ) );
        const bool sunken( flags & ( State_Sunken|State_On ) );

        if( isInToolBar )
        {

            animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );

        } else {

            // mouseOver has precedence over focus
            animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
            animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

        }

        // toolbar animation
        QWidget* parent( widget ? widget->parentWidget():0 );
        const bool toolBarAnimated( isInToolBar && animations().toolBarEngine().isAnimated( parent ) );
        const QRect animatedRect( animations().toolBarEngine().animatedRect( parent ) );
        const QRect currentRect( animations().toolBarEngine().currentRect( parent ) );
        const bool current( isInToolBar && currentRect.intersects( rect.translated( widget->mapToParent( QPoint( 0,0 ) ) ) ) );
        const bool toolBarTimerActive( isInToolBar && animations().toolBarEngine().isTimerActive( widget->parentWidget() ) );

        // normal toolbutton animation
        const bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
        const bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );

        /* FIXME: this all logic is messy. The conditions to trigger the call to drawPrimitive can likely be simplified */

        // local copy of option
        QStyleOptionToolButton tOpt( *tool );
        tOpt.palette = option->palette;

        const QRect buttonRect( subControlRect( CC_ToolButton, tool, SC_ToolButton, widget ) );

        bool drawn(false);
        if( enabled && !( mouseOver || hasFocus || sunken ) )
        {

            if( hoverAnimated || ( focusAnimated && !hasFocus ) || ( ( ( toolBarAnimated && animatedRect.isNull() )||toolBarTimerActive ) && current ) )
            {
                tOpt.rect = buttonRect;
                tOpt.state = flags;
                drawPanelButtonToolPrimitive( &tOpt, painter, widget );
                drawn = true;
            }

        }

        // State_AutoRaise: only draw button when State_MouseOver
        State bflags = tool->state;
        if( bflags & State_AutoRaise && !( bflags & State_MouseOver ) )
        { bflags &= ~State_Raised; }

        tOpt.state = bflags;

        if( tool->subControls & SC_ToolButton && ( bflags & ( State_Sunken | State_On | State_Raised ) ) && !drawn )
        {
            tOpt.rect = buttonRect;
            drawPanelButtonToolPrimitive( &tOpt, painter, widget );
        }

        if( tool->subControls & SC_ToolButtonMenu )
        {

            tOpt.rect = subControlRect( CC_ToolButton, tool, SC_ToolButtonMenu, widget );
            painter->save();
            drawIndicatorButtonDropDownPrimitive( &tOpt, painter, widget );
            painter->restore();

        } else if( tool->features & QStyleOptionToolButton::HasMenu ) {

            // This is requesting KDE3-style arrow indicator, per Qt 4.4 behavior. Qt 4.3 prefers to hide
            // the fact of the menu's existence. Whee! Since we don't know how to paint this right,
            // though, we have to have some metrics set for it to look nice.
            const int size( ToolButton_InlineMenuIndicatorSize );
            if( size )
            {

                const int xOff( ToolButton_InlineMenuIndicatorXOff );
                const int yOff( ToolButton_InlineMenuIndicatorYOff );

                tOpt.rect = QRect( buttonRect.right() + xOff + 1, buttonRect.bottom() + yOff + 1, size, size );
                painter->save();
                drawIndicatorButtonDropDownPrimitive( &tOpt, painter, widget );
                painter->restore();

            }

        }

        // CE_ToolButtonLabel expects a readjusted rect, for the button area proper
        QStyleOptionToolButton labelOpt = *tool;
        labelOpt.rect = buttonRect;
        drawToolButtonLabelControl( &labelOpt, painter, widget );

        return true;

    }

    //_____________________________________________________________________
    void Style::oxygenConfigurationChanged( void )
    {

        // reset helper configuration
        helper().reloadConfig();

        // background pixmap
        helper().setBackgroundPixmap( StyleConfigData::backgroundPixmap() );

        // reset config
        StyleConfigData::self()->readConfig();

        // update caches size
        int cacheSize( StyleConfigData::cacheEnabled() ?
            StyleConfigData::maxCacheSize():0 );

        helper().setMaxCacheSize( cacheSize );

        // reinitialize engines
        animations().setupEngines();
        transitions().setupEngines();
        windowManager().initialize();
        shadowHelper().reloadConfig();

        // widget explorer
        widgetExplorer().setEnabled( StyleConfigData::widgetExplorerEnabled() );
        widgetExplorer().setDrawWidgetRects( StyleConfigData::drawWidgetRects() );

        // scrollbar button dimentions.
        /* it has to be reinitialized here because scrollbar width might have changed */
        _noButtonHeight = 0;
        _singleButtonHeight = qMax( StyleConfigData::scrollBarWidth() * 7 / 10, 14 );
        _doubleButtonHeight = 2*_singleButtonHeight;

        _showMnemonics = StyleConfigData::showMnemonics();

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

        // tabbar shape
        switch( StyleConfigData::tabStyle() )
        {
            case StyleConfigData::TS_PLAIN:
            _tabBarTabShapeControl = &Style::drawTabBarTabShapeControl_Plain;
            break;

            default:
            case StyleConfigData::TS_SINGLE:
            _tabBarTabShapeControl = &Style::drawTabBarTabShapeControl_Single;
            break;
        }

        // frame focus
        if( StyleConfigData::viewDrawFocusIndicator() ) _frameFocusPrimitive = &Style::drawFrameFocusRectPrimitive;
        else _frameFocusPrimitive = &Style::emptyPrimitive;

    }

    //_____________________________________________________________________
    void Style::globalPaletteChanged( void )
    {
        helper().reloadConfig();
        helper().invalidateCaches();
    }

    //____________________________________________________________________
    QIcon Style::standardIconImplementation(
        StandardPixmap standardIcon,
        const QStyleOption *option,
        const QWidget *widget ) const
    {

        switch( standardIcon )
        {

            // copied from kstyle
            case SP_DesktopIcon: return KIcon( "user-desktop" );
            case SP_TrashIcon: return KIcon( "user-trash" );
            case SP_ComputerIcon: return KIcon( "computer" );
            case SP_DriveFDIcon: return KIcon( "media-floppy" );
            case SP_DriveHDIcon: return KIcon( "drive-harddisk" );
            case SP_DriveCDIcon: return KIcon( "drive-optical" );
            case SP_DriveDVDIcon: return KIcon( "drive-optical" );
            case SP_DriveNetIcon: return KIcon( "folder-remote" );
            case SP_DirHomeIcon: return KIcon( "user-home" );
            case SP_DirOpenIcon: return KIcon( "document-open-folder" );
            case SP_DirClosedIcon: return KIcon( "folder" );
            case SP_DirIcon: return KIcon( "folder" );

            //TODO: generate ( !? ) folder with link emblem
            case SP_DirLinkIcon: return KIcon( "folder" );

            //TODO: look for a better icon
            case SP_FileIcon: return KIcon( "text-plain" );

            //TODO: generate ( !? ) file with link emblem
            case SP_FileLinkIcon: return KIcon( "text-plain" );

            //TODO: find correct icon
            case SP_FileDialogStart: return KIcon( "media-playback-start" );

            //TODO: find correct icon
            case SP_FileDialogEnd: return KIcon( "media-playback-stop" );

            case SP_FileDialogToParent: return KIcon( "go-up" );
            case SP_FileDialogNewFolder: return KIcon( "folder-new" );
            case SP_FileDialogDetailedView: return KIcon( "view-list-details" );
            case SP_FileDialogInfoView: return KIcon( "document-properties" );
            case SP_FileDialogContentsView: return KIcon( "view-list-icons" );
            case SP_FileDialogListView: return KIcon( "view-list-text" );
            case SP_FileDialogBack: return KIcon( "go-previous" );
            case SP_MessageBoxInformation: return KIcon( "dialog-information" );
            case SP_MessageBoxWarning: return KIcon( "dialog-warning" );
            case SP_MessageBoxCritical: return KIcon( "dialog-error" );
            case SP_MessageBoxQuestion: return KIcon( "dialog-information" );
            case SP_DialogOkButton: return KIcon( "dialog-ok" );
            case SP_DialogCancelButton: return KIcon( "dialog-cancel" );
            case SP_DialogHelpButton: return KIcon( "help-contents" );
            case SP_DialogOpenButton: return KIcon( "document-open" );
            case SP_DialogSaveButton: return KIcon( "document-save" );
            case SP_DialogCloseButton: return KIcon( "dialog-close" );
            case SP_DialogApplyButton: return KIcon( "dialog-ok-apply" );
            case SP_DialogResetButton: return KIcon( "document-revert" );
            case SP_DialogDiscardButton: return KIcon( "dialog-cancel" );
            case SP_DialogYesButton: return KIcon( "dialog-ok-apply" );
            case SP_DialogNoButton: return KIcon( "dialog-cancel" );
            case SP_ArrowUp: return KIcon( "go-up" );
            case SP_ArrowDown: return KIcon( "go-down" );
            case SP_ArrowLeft: return KIcon( "go-previous-view" );
            case SP_ArrowRight: return KIcon( "go-next-view" );
            case SP_ArrowBack: return KIcon( "go-previous" );
            case SP_ArrowForward: return KIcon( "go-next" );
            case SP_BrowserReload: return KIcon( "view-refresh" );
            case SP_BrowserStop: return KIcon( "process-stop" );
            case SP_MediaPlay: return KIcon( "media-playback-start" );
            case SP_MediaStop: return KIcon( "media-playback-stop" );
            case SP_MediaPause: return KIcon( "media-playback-pause" );
            case SP_MediaSkipForward: return KIcon( "media-skip-forward" );
            case SP_MediaSkipBackward: return KIcon( "media-skip-backward" );
            case SP_MediaSeekForward: return KIcon( "media-seek-forward" );
            case SP_MediaSeekBackward: return KIcon( "media-seek-backward" );
            case SP_MediaVolume: return KIcon( "audio-volume-medium" );
            case SP_MediaVolumeMuted: return KIcon( "audio-volume-muted" );

            default: break;

        }

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
            buttonColor = qApp->palette().window().color();
            iconColor   = qApp->palette().windowText().color();

        } else {

            // KCS is always safe
            buttonColor = KColorScheme( QPalette::Active, KColorScheme::Window, helper().config() ).background().color();
            iconColor   = KColorScheme( QPalette::Active, KColorScheme::Window, helper().config() ).foreground().color();

        }

        switch( standardIcon )
        {

            case SP_TitleBarNormalButton:
            {
                QPixmap realpm( pixelMetric( QStyle::PM_SmallIconSize,0,0 ), pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );
                realpm.fill( Qt::transparent );
                QPixmap pm = helper().windecoButton( buttonColor, false, 15 );
                QPainter painter( &realpm );
                painter.drawPixmap( 1,1,pm );
                painter.setRenderHints( QPainter::Antialiasing );

                // should use the same icons as in the deco
                QPointF points[4] = {QPointF( 8.5, 6 ), QPointF( 11, 8.5 ), QPointF( 8.5, 11 ), QPointF( 6, 8.5 )};
                {

                    const qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( helper().calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolygon( points, 4 );
                }

                {
                    const qreal width( 1.1 );
                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolygon( points, 4 );
                }
                painter.end();

                return QIcon( realpm );
            }

            case SP_TitleBarShadeButton:
            {
                QPixmap realpm( pixelMetric( QStyle::PM_SmallIconSize,0,0 ), pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );
                realpm.fill( Qt::transparent );
                QPixmap pm = helper().windecoButton( buttonColor, false, 15 );
                QPainter painter( &realpm );
                painter.drawPixmap( 1,1,pm );
                painter.setRenderHints( QPainter::Antialiasing );
                {

                    qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( helper().calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( QPointF( 6.5,6.5 ), QPointF( 8.75,8.75 ) );
                    painter.drawLine( QPointF( 8.75,8.75 ), QPointF( 11.0,6.5 ) );
                    painter.drawLine( QPointF( 6.5,11.0 ), QPointF( 11.0,11.0 ) );
                }

                {
                    qreal width( 1.1 );
                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( QPointF( 6.5,6.5 ), QPointF( 8.75,8.75 ) );
                    painter.drawLine( QPointF( 8.75,8.75 ), QPointF( 11.0,6.5 ) );
                    painter.drawLine( QPointF( 6.5,11.0 ), QPointF( 11.0,11.0 ) );
                }

                painter.end();

                return QIcon( realpm );
            }

            case SP_TitleBarUnshadeButton:
            {
                QPixmap realpm( pixelMetric( QStyle::PM_SmallIconSize,0,0 ), pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );
                realpm.fill( Qt::transparent );
                QPixmap pm = helper().windecoButton( buttonColor, false, 15 );
                QPainter painter( &realpm );
                painter.drawPixmap( 1,1,pm );
                painter.setRenderHints( QPainter::Antialiasing );

                {

                    qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( helper().calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( QPointF( 6.5,8.75 ), QPointF( 8.75,6.5 ) );
                    painter.drawLine( QPointF( 8.75,6.5 ), QPointF( 11.0,8.75 ) );
                    painter.drawLine( QPointF( 6.5,11.0 ), QPointF( 11.0,11.0 ) );
                }

                {
                    qreal width( 1.1 );
                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( QPointF( 6.5,8.75 ), QPointF( 8.75,6.5 ) );
                    painter.drawLine( QPointF( 8.75,6.5 ), QPointF( 11.0,8.75 ) );
                    painter.drawLine( QPointF( 6.5,11.0 ), QPointF( 11.0,11.0 ) );
                }
                painter.end();

                return QIcon( realpm );
            }

            case SP_TitleBarCloseButton:
            case SP_DockWidgetCloseButton:
            {
                QPixmap realpm( pixelMetric( QStyle::PM_SmallIconSize,0,0 ), pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );
                realpm.fill( Qt::transparent );
                QPixmap pm = helper().windecoButton( buttonColor, false, 15 );
                QPainter painter( &realpm );
                painter.drawPixmap( 1,1,pm );
                painter.setRenderHints( QPainter::Antialiasing );
                painter.setBrush( Qt::NoBrush );
                {

                    qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( helper().calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( QPointF( 6.5,6.5 ), QPointF( 11.0,11.0 ) );
                    painter.drawLine( QPointF( 11.0,6.5 ), QPointF( 6.5,11.0 ) );
                }

                {
                    qreal width( 1.1 );
                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawLine( QPointF( 6.5,6.5 ), QPointF( 11.0,11.0 ) );
                    painter.drawLine( QPointF( 11.0,6.5 ), QPointF( 6.5,11.0 ) );
                }

                painter.end();

                return QIcon( realpm );
            }

            case SP_ToolBarHorizontalExtensionButton:
            {

                QPixmap realpm( pixelMetric( QStyle::PM_SmallIconSize,0,0 ), pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );
                realpm.fill( Qt::transparent );
                QPainter painter( &realpm );
                painter.setRenderHints( QPainter::Antialiasing );
                painter.setBrush( Qt::NoBrush );

                painter.translate( qreal( realpm.width() )/2.0, qreal( realpm.height() )/2.0 );

                const bool reverseLayout( option && option->direction == Qt::RightToLeft );
                QPolygonF a = genericArrow( reverseLayout ? ArrowLeft:ArrowRight, ArrowTiny );
                {
                    qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( helper().calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( a );
                }

                {
                    qreal width( 1.1 );
                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( a );
                }

                return QIcon( realpm );
            }

            case SP_ToolBarVerticalExtensionButton:
            {
                QPixmap realpm( pixelMetric( QStyle::PM_SmallIconSize,0,0 ), pixelMetric( QStyle::PM_SmallIconSize,0,0 ) );
                realpm.fill( Qt::transparent );
                QPainter painter( &realpm );
                painter.setRenderHints( QPainter::Antialiasing );
                painter.setBrush( Qt::NoBrush );

                QPolygonF a = genericArrow( ArrowDown, ArrowTiny );
                {
                    qreal width( 1.1 );
                    painter.translate( 0, 0.5 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( helper().calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( a );
                }

                {
                    qreal width( 1.1 );
                    painter.translate( 0,-1 );
                    painter.setBrush( Qt::NoBrush );
                    painter.setPen( QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    painter.drawPolyline( a );
                }

                return QIcon( realpm );
            }

            default:
            return QCommonStyle::standardIconImplementation( standardIcon, option, widget );
        }
    }

    //_____________________________________________________________
    void Style::initializeKGlobalSettings( void )
    {

        if( qApp && !qApp->inherits( "KApplication" ) )
        {
            /*
            for Qt, non-KDE applications, needs to explicitely activate KGlobalSettings.
            On the other hand, it is done internally in kApplication constructor,
            so no need to duplicate here.
            */
            KGlobalSettings::self()->activate( KGlobalSettings::ListenForChanges );
        }

        // connect palette changes to local slot, to make sure caches are cleared
        connect( KGlobalSettings::self(), SIGNAL(kdisplayPaletteChanged()), this, SLOT(globalPaletteChanged()) );

        // update flag
        _kGlobalSettingsInitialized = true;

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

    //_________________________________________________________________________________
    void Style::renderDialSlab( QPainter *painter, const QRect& r, const QColor &color, const QStyleOption *option, StyleOptions opts, qreal opacity, AnimationMode mode ) const
    {

        // cast option
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return;

        // adjust rect to be square, and centered
        const int dimension( qMin( r.width(), r.height() ) );
        const QRect rect( centerRect( r, dimension, dimension ) );

        // calculate glow color
        const QColor glow( slabShadowColor( color, opts, opacity, mode ) );

        // get main slab
        QPixmap pix( helper().dialSlab( color, glow, 0.0, dimension ) );
        const qreal baseOffset( 3.5 );

        const QColor light( helper().calcLightColor( color ) );
        const QColor shadow( helper().calcShadowColor( color ) );

        QPainter p( &pix );
        p.setPen( Qt::NoPen );
        p.setRenderHints( QPainter::Antialiasing );

        // indicator
        qreal angle( 0 );
        if( sliderOption->maximum == sliderOption->minimum ) angle = M_PI / 2;
        else {

            qreal fraction( qreal( sliderOption->sliderPosition - sliderOption->minimum )/qreal( sliderOption->maximum - sliderOption->minimum ) );
            if( !sliderOption->upsideDown ) fraction = 1.0 - fraction;

            if( sliderOption->dialWrapping ) angle = 1.5*M_PI - fraction*2*M_PI;
            else  angle = ( M_PI*8 - fraction*10*M_PI )/6;
        }

        QPointF center( pix.rect().center() );
        const int sliderWidth( dimension/6 );
        const qreal radius( 0.5*( dimension - 2*sliderWidth ) );
        center += QPointF( radius*cos( angle ), -radius*sin( angle ) );

        QRectF sliderRect( 0, 0, sliderWidth, sliderWidth );
        sliderRect.moveCenter( center );

        // outline circle
        const qreal offset( 0.3 );
        QLinearGradient lg( 0, baseOffset, 0, baseOffset + 2*sliderRect.height() );
        p.setBrush( light );
        p.setPen( Qt::NoPen );
        p.drawEllipse( sliderRect.translated( 0, offset ) );

        // mask
        p.setPen( Qt::NoPen );
        p.save();
        p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
        p.setBrush( QBrush( Qt::black ) );
        p.drawEllipse( sliderRect );
        p.restore();

        // shadow
        p.translate( sliderRect.topLeft() );
        helper().drawInverseShadow( p, shadow.darker( 200 ), 0.0, sliderRect.width(), 0.0 );

        // glow
        if( glow.isValid() ) helper().drawInverseGlow( p, glow, 0.0, sliderRect.width(),  sliderRect.width() );

        p.end();

        painter->drawPixmap( rect.topLeft(), pix );

        return;

    }

    //____________________________________________________________________________________
    void Style::renderButtonSlab( QPainter *painter, QRect r, const QColor &color, StyleOptions options, qreal opacity,
        AnimationMode mode,
        TileSet::Tiles tiles ) const
    {
        if( ( r.width() <= 0 ) || ( r.height() <= 0 ) ) return;

        r.translate( 0,-1 );
        if( !painter->clipRegion().isEmpty() ) painter->setClipRegion( painter->clipRegion().translated( 0,-1 ) );

        // fill
        if( !( options & NoFill ) ) helper().fillButtonSlab( *painter, r, color, options&Sunken );

        // edges
        // for slabs, hover takes precedence over focus ( other way around for holes )
        // but in any case if the button is sunken we don't show focus nor hover
        TileSet *tile(0L);
        if( options & Sunken )
        {
            tile = helper().slabSunken( color );

        } else {

            QColor glow = slabShadowColor( color, options, opacity, mode );
            tile = helper().slab( color, glow, 0.0 );

        }

        if( tile )
        { tile->render( r, painter, tiles ); }

    }

    //____________________________________________________________________________________
    void Style::renderSlab(
        QPainter *painter, QRect r,
        const QColor &color,
        StyleOptions options, qreal opacity,
        AnimationMode mode,
        TileSet::Tiles tiles ) const
    {

        // check rect
        if( !r.isValid() ) return;

        // this is needed for button vertical alignment
        r.translate( 0,-1 );
        if( !painter->clipRegion().isEmpty() ) painter->setClipRegion( painter->clipRegion().translated( 0,-1 ) );

        // additional adjustment for sunken frames
        if( options & Sunken ) r.adjust( -1,0,1,2 );

        // fill
        if( !( options & NoFill ) )
        {
            painter->save();
            painter->setRenderHint( QPainter::Antialiasing );
            painter->setPen( Qt::NoPen );

            if( helper().calcShadowColor( color ).value() > color.value() && ( options & Sunken ) )
            {

                QLinearGradient innerGradient( 0, r.top(), 0, r.bottom() + r.height() );
                innerGradient.setColorAt( 0.0, color );
                innerGradient.setColorAt( 1.0, helper().calcLightColor( color ) );
                painter->setBrush( innerGradient );

            } else {

                QLinearGradient innerGradient( 0, r.top() - r.height(), 0, r.bottom() );
                innerGradient.setColorAt( 0.0, helper().calcLightColor( color ) );
                innerGradient.setColorAt( 1.0, color );
                painter->setBrush( innerGradient );

            }

            helper().fillSlab( *painter, r );

            painter->restore();
        }

        // edges
        // for slabs, hover takes precedence over focus ( other way around for holes )
        // but in any case if the button is sunken we don't show focus nor hover
        TileSet *tile( 0 );
        if( ( options & Sunken ) && color.isValid() )
        {
            tile = helper().slabSunken( color );

        } else {

            // calculate proper glow color based on current settings and opacity
            const QColor glow( slabShadowColor( color, options, opacity, mode ) );
            if( color.isValid() || glow.isValid() ) tile = helper().slab( color, glow , 0.0 );
            else return;

        }

        // render tileset
        if( tile ) tile->render( r, painter, tiles );

    }

    //______________________________________________________________________________________________________________________________
    void Style::fillTabBackground( QPainter* painter, const QRect &r, const QColor &color, QTabBar::Shape shape, const QWidget* widget ) const
    {

        // filling
        QRect fillRect( r );
        switch( shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            fillRect.adjust( 4, 4, -4, -6 );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            fillRect.adjust( 4, 4, -4, -4 );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            fillRect.adjust( 4, 3, -5, -5 );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            fillRect.adjust( 5, 3, -4, -5 );
            break;

            default: return;

        }

        if( widget ) helper().renderWindowBackground( painter, fillRect, widget, color );
        else painter->fillRect( fillRect, color );

    }

    //______________________________________________________________________________________________________________________________
    void Style::fillTab( QPainter* painter, const QRect &r, const QColor &color, QTabBar::Shape shape, bool active ) const
    {

        const QColor dark( helper().calcDarkColor( color ) );
        const QColor shadow( helper().calcShadowColor( color ) );
        const QColor light( helper().calcLightColor( color ) );
        const QRect fillRect( r.adjusted( 4, 3,-4,-5 ) );

        QLinearGradient highlight;
        switch( shape )
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            highlight = QLinearGradient( fillRect.topLeft(), fillRect.bottomLeft() );
            break;

            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
            highlight = QLinearGradient( fillRect.bottomLeft(), fillRect.topLeft() );
            break;

            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
            highlight = QLinearGradient( fillRect.topRight(), fillRect.topLeft() );
            break;

            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
            highlight = QLinearGradient( fillRect.topLeft(), fillRect.topRight() );
            break;

            default: return;

        }

        if( active ) {

            highlight.setColorAt( 0.0, helper().alphaColor( light, 0.5 ) );
            highlight.setColorAt( 0.1, helper().alphaColor( light, 0.5 ) );
            highlight.setColorAt( 0.25, helper().alphaColor( light, 0.3 ) );
            highlight.setColorAt( 0.5, helper().alphaColor( light, 0.2 ) );
            highlight.setColorAt( 0.75, helper().alphaColor( light, 0.1 ) );
            highlight.setColorAt( 0.9, Qt::transparent );

        } else {

            // inactive
            highlight.setColorAt( 0.0, helper().alphaColor( light, 0.1 ) );
            highlight.setColorAt( 0.4, helper().alphaColor( dark, 0.5 ) );
            highlight.setColorAt( 0.8, helper().alphaColor( dark, 0.4 ) );
            highlight.setColorAt( 0.9, Qt::transparent );

        }

        painter->setRenderHints( QPainter::Antialiasing );
        painter->setPen( Qt::NoPen );

        painter->setBrush( highlight );
        painter->drawRoundedRect( fillRect, 2, 2 );

    }

    //____________________________________________________________________________________________________
    void Style::renderSpinBoxArrow( QPainter* painter, const QStyleOptionSpinBox* option, const QWidget* widget, const SubControl& subControl ) const
    {

        const QPalette& palette( option->palette );

        const State& flags( option->state );

        bool enabled( flags & State_Enabled );
        bool atLimit( false );
        if( enabled )
        {

            if( const QSpinBox* spinbox = qobject_cast<const QSpinBox*>( widget ) )
            {

                // cast to spinbox and check if at limit
                const int value( spinbox->value() );
                if( !spinbox->wrapping() && (( subControl == SC_SpinBoxUp && value == spinbox->maximum() ) ||
                    ( subControl == SC_SpinBoxDown && value == spinbox->minimum() ) ) )
                    { atLimit = true; }

            } else if( const QDoubleSpinBox* spinbox = qobject_cast<const QDoubleSpinBox*>( widget ) ) {

                // cast to spinbox and check if at limit
                const double value( spinbox->value() );
                if( !spinbox->wrapping() && (( subControl == SC_SpinBoxUp && value == spinbox->maximum() ) ||
                    ( subControl == SC_SpinBoxDown && value == spinbox->minimum() ) ) )
                    { atLimit = true; }

            }

        }

        enabled &= !atLimit;
        const bool mouseOver( enabled && ( flags & State_MouseOver ) );

        // check animation state
        const bool subControlHover( enabled && mouseOver && ( option->activeSubControls & subControl ) );
        animations().spinBoxEngine().updateState( widget, subControl, subControlHover );

        const bool animated( enabled && animations().spinBoxEngine().isAnimated( widget, subControl ) );
        const qreal opacity( animations().spinBoxEngine().opacity( widget, subControl ) );

        QColor color;
        if( animated )
        {

            QColor highlight = helper().viewHoverBrush().brush( palette ).color();
            color = KColorUtils::mix( palette.color( QPalette::Text ), highlight, opacity );

        } else if( subControlHover ) {

            color = helper().viewHoverBrush().brush( palette ).color();

        } else if( atLimit ) {

            color = palette.color( QPalette::Disabled, QPalette::Text );

        } else {

            color = palette.color( QPalette::Text );

        }

        const qreal penThickness = 1.6;
        const QColor background = palette.color( QPalette::Background );

        const QPolygonF a( genericArrow( ( subControl == SC_SpinBoxUp ) ? ArrowUp:ArrowDown, ArrowNormal ) );
        const QRect arrowRect( subControlRect( CC_SpinBox, option, subControl, widget ) );

        painter->save();
        painter->translate( arrowRect.center() );
        painter->setRenderHint( QPainter::Antialiasing );

        painter->setPen( QPen( helper().decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );
        painter->restore();

        return;

    }

    //___________________________________________________________________________________
    void Style::renderSplitter( const QStyleOption* option, QPainter* painter, const QWidget* widget, bool horizontal ) const
    {

        const QPalette& palette( option->palette );
        const QRect& r( option->rect );
        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool mouseOver( enabled && ( flags & ( State_MouseOver|State_Sunken ) ) );

        // get orientation
        const Qt::Orientation orientation( horizontal ? Qt::Horizontal : Qt::Vertical );

        bool animated( false );
        qreal opacity( AnimationData::OpacityInvalid );

        if( enabled )
        {
            if( qobject_cast<const QMainWindow*>( widget ) )
            {

                animations().dockSeparatorEngine().updateRect( widget, r, orientation, mouseOver );
                animated = animations().dockSeparatorEngine().isAnimated( widget, r, orientation );
                opacity = animated ? animations().dockSeparatorEngine().opacity( widget, orientation ) : AnimationData::OpacityInvalid;

            } else if(  QPaintDevice* device = painter->device() ) {

                /*
                try update QSplitterHandle using painter device, because Qt passes
                QSplitter as the widget to the QStyle primitive.
                */
                animations().splitterEngine().updateState( device, mouseOver );
                animated = animations().splitterEngine().isAnimated( device );
                opacity = animations().splitterEngine().opacity( device );

            }
        }

        // get base color
        const QColor color = palette.color( QPalette::Background );

        if( horizontal )
        {
            const int h = r.height();

            if( animated || mouseOver )
            {
                const QColor highlight = helper().alphaColor( helper().calcLightColor( color ),0.5*( animated ? opacity:1.0 ) );
                const qreal a( r.height() > 30 ? 10.0/r.height():0.1 );
                QLinearGradient lg( 0, r.top(), 0, r.bottom() );
                lg.setColorAt( 0, Qt::transparent );
                lg.setColorAt( a, highlight );
                lg.setColorAt( 1.0-a, highlight );
                lg.setColorAt( 1, Qt::transparent );
                painter->fillRect( r, lg );
            }

            const int ngroups( qMax( 1,h / 250 ) );
            int center( ( h - ( ngroups-1 ) * 250 ) /2 + r.top() );
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                helper().renderDot( painter, QPoint( r.left()+1, center-3 ), color );
                helper().renderDot( painter, QPoint( r.left()+1, center ), color );
                helper().renderDot( painter, QPoint( r.left()+1, center+3 ), color );
            }

        } else {

            const int w( r.width() );
            if( animated || mouseOver )
            {
                const QColor highlight( helper().alphaColor( helper().calcLightColor( color ),0.5*( animated ? opacity:1.0 ) ) );
                const qreal a( r.width() > 30 ? 10.0/r.width():0.1 );
                QLinearGradient lg( r.left(), 0, r.right(), 0 );
                lg.setColorAt( 0, Qt::transparent );
                lg.setColorAt( a, highlight );
                lg.setColorAt( 1.0-a, highlight );
                lg.setColorAt( 1, Qt::transparent );
                painter->fillRect( r, lg );

            }

            const int ngroups( qMax( 1, w / 250 ) );
            int center = ( w - ( ngroups-1 ) * 250 ) /2 + r.left();
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                helper().renderDot( painter, QPoint( center-3, r.top()+1 ), color );
                helper().renderDot( painter, QPoint( center, r.top()+1 ), color );
                helper().renderDot( painter, QPoint( center+3, r.top()+1 ), color );
            }

        }

    }

    //____________________________________________________________________________________________________
    void Style::renderTitleBarButton( QPainter* painter, const QStyleOptionTitleBar* option, const QWidget* widget, const SubControl& subControl ) const
    {

        const QRect r = subControlRect( CC_TitleBar, option, subControl, widget );
        if( !r.isValid() ) return;

        QPalette palette = option->palette;

        painter->save();
        painter->drawPixmap( r, helper().windecoButton( palette.window().color(), true, r.height() ) );
        painter->setRenderHints( QPainter::Antialiasing );
        painter->setBrush( Qt::NoBrush );

        const State& flags( option->state );
        const bool enabled( flags & State_Enabled );
        const bool active( enabled && ( option->titleBarState & Qt::WindowActive ) );

        // enable state transition
        animations().widgetEnabilityEngine().updateState( widget, AnimationEnable, active );
        if( animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
        { palette = helper().mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }

        const bool sunken( flags&State_Sunken );
        const bool mouseOver( ( !sunken ) && widget && r.translated( widget->mapToGlobal( QPoint( 0,0 ) ) ).contains( QCursor::pos() ) );

        animations().mdiWindowEngine().updateState( widget, subControl, enabled && mouseOver );
        const bool animated( enabled && animations().mdiWindowEngine().isAnimated( widget, subControl ) );
        const qreal opacity( animations().mdiWindowEngine().opacity( widget, subControl ) );

        {

            // contrast pixel
            const QColor contrast = helper().calcLightColor( option->palette.color( QPalette::Active, QPalette::Window ) );
            const qreal width( 1.1 );
            painter->translate( 0, 0.5 );
            painter->setPen( QPen( contrast, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            renderTitleBarIcon( painter, QRectF( r ).adjusted( -2.5,-2.5,0,0 ), subControl );

        }

        {

            // button color
            QColor color;
            if( animated )
            {

                const QColor base( palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText ) );
                const QColor glow( subControl == SC_TitleBarCloseButton ?
                    helper().viewNegativeTextBrush().brush( palette ).color():
                    helper().viewHoverBrush().brush( palette ).color() );

                color = KColorUtils::mix( base, glow, opacity );

            } else if( mouseOver ) {

                color = ( subControl == SC_TitleBarCloseButton ) ?
                    helper().viewNegativeTextBrush().brush( palette ).color():
                    helper().viewHoverBrush().brush( palette ).color();

            } else {

                color = palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText );

            }

            // main icon painting
            const qreal width( 1.1 );
            painter->translate( 0,-1 );
            painter->setPen( QPen( color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
            renderTitleBarIcon( painter, QRectF( r ).adjusted( -2.5,-2.5,0,0 ), subControl );

        }

        painter->restore();

    }

    //____________________________________________________________________________________
    void Style::renderTitleBarIcon( QPainter *painter, const QRectF &r, const SubControl& subControl ) const
    {

        painter->save();
        painter->translate( r.topLeft() );
        switch( subControl )
        {
            case SC_TitleBarContextHelpButton:
            {
                painter->translate( 1.5, 1.5 );
                painter->drawArc( 7,5,4,4,135*16, -180*16 );
                painter->drawArc( 9,8,4,4,135*16,45*16 );
                painter->drawPoint( 9,12 );
                break;
            }
            case SC_TitleBarMinButton:
            {
                painter->drawLine( QPointF( 7.5, 9.5 ), QPointF( 10.5,12.5 ) );
                painter->drawLine( QPointF( 10.5,12.5 ), QPointF( 13.5, 9.5 ) );
                break;
            }
            case SC_TitleBarNormalButton:
            {
                painter->translate( 1.5, 1.5 );
                QPoint points[4] = {QPoint( 9, 6 ), QPoint( 12, 9 ), QPoint( 9, 12 ), QPoint( 6, 9 )};
                painter->drawPolygon( points, 4 );
                break;
            }
            case SC_TitleBarMaxButton:
            {
                painter->drawLine( QPointF( 7.5,11.5 ), QPointF( 10.5, 8.5 ) );
                painter->drawLine( QPointF( 10.5, 8.5 ), QPointF( 13.5,11.5 ) );
                break;
            }
            case SC_TitleBarCloseButton:
            {
                painter->drawLine( QPointF( 7.5,7.5 ), QPointF( 13.5,13.5 ) );
                painter->drawLine( QPointF( 13.5,7.5 ), QPointF( 7.5,13.5 ) );
                break;
            }
            case SC_TitleBarShadeButton:
            {
                painter->drawLine( QPointF( 7.5, 13.5 ), QPointF( 13.5, 13.5 ) );
                painter->drawLine( QPointF( 7.5, 7.5 ), QPointF( 10.5,10.5 ) );
                painter->drawLine( QPointF( 10.5,10.5 ), QPointF( 13.5, 7.5 ) );
                break;
            }
            case SC_TitleBarUnshadeButton:
            {
                painter->drawLine( QPointF( 7.5,10.5 ), QPointF( 10.5, 7.5 ) );
                painter->drawLine( QPointF( 10.5, 7.5 ), QPointF( 13.5,10.5 ) );
                painter->drawLine( QPointF( 7.5,13.0 ), QPointF( 13.5,13.0 ) );
                break;
            }
            default:
            break;
        }
        painter->restore();
    }

    //__________________________________________________________________________
    void Style::renderHeaderBackground( const QRect& r, const QPalette& palette, QPainter* painter, const QWidget* widget, bool horizontal, bool reverse ) const
    {

        // use window background for the background
        if( widget ) helper().renderWindowBackground( painter, r, widget, palette );
        else painter->fillRect( r, palette.color( QPalette::Window ) );

        if( horizontal ) renderHeaderLines( r, palette, painter, TileSet::Bottom );
        else if( reverse ) renderHeaderLines( r, palette, painter, TileSet::Left );
        else renderHeaderLines( r, palette, painter, TileSet::Right );

    }

    //__________________________________________________________________________
    void Style::renderHeaderLines( const QRect& r, const QPalette& palette, QPainter* painter, TileSet::Tiles tiles ) const
    {

        // add horizontal lines
        const QColor color( palette.color( QPalette::Window ) );
        const QColor dark( helper().calcDarkColor( color ) );
        const QColor light( helper().calcLightColor( color ) );

        painter->save();
        QRect rect( r );
        if( tiles & TileSet::Bottom  )
        {

            painter->setPen( dark );
            if( tiles & TileSet::Left ) painter->drawPoint( rect.bottomLeft() );
            else if( tiles& TileSet::Right ) painter->drawPoint( rect.bottomRight() );
            else painter->drawLine( rect.bottomLeft(), rect.bottomRight() );

            rect.adjust( 0,0,0,-1 );
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

            rect.adjust( 1,0,0,0 );
            painter->setPen( light );
            painter->drawLine( rect.topLeft(), rect.bottomLeft() );

        } else if( tiles & TileSet::Right ) {

            painter->setPen( dark );
            painter->drawLine( rect.topRight(), rect.bottomRight() );

            rect.adjust( 0,0,-1,0 );
            painter->setPen( light );
            painter->drawLine( rect.topRight(), rect.bottomRight() );

        }

        painter->restore();

        return;

    }

    //__________________________________________________________________________
    void Style::renderMenuItemBackground( const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
    {
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        const QRect animatedRect( animations().menuEngine().animatedRect( widget ) );
        if( !animatedRect.isNull() )
        {

            if( animatedRect.intersects( r ) )
            {
                const QColor color( helper().menuBackgroundColor( helper().calcMidColor( palette.color( QPalette::Window ) ), widget, animatedRect.center() ) );
                renderMenuItemRect( option, animatedRect, color, palette, painter );
            }

        } else if( animations().menuEngine().isTimerActive( widget ) ) {

            const QRect previousRect( animations().menuEngine().currentRect( widget, Previous ) );
            if( previousRect.intersects( r ) )
            {

                const QColor color( helper().menuBackgroundColor( helper().calcMidColor( palette.color( QPalette::Window ) ), widget, previousRect.center() ) );
                renderMenuItemRect( option, previousRect, color, palette, painter );
            }

        } else if( animations().menuEngine().isAnimated( widget, Previous ) ) {

            QRect previousRect( animations().menuEngine().currentRect( widget, Previous ) );
            if( previousRect.intersects( r ) )
            {
                const qreal opacity(  animations().menuEngine().opacity( widget, Previous ) );
                const QColor color( helper().menuBackgroundColor( helper().calcMidColor( palette.color( QPalette::Window ) ), widget, previousRect.center() ) );
                renderMenuItemRect( option, previousRect, color, palette, painter, opacity );
            }

        }

        return;
    }

    //__________________________________________________________________________
    void Style::renderMenuItemRect( const QStyleOption* opt, const QRect& r, const QColor& base, const QPalette& palette, QPainter* painter, qreal opacity ) const
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
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>( opt );
        if( menuItemOption && menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu )
        {

            QPixmap pm( r.size() );
            pm.fill( Qt::transparent );
            QPainter pp( &pm );
            QRect rr( QPoint( 0,0 ), r.size() );

            pp.setRenderHint( QPainter::Antialiasing );
            pp.setPen( Qt::NoPen );

            pp.setBrush( color );
            helper().fillHole( pp, rr );

            helper().holeFlat( color, 0.0 )->render( rr.adjusted( 1, 2, -2, -1 ), &pp );

            QRect maskr( visualRect( opt->direction, rr, QRect( rr.width()-40, 0, 40,rr.height() ) ) );
            QLinearGradient gradient(
                visualPos( opt->direction, maskr, QPoint( maskr.left(), 0 ) ),
                visualPos( opt->direction, maskr, QPoint( maskr.right()-4, 0 ) ) );
            gradient.setColorAt( 0.0, Qt::black );
            gradient.setColorAt( 1.0, Qt::transparent );
            pp.setBrush( gradient );
            pp.setCompositionMode( QPainter::CompositionMode_DestinationIn );
            pp.drawRect( maskr );

            if( opacity >= 0 && opacity < 1 )
            {
                pp.setCompositionMode( QPainter::CompositionMode_DestinationIn );
                pp.fillRect( pm.rect(), helper().alphaColor( Qt::black, opacity ) );
            }

            pp.end();

            painter->drawPixmap( handleRTL( opt, r ), pm );

        } else {

            if( opacity >= 0 && opacity < 1 )
            { color.setAlphaF( opacity ); }

            helper().holeFlat( color, 0.0 )->render( r.adjusted( 1,2,-2,-1 ), painter, TileSet::Full );

        }

    }

    //________________________________________________________________________
    void Style::renderCheckBox(
        QPainter *painter, const QRect &rect, const QPalette &palette,
        StyleOptions options, CheckBoxState state,
        qreal opacity,
        AnimationMode mode ) const
    {

        const int s( qMin( rect.width(), rect.height() ) );
        const QRect r( centerRect( rect, s, s ) );

        if( !( options & NoFill ) )
        {
            if( options & Sunken ) helper().holeFlat( palette.color( QPalette::Window ), 0.0, false )->render( r, painter, TileSet::Full );
            else renderSlab( painter, r, palette.color( QPalette::Button ), options, opacity, mode, TileSet::Ring );
        }

        // check mark
        const qreal x( r.center().x() - 3.5 );
        const qreal y( r.center().y() - 2.5 );

        if( state != CheckOff )
        {

            qreal penThickness( 2.0 );
            const QColor color( palette.color( ( options&Sunken ) ? QPalette::WindowText:QPalette::ButtonText ) );
            const QColor background( palette.color( ( options&Sunken ) ? QPalette::Window:QPalette::Button ) );
            QPen pen( helper().decoColor( background, color ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
            QPen contrastPen( helper().calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
            if( state == CheckTriState )
            {
                QVector<qreal> dashes;
                if( StyleConfigData::checkBoxStyle() == StyleConfigData::CS_CHECK )
                {

                    dashes << 1.0 << 2.0;
                    penThickness = 1.3;
                    pen.setWidthF( penThickness );
                    contrastPen.setWidthF( penThickness );

                } else {

                    dashes << 0.4 << 2.0;

                }
                pen.setDashPattern( dashes );
                contrastPen.setDashPattern( dashes );
            }

            painter->save();
            if( !( options&Sunken ) ) painter->translate( 0, -1 );
            painter->setRenderHint( QPainter::Antialiasing );

            const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );
            if( StyleConfigData::checkBoxStyle() == StyleConfigData::CS_CHECK )
            {

                painter->setPen( contrastPen );
                painter->translate( 0, offset );
                painter->drawLine( QPointF( x+9, y ), QPointF( x+3,y+7 ) );
                painter->drawLine( QPointF( x, y+4 ), QPointF( x+3,y+7 ) );

                painter->setPen( pen );
                painter->translate( 0, -offset );
                painter->drawLine( QPointF( x+9, y ), QPointF( x+3,y+7 ) );
                painter->drawLine( QPointF( x, y+4 ), QPointF( x+3,y+7 ) );

            } else {

                if( options&Sunken )
                {

                    painter->setPen( contrastPen );
                    painter->translate( 0, offset );
                    painter->drawLine( QPointF( x+8, y ), QPointF( x+1,y+7 ) );
                    painter->drawLine( QPointF( x+8, y+7 ), QPointF( x+1,y ) );

                    painter->setPen( pen );
                    painter->translate( 0, -offset );
                    painter->drawLine( QPointF( x+8, y ), QPointF( x+1,y+7 ) );
                    painter->drawLine( QPointF( x+8, y+7 ), QPointF( x+1,y ) );

                } else {

                    painter->setPen( contrastPen );
                    painter->translate( 0, offset );
                    painter->drawLine( QPointF( x+8, y-1 ), QPointF( x,y+7 ) );
                    painter->drawLine( QPointF( x+8, y+7 ), QPointF( x,y-1 ) );

                    painter->setPen( pen );
                    painter->translate( 0, -offset );
                    painter->drawLine( QPointF( x+8, y-1 ), QPointF( x,y+7 ) );
                    painter->drawLine( QPointF( x+8, y+7 ), QPointF( x,y-1 ) );

                }
            }
            painter->restore();
        }

        return;

    }

    //___________________________________________________________________
    void Style::renderRadioButton(
        QPainter* painter, const QRect& rect,
        const QPalette& palette,
        StyleOptions options,
        CheckBoxState state,
        qreal opacity,
        AnimationMode mode ) const
    {

        const int s( CheckBox_Size );
        const QRect r( centerRect( rect, s, s ) );
        const int x = r.x();
        const int y = r.y();

        const QColor color( palette.color( QPalette::Button ) );
        const QColor glow( slabShadowColor( color, options, opacity, mode ) );
        painter->drawPixmap( x, y, helper().roundSlab( color, glow, 0.0 ) );

        // draw the radio mark
        if( state == CheckOn )
        {
            const qreal radius( 2.6 );
            const qreal dx( 0.5*r.width() - radius );
            const qreal dy( 0.5*r.height() - radius );

            painter->save();
            painter->setRenderHints( QPainter::Antialiasing );
            painter->setPen( Qt::NoPen );

            const QColor background( palette.color( QPalette::Button ) );
            const QColor color( palette.color( QPalette::ButtonText ) );

            painter->setBrush( helper().calcLightColor( background ) );
            painter->translate( 0, radius/2 );
            painter->drawEllipse( QRectF( r ).adjusted( dx, dy, -dx, -dy ) );

            painter->setBrush( helper().decoColor( background, color ) );
            painter->translate( 0, -radius/2 );
            painter->drawEllipse( QRectF( r ).adjusted( dx, dy, -dx, -dy ) );
            painter->restore();

        }

        return;
    }

    //______________________________________________________________________________
    void Style::renderScrollBarHole(
        QPainter *painter, const QRect &r, const QColor &color,
        const Qt::Orientation& orientation, const TileSet::Tiles& tiles ) const
    {

        if( !r.isValid() ) return;

        // one need to make smaller shadow
        // notably on the size when rect height is too high
        const bool smallShadow( orientation == Qt::Horizontal ? r.height() < 10 : r.width() < 10 );
        helper().scrollHole( color, orientation, smallShadow )->render( r, painter, tiles );

    }

    //______________________________________________________________________________
    void Style::renderScrollBarHandle(
        QPainter* painter, const QRect& r, const QPalette& palette,
        const Qt::Orientation& orientation, const bool& hover, const qreal& opacity ) const
    {

        if( !r.isValid() ) return;

        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );

        // draw the hole as background
        const bool horizontal( orientation == Qt::Horizontal );
        const QRect holeRect( horizontal ? r.adjusted( -4,0,4,0 ) : r.adjusted( 0,-3,0,4 ) );
        renderScrollBarHole( painter, holeRect, palette.color( QPalette::Window ), orientation, horizontal ? TileSet::Vertical : TileSet::Horizontal );

        // draw the slider itself
        QRectF rect( horizontal ? r.adjusted( 3, 2, -3, -3 ):r.adjusted( 3, 4, -3, -3 ) );

        if( !rect.isValid() )
        {
            // e.g. not enough height
            painter->restore();
            return;
        }

        const QColor color( palette.color( QPalette::Button ) );

        // draw the slider
        const qreal radius = 3.5;

        // glow / shadow
        QColor glow;
        const QColor shadow( helper().alphaColor( helper().calcShadowColor( color ), 0.4 ) );
        const QColor hovered( helper().viewHoverBrush().brush( QPalette::Active ).color() );

        if( opacity >= 0 ) glow = KColorUtils::mix( shadow, hovered, opacity );
        else if( hover ) glow = hovered;
        else glow = shadow;

        helper().scrollHandle( color, glow )->
            render( rect.adjusted( -3, -3, 3, 3 ).toRect(),
            painter, TileSet::Full );

        // contents
        const QColor mid( helper().calcMidColor( color ) );
        QLinearGradient lg( 0, rect.top(), 0, rect.bottom() );
        lg.setColorAt(0, color );
        lg.setColorAt(1, mid );
        painter->setPen( Qt::NoPen );
        painter->setBrush( lg );
        painter->drawRoundedRect( rect.adjusted( 1, 1, -1, -1), radius - 2, radius - 2 );

        // bevel pattern
        const QColor light( helper().calcLightColor( color ) );

        QLinearGradient patternGradient( 0, 0, horizontal ? 30:0, horizontal? 0:30 );
        patternGradient.setSpread( QGradient::ReflectSpread );
        patternGradient.setColorAt( 0.0, Qt::transparent );
        patternGradient.setColorAt( 1.0, helper().alphaColor( light, 0.1 ) );

        QRectF bevelRect( rect );
        if( horizontal ) bevelRect.adjust( 0, 3, 0, -3 );
        else bevelRect.adjust( 3, 0, -3, 0 );

        if( bevelRect.isValid() )
        {
            painter->setBrush( patternGradient );
            painter->drawRect( bevelRect );
        }

        painter->restore();

    }

    //______________________________________________________________________________
    void Style::renderScrollBarArrow(
        QPainter* painter, const QRect& r, const QColor& color, const QColor& background,
        ArrowOrientation orientation ) const
    {

        const qreal penThickness = 1.6;
        QPolygonF a( genericArrow( orientation, ArrowNormal ) );

        const QColor contrast( helper().calcLightColor( background ) );
        const QColor base( helper().decoColor( background, color ) );

        painter->save();
        painter->translate( r.center() );
        painter->setRenderHint( QPainter::Antialiasing );

        const qreal offset( qMin( penThickness, qreal( 1.0 ) ) );
        painter->translate( 0,offset );
        painter->setPen( QPen( contrast, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );
        painter->translate( 0,-offset );

        painter->setPen( QPen( base, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
        painter->drawPolyline( a );
        painter->restore();

        return;

    }

    //______________________________________________________________________________
    QColor Style::scrollBarArrowColor( const QStyleOption* option, const SubControl& control, const QWidget* widget ) const
    {
        const QRect& r( option->rect );
        const QPalette& palette( option->palette );
        QColor color( palette.color( QPalette::WindowText ) );

        // check enabled state
        const bool enabled( option->state & State_Enabled );
        if( !enabled ) return color;

        if( const QAbstractSlider* slider = qobject_cast<const QAbstractSlider*>( widget ) )
        {
            const int value( slider->value() );
            if(
                ( control == SC_ScrollBarSubLine && value == slider->minimum() ) ||
                ( control == SC_ScrollBarAddLine && value == slider->maximum() ) ) {

                // manually disable arrow, to indicate that scrollbar is at limit
                return palette.color( QPalette::Disabled, QPalette::WindowText );
            }
        }


        const bool hover( animations().scrollBarEngine().isHovered( widget, control ) );
        const bool animated( animations().scrollBarEngine().isAnimated( widget, control ) );
        const qreal opacity( animations().scrollBarEngine().opacity( widget, control ) );

        QPoint position( hover ? widget->mapFromGlobal( QCursor::pos() ) : QPoint( -1, -1 ) );
        if( hover && r.contains( position ) )
        {
            // we need to update the arrow controlRect on fly because there is no
            // way to get it from the styles directly, outside of repaint events
            animations().scrollBarEngine().setSubControlRect( widget, control, r );
        }


        if( r.intersects(  animations().scrollBarEngine().subControlRect( widget, control ) ) )
        {

            QColor highlight = helper().viewHoverBrush().brush( palette ).color();
            if( animated )
            {
                color = KColorUtils::mix( color, highlight, opacity );

            } else if( hover ) {

                color = highlight;

            }

        }

        return color;

    }

    //______________________________________________________________________________
    void Style::renderSliderTickmarks( QPainter* painter, const QStyleOptionSlider* option,  const QWidget* widget ) const
    {

        const int& ticks( option->tickPosition );
        const int available( pixelMetric( PM_SliderSpaceAvailable, option, widget ) );
        int interval = option->tickInterval;
        if( interval < 1 ) interval = option->pageStep;
        if( interval < 1 ) return;

        const QRect& r( option->rect );
        const QPalette& palette( option->palette );

        const int fudge( pixelMetric( PM_SliderLength, option, widget ) / 2 );
        int current( option->minimum );

        // Since there is no subrect for tickmarks do a translation here.
        painter->save();
        painter->translate( r.x(), r.y() );

        if( option->orientation == Qt::Horizontal )
        {
            QColor base( helper().backgroundColor( palette.color( QPalette::Window ), widget, r.center() ) );
            painter->setPen( helper().calcDarkColor( base ) );
        }

        int tickSize( option->orientation == Qt::Horizontal ? r.height()/3:r.width()/3 );

        while( current <= option->maximum )
        {

            const int position( sliderPositionFromValue( option->minimum, option->maximum, current, available ) + fudge );

            // calculate positions
            if( option->orientation == Qt::Horizontal )
            {
                if( ticks == QSlider::TicksAbove ) painter->drawLine( position, 0, position, tickSize );
                else if( ticks == QSlider::TicksBelow ) painter->drawLine( position, r.height()-tickSize, position, r.height() );
                else {
                    painter->drawLine( position, 0, position, tickSize );
                    painter->drawLine( position, r.height()-tickSize, position, r.height() );
                }

            } else {

                QColor base( helper().backgroundColor( palette.color( QPalette::Window ), widget, QPoint( r.center().x(), position ) ) );
                painter->setPen( helper().calcDarkColor( base ) );

                if( ticks == QSlider::TicksAbove ) painter->drawLine( 0, position, tickSize, position );
                else if( ticks == QSlider::TicksBelow ) painter->drawLine( r.width()-tickSize, position, r.width(), position );
                else {
                    painter->drawLine( 0, position, tickSize, position );
                    painter->drawLine( r.width()-tickSize, position, r.width(), position );
                }
            }

            // go to next position
            current += interval;

        }

        painter->restore();
    }

    //____________________________________________________________________________________
    QColor Style::slabShadowColor( QColor color, StyleOptions options, qreal opacity, AnimationMode mode ) const
    {

        QColor glow;
        if( mode == AnimationNone || opacity < 0 )
        {

            if( options & Hover ) glow = helper().viewHoverBrush().brush( QPalette::Active ).color();
            else if( options & Focus ) glow = helper().viewFocusBrush().brush( QPalette::Active ).color();
            else if( ( options & SubtleShadow ) && color.isValid() ) glow = helper().alphaColor( helper().calcShadowColor( color ), 0.15 );


        } else if( mode == AnimationHover ) {

            // animated color, hover
            if( options & Focus ) glow = helper().viewFocusBrush().brush( QPalette::Active ).color();
            else if( ( options & SubtleShadow ) && color.isValid() ) glow = helper().alphaColor( helper().calcShadowColor( color ), 0.15 );

            if( glow.isValid() ) glow = KColorUtils::mix( glow,  helper().viewHoverBrush().brush( QPalette::Active ).color(), opacity );
            else glow = helper().alphaColor(  helper().viewHoverBrush().brush( QPalette::Active ).color(), opacity );

        } else if( mode == AnimationFocus ) {

            if( options & Hover ) glow = helper().viewHoverBrush().brush( QPalette::Active ).color();
            else if( ( options & SubtleShadow ) && color.isValid() ) glow = helper().alphaColor( helper().calcShadowColor( color ), 0.15 );

            if( glow.isValid() ) glow = KColorUtils::mix( glow,  helper().viewFocusBrush().brush( QPalette::Active ).color(), opacity );
            else glow = helper().alphaColor(  helper().viewFocusBrush().brush( QPalette::Active ).color(), opacity );

        }

        return glow;
    }

    //____________________________________________________________________________________
    QPolygonF Style::genericArrow( Style::ArrowOrientation orientation, Style::ArrowSize size ) const
    {

        QPolygonF a;
        switch( orientation )
        {
            case ArrowUp:
            {
                if( size == ArrowTiny ) a << QPointF( -1.75, 1.125 ) << QPointF( 0.5, -1.125 ) << QPointF( 2.75, 1.125 );
                else if( size == ArrowSmall ) a << QPointF( -2,1.5 ) << QPointF( 0.5, -1.5 ) << QPointF( 3,1.5 );
                else a << QPointF( -3,2.5 ) << QPointF( 0.5, -1.5 ) << QPointF( 4,2.5 );
                break;
            }

            case ArrowDown:
            {
                if( size == ArrowTiny ) a << QPointF( -1.75, -1.125 ) << QPointF( 0.5, 1.125 ) << QPointF( 2.75, -1.125 );
                else if( size == ArrowSmall ) a << QPointF( -2,-1.5 ) << QPointF( 0.5, 1.5 ) << QPointF( 3,-1.5 );
                else a << QPointF( -3,-1.5 ) << QPointF( 0.5, 2.5 ) << QPointF( 4,-1.5 );
                break;
            }

            case ArrowLeft:
            {
                if( size == ArrowTiny ) a << QPointF( 1.125, -1.75 ) << QPointF( -1.125, 0.5 ) << QPointF( 1.125, 2.75 );
                else if( size == ArrowSmall ) a << QPointF( 1.5,-2 ) << QPointF( -1.5, 0.5 ) << QPointF( 1.5,3 );
                else a << QPointF( 2.5,-3 ) << QPointF( -1.5, 0.5 ) << QPointF( 2.5,4 );
                break;
            }

            case ArrowRight:
            {
                if( size == ArrowTiny ) a << QPointF( -1.125, -1.75 ) << QPointF( 1.125, 0.5 ) << QPointF( -1.125, 2.75 );
                else if( size == ArrowSmall ) a << QPointF( -1.5,-2 ) << QPointF( 1.5, 0.5 ) << QPointF( -1.5,3 );
                else a << QPointF( -1.5,-3 ) << QPointF( 2.5, 0.5 ) << QPointF( -1.5,4 );
                break;
            }

            default: break;

        }

        return a;

    }

}
