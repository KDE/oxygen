// krazy:excludeall=qclasses

/* Oxygen widget style for KDE 4
   Copyright (C) 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
   Copyright (C) 2008 Long Huynh Huu <long.upcase@googlemail.com>
   Copyright (C) 2007-2008 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
   Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>
   Copyright (C) 2001-2002 Chris Lee <clee@kde.org>
   Copyright (C) 2001-2002 Carsten Pfeiffer <pfeiffer@kde.org>
   Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>
   Copyright (c) 2002 Malte Starostik <malte@kde.org>
   Copyright (C) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
   Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>
   Copyright (C) 2001-2002 Fredrik Höglund <fredrik@kde.org>
   Copyright (C) 2000 Daniel M. Duley <mosfet@kde.org>
   Copyright (C) 2000 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2001 Martijn Klingens <klingens@kde.org>

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

#include "oxygenstyle.h"
#include "oxygenstyle.moc"

#include <QtGui/QAbstractItemView>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDial>
#include <QtGui/QDockWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPaintEvent>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSpinBox>
#include <QtGui/QSplitterHandle>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QToolBox>
#include <QtGui/QToolButton>
#include <QtGui/QX11Info>

#include <QtDBus/QtDBus>

#include <KGlobal>
#include <KGlobalSettings>
#include <KConfigGroup>
#include <KColorUtils>
#include <KDebug>
#include <KTitleWidget>

#include "oxygenanimations.h"
#include "oxygenframeshadow.h"
#include "oxygenstyleconfigdata.h"
#include "oxygentransitions.h"
#include "oxygenwindowmanager.h"
#include "oxygenwidgetexplorer.h"

// We need better holes! Bevel color and shadow color are currently based on
// only one color, even though they are different things; also, we don't really
// know what bevel color should be based on... (and shadow color for white
// views looks rather bad). For now at least, just using QPalette::Window
// everywhere seems best...

/* These are to link libkio even if 'smart' linker is used */
#include <kio/authinfo.h>
extern "C" KIO::AuthInfo* _oxygen_init_kio() { return new KIO::AuthInfo(); }

K_EXPORT_STYLE("Oxygen", Oxygen::Style)

K_GLOBAL_STATIC_WITH_ARGS(Oxygen::StyleHelper, globalHelper, ("oxygen"))

// ie glowwidth which we want to un-reserve space for in the tabs
static const int gw = 1;

// these parameters replace the KStyle PushButton::PressedShiftVertical because
// the latter is not flexible enough. They are implemented manually in
// Style::drawControl
static const int pushButtonPressedShiftVertical = 1;
static const int toolButtonPressedShiftVertical = 1;

//_____________________________________________
static void cleanupBefore()
{ globalHelper->invalidateCaches(); }

namespace Oxygen
{
    //_____________________________________________
    Style::Style() :
        CE_CapacityBar( newControlElement( "CE_CapacityBar" ) ),
        _helper(*globalHelper),
        _animations( new Animations( this ) ),
        _transitions( new Transitions( this ) ),
        _windowManager( new WindowManager( this ) ),
        _frameShadowFactory( new FrameShadowFactory( this ) ),
        _widgetExplorer( new WidgetExplorer( this ) )
    {

        qAddPostRoutine(cleanupBefore);

        // connect to KGlobalSettings signals so we will be notified when the
        // system palette (in particular, the contrast) is changed
        QDBusConnection::sessionBus().connect(
            QString(), "/KGlobalSettings",
            "org.kde.KGlobalSettings",
            "notifyChange", this,
            SLOT(globalSettingsChange(int,int))
            );

        // call the slot directly; this initial call will set up things that also
        // need to be reset when the system palette changes
        globalSettingsChange(KGlobalSettings::PaletteChanged, 0);

        setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth, 1);

        // TODO: change this when double buttons are implemented
        setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleBotButton, true);
        setWidgetLayoutProp(WT_ScrollBar, ScrollBar::MinimumSliderHeight, 21);
        setWidgetLayoutProp(WT_ScrollBar, ScrollBar::ArrowColor,QPalette::WindowText);
        setWidgetLayoutProp(WT_ScrollBar, ScrollBar::ActiveArrowColor,QPalette::HighlightedText);

        //NOTE: These button heights are arbitrarily chosen
        // in a way that they don't consume too much space and
        // still are usable with i.e. touchscreens
        //TODO: This hasn't been tested though
        setWidgetLayoutProp( WT_ScrollBar, ScrollBar::SingleButtonHeight, qMax(OxygenStyleConfigData::scrollBarWidth() * 7 / 10, 14) );
        setWidgetLayoutProp( WT_ScrollBar, ScrollBar::DoubleButtonHeight, qMax(OxygenStyleConfigData::scrollBarWidth() * 14 / 10, 28) );
        setWidgetLayoutProp( WT_ScrollBar, ScrollBar::BarWidth, OxygenStyleConfigData::scrollBarWidth() + 2);

        setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin, 5); //also used by toolbutton
        setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Left, 11);
        setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Right, 11);
        setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Top, -1);
        setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Bot, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Left, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Right, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Top, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Bot, 0);

        // these are left to zero because the kstyle implementation that uses them
        // is not flexible enough for oxygen needs.
        // Instead we re-implement KStyle::drawControl when needed and perform the specific
        // handling of pressed buttons vertical shif there
        setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftHorizontal, 0);
        setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftVertical, 0 );

        setWidgetLayoutProp(WT_Splitter, Splitter::Width, 3);

        setWidgetLayoutProp(WT_CheckBox, CheckBox::Size, 21);
        setWidgetLayoutProp(WT_CheckBox, CheckBox::BoxTextSpace, 4);
        setWidgetLayoutProp(WT_RadioButton, RadioButton::Size, 21);
        setWidgetLayoutProp(WT_RadioButton, RadioButton::BoxTextSpace, 4);

        setWidgetLayoutProp(WT_DockWidget, DockWidget::TitleTextColor, QPalette::WindowText);
        setWidgetLayoutProp(WT_DockWidget, DockWidget::FrameWidth, 0);
        setWidgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, 3);
        setWidgetLayoutProp(WT_DockWidget, DockWidget::SeparatorExtent, 3);

        setWidgetLayoutProp(WT_Menu, Menu::FrameWidth, 5);

        setWidgetLayoutProp(WT_MenuBar, MenuBar::ItemSpacing, 0);
        setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin, 0);
        setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin+Left, 0);
        setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin+Right, 0);

        setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin, 3);
        setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin+Left, 5);
        setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin+Right, 5);

        setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckAlongsideIcon, 1);
        setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckWidth, 16);
        setWidgetLayoutProp(WT_MenuItem, MenuItem::MinHeight,  20);

        setWidgetLayoutProp(WT_ProgressBar, ProgressBar::BusyIndicatorSize, 10);
        setWidgetLayoutProp(WT_ProgressBar, ProgressBar::GrooveMargin, 2);

        setWidgetLayoutProp(WT_TabBar, TabBar::TabOverlap, 0);
        setWidgetLayoutProp(WT_TabBar, TabBar::BaseOverlap, 7);
        setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin, 4);
        setWidgetLayoutProp(WT_TabBar, TabBar::TabFocusMargin, 0);
        setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Left, 5);
        setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Right, 5);
        setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Top, 2);
        setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Bot, 4);
        setWidgetLayoutProp(WT_TabBar, TabBar::ScrollButtonWidth, 18);

        setWidgetLayoutProp(WT_TabWidget, TabWidget::ContentsMargin, 4);

        setWidgetLayoutProp(WT_Slider, Slider::HandleThickness, 23);
        setWidgetLayoutProp(WT_Slider, Slider::HandleLength, 15);

        setWidgetLayoutProp(WT_SpinBox, SpinBox::FrameWidth, 3);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin, 0);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Left, 1);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Right, 0);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Top, 0);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Bot, 0);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonWidth, 19);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonSpacing, 0);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin, 0);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Left, 2);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Right, 7);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Top, 4);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Bot, 3);
        setWidgetLayoutProp(WT_SpinBox, SpinBox::SupportFrameless, 1);

        setWidgetLayoutProp(WT_ComboBox, ComboBox::FrameWidth, 3);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin, 0);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Left, 2);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Right, 0);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Top, 0);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Bot, 0);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonWidth, 19);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin, 2);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::SupportFrameless, 1);

        setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Left, 0);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Right, 4);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Top, 2);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Bot, 1);
        setWidgetLayoutProp(WT_ComboBox, ComboBox::FocusMargin, 0);

        setWidgetLayoutProp(WT_ToolBar, ToolBar::FrameWidth, 0);
        setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemSpacing, 1);
        setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemMargin, 1);
        setWidgetLayoutProp(WT_ToolBar, ToolBar::ExtensionExtent, 16);

        setWidgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin, 4);
        setWidgetLayoutProp(WT_ToolButton, ToolButton::FocusMargin,    0);
        setWidgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorSize, 8);
        setWidgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorXOff, -11);
        setWidgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorYOff, -10);

        setWidgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, 3);
        setWidgetLayoutProp(WT_GroupBox, GroupBox::TitleTextColor, ColorMode(QPalette::WindowText));

        setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin, 0);
        setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin+Left, 5);
        setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin+Right, 5);

        setWidgetLayoutProp(WT_Window, Window::TitleTextColor, QPalette::WindowText);

    }

    //___________________________________________________________________________________
    void Style::drawComplexControl(ComplexControl control,const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
    {
        switch (control)
        {
            case CC_GroupBox:
            if( drawGroupBoxComplexControl( option, painter, widget) ) return;
            else break;

            case CC_Dial:
            if( drawDialComplexControl( option, painter, widget) ) return;
            else break;

            case CC_ToolButton:
            if( drawToolButtonComplexControl( option, painter, widget) ) return;
            else break;

            case CC_Slider:
            {
                if( const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option))
                {

                    // if tickmarks are not requested, fall back to default
                    if( !(slider->subControls & SC_SliderTickmarks) || slider->tickPosition == QSlider::NoTicks ) break;

                    // draw oxygen custom tickmarks
                    drawSliderTickmarks( slider, painter, widget );

                    // disable tickmarks drawing and draw using default
                    // this is necessary because by default kstyle does not allow style
                    // to customize tickmarks painting
                    QStyleOptionSlider local( *slider );
                    local.subControls &= ~SC_SliderTickmarks;
                    KStyle::drawComplexControl(control,&local,painter,widget);
                    return;

                } else break;
            }

            default: break;

        }

        return KStyle::drawComplexControl(control,option,painter,widget);

    }

    //___________________________________________________________________________________
    void Style::drawItemText(
        QPainter* painter, const QRect& r, int alignment, const QPalette& palette, bool enabled,
        const QString &text, QPalette::ColorRole textRole ) const
    {

        if( !animations().widgetEnabilityEngine().enabled() )
        { return KStyle::drawItemText( painter, r, alignment, palette, enabled, text, textRole ); }

        /*
        check if painter engine is registered to WidgetEnabilityEngine, and animated
        if yes, merge the palettes. Note: a static_cast is safe here, since only the address
        of the pointer is used, not the actual content
        */
        const QWidget* widget( static_cast<const QWidget*>( painter->device() ) );
        if( widget && animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
        {

            QPalette pal = _helper.mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  );
            return KStyle::drawItemText( painter, r, alignment, pal, enabled, text, textRole );

        } else {

            return KStyle::drawItemText( painter, r, alignment, palette, enabled, text, textRole );

        }

    }

    //___________________________________________________________________________________
    bool Style::drawGroupBoxComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
    {

        if(const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option))
        {
            bool isFlat = groupBox->features & QStyleOptionFrameV2::Flat;

            if( isFlat)
            {
                QFont font = painter->font();
                QFont oldFont = font;
                font.setBold(true);
                painter->setFont(font);
                KStyle::drawComplexControl(CC_GroupBox,option,painter,widget);
                painter->setFont(oldFont);
                return true;
            }
        }

        return false;
    }

    //___________________________________________________________________________________
    bool Style::drawDialComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
    {

        const bool enabled = option->state & State_Enabled;
        const bool mouseOver(enabled && (option->state & State_MouseOver));
        const bool hasFocus( enabled && (option->state & State_HasFocus));

        StyleOptions opts = 0;
        if( (option->state & State_On) || (option->state & State_Sunken)) opts |= Sunken;
        if( option->state & State_HasFocus) opts |= Focus;
        if( enabled && (option->state & State_MouseOver)) opts |= Hover;

        // mouseOver has precedence over focus
        animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
        animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        QRect rect( option->rect );
        const QPalette &pal( option->palette );
        QColor color( pal.color(QPalette::Button) );

        if( enabled && animations().widgetStateEngine().isAnimated( widget, AnimationHover ) && !(opts & Sunken ) )
        {

            qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
            renderDialSlab( painter, rect, pal.color(QPalette::Button), option, opts, opacity, AnimationHover );

        } else if( enabled && !mouseOver && animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) && !(opts & Sunken ) ) {

            qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );
            renderDialSlab( painter, rect, pal.color(QPalette::Button), option, opts, opacity, AnimationFocus );

        } else {

            renderDialSlab( painter, rect, pal.color(QPalette::Button), option, opts);

        }

        return true;

    }

    //___________________________________________________________________________________
    bool Style::drawToolButtonComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
    {

        if( !widget ) return false;

        // check autoRaise state
        State flags( option->state );
        bool isInToolBar( widget->parent() && widget->parent()->inherits( "QToolBar" ) );
        if( !( isInToolBar || (flags & State_AutoRaise) ) ) return false;

        // get rect and palette
        QRect rect( option->rect );
        QPalette palette( option->palette );

        // local clone of toolbutton option
        const QStyleOptionToolButton *tbOption( qstyleoption_cast<const QStyleOptionToolButton *>(option) );
        if( !tbOption ) return false;

        // make local copy
        QStyleOptionToolButton localTbOption(*tbOption);

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));
        const bool hasFocus(enabled && (flags&State_HasFocus));
        const bool sunken( (flags & State_Sunken) || (flags & State_On) );

        if( isInToolBar )
        {

            animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );

        } else {

            // mouseOver has precedence over focus
            animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
            animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

        }

        // toolbar animation
        bool toolBarAnimated( isInToolBar && animations().toolBarEngine().isAnimated( widget->parentWidget() ) );
        QRect animatedRect( animations().toolBarEngine().animatedRect( widget->parentWidget() ) );
        QRect currentRect( animations().toolBarEngine().currentRect( widget->parentWidget() ) );
        bool current( isInToolBar && currentRect.intersects( rect.translated( widget->mapToParent( QPoint(0,0) ) ) ) );
        bool toolBarTimerActive( isInToolBar && animations().toolBarEngine().isTimerActive( widget->parentWidget() ) );

        // normal toolbutton animation
        bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
        bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );

        if( enabled && !(mouseOver || hasFocus || sunken ) )
        {

            if( hoverAnimated || (focusAnimated && !hasFocus) || ( ((toolBarAnimated && animatedRect.isNull())||toolBarTimerActive) && current ) )
            {
                QRect buttonRect = subControlRect(CC_ToolButton, option, SC_ToolButton, widget);
                localTbOption.rect = buttonRect;
                localTbOption.state = flags;
                drawKStylePrimitive(WT_ToolButton, ToolButton::Panel, &localTbOption, buttonRect, palette, flags, painter, widget);
            }

        }

        // copy code from kstyle. and modify it to handle arrow hover properly
        QRect buttonRect = subControlRect( CC_ToolButton, tbOption, SC_ToolButton, widget);
        QRect menuRect = subControlRect( CC_ToolButton, tbOption, SC_ToolButtonMenu, widget);

        // State_AutoRaise: only draw button when State_MouseOver
        State bflags = tbOption->state;
        if( (bflags & State_AutoRaise) && !(bflags & State_MouseOver) )
        { bflags &= ~State_Raised; }

        State mflags = bflags;

        localTbOption.palette = palette;

        if( (tbOption->subControls & SC_ToolButton) && (bflags & (State_Sunken | State_On | State_Raised) ) )
        {
            localTbOption.rect = buttonRect;
            localTbOption.state = bflags;
            drawPrimitive(PE_PanelButtonTool, &localTbOption, painter, widget);
        }

        if( tbOption->subControls & SC_ToolButtonMenu)
        {

            localTbOption.rect = menuRect;
            localTbOption.state = mflags;
            drawPrimitive(PE_IndicatorButtonDropDown, &localTbOption, painter, widget );

        } else if( tbOption->features & QStyleOptionToolButton::HasMenu) {

            // This is requesting KDE3-style arrow indicator, per Qt 4.4 behavior. Qt 4.3 prefers to hide
            // the fact of the menu's existence. Whee! Since we don't know how to paint this right,
            // though, we have to have some metrics set for it to look nice.
            int size = widgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorSize, option, widget );
            if( size)
            {

                int xOff = widgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorXOff, option, widget );
                int yOff = widgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorYOff, option, widget );

                QRect r = QRect(buttonRect.right() + xOff, buttonRect.bottom() + yOff, size, size);
                localTbOption.rect  = r;
                localTbOption.state = bflags;
                drawPrimitive(PE_IndicatorButtonDropDown, &localTbOption, painter, widget );

            }

        }

        if( flags & State_HasFocus)
        {
            QRect focusRect = rect;
            localTbOption.rect = focusRect;
            localTbOption.state = bflags;
            drawKStylePrimitive(WT_ToolButton, Generic::FocusIndicator, &localTbOption, focusRect, palette, bflags, painter, widget );
        }

        // CE_ToolButtonLabel expects a readjusted rect, for the button area proper
        QStyleOptionToolButton labelOpt = *tbOption;
        labelOpt.rect = buttonRect;
        drawControl(CE_ToolButtonLabel, &labelOpt, painter, widget );

        return true;

    }

    //___________________________________________________________________________________
    void Style::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const
    {

        switch (element)
        {

            case PE_Widget:
            {
                // check widget and attributes
                if( !widget || !widget->testAttribute(Qt::WA_StyledBackground) || widget->testAttribute(Qt::WA_NoSystemBackground))
                { return KStyle::drawPrimitive( element, option, p, widget ); }

                if( !( (widget->windowFlags() & Qt::WindowType_Mask) & (Qt::Window|Qt::Dialog) ) )
                { return KStyle::drawPrimitive( element, option, p, widget ); }

                if( !widget->isWindow() )
                { return KStyle::drawPrimitive( element, option, p, widget ); }

                // normal "window" background
                _helper.renderWindowBackground(p, option->rect, widget, option->palette );
                return;

            }

            case PE_PanelMenu:
            {

                // do nothing if menu is embedded in another widget
                // this corresponds to having a transparent background
                if( widget && !widget->isWindow() ) return;

                const QStyleOptionMenuItem* mOpt( qstyleoption_cast<const QStyleOptionMenuItem*>(option) );
                if( !( mOpt && widget ) ) return;
                QRect r = mOpt->rect;
                QColor color = mOpt->palette.window().color();

                bool hasAlpha( hasAlphaChannel( widget ) );
                if( hasAlpha )
                {

                    p->setCompositionMode(QPainter::CompositionMode_Source );
                    TileSet *tileSet( _helper.roundCorner(color) );
                    tileSet->render( r, p );

                    p->setCompositionMode(QPainter::CompositionMode_SourceOver );
                    p->setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

                }

                _helper.renderMenuBackground( p, r, widget, mOpt->palette );

                if( hasAlpha ) p->setClipping( false );
                _helper.drawFloatFrame( p, r, color, !hasAlpha );

                return;

            }

            case PE_FrameMenu:
            {

                if( option && widget && widget->inherits( "QToolBar" ) )
                {
                    _helper.renderWindowBackground( p, option->rect, widget, option->palette );
                    _helper.drawFloatFrame( p, option->rect, option->palette.window().color(), true );
                }

                return;

            }

            // disable painting of PE_PanelScrollAreaCorner
            // the default implementation fills the rect with the window background color
            // which does not work for windows that have gradients.
            case PE_PanelScrollAreaCorner: return;

            // tooltip_ labels
            case PE_PanelTipLabel:
            {
                if( !OxygenStyleConfigData::toolTipDrawStyledFrames ) break;
                QRect r = option->rect;

                QColor color = option->palette.brush(QPalette::ToolTipBase).color();
                QColor topColor = _helper.backgroundTopColor(color);
                QColor bottomColor = _helper.backgroundBottomColor(color);

                QLinearGradient gr( 0, r.top(), 0, r.bottom() );
                gr.setColorAt(0, topColor );
                gr.setColorAt(1, bottomColor );


                // contrast pixmap
                QLinearGradient gr2( 0, r.top(), 0, r.bottom() );
                gr2.setColorAt(0, _helper.calcLightColor( bottomColor ) );
                gr2.setColorAt(0.9, bottomColor );

                p->save();

                if( compositingActive() )
                {
                    p->setRenderHint(QPainter::Antialiasing);

                    QRectF local( r );
                    local.adjust( 0.5, 0.5, -0.5, -0.5 );

                    p->setPen( Qt::NoPen );
                    p->setBrush( gr );
                    p->drawRoundedRect( local, 4, 4 );

                    p->setBrush( Qt::NoBrush );
                    p->setPen(QPen( gr2, 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    p->drawRoundedRect( local, 4, 4 );

                } else {

                    p->setPen( Qt::NoPen );
                    p->setBrush( gr );
                    p->drawRect( r );

                    p->setBrush( Qt::NoBrush );
                    p->setPen(QPen( gr2, 1.1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    p->drawRect( r );

                }

                p->restore();

                return;
            }

            case PE_PanelItemViewItem:
            {

                const QStyleOptionViewItemV4 *opt = qstyleoption_cast<const QStyleOptionViewItemV4*>(option);
                const QAbstractItemView *view = qobject_cast<const QAbstractItemView *>(widget);
                bool hover = (option->state & State_MouseOver) && (!view || view->selectionMode() != QAbstractItemView::NoSelection);

                bool hasCustomBackground = opt->backgroundBrush.style() != Qt::NoBrush && !(option->state & State_Selected);
                bool hasSolidBackground = !hasCustomBackground || opt->backgroundBrush.style() == Qt::SolidPattern;

                if( !hover && !(option->state & State_Selected) && !hasCustomBackground && !(opt->features & QStyleOptionViewItemV2::Alternate) )
                { return; }

                QPalette::ColorGroup cg;
                if( option->state & State_Enabled) cg = (option->state & State_Active) ? QPalette::Normal : QPalette::Inactive;
                else cg = QPalette::Disabled;

                QColor color;
                if( hasCustomBackground && hasSolidBackground) color = opt->backgroundBrush.color();
                else color = option->palette.color(cg, QPalette::Highlight);

                if( hover && !hasCustomBackground)
                {
                    if( !(option->state & State_Selected)) color.setAlphaF(0.2);
                    else color = color.lighter(110);
                }

                if( opt && (opt->features & QStyleOptionViewItemV2::Alternate))
                { p->fillRect(option->rect, option->palette.brush(cg, QPalette::AlternateBase)); }

                if( !hover && !(option->state & State_Selected) && !hasCustomBackground)
                { return; }

                if( hasCustomBackground && !hasSolidBackground) {

                    const QPointF oldBrushOrigin = p->brushOrigin();
                    p->setBrushOrigin(opt->rect.topLeft());
                    p->setBrush(opt->backgroundBrush);
                    p->setPen(Qt::NoPen);
                    p->drawRect(opt->rect);
                    p->setBrushOrigin(oldBrushOrigin);

                } else {

                    // get selection tileset
                    QRect r = option->rect;
                    TileSet *tileSet( _helper.selection( color, r.height(), hasCustomBackground ) );

                    bool roundedLeft  = false;
                    bool roundedRight = false;
                    if( opt )
                    {

                        roundedLeft  = (opt->viewItemPosition == QStyleOptionViewItemV4::Beginning);
                        roundedRight = (opt->viewItemPosition == QStyleOptionViewItemV4::End);
                        if( opt->viewItemPosition == QStyleOptionViewItemV4::OnlyOne ||
                            opt->viewItemPosition == QStyleOptionViewItemV4::Invalid ||
                            (view && view->selectionBehavior() != QAbstractItemView::SelectRows))
                        {
                            roundedLeft  = true;
                            roundedRight = true;
                        }

                    }

                    bool reverseLayout = option->direction == Qt::RightToLeft;

                    TileSet::Tiles tiles = TileSet::Center;
                    if( !reverseLayout ? roundedLeft : roundedRight) tiles |= TileSet::Left;
                    else r.adjust( -8, 0, 0, 0 );

                    if( !reverseLayout ? roundedRight : roundedLeft) tiles |= TileSet::Right;
                    else r.adjust( 0, 0, 8, 0 );

                    if( r.isValid()) tileSet->render( r, p, tiles );
                }

                return;
            }

            // this uses "standard" radio buttons to draw in Qt3 lists.
            case PE_Q3CheckListExclusiveIndicator:
            {

                State flags = option->state;
                QRect r = option->rect;

                // HACK: for some reason height is calculated using the font,
                // which here results in bad centerin.
                // also: centering is incorrect
                if( r.height() < r.width() ) r.setHeight( r.width() );
                r.translate( 0, 2 );

                QPalette pal = option->palette;
                if( flags & State_On) drawKStylePrimitive( WT_RadioButton, RadioButton::RadioOn, option, r, pal, flags, p, widget);
                else drawKStylePrimitive( WT_RadioButton, RadioButton::RadioOff, option, r, pal, flags, p, widget);
                return;

            }

            // this uses "standard" checkboxes to draw in Qt3 lists.
            case PE_Q3CheckListIndicator:
            {

                State flags = option->state;
                QRect r = option->rect;

                // HACK: for some reason height is calculated using the font,
                // which here results in bad centerin.
                // also: centering is incorrect
                if( r.height() < r.width() ) r.setHeight( r.width() );
                r.translate( 0, 2 );

                QPalette pal = option->palette;
                if( flags & State_NoChange) drawKStylePrimitive( WT_CheckBox, CheckBox::CheckTriState, option, r, pal, flags, p, widget);
                else if( flags & State_On) drawKStylePrimitive( WT_CheckBox, CheckBox::CheckOn, option, r, pal, flags, p, widget);
                else drawKStylePrimitive( WT_CheckBox, CheckBox::CheckOff, option, r, pal, flags, p, widget);
                return;

            }


            default: KStyle::drawPrimitive( element, option, p, widget );
        }
    }

    //___________________________________________________________________________________
    void Style::drawControl(ControlElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const
    {

        if( element == CE_CapacityBar )
        {
            drawCapacityBar( option, p, widget );
            return;
        }

        switch (element)
        {

            case CE_RubberBand:
            {
                if( const QStyleOptionRubberBand *rbOpt = qstyleoption_cast<const QStyleOptionRubberBand *>(option))
                {
                    p->save();
                    QColor color = rbOpt->palette.color(QPalette::Highlight);
                    p->setPen(KColorUtils::mix(color, rbOpt->palette.color(QPalette::Active, QPalette::WindowText)));
                    color.setAlpha(50);
                    p->setBrush(color);
                    p->setClipRegion(rbOpt->rect);
                    p->drawRect(rbOpt->rect.adjusted(0,0,-1,-1));
                    p->restore();
                    return;
                }
                break;
            }

            case CE_ProgressBar:
            if( const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option))
            {
                // same as QCommonStyle::drawControl, except that it handles animations
                QStyleOptionProgressBarV2 subopt = *pb;
                subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
                drawControl(CE_ProgressBarGroove, &subopt, p, widget);

                if( animations().progressBarEngine().busyIndicatorEnabled() && pb->maximum == 0 && pb->minimum == 0 )
                { animations().progressBarEngine().startBusyTimer(); }

                if( animations().progressBarEngine().isAnimated( widget ) )
                { subopt.progress = animations().progressBarEngine().value( widget ); }

                subopt.rect = subElementRect(SE_ProgressBarContents, &subopt, widget);
                drawControl(CE_ProgressBarContents, &subopt, p, widget);

                if( pb->textVisible)
                {
                    subopt.rect = subElementRect(SE_ProgressBarLabel, pb, widget);
                    drawControl(CE_ProgressBarLabel, &subopt, p, widget);
                }

            }
            return;

            case CE_ComboBoxLabel:
            {
                //same as CommonStyle, except for filling behind icon
                if( const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option))
                {

                    QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);

                    p->save();
                    if( !cb->currentIcon.isNull() )
                    {

                        QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                        QPixmap pixmap = cb->currentIcon.pixmap(cb->iconSize, mode);
                        QRect iconRect(editRect);
                        iconRect.setWidth(cb->iconSize.width() + 4);
                        iconRect = alignedRect(
                            cb->direction,
                            Qt::AlignLeft | Qt::AlignVCenter,
                            iconRect.size(), editRect);

                        drawItemPixmap(p, iconRect, Qt::AlignCenter, pixmap);

                        if( cb->direction == Qt::RightToLeft) editRect.translate(-4 - cb->iconSize.width(), 0);
                        else editRect.translate(cb->iconSize.width() + 4, 0);
                    }

                    if( !cb->currentText.isEmpty() && !cb->editable)
                    {
                        drawItemText(
                            p, editRect.adjusted(1, 0, -1, 0),
                            visualAlignment(cb->direction, Qt::AlignLeft | Qt::AlignVCenter),
                            cb->palette, cb->state & State_Enabled, cb->currentText, QPalette::ButtonText );
                    }
                    p->restore();
                    return;
                }
                break;
            }

            case CE_TabBarTabLabel:
            {

                // bypass KStyle entirely because it makes it completely impossible
                // to handle both KDE and Qt applications at the same time
                // however, adds some extras spaces for icons in order not to conflict with tab margin
                // (which QCommonStyle does not handle right)
                const QStyleOptionTabV3 *tab = qstyleoption_cast<const QStyleOptionTabV3 *>(option);
                if( tab && !tab->icon.isNull() )
                {

                    QStyleOptionTabV3 tabV3(*tab);
                    bool verticalTabs = tabV3.shape == QTabBar::RoundedEast
                        || tabV3.shape == QTabBar::RoundedWest
                        || tabV3.shape == QTabBar::TriangularEast
                        || tabV3.shape == QTabBar::TriangularWest;
                    if( verticalTabs ) tabV3.rect.adjust( 0, 0, 0, -3 );
                    else if( tabV3.direction == Qt::RightToLeft ) tabV3.rect.adjust( 0, 0, -3, 0 );
                    else tabV3.rect.adjust( 3, 0, 0, 0 );

                    return QCommonStyle::drawControl( element, &tabV3, p, widget);

                } else return QCommonStyle::drawControl( element, option, p, widget);

            }

            // re-implement from kstyle to handle pressed
            // down vertical shift properly
            case CE_ToolButtonLabel:
            {

                // cast option and check
                const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option);
                if( !tbOpt ) return KStyle::drawControl(element, option, p, widget);

                // copy option and adjust rect
                QStyleOptionToolButton local = *tbOpt;

                // disable mouseOver effect if toolbar is animated
                if( widget && animations().toolBarEngine().isAnimated( widget->parentWidget() ) )
                { local.state &= ~State_MouseOver; }

                // check whether button is pressed
                const bool active = (option->state & State_On) || (option->state & State_Sunken);
                if( !active )  return KStyle::drawControl(element, &local, p, widget);

                // check autoRaise
                if( option->state & State_AutoRaise ) return KStyle::drawControl(element, &local, p, widget);

                // check button parent. Right now the fix addresses only toolbuttons located
                // in a menu, in order to fix the KMenu title rendering issue
                if( !( widget && widget->parent() && widget->parent()->inherits( "QMenu" ) ) )
                { return KStyle::drawControl(element, &local, p, widget); }

                // adjust vertical position
                local.rect.translate( 0, toolButtonPressedShiftVertical );
                return KStyle::drawControl(element, &local, p, widget);

            }

            case CE_HeaderEmptyArea:
            {

                // use the same background as in drawHeaderPrimitive
                QPalette pal( option->palette );

                if( widget && animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
                { pal = _helper.mergePalettes( pal, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }

                QRect r( option->rect );
                bool horizontal( option->state & QStyle::State_Horizontal );
                bool reverse( option->direction == Qt::RightToLeft );
                renderHeaderBackground( r, pal, p, widget, horizontal, reverse );

                return;
            }

            case CE_ShapedFrame:
            {

                // for frames embedded in KTitleWidget, just paint the window background
                if( widget && qobject_cast<KTitleWidget*>(widget->parentWidget()) )
                {
                    _helper.renderWindowBackground(p, option->rect, widget, widget->window()->palette());
                    return;
                }

                // cast option and check
                const QStyleOptionFrameV3* frameOpt = qstyleoption_cast<const QStyleOptionFrameV3*>( option );
                if( !frameOpt ) break;

                int frameShape  = frameOpt->frameShape;
                switch( frameShape )
                {

                    case QFrame::HLine:
                    {
                        _helper.drawSeparator(p, option->rect, option->palette.color(QPalette::Window), Qt::Horizontal);
                        return;
                    }

                    case QFrame::VLine:
                    {
                        _helper.drawSeparator(p, option->rect, option->palette.color(QPalette::Window), Qt::Vertical);
                        return;
                    }

                    default:
                    {

                        // use KStyle
                        KStyle::drawControl(element, option, p, widget);
                        return;

                    }
                }

                break;

            }

            default: break;
        }

        KStyle::drawControl(element, option, p, widget);

    }

    //_________________________________________________________________________
    void Style::drawKStylePrimitive(WidgetType widgetType, int primitive,
        const QStyleOption* opt,
        const QRect &r,
        const QPalette &palette,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        QPalette pal( palette );
        if( widget && !widget->inherits( "QMdiSubWindow" ) )
        {
            if( animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
            { pal = _helper.mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }
        }

        switch (widgetType)
        {
            case WT_PushButton:
            if( drawPushButtonPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_ToolBoxTab:
            if( drawToolBoxTabPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_ProgressBar:
            if( drawProgressBarPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_MenuBar:
            if( drawMenuBarPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_MenuBarItem:
            if( drawMenuBarItemPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_Menu:
            if( drawMenuPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_MenuItem:
            if( drawMenuItemPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_DockWidget:
            if( drawDockWidgetPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_StatusBar:
            if( drawStatusBarPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_CheckBox:
            if( drawCheckBoxPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_RadioButton:
            if( drawRadioButtonPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_ScrollBar:
            if( drawScrollBarPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_TabBar:
            if( drawTabBarPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_TabWidget:
            if( drawTabWidgetPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_Window:
            if( drawWindowPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_Splitter:
            if( drawSplitterPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;


            case WT_Slider:
            if( drawSliderPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_SpinBox:
            if( drawSpinBoxPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;


            case WT_ComboBox:
            if( drawComboBoxPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_Header:
            if( drawHeaderPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_Tree:
            if( drawTreePrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_LineEdit:
            if( drawLineEditPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_GroupBox:
            if( drawGroupBoxPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_ToolBar:
            if( drawToolBarPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_ToolButton:
            if( drawToolButtonPrimitive( primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;
            else break;

            case WT_Limit: //max value for the enum, only here to silence the compiler
            case WT_Generic: // handled below since the primitives arevalid for all WT_ types
            default: break;
        }

        // generic primitive
        if( drawGenericPrimitive( widgetType, primitive, opt, r, pal, flags, p, widget, kOpt ) ) return;

        // default fallback
        KStyle::drawKStylePrimitive(widgetType, primitive, opt, r, pal, flags, p, widget, kOpt);
    }

    //___________________________________________________________________
    bool Style::drawPushButtonPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( opt );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );
        StyleOptions opts = 0;
        const bool enabled = flags & State_Enabled;
        switch (primitive)
        {
            case PushButton::Panel:
            {

                const bool mouseOver(enabled && (flags & State_MouseOver));
                const bool hasFocus( enabled && (flags & State_HasFocus));

                if( (flags & State_On) || (flags & State_Sunken)) opts |= Sunken;
                if( flags & State_HasFocus) opts |= Focus;
                if( enabled && (flags & State_MouseOver)) opts |= Hover;

                // update animation state
                animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
                animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

                // store animation state
                bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
                bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );
                qreal hoverOpacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
                qreal focusOpacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );

                // decide if widget must be rendered flat.
                /*
                The decision is made depending on
                - whether the "flat" flag is set in the option
                - whether the widget is hight enough to render both icons and normal margins
                Note: in principle one should also check for the button text height
                */
                const QStyleOptionButton* bOpt( qstyleoption_cast< const QStyleOptionButton* >( opt ) );
                bool flat = ( bOpt && (
                    bOpt->features.testFlag( QStyleOptionButton::Flat ) ||
                    ( (!bOpt->icon.isNull()) && sizeFromContents( CT_PushButton, opt, bOpt->iconSize, widget ).height() > r.height() ) ) );

                if( flat )
                {


                    QRect slitRect(r);
                    if( !( opts & Sunken) )
                    {
                        // hover rect
                        if( enabled && hoverAnimated )
                        {

                            QColor glow( _helper.alphaColor( _helper.viewFocusBrush().brush(QPalette::Active).color(), hoverOpacity ) );
                            _helper.slitFocused( glow )->render(slitRect, p);

                        } else if( mouseOver) {

                            _helper.slitFocused(_helper.viewFocusBrush().brush(QPalette::Active).color())->render(slitRect, p);

                        }

                    } else {

                        slitRect.adjust( 0, 0, 0, -1 );

                        // flat pressed-down buttons do not get focus effect,
                        // consistently with tool buttons
                        if( enabled && hoverAnimated )
                        {

                            _helper.renderHole( p, pal.color(QPalette::Window), slitRect, false, mouseOver, hoverOpacity, AnimationHover, TileSet::Ring );

                        } else {

                            _helper.renderHole( p, pal.color(QPalette::Window), slitRect, false, mouseOver);

                        }

                    }

                } else {

                    if( enabled && hoverAnimated && !(opts & Sunken ) )
                    {

                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts, hoverOpacity, AnimationHover, TileSet::Ring );

                    } else if( enabled && !mouseOver && focusAnimated && !(opts & Sunken ) ) {

                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts, focusOpacity, AnimationFocus, TileSet::Ring );

                    } else {

                        renderButtonSlab(p, r, pal.color(QPalette::Button), opts);

                    }

                }

                return true;
            }

            case PushButton::DefaultButtonFrame: return true;

            case Generic::Text:
            {
                if( const QStyleOptionButton* bOpt = qstyleoption_cast<const QStyleOptionButton*>(opt) )
                {
                    // when icon is drawn, need to adjust the text rect
                    // for consistency with other buttons and labels
                    if( !bOpt->icon.isNull() )
                    {
                        KStyle::drawKStylePrimitive( WT_PushButton, primitive, opt, r.adjusted( 0, 0, 0, 1 ), pal, flags, p, widget, kOpt );
                        return true;
                    }
                }

                return false;

            }

            default: return false;
        }

    }

    //___________________________________________________________________
    bool Style::drawToolBoxTabPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( pal );
        Q_UNUSED( flags );
        Q_UNUSED( kOpt );

        const bool enabled( flags&State_Enabled );
        const bool selected( flags&State_Selected );
        const bool mouseOver( enabled && !selected && (flags&State_MouseOver) );

        const bool reverseLayout = opt->direction == Qt::RightToLeft;
        switch (primitive)
        {
            case ToolBoxTab::Panel:
            {

                const QStyleOptionToolBox *option = qstyleoption_cast<const QStyleOptionToolBox *>(opt);
                if(!(option && widget)) return true;

                const QStyleOptionToolBoxV2 *v2 = qstyleoption_cast<const QStyleOptionToolBoxV2 *>(opt);
                if( v2 && v2->position == QStyleOptionToolBoxV2::Beginning && selected ) return true;

                bool animated( false );
                qreal opacity( AnimationData::OpacityInvalid );
                if( enabled )
                {
                    // try retrieve QSplitterHandle, from painter device.
                    if( const QAbstractButton* button = dynamic_cast<const QAbstractButton*>(p->device()) )
                    {
                        animations().widgetStateEngine().updateState( button, AnimationHover, mouseOver );
                        animated = animations().widgetStateEngine().isAnimated( button, AnimationHover );
                        opacity = animations().widgetStateEngine().opacity( button, AnimationHover );
                    }

                }

                // save colors
                QColor color( widget->palette().color(QPalette::Window) ); // option returns a wrong color
                QColor dark( _helper.calcDarkColor(color) );
                QList<QColor> colors;
                colors.push_back( _helper.calcLightColor(color) );

                if( mouseOver || animated )
                {

                    QColor highlight = _helper.viewHoverBrush().brush(pal).color();
                    if( animated )
                    {

                        colors.push_back( KColorUtils::mix( dark, highlight, opacity ) );
                        colors.push_back( _helper.alphaColor( highlight, 0.2*opacity ) );

                    } else {

                        colors.push_back( highlight );
                        colors.push_back( _helper.alphaColor( highlight, 0.2 ) );

                    }

                } else colors.push_back( dark );

                // create path
                p->save();
                QPainterPath path;
                int y = r.height()*15/100;
                if( reverseLayout) {
                    path.moveTo(r.left()+52, r.top());
                    path.cubicTo(QPointF(r.left()+50-8, r.top()), QPointF(r.left()+50-10, r.top()+y), QPointF(r.left()+50-10, r.top()+y));
                    path.lineTo(r.left()+18+9, r.bottom()-y);
                    path.cubicTo(QPointF(r.left()+18+9, r.bottom()-y), QPointF(r.left()+19+6, r.bottom()-1-0.3), QPointF(r.left()+19, r.bottom()-1-0.3));
                    p->setClipRect( QRect( r.left()+21, r.top(), 28, r.height() ) );
                } else {
                    path.moveTo(r.right()-52, r.top());
                    path.cubicTo(QPointF(r.right()-50+8, r.top()), QPointF(r.right()-50+10, r.top()+y), QPointF(r.right()-50+10, r.top()+y));
                    path.lineTo(r.right()-18-9, r.bottom()-y);
                    path.cubicTo(QPointF(r.right()-18-9, r.bottom()-y), QPointF(r.right()-19-6, r.bottom()-1-0.3), QPointF(r.right()-19, r.bottom()-1-0.3));
                    p->setClipRect( QRect( r.right()-48, r.top(), 32, r.height() ) );
                }


                // paint
                p->setRenderHint(QPainter::Antialiasing, true);
                p->translate(0,2);
                foreach( const QColor& color, colors )
                {
                    p->setPen(color);
                    p->drawPath(path);
                    p->translate(0,-1);
                }
                p->restore();

                p->save();
                p->setRenderHint(QPainter::Antialiasing, false);
                p->translate(0,2);
                foreach( const QColor& color, colors )
                {
                    p->setPen( color );
                    if( reverseLayout) {
                        p->drawLine(r.left()+50-1, r.top(), r.right(), r.top());
                        p->drawLine(r.left()+20, r.bottom()-2, r.left(), r.bottom()-2);
                    } else {
                        p->drawLine(r.left(), r.top(), r.right()-50+1, r.top());
                        p->drawLine(r.right()-20, r.bottom()-2, r.right(), r.bottom()-2);
                    }
                    p->translate(0,-1);
                }

                p->restore();
                return true;
            }

            default: return false;
        }
    }

    //______________________________________________________
    bool Style::drawProgressBarPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( widget );
        Q_UNUSED( flags );
        Q_UNUSED( kOpt );

        const QStyleOptionProgressBarV2 *pbOpt = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt);
        Qt::Orientation orientation = pbOpt? pbOpt->orientation : Qt::Horizontal;

        // adjust rect to match other sunken frames
        QRect rect = r;
        if( orientation == Qt::Horizontal ) rect.adjust( 1, 0, -1, 0 );
        else rect.adjust( 0, 2, 0, -2 );

        switch (primitive)
        {
            case ProgressBar::Groove:
            {
                renderScrollBarHole(p, rect, pal.color(QPalette::Window), orientation);
                return true;
            }

            case ProgressBar::Indicator:
            case ProgressBar::BusyIndicator:
            {

                if( r.width() < 4 || r.height() < 4 ) return true;
                QPixmap pixmap( _helper.progressBarIndicator( pal, rect ) );
                p->drawPixmap( rect.adjusted(-1, -2, 0, 0).topLeft(), pixmap );
                return true;

            }

            default: return false;

        }
    }


    //______________________________________________________
    bool Style::drawMenuBarPrimitive(
        int primitive,
        const QStyleOption*,
        const QRect &, const QPalette&,
        State, QPainter* ,
        const QWidget*,
        KStyle::Option* ) const
    {
        switch (primitive)
        {
            case MenuBar::EmptyArea: return true;
            default: return false;
        }
    }

    //______________________________________________________
    bool Style::drawMenuBarItemPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {
        Q_UNUSED( opt );
        Q_UNUSED( widget );
        switch (primitive)
        {
            case MenuBarItem::Panel:
            {

                const bool enabled  = flags & State_Enabled;
                const bool active  = flags & State_Selected;

                if( !enabled ) return true;

                const bool animated( animations().menuBarEngine().isAnimated(widget, r.topLeft() ) );
                qreal opacity( animations().menuBarEngine().opacity( widget, r.topLeft() ) );
                QRect currentRect( animations().menuBarEngine().currentRect( widget, r.topLeft() ) );
                QRect animatedRect( animations().menuBarEngine().animatedRect( widget ) );

                const bool intersected( animatedRect.intersects( r ) );
                const bool current( currentRect.contains( r.topLeft() ) );
                const bool timerIsActive( animations().menuBarEngine().isTimerActive(widget) );

                // do nothing in case of empty intersection between animated rect and current
                if( animated && !( animatedRect.isNull() || intersected ) ) return true;

                if( active || animated || timerIsActive )
                {

                    QColor color = pal.color(QPalette::Window);
                    if( OxygenStyleConfigData::menuHighlightMode() != OxygenStyleConfigData::MM_DARK)
                    {

                        if(flags & State_Sunken)
                        {

                            if( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG) color = pal.color(QPalette::Highlight);
                            else color = KColorUtils::mix(color, KColorUtils::tint(color, pal.color(QPalette::Highlight), 0.6));

                        } else {

                            if( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG) color = KColorUtils::tint(color, _helper.viewHoverBrush().brush(pal).color());
                            else color = KColorUtils::mix(color, KColorUtils::tint(color, _helper.viewHoverBrush().brush(pal).color()));
                        }

                    } else color = _helper.calcMidColor( _helper.backgroundColor( color, widget, r.center() ) );

                    // drawing
                    if( animated && intersected )
                    {

                        _helper.holeFlat(color, 0.0)->render(animatedRect.adjusted(1,1,-1,-1), p, TileSet::Full);

                    } else if( timerIsActive && current ) {

                        _helper.holeFlat(color, 0.0)->render(r.adjusted(1,1,-1,-1), p, TileSet::Full);

                    } else if( animated && current ) {

                        color.setAlphaF( opacity );
                        _helper.holeFlat(color, 0.0)->render(r.adjusted(1,1,-1,-1), p, TileSet::Full);

                    } else if( active ) {

                        _helper.holeFlat(color, 0.0)->render(r.adjusted(1,1,-1,-1), p, TileSet::Full);

                    }

                }

                return true;
            }

            case Generic::Text:
            {
                KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
                QPalette::ColorRole role( QPalette::WindowText );
                if( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG && (flags & State_Sunken) && (flags & State_Enabled) )
                { role = QPalette::HighlightedText; }

                drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled, textOpts->text, role);
                return true;
            }

            default: return false;
        }

    }

    //______________________________________________________
    bool Style::drawMenuPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect& r, const QPalette& pal,
        State, QPainter* p,
        const QWidget* widget,
        KStyle::Option* ) const
    {

        switch (primitive)
        {
            case Generic::Frame:
            return true;

            case Menu::Background:
            {

                QRect animatedRect( animations().menuEngine().animatedRect( widget ) );
                if( !animatedRect.isNull() )
                {

                    if( animatedRect.intersects( r ) )
                    {
                        QColor color( _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, animatedRect.center() ) );
                        renderMenuItemRect( opt, animatedRect, color, pal, p );
                    }

                } else if( animations().menuEngine().isTimerActive( widget ) ) {

                    QRect previousRect( animations().menuEngine().currentRect( widget, Previous ) );
                    if( previousRect.intersects( r ) )
                    {

                        QColor color( _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, previousRect.center() ) );
                        renderMenuItemRect( opt, previousRect, color, pal, p );
                    }

                } else if( animations().menuEngine().isAnimated(widget, Previous ) ) {

                    QRect previousRect( animations().menuEngine().currentRect( widget, Previous ) );
                    if( previousRect.intersects( r ) )
                    {
                        qreal opacity(  animations().menuEngine().opacity( widget, Previous ) );
                        QColor color( _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, previousRect.center() ) );
                        renderMenuItemRect( opt, previousRect, color, pal, p, opacity );
                    }

                }

                return true;

            }

            case Menu::TearOff:
            return true;

            case Menu::Scroller:
            return true;

            default: return false;
        }
    }

    //______________________________________________________
    bool Style::drawMenuItemPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));

        switch( primitive )
        {

            case MenuItem::Separator:
            {
                // cast option
                if( const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>(opt) )
                {

                    // check text and icon
                    // separators with non empty text are rendered as checked toolbuttons
                    if( !menuItemOption->text.isEmpty() )
                    {

                        QStyleOptionToolButton toolbuttonOpt;
                        toolbuttonOpt.features = QStyleOptionToolButton::None;
                        toolbuttonOpt.state = State_On|State_Sunken|State_Enabled;
                        toolbuttonOpt.rect = r.adjusted(-2, -3, 2, 2);
                        toolbuttonOpt.subControls = SC_ToolButton;
                        toolbuttonOpt.icon =  menuItemOption->icon;

                        QFont font = widget->font();
                        toolbuttonOpt.font = widget->font();
                        toolbuttonOpt.font.setBold(true);

                        toolbuttonOpt.iconSize = QSize(
                            pixelMetric(QStyle::PM_SmallIconSize,0,0),
                            pixelMetric(QStyle::PM_SmallIconSize,0,0) );

                        // for now menu size is not calculated properly
                        // (meaning it doesn't account for titled separators width
                        // as a fallback, we elide the text to be displayed
                        if( !menuItemOption->text.isEmpty() )
                        {
                            int width( r.width() );
                            if( !menuItemOption->icon.isNull() )
                            { width -= toolbuttonOpt.iconSize.width() + 2; }

                            width -= 2*widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + MainMargin, &toolbuttonOpt, widget) +
                                widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + Left, &toolbuttonOpt, widget) +
                                widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + Right, &toolbuttonOpt, widget);

                            toolbuttonOpt.text = QFontMetrics( toolbuttonOpt.font ).elidedText( menuItemOption->text, Qt::ElideRight, width );
                        }

                        toolbuttonOpt.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
                        drawComplexControl( CC_ToolButton, &toolbuttonOpt, p, widget );
                        return true;

                    }

                }

                // in all other cases draw regular separator
                QColor color( _helper.menuBackgroundColor( pal.color(QPalette::Window), widget, r.center() ) );
                _helper.drawSeparator(p, r, color, Qt::Horizontal);
                return true;
            }

            case MenuItem::ItemIndicator:
            {

                // check if there is a 'sliding' animation in progress, in which case, do nothing
                QRect animatedRect( animations().menuEngine().animatedRect( widget ) );
                if( !animatedRect.isNull() ) return true;

                if( enabled)
                {

                    bool animated( animations().menuEngine().isAnimated(widget, Current ) );
                    QRect currentRect( animations().menuEngine().currentRect( widget, Current ) );
                    const bool intersected( currentRect.contains( r.topLeft() ) );

                    QColor color( _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, r.center() ) );
                    if( animated && intersected ) renderMenuItemRect( opt, r, color, pal, p, animations().menuEngine().opacity( widget, Current ) );
                    else renderMenuItemRect( opt, r, color, pal, p );

                } else drawKStylePrimitive(WT_Generic, Generic::FocusIndicator, opt, r, pal, flags, p, widget, kOpt);

                return true;
            }

            case Generic::Text:
            {
                KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
                QPalette::ColorRole role( QPalette::WindowText );

                bool animated( animations().menuEngine().isAnimated(widget, Current ) );
                QRect animatedRect( animations().menuEngine().animatedRect( widget ) );

                if( (!animated) && OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG && (flags & State_Selected) && (flags & State_Enabled) )
                { role = QPalette::HighlightedText; }

                drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled, textOpts->text, role);

                return true;
            }

            case Generic::ArrowRight:
            case Generic::ArrowLeft:
            {
                // always draw in window text color due to fade-out
                extractOption<KStyle::ColorOption*>(kOpt)->color = QPalette::WindowText;
                return false;
            }

            case MenuItem::CheckColumn: return true;

            case MenuItem::CheckOn:
            {
                QPalette local( pal );
                local.setColor( QPalette::Window, _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, r.topLeft() ) );
                renderCheckBox(p, r.adjusted(2,-2,2,2), local, enabled, false, mouseOver, CheckBox::CheckOn, true);
                return true;
            }

            case MenuItem::CheckOff:
            {
                QPalette local( pal );
                local.setColor( QPalette::Window, _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, r.topLeft() ) );
                renderCheckBox(p, r.adjusted(2,-2,2,2), local, enabled, false, mouseOver, CheckBox::CheckOff, true);
                return true;
            }

            case MenuItem::RadioOn:
            {
                renderRadioButton(p, r.adjusted(2,-1,2,2), pal, enabled, false, mouseOver, RadioButton::RadioOn, true);
                return true;
            }

            case MenuItem::RadioOff:
            {
                renderRadioButton(p, r.adjusted(2,-1,2,2), pal, enabled, false, mouseOver, RadioButton::RadioOff, true);
                return true;
            }

            case MenuItem::CheckIcon:  return true;
            default: return false;
        }
    }

    //______________________________________________________
    bool Style::drawDockWidgetPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( kOpt );
        const bool reverseLayout = opt->direction == Qt::RightToLeft;
        switch (primitive)
        {

            case Generic::Text:
            {

                const QStyleOptionDockWidget* dwOpt = ::qstyleoption_cast<const QStyleOptionDockWidget*>(opt);
                if( !dwOpt) return true;
                const QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(opt);
                bool verticalTitleBar = v2 ? v2->verticalTitleBar : false;

                QRect btnr = subElementRect(dwOpt->floatable ? SE_DockWidgetFloatButton : SE_DockWidgetCloseButton, opt, widget);
                int fw = widgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, opt, widget);
                QRect r = dwOpt->rect.adjusted(fw, fw, -fw, -fw);
                if( verticalTitleBar) {

                    if(btnr.isValid()) r.setY(btnr.y()+btnr.height());

                } else if(reverseLayout) {

                    if(btnr.isValid()) r.setLeft(btnr.x()+btnr.width());
                    r.adjust(0,0,-4,0);

                } else {

                    if(btnr.isValid())  r.setRight(btnr.x());
                    r.adjust(4,0,0,0);

                }

                QString title = dwOpt->title;
                QString tmpTitle = title;
                if(tmpTitle.contains("&"))
                {
                    int pos = tmpTitle.indexOf("&");
                    if(!(tmpTitle.size()-1 > pos && tmpTitle.at(pos+1) == QChar('&'))) tmpTitle.remove(pos, 1);

                }

                int tw = dwOpt->fontMetrics.width(tmpTitle);
                int width = verticalTitleBar ? r.height() : r.width();
                if( width < tw) title = dwOpt->fontMetrics.elidedText(title, Qt::ElideRight, width, Qt::TextShowMnemonic);

                if( verticalTitleBar)
                {

                    QSize s = r.size();
                    s.transpose();
                    r.setSize(s);

                    p->save();
                    p->translate(r.left(), r.top() + r.width());
                    p->rotate(-90);
                    p->translate(-r.left(), -r.top());
                    drawItemText(p, r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, dwOpt->palette, dwOpt->state & State_Enabled, title, QPalette::WindowText);
                    p->restore();


                } else {

                    drawItemText(p, r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, dwOpt->palette, dwOpt->state & State_Enabled, title, QPalette::WindowText);

                }

                return true;

            }

            case Generic::Frame:
            return true;

            case DockWidget::TitlePanel:
            {
                // The frame is draw in the eventfilter
                // This is because when a dockwidget has a titlebarwidget, then we can not
                //  paint on the dockwidget prober here
                return true;
            }

            case DockWidget::SeparatorHandle:
            if( flags&State_Horizontal) drawKStylePrimitive(WT_Splitter, Splitter::HandleVert, opt, r, pal, flags, p, widget);
            else drawKStylePrimitive(WT_Splitter, Splitter::HandleHor, opt, r, pal, flags, p, widget);
            return true;

            default: return false;

        }

    }

    //______________________________________________________
    bool Style::drawStatusBarPrimitive(
        int primitive,
        const QStyleOption*,
        const QRect &, const QPalette &,
        State, QPainter*,
        const QWidget*,
        KStyle::Option* ) const
    {

        switch (primitive)
        {
            case Generic::Frame: return true;
            default: return false;
        }

    }

    //______________________________________________________
    bool Style::drawCheckBoxPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( opt );
        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));

        switch(primitive)
        {
            case CheckBox::CheckOn:
            case CheckBox::CheckOff:
            case CheckBox::CheckTriState:
            {

                bool hasFocus = flags & State_HasFocus;

                // mouseOver has precedence over focus
                animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
                animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus&&!mouseOver );

                if( enabled && animations().widgetStateEngine().isAnimated( widget, AnimationHover ) )
                {

                    qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
                    renderCheckBox(p, r, pal, enabled, hasFocus, mouseOver, primitive, false, opacity, AnimationHover );

                } else if( enabled && !hasFocus && animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) ) {

                    qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );
                    renderCheckBox(p, r, pal, enabled, hasFocus, mouseOver, primitive, false, opacity, AnimationFocus );

                } else renderCheckBox(p, r, pal, enabled, hasFocus, mouseOver, primitive);

                return true;

            }

            case Generic::Text:
            {

                KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
                drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled, textOpts->text, QPalette::WindowText);
                return true;

            }

            default: return false;

        }
    }

    //______________________________________________________
    bool Style::drawRadioButtonPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( opt );

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));

        switch(primitive)
        {
            case RadioButton::RadioOn:
            case RadioButton::RadioOff:
            {


                bool hasFocus = flags & State_HasFocus;

                // mouseOver has precedence over focus
                animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
                animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

                if( enabled && animations().widgetStateEngine().isAnimated( widget, AnimationHover ) )
                {

                    qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
                    renderRadioButton(p, r, pal, enabled, hasFocus, mouseOver, primitive, true, opacity, AnimationHover );

                } else if(  enabled && animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) ) {

                    qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );
                    renderRadioButton(p, r, pal, enabled, hasFocus, mouseOver, primitive, true, opacity, AnimationFocus );

                } else renderRadioButton(p, r, pal, enabled, hasFocus, mouseOver, primitive);

                return true;
            }

            case Generic::Text:
            {
                KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
                drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled,
                    textOpts->text, QPalette::WindowText);
                return true;
            }

            default: return false;
        }

    }

    //______________________________________________________
    bool Style::drawScrollBarPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &rect, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( widget );
        Q_UNUSED( kOpt );

        const bool reverseLayout = opt->direction == Qt::RightToLeft;
        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));

        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        animations().scrollBarEngine().updateState( widget, enabled && slider && (slider->activeSubControls & SC_ScrollBarSlider) );

        /*
        translate scrollbar rect to reduce the spacing with
        respect to the associated view
        */
        QRect r( rect );
        if( slider )
        {
            if( slider->orientation == Qt::Horizontal ) r.adjust( 0, 1, 0, -1 );
            else r.adjust( 1, 0, -1, 0 );
        }

        switch (primitive)
        {
            case ScrollBar::DoubleButtonHor:

            if( reverseLayout) renderScrollBarHole(p, QRect(r.right()+1, r.top(), 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Bottom | TileSet::Left);
            else renderScrollBarHole(p, QRect(r.left()-5, r.top(), 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Right | TileSet::Bottom);
            return false;

            case ScrollBar::DoubleButtonVert:
            renderScrollBarHole(p, QRect(r.left(), r.top()-5, r.width(), 5), pal.color(QPalette::Window), Qt::Vertical, TileSet::Bottom | TileSet::Left | TileSet::Right);
            return false;

            case ScrollBar::SingleButtonHor:
            if( reverseLayout) renderScrollBarHole(p, QRect(r.left()-5, r.top(), 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Right | TileSet::Bottom);
            else renderScrollBarHole(p, QRect(r.right()+1, r.top(), 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Left | TileSet::Bottom);
            return false;

            case ScrollBar::SingleButtonVert:
            renderScrollBarHole(p, QRect(r.left(), r.bottom()+3, r.width(), 5), pal.color(QPalette::Window), Qt::Vertical, TileSet::Top | TileSet::Left | TileSet::Right);
            return false;

            case ScrollBar::GrooveAreaVertTop:
            {
                renderScrollBarHole(p, r.adjusted(0,2,0,12), pal.color(QPalette::Window), Qt::Vertical, TileSet::Left | TileSet::Right | TileSet::Center | TileSet::Top);
                return true;
            }

            case ScrollBar::GrooveAreaVertBottom:
            {
                renderScrollBarHole(p, r.adjusted(0,-10,0,0), pal.color(QPalette::Window), Qt::Vertical, TileSet::Left | TileSet::Right | TileSet::Center | TileSet::Bottom);
                return true;
            }

            case ScrollBar::GrooveAreaHorLeft:
            {
                QRect rect = (reverseLayout) ? r.adjusted(0,0,10,0) : r.adjusted(0,0,12,0);
                renderScrollBarHole(p, rect, pal.color(QPalette::Window), Qt::Horizontal, TileSet::Left | TileSet::Center | TileSet::Top | TileSet::Bottom);
                return true;
            }

            case ScrollBar::GrooveAreaHorRight:
            {
                QRect rect = (reverseLayout) ? r.adjusted(-12,0,0,0) : r.adjusted(-10,0,0,0);
                renderScrollBarHole(p, rect, pal.color(QPalette::Window), Qt::Horizontal, TileSet::Right | TileSet::Center | TileSet::Top | TileSet::Bottom);
                return true;
            }

            case ScrollBar::SliderHor:
            {
                bool animated( enabled && animations().scrollBarEngine().isAnimated( widget, SC_ScrollBarSlider ) );
                if( animated ) renderScrollBarHandle(p, r, pal, Qt::Horizontal, mouseOver, animations().scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
                else renderScrollBarHandle(p, r, pal, Qt::Horizontal, mouseOver );
                return true;
            }

            case ScrollBar::SliderVert:
            {
                bool animated( enabled && animations().scrollBarEngine().isAnimated( widget, SC_ScrollBarSlider ) );
                if( animated ) renderScrollBarHandle(p, r, pal, Qt::Vertical, mouseOver, animations().scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
                else renderScrollBarHandle(p, r, pal, Qt::Vertical, mouseOver );
                return true;
            }

            case Generic::ArrowUp:
            case Generic::ArrowDown:
            case Generic::ArrowLeft:
            case Generic::ArrowRight:
            { return drawGenericArrow( WT_ScrollBar, primitive, opt, r, pal, flags, p, widget, kOpt ); }

            default: return false;

        }
    }


    //______________________________________________________
    bool Style::drawTabBarPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {
        Q_UNUSED( kOpt );
        const bool reverseLayout = opt->direction == Qt::RightToLeft;

        switch (primitive)
        {
            case TabBar::NorthTab:
            case TabBar::SouthTab:
            case TabBar::WestTab:
            case TabBar::EastTab:
            {
                const QStyleOptionTabV2* tabOpt = qstyleoption_cast<const QStyleOptionTabV2*>(opt);
                if( !tabOpt) return false;

                renderTab(p, r, pal, flags, tabOpt, reverseLayout, widget);
                return true;

            }

            case TabBar::IndicatorTear:
            {

                const QStyleOptionTab* option( qstyleoption_cast<const QStyleOptionTab*>(opt) );
                if(!option) return true;

                // in fact with current version of Qt (4.6.0) the cast fails and document mode is always false
                // this will hopefully be fixed in later versions
                const QStyleOptionTabV3* tabOptV3( qstyleoption_cast<const QStyleOptionTabV3*>(opt) );
                bool documentMode = tabOptV3 ? tabOptV3->documentMode : false;

                const QTabWidget *tabWidget = (widget && widget->parentWidget()) ? qobject_cast<const QTabWidget *>(widget->parentWidget()) : NULL;
                documentMode |= (tabWidget ? tabWidget->documentMode() : true );

                TileSet::Tiles tiles;
                QRect rect;
                QRect clip;
                QRect gr = r; // fade the tab there
                bool vertical = false;

                switch( tabOrientation(option->shape) )
                {

                    case TabNorth:
                    if( documentMode || (option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) )
                    {

                        tiles = TileSet::Top;
                        rect = QRect(r.x()-7, r.y()+r.height()-7-1, 14+7, 7);


                    } else {

                        tiles = reverseLayout ? TileSet::Right : TileSet::Left;
                        tiles |= TileSet::Top;
                        rect = QRect(r.x(), r.y()+r.height()-7-1, 7, 14);

                    }

                    gr.adjust( 0, 0, 0, -3 );

                    rect.translate(-gw,0);
                    rect = visualRect(option->direction, r, rect);
                    if( !reverseLayout ) gr.translate(-gw,0);
                    break;

                    case TabSouth:
                    if( documentMode || (option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) )
                    {

                        tiles = TileSet::Bottom;
                        rect = QRect(r.x()-8, r.y(), 14+7, 7);

                        if( reverseLayout ) clip = QRect(r.x()+2, r.y() + 3, 14-1, 3);
                        else clip = QRect(r.x()-7, r.y() + 3, 14-1, 3);

                    } else {

                        tiles = reverseLayout ? TileSet::Right : TileSet::Left;
                        tiles |= TileSet::Bottom;
                        rect = QRect(r.x(), r.y()-7, 7, 14);
                        if( reverseLayout ) clip = QRect( r.x()+2, r.y() + 3, 6, 3);
                        else clip = QRect(r.x(), r.y()+3, 6, 3);

                    }

                    gr.adjust( 0, 3, 0, 0 );

                    rect.translate(-gw,0);
                    rect = visualRect(option->direction, r, rect);
                    if( !reverseLayout ) gr.translate(-gw,0);
                    break;

                    case TabWest:
                    if( documentMode || (option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) )
                    {

                        tiles = TileSet::Left;
                        rect = QRect(r.x()+r.width()-8, r.y()-7, 7, 6+14);
                        clip = QRect(r.x()+r.width()-8, r.y()-7, 4, 14-2);

                    } else {

                        tiles = TileSet::Top|TileSet::Left;
                        rect = QRect(r.x()+r.width()-8, r.y(), 7, 7);

                    }

                    gr.adjust( 0, 0, -3, 0 );
                    vertical = true;
                    rect.translate(0,-gw);
                    gr.translate(0,-gw);
                    break;

                    case TabEast:
                    if( documentMode || (option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) )
                    {

                        tiles = TileSet::Right;
                        rect = QRect(r.x()+1, r.y()-7, 7, 6+14);
                        clip = QRect(r.x()+4, r.y()-7, 4, 14-2);

                    } else {

                        tiles = TileSet::Top|TileSet::Right;
                        rect = QRect(r.x()-6, r.y(), 14, 7);

                    }

                    gr.adjust( 3, 0, 0, 0 );
                    vertical = true;
                    rect.translate(0,-gw);
                    gr.translate(0,-gw);
                    break;

                    default: return true;
                }

                // fade tabbar
                QPixmap pm(gr.size());
                pm.fill(Qt::transparent);
                QPainter pp(&pm);

                int w = 0, h = 0;
                if( vertical) h = gr.height();
                else w = gr.width();

                QLinearGradient grad;
                if( reverseLayout && !vertical ) grad = QLinearGradient( 0, 0, w, h );
                else grad = QLinearGradient(w, h, 0, 0);

                grad.setColorAt(0, Qt::transparent );
                grad.setColorAt(0.6, Qt::black);

                _helper.renderWindowBackground(&pp, pm.rect(), widget, pal);
                pp.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
                pp.fillRect(pm.rect(), QBrush(grad));
                pp.end();
                p->drawPixmap(gr.topLeft()+QPoint(0,-1),pm);

                if( !(documentMode && flags&State_Selected) )
                {
                    // clipping is done by drawing the
                    // window background over the requested rect
                    if( clip.isValid() )
                    {
                        // translate clip because the corresponding slab rect also is.
                        clip.translate(0,-1);
                        if( const QWidget* parent = checkAutoFillBackground( widget ) ) p->fillRect( clip, parent->palette().color( parent->backgroundRole() ) );
                        else _helper.renderWindowBackground(p, clip, widget, pal);
                    }
                    renderSlab(p, rect, opt->palette.color(QPalette::Window), NoFill, tiles );
                }
                return true;
            }

            case TabBar::BaseFrame:
            {

                const QStyleOptionTabBarBase* tabOpt = qstyleoption_cast<const QStyleOptionTabBarBase*>(opt);

                // HACK: When drawing corner widget the
                // tabbar area is not given, we use the widget
                // itself to calculate the needed base frame
                // part
                const QTabWidget *tabWidget = qobject_cast<const QTabWidget *>(widget);
                if( widget && !tabWidget ) tabWidget = qobject_cast<const QTabWidget *>(widget->parent() );
                if( !tabOpt->tabBarRect.isValid() && !tabWidget) return true;

                const QWidget* leftWidget = ( tabWidget && widget->isVisible() && tabWidget->cornerWidget(Qt::TopLeftCorner) ) ? tabWidget->cornerWidget(Qt::TopLeftCorner):0;
                const QWidget* rightWidget = ( tabWidget && widget->isVisible() && tabWidget->cornerWidget(Qt::TopRightCorner) ) ? tabWidget->cornerWidget(Qt::TopRightCorner):0;

                switch( tabOrientation(tabOpt->shape) )
                {
                    case TabNorth:
                    {

                        if( r.left() < tabOpt->tabBarRect.left())
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setRight(tabOpt->tabBarRect.left());
                            else if( leftWidget )
                            {

                                fr.setRight(fr.left() + leftWidget->width());
                                fr.translate( 0, -2 );

                            } else return true;

                            fr.adjust(-7,-gw,7,-1-gw);
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Top);
                        }

                        if( tabOpt->tabBarRect.right() < r.right())
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setLeft(tabOpt->tabBarRect.right());
                            else if( rightWidget )
                            {

                                fr.setLeft(fr.right() - rightWidget->width());
                                fr.translate( 0, 4 );

                            } else return true;

                            fr.adjust(-7,-gw,7,-1-gw);
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Top);
                        }

                        return true;
                    }

                    case TabSouth:
                    {

                        if( r.left() < tabOpt->tabBarRect.left())
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setRight(tabOpt->tabBarRect.left());
                            else if( leftWidget )
                            {

                                fr.setRight(fr.left() + leftWidget->width());
                                fr.translate( 0, -3 );

                            } else return true;

                            fr.adjust(-7,gw,7,-1+gw);
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                        }

                        if( tabOpt->tabBarRect.right() < r.right())
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setLeft(tabOpt->tabBarRect.right());
                            else if( rightWidget ) {

                                fr.setLeft(fr.right() - rightWidget->width());
                                fr.translate( 0, -3 );

                            } else return true;

                            fr.adjust(-6,gw,7,-1+gw);
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                        }

                        return true;
                    }

                    case TabEast:
                    {

                        if( r.top() < tabOpt->tabBarRect.top() )
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setBottom(tabOpt->tabBarRect.top());
                            else if( leftWidget )
                            {
                                fr.setBottom( fr.top() + leftWidget->height() );
                                fr.translate( -3, 0 );

                            } else return true;

                            fr.adjust( gw+1, -7, gw, 7 );
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Right);

                        }

                        if( tabOpt->tabBarRect.bottom() < r.bottom() )
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setTop(tabOpt->tabBarRect.bottom());
                            else if( rightWidget )
                            {
                                fr.setTop( fr.bottom() - rightWidget->height() );
                                fr.translate( -3, 0 );
                            }

                            fr.adjust( gw+1,-7,gw, 7 );
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Right);

                        }

                        break;
                    }

                    case TabWest:
                    {

                        if( r.top() < tabOpt->tabBarRect.top() )
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setBottom(tabOpt->tabBarRect.top());
                            else if( leftWidget )
                            {
                                fr.setBottom( fr.top() + leftWidget->height() );
                                fr.translate( 3, 0 );

                            } else return true;

                            fr.adjust( -gw,-7,-1-gw, 7 );
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Left);

                        }

                        if( tabOpt->tabBarRect.bottom() < r.bottom() )
                        {
                            QRect fr = r;
                            if( tabOpt->tabBarRect.isValid() ) fr.setTop(tabOpt->tabBarRect.bottom());
                            else if( rightWidget )
                            {
                                fr.setTop( fr.bottom() - rightWidget->height() );
                                fr.translate( 3, 0 );
                            }

                            fr.adjust( -gw,-7,-1-gw, 7 );
                            renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Left);

                        }

                        break;
                    }

                    default: return false;
                }

                return true;
            }

            default: return false;
        }

    }

    //______________________________________________________
    bool Style::drawTabWidgetPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( flags );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );

        const bool reverseLayout = opt->direction == Qt::RightToLeft;
        switch (primitive)
        {
            case Generic::Frame:
            {
                const QStyleOptionTabWidgetFrame* tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(opt);

                // no frame is drawn when tabbar is empty.
                // this is consistent with the tabWidgetTabContents subelementRect
                if( tabOpt->tabBarSize.isEmpty() ) return true;

                // tab
                int w = tabOpt->tabBarSize.width();
                int h = tabOpt->tabBarSize.height();

                // left corner widget
                int lw = tabOpt->leftCornerWidgetSize.width();
                int lh = tabOpt->leftCornerWidgetSize.height();

                // right corner
                int rw = tabOpt->rightCornerWidgetSize.width();
                int rh = tabOpt->rightCornerWidgetSize.height();

                const QTabWidget* tw( qobject_cast<const QTabWidget*>(widget) );

                switch( tabOrientation(tabOpt->shape) )
                {
                    case TabNorth:
                    if( w+lw > 0 )
                    {
                        renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom | TileSet::Right);

                        if(reverseLayout)
                        {

                            // Left and right widgets are placed right and left when in reverse mode
                            // left side
                            QRect slabRect( -gw, r.y()-gw, r.width() - w - lw+7+gw, 7);
                            if( tw ) slabRect.setRight( qMax( slabRect.right(), tw->rect().left() + rw + 7 + gw ) );
                            renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);

                            // right side
                            if( lw > 0) renderSlab(p, QRect(r.right() - lw-7+gw, r.y()-gw, lw+7, 7), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);

                        } else {

                            // left side
                            if( lw > 0) renderSlab(p, QRect(-gw, r.y()-gw, lw+11, 7), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);

                            // right side
                            QRect slabRect( w+lw-7, r.y()-gw, r.width() - w - lw+7+gw, 7);
                            if( tw ) slabRect.setLeft( qMin( slabRect.left(), tw->rect().right() - rw - 7 ) );
                            renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);

                        }

                    }

                    return true;

                    case TabSouth:
                    if( w+lw > 0 )
                    {
                        renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top | TileSet::Right);
                        if(reverseLayout)
                        {

                            // Left and right widgets are placed right and left when in reverse mode
                            // need to clip the painter to avoid overlapping shadow
                            // on can't reduce the rect because 7 is the minimum height for tileset rendering
                            QRect slabRect( -gw, r.bottom()-7+gw, r.width() - w - lw + 7 + gw, 7 );
                            QRect clipRect( -gw, r.bottom()-6+gw, r.width() - w - lw + 7 + gw, 6 );
                            if( tw )
                            {
                                slabRect.setRight( qMax( slabRect.right(), tw->rect().left() + rw + 7 + gw ) );
                                clipRect.setRight( qMax( slabRect.right(), tw->rect().left() + rw + 7 + gw ) );
                            }

                            p->save();
                            p->setClipRect( clipRect );
                            renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);
                            p->restore();

                            if( lw > 0) renderSlab(p, QRect(r.right() - lw-7+gw, r.bottom()-7+gw, lw+7, 7), pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);

                        } else {

                            if( lw > 0) renderSlab(p, QRect(-gw, r.bottom()-7+gw, lw+7+gw, 7), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);

                            // need to clip the painter to avoid overlapping shadow
                            // on can't reduce the rect because 7 is the minimum height for tileset rendering
                            QRect slabRect( w+lw-7, r.bottom()-7+gw, r.width() - w - lw+7+gw, 7 );
                            QRect clipRect( w+lw-7, r.bottom()-6+gw, r.width() - w - lw+7+gw, 6 );
                            if( tw )
                            {
                                slabRect.setLeft( qMin( slabRect.left(), tw->rect().right() - rw - 7 ) );
                                clipRect.setLeft( qMin( slabRect.left(), tw->rect().right() - rw - 7 ) );
                            }

                            p->save();
                            p->setClipRect( clipRect );
                            renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                            p->restore();

                        }

                    }
                    return true;

                    case TabWest:
                    if( h+lh > 0 )
                    {

                        renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right | TileSet::Bottom);

                        if( lh > 0) renderSlab(p, QRect(r.x()-gw, r.y()-gw, 7, lh+7 + 1), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);
                        QRect slabRect( r.x()-gw, r.y()+h+lh-7, 7, r.height() - h - lh+7+gw);
                        if( tw ) slabRect.setTop( qMin( slabRect.top(), tw->rect().bottom() - rh -7 ) );
                        renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);

                    }
                    return true;

                    case TabEast:
                    if( h+lh > 0 )
                    {
                        renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left | TileSet::Bottom);

                        if( lh > 0) renderSlab(p, QRect(r.right()+1-7+gw, r.y()-gw, 7, lh+7+gw), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);

                        QRect slabRect(r.right()+1-7+gw, r.y()+h+lh-7, 7, r.height() - h - lh+7+gw );
                        if( tw ) slabRect.setTop( qMin( slabRect.top(), tw->rect().bottom() - rh -7 ) );
                        renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);

                    }
                    return true;

                    default: return true;
                }
            }

            default: return false;

        }
    }

    //_________________________________________________________
    bool Style::drawWindowPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        switch (primitive)
        {
            case Generic::Frame:
            {
                _helper.drawFloatFrame(p, r, pal.window().color(), false );
                return true;
            }

            case Generic::Text:
            {

                const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt);
                const bool enabled = flags & State_Enabled;
                const bool active = enabled && (tb->titleBarState & Qt::WindowActive );

                // enable state transition
                QPalette palette( pal );
                animations().widgetEnabilityEngine().updateState( widget, AnimationEnable, active );
                if( animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
                { palette = _helper.mergePalettes( pal, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }

                KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
                palette.setCurrentColorGroup( active ? QPalette::Active: QPalette::Disabled );
                drawItemText( p, r, Qt::AlignVCenter | textOpts->hAlign, palette, active, textOpts->text, QPalette::WindowText);

                return true;
            }

            case Window::TitlePanel: return true;

            // menu button. Use icon if available
            case Window::ButtonMenu:
            if( r.isValid() )
            {

                KStyle::TitleButtonOption* tbkOpts = extractOption<KStyle::TitleButtonOption*>(kOpt);
                if( !tbkOpts->icon.isNull() ) tbkOpts->icon.paint(p, r);
                return true;

            }
            return false;

            // other title bar icons. Use build in pixmaps
            case Window::ButtonMin:
            case Window::ButtonMax:
            case Window::ButtonRestore:
            case Window::ButtonClose:
            case Window::ButtonShade:
            case Window::ButtonUnshade:
            case Window::ButtonHelp:
            if( r.isValid() )
            {
                KStyle::TitleButtonOption* tbkOpts = extractOption<KStyle::TitleButtonOption*>(kOpt);
                p->save();
                p->drawPixmap(r.topLeft(), _helper.windecoButton(pal.window().color(), tbkOpts->active,  r.height()));
                p->setRenderHints(QPainter::Antialiasing);
                p->setBrush(Qt::NoBrush);

                const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt);
                const bool enabled = flags & State_Enabled;
                const bool active = enabled && (tb->titleBarState & Qt::WindowActive );

                // enable state transition
                QPalette palette( pal );
                animations().widgetEnabilityEngine().updateState( widget, AnimationEnable, active );
                if( animations().widgetEnabilityEngine().isAnimated( widget, AnimationEnable ) )
                { palette = _helper.mergePalettes( pal, animations().widgetEnabilityEngine().opacity( widget, AnimationEnable )  ); }

                const bool sunken( flags&State_Sunken );
                const bool mouseOver = (!sunken) && widget && r.translated( widget->mapToGlobal( QPoint(0,0) ) ).contains( QCursor::pos() );

                animations().mdiWindowEngine().updateState( widget, primitive, enabled && mouseOver );
                const bool animated( enabled && animations().mdiWindowEngine().isAnimated( widget, primitive ) );
                const qreal opacity( animations().mdiWindowEngine().opacity( widget, primitive ) );

                {

                    // contrast pixel
                    QColor contrast = _helper.calcLightColor( pal.color( QPalette::Active, QPalette::WindowText ) );

                    qreal width( 1.1 );
                    p->translate(0, 0.5);
                    p->setPen(QPen( contrast, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    renderWindowIcon(p, QRectF(r).adjusted(-2.5,-2.5,0,0), primitive);

                }

                {

                    // button color
                    QColor color;
                    if( animated )
                    {

                        QColor base( palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText ) );
                        QColor glow( ( primitive == Window::ButtonClose ) ?
                            _helper.viewNegativeTextBrush().brush( palette ).color():
                            _helper.viewHoverBrush().brush( palette ).color() );

                        color = KColorUtils::mix( base, glow, opacity );

                    } else if( mouseOver ) {

                        color = ( primitive == Window::ButtonClose ) ?
                            _helper.viewNegativeTextBrush().brush( palette ).color():
                            _helper.viewHoverBrush().brush( palette ).color();

                    } else {

                        color = palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText );

                    }

                    // main icon painting
                    qreal width( 1.1 );
                    p->translate(0,-1);
                    p->setPen(QPen( color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    renderWindowIcon(p, QRectF(r).adjusted(-2.5,-2.5,0,0), primitive);

                }

                p->restore();
            }
            return true;

            default: return false;

        }
    }

    //_________________________________________________________
    bool Style::drawSplitterPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( opt );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & (State_MouseOver|State_Sunken) ));

        bool animated( false );
        qreal opacity( AnimationData::OpacityInvalid );

        if( enabled )
        {
            // try retrieve QSplitterHandle, from painter device.
            if( const QSplitterHandle* handle = dynamic_cast<const QSplitterHandle*>(p->device()) )
            {

                animations().widgetStateEngine().updateState( handle, AnimationHover, mouseOver );
                animated = animations().widgetStateEngine().isAnimated( handle, AnimationHover );
                opacity = animations().widgetStateEngine().opacity( handle, AnimationHover );

            } else if( widget && widget->inherits( "QMainWindow" ) ) {

                // get orientation
                Qt::Orientation orientation( flags & QStyle::State_Horizontal ? Qt::Horizontal : Qt::Vertical );
                animations().dockSeparatorEngine().updateRect( widget, r, orientation, mouseOver );
                animated = animations().dockSeparatorEngine().isAnimated( widget, r, orientation );
                opacity = animated ? animations().dockSeparatorEngine().opacity( widget, orientation ) : AnimationData::OpacityInvalid;

            }
        }

        switch (primitive)
        {
            case Splitter::HandleHor:
            {
                int h = r.height();
                QColor color = pal.color(QPalette::Background);

                if( animated || mouseOver )
                {
                    QColor highlight = _helper.alphaColor(_helper.calcLightColor(color),0.5*( animated ? opacity:1.0 ) );
                    qreal a( r.height() > 30 ? 10.0/r.height():0.1 );
                    QLinearGradient lg( 0, r.top(), 0, r.bottom() );
                    lg.setColorAt(0, Qt::transparent );
                    lg.setColorAt(a, highlight );
                    lg.setColorAt(1.0-a, highlight );
                    lg.setColorAt(1, Qt::transparent );
                    p->fillRect( r, lg );
                }

                int ngroups = qMax(1,h / 250);
                int center = (h - (ngroups-1) * 250) /2 + r.top();
                for(int k = 0; k < ngroups; k++, center += 250) {
                    renderDot(p, QPointF(r.left()+1, center-3), color);
                    renderDot(p, QPointF(r.left()+1, center), color);
                    renderDot(p, QPointF(r.left()+1, center+3), color);
                }
                return true;
            }

            case Splitter::HandleVert:
            {
                int w = r.width();
                QColor color = pal.color(QPalette::Background);

                if( animated || mouseOver )
                {
                    QColor highlight = _helper.alphaColor(_helper.calcLightColor(color),0.5*( animated ? opacity:1.0 ) );
                    qreal a( r.width() > 30 ? 10.0/r.width():0.1 );
                    QLinearGradient lg( r.left(), 0, r.right(), 0 );
                    lg.setColorAt(0, Qt::transparent );
                    lg.setColorAt(a, highlight );
                    lg.setColorAt(1.0-a, highlight );
                    lg.setColorAt(1, Qt::transparent );
                    p->fillRect( r, lg );
                }

                int ngroups = qMax(1, w / 250);
                int center = (w - (ngroups-1) * 250) /2 + r.left();
                for(int k = 0; k < ngroups; k++, center += 250) {
                    renderDot(p, QPointF(center-3, r.top()+1), color);
                    renderDot(p, QPointF(center, r.top()+1), color);
                    renderDot(p, QPointF(center+3, r.top()+1), color);
                }
                return true;
            }

            default: return false;

        }
    }

    //_________________________________________________________
    void Style::drawSliderTickmarks(
        const QStyleOptionSlider* opt,
        QPainter* p,
        const QWidget* widget ) const
    {

        int ticks = opt->tickPosition;
        int available = pixelMetric(PM_SliderSpaceAvailable, opt, widget);
        int interval = opt->tickInterval;
        if( interval < 1 ) interval = opt->pageStep;
        if( interval < 1 ) return;

        QRect r( widget->rect() );
        QPalette pal( widget->palette() );

        const int len = pixelMetric(PM_SliderLength, opt, widget);
        const int fudge = len / 2;
        int current( opt->minimum );

        // Since there is no subrect for tickmarks do a translation here.
        p->save();
        p->translate(r.x(), r.y());

        if( opt->orientation == Qt::Horizontal )
        {
            QColor base( _helper.backgroundColor( pal.color( QPalette::Window ), widget, r.center() ) );
            p->setPen( _helper.calcDarkColor( base ) );
        }

        int tickSize( opt->orientation == Qt::Horizontal ? r.height()/3:r.width()/3 );

        while( current <= opt->maximum )
        {

            int position( sliderPositionFromValue(opt->minimum, opt->maximum, current, available) + fudge );

            // calculate positions
            if( opt->orientation == Qt::Horizontal )
            {
                if( ticks == QSlider::TicksAbove ) p->drawLine( position, 0, position, tickSize );
                else if( ticks == QSlider::TicksBelow ) p->drawLine( position, r.height()-tickSize, position, r.height() );
                else {
                    p->drawLine( position, 0, position, tickSize );
                    p->drawLine( position, r.height()-tickSize, position, r.height() );
                }

            } else {

                QColor base( _helper.backgroundColor( pal.color( QPalette::Window ), widget, QPoint( r.center().x(), position ) ) );
                p->setPen( _helper.calcDarkColor( base ) );

                if( ticks == QSlider::TicksAbove ) p->drawLine( 0, position, tickSize, position );
                else if( ticks == QSlider::TicksBelow ) p->drawLine( r.width()-tickSize, position, r.width(), position );
                else {
                    p->drawLine( 0, position, tickSize, position );
                    p->drawLine( r.width()-tickSize, position, r.width(), position );
                }
            }

            // go to next position
            int next( current + interval );
            if( next < current ) break;
            current = next;

        }

        p->restore();
    }

    //_________________________________________________________
    bool Style::drawSliderPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( kOpt );

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));

        const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt);
        animations().sliderEngine().updateState( widget, enabled && slider && (slider->activeSubControls & SC_SliderHandle) );

        switch (primitive)
        {
            case Slider::HandleHor:
            case Slider::HandleVert:
            {

                StyleOptions opts = (flags & State_HasFocus ? Focus : StyleOption());
                if( enabled &&  animations().sliderEngine().isAnimated( widget ) )
                {

                    renderSlab(p, r, pal.color(QPalette::Button), opts,  animations().sliderEngine().opacity( widget ), AnimationHover, TileSet::Ring );

                } else {

                    if(slider)
                    { if( (slider->activeSubControls & SC_SliderHandle) && mouseOver ) opts |= Hover; }

                    renderSlab(p, r, pal.color(QPalette::Button), opts);

                }

                return true;
            }

            case Slider::GrooveHor:
            case Slider::GrooveVert:
            {

                bool horizontal = primitive == Slider::GrooveHor;

                if( horizontal) {

                    int center = r.y()+r.height()/2;
                    _helper.groove(pal.color(QPalette::Window), 0.0)->render( QRect( r.left()+1, center-2, r.width()-2, 5  ), p);

                } else {

                    int center = r.x()+r.width()/2;
                    _helper.groove(pal.color(QPalette::Window), 0.0)->render( QRect( center-2, r.top()+1, 5, r.height()-2 ), p);

                }

                return true;
            }

            default: return false;

        }

    }

    //_________________________________________________________
    bool Style::drawSpinBoxPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {
        Q_UNUSED( opt );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));
        bool hasFocus = flags & State_HasFocus;
        const QColor inputColor = enabled?pal.color(QPalette::Base):pal.color(QPalette::Window);

        switch (primitive)
        {

            case Generic::Frame:
            {
                //QRect fr( r.adjusted(0,1,0,-1) );
                QRect fr( r.adjusted(1,1,-1,-1) );
                p->save();
                p->setRenderHint(QPainter::Antialiasing);
                p->setPen(Qt::NoPen);
                p->setBrush(inputColor);


                const QStyleOptionSpinBox* sbOpt( qstyleoption_cast<const QStyleOptionSpinBox*>( opt ) );
                if( sbOpt && !sbOpt->frame )
                {
                    // frameless spinbox
                    // frame is adjusted to have the same dimensions as a frameless editor
                    p->fillRect(r, inputColor);
                    p->restore();

                } else {

                    // normal spinbox
                    //_helper.fillHole(*p, r.adjusted( -1, -1, 1, 0 ) );
                    _helper.fillHole(*p, r.adjusted( 0, -1, 0, 0 ) );
                    p->restore();

                    // TODO use widget background role?
                    // We really need the color of the widget behind to be "right",
                    // but the shadow needs to be colored as the inner widget; needs
                    // changes in helper.

                    QColor local( pal.color(QPalette::Window) );
                    animations().lineEditEngine().updateState( widget, AnimationHover, mouseOver );
                    animations().lineEditEngine().updateState( widget, AnimationFocus, hasFocus );
                    if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationFocus ) )
                    {

                        _helper.renderHole( p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, AnimationFocus ), AnimationFocus, TileSet::Ring);

                    } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationHover ) ) {

                        _helper.renderHole( p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, AnimationHover ), AnimationHover, TileSet::Ring);

                    } else {

                        _helper.renderHole( p, local, fr, hasFocus, mouseOver);

                    }

                }

                return true;
            }

            case SpinBox::EditField:
            case SpinBox::ButtonArea:
            case SpinBox::UpButton:
            case SpinBox::DownButton:
            return true;

            default: return false;

        }

    }

    //_________________________________________________________
    bool Style::drawComboBoxPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));
        bool editable = false;

        if( const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) )
        { editable = cb->editable; }

        bool hasFocus = flags & State_HasFocus;
        StyleOptions opts = (flags & State_HasFocus ? Focus : StyleOption());
        if( mouseOver) opts |= Hover;

        const QColor inputColor = enabled ? pal.color(QPalette::Base) : pal.color(QPalette::Window);
        QRect editField = subControlRect(CC_ComboBox, qstyleoption_cast<const QStyleOptionComplex*>(opt), SC_ComboBoxEditField, widget);

        if( editable )
        {

            // focus takes precedence over hover for editable comboboxes
            animations().lineEditEngine().updateState( widget, AnimationFocus, hasFocus );
            animations().lineEditEngine().updateState( widget, AnimationHover, mouseOver && !hasFocus );

        } else {

            // hover takes precedence over focus for read-only comboboxes
            animations().lineEditEngine().updateState( widget, AnimationHover, mouseOver );
            animations().lineEditEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

        }

        switch (primitive)
        {
            case Generic::Frame:
            {
                const QStyleOptionComboBox* cbOpt = qstyleoption_cast<const QStyleOptionComboBox *>(opt);

                // TODO: pressed state
                if(!editable)
                {

                    if( cbOpt && !cbOpt->frame )
                    {

                        // do nothing.

                    } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationHover ) ) {

                        qreal opacity( animations().lineEditEngine().opacity( widget, AnimationHover ) );
                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts, opacity, AnimationHover, TileSet::Ring );

                    } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationFocus ) ) {

                        qreal opacity( animations().lineEditEngine().opacity( widget, AnimationFocus ) );
                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts, opacity, AnimationFocus, TileSet::Ring );

                    } else {

                        renderButtonSlab(p, r, pal.color(QPalette::Button), opts);

                    }

                } else {

                    QRect fr = r.adjusted(1,1,-1,-1);

                    // input area
                    p->save();
                    p->setRenderHint(QPainter::Antialiasing);
                    p->setPen(Qt::NoPen);
                    p->setBrush(inputColor);

                    if( cbOpt && !cbOpt->frame )
                    {

                        // adjust rect to match frameLess editors
                        p->fillRect(r, inputColor);
                        p->restore();

                    } else {


                        _helper.fillHole(*p, r.adjusted( 0, -1, 0, 0 ) );
                        p->restore();

                        QColor local( pal.color(QPalette::Window) );
                        if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationFocus ) )
                        {

                            _helper.renderHole( p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, AnimationFocus ), AnimationFocus, TileSet::Ring);

                        } else if( enabled && animations().lineEditEngine().isAnimated( widget, AnimationHover ) ) {

                            _helper.renderHole( p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, AnimationHover ), AnimationHover, TileSet::Ring);

                        } else {

                            _helper.renderHole( p, local, fr, hasFocus && enabled, mouseOver);

                        }

                    }

                }

                return true;
            }

            case ComboBox::EditField: return true;
            case ComboBox::Button: return true;

            case Generic::ArrowDown:
            {
                KStyle::ColorOption* colorOpt   = extractOption<KStyle::ColorOption*>(kOpt);
                colorOpt->color = ColorMode( editable ? QPalette::Text : QPalette::ButtonText );
                drawGenericPrimitive( WT_ComboBox, Generic::ArrowDown, opt, r, pal, flags, p, widget, colorOpt );
                return true;
            }

            default: return false;

        }

    }

    //_________________________________________________________
    bool Style::drawHeaderPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( flags );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );
        switch (primitive)
        {
            case Header::SectionHor:
            case Header::SectionVert:
            {

                if( const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt))
                {

                    const bool horizontal( primitive == Header::SectionHor );
                    const bool reverse( opt->direction == Qt::RightToLeft );
                    const bool isFirst( horizontal && ( header->position == QStyleOptionHeader::Beginning ) );
                    const bool isCorner( widget && widget->inherits( "QTableCornerButton" ) );

                    // corner header lines
                    if( isCorner )
                    {

                        if( widget ) _helper.renderWindowBackground(p, r, widget, pal);
                        else p->fillRect( r, pal.color( QPalette::Window ) );
                        if( reverse ) renderHeaderLines( r, pal, p, TileSet::Bottom | TileSet::Left );
                        else renderHeaderLines( r, pal, p, TileSet::Bottom | TileSet::Right );

                    } else renderHeaderBackground( r, pal, p, widget, horizontal, reverse );

                    // dots
                    p->save();
                    p->setPen(pal.color(QPalette::Text));
                    QColor color = pal.color(QPalette::Window);
                    QRect rect(r);

                    if(primitive == Header::SectionHor)
                    {

                        if(header->section != 0 || isFirst )
                        {
                            int center = r.center().y();
                            int pos = reverse ? r.left()+1 : r.right()-1;
                            renderDot(p, QPointF(pos, center-3), color);
                            renderDot(p, QPointF(pos, center), color);
                            renderDot(p, QPointF(pos, center+3), color);
                        }

                    } else {

                        int center = r.center().x();
                        int pos = r.bottom()-1;
                        renderDot(p, QPointF(center-3, pos), color);
                        renderDot(p, QPointF(center, pos), color);
                        renderDot(p, QPointF(center+3, pos), color);

                    }

                    p->restore();
                }

                return true;
            }

            default: return false;
        }
    }

    //_________________________________________________________
    bool Style::drawTreePrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( widget );

        const bool reverseLayout = opt->direction == Qt::RightToLeft;
        switch (primitive)
        {
            case Tree::VerticalBranch:
            case Tree::HorizontalBranch:
            {
                if( OxygenStyleConfigData::viewDrawTreeBranchLines())
                {
                    QBrush brush( KColorUtils::mix( pal.text().color(), pal.background().color(), 0.8 ) );
                    p->fillRect( r, brush );
                }

                return true;
            }

            case Tree::ExpanderOpen:
            case Tree::ExpanderClosed:
            {
                int radius = (r.width() - 4) / 2;
                int centerx = r.x() + r.width()/2;
                int centery = r.y() + r.height()/2;

                const bool enabled = flags & State_Enabled;
                const bool mouseOver(enabled && (flags & State_MouseOver));
                QColor expanderColor( mouseOver ?
                    _helper.viewHoverBrush().brush(pal).color():
                    pal.text().color() );

                if(!OxygenStyleConfigData::viewDrawTriangularExpander())
                {
                    // plus or minus
                    p->save();
                    p->setPen( expanderColor );
                    p->drawLine( centerx - radius, centery, centerx + radius, centery );

                    // Collapsed = On
                    if( primitive == Tree::ExpanderClosed)
                    { p->drawLine( centerx, centery - radius, centerx, centery + radius ); }

                    p->restore();

                } else {

                    KStyle::ColorOption* colorOpt   = extractOption<KStyle::ColorOption*>(kOpt);
                    colorOpt->color = ColorMode( QPalette::Text );

                    p->save();
                    p->translate( centerx, centery );

                    QPolygonF a;

                    // get size from option

                    ArrowSize size = ArrowSmall;
                    switch( OxygenStyleConfigData::viewTriangularExpanderSize() )
                    {
                        case OxygenStyleConfigData::TE_TINY: size = ArrowTiny; break;
                        case OxygenStyleConfigData::TE_NORMAL: size = ArrowNormal; break;
                        default:
                        case OxygenStyleConfigData::TE_SMALL: size = ArrowSmall; break;
                    }

                    if( primitive == Tree::ExpanderClosed )
                    {

                        p->translate( 0.5, 0 );
                        if( reverseLayout ) a = genericArrow( Generic::ArrowLeft, size );
                        else a = genericArrow( Generic::ArrowRight, size );

                    } else {

                        p->translate( 0, 0.5 );
                        a = genericArrow( Generic::ArrowDown, size );

                    }
                    qreal penThickness = 1.2;

                    p->setPen( QPen( expanderColor, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
                    p->setRenderHint(QPainter::Antialiasing);
                    p->drawPolyline( a );
                    p->restore();

                }

                return true;
            }

            default: return false;

        }
    }

    //_________________________________________________________
    bool Style::drawLineEditPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( kOpt );

        const bool enabled = flags & State_Enabled;
        switch (primitive)
        {

            case LineEdit::Panel:
            {
                if( const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
                {

                    const QBrush inputBrush = enabled?pal.base():pal.window();
                    const int lineWidth(panel->lineWidth);

                    if( lineWidth > 0)
                    {
                        p->save();
                        p->setRenderHint(QPainter::Antialiasing);
                        p->setPen(Qt::NoPen);
                        p->setBrush(inputBrush);

                        _helper.fillHole(*p, r.adjusted( 0, -1, 0, 0 ) );
                        drawPrimitive(PE_FrameLineEdit, panel, p, widget);

                        p->restore();

                    } else  {

                        p->fillRect(r.adjusted(2,2,-2,-2), inputBrush);

                    }
                }
                return false;
            }

            default: return false;

        }

    }

    //_________________________________________________________
    bool Style::drawGroupBoxPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {
        Q_UNUSED( opt );
        Q_UNUSED( flags );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );
        switch (primitive)
        {
            case Generic::Frame:
            {

                QColor base( _helper.backgroundColor( pal.color( QPalette::Window ), widget, r.center() ) );
                QColor color( base );

                p->save();
                p->setRenderHint(QPainter::Antialiasing);
                p->setPen(Qt::NoPen);

                QLinearGradient innerGradient(0, r.top()-r.height()+12, 0, r.bottom()+r.height()-19);
                QColor light = _helper.calcLightColor(color);
                light.setAlphaF(0.4);
                innerGradient.setColorAt(0.0, light);
                color.setAlphaF(0.4);
                innerGradient.setColorAt(1.0, color);
                p->setBrush(innerGradient);
                p->setClipRect(r.adjusted(0, 0, 0, -19));
                _helper.fillSlab(*p, r);

                TileSet *slopeTileSet = _helper.slope( base, 0.0);
                p->setClipping(false);
                slopeTileSet->render( r, p );

                p->restore();

                return true;
            }

            case GroupBox::FlatFrame: return true;
            default: return false;

        }

    }

    //_________________________________________________________
    bool Style::drawToolBarPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {
        Q_UNUSED( opt );
        Q_UNUSED( widget );
        Q_UNUSED( kOpt );
        switch (primitive)
        {
            case ToolBar::HandleHor:
            {
                int counter = 1;
                int center = r.left()+r.width()/2;
                for(int j = r.top()+2; j <= r.bottom()-3; j+=3)
                {
                    if(counter%2 == 0)
                    {
                        renderDot(p, QPoint(center+1, j), pal.color(QPalette::Background));
                    } else {
                        renderDot(p, QPoint(center-2, j), pal.color(QPalette::Background));
                    }
                    counter++;
                }
                return true;
            }

            case ToolBar::HandleVert:
            {
                int counter = 1;
                int center = r.top()+r.height()/2;
                for(int j = r.left()+2; j <= r.right()-3; j+=3)
                {
                    if(counter%2 == 0)
                    {
                        renderDot(p, QPoint(j, center+1), pal.color(QPalette::Background));
                    } else {
                        renderDot(p, QPoint(j, center-2), pal.color(QPalette::Background));
                    }

                    counter++;
                }

                return true;
            }

            case ToolBar::Separator:
            {
                if(OxygenStyleConfigData::toolBarDrawItemSeparator())
                {
                    QColor color = pal.color(QPalette::Window);
                    if(flags & State_Horizontal) _helper.drawSeparator(p, r, color, Qt::Vertical);
                    else _helper.drawSeparator(p, r, color, Qt::Horizontal);
                }

                return true;
            }

            case ToolBar::PanelHor:
            case ToolBar::PanelVert:
            {

                // when timeLine is running draw border event if not hovered
                bool toolBarAnimated( animations().toolBarEngine().isFollowMouseAnimated( widget ) );
                QRect animatedRect( animations().toolBarEngine().animatedRect( widget ) );
                bool toolBarIntersected( toolBarAnimated && animatedRect.intersects( r ) );
                if( toolBarIntersected )
                { _helper.slitFocused(_helper.viewFocusBrush().brush(QPalette::Active).color())->render(animatedRect, p); }

                return true;

            }

            default: return false;
        }
    }

    //_________________________________________________________
    bool Style::drawToolButtonPrimitive(
        int primitive,
        const QStyleOption* opt,
        const QRect &r, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( opt );
        Q_UNUSED( kOpt );
        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));
        const bool hasFocus(enabled && (flags & State_HasFocus));
        const bool reverseLayout = opt->direction == Qt::RightToLeft;

        switch (primitive)
        {

            case ToolButton::Panel:
            {

                bool autoRaised( flags & State_AutoRaise );

                // check whether toolbutton is in toolbar
                bool isInToolBar( widget && widget->parent() && widget->parent()->inherits( "QToolBar" ) );

                // toolbar engine
                bool toolBarAnimated( isInToolBar && widget && ( animations().toolBarEngine().isAnimated( widget->parentWidget() ) || animations().toolBarEngine().isFollowMouseAnimated( widget->parentWidget() ) ) );
                QRect animatedRect( (isInToolBar && widget) ? animations().toolBarEngine().animatedRect( widget->parentWidget() ):QRect() );
                QRect childRect( (widget && widget->parentWidget()) ? r.translated( widget->mapToParent( QPoint(0,0) ) ):QRect() );
                QRect currentRect(  widget ? animations().toolBarEngine().currentRect( widget->parentWidget() ):QRect() );
                bool current( isInToolBar && widget && widget->parentWidget() && currentRect.intersects( r.translated( widget->mapToParent( QPoint(0,0) ) ) ) );
                bool toolBarTimerActive( isInToolBar && widget && animations().toolBarEngine().isTimerActive( widget->parentWidget() ) );
                qreal toolBarOpacity( ( isInToolBar && widget ) ? animations().toolBarEngine().opacity( widget->parentWidget() ):0 );

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

                // hover rect
                if( widget )
                {

                    // check if parent is tabbar, in which case one must add some extra
                    // painting below the button
                    if( const QTabBar *tb =  qobject_cast<const QTabBar*>(widget->parent()))
                    {

                        // always autoRaise toobuttons in tabbars
                        autoRaised = true;

                        QPalette::ColorGroup colorGroup = tb->palette().currentColorGroup();
                        QTabWidget* tw( qobject_cast<QTabWidget*>(tb->parent() ) );
                        const bool documentMode( tb->documentMode() || !tw );

                        // get corner widgets if any
                        const QWidget* leftWidget( tw ? tw->cornerWidget( Qt::TopLeftCorner ):0 );
                        const QWidget* rightWidget( tw ? tw->cornerWidget( Qt::TopRightCorner ):0 );

                        // prepare painting, clipping and tiles
                        TileSet::Tiles tiles = 0;
                        QRect slabRect;
                        QRect clipRect;

                        // paint relevant region depending on tabbar shape and whether widget is on the edge
                        switch( tabOrientation( tb->shape() ) )
                        {
                            case TabNorth:
                            {

                                // need to swap left and right widgets in reverse mode
                                if( reverseLayout ) qSwap( leftWidget, rightWidget );

                                clipRect = r.adjusted(0,-gw,0,-4);
                                tiles = TileSet::Top;

                                // check border right
                                if( !documentMode && !rightWidget && widget->geometry().right() >= tb->rect().right() )
                                {

                                    tiles |= TileSet::Right;
                                    slabRect = QRect(r.left()-7, r.bottom()-6-gw, r.width()+7+1, 5);

                                } else if( !documentMode && !leftWidget && widget->geometry().left() <= tb->rect().left() ) {

                                    tiles |= TileSet::Left;
                                    slabRect = QRect(r.left()-1, r.bottom()-6-gw, r.width()+7+1, 5);

                                } else {

                                    slabRect = QRect(r.left()-7, r.bottom()-6-gw, r.width()+14, 5);

                                }

                                break;
                            }

                            case TabSouth:
                            {

                                // need to swap left and right widgets in reverse mode
                                if( reverseLayout ) qSwap( leftWidget, rightWidget );

                                tiles = TileSet::Bottom;
                                clipRect = r.adjusted(0,2+gw,0,0);

                                if( !documentMode && !rightWidget && widget->geometry().right() >= tb->rect().right() )
                                {

                                    tiles |= TileSet::Right;
                                    slabRect = QRect(r.left()-7, r.top()+gw+1, r.width()+7+1, 5);

                                } else if( !documentMode && !leftWidget && widget->geometry().left() <= tb->rect().left() ) {

                                    tiles |= TileSet::Left;
                                    slabRect = QRect(r.left()-1, r.top()+gw+1, r.width()+7+1, 5);

                                } else {

                                    slabRect = QRect(r.left()-7, r.top()+gw+1, r.width()+14, 5);

                                }

                                break;
                            }

                            case TabEast:
                            {

                                tiles = TileSet::Right;
                                clipRect = r.adjusted(3+gw,0,-2,0);
                                if( !documentMode && !rightWidget && widget->geometry().bottom() >= tb->rect().bottom() )
                                {
                                    tiles |= TileSet::Bottom;
                                    slabRect = QRect(r.left()+gw+3, r.top()-7, 4, r.height()+7+1);

                                } else {

                                    slabRect = QRect(r.left()+gw+3, r.top()-6, 4, r.height()+14);

                                }

                                break;
                            }


                            case TabWest:
                            {

                                // west
                                tiles |= TileSet::Left;
                                clipRect = r.adjusted(2-gw,0,-3, 0);

                                if( !documentMode && !rightWidget && widget->geometry().bottom() >= tb->rect().bottom() )
                                {

                                    tiles |= TileSet::Bottom;
                                    slabRect = QRect(r.right()-6-gw, r.top()-7, 5, r.height()+7+1);

                                } else {

                                    slabRect = QRect(r.right()-6-gw, r.top()-6, 5, r.height()+14);

                                }

                                break;
                            }

                            default:
                            break;
                        }

                        if( clipRect.isValid() )
                        {
                            QPalette local( widget->parentWidget() ? widget->parentWidget()->palette() : pal );

                            // check whether parent has autofill background flag
                            if( const QWidget* parent = checkAutoFillBackground( widget ) ) p->fillRect( clipRect, parent->palette().color( parent->backgroundRole() ) );
                            else _helper.renderWindowBackground(p, clipRect, widget, local);

                        }

                        if( slabRect.isValid() )
                        {
                            p->save();
                            p->setClipRect( slabRect );
                            renderSlab(p, slabRect, pal.color(colorGroup, QPalette::Window), NoFill, tiles );
                            p->restore();
                        }

                        // end painting here.
                        // no slitRect for tabbar arrow buttons
                        return true;

                    }

                }

                // slit rect
                QRect slitRect = r;

                // non autoraised tool buttons get same slab as regular buttons
                if( widget && !autoRaised )
                {

                    StyleOptions opts = 0;

                    // "normal" parent, and non "autoraised" (that is: always raised) buttons
                    if( (flags & State_On) || (flags & State_Sunken)) opts |= Sunken;
                    if( flags & State_HasFocus) opts |= Focus;
                    if( enabled && (flags & State_MouseOver)) opts |= Hover;


                    TileSet::Tiles tiles = TileSet::Ring;

                    // adjust tiles and rect in case of menubutton
                    if( const QToolButton* t = qobject_cast<const QToolButton*>( widget ) )
                    {
                        if( t->popupMode()==QToolButton::MenuButtonPopup )
                        {
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

                    // render slab
                    renderButtonSlab( p, slitRect, pal.color(QPalette::Button), opts, opacity, mode, tiles );

                    return true;

                }

                //! fine tuning of slitRect geometry
                if( widget && widget->inherits( "QDockWidgetTitleButton" ) ) slitRect.adjust( 1, 0, 0, 0 );
                else if( widget && widget->inherits( "QToolBarExtension" ) ) slitRect.adjust( 1, 1, -1, -1 );
                else if( widget && widget->objectName() == "qt_menubar_ext_button" ) slitRect.adjust( -1, -1, 0, 0 );

                // normal (auto-raised) toolbuttons
                bool hasFocus = flags & State_HasFocus;

                if((flags & State_Sunken) || (flags & State_On) )
                {
                    if( enabled && hoverAnimated )
                    {

                        _helper.renderHole( p, pal.color(QPalette::Window), slitRect, hasFocus, mouseOver, hoverOpacity, AnimationHover, TileSet::Ring );

                    } else if( toolBarAnimated ) {

                        if( enabled && animatedRect.isNull() && current  )
                        {

                            _helper.renderHole( p, pal.color(QPalette::Window), slitRect, hasFocus, mouseOver, toolBarOpacity, AnimationHover, TileSet::Ring );

                        } else {

                            _helper.renderHole( p, pal.color(QPalette::Window), slitRect, false, false);

                        }

                    } else if( toolBarTimerActive && current ) {

                        _helper.renderHole( p, pal.color(QPalette::Window), slitRect, hasFocus, true );

                    } else {

                        _helper.renderHole( p, pal.color(QPalette::Window), slitRect, hasFocus, mouseOver);

                    }

                } else {

                    if( enabled && hoverAnimated ) {

                        QColor glow( _helper.alphaColor( _helper.viewFocusBrush().brush(QPalette::Active).color(), hoverOpacity ) );
                        _helper.slitFocused( glow )->render(slitRect, p);

                    } else if( toolBarAnimated ) {

                        if( enabled && animatedRect.isNull() && current )
                        {
                            QColor glow( _helper.alphaColor( _helper.viewFocusBrush().brush(QPalette::Active).color(), toolBarOpacity ) );
                            _helper.slitFocused( glow )->render(slitRect, p);
                        }

                    } else if( hasFocus || mouseOver || (toolBarTimerActive && current ) ) {

                        _helper.slitFocused(_helper.viewFocusBrush().brush(QPalette::Active).color())->render(slitRect, p);

                    }

                }

                return true;
            }

            default: return false;

        }

    }


    //_________________________________________________________
    bool Style::drawGenericPrimitive(
        WidgetType widgetType,
        int primitive,
        const QStyleOption* opt,
        const QRect &rect, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        switch (primitive)
        {

            case Generic::ArrowUp:
            case Generic::ArrowDown:
            case Generic::ArrowLeft:
            case Generic::ArrowRight:
            { return drawGenericArrow( widgetType, primitive, opt, rect, pal, flags, p, widget, kOpt ); }

            case Generic::Frame:
            { return drawGenericFrame( widgetType, primitive, opt, rect, pal, flags, p, widget, kOpt ); }

            case Generic::FocusIndicator:
            { return drawFocusIndicator( widgetType, primitive, opt, rect, pal, flags, p, widget, kOpt ); }

            default: return false;
        }

    }

    //_________________________________________________________
    bool Style::drawGenericArrow(
        WidgetType widgetType,
        int primitive,
        const QStyleOption* opt,
        const QRect &rect, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        const bool enabled = flags & State_Enabled;
        const bool mouseOver(enabled && (flags & State_MouseOver));
        const bool sunken(enabled && (flags & State_Sunken));
        StyleOptions opts = 0;

        QRect r( rect );
        p->save();

        // define gradient and polygon for drawing arrow
        QPolygonF a = genericArrow( primitive, ArrowNormal );

        qreal penThickness = 1.6;
        bool drawContrast = true;
        KStyle::ColorOption* colorOpt = extractOption<KStyle::ColorOption*>(kOpt);
        QColor color = colorOpt->color.color(pal);
        QColor background = pal.color(QPalette::Window);

        // customize color and adjust position depending on widget
        if( widgetType == WT_PushButton )
        {

            r.translate( 0, 1 );

        } else if( widgetType == WT_SpinBox ) {

            // get subcontrol type
            SubControl subControl;
            if( primitive == Generic::ArrowUp ) subControl = SC_SpinBoxUp;
            else if( primitive == Generic::ArrowDown ) subControl = SC_SpinBoxDown;
            else subControl = SC_None;

            // try cast option
            const QStyleOptionSpinBox *sbOpt = qstyleoption_cast<const QStyleOptionSpinBox *>(opt);
            const bool subControlHover( enabled && mouseOver && sbOpt && subControl != SC_None && (sbOpt->activeSubControls&subControl) );

            // check animation state
            animations().spinBoxEngine().updateState( widget, subControl, subControlHover );
            const bool animated( enabled && animations().spinBoxEngine().isAnimated( widget, subControl ) );
            qreal opacity( animations().spinBoxEngine().opacity( widget, subControl ) );

            if( animated )
            {

                QColor highlight = _helper.viewHoverBrush().brush(pal).color();
                color = KColorUtils::mix( pal.color( QPalette::Text ), highlight, opacity );

            } else if( subControlHover ) {

                color = _helper.viewHoverBrush().brush(pal).color();

            } else {

                color = pal.color( QPalette::Text );

            }

            background = pal.color( QPalette::Background );
            drawContrast = false;

        } else if( widgetType == WT_ComboBox ) {

            // combobox
            if( const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) )
            {

                const QComboBox* comboBox = qobject_cast<const QComboBox*>( widget );
                bool empty( comboBox && !comboBox->count() );

                if( cb->editable )
                {

                    if( enabled && empty ) color = pal.color( QPalette::Disabled,  QPalette::Text );
                    else {

                        // check animation state
                        const bool subControlHover( enabled && mouseOver && cb->activeSubControls&SC_ComboBoxArrow );
                        animations().comboBoxEngine().updateState( widget, AnimationHover, subControlHover  );

                        const bool animated( enabled && animations().comboBoxEngine().isAnimated( widget, AnimationHover ) );
                        const qreal opacity( animations().comboBoxEngine().opacity( widget, AnimationHover ) );

                        if( animated )
                        {

                            QColor highlight = _helper.viewHoverBrush().brush(pal).color();
                            color = KColorUtils::mix( pal.color( QPalette::Text ), highlight, opacity );

                        } else if( subControlHover ) {

                            color = _helper.viewHoverBrush().brush(pal).color();

                        } else {

                            color = pal.color( QPalette::Text );

                        }

                    }

                    background = pal.color( QPalette::Background );

                    if( enabled ) drawContrast = false;
                    r.translate( 0, 1 );

                } else {

                    if( enabled && empty ) color = pal.color( QPalette::Disabled,  QPalette::ButtonText );
                    else color  = pal.color( QPalette::ButtonText );
                    background = pal.color( QPalette::Button );

                }

            }
        } else if( widgetType == WT_Header ) {

            if( mouseOver ) color = _helper.viewHoverBrush().brush(pal).color();

        } else if( widgetType == WT_ScrollBar ) {

            const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt);
            const bool vertical = (slider && slider->orientation == Qt::Vertical);
            const bool reverseLayout = (opt->direction == Qt::RightToLeft);

            // handle scrollbar arrow hover
            // first get relevant subcontrol type matching arrow
            SubControl subcontrol( SC_None );
            if( vertical )  subcontrol = (primitive == Generic::ArrowDown) ? SC_ScrollBarAddLine:SC_ScrollBarSubLine;
            else if( reverseLayout ) subcontrol = (primitive == Generic::ArrowLeft) ? SC_ScrollBarAddLine:SC_ScrollBarSubLine;
            else subcontrol = (primitive == Generic::ArrowLeft) ? SC_ScrollBarSubLine:SC_ScrollBarAddLine;

            if( enabled )
            {

                bool hover( animations().scrollBarEngine().isHovered( widget, subcontrol ) );
                bool animated( animations().scrollBarEngine().isAnimated( widget, subcontrol ) );
                qreal opacity( animations().scrollBarEngine().opacity( widget, subcontrol ) );

                QPoint position( hover ? widget->mapFromGlobal( QCursor::pos() ) : QPoint( -1, -1 ) );
                if( hover && r.contains( position ) )
                {
                    // we need to update the arrow subcontrolRect on fly because there is no
                    // way to get it from the styles directly, outside of repaint events
                    animations().scrollBarEngine().setSubControlRect( widget, subcontrol, r );
                }

                if( r.intersects(  animations().scrollBarEngine().subControlRect( widget, subcontrol ) ) )
                {

                    QColor highlight = _helper.viewHoverBrush().brush(pal).color();
                    if( animated )
                    {
                        color = KColorUtils::mix( color, highlight, opacity );

                    } else if( hover ) {

                        color = highlight;

                    }

                }

            }

        } else if( const QToolButton *tool = qobject_cast<const QToolButton *>(widget)) {

            QColor highlight = _helper.viewHoverBrush().brush(pal).color();
            color = pal.color( QPalette::WindowText );

            // toolbuttons
            if( tool->popupMode()==QToolButton::MenuButtonPopup )
            {

                if(!tool->autoRaise())
                {

                    const bool hasFocus(enabled && (flags & State_HasFocus));

                    // mouseOver has precedence over focus
                    animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
                    animations().widgetStateEngine().updateState( widget, AnimationFocus, hasFocus && !mouseOver );

                    bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
                    bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, AnimationFocus ) );

                    qreal hoverOpacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );
                    qreal focusOpacity( animations().widgetStateEngine().opacity( widget, AnimationFocus ) );

                    color = pal.color( QPalette::ButtonText );
                    background = pal.color( QPalette::Button );

                    // arrow rect
                    QRect frameRect( r.adjusted(-10,0,0,0) );
                    if( (flags & State_On) || (flags & State_Sunken) )
                    {
                        frameRect.adjust( 2, 0, 0, -1 );
                        opts |= Sunken;
                    }

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

                    p->save();
                    p->setClipRect( frameRect.adjusted( 6, 0, 0, 0 ), Qt::IntersectClip );
                    renderSlab(p, frameRect, pal.color(QPalette::Button), opts, opacity, mode, TileSet::Bottom | TileSet::Top | TileSet::Right);
                    p->restore();

                    a.translate(-3,1);

                    QColor color = pal.color(QPalette::Window);
                    QColor light = _helper.calcLightColor(color);
                    QColor dark = _helper.calcDarkColor(color);
                    dark.setAlpha(200);
                    light.setAlpha(150);
                    int yTop( r.top()+2 );
                    if( sunken ) yTop += 1;
                    int yBottom( r.bottom()-4 );
                    p->setPen(QPen(light,1));

                    p->drawLine(r.x()-5, yTop+1, r.x()-5, yBottom);
                    p->drawLine(r.x()-3, yTop+2, r.x()-3, yBottom);
                    p->setPen(QPen(dark,1));
                    p->drawLine(r.x()-4, yTop, r.x()-4, yBottom);

                }

                // handle arrow over animation
                if( const QStyleOptionToolButton *tbOption = qstyleoption_cast<const QStyleOptionToolButton *>(opt) )
                {

                    const bool arrowHover( enabled && mouseOver && (tbOption->activeSubControls & SC_ToolButtonMenu));
                    animations().toolButtonEngine().updateState( widget, AnimationHover, arrowHover );
                    bool animated( enabled && animations().toolButtonEngine().isAnimated( widget, AnimationHover ) );
                    qreal opacity( animations().toolButtonEngine().opacity( widget, AnimationHover) );

                    if( animated ) color = KColorUtils::mix( color, highlight, opacity );
                    else if( arrowHover ) color = highlight;
                    else color = pal.color( QPalette::WindowText );

                }

            } else {

                // toolbutton animation
                // when the arrow is painted directly on the icon, button hover and arrow hover
                // are identical. The generic widget engine is used.
                animations().widgetStateEngine().updateState( widget, AnimationHover, mouseOver );
                bool animated( animations().widgetStateEngine().isAnimated( widget, AnimationHover ) );
                qreal opacity( animations().widgetStateEngine().opacity( widget, AnimationHover ) );

                if( animated ) color = KColorUtils::mix( color, highlight, opacity );
                else if( mouseOver ) color = highlight;
                else color = pal.color( QPalette::WindowText );

                // smaller down arrow for menu indication on toolbuttons
                penThickness = 1.4;
                a = genericArrow( primitive, ArrowSmall );

            }
        }

        // white reflection
        p->translate(int(r.x()+r.width()/2), int(r.y()+r.height()/2));
        p->setRenderHint(QPainter::Antialiasing);

        qreal offset( qMin( penThickness, qreal(1.0)) );
        if( drawContrast )
        {

            p->translate(0,offset);
            p->setPen(QPen(_helper.calcLightColor(pal.color(QPalette::Window)), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p->drawPolyline(a);
            p->translate(0,-offset);

        }

        p->setPen(QPen( _helper.decoColor( background, color ) , penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p->drawPolyline(a);
        p->restore();

        return true;
    }

    //_________________________________________________________
    bool Style::drawGenericFrame(
        WidgetType widgetType,
        int primitive,
        const QStyleOption* opt,
        const QRect &rect, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( primitive );
        Q_UNUSED( opt );
        Q_UNUSED( kOpt );

        const bool enabled = flags & State_Enabled;

        QRect r( rect );
        const bool isInputWidget( widget && widget->testAttribute( Qt::WA_Hover ) );
        const bool hoverHighlight( enabled && isInputWidget && (flags&State_MouseOver) );

        //const bool focusHighlight( enabled && isInputWidget && (flags&State_HasFocus) );
        bool focusHighlight(false);
        if( enabled && isInputWidget && (flags&State_HasFocus) ) focusHighlight = true;
        else if( isKTextEditFrame( widget ) && widget->parentWidget()->hasFocus() )
        { focusHighlight = true; }

        // assume focus takes precedence over hover
        animations().lineEditEngine().updateState( widget, AnimationFocus, focusHighlight );
        animations().lineEditEngine().updateState( widget, AnimationHover, hoverHighlight && !focusHighlight );
        if( flags & State_Sunken)
        {
            QRect local( r.adjusted( 1, 1, -1, -1 ) );
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

                _helper.renderHole(
                    p, pal.color(QPalette::Window), local, focusHighlight, hoverHighlight,
                    opacity, mode, TileSet::Ring );

            }

        } else if(widgetType == WT_Generic && (flags & State_Raised)) {

            renderSlab(p, r.adjusted(-1, -1, 1, 1), pal.color(QPalette::Background), NoFill);

        }

        return true;
    }

    //_________________________________________________________
    bool Style::drawFocusIndicator(
        WidgetType widgetType,
        int primitive,
        const QStyleOption* opt,
        const QRect &rect, const QPalette &pal,
        State flags, QPainter* p,
        const QWidget* widget,
        KStyle::Option* kOpt) const
    {

        Q_UNUSED( widgetType );
        Q_UNUSED( primitive );
        Q_UNUSED( opt );
        Q_UNUSED( kOpt );

        if( const QAbstractItemView *aiv = qobject_cast<const QAbstractItemView*>(widget) )
        {
            if( OxygenStyleConfigData::viewDrawFocusIndicator() &&
                aiv->selectionMode() != QAbstractItemView::SingleSelection &&
                aiv->selectionMode() != QAbstractItemView::NoSelection)
            {

                const QRect r( rect.adjusted( 2, 0, -2, -2 ) );
                QLinearGradient lg(r.bottomLeft(), r.bottomRight() );
                lg.setColorAt(0.0, Qt::transparent);
                if( flags & State_Selected) {

                    lg.setColorAt(0.2, pal.color(QPalette::BrightText));
                    lg.setColorAt(0.8, pal.color(QPalette::BrightText));

                } else {

                    lg.setColorAt(0.2, pal.color(QPalette::Text));
                    lg.setColorAt(0.8, pal.color(QPalette::Text));

                }

                lg.setColorAt(1.0, Qt::transparent);

                p->save();
                p->setRenderHint(QPainter::Antialiasing, false);
                p->setPen(QPen(lg, 1));
                p->drawLine(r.bottomLeft(), r.bottomRight() );
                p->restore();

            }
        }

        // we don't want the stippled focus indicator in oxygen
        if( !( widget && widget->inherits("Q3ListView") ) ) return true;
        else return false;
    }

    //______________________________________________________________
    void Style::drawCapacityBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const
    {

        // cast option
        const QStyleOptionProgressBar* cbOption( qstyleoption_cast<const QStyleOptionProgressBar*>( option ) );
        if( !cbOption ) return;

        // draw container
        QStyleOptionProgressBarV2 sub_opt(*cbOption);
        sub_opt.rect = subElementRect( QStyle::SE_ProgressBarGroove, cbOption, widget);
        drawControl( QStyle::CE_ProgressBarGroove, &sub_opt, p, widget);

        // draw bar
        sub_opt.rect = subElementRect( QStyle::SE_ProgressBarContents, cbOption, widget);
        drawControl( QStyle::CE_ProgressBarContents, &sub_opt, p, widget);

        // draw label
        sub_opt.rect = subElementRect( QStyle::SE_ProgressBarLabel, cbOption, widget);
        drawControl( QStyle::CE_ProgressBarLabel, &sub_opt, p, widget);

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
            // also need to make the scrollarea background plain (using autofill background)
            // so that optional vertical scrollbar background is not transparent either.
            // TODO: possibly add an event filter to use the "normal" window background
            // instead of something flat.
            scrollArea->setAutoFillBackground( true );
            return;
        }


        // check frame style and background role
        if( scrollArea->frameShape() != QFrame::NoFrame ) return;
        if( scrollArea->backgroundRole() != QPalette::Window ) return;

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

    //______________________________________________________________
    void Style::polish(QWidget* widget)
    {
        if( !widget) return;

        // register widget to animations
        animations().registerWidget( widget );
        transitions().registerWidget( widget );
        windowManager().registerWidget( widget );
        frameShadowFactory().registerWidget( widget, _helper );

        // scroll areas
        if( QAbstractScrollArea* scrollArea = qobject_cast<QAbstractScrollArea*>(widget) )
        {

            polishScrollArea( scrollArea );

        } else if( widget->inherits( "Q3ListView" ) ) {

            widget->installEventFilter(this);
            widget->setAttribute(Qt::WA_Hover);

        }

        // several widgets set autofill background to false, which effectively breaks the background
        // gradient rendering. Instead of patching all concerned applications,
        // we change the background here
        if( widget->inherits( "MessageList::Core::Widget" ) )
        { widget->setAutoFillBackground( false ); }

        // KTextEdit frames
        // static cast is safe here, since isKTextEdit already checks that widget inherits from QFrame
        if( isKTextEditFrame( widget ) && static_cast<QFrame*>(widget)->frameStyle() == (QFrame::StyledPanel | QFrame::Sunken) )
        {

            widget->setAttribute( Qt::WA_Hover );
            animations().lineEditEngine().registerWidget( widget, AnimationHover|AnimationFocus );

        }

        // adjust flags for windows and dialogs
        switch (widget->windowFlags() & Qt::WindowType_Mask)
        {

            case Qt::Window:
            case Qt::Dialog:
            widget->setAttribute(Qt::WA_StyledBackground);
            break;

            default: break;

        }

        if(
            qobject_cast<QAbstractItemView*>(widget)
            || qobject_cast<QAbstractSpinBox*>(widget)
            || qobject_cast<QCheckBox*>(widget)
            || qobject_cast<QComboBox*>(widget)
            || qobject_cast<QDial*>(widget)
            || qobject_cast<QLineEdit*>(widget)
            || qobject_cast<QPushButton*>(widget)
            || qobject_cast<QRadioButton*>(widget)
            || qobject_cast<QScrollBar*>(widget)
            || qobject_cast<QSlider*>(widget)
            || qobject_cast<QSplitterHandle*>(widget)
            || qobject_cast<QTabBar*>(widget)
            || qobject_cast<QTextEdit*>(widget)
            || qobject_cast<QToolButton*>(widget)
            )
        { widget->setAttribute(Qt::WA_Hover); }

        // checkable group boxes
        if( QGroupBox* groupBox = qobject_cast<QGroupBox*>(widget) )
        {

            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover ); }

        } else if( qobject_cast<QAbstractButton*>(widget) && qobject_cast<QDockWidget*>( widget->parent() ) ) {

            widget->setAttribute(Qt::WA_Hover);

        } else if( qobject_cast<QAbstractButton*>(widget) && qobject_cast<QToolBox*>( widget->parent() ) ) {

            widget->setAttribute(Qt::WA_Hover);

        }

        /*
            I do not understand the setContentsMargins below
            nor what effect removing it has on the layout:
            the horizontal alignment of some widgets starts varying from
            toolbar to toolbar if removed.
            (Hugo 05/18/2010)
        */
        if( qobject_cast<QToolBar *>(widget->parent()) )
        { widget->setContentsMargins(0,0,0,1); }

        if( qobject_cast<QToolButton*>(widget) )
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

            widget->setBackgroundRole(QPalette::NoRole);

        } else if( qobject_cast<QMenuBar*>(widget)) {

            widget->setBackgroundRole(QPalette::NoRole);

        } else if( widget->inherits( "KMultiTabBar" ) ) {

            // kMultiTabBar margins are set to unity for alignment
            // with (usually sunken) neighbor frames
            widget->setContentsMargins( 1, 1, 1, 1 );

        } else if( widget->inherits("Q3ToolBar") || qobject_cast<QToolBar*>(widget) ) {

            widget->setBackgroundRole(QPalette::NoRole);
            widget->setAttribute(Qt::WA_TranslucentBackground);
            widget->installEventFilter(this);

            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
            #endif

        } else if( widget->inherits( "QTipLabel" ) ) {

            widget->setBackgroundRole(QPalette::NoRole);
            widget->setAttribute(Qt::WA_TranslucentBackground);

            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
            #endif

        } else if( qobject_cast<QScrollBar*>(widget) ) {

            widget->setAttribute(Qt::WA_OpaquePaintEvent, false);

            // when painted in konsole, one needs to paint the window background below
            // the scrollarea, otherwise an ugly flat background is used
            if( widget->parent() && widget->parent()->inherits( "Konsole::TerminalDisplay" ) )
            { widget->installEventFilter( this ); }

        } else if( qobject_cast<QDockWidget*>(widget)) {

            widget->setBackgroundRole(QPalette::NoRole);
            widget->setAttribute(Qt::WA_TranslucentBackground);
            widget->setContentsMargins(3,3,3,3);
            widget->installEventFilter(this);

        } else if( qobject_cast<QMdiSubWindow*>(widget) ) {

            widget->setAutoFillBackground( false );
            widget->installEventFilter( this );

        } else if( qobject_cast<QToolBox*>(widget)) {

            widget->setBackgroundRole(QPalette::NoRole);
            widget->setAutoFillBackground(false);
            widget->setContentsMargins(5,5,5,5);
            widget->installEventFilter(this);

        } else if( widget->parentWidget() && widget->parentWidget()->parentWidget() && qobject_cast<QToolBox*>(widget->parentWidget()->parentWidget()->parentWidget())) {

            widget->setBackgroundRole(QPalette::NoRole);
            widget->setAutoFillBackground(false);
            widget->parentWidget()->setAutoFillBackground(false);

        } else if( qobject_cast<QMenu*>(widget) ) {

            widget->setAttribute(Qt::WA_TranslucentBackground);
            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
            #endif

        } else if( widget->inherits("QComboBoxPrivateContainer")) {

            widget->installEventFilter(this);
            widget->setAttribute(Qt::WA_TranslucentBackground);
            #ifdef Q_WS_WIN
            //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
            widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
            #endif

        } else if( widget->inherits( "KWin::GeometryTip" ) ) {

            // special handling of kwin geometry tip widget
            widget->installEventFilter(this);
            widget->setAttribute(Qt::WA_NoSystemBackground);
            widget->setAttribute(Qt::WA_TranslucentBackground);
            if( QLabel* label = qobject_cast<QLabel*>( widget ) )
            {
                label->setFrameStyle( QFrame::NoFrame );
                label->setMargin(5);
            }

            #ifdef Q_WS_WIN
            widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
            #endif

        } else if(  qobject_cast<QFrame*>(widget) ) {

            if( qobject_cast<KTitleWidget*>(widget->parentWidget()))
            { widget->setBackgroundRole( QPalette::Window ); }

        }

        // base class polishing
        KStyle::polish(widget);

    }

    //_______________________________________________________________
    void Style::unpolish(QWidget* widget)
    {

        // register widget to animations
        animations().unregisterWidget( widget );
        transitions().unregisterWidget( widget );
        windowManager().unregisterWidget( widget );
        frameShadowFactory().unregisterWidget( widget );

        if( isKTextEditFrame( widget ) )
        { widget->setAttribute( Qt::WA_Hover, false  ); }

        if( widget && widget->inherits( "Q3ListView" ) ) {

            widget->removeEventFilter(this);
            widget->setAttribute(Qt::WA_Hover, false );

        }

        // event filters
        switch (widget->windowFlags() & Qt::WindowType_Mask)
        {

            case Qt::Window:
            case Qt::Dialog:
            widget->removeEventFilter(this);
            widget->setAttribute(Qt::WA_StyledBackground, false);
            break;

            default:
            break;

        }

        // checkable group boxes
        if( QGroupBox* groupBox = qobject_cast<QGroupBox*>(widget) )
        {
            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover ); }
        }

        // hover flags
        if(
            qobject_cast<QAbstractItemView*>(widget)
            || qobject_cast<QAbstractSpinBox*>(widget)
            || qobject_cast<QCheckBox*>(widget)
            || qobject_cast<QComboBox*>(widget)
            || qobject_cast<QDial*>(widget)
            || qobject_cast<QLineEdit*>(widget)
            || qobject_cast<QPushButton*>(widget)
            || qobject_cast<QRadioButton*>(widget)
            || qobject_cast<QScrollBar*>(widget)
            || qobject_cast<QSlider*>(widget)
            || qobject_cast<QSplitterHandle*>(widget)
            || qobject_cast<QTabBar*>(widget)
            || qobject_cast<QTextEdit*>(widget)
            || qobject_cast<QToolButton*>(widget)
            )
        { widget->setAttribute(Qt::WA_Hover, false); }

        // checkable group boxes
        if( QGroupBox* groupBox = qobject_cast<QGroupBox*>(widget) )
        {
            if( groupBox->isCheckable() )
            { groupBox->setAttribute( Qt::WA_Hover, false ); }
        }

        if( qobject_cast<QMenuBar*>(widget)
            || (widget && widget->inherits("Q3ToolBar"))
            || qobject_cast<QToolBar*>(widget)
            || (widget && qobject_cast<QToolBar *>(widget->parent()))
            || qobject_cast<QToolBox*>(widget))
        {
            widget->setBackgroundRole(QPalette::Button);
            widget->removeEventFilter(this);
            widget->clearMask();
        }

        if( widget->inherits( "QTipLabel" ) )
        {

            widget->setAttribute(Qt::WA_PaintOnScreen, false);
            widget->setAttribute(Qt::WA_NoSystemBackground, false);
            widget->clearMask();

        } else if( qobject_cast<QScrollBar*>(widget) ) {

            widget->setAttribute(Qt::WA_OpaquePaintEvent);

        } else if( qobject_cast<QDockWidget*>(widget) ) {

            widget->setContentsMargins(0,0,0,0);
            widget->clearMask();

        } else if( qobject_cast<QToolBox*>(widget) ) {

            widget->setBackgroundRole(QPalette::Button);
            widget->setContentsMargins(0,0,0,0);
            widget->removeEventFilter(this);

        } else if( qobject_cast<QMenu*>(widget) ) {

            widget->setAttribute(Qt::WA_PaintOnScreen, false);
            widget->setAttribute(Qt::WA_NoSystemBackground, false);
            widget->clearMask();

        } else if( widget->inherits("QComboBoxPrivateContainer") ) widget->removeEventFilter(this);

        KStyle::unpolish(widget);

    }

    //_____________________________________________________________________
    void Style::globalSettingsChange(int type, int /*arg*/)
    {

        // reset helper configuration
        if( type == KGlobalSettings::PaletteChanged) _helper.reloadConfig();

        OxygenStyleConfigData::self()->readConfig();

        // reinitialize engines
        animations().setupEngines();
        transitions().setupEngines();
        windowManager().initialize();

        widgetExplorer().setEnabled( OxygenStyleConfigData::widgetExplorerEnabled() );

    }

    //__________________________________________________________________________
    void Style::renderHeaderBackground( const QRect& r, const QPalette& pal, QPainter* p, const QWidget* widget, bool horizontal, bool reverse ) const
    {

        // use window background for the background
        if( widget ) _helper.renderWindowBackground(p, r, widget, pal);
        else p->fillRect( r, pal.color( QPalette::Window ) );

        if( horizontal ) renderHeaderLines( r, pal, p, TileSet::Bottom );
        else if( reverse ) renderHeaderLines( r, pal, p, TileSet::Left );
        else renderHeaderLines( r, pal, p, TileSet::Right );

    }

    //__________________________________________________________________________
    void Style::renderHeaderLines( const QRect& r, const QPalette& pal, QPainter* p, TileSet::Tiles tiles ) const
    {

        // add horizontal lines
        QColor color = pal.color(QPalette::Window);
        QColor dark  = _helper.calcDarkColor(color);
        QColor light = _helper.calcLightColor(color);

        p->save();
        QRect rect( r );
        if( tiles & TileSet::Bottom  )
        {

            p->setPen(dark);
            if( tiles & TileSet::Left ) p->drawPoint( rect.bottomLeft() );
            else if( tiles& TileSet::Right ) p->drawPoint( rect.bottomRight() );
            else p->drawLine(rect.bottomLeft(), rect.bottomRight());

            rect.adjust(0,0,0,-1);
            p->setPen(light);
            if( tiles & TileSet::Left )
            {
                p->drawLine(rect.bottomLeft(), rect.bottomLeft()+QPoint( 1, 0 ) );
                p->drawLine(rect.bottomLeft()+ QPoint( 1, 0 ), rect.bottomLeft()+QPoint( 1, 1 ) );

            } else if( tiles & TileSet::Right ) {

                p->drawLine(rect.bottomRight(), rect.bottomRight() - QPoint( 1, 0 ) );
                p->drawLine(rect.bottomRight() - QPoint( 1, 0 ), rect.bottomRight() - QPoint( 1, -1 ) );

            } else {

                p->drawLine(rect.bottomLeft(), rect.bottomRight());
            }
        } else if( tiles & TileSet::Left ) {

            p->setPen(dark);
            p->drawLine(rect.topLeft(), rect.bottomLeft());

            rect.adjust(1,0,0,0);
            p->setPen(light);
            p->drawLine(rect.topLeft(), rect.bottomLeft());

        } else if( tiles & TileSet::Right ) {

            p->setPen(dark);
            p->drawLine(rect.topRight(), rect.bottomRight());

            rect.adjust(0,0,-1,0);
            p->setPen(light);
            p->drawLine(rect.topRight(), rect.bottomRight());

        }

        p->restore();

        return;

    }

    //__________________________________________________________________________
    void Style::renderMenuItemRect( const QStyleOption* opt, const QRect& r, const QColor& base, const QPalette& pal, QPainter* p, qreal opacity ) const
    {

        if( opacity == 0 ) return;

        // get relevant color
        QColor color(base);
        if( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG)
        {

            color = pal.color(QPalette::Highlight);

        } else if( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE) {

            color = KColorUtils::mix(color, KColorUtils::tint(color, pal.color(QPalette::Highlight), 0.6));

        } else color = _helper.calcMidColor( color );

        // special painting for items with submenus
        const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>(opt);
        if( menuItemOption && menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu )
        {

            QPixmap pm(r.size());
            pm.fill(Qt::transparent);
            QPainter pp(&pm);
            QRect rr(QPoint(0,0), r.size());

            pp.setRenderHint(QPainter::Antialiasing);
            pp.setPen(Qt::NoPen);

            pp.setBrush(color);
            _helper.fillHole(pp, rr);

            _helper.holeFlat(color, 0.0)->render(rr.adjusted( 1, 2, -2, -1 ), &pp);

            QRect maskr( visualRect(opt->direction, rr, QRect(rr.width()-40, 0, 40,rr.height())) );
            QLinearGradient gradient(
                visualPos(opt->direction, maskr, QPoint(maskr.left(), 0)),
                visualPos(opt->direction, maskr, QPoint(maskr.right()-4, 0)));
            gradient.setColorAt( 0.0, Qt::black );
            gradient.setColorAt( 1.0, Qt::transparent );
            pp.setBrush(gradient);
            pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            pp.drawRect(maskr);

            if( opacity >= 0 && opacity < 1 )
            {
                pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                pp.fillRect(pm.rect(), _helper.alphaColor( Qt::black, opacity ) );
            }

            pp.end();

            p->drawPixmap(handleRTL(opt, r), pm);

        } else {

            if( opacity >= 0 && opacity < 1 )
            { color.setAlphaF( opacity ); }

            _helper.holeFlat(color, 0.0)->render(r.adjusted(1,2,-2,-1), p, TileSet::Full);

        }

    }

    //____________________________________________________________________________________
    QColor Style::slabShadowColor( QColor color, StyleOptions opts, qreal opacity, AnimationMode mode ) const
    {

        QColor glow;
        if( mode == AnimationNone || opacity < 0 )
        {

            if( opts & Hover ) glow = _helper.viewHoverBrush().brush(QPalette::Active).color();
            else if( opts & Focus ) glow = _helper.viewFocusBrush().brush(QPalette::Active).color();
            else if( opts & SubtleShadow ) glow = _helper.alphaColor(_helper.calcShadowColor(color), 0.15 );


        } else if( mode == AnimationHover ) {

            // animated color, hover
            if( opts&Focus ) glow = _helper.viewFocusBrush().brush(QPalette::Active).color();
            else if( opts&SubtleShadow ) glow = _helper.alphaColor(_helper.calcShadowColor(color), 0.15 );

            if( glow.isValid() ) glow = KColorUtils::mix( glow,  _helper.viewHoverBrush().brush(QPalette::Active).color(), opacity );
            else glow = _helper.alphaColor(  _helper.viewHoverBrush().brush(QPalette::Active).color(), opacity );

        } else if( mode == AnimationFocus ) {

            if( opts&Hover ) glow = _helper.viewHoverBrush().brush(QPalette::Active).color();
            else if( opts&SubtleShadow ) glow = _helper.alphaColor(_helper.calcShadowColor(color), 0.15 );

            if( glow.isValid() ) glow = KColorUtils::mix( glow,  _helper.viewFocusBrush().brush(QPalette::Active).color(), opacity );
            else glow = _helper.alphaColor(  _helper.viewFocusBrush().brush(QPalette::Active).color(), opacity );

        }

        return glow;
    }

    //___________________________________________________________________________________
    void Style::renderDialSlab( QPainter *painter, QRect r, const QColor &color, const QStyleOption *option, StyleOptions opts, qreal opacity, AnimationMode mode) const
    {

        // cast option
        const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
        if( !sliderOption ) return;

        // adjust rect to be square, and centered
        int dimension(qMin( r.width(), r.height() ));
        QRect rect( r.topLeft() + QPoint( (r.width()-dimension)/2, (r.height()-dimension)/2 ), QSize( dimension, dimension ) );

        // calculate glow color
        QColor glow = slabShadowColor( color, opts, opacity, mode );

        // get main slab
        QPixmap pix( glow.isValid() ? _helper.dialSlabFocused( color, glow, 0.0, dimension ) : _helper.dialSlab( color, 0.0, dimension ));
        const qreal baseOffset = 3.5;

        QColor light  = _helper.calcLightColor(color);
        QColor shadow( _helper.calcShadowColor(color) );

        QPainter p( &pix );
        p.setPen( Qt::NoPen );
        p.setRenderHints(QPainter::Antialiasing);

        // indicator
        // might use cache here
        // angle calculation from qcommonstyle.cpp (c) Trolltech 1992-2007, ASA.
        qreal angle(0);
        if( sliderOption->maximum == sliderOption->minimum ) angle = M_PI / 2;
        else {

            const qreal fraction( qreal(sliderOption->sliderValue - sliderOption->minimum)/qreal(sliderOption->maximum - sliderOption->minimum));
            if( sliderOption->dialWrapping ) angle = 1.5*M_PI - fraction*2*M_PI;
            else  angle = (M_PI*8 - fraction*10*M_PI)/6;
        }

        QPointF center( pix.rect().center() );
        const int sliderWidth = dimension/6;
        const qreal radius( 0.5*( dimension - 2*sliderWidth ) );
        center += QPointF( radius*cos(angle), -radius*sin(angle));

        QRectF sliderRect( 0, 0, sliderWidth, sliderWidth );
        sliderRect.moveCenter( center );

        // outline circle
        const qreal offset = 0.3;
        QLinearGradient lg( 0, baseOffset, 0, baseOffset + 2*sliderRect.height() );
        p.setBrush( light );
        p.setPen( Qt::NoPen );
        p.drawEllipse( sliderRect.translated(0, offset) );

        // mask
        p.setPen( Qt::NoPen );
        p.save();
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush(QBrush(Qt::black));
        p.drawEllipse( sliderRect );
        p.restore();

        // shadow
        p.translate( sliderRect.topLeft() );
        _helper.drawInverseShadow( p, shadow.darker(200), 0.0, sliderRect.width(), 0.0 );

        // glow
        if( glow.isValid() ) _helper.drawInverseGlow( p, glow, 0.0, sliderRect.width(),  sliderRect.width() );

        p.end();

        painter->drawPixmap( rect.topLeft(), pix );

        return;

    }

    //____________________________________________________________________________________
    void Style::renderButtonSlab(QPainter *p, QRect r, const QColor &color, StyleOptions opts, qreal opacity, AnimationMode mode, TileSet::Tiles tiles) const
    {
        if( (r.width() <= 0) || (r.height() <= 0)) return;

        r.translate(0,-1);
        if( opts & Sunken) r.adjust(-1,0,1,2);

        // fill
        if( !(opts & NoFill))
        {
            p->save();
            p->setRenderHint(QPainter::Antialiasing);
            p->setPen(Qt::NoPen);

            if( _helper.calcShadowColor(color).value() > color.value() && (opts & Sunken) )
            {

                QLinearGradient innerGradient(0, r.top(), 0, r.bottom() + r.height());
                innerGradient.setColorAt(0.0, color);
                innerGradient.setColorAt(1.0, _helper.calcLightColor(color));
                p->setBrush(innerGradient);

            } else if(opts & Sunken) {


                QLinearGradient innerGradient(0, r.top() - r.height(), 0, r.bottom());
                innerGradient.setColorAt(0.0, _helper.calcLightColor(color));
                innerGradient.setColorAt(1.0, color);
                p->setBrush(innerGradient);

            } else {

                QLinearGradient innerGradient(0, r.top()-0.2*r.height(), 0, r.bottom()+ 0.4*r.height() );
                innerGradient.setColorAt(0.0, _helper.calcLightColor(color));
                innerGradient.setColorAt(0.6, color );
                p->setBrush(innerGradient);

            }

            if( opts & Sunken ) _helper.fillSlab(*p, r.adjusted(0,0,0,-1) );
            else _helper.fillSlab(*p, r);
            p->restore();

        }

        // edges
        // for slabs, hover takes precedence over focus (other way around for holes)
        // but in any case if the button is sunken we don't show focus nor hover
        TileSet *tile;
        if( opts & Sunken)
        {
            tile = _helper.slabSunken(color, 0.0);

        } else {

            QColor glow = slabShadowColor( color, opts, opacity, mode );
            tile = glow.isValid() ? _helper.slabFocused(color, glow, 0.0) : _helper.slab(color, 0.0);

        }

        if( opts & Sunken ) tile->render(r.adjusted(0,0,0,-1), p, tiles);
        else tile->render(r, p, tiles);

    }

    //____________________________________________________________________________________
    void Style::renderSlab(QPainter *p, QRect r, const QColor &color, StyleOptions opts, qreal opacity,
        AnimationMode mode,
        TileSet::Tiles tiles) const
    {
        if( (r.width() <= 0) || (r.height() <= 0)) return;

        // this is needed for button vertical alignment
        r.translate(0,-1);
        if( !p->clipRegion().isEmpty() ) p->setClipRegion( p->clipRegion().translated(0,-1) );

        // additional adjustment for sunken frames
        if( opts & Sunken) r.adjust(-1,0,1,2);

        // fill
        if( !(opts & NoFill))
        {
            p->save();
            p->setRenderHint(QPainter::Antialiasing);
            p->setPen(Qt::NoPen);

            if( _helper.calcShadowColor(color).value() > color.value() && (opts & Sunken) )
            {

                QLinearGradient innerGradient(0, r.top(), 0, r.bottom() + r.height());
                innerGradient.setColorAt(0.0, color);
                innerGradient.setColorAt(1.0, _helper.calcLightColor(color));
                p->setBrush(innerGradient);

            } else {

                QLinearGradient innerGradient(0, r.top() - r.height(), 0, r.bottom());
                innerGradient.setColorAt(0.0, _helper.calcLightColor(color));
                innerGradient.setColorAt(1.0, color);
                p->setBrush(innerGradient);

            }

            _helper.fillSlab(*p, r);

            p->restore();
        }

        // edges
        // for slabs, hover takes precedence over focus (other way around for holes)
        // but in any case if the button is sunken we don't show focus nor hover
        TileSet *tile;
        if( opts & Sunken)
        {
            tile = _helper.slabSunken(color, 0.0);

        } else {

            // calculate proper glow color based on current settings and opacity
            QColor glow( slabShadowColor( color, opts, opacity, mode ) );
            tile = glow.isValid() ? _helper.slabFocused(color, glow , 0.0) : _helper.slab(color, 0.0);

        }

        tile->render(r, p, tiles);

    }

    //______________________________________________________________________________
    void Style::renderScrollBarHole(QPainter *p, const QRect &r, const QColor &color,
        Qt::Orientation orientation, TileSet::Tiles tiles) const
    {
        if( r.isValid())
        {

            // one need to make smaller shadow
            // (notably on the size when rect height is too high)
            bool smallShadow = r.height() < 10;
            _helper.scrollHole( color, orientation, smallShadow)->render(r, p, tiles);

        }

    }

    //______________________________________________________________________________
    void Style::renderScrollBarHandle(
        QPainter *p, const QRect &r, const QPalette &pal,
        Qt::Orientation orientation, bool hover, qreal opacity ) const
    {
        if( !r.isValid()) return;
        p->save();
        p->setRenderHints(QPainter::Antialiasing);
        QColor color = pal.color(QPalette::Button);
        QColor light = _helper.calcLightColor(color);
        QColor mid = _helper.calcMidColor(color);
        QColor dark = _helper.calcDarkColor(color);
        QColor shadow = _helper.calcShadowColor(color);
        bool horizontal = orientation == Qt::Horizontal;

        // draw the hole as background
        //const QRect holeRect = horizontal ? r.adjusted(-4,0,4,0) : r.adjusted(0,-3,0,4);
        const QRect holeRect = horizontal ? r.adjusted(-4,0,4,0) : r.adjusted(0,-3,0,4);
        renderScrollBarHole(p, holeRect,
            pal.color(QPalette::Window), orientation,
            horizontal ? TileSet::Top | TileSet::Bottom | TileSet::Center
            : TileSet::Left | TileSet::Right | TileSet::Center);

        // draw the slider itself
        QRectF rect = r.adjusted(3, horizontal ? 2 : 4, -3, -3);
        if( !rect.isValid()) { // e.g. not enough height
            p->restore();
            return;
        }

        // draw the slider
        QColor glowColor;
        if( !OxygenStyleConfigData::scrollBarColored())
        {
            QColor base = KColorUtils::mix(dark, shadow, 0.5);
            QColor hovered = _helper.viewHoverBrush().brush(QPalette::Active).color();

            if( opacity >= 0 ) glowColor = KColorUtils::mix( base, hovered, opacity );
            else if( hover ) glowColor = hovered;
            else glowColor = base;

        } else {

            glowColor = KColorUtils::mix(dark, shadow, 0.5);

        }

        // glow / shadow
        p->setPen(Qt::NoPen);
        p->setBrush(_helper.alphaColor(glowColor, 0.6));
        p->drawRoundedRect(rect.adjusted(-0.8,-0.8,0.8,0.8), 3, 3);
        p->setPen(QPen( _helper.alphaColor(glowColor, 0.3),  1.5));
        if( horizontal) p->drawRoundedRect(rect.adjusted(-1.2,-0.8,1.2,0.8), 3, 3);
        else p->drawRoundedRect(rect.adjusted(-0.8,-1.2,0.8,1.2), 3, 3);

        // colored background
        p->setPen(Qt::NoPen);
        if( OxygenStyleConfigData::scrollBarColored())
        {

            if( opacity >= 0 ) p->setBrush( KColorUtils::mix( color, pal.color(QPalette::Highlight), opacity ) );
            else if( hover ) p->setBrush(  pal.color(QPalette::Highlight) );
            else p->setBrush( color );
            p->drawRoundedRect(rect, 2, 2);

        }

        // slider gradient
        {
            QLinearGradient sliderGradient( rect.topLeft(), horizontal ? rect.bottomLeft() : rect.topRight());
            if( !OxygenStyleConfigData::scrollBarColored()) {
                sliderGradient.setColorAt(0.0, color);
                sliderGradient.setColorAt(1.0, mid);
            } else {
                sliderGradient.setColorAt(0.0, _helper.alphaColor( light, 0.6 ));
                sliderGradient.setColorAt(0.3, _helper.alphaColor( dark, 0.3 ));
                sliderGradient.setColorAt(1.0, _helper.alphaColor( light, 0.8 ));
            }

            p->setBrush(sliderGradient);
            p->drawRoundedRect(rect, 2, 2);
        }

        // pattern
        if( OxygenStyleConfigData::scrollBarBevel() )
        {
            QPoint offset = horizontal ? QPoint(-rect.left(), 0) : QPoint(0, -rect.top()); // don't let the pattern move
            QPoint periodEnd = offset + (horizontal ? QPoint(30, 0) : QPoint(0, 30));
            QLinearGradient patternGradient(rect.topLeft()+offset, rect.topLeft()+periodEnd);
            if( !OxygenStyleConfigData::scrollBarColored()) {
                patternGradient.setColorAt(0.0, _helper.alphaColor(shadow, 0.1));
                patternGradient.setColorAt(1.0, _helper.alphaColor(light, 0.1));
            } else {
                patternGradient.setColorAt(0.0, _helper.alphaColor(shadow, 0.15));
                patternGradient.setColorAt(1.0, _helper.alphaColor(light, 0.15));
            }
            patternGradient.setSpread(QGradient::ReflectSpread);

            p->setBrush(patternGradient);
            p->drawRoundedRect(rect, 2, 2);
        }

        if( OxygenStyleConfigData::scrollBarColored()) {
            p->restore();
            return;
        }

        // bevel
        {
            QLinearGradient bevelGradient( rect.topLeft(), horizontal ? rect.topRight() : rect.bottomLeft());
            bevelGradient.setColorAt(0.0, Qt::transparent);
            bevelGradient.setColorAt(0.5, light);
            bevelGradient.setColorAt(1.0, Qt::transparent);

            rect.adjust(0.5, 0.5, -0.5, -0.5); // for sharper lines
            p->setPen(QPen(bevelGradient, 1.0));
            p->drawLine(rect.topLeft(), horizontal ? rect.topRight() : rect.bottomLeft());
            p->drawLine(rect.bottomRight(), horizontal ? rect.bottomLeft() : rect.topRight());
        }

        p->restore();

    }

    //______________________________________________________________________________
    QPolygonF Style::genericArrow( int primitive, Style::ArrowSize size ) const
    {

        QPolygonF a;
        switch( primitive )
        {
            case Generic::ArrowUp:
            {
                if( size == ArrowTiny ) a << QPointF( -1.75, 1.125 ) << QPointF( 0.5, -1.125 ) << QPointF( 2.75, 1.125 );
                else if( size == ArrowSmall ) a << QPointF( -2,1.5) << QPointF(0.5, -1.5) << QPointF(3,1.5);
                else a << QPointF( -3,2.5) << QPointF(0.5, -1.5) << QPointF(4,2.5);
                break;
            }

            case Generic::ArrowDown:
            {
                if( size == ArrowTiny ) a << QPointF( -1.75, -1.125 ) << QPointF( 0.5, 1.125 ) << QPointF( 2.75, -1.125 );
                else if( size == ArrowSmall ) a << QPointF( -2,-1.5) << QPointF(0.5, 1.5) << QPointF(3,-1.5);
                else a << QPointF( -3,-2.5) << QPointF(0.5, 1.5) << QPointF(4,-2.5);
                break;
            }

            case Generic::ArrowLeft:
            {
                if( size == ArrowTiny ) a << QPointF( 1.125, -1.75 ) << QPointF( -1.125, 0.5 ) << QPointF( 1.125, 2.75 );
                else if( size == ArrowSmall ) a << QPointF(1.5,-2) << QPointF(-1.5, 0.5) << QPointF(1.5,3);
                else a << QPointF(2.5,-3) << QPointF(-1.5, 0.5) << QPointF(2.5,4);
                break;
            }

            case Generic::ArrowRight:
            {
                if( size == ArrowTiny ) a << QPointF( -1.125, -1.75 ) << QPointF( 1.125, 0.5 ) << QPointF( -1.125, 2.75 );
                else if( size == ArrowSmall ) a << QPointF(-1.5,-2) << QPointF(1.5, 0.5) << QPointF(-1.5,3);
                else a << QPointF(-2.5,-3) << QPointF(1.5, 0.5) << QPointF(-2.5,4);
                break;
            }

            default: break;

        }

        return a;


    }

    //________________________________________________________________________
    void Style::renderCheckBox(QPainter *p, const QRect &rect, const QPalette &pal,
        bool enabled, bool hasFocus, bool mouseOver, int primitive,
        bool sunken,
        qreal opacity,
        AnimationMode mode ) const
    {
        Q_UNUSED(enabled);

        int s = qMin(rect.width(), rect.height());
        QRect r = centerRect(rect, s, s);

        StyleOptions opts;
        if( hasFocus) opts |= Focus;
        if( mouseOver) opts |= Hover;

        if(sunken) _helper.holeFlat(pal.color(QPalette::Window), 0.0)->render(r, p, TileSet::Full);
        else renderSlab(p, r, pal.color(QPalette::Button), opts, opacity, mode, TileSet::Ring );

        // check mark
        double x = r.center().x() - 3.5, y = r.center().y() - 2.5;

        if( primitive != CheckBox::CheckOff)
        {
            qreal penThickness = 2.0;

            QColor color =  (sunken) ? pal.color(QPalette::WindowText): pal.color(QPalette::ButtonText);
            QColor background =  (sunken) ? pal.color(QPalette::Window): pal.color(QPalette::Button);

            QPen pen( _helper.decoColor( background, color ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            QPen contrastPen( _helper.calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

            pen.setCapStyle(Qt::RoundCap);
            if( primitive == CheckBox::CheckTriState)
            {
                QVector<qreal> dashes;
                if( OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_CHECK)
                {
                    dashes << 1.0 << 2.0;
                    penThickness = 1.3;
                    pen.setWidthF(penThickness);
                    contrastPen.setWidthF(penThickness);
                }
                else {
                    dashes << 0.4 << 2.0;
                }
                pen.setDashPattern(dashes);
                contrastPen.setDashPattern(dashes);
            }

            p->save();
            if( !sunken ) p->translate(0, -1);
            p->setRenderHint(QPainter::Antialiasing);
            qreal offset( qMin( penThickness, qreal(1.0) ) );
            if( OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_CHECK)
            {

                p->setPen(contrastPen);
                p->translate( 0, offset );
                p->drawLine(QPointF(x+9, y), QPointF(x+3,y+7));
                p->drawLine(QPointF(x, y+4), QPointF(x+3,y+7));

                p->setPen(pen);
                p->translate( 0, -offset );
                p->drawLine(QPointF(x+9, y), QPointF(x+3,y+7));
                p->drawLine(QPointF(x, y+4), QPointF(x+3,y+7));

            } else {

                if( sunken)
                {

                    p->setPen(contrastPen);
                    p->translate( 0, offset );
                    p->drawLine(QPointF(x+8, y), QPointF(x+1,y+7));
                    p->drawLine(QPointF(x+8, y+7), QPointF(x+1,y));

                    p->setPen(pen);
                    p->translate( 0, -offset );
                    p->drawLine(QPointF(x+8, y), QPointF(x+1,y+7));
                    p->drawLine(QPointF(x+8, y+7), QPointF(x+1,y));

                } else {

                    p->setPen(contrastPen);
                    p->translate( 0, offset );
                    p->drawLine(QPointF(x+8, y-1), QPointF(x,y+7));
                    p->drawLine(QPointF(x+8, y+7), QPointF(x,y-1));

                    p->setPen(pen);
                    p->translate( 0, -offset );
                    p->drawLine(QPointF(x+8, y-1), QPointF(x,y+7));
                    p->drawLine(QPointF(x+8, y+7), QPointF(x,y-1));

                }
            }
            p->restore();
        }
    }

    //___________________________________________________________________
    void Style::renderRadioButton(QPainter *p, const QRect &r, const QPalette &pal,
        bool enabled, bool hasFocus, bool mouseOver, int prim,
        bool drawButton, qreal opacity,
        AnimationMode mode ) const
    {
        Q_UNUSED(enabled);

        int s = widgetLayoutProp(WT_RadioButton, RadioButton::Size);
        QRect r2(r.x() + (r.width()-s)/2, r.y() + (r.height()-s)/2, s, s);
        int x = r2.x();
        int y = r2.y();

        if( drawButton )
        {

            StyleOptions opts;
            if( hasFocus) opts |= Focus;
            if( mouseOver) opts |= Hover;
            QColor color( pal.color(QPalette::Button) );
            QColor glow( slabShadowColor( color, opts, opacity, mode ) );

            QPixmap slabPixmap = glow.isValid() ?  _helper.roundSlabFocused( color, glow, 0.0 ):_helper.roundSlab( color, 0.0 );
            p->drawPixmap(x, y, slabPixmap);

        }

        // draw the radio mark
        switch (prim)
        {
            case RadioButton::RadioOn:
            {
                const double radius = 2.6;
                double dx = r2.width() * 0.5 - radius;
                double dy = r2.height() * 0.5 - radius;
                p->save();
                p->setRenderHints(QPainter::Antialiasing);
                p->setPen(Qt::NoPen);

                QColor background( pal.color( QPalette::Button ) );
                QColor color( pal.color(QPalette::ButtonText) );

                p->setBrush( _helper.calcLightColor( background ) );
                p->translate( 0, radius/2 );
                p->drawEllipse(QRectF(r2).adjusted(dx, dy, -dx, -dy));

                p->setBrush( _helper.decoColor( background, color ) );
                p->translate( 0, -radius/2 );
                p->drawEllipse(QRectF(r2).adjusted(dx, dy, -dx, -dy));
                p->restore();

                return;
            }
            case RadioButton::RadioOff:
            {
                // empty
                return;
            }

            default:
            // StateTristate, shouldn't happen...
            return;
        }
    }

    //_____________________________________________________________
    void Style::renderDot(QPainter *p, const QPointF &point, const QColor &baseColor) const
    {
        Q_UNUSED(baseColor)
            const qreal diameter = 1.8;
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);

        p->setBrush(_helper.calcLightColor(baseColor));
        p->drawEllipse(QRectF(point.x()-diameter/2+1.0, point.y()-diameter/2+1.0, diameter, diameter));
        p->setBrush(_helper.calcDarkColor(baseColor));
        p->drawEllipse(QRectF(point.x()-diameter/2+0.5, point.y()-diameter/2+0.5, diameter, diameter));

        p->restore();
    }

    //_____________________________________________________________
    TileSet::Tiles Style::tilesByShape(QTabBar::Shape shape) const
    {
        switch( tabOrientation( shape ) )
        {
            case TabNorth: return TileSet::Top | TileSet::Left | TileSet::Right;
            case TabSouth: return TileSet::Bottom | TileSet::Left | TileSet::Right;
            case TabEast: return TileSet::Right | TileSet::Top | TileSet::Bottom;
            case TabWest: return TileSet::Left | TileSet::Top | TileSet::Bottom;
            default:
            return TileSet::Ring;
        }
    }

    //_____________________________________________________________________
    void Style::renderTab(
        QPainter*p, const QRect& r,
        const QPalette& pal,
        State flags,
        const QStyleOptionTabV2 *tabOpt,
        const bool reverseLayout,
        const QWidget *widget) const
    {

        const bool enabled( flags & State_Enabled );
        const bool mouseOver(enabled && (flags & State_MouseOver) );
        const bool selected( flags&State_Selected );

        const QStyleOptionTab::TabPosition pos = tabOpt->position;
        const QStyleOptionTabV3 *tabOptV3 = qstyleoption_cast<const QStyleOptionTabV3 *>(tabOpt);

        // HACK: determine whether a connection to a frame (like in tab widgets) has to be considered
        bool documentMode = tabOptV3 ? tabOptV3->documentMode : false;
        const QTabWidget *tabWidget = (widget && widget->parentWidget()) ? qobject_cast<const QTabWidget *>(widget->parentWidget()) : NULL;
        documentMode |= (tabWidget ? tabWidget->documentMode() : true );

        // need to get widget rect to fix some cornerCases
        QRect widgetRect( tabWidget ? tabWidget->rect().translated( -widget->pos() ):QRect() );

        TabOrientation orientation( tabOrientation( tabOpt->shape ) );
        const bool northAlignment = (orientation == TabNorth );
        const bool southAlignment = (orientation == TabSouth );
        const bool westAlignment = (orientation == TabWest );
        const bool eastAlignment = (orientation == TabEast );
        const bool horizontal = (northAlignment || southAlignment);

        const bool leftCornerWidget = reverseLayout ?
            (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget) :
            (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget);

        const bool rightCornerWidget = reverseLayout ?
            (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget) :
            (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget);

        const bool isFirst = (pos == QStyleOptionTab::Beginning || pos == QStyleOptionTab::OnlyOneTab );
        const bool isLast = (pos == QStyleOptionTab::End || pos == QStyleOptionTab::OnlyOneTab );

        const bool isLeftOfSelected =  reverseLayout ?
            (tabOpt->selectedPosition == QStyleOptionTab::PreviousIsSelected) :
            (tabOpt->selectedPosition == QStyleOptionTab::NextIsSelected);

        const bool isRightOfSelected =  reverseLayout ?
            (tabOpt->selectedPosition == QStyleOptionTab::NextIsSelected) :
            (tabOpt->selectedPosition == QStyleOptionTab::PreviousIsSelected);

        const bool isLeftMost =  ((reverseLayout && horizontal) ?
            (tabOpt->position == QStyleOptionTab::End) :
            (tabOpt->position == QStyleOptionTab::Beginning)) ||
            tabOpt->position == QStyleOptionTab::OnlyOneTab;

        const bool isRightMost = ((reverseLayout && horizontal) ?
            (tabOpt->position == QStyleOptionTab::Beginning) :
            (tabOpt->position == QStyleOptionTab::End)) ||
            tabOpt->position == QStyleOptionTab::OnlyOneTab;

        const bool isTopMost = isLeftMost && !horizontal;
        const bool isBottomMost = isRightMost && !horizontal;

        bool isFrameAligned( false );
        if( horizontal )
        {

            isFrameAligned =  reverseLayout ?
                (isRightMost && !(tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget)) :
                (isLeftMost && !(tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget));

        } else {

            isFrameAligned = isTopMost && !(tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget);

        }

        isFrameAligned &= !documentMode;

        bool isEdge( false );

        const QColor color = pal.color(QPalette::Window);
        StyleOptions selectedTabOpts = OxygenStyleConfigData::tabSubtleShadow() ? SubtleShadow | NoFill : NoFill;
        StyleOptions hoverTabOpts = NoFill | Hover;
        StyleOptions deselectedTabOpts = NoFill;

        // if painter device is not the current widget
        // one must paint a non transparent background behind the tab
        // this occurs notably when dragging tabs around.
        // ideally it would be better if Qt would set a flag to the option
        // when doing this
        const bool drawBackground( p->device() != widget );

        // this is the part of the tab that mimics the tabbar baseline
        TileSet::Tiles frameTiles = (horizontal) ?
            (northAlignment ? TileSet::Top : TileSet::Bottom):
            (westAlignment ? TileSet::Left : TileSet::Right);


        switch (OxygenStyleConfigData::tabStyle())
        {

            case OxygenStyleConfigData::TS_SINGLE:
            {
                QRect tabRect = r;

                // tabRect defines the position of the tab
                if( horizontal)
                {

                    // selected tabs are taller
                    if( selected)
                    {

                        if( northAlignment) tabRect.adjust(0,-1,0,2);
                        else tabRect.adjust(0,-2,0,1);

                    } else {

                        // deselected
                        if( northAlignment) tabRect.adjust(0,1,0,2);
                        else tabRect.adjust(0,-2,0,-1);

                    }

                    // reduces the space between tabs
                    if( !isLeftMost) tabRect.adjust(-gw,0,0,0);
                    if( !isRightMost) tabRect.adjust(0,0,gw,0);

                } else {

                    // east and west tabs
                    // selected tabs are taller
                    if( selected)
                    {

                        if( westAlignment) tabRect.adjust(-1,0,2,0);
                        else tabRect.adjust(-2,0,1,0);

                    } else {

                        // deselected
                        if( westAlignment) tabRect.adjust(1,0,2,0);
                        else tabRect.adjust(-2,0,-1,0);

                    }

                    // reduces the space between tabs
                    if( !isTopMost ) tabRect.adjust(0,-gw,0,0);
                    if( !isBottomMost ) tabRect.adjust(0,0,0,gw);
                }

                // frameRect defines the part of the frame which
                // holds the content and is connected to the tab
                QRect frameRect;
                if( horizontal)
                {

                    if( northAlignment) frameRect = r.adjusted(-7, r.height()-gw-7, 7, 0);
                    else frameRect = r.adjusted(-7, 0, 7, -r.height()+gw+7);

                    if( isLast && !documentMode && !widgetRect.isNull() )
                    {
                        if( reverseLayout )
                        {
                            isEdge = frameRect.left() < widgetRect.left()-5;
                            frameRect.setLeft( qMax( frameRect.left(), widgetRect.left()-1 ) );
                        } else {
                            isEdge = frameRect.right() > widgetRect.right()+5;
                            frameRect.setRight( qMin( frameRect.right(), widgetRect.right()+1 ) );
                        }
                    }

                } else {

                    // vertical
                    if( westAlignment) frameRect = r.adjusted(r.width()-gw-7, -7, 0, 7);
                    else frameRect = r.adjusted(0, -7, -r.width()+gw+7, 7);

                    if( isLast && !documentMode && !widgetRect.isNull() )
                    {
                        isEdge = frameRect.bottom() > widgetRect.bottom()+5;
                        frameRect.setBottom( qMin( frameRect.bottom(), widgetRect.bottom()+1 ) );
                    }
                }

                // HACK: Workaround for misplaced tab
                if( southAlignment) {

                    frameRect.translate( 0, -1 );
                    if( selected) tabRect.translate( 0, -1 );
                    else tabRect.adjust(0,0,0,-1);

                }

                // handle the rightmost and leftmost tabs
                // if document mode is not enabled, draw the rounded frame corner (which is visible if the tab is not selected)
                // also fill the small gap between that corner and the actial frame
                if( horizontal)
                {

                    if( isLeftMost )
                    {
                        if( isFrameAligned && !reverseLayout )
                        {
                            if( !selected)
                            {
                                frameRect.adjust(-gw+7,0,0,0);
                                frameTiles |= TileSet::Left;
                            }

                            if( northAlignment) renderSlab(p, QRect(r.x()-gw, r.bottom()-11, 2, 18), color, NoFill, TileSet::Left);
                            else if( selected ) renderSlab(p, QRect(r.x()-gw, r.top()-6, 2, 17), color, NoFill, TileSet::Left);
                            else renderSlab(p, QRect(r.x()-gw, r.top()-6, 2, 18), color, NoFill, TileSet::Left);

                        } else if( isEdge ) {

                            if( northAlignment) renderSlab(p, QRect(r.x()-gw+1, r.bottom()-11, 2, 16), color, NoFill, TileSet::Left);
                            else renderSlab(p, QRect(r.x()-gw+1, r.top()-3, 2, 15), color, NoFill, TileSet::Left);

                        }

                        if( !reverseLayout ) tabRect.adjust(-gw,0,0,0);

                    } else if( isRightMost ) {

                        // reverseLayout
                        if( isFrameAligned && reverseLayout )
                        {
                            if( !selected)
                            {
                                frameRect.adjust(0,0,gw-7,0);
                                frameTiles |= TileSet::Right;
                            }

                            if( northAlignment) renderSlab(p, QRect(r.right(), r.bottom()-11, 2, 18), color, NoFill, TileSet::Right);
                            else if( selected ) renderSlab(p, QRect(r.right(), r.top()-6, 2, 17), color, NoFill, TileSet::Right);
                            else renderSlab(p, QRect(r.right(), r.top()-6, 2, 18), color, NoFill, TileSet::Right);

                        } else if( isEdge ) {

                            if( northAlignment) renderSlab(p, QRect(r.right()-1, r.bottom()-11, 2, 16), color, NoFill, TileSet::Right);
                            else renderSlab(p, QRect(r.right()-1, r.top()-3, 2, 15), color, NoFill, TileSet::Right);

                        }

                        if( reverseLayout ) tabRect.adjust(0,0,gw,0);
                    }

                } else {

                    // vertical
                    if( isTopMost && isFrameAligned )
                    {

                        if( !selected)
                        {
                            frameRect.adjust(0,-gw+7,0,0);
                            frameTiles |= TileSet::Top;
                        }

                        if( westAlignment) renderSlab(p, QRect(r.right()-11, r.y()-gw, 18, 2), color, NoFill, TileSet::Top);
                        else renderSlab(p, QRect(r.x()-6, r.y()-gw, 18, 2), color, NoFill, TileSet::Top);

                    } else if( isEdge ) {

                        if( westAlignment) renderSlab(p, QRect(r.right()-11, r.bottom()-gw, 15, 2), color, NoFill, TileSet::Bottom);
                        else renderSlab(p, QRect(r.x()-3, r.bottom()-gw, 15, 2), color, NoFill, TileSet::Bottom);

                    }

                    tabRect.adjust(0,-gw,0,0);
                }

                p->save();

                // draw the remaining parts of the frame
                if( !selected)
                {

                    renderSlab(p, frameRect, color, NoFill, frameTiles);

                } else {

                    // when selected only draw parts of the frame to appear connected to the content
                    QRegion clipRegion;
                    if( horizontal && !(isLeftMost && !reverseLayout))
                    {
                        QRegion frameRegionLeft = QRegion(QRect(frameRect.x(), frameRect.y(), tabRect.x()-frameRect.x() + 2, frameRect.height()));
                        clipRegion += frameRegionLeft;
                    }

                    if( horizontal && !(isRightMost && reverseLayout))
                    {
                        QRegion frameRegionRight = QRegion(QRect(tabRect.right() - gw, frameRect.y(), frameRect.right()-tabRect.right(), frameRect.height()));
                        clipRegion += frameRegionRight;
                    }

                    if( !horizontal && !isTopMost)
                    {
                        QRegion frameRegionTop = QRegion(QRect(frameRect.x(), frameRect.y(), frameRect.width(), tabRect.y() - frameRect.y() + 3));
                        clipRegion += frameRegionTop;
                    }

                    if( !horizontal /* && !isBottomMost */)
                    {
                        QRegion frameRegionTop = QRegion(QRect(frameRect.x(), tabRect.bottom() - 1, frameRect.width(), tabRect.y() - frameRect.y() + 3));
                        clipRegion += frameRegionTop;
                    }

                    p->save();
                    p->setClipRegion(clipRegion);
                    renderSlab(p, frameRect, color, NoFill, frameTiles);
                    p->restore();

                    // connect active tabs to the frame
                    p->setPen(QPen(_helper.alphaColor( _helper.calcLightColor(color), 0.5), 2));
                    if( northAlignment)
                    {

                        // don't draw the connection for a frame aligned tab
                        // except for RTL-layout
                        if( !isFrameAligned || reverseLayout) renderSlab( p, QRect(tabRect.x() - 6, tabRect.bottom()-9, 17, 7), color, NoFill, TileSet::Top );
                        if( !isFrameAligned || !reverseLayout) renderSlab( p, QRect( tabRect.right()-10, tabRect.bottom()-9, 17, 7), color, NoFill, TileSet::Top );

                    } else if( southAlignment) {

                        if( !isFrameAligned || reverseLayout)
                        {
                            if( isLeftMost && !reverseLayout ) renderSlab( p, QRect( tabRect.x()-6, tabRect.y()+3, 17, 7), color, NoFill, TileSet::Bottom );
                            else renderSlab( p, QRect( tabRect.x()-5, tabRect.y()+3, 16, 7), color, NoFill, TileSet::Bottom );
                        }

                        if( !isFrameAligned || !reverseLayout)
                        {
                            if( isRightMost && reverseLayout ) renderSlab( p, QRect( tabRect.right()-10, tabRect.y()+3, 17, 7), color, NoFill, TileSet::Bottom );
                            else renderSlab( p, QRect( tabRect.right()-10, tabRect.y()+3, 16, 7), color, NoFill, TileSet::Bottom );
                        }

                    } else if( eastAlignment) {

                        if( !isFrameAligned) renderSlab( p, QRect( tabRect.x() + 3, tabRect.y() - 7, 7, 18 ), color, NoFill, TileSet::Right );
                        renderSlab( p, QRect( tabRect.x() + 3, tabRect.bottom()-10, 7, 16 ), color, NoFill, TileSet::Right );

                    } else {

                        // west aligned
                        if( !isFrameAligned) renderSlab( p, QRect( tabRect.right()-9, tabRect.y()-7, 7, 18 ), color, NoFill, TileSet::Left );
                        renderSlab( p, QRect( tabRect.right()-9, tabRect.bottom()-10, 7, 16 ), color, NoFill, TileSet::Left );

                    }
                }

                p->setClipRect(tabRect);

                // get timeLine
                if( !selected && enabled && animations().tabBarEngine().isAnimated( widget, r.topLeft() ) )
                {

                    renderSlab(p, tabRect, color, mouseOver ? hoverTabOpts : deselectedTabOpts,
                        animations().tabBarEngine().opacity( widget, r.topLeft() ), AnimationHover,
                        tilesByShape(tabOpt->shape));

                } else {

                    renderSlab(p, tabRect, color, selected ? selectedTabOpts : (mouseOver ? hoverTabOpts : deselectedTabOpts), tilesByShape(tabOpt->shape));

                }

                // filling
                Qt::Orientation orientation( horizontal ? Qt::Horizontal : Qt::Vertical );
                bool inverted( southAlignment || eastAlignment);
                if( drawBackground )
                {
                    QRect fillRect = tabRect.adjusted(4,(orientation == Qt::Horizontal && !inverted) ? 3 : 4,-4,-4);
                    if( widget ) _helper.renderWindowBackground(p, fillRect, widget,widget->window()->palette());
                    else p->fillRect( fillRect, pal.color( QPalette::Window ) );
                }

                fillTab(p, tabRect, color, orientation, selected, inverted );

                p->restore();
                return;

            }

            case OxygenStyleConfigData::TS_PLAIN:
            {

                const QColor backgroundColor = _helper.backgroundColor( color, widget, r.center() );
                const QColor midColor = _helper.alphaColor(_helper.calcDarkColor(backgroundColor), 0.4);
                const QColor darkColor = _helper.alphaColor(_helper.calcDarkColor(backgroundColor), 0.6);

                if( northAlignment || southAlignment )
                {

                    // the tab part of the tab - ie subtracted the fairing to the frame
                    QRect Rc = southAlignment ? r.adjusted(-gw,6+gw,gw,gw) : r.adjusted(-gw,-gw,gw,-7-gw);

                    // the area where the fairing should appear
                    QRect Rb(Rc.x(), southAlignment?r.top()+gw:Rc.bottom()+1, Rc.width(), r.height()-Rc.height() );

                    if( selected )
                    {

                        p->save();

                        int x,y,w,h;
                        r.getRect(&x, &y, &w, &h);

                        QRect tabRect( southAlignment ? Rc.adjusted(0,-10,0,0):  Rc.adjusted(0,0,0,10) );
                        if(southAlignment) renderSlab(p, tabRect, color, NoFill, TileSet::Bottom | TileSet::Left | TileSet::Right);
                        else renderSlab(p, tabRect, color, NoFill, TileSet::Top | TileSet::Left | TileSet::Right);

                        // some "position specific" paintings...
                        // draw the left connection from the panel border to the tab
                        if(isFirst && !reverseLayout && !leftCornerWidget && !documentMode )
                        {
                            if( southAlignment ) renderSlab(p, Rb.adjusted(0, -7, 0, 4), color, NoFill, TileSet::Left);
                            else renderSlab(p, Rb.adjusted(0, -4, 0, 7), color, NoFill, TileSet::Left);

                        } else if( isLeftMost ) {

                            // horizontal tileSet to connect to main line
                            if( southAlignment ) renderSlab( p, QRect(Rb.left()-6, Rb.top(),16,6), color, NoFill, TileSet::Bottom );
                            else renderSlab( p, QRect(Rb.left()-6, Rb.bottom()-6,17,6), color, NoFill, TileSet::Top );

                        }

                        // draw the right connection from the panel border to the tab
                        if(isFirst && reverseLayout && !rightCornerWidget && !documentMode )
                        {

                            if( southAlignment ) renderSlab(p, Rb.adjusted(0, -7, 0, 4), color, NoFill, TileSet::Right);
                            else renderSlab(p, Rb.adjusted(0, -4, 0, 7), color, NoFill, TileSet::Right);

                        } else if( isRightMost ) {

                            if( southAlignment ) renderSlab( p, QRect(Rb.right()-9, Rb.top(),16,6), color, NoFill, TileSet::Bottom );
                            else renderSlab( p, QRect(Rb.right()-10, Rb.bottom()-6,17,6), color, NoFill, TileSet::Top );
                        }

                        // filling (this is identical to the TS_SINGLE code)
                        Qt::Orientation orientation( horizontal ? Qt::Horizontal : Qt::Vertical );
                        bool inverted( southAlignment || eastAlignment);
                        if( drawBackground )
                        {
                            QRect fillRect = tabRect.adjusted(4,(orientation == Qt::Horizontal && !inverted) ? 3 : 4,-4,-4);
                            if( widget ) _helper.renderWindowBackground(p, fillRect, widget,widget->window()->palette());
                            else p->fillRect( fillRect, pal.color( QPalette::Window ) );
                        }

                        fillTab(p, tabRect, color, orientation, selected, inverted );
                        p->restore();

                    } else {

                        // inactive tabs
                        int x,y,w,h;
                        p->save(); // we only use the clipping and AA for inactive tabs
                        p->setPen(darkColor);
                        p->setBrush(midColor);
                        p->setRenderHints(QPainter::Antialiasing);

                        if( northAlignment )
                        {

                            r.adjusted(0,5-gw,0,-gw).getRect(&x, &y, &w, &h);
                            p->setClipRect(x-4, y, w+8, h-5);
                            p->setClipRect(x, y, w, h, Qt::UniteClip);

                            // TODO: also account for case isLeftMost&&isRightMost,
                            // which can happen when dragging tabs
                            if( isLeftMost && isRightMost )
                            {

                                QPainterPath path;
                                x-=gw;
                                w+=gw;
                                path.moveTo(x+2.5, y+h-3-( isFrameAligned ? 0 : 2 ));
                                path.lineTo(x+2.5, y+2.5);
                                path.arcTo(QRectF(x+2.5, y+0.5, 9, 9), 180, -90);
                                path.lineTo(QPointF( x + w - 1.5 - 4.5, y+0.5));
                                path.arcTo( QRectF( x+w - 1.5 - 9, y+0.5, 9, 9 ), 90, -90 );
                                path.lineTo(QPointF( x+w - 1.5, y+h-3-( isFrameAligned ? 0 : 2 ) ));
                                p->drawPath(path);

                            } else if( isLeftMost ) {

                                QPainterPath path;
                                x-=gw;
                                w+=gw;
                                path.moveTo(x+2.5, y+h-3-( isFrameAligned ? 0 : 2 ));
                                path.lineTo(x+2.5, y+2.5);
                                path.arcTo(QRectF(x+2.5, y+0.5, 9, 9), 180, -90);
                                path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+0.5));
                                path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+h-5));
                                p->drawPath(path);

                            } else if( isRightMost ) {

                                QPainterPath path;
                                w+=gw;
                                path.moveTo(x+w-2.5, y+h-3- ( isFrameAligned ? 0 : 2 ) );
                                path.lineTo(x+w-2.5, y+2.5);
                                path.arcTo(QRectF(x+w-9-2.5, y+0.5, 9, 9), 0, 90);
                                path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+0.5));
                                path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+h-5));
                                p->drawPath(path);

                            } else {


                                p->drawLine(QPointF(x-(isRightOfSelected?2:0), y+0.5), QPointF(x+w+(isRightOfSelected?2:0)+(isLeftOfSelected?2:0), y+0.5));
                                if(!isLeftOfSelected) p->drawLine(QPointF(x+w+0.5, y+1.5), QPointF(x+w+0.5, y+h-4));
                                p->fillRect(x-(isRightOfSelected ? 2 : 0), y+1, w+(isLeftOfSelected||isRightOfSelected ? (isRightOfSelected ? 3 : 3-gw) : 0), h-5, midColor);

                            }

                        } else {

                            // southAlignment
                            r.adjusted(0,gw,0,-4+gw).getRect(&x, &y, &w, &h);

                            if( isLeftMost && isRightMost )
                            {

                                QPainterPath path;
                                x-=gw;
                                w+=gw;
                                path.moveTo(x+2.5, y+3+( isFrameAligned ? 0 : 2));
                                path.lineTo(x+2.5, y+h-2.5);
                                path.arcTo(QRectF(x+2.5, y+h-9.5, 9, 9), 180, 90);
                                path.lineTo(QPointF(x+w - 1.5 -4.5, y+h-0.5));
                                path.arcTo( QRectF( x+w - 1.5 - 9, y+h-0.5 - 9, 9, 9 ), -90, 90 );
                                path.lineTo(QPointF(x+w-1.5, y+4 ) );
                                p->drawPath(path);

                            } else if(isLeftMost) {

                                QPainterPath path;
                                x-=gw;
                                w+=gw;
                                path.moveTo(x+2.5, y+2+( isFrameAligned ? 0 : 2));
                                path.lineTo(x+2.5, y+h-2.5);
                                path.arcTo(QRectF(x+2.5, y+h-9.5, 9, 9), 180, 90);
                                path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+h-0.5));
                                path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+4));
                                p->drawPath(path);

                            } else if(isRightMost) {

                                QPainterPath path;
                                w+=gw;
                                path.moveTo(x+w-2.5, y+2+( isFrameAligned ? 0 : 2));
                                path.lineTo(x+w-2.5, y+h-2.5);
                                path.arcTo(QRectF(x+w-9-2.5, y+h-9.5, 9, 9), 0, -90);
                                path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+h-0.5));
                                path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+4));
                                p->drawPath(path);

                            } else {

                                p->drawLine(QPointF(x-(isRightOfSelected?2:0), y+h-0.5), QPointF(x+w+(isRightOfSelected ?2:0)+(isLeftOfSelected ?2:0), y+h-0.5));
                                if(!isLeftOfSelected) p->drawLine(QPointF(x+w+0.5, y+2.5), QPointF(x+w+0.5, y+h-4));
                                p->fillRect(x, y+2, w+(isLeftOfSelected ?2:0), h-2, midColor);

                            }
                        }
                        p->restore();

                        TileSet::Tiles tiles = southAlignment?TileSet::Bottom:TileSet::Top;
                        QRect frameRect(Rb.left(), Rb.y(), Rb.width(), 6);

                        if(isLeftMost)
                        {

                            if( isFrameAligned ) tiles |= TileSet::Left;
                            if( reverseLayout || documentMode || !isFrameAligned )
                            { frameRect.adjust( -6, 0, 0, 0); }

                            if( isLast && !documentMode && !widgetRect.isNull() && reverseLayout )
                            { frameRect.setLeft( qMax( frameRect.left(), widgetRect.left()-1 ) ); }

                        } else if( isRightOfSelected ) frameRect.adjust(-10+gw,0,0,0);
                        else frameRect.adjust(-7+gw,0,0,0);

                        if(isRightMost)
                        {

                            if( isFrameAligned ) tiles |= TileSet::Right;
                            else frameRect.adjust(0,0,6,0);

                            if( isLast && !documentMode && !widgetRect.isNull() && !reverseLayout )
                            { frameRect.setRight( qMin( frameRect.right(), widgetRect.right()+1 ) ); }

                        } else if( isLeftOfSelected ) frameRect.adjust(0,0,10-gw,0);
                        else frameRect.adjust(0,0,7-gw,0);

                        if( animations().tabBarEngine().isAnimated( widget, r.topLeft() ) )
                        {

                            renderSlab(p, frameRect, color, NoFill| Hover,
                                animations().tabBarEngine().opacity( widget, r.topLeft() ),
                                AnimationHover,
                                tiles );

                        } else if( mouseOver) renderSlab(p, frameRect, color, NoFill| Hover, tiles);
                        else renderSlab(p, frameRect, color, NoFill, tiles);

                    }

                } else {

                    // westAlignment and eastAlignment
                    // the tab part of the tab - ie subtracted the fairing to the frame
                    QRect Rc = eastAlignment ? r.adjusted(7+gw,-gw,gw,gw) : r.adjusted(-gw,-gw,-7-gw,gw);

                    // the area where the fairing should appear
                    const QRect Rb(eastAlignment ? r.x()+gw: Rc.right()+1, Rc.top(), r.width()-Rc.width(), Rc.height() );

                    if( selected)
                    {

                        p->save();

                        int x,y,w,h;
                        r.getRect(&x, &y, &w, &h);
                        QRect tabRect( eastAlignment ? Rc.adjusted(-10,0,0,0):Rc.adjusted(0,0,10,0) );

                        if(eastAlignment) renderSlab(p, tabRect, color, NoFill, TileSet::Top | TileSet::Right | TileSet::Bottom);
                        else renderSlab(p, tabRect, color, NoFill, TileSet::Top | TileSet::Left | TileSet::Bottom);

                        // some "position specific" paintings...
                        // draw the top connection from the panel border to the tab
                        if(isFirst && !leftCornerWidget && !documentMode )
                        {
                            if( eastAlignment ) renderSlab(p, Rb.adjusted( -7, 0, 4, 0), color, NoFill, TileSet::Top);
                            else renderSlab(p, Rb.adjusted( -4, 0, 7, 0 ), color, NoFill, TileSet::Top);

                        } else if( isLeftMost ) {

                            if( eastAlignment ) renderSlab( p, QRect(Rb.left()+1, Rb.top()-6,6,16), color, NoFill, TileSet::Right );
                            else renderSlab( p, QRect(Rb.right()-6, Rb.top()-6,6,16), color, NoFill, TileSet::Left );

                        }

                        if( isRightMost )
                        {

                            if( eastAlignment ) renderSlab( p, QRect(Rb.left()+1, Rb.bottom()-9,6,16), color, NoFill, TileSet::Right );
                            else renderSlab( p, QRect(Rb.right()-6, Rb.bottom()-10,6,17), color, NoFill, TileSet::Left );

                        }

                        // filling (this is identical to the TS_SINGLE code)
                        Qt::Orientation orientation( horizontal ? Qt::Horizontal : Qt::Vertical );
                        bool inverted( southAlignment || eastAlignment);
                        if( drawBackground )
                        {
                            QRect fillRect = tabRect.adjusted(4,(orientation == Qt::Horizontal && !inverted) ? 3 : 4,-4,-4);
                            if( widget ) _helper.renderWindowBackground(p, fillRect, widget,widget->window()->palette());
                            else p->fillRect( fillRect, pal.color( QPalette::Window ) );
                        }

                        fillTab(p, tabRect, color, orientation, selected, inverted );
                        p->restore();

                    } else {

                        // inactive tabs
                        int x,y,w,h;
                        p->save();

                        // we only use the clipping and AA for inactive tabs
                        p->setPen(darkColor);
                        p->setBrush(midColor);
                        p->setRenderHints(QPainter::Antialiasing);

                        if( westAlignment)
                        {
                            // west alignment
                            r.adjusted(5-gw,0,-5-gw,0).getRect(&x, &y, &w, &h);

                            if( isLeftMost && isRightMost ) {

                                // at top
                                QPainterPath path;
                                y = y + 1.5;

                                path.moveTo(x+w+0.5 + ( isFrameAligned ? 2 : 0), y+0.5 );
                                path.lineTo(x+5.0, y+0.5);
                                path.arcTo(QRectF(x+0.5, y+0.5, 9.5, 9.5), 90, 90);
                                path.lineTo(x+0.5, y+h-2.5-4.5);
                                path.arcTo( QRectF( x+0.5, y+h-2.5-9, 9, 9 ), 180, 90 );
                                path.lineTo(x+w+( 0.5 ), y+h-2.5);
                                p->drawPath(path);

                            } else if( isLeftMost ) {

                                // at top
                                QPainterPath path;
                                y += 1.5;

                                path.moveTo(x+w+0.5 + ( isFrameAligned ? 2 : 0), y+0.5);
                                path.lineTo(x+5.0, y+0.5);
                                path.arcTo(QRectF(x+0.5, y+0.5, 9.5, 9.5), 90, 90);
                                path.lineTo(x+0.5, y+h+0.5);
                                path.lineTo(x+w+1.0, y+h+0.5);
                                p->drawPath(path);

                            } else if( isRightMost ) {

                                // at bottom
                                QPainterPath path;

                                path.moveTo(x+w+0.5, y+h-0.5);
                                path.lineTo(x+5.0, y+h-0.5);
                                path.arcTo(QRectF(x+0.5, y+h-0.5-9.5, 9.5, 9.5), 270, -90);
                                path.lineTo(x+0.5, y-0.5);
                                path.lineTo(x+w+0.5, y-0.5);
                                p->drawPath(path);

                            } else {

                                // leftline
                                p->drawLine(QPointF(x+0.5, y-0.5 - (isRightOfSelected ? 2:0) ), QPointF(x+0.5, y+h-0.5 + (isLeftOfSelected ? 2:0)));
                                if( !isLeftOfSelected ) p->drawLine(QPointF(x+0.5, y+h-0.5), QPointF(x+w-0.5, y+h-0.5));
                                p->fillRect(x, y - (isRightOfSelected ? 2:0), w, h+(isRightOfSelected ? 2:0)+ (isLeftOfSelected ? 2:0), midColor);

                            }

                        } else {

                            // eastAlignment
                            r.adjusted(5+gw,0,-5+gw,0).getRect(&x, &y, &w, &h);
                            if( isLeftMost && isRightMost )
                            {

                                // at top
                                QPainterPath path;
                                y = y + 1.5;

                                path.moveTo(x-0.5 - ( isFrameAligned ? 2:0 ), y+0.5 );
                                path.lineTo(x+w-5.0, y+0.5);
                                path.arcTo(QRectF(x+w-0.5-9.5, y+0.5, 9.5, 9.5), 90, -90);
                                path.lineTo(x+w-0.5, y+h-2.5 -4.5 );
                                path.arcTo( QRectF( x+w-0.5-9, y+h-2.5-9, 9, 9 ), 0, -90 );
                                path.lineTo(x-0.5, y+h-2.5);
                                p->drawPath(path);

                            } else if( isLeftMost) {

                                // at top
                                QPainterPath path;
                                y = y + 1.5;

                                path.moveTo(x-0.5 - ( isFrameAligned ? 2:0 ), y+0.5 );
                                path.lineTo(x+w-5.0, y+0.5);
                                path.arcTo(QRectF(x+w-0.5-9.5, y+0.5, 9.5, 9.5), 90, -90);
                                path.lineTo(x+w-0.5, y+h+0.5);
                                path.lineTo(x-0.5, y+h+0.5);
                                p->drawPath(path);

                            } else if( isRightMost ) {

                                // at bottom
                                QPainterPath path;

                                path.moveTo(x-0.5, y+h-0.5);
                                path.lineTo(x+w-5.0, y+h-0.5);
                                path.arcTo(QRectF(x+w-0.5-9.5, y+h-0.5-9.5, 9.5, 9.5), -90, 90);
                                path.lineTo(x+w-0.5, y-0.5);
                                path.lineTo(x-0.5, y-0.5);
                                p->drawPath(path);

                            } else {

                                p->drawLine(QPointF(x+w-0.5, y - (isRightOfSelected ? 2:0) ), QPointF(x+w-0.5, y+h-0.5 + (isLeftOfSelected ? 2:0)));
                                if( !isLeftOfSelected ) p->drawLine(QPointF(x+0.5, y+h-0.5), QPointF(x+w-1.5, y+h-0.5));
                                p->fillRect(x, y - (isRightOfSelected ? 2:0), w, h + (isRightOfSelected ? 2:0) + (isLeftOfSelected ? 2:0), midColor);

                            }
                        }
                        p->restore();

                        TileSet::Tiles tiles = eastAlignment ? TileSet::Right : TileSet::Left;
                        QRect frameRect(Rb.left(), Rb.y(), 7, Rb.height());

                        if(isLeftMost)
                        {

                            // at top
                            if( isFrameAligned ) tiles |= TileSet::Top;
                            else {
                                renderSlab(p, QRect(frameRect.left(), frameRect.y()-7, frameRect.width(), 2+14), color, NoFill, tiles);
                                frameRect.adjust(0,-5,0,0);
                            }

                        } else if( isRightOfSelected ) frameRect.adjust(0,-10+gw,0,0);
                        else  frameRect.adjust(0,-7+gw,0,0);

                        if(isRightMost)
                        {

                            // at bottom
                            if( isFrameAligned && !reverseLayout) tiles |= TileSet::Top;
                            frameRect.adjust(0,0,0,7);

                        } else if( isLeftOfSelected )  frameRect.adjust(0,0,0,10-gw);
                        else frameRect.adjust(0,0,0,7-gw);

                        if( isLast && !documentMode && !widgetRect.isNull() )
                        { frameRect.setBottom( qMin( frameRect.bottom(), widgetRect.bottom()+1 ) ); }

                        if( animations().tabBarEngine().isAnimated( widget, r.topLeft() ) ) {

                            renderSlab(p, frameRect, color, NoFill| Hover,
                                animations().tabBarEngine().opacity( widget, r.topLeft() ),
                                AnimationHover,
                                tiles );

                        } else if( mouseOver) renderSlab(p, frameRect, color, NoFill| Hover, tiles);
                        else renderSlab(p, frameRect, color, NoFill, tiles);

                    }

                }
            }

        }

    }

    //______________________________________________________________________________________________________________________________
    void Style::fillTab(QPainter *p, const QRect &r, const QColor &color, Qt::Orientation orientation, bool active, bool inverted) const
    {
        QColor dark = _helper.calcDarkColor(color);
        QColor shadow = _helper.calcShadowColor(color);
        QColor light = _helper.calcLightColor(color);
        QColor hl = _helper.viewFocusBrush().brush(QPalette::Active).color();

        QRect fillRect = r.adjusted(4, 3,-4,-5);

        QLinearGradient highlight;
        if( orientation == Qt::Horizontal)
        {

            if( !inverted)
            {

                highlight = QLinearGradient(fillRect.topLeft(), fillRect.bottomLeft());

            } else {

                // inverted
                highlight = QLinearGradient(fillRect.bottomLeft(), fillRect.topLeft());

            }

        } else {

            // vertical tab fill
            if( !inverted)
            {

                highlight = QLinearGradient(fillRect.topLeft(), fillRect.topRight());

            } else {

                // inverted
                highlight = QLinearGradient(fillRect.topRight(), fillRect.topLeft());

            }
        }

        if( active) {

            highlight.setColorAt(0.0, _helper.alphaColor(light, 0.5));
            highlight.setColorAt(0.1, _helper.alphaColor(light, 0.5));
            highlight.setColorAt(0.25, _helper.alphaColor(light, 0.3));
            highlight.setColorAt(0.5, _helper.alphaColor(light, 0.2));
            highlight.setColorAt(0.75, _helper.alphaColor(light, 0.1));
            highlight.setColorAt(0.9, Qt::transparent);

        } else {

            // inactive
            highlight.setColorAt(0.0, _helper.alphaColor(light, 0.1));
            highlight.setColorAt(0.4, _helper.alphaColor(dark, 0.5));
            highlight.setColorAt(0.8, _helper.alphaColor(dark, 0.4));
            highlight.setColorAt(0.9, Qt::transparent);

        }

        p->setRenderHints(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);

        p->setBrush(highlight);
        p->drawRoundedRect(fillRect,2,2);
    }

    //______________________________________________________________________________________________________________________________
    int Style::styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget, QStyleHintReturn * returnData) const
    {
        switch (hint) {

            case SH_ScrollView_FrameOnlyAroundContents:
            return true;

            case SH_ItemView_ShowDecorationSelected:
            return false;

            case SH_RubberBand_Mask:
            {
                const QStyleOptionRubberBand *opt = qstyleoption_cast<const QStyleOptionRubberBand *>(option);
                if( !opt) return false;
                if( QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask*>(returnData))
                {

                    mask->region = option->rect;

                    // need to check on widget before removing inner region
                    // in order to still preserve rubberband in MainWindow and QGraphicsView
                    // in QMainWindow because it looks better
                    // in QGraphicsView because the painting fails completely otherwise
                    if( !( widget && (
                        qobject_cast<const QGraphicsView*>( widget->parent() ) ||
                        qobject_cast<const QMainWindow*>( widget->parent() ) ) ) )
                        { mask->region -= option->rect.adjusted(1,1,-1,-1); }

                        return true;
                }
                return false;
            }

            // set no mask for window frame as round corners are rendered antialiased
            case SH_WindowFrame_Mask: return true;

            case SH_ToolTip_Mask:
            case SH_Menu_Mask:
            {

                if( !hasAlphaChannel( widget ) && (!widget || widget->isWindow() ) )
                {

                    // mask should be set only if compositing is disabled
                    if( QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData))
                    { mask->region = _helper.roundedMask( option->rect ); }

                }

                return true;

            }

            default: return KStyle::styleHint(hint, option, widget, returnData);
        }
    }

    //______________________________________________________________________________________________________________________________
    int Style::pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const
    {
        switch(m)
        {

            case PM_DefaultTopLevelMargin: return 11;
            case PM_DefaultChildMargin: return 4;
            case PM_DefaultLayoutSpacing: return 4;
            case PM_ButtonMargin: return 5;

            case PM_DefaultFrameWidth:
            if( !widget ) break;
            else if( qobject_cast<const QLineEdit*>(widget) ) return 3;
            else if( const QFrame* frame = qobject_cast<const QFrame*>(widget) )
            {
                return frame->frameShadow() == QFrame::Raised ? 4:3;

            } else if( qobject_cast<const QComboBox*>(widget)) return 3;
            break;

            // tooltip label
            case PM_ToolTipLabelFrameWidth:
            {
                if( !OxygenStyleConfigData::toolTipDrawStyledFrames ) break;
                return 3;
            }

            // spacing between widget and scrollbars
            case PM_ScrollView_ScrollBarSpacing: return -2;

            default: break;
        }

        return KStyle::pixelMetric(m,opt,widget);

    }

    //______________________________________________________________________________________________________________________________
    QSize Style::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const
    {
        switch(type)
        {
            case CT_GroupBox:
            {
                // adjust groupbox width to bold label font
                if( const QStyleOptionGroupBox* gbOpt = qstyleoption_cast<const QStyleOptionGroupBox*>(option))
                {
                    QSize size = KStyle::sizeFromContents(type, option, contentsSize, widget);
                    int labelWidth = subControlRect(CC_GroupBox, gbOpt, SC_GroupBoxLabel, widget).width();
                    size.setWidth(qMax(size.width(), labelWidth));
                    return size;
                }
            }
            case CT_ToolButton:
            {
                QSize size = contentsSize;

                if( const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option) )
                {

                    if( (!tbOpt->icon.isNull()) && (!tbOpt->text.isEmpty()) && tbOpt->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                    {

                        // TODO: Make this font size dependent
                        size.setHeight(size.height()-5);

                    }

                }

                // We want to avoid super-skiny buttons, for things like "up" when icons + text
                // For this, we would like to make width >= height.
                // However, once we get here, QToolButton may have already put in the menu area
                // (PM_MenuButtonIndicator) into the width. So we may have to take it out, fix things
                // up, and add it back in. So much for class-independent rendering...
                int   menuAreaWidth = 0;
                if( const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option))
                {

                    if( tbOpt->features & QStyleOptionToolButton::MenuButtonPopup)
                    {

                        menuAreaWidth = pixelMetric(QStyle::PM_MenuButtonIndicator, option, widget);

                    } else if( tbOpt->features & QStyleOptionToolButton::HasMenu) {

                        size.setWidth(size.width() + widgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorSize, tbOpt, widget));

                    }

                }
                size.setWidth(size.width() - menuAreaWidth);
                if( size.width() < size.height())
                    size.setWidth(size.height());
                size.setWidth(size.width() + menuAreaWidth);

                const QToolButton* t=qobject_cast<const QToolButton*>(widget);
                if( t && t->autoRaise()==true)
                {
                    int width = size.width() +
                        2*widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + MainMargin, option, widget) +
                        widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + Left, option, widget) +
                        widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + Right, option, widget);

                    int height = size.height() +
                        2*widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + MainMargin, option, widget) +
                        widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + Top, option, widget) +
                        widgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin + Bot, option, widget);

                    return QSize(width, height);

                } else {

                    int width = size.width() +
                        2*widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + MainMargin, option, widget);

                    int height = size.height() +
                        2*widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + MainMargin, option, widget)
                        + widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Top, option, widget)
                        + widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Bot, option, widget);

                    return QSize(width, height);
                }
            }

            // combobox
            case CT_ComboBox:
            {
                QSize size( KStyle::sizeFromContents( type, option, contentsSize, widget));
                const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option);
                if( cb && !cb->editable && !cb->currentIcon.isNull() ) size.rheight()+=1;

                // also expand to account for scrollbar
                size.rwidth() += OxygenStyleConfigData::scrollBarWidth() - 6;
                return size;

            }

            // separators
            case CT_MenuItem:
            {

                if( const QStyleOptionMenuItem* menuItemOption = qstyleoption_cast<const QStyleOptionMenuItem*>(option) )
                {
                    if( menuItemOption->menuItemType == QStyleOptionMenuItem::Separator )
                    {

                        // separator can have a title and an icon
                        // in that case they are rendered as menubar 'title', which
                        // corresponds to checked toolbuttons.
                        // a rectangle identical to the one of normal items is returned.
                        if( !( menuItemOption->text.isEmpty() && menuItemOption->icon.isNull() ) )
                        {
                            QStyleOptionMenuItem local( *menuItemOption );
                            local.menuItemType = QStyleOptionMenuItem::Normal;
                            return sizeFromContents( type, &local, contentsSize, widget );
                        }

                    }
                }

                // in all other cases the default behavior is good enough
                break;

            }

            case CT_TabWidget:
            {

                if( qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(option) ) return KStyle::sizeFromContents( type, option, contentsSize, widget );
                else {
                    // this handles tab layout properly in all tested cases event if the option does not have the correct style.
                    int m = widgetLayoutProp(WT_TabWidget, TabWidget::ContentsMargin, option, widget);
                    return KStyle::sizeFromContents( type, option, contentsSize, widget ) + QSize( m, 0 );
                }
            }

            default: break;
        }
        return KStyle::sizeFromContents(type, option, contentsSize, widget);
    }

    //______________________________________________________________________________________________________________________________
    QRect Style::subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
    {
        QRect r = option->rect;

        switch (control)
        {
            case CC_GroupBox:
            {
                const QStyleOptionGroupBox *gbOpt = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
                if( !gbOpt) break;

                bool isFlat = gbOpt->features & QStyleOptionFrameV2::Flat;

                switch (subControl)
                {

                    case SC_GroupBoxFrame: return r.adjusted( -1, -2, 1, 0 );

                    case SC_GroupBoxContents:
                    {
                        int th = gbOpt->fontMetrics.height() + 8;
                        QRect cr = subElementRect(SE_CheckBoxIndicator, option, widget);
                        int fw = widgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, option, widget);

                        bool checkable = gbOpt->subControls & QStyle::SC_GroupBoxCheckBox;
                        bool emptyText = gbOpt->text.isEmpty();

                        r.adjust(fw, fw, -fw, -fw);
                        if( checkable && !emptyText) r.adjust(0, qMax(th, cr.height()), 0, 0);
                        else if( checkable) r.adjust(0, cr.height(), 0, 0);
                        else if( !emptyText) r.adjust(0, th, 0, 0);

                        // add additional indentation to flat group boxes
                        if( isFlat)
                        {
                            int leftMarginExtension = 16;
                            r = visualRect(option->direction,r,r.adjusted(leftMarginExtension,0,0,0));
                        }

                        return r;
                    }

                    case SC_GroupBoxCheckBox:
                    case SC_GroupBoxLabel:
                    {
                        QFont font = widget->font();

                        // calculate text width assuming bold text in flat group boxes
                        if( isFlat ) font.setBold(true);

                        QFontMetrics fontMetrics = QFontMetrics(font);
                        int h = fontMetrics.height();
                        int tw = fontMetrics.size(Qt::TextShowMnemonic, gbOpt->text + QLatin1String("  ")).width();
                        r.setHeight(h);

                        // translate down by 6 pixels in non flat mode,
                        // to avoid collision with groupbox frame
                        if( !isFlat ) r.moveTop(6);

                        QRect cr;
                        if(gbOpt->subControls & QStyle::SC_GroupBoxCheckBox)
                        {
                            cr = subElementRect(SE_CheckBoxIndicator, option, widget);
                            QRect gcr((gbOpt->rect.width() - tw -cr.width())/2 , (h-cr.height())/2+r.y(), cr.width(), cr.height());
                            if(subControl == SC_GroupBoxCheckBox)
                            {
                                if( !isFlat) return visualRect(option->direction, option->rect, gcr);
                                else return visualRect(option->direction, option->rect, QRect(0,0,cr.width(),cr.height()));
                            }
                        }

                        // left align labels in flat group boxes, center align labels in framed group boxes
                        if( isFlat) r = QRect(cr.width(),r.y(),tw,r.height());
                        else r = QRect((gbOpt->rect.width() - tw - cr.width())/2 + cr.width(), r.y(), tw, r.height());

                        return visualRect(option->direction, option->rect, r);
                    }

                    default: break;

                }
                break;
            }

            case CC_ComboBox:
            {
                // add the same width as we do in eventFilter
                if(subControl == SC_ComboBoxListBoxPopup)
                { return r.adjusted(2,0,-2,0); }
            }

            default:
            break;
        }

        return KStyle::subControlRect(control, option, subControl, widget);
    }

    //______________________________________________________________________________________________________________________________
    QRect Style::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
    {
        QRect r;

        switch (sr)
        {

            case SE_TabBarTabText:
            {

                // bypass KStyle entirely because it makes it completely impossible
                // to handle both KDE and Qt applications at the same time
                QRect r( QCommonStyle::subElementRect(sr, opt, widget).adjusted( 6, 0, -6, 0 ) );
                if( const QStyleOptionTabV3* tov3 = qstyleoption_cast<const QStyleOptionTabV3*>(opt) )
                {
                    switch( tabOrientation( tov3->shape ) )
                    {

                        case TabEast:
                        case TabNorth:
                        if( opt->state & State_Selected ) r.translate( 0, -1 );
                        break;

                        default: break;

                    }

                }

                return r;

            }

            case SE_TabWidgetTabContents:
            {
                if( const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt))
                {
                    bool tabBarVisible( !twf->tabBarSize.isEmpty() );
                    if( !tabBarVisible ) return opt->rect;

                    QRect r = KStyle::subElementRect(sr, opt, widget);

                    // this is needed to match slab translations introduced for
                    // vertical alignment with labels
                    if( twf->lineWidth != 0 ) r.translate( 0, -1 );

                    switch( tabOrientation( twf->shape ) )
                    {
                        case TabNorth:
                        if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( 0, -3, 0, 0 );
                        break;

                        case TabSouth:
                        if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( 0, 0, 0, 2 );
                        else if( tabBarVisible ) r.adjust( 0, 0, 0, -1 );
                        break;

                        case TabEast:
                        if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( 0, 0, 3, 0 );
                        break;

                        case TabWest:
                        if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( -3, 0, 0, 0 );
                        break;

                        default:
                        break;

                    }

                    return r;

                } else return KStyle::subElementRect(sr, opt, widget);

            }

            case SE_TabWidgetTabPane:
            {
                if( const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt))
                {
                    QStyleOptionTab tabopt;
                    tabopt.shape = twf->shape;
                    int overlap = pixelMetric(PM_TabBarBaseOverlap, &tabopt, widget);

                    switch( tabOrientation( twf->shape ) )
                    {
                        case TabNorth:

                        // this line is what causes the differences between drawing corner widgets in KStyle and drawing them in Qt
                        // TODO: identify where the lineWidth difference come from
                        if( twf->lineWidth == 0) overlap -= 1;

                        r = QRect(QPoint(0,qMax(twf->tabBarSize.height() - overlap, 0)),
                            QSize(twf->rect.width(), qMin(twf->rect.height() - twf->tabBarSize.height() + overlap, twf->rect.height())));
                        break;

                        case TabSouth:
                        r = QRect(QPoint(0,0), QSize(twf->rect.width(), qMin(twf->rect.height() - twf->tabBarSize.height() + overlap, twf->rect.height())));
                        break;

                        case TabEast:
                        r = QRect(QPoint(0, 0), QSize(qMin(twf->rect.width() - twf->tabBarSize.width() + overlap, twf->rect.width()), twf->rect.height()));
                        break;

                        case TabWest:
                        r = QRect(QPoint(qMax(twf->tabBarSize.width() - overlap, 0), 0),
                        QSize(qMin(twf->rect.width() - twf->tabBarSize.width() + overlap, twf->rect.width()), twf->rect.height()));
                        break;
                    }

                }
                return r;
            }

            case SE_TabBarTabLeftButton:
            case SE_TabBarTabRightButton:
            {
                int offset(
                    (widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Top, opt, widget) -
                    widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Bot, opt, widget) )/2 );
                return KStyle::subElementRect( sr, opt, widget ).translated( 0, offset );
            }

            case SE_TabWidgetTabBar:
            {
                const QStyleOptionTabWidgetFrame *twf  = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
                if(!twf) return QRect();
                r = QRect(QPoint(0,0), twf->tabBarSize);

                switch( tabOrientation( twf->shape ) )
                {
                    case TabNorth:
                    {
                        r.setWidth(qMin(r.width(), twf->rect.width()
                            - twf->leftCornerWidgetSize.width()
                            - twf->rightCornerWidgetSize.width()));
                        r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(), 0));
                        r = visualRect(twf->direction, twf->rect, r);
                        break;
                    }

                    case TabSouth:
                    {
                        r.setWidth(qMin(r.width(), twf->rect.width()
                            - twf->leftCornerWidgetSize.width()
                            - twf->rightCornerWidgetSize.width()));
                        r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(),
                            twf->rect.height() - twf->tabBarSize.height()));
                        r = visualRect(twf->direction, twf->rect, r);
                        break;
                    }

                    case TabEast:
                    {
                        r.setHeight(qMin(r.height(), twf->rect.height()
                            - twf->leftCornerWidgetSize.height()
                            - twf->rightCornerWidgetSize.height()));
                        r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width(),
                            twf->leftCornerWidgetSize.height()));
                        break;
                    }

                    case TabWest:
                    {
                        r.setHeight(qMin(r.height(), twf->rect.height()
                            - twf->leftCornerWidgetSize.height()
                            - twf->rightCornerWidgetSize.height()));
                        r.moveTopLeft(QPoint(0, twf->leftCornerWidgetSize.height()));
                    }
                    break;
                }
                return r;
            }

            case SE_TabWidgetLeftCorner:
            {
                const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
                if(!twf) return QRect();
                const QTabWidget* tb = qobject_cast<const QTabWidget*>(widget);

                QRect paneRect = subElementRect(SE_TabWidgetTabPane, twf, widget);
                switch( tabOrientation( twf->shape ) )
                {
                    case TabNorth:
                    r = QRect(QPoint(paneRect.x(), paneRect.y() - twf->leftCornerWidgetSize.height() + (tb && tb->documentMode() ? 0 : gw)), twf->leftCornerWidgetSize);
                    r = visualRect(twf->direction, twf->rect, r);
                    r.adjust( 0, 3, 0, 2 );
                    break;

                    case TabSouth:
                    r = QRect(QPoint(paneRect.x(), paneRect.height() ), twf->leftCornerWidgetSize);
                    r = visualRect(twf->direction, twf->rect, r);
                    r.translate( 0, -4 );
                    break;

                    case TabWest:
                    r = QRect(QPoint(paneRect.x() - twf->leftCornerWidgetSize.width(), paneRect.y()), twf->leftCornerWidgetSize);
                    r.translate( 4, 0 );
                    break;

                    case TabEast:
                    r = QRect(QPoint(paneRect.x() + paneRect.width(), paneRect.y()), twf->leftCornerWidgetSize);
                    r.translate( -4, 0 );
                    break;
                    default:
                    break;
                }

                return r;

            }

            case SE_TabWidgetRightCorner:
            {
                const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
                if(!twf) return QRect();
                const QTabWidget* tb = qobject_cast<const QTabWidget*>(widget);

                QRect paneRect = subElementRect(SE_TabWidgetTabPane, twf, widget);
                switch( tabOrientation( twf->shape ) )
                {
                    case TabNorth:
                    r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.y() - twf->rightCornerWidgetSize.height() + (tb && tb->documentMode() ? 0 : gw)), twf->rightCornerWidgetSize);
                    r = visualRect(twf->direction, twf->rect, r);
                    r.adjust( 0, 3, 0, 2 );
                    break;

                    case TabSouth:
                    r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.height()), twf->rightCornerWidgetSize);
                    r = visualRect(twf->direction, twf->rect, r);
                    r.translate( 0, -4 );
                    break;

                    case TabWest:
                    r = QRect(QPoint(paneRect.x() - twf->rightCornerWidgetSize.width(), paneRect.y() + paneRect.height() - twf->rightCornerWidgetSize.height()), twf->rightCornerWidgetSize);
                    r.translate( 4, 0 );
                    break;

                    case TabEast:
                    r = QRect(QPoint(paneRect.x() + paneRect.width(), paneRect.y() + paneRect.height() - twf->rightCornerWidgetSize.height()), twf->rightCornerWidgetSize);
                    r.translate( -4, 0 );
                    break;

                    default:
                    break;
                }

                return r;
            }

            case SE_TabBarTearIndicator:
            {
                const QStyleOptionTab *option = qstyleoption_cast<const QStyleOptionTab *>(opt);
                if(!option) return QRect();

                switch( tabOrientation( option->shape ) )
                {
                    case TabNorth:
                    case TabSouth:
                    r.setRect(option->rect.left(), option->rect.top(), 8, option->rect.height());
                    break;

                    case TabWest:
                    case TabEast:
                    r.setRect(option->rect.left(), option->rect.top(), option->rect.width(), 8);
                    break;
                    default:
                    break;
                }

                r = visualRect(opt->direction, opt->rect, r);
                return r;
            }

            default:
            return KStyle::subElementRect(sr, opt, widget);
        }

    }

    //____________________________________________________________________________________
    void Style::renderWindowIcon(QPainter *p, const QRectF &r, int &type) const
    {
        // TODO: make icons smaller
        p->save();
        p->translate(r.topLeft());
        switch(type)
        {
            case Window::ButtonHelp:
            {
                p->translate(1.5, 1.5);
                p->drawArc(7,5,4,4,135*16, -180*16);
                p->drawArc(9,8,4,4,135*16,45*16);
                p->drawPoint(9,12);
                break;
            }
            case Window::ButtonMin:
            {
                p->drawLine(QPointF( 7.5, 9.5), QPointF(10.5,12.5));
                p->drawLine(QPointF(10.5,12.5), QPointF(13.5, 9.5));
                break;
            }
            case Window::ButtonRestore:
            {
                p->translate(1.5, 1.5);
                QPoint points[4] = {QPoint(9, 6), QPoint(12, 9), QPoint(9, 12), QPoint(6, 9)};
                p->drawPolygon(points, 4);
                break;
            }
            case Window::ButtonMax:
            {
                p->drawLine(QPointF( 7.5,11.5), QPointF(10.5, 8.5));
                p->drawLine(QPointF(10.5, 8.5), QPointF(13.5,11.5));
                break;
            }
            case Window::ButtonClose:
            {
                p->drawLine(QPointF( 7.5,7.5), QPointF(13.5,13.5));
                p->drawLine(QPointF(13.5,7.5), QPointF( 7.5,13.5));
                break;
            }
            case Window::ButtonShade:
            {
                p->drawLine(QPointF( 7.5, 13.5 ), QPointF(13.5, 13.5));
                p->drawLine(QPointF( 7.5, 7.5), QPointF(10.5,10.5));
                p->drawLine(QPointF(10.5,10.5), QPointF(13.5, 7.5));
                break;
            }
            case Window::ButtonUnshade:
            {
                p->drawLine(QPointF( 7.5,10.5), QPointF(10.5, 7.5));
                p->drawLine(QPointF(10.5, 7.5), QPointF(13.5,10.5));
                p->drawLine(QPointF( 7.5,13.0), QPointF(13.5,13.0));
                break;
            }
            default:
            break;
        }
        p->restore();
    }

    //_____________________________________________________________________
    bool Style::eventFilter(QObject *obj, QEvent *ev)
    {
        if( KStyle::eventFilter(obj, ev) ) return true;

        if( QToolBar *t = qobject_cast<QToolBar*>(obj) ) { return eventFilterToolBar( t, ev ); }
        if( QDockWidget*dw = qobject_cast<QDockWidget*>(obj) ) { return eventFilterDockWidget( dw, ev ); }
        if( QToolBox *tb = qobject_cast<QToolBox*>(obj) ) { return eventFilterToolBox( tb, ev ); }
        if( QMdiSubWindow *sw = qobject_cast<QMdiSubWindow*>(obj) ) { return eventFilterMdiSubWindow( sw, ev ); }

        // cast to QWidget
        QWidget *widget = static_cast<QWidget*>(obj);

        if( widget->inherits( "Q3ListView" ) ) { return eventFilterQ3ListView( widget, ev ); }
        if( widget->inherits( "QComboBoxPrivateContainer" ) ) { return eventFilterComboBoxContainer( widget, ev ); }
        if( widget->inherits( "QScrollBar" ) ) { return eventFilterScrollBar( widget, ev ); }
        if( widget->inherits( "KWin::GeometryTip" ) ) { return eventFilterGeometryTip( widget, ev ); }

        return false;

    }

    //_____________________________________________________________________
    bool Style::eventFilterToolBar( QToolBar* t, QEvent* ev )
    {
        switch(ev->type())
        {
            case QEvent::Show:
            case QEvent::Resize:
            {
                // make sure mask is appropriate
                if( t->isFloating() && !hasAlphaChannel(t) ) t->setMask(_helper.roundedMask( t->rect() ));
                else  t->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                QPainter p(t);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());

                QRect r = t->rect();
                QColor color = t->palette().window().color();

                // default painting when not floating
                if( !t->isFloating() ) {

                    // background has to be rendered explicitly
                    // when one of the parent has autofillBackground set to true
                    if( checkAutoFillBackground(t) )
                    { _helper.renderWindowBackground(&p, r, t, color); }

                    return false;

                }

                bool hasAlpha( hasAlphaChannel(t) );
                if( hasAlpha )
                {
                    p.setCompositionMode(QPainter::CompositionMode_Source );
                    TileSet *tileSet( _helper.roundCorner(color) );
                    tileSet->render( r, &p );

                    p.setCompositionMode(QPainter::CompositionMode_SourceOver );
                    p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );
                }

                // background
                _helper.renderWindowBackground(&p, r, t, color);

                if( t->isMovable() )
                {
                    // remaining painting: need to add handle
                    // this is copied from QToolBar::paintEvent
                    QStyleOptionToolBar opt;
                    opt.initFrom( t );
                    if( t->orientation() == Qt::Horizontal)
                    {
                        opt.rect = handleRTL( &opt, QRect( r.topLeft(), QSize( 8, r.height() ) ) );
                        opt.state |= QStyle::State_Horizontal;
                    } else {
                        opt.rect = handleRTL( &opt, QRect( r.topLeft(), QSize( r.width(), 8 ) ) );
                    }

                    drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, &p, t);
                }

                // frame
                if( hasAlpha ) p.setClipping( false );
                _helper.drawFloatFrame( &p, r, color, !hasAlpha );

                return true;

            }
            default: return false;
        }

    }

    //__________________________________________________________________________________
    bool Style::eventFilterQ3ListView( QWidget* widget, QEvent* ev )
    {
        // this apparently fixes a Qt bug with Q3ListView, consisting in
        // the fact that Focus events do not trigger repaint of these
        switch(ev->type())
        {
            case QEvent::FocusIn: widget->update(); return false;
            case QEvent::FocusOut: widget->update(); return false;
            default: return false;
        }

    }

    //_________________________________________________________
    bool Style::eventFilterComboBoxContainer( QWidget* widget, QEvent* ev )
    {
        switch(ev->type())
        {

            case QEvent::Show:
            case QEvent::Resize:
            {
                if( !hasAlphaChannel(widget) ) widget->setMask(_helper.roundedMask( widget->rect() ));
                else  widget->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                QPainter p(widget);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());

                QRect r = widget->rect();
                QColor color = widget->palette().window().color();

                bool hasAlpha( hasAlphaChannel( widget ) );
                if( hasAlpha )
                {
                    TileSet *tileSet( _helper.roundCorner(color) );
                    tileSet->render( r, &p );
                    p.setCompositionMode(QPainter::CompositionMode_SourceOver );
                    p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

                }

                _helper.renderMenuBackground( &p, e->rect(), widget, widget->palette() );

                // frame
                if( hasAlpha ) p.setClipping( false );

                _helper.drawFloatFrame( &p, r, color, !hasAlpha );
                return false;

            }
            default: return false;
        }
    }

    //_________________________________________________________
    bool Style::eventFilterScrollBar( QWidget* widget, QEvent* ev )
    {

        if( ev->type() == QEvent::Paint )
        {
            QPainter p( widget );
            p.setClipRegion( static_cast<QPaintEvent*>(ev)->region() );
            _helper.renderWindowBackground(&p, widget->rect(), widget,widget->palette());
        }

        return false;
    }

    //____________________________________________________________________________
    bool Style::eventFilterGeometryTip( QWidget* widget, QEvent* ev )
    {
        switch( ev->type() )
        {

            case QEvent::Show:
            case QEvent::Resize:
            {

                // make sure mask is appropriate
                if( !hasAlphaChannel(widget) ) widget->setMask(_helper.roundedMask( widget->rect() ));
                else  widget->clearMask();
                return false;
            }

            case QEvent::Paint:
            {

                QColor color = widget->palette().window().color();
                QRect r = widget->rect();

                QPainter p(widget);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());

                bool hasAlpha( hasAlphaChannel( widget ) );
                if( hasAlpha )
                {

                    p.setCompositionMode(QPainter::CompositionMode_Source );
                    TileSet *tileSet( _helper.roundCorner(color) );
                    tileSet->render( r, &p );

                    p.setCompositionMode(QPainter::CompositionMode_SourceOver );
                    p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

                }

                _helper.renderMenuBackground(&p, r, widget,color );

                // frame
                if( hasAlpha ) p.setClipping( false );
                _helper.drawFloatFrame( &p, r, color, !hasAlpha );

            }

            return false;

            default: return false;

        }

        // continue with normal painting
        return false;

    }

    //____________________________________________________________________________
    bool Style::eventFilterMdiSubWindow( QMdiSubWindow* sw, QEvent* ev )
    {

        if( ev->type() == QEvent::Paint)
        {

            QPainter p(sw);
            QRect clip( static_cast<QPaintEvent*>(ev)->rect() );
            if( sw->isMaximized() ) _helper.renderWindowBackground(&p, clip, sw, sw->palette() );
            else {

                p.setClipRect( clip );

                QRect r( sw->rect() );
                TileSet *tileSet( _helper.roundCorner( sw->palette().color( sw->backgroundRole() ) ) );
                tileSet->render( r, &p );

                p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );
                _helper.renderWindowBackground(&p, clip, sw, sw, sw->palette(), 0, 58 );

            }

        }

        // continue with normal painting
        return false;

    }

    //____________________________________________________________________________
    bool Style::eventFilterDockWidget( QDockWidget* dw, QEvent* ev )
    {
        switch( ev->type() )
        {
            case QEvent::Show:
            case QEvent::Resize:
            {
                // make sure mask is appropriate
                if( dw->isFloating() && !hasAlphaChannel(dw) ) dw->setMask(_helper.roundedMask( dw->rect() ));
                else  dw->clearMask();
                return false;
            }

            case QEvent::Paint:
            {
                QPainter p(dw);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());

                const QColor color = dw->palette().color(QPalette::Window);
                if(dw->isWindow())
                {

                    QPainter p(dw);
                    QRect r = dw->rect();
                    QColor color = dw->palette().window().color();

                    #ifndef Q_WS_WIN
                    bool hasAlpha( hasAlphaChannel(dw ) );
                    if( hasAlpha )
                    {
                        p.setCompositionMode(QPainter::CompositionMode_Source );
                        TileSet *tileSet( _helper.roundCorner(color) );
                        tileSet->render( r, &p );

                        // set clip region
                        p.setCompositionMode(QPainter::CompositionMode_SourceOver );
                        p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );
                    }
                    #endif

                    _helper.renderWindowBackground(&p, r, dw, color);

                    #ifndef Q_WS_WIN
                    if( hasAlpha ) p.setClipping( false );
                    #endif

                    _helper.drawFloatFrame(&p, r, color, !hasAlpha );

                } else {

                    QRect r( dw->rect() );

                    _helper.renderWindowBackground(&p, r, dw, color);

                    // adjust color
                    QColor local( _helper.backgroundColor( color, dw, r.center() ) );
                    TileSet *tileSet = _helper.dockFrame(local, r.width());
                    tileSet->render(r, &p);

                }

                return false;
            }

            default: return false;

        }

    }

    //____________________________________________________________________________
    bool Style::eventFilterToolBox( QToolBox* tb, QEvent* ev )
    {

        if( ev->type() == QEvent::Paint)
        {
            if(tb->frameShape() != QFrame::NoFrame)
            {

                QRect r = tb->rect();
                StyleOptions opts = NoFill;

                QPainter p(tb);
                p.setClipRegion(((QPaintEvent*)ev)->region());
                renderSlab(&p, r, tb->palette().color(QPalette::Button), opts);
            }
        }

        return false;
    }

    //____________________________________________________________________
    bool Style::hasAlphaChannel( const QWidget* widget ) const
    {
        #ifdef Q_WS_X11
        if( compositingActive() )
        {

            if( widget ) return widget->x11Info().depth() == 32;
            else return QX11Info().appDepth() == 32;

        } else return false;

        #else
        return compositingActive();
        #endif

    }

    //____________________________________________________________________
    const QWidget* Style::checkAutoFillBackground( const QWidget* w ) const
    {
        if( !w ) return NULL;
        if( w->autoFillBackground() ) return w;
        for( const QWidget* parent = w->parentWidget(); parent!=0; parent = parent->parentWidget() )
        {
            if( parent->autoFillBackground() ) return parent;
            if( parent == w->window() ) break;
        }

        return NULL;
    }

    //____________________________________________________________________
    QIcon Style::standardIconImplementation(
        StandardPixmap standardIcon,
        const QStyleOption *option,
        const QWidget *widget) const
    {
        // get button color (unfortunately option and widget might not be set)
        QColor buttonColor;
        QColor iconColor;
        if( option) {
            buttonColor = option->palette.window().color();
            iconColor   = option->palette.windowText().color();
        } else if( widget) {
            buttonColor = widget->palette().window().color();
            iconColor   = widget->palette().windowText().color();
        } else if( qApp) {
            // might not have a QApplication
            buttonColor = qApp->palette().window().color();
            iconColor   = qApp->palette().windowText().color();
        } else {// KCS is always safe
            buttonColor = KColorScheme(QPalette::Active, KColorScheme::Window,
                _helper.config()).background().color();
            iconColor   = KColorScheme(QPalette::Active, KColorScheme::Window,
                _helper.config()).foreground().color();
        }

        switch (standardIcon)
        {

            case SP_TitleBarNormalButton:
            {
                QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
                realpm.fill(Qt::transparent);
                QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
                QPainter painter(&realpm);
                painter.drawPixmap(1,1,pm);
                painter.setRenderHints(QPainter::Antialiasing);

                // should use the same icons as in the deco
                QPointF points[4] = {QPointF(8.5, 6), QPointF(11, 8.5), QPointF(8.5, 11), QPointF(6, 8.5)};
                {

                    qreal width( 1.1 );
                    painter.translate(0, 0.5);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawPolygon(points, 4);
                }

                {
                    qreal width( 1.1 );
                    painter.translate(0,-1);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawPolygon(points, 4);
                }
                painter.end();

                return QIcon(realpm);
            }

            case SP_TitleBarShadeButton:
            {
                QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
                realpm.fill(Qt::transparent);
                QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
                QPainter painter(&realpm);
                painter.drawPixmap(1,1,pm);
                painter.setRenderHints(QPainter::Antialiasing);
                {

                    qreal width( 1.1 );
                    painter.translate(0, 0.5);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
                    painter.drawLine( QPointF(8.75,8.75), QPointF(11.0,6.5) );
                    painter.drawLine( QPointF(6.5,11.0), QPointF(11.0,11.0) );
                }

                {
                    qreal width( 1.1 );
                    painter.translate(0,-1);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
                    painter.drawLine( QPointF(8.75,8.75), QPointF(11.0,6.5) );
                    painter.drawLine( QPointF(6.5,11.0), QPointF(11.0,11.0) );
                }

                painter.end();

                return QIcon(realpm);
            }

            case SP_TitleBarUnshadeButton:
            {
                QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
                realpm.fill(Qt::transparent);
                QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
                QPainter painter(&realpm);
                painter.drawPixmap(1,1,pm);
                painter.setRenderHints(QPainter::Antialiasing);

                {

                    qreal width( 1.1 );
                    painter.translate(0, 0.5);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine( QPointF(6.5,8.75), QPointF(8.75,6.5) );
                    painter.drawLine( QPointF(8.75,6.5), QPointF(11.0,8.75) );
                    painter.drawLine( QPointF(6.5,11.0), QPointF(11.0,11.0) );
                }

                {
                    qreal width( 1.1 );
                    painter.translate(0,-1);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine( QPointF(6.5,8.75), QPointF(8.75,6.5) );
                    painter.drawLine( QPointF(8.75,6.5), QPointF(11.0,8.75) );
                    painter.drawLine( QPointF(6.5,11.0), QPointF(11.0,11.0) );
                }
                painter.end();

                return QIcon(realpm);
            }

            case SP_TitleBarCloseButton:
            case SP_DockWidgetCloseButton:
            {
                QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
                realpm.fill(Qt::transparent);
                QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
                QPainter painter(&realpm);
                painter.drawPixmap(1,1,pm);
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setBrush(Qt::NoBrush);
                {

                    qreal width( 1.1 );
                    painter.translate(0, 0.5);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine( QPointF(6.5,6.5), QPointF(11.0,11.0) );
                    painter.drawLine( QPointF(11.0,6.5), QPointF(6.5,11.0) );
                }

                {
                    qreal width( 1.1 );
                    painter.translate(0,-1);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawLine( QPointF(6.5,6.5), QPointF(11.0,11.0) );
                    painter.drawLine( QPointF(11.0,6.5), QPointF(6.5,11.0) );
                }

                painter.end();

                return QIcon(realpm);
            }

            case SP_ToolBarHorizontalExtensionButton:
            {

                QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
                realpm.fill(Qt::transparent);
                QPainter painter(&realpm);
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setBrush(Qt::NoBrush);

                painter.translate( qreal(realpm.width())/2.0, qreal(realpm.height())/2.0 );

                bool reverse( option && option->direction == Qt::RightToLeft );
                QPolygonF a = genericArrow( reverse ? Generic::ArrowLeft:Generic::ArrowRight, ArrowTiny );
                {
                    qreal width( 1.1 );
                    painter.translate(0, 0.5);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawPolyline( a );
                }

                {
                    qreal width( 1.1 );
                    painter.translate(0,-1);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawPolyline( a );
                }

                return QIcon( realpm );
            }

            case SP_ToolBarVerticalExtensionButton:
            {
                QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
                realpm.fill(Qt::transparent);
                QPainter painter(&realpm);
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setBrush(Qt::NoBrush);

                QPolygonF a = genericArrow( Generic::ArrowDown, ArrowTiny );
                {
                    qreal width( 1.1 );
                    painter.translate(0, 0.5);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawPolyline( a );
                }

                {
                    qreal width( 1.1 );
                    painter.translate(0,-1);
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    painter.drawPolyline( a );
                }

                return QIcon( realpm );
            }

            default:
            return KStyle::standardIconImplementation(standardIcon, option, widget);
        }
    }

    //____________________________________________________________________
    QPoint Style::handleRTL(const QStyleOption* opt, const QPoint& pos) const
    { return visualPos(opt->direction, opt->rect, pos); }

    //____________________________________________________________________
    QRect Style::handleRTL(const QStyleOption* opt, const QRect& subRect) const
    { return visualRect(opt->direction, opt->rect, subRect); }

}
