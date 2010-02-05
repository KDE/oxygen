/* Oxygen widget style for KDE 4
   Copyright (C) 2008 Long Huynh Huu <long.upcase@googlemail.com>
   Copyright (C) 2007-2008 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
   Copyright (C) 2003-2005 Sandro Giessl <sandro@giessl.com>
   Copyright (C) 2001-2002 Chris Lee <clee@kde.org>
   Copyright (C) 2001-2002 Carsten Pfeiffer <pfeiffer@kde.org>
   Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>
   Copyright (c) 2002 Malte Starostik <malte@kde.org>
   Copyright (C) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>
   Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
   Copyright (C) 2001-2002 Fredrik Höglund  <fredrik@kde.org>
   Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
   Copyright (C) 2000 Dirk Mueller          <mueller@kde.org>
   Copyright (C) 2001 Martijn Klingens      <klingens@kde.org>

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

#include "oxygen.h"
#include "oxygen.moc"

#include <QtGui/QAbstractItemView>
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QGraphicsView>
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
#include <QtGui/QDial>

#include <QtDBus/QtDBus>

#include <KGlobal>
#include <KGlobalSettings>
#include <KConfigGroup>
#include <KColorUtils>
#include <KDebug>
#include <KWindowSystem>
#include <KTitleWidget>

#include "helper.h"
#include "lib/tileset.h"

#include "oxygenanimations.h"
#include "oxygentransitions.h"
#include "oxygenstyleconfigdata.h"

// We need better holes! Bevel color and shadow color are currently based on
// only one color, even though they are different things; also, we don't really
// know what bevel color should be based on... (and shadow color for white
// views looks rather bad). For now at least, just using QPalette::Window
// everywhere seems best...
#define HOLE_COLOR_OUTSIDE

/* These are to link libkio even if 'smart' linker is used */
#include <kio/authinfo.h>
extern "C" KIO::AuthInfo* _oxygen_init_kio() { return new KIO::AuthInfo(); }

K_EXPORT_STYLE("Oxygen", OxygenStyle)

K_GLOBAL_STATIC_WITH_ARGS(OxygenStyleHelper, globalHelper, ("oxygen"))

// ie glowwidth which we want to un-reserve space for in the tabs
static const int gw = 1;

// these parameters replace the KStyle PushButton::PressedShiftVertical because
// the latter is not flexible enough. They are implemented manually in
// OxygenStyle::drawControl
static const int pushButtonPressedShiftVertical = 1;
static const int toolButtonPressedShiftVertical = 1;

//_____________________________________________
static void cleanupBefore()
{
    OxygenStyleHelper *h = globalHelper;
    h->invalidateCaches();
}

//_____________________________________________
OxygenStyle::OxygenStyle() :
    KStyle(),
    CE_CapacityBar( newControlElement( "CE_CapacityBar" ) ),
    _helper(*globalHelper),
    _animations( new Oxygen::Animations( this ) ),
    _transitions( new Oxygen::Transitions( this ) )
{
    _sharedConfig = _helper.config();

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
    setWidgetLayoutProp( WT_ScrollBar, ScrollBar::BarWidth, OxygenStyleConfigData::scrollBarWidth());

    setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin, 5); //also used by toolbutton
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Left, 11);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Right, 11);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Top, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Bot, -1);
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

    setWidgetLayoutProp(WT_CheckBox, CheckBox::Size, 23);
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

    setWidgetLayoutProp(WT_SpinBox, SpinBox::FrameWidth, 4);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Left, 1);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Right, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Top, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Bot, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonWidth, 19);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonSpacing, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Left, 2);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Right, 8);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Top, 5);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Bot, 4);

    setWidgetLayoutProp(WT_ComboBox, ComboBox::FrameWidth, 4);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Left, 1);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Right, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Top, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Bot, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonWidth, 19);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Left, 2);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Right, 9);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Top, 6);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Bot, 3);
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

    setWidgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, 5);
    setWidgetLayoutProp(WT_GroupBox, GroupBox::TitleTextColor, ColorMode(QPalette::WindowText));

    setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin, 5);

    setWidgetLayoutProp(WT_Window, Window::TitleTextColor, QPalette::WindowText);

}

//___________________________________________________________________________________
void OxygenStyle::drawComplexControl(ComplexControl control,const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
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

        default: break;

    }

    return KStyle::drawComplexControl(control,option,painter,widget);

}

//___________________________________________________________________________________
bool OxygenStyle::drawGroupBoxComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{

    if(const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option))
    {
        bool isFlat = groupBox->features & QStyleOptionFrameV2::Flat;

        if (isFlat)
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
bool OxygenStyle::drawDialComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{

    const bool enabled = option->state & State_Enabled;
    const bool mouseOver(enabled && (option->state & State_MouseOver));
    const bool hasFocus( enabled && (option->state & State_HasFocus));

    StyleOptions opts = 0;
    if ((option->state & State_On) || (option->state & State_Sunken)) opts |= Sunken;
    if (option->state & State_HasFocus) opts |= Focus;
    if (enabled && (option->state & State_MouseOver)) opts |= Hover;

    animations().widgetStateEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
    animations().widgetStateEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

    QRect rect( option->rect );
    const QPalette &pal( option->palette );
    QColor color( pal.color(QPalette::Button) );

    if( enabled && animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationHover ) && !(opts & Sunken ) )
    {

        qreal opacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationHover ) );
        renderDialSlab( painter, rect, pal.color(QPalette::Button), option, opts, opacity, Oxygen::AnimationHover );

    } else if( enabled && !mouseOver && animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationFocus ) && !(opts & Sunken ) ) {

        qreal opacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationFocus ) );
        renderDialSlab( painter, rect, pal.color(QPalette::Button), option, opts, opacity, Oxygen::AnimationFocus );

    } else {

        renderDialSlab( painter, rect, pal.color(QPalette::Button), option, opts);

    }

    return true;

}

//___________________________________________________________________________________
bool OxygenStyle::drawToolButtonComplexControl( const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{

    if( !widget ) return false;

    // handle inactive (but animated) toolbuttons
    //Extract the stuff we need out of the option
    State flags( option->state );
    QRect rect( option->rect );
    QPalette palette( option->palette );
    QStyleOption tOpt(*option);

    bool isInToolBar( widget->parent() && widget->parent()->inherits( "QToolBar" ) );

    const bool enabled = flags & State_Enabled;
    const bool mouseOver(enabled && (flags & State_MouseOver));
    const bool hasFocus(enabled && (flags&State_HasFocus));
    const bool sunken( (flags & State_Sunken) || (flags & State_On) );

    if( isInToolBar )
    {

        animations().toolBarEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );

    } else {

        animations().widgetStateEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
        animations().widgetStateEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

    }

    bool hoverAnimated( isInToolBar ?
        animations().toolBarEngine().isAnimated( widget, Oxygen::AnimationHover ):
        animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationHover ) );

    bool focusAnimated( isInToolBar ?
        animations().toolBarEngine().isAnimated( widget, Oxygen::AnimationFocus ):
        animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationFocus ) );

    if( enabled && !(mouseOver || hasFocus || sunken ) )
    {

        if( hoverAnimated || (focusAnimated && !hasFocus) )
        {
            QRect buttonRect = subControlRect(CC_ToolButton, option, SC_ToolButton, widget);
            tOpt.rect = buttonRect;
            tOpt.state = flags;
            drawKStylePrimitive(WT_ToolButton, ToolButton::Panel, &tOpt, buttonRect, palette, flags, painter, widget);
        }

    }

    // always return false to continue with "default" painting
    return false;

}

//___________________________________________________________________________________
void OxygenStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const
{

    switch (element)
    {

        case PE_PanelMenu:
        {

            const QStyleOptionMenuItem* mOpt( qstyleoption_cast<const QStyleOptionMenuItem*>(option) );
            if( !mOpt ) return;
            QRect r = mOpt->rect;
            QColor color = mOpt->palette.window().color();

            if( compositingActive() )
            {
                TileSet *tileSet( _helper.roundCorner(color) );
                tileSet->render( r, p );

                // set clip region
                p->setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

            }

            // background
            int splitY = qMin(200, 3*r.height()/4);

            QRect upperRect = QRect(0, 0, r.width(), splitY);
            QPixmap tile = _helper.verticalGradient(color, splitY);
            p->drawTiledPixmap(upperRect, tile);

            QRect lowerRect = QRect(0,splitY, r.width(), r.height() - splitY);
            p->fillRect(lowerRect, _helper.backgroundBottomColor(color));

            // frame
            if( compositingActive() ) p->setClipping( false );
            _helper.drawFloatFrame( p, r, color );
            return;

        }

        // disable painting of PE_PanelScrollAreaCorner
        // the default implementation fills the rect with the window background color
        // which does not work for windows that have gradients.
        case PE_PanelScrollAreaCorner: return;

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
            if (flags & State_On) drawKStylePrimitive( WT_RadioButton, RadioButton::RadioOn, option, r, pal, flags, p, widget);
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
            if (flags & State_NoChange) drawKStylePrimitive( WT_CheckBox, CheckBox::CheckTriState, option, r, pal, flags, p, widget);
            else if (flags & State_On) drawKStylePrimitive( WT_CheckBox, CheckBox::CheckOn, option, r, pal, flags, p, widget);
            else drawKStylePrimitive( WT_CheckBox, CheckBox::CheckOff, option, r, pal, flags, p, widget);
            return;

        }


        default: KStyle::drawPrimitive( element, option, p, widget );
    }
}

//___________________________________________________________________________________
void OxygenStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const
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
            if (const QStyleOptionRubberBand *rbOpt = qstyleoption_cast<const QStyleOptionRubberBand *>(option))
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
        // same as QCommonStyle::drawControl, except that it handles animations
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(option))
        {
            QStyleOptionProgressBarV2 subopt = *pb;
            subopt.rect = subElementRect(SE_ProgressBarGroove, pb, widget);
            drawControl(CE_ProgressBarGroove, &subopt, p, widget);

            if( animations().progressBarEngine().isAnimated( widget ) )
            { subopt.progress = animations().progressBarEngine().value( widget ); }

            subopt.rect = subElementRect(SE_ProgressBarContents, &subopt, widget);
            drawControl(CE_ProgressBarContents, &subopt, p, widget);

            if (pb->textVisible)
            {
                subopt.rect = subElementRect(SE_ProgressBarLabel, pb, widget);
                drawControl(CE_ProgressBarLabel, &subopt, p, widget);
            }

        }
        return;

        case CE_ComboBoxLabel:
        //same as CommonStyle, except for fiilling behind icon
        {
            if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option))
            {

                QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);
                p->save();
                p->setClipRect(editRect);
                if (!cb->currentIcon.isNull())
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

                    if (cb->direction == Qt::RightToLeft) editRect.translate(-4 - cb->iconSize.width(), 0);
                    else editRect.translate(cb->iconSize.width() + 4, 0);
                }

                if (!cb->currentText.isEmpty() && !cb->editable)
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

            // check whether button is pressed
            const bool active = (option->state & State_On) || (option->state & State_Sunken);
            if( !active )  return KStyle::drawControl(element, option, p, widget);

            // cast option and check
            const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option);
            if( !( tbOpt && active ) ) return KStyle::drawControl(element, option, p, widget);

            // case button and check
            const QToolButton* toolButton( qobject_cast<const QToolButton*>( widget ) );
            if( !toolButton || toolButton->autoRaise() ) return KStyle::drawControl(element, option, p, widget);

            // check button parent. Right now the fix addresses only toolbuttons located
            // in a menu, in order to fix the KMenu title rendering issue
            if( !( toolButton->parent() && toolButton->parent()->inherits( "QMenu" ) ) )
            { return KStyle::drawControl(element, option, p, widget); }

            // adjust vertical position
            QStyleOptionToolButton local( *tbOpt );
            local.rect.translate( 0, toolButtonPressedShiftVertical );
            return KStyle::drawControl(element, &local, p, widget);

        }


        default: break;
    }
    KStyle::drawControl(element, option, p, widget);
}

//_________________________________________________________________________
void OxygenStyle::drawKStylePrimitive(WidgetType widgetType, int primitive,
    const QStyleOption* opt,
    const QRect &r,
    const QPalette &palette,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{

    QPalette pal( palette );
    if( widget && opt )
    {
      if( animations().widgetEnabilityEngine().isAnimated( widget, Oxygen::AnimationEnable ) )
      { pal = _helper.mergePalettes( palette, animations().widgetEnabilityEngine().opacity( widget, Oxygen::AnimationEnable )  ); }
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
bool OxygenStyle::drawPushButtonPrimitive(
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

            if ((flags & State_On) || (flags & State_Sunken)) opts |= Sunken;
            if (flags & State_HasFocus) opts |= Focus;
            if (enabled && (flags & State_MouseOver)) opts |= Hover;

            // update animation state
            animations().widgetStateEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
            animations().widgetStateEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

            // store animation state
            bool hoverAnimated( animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationHover ) );
            bool focusAnimated( animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationFocus ) );
            qreal hoverOpacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationHover ) );
            qreal focusOpacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationFocus ) );

            const QStyleOptionButton* bOpt( qstyleoption_cast< const QStyleOptionButton* >( opt ) );
            if( bOpt && ( bOpt->features & QStyleOptionButton::Flat ) )
            {

                // hover rect
                QRect slitRect = r;

                if( enabled && hoverAnimated )
                {

                    QColor glow( _helper.alphaColor( _viewFocusBrush.brush(QPalette::Active).color(), hoverOpacity ) );
                    _helper.slitFocused( glow )->render(slitRect, p);

                } else if( mouseOver) {

                    _helper.slitFocused(_viewFocusBrush.brush(QPalette::Active).color())->render(slitRect, p);

                }

            } else {

                if( enabled && hoverAnimated && !(opts & Sunken ) )
                {

                    renderButtonSlab( p, r, pal.color(QPalette::Button), opts, hoverOpacity, Oxygen::AnimationHover, TileSet::Ring );

                } else if( enabled && !mouseOver && focusAnimated && !(opts & Sunken ) ) {

                    renderButtonSlab( p, r, pal.color(QPalette::Button), opts, focusOpacity, Oxygen::AnimationFocus, TileSet::Ring );

                } else {

                    renderButtonSlab(p, r, pal.color(QPalette::Button), opts);

                }

            }

            return true;
        }

        case PushButton::DefaultButtonFrame: return true;
        default: return false;
    }

}

//___________________________________________________________________
bool OxygenStyle::drawToolBoxTabPrimitive(
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

    const bool reverseLayout = opt->direction == Qt::RightToLeft;
    switch (primitive)
    {
        case ToolBoxTab::Panel:
        {
            const QStyleOptionToolBox *option = qstyleoption_cast<const QStyleOptionToolBox *>(opt);
            if(!(option && widget)) return true;

            const QStyleOptionToolBoxV2 *v2 = qstyleoption_cast<const QStyleOptionToolBoxV2 *>(opt);

            if (v2 && v2->position == QStyleOptionToolBoxV2::Beginning) return true;

            p->save();
            QColor color = widget->palette().color(QPalette::Window); // option returns a wrong color
            QColor light = _helper.calcLightColor(color);
            QColor dark = _helper.calcDarkColor(color);

            QPainterPath path;
            int y = r.height()*15/100;
            if (reverseLayout) {
                path.moveTo(r.left()+52, r.top());
                path.cubicTo(QPointF(r.left()+50-8, r.top()), QPointF(r.left()+50-10, r.top()+y), QPointF(r.left()+50-10, r.top()+y));
                path.lineTo(r.left()+18+9, r.bottom()-y);
                path.cubicTo(QPointF(r.left()+18+9, r.bottom()-y), QPointF(r.left()+19+6, r.bottom()-1-0.3), QPointF(r.left()+19, r.bottom()-1-0.3));
            } else {
                path.moveTo(r.right()-52, r.top());
                path.cubicTo(QPointF(r.right()-50+8, r.top()), QPointF(r.right()-50+10, r.top()+y), QPointF(r.right()-50+10, r.top()+y));
                path.lineTo(r.right()-18-9, r.bottom()-y);
                path.cubicTo(QPointF(r.right()-18-9, r.bottom()-y), QPointF(r.right()-19-6, r.bottom()-1-0.3), QPointF(r.right()-19, r.bottom()-1-0.3));
            }

            p->setRenderHint(QPainter::Antialiasing, true);
            p->translate(0,1);
            p->setPen(light);
            p->drawPath(path);
            p->translate(0,-1);
            p->setPen(dark);
            p->drawPath(path);

            p->setRenderHint(QPainter::Antialiasing, false);
            if (reverseLayout) {
                p->drawLine(r.left()+50-1, r.top(), r.right(), r.top());
                p->drawLine(r.left()+20, r.bottom()-2, r.left(), r.bottom()-2);
                p->setPen(light);
                p->drawLine(r.left()+50, r.top()+1, r.right(), r.top()+1);
                p->drawLine(r.left()+20, r.bottom()-1, r.left(), r.bottom()-1);
            } else {
                p->drawLine(r.left(), r.top(), r.right()-50+1, r.top());
                p->drawLine(r.right()-20, r.bottom()-2, r.right(), r.bottom()-2);
                p->setPen(light);
                p->drawLine(r.left(), r.top()+1, r.right()-50, r.top()+1);
                p->drawLine(r.right()-20, r.bottom()-1, r.right(), r.bottom()-1);
            }

            p->restore();
            return true;
        }

        default: return false;
    }
}

//______________________________________________________
bool OxygenStyle::drawProgressBarPrimitive(
    int primitive,
    const QStyleOption* opt,
    const QRect &r, const QPalette &pal,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{

    Q_UNUSED( widget );
    Q_UNUSED( kOpt );

    const bool enabled = flags & State_Enabled;
    QColor bg = enabled?pal.color(QPalette::Base):pal.color(QPalette::Background); // background
    QColor fg = enabled?pal.color(QPalette::Highlight):pal.color(QPalette::Background).dark(110); // foreground
    const QStyleOptionProgressBarV2 *pbOpt = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt);
    Qt::Orientation orientation = pbOpt? pbOpt->orientation : Qt::Horizontal;

    QRect rect = r;

    switch (primitive)
    {
        case ProgressBar::Groove:
        {
            renderScrollBarHole(p, r, pal.color(QPalette::Window), orientation);
            return true;
        }

        case ProgressBar::Indicator:
        {
            if (r.width() < 2 || r.height() < 2) return true;
        }

        case ProgressBar::BusyIndicator:
        {

            QPixmap pixmap( _helper.progressBarIndicator( pal, rect ) );
            p->drawPixmap( rect.adjusted(-1, -2, 0, 0).topLeft(), pixmap );
            return true;

        }

        default: return false;

    }
}


//______________________________________________________
bool OxygenStyle::drawMenuBarPrimitive(
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
bool OxygenStyle::drawMenuBarItemPrimitive(
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
            bool active  = flags & State_Selected;

            bool animated( animations().menuBarEngine().isAnimated(widget, r.topLeft() ) );
            qreal opacity( animations().menuBarEngine().opacity( widget, r.topLeft() ) );
            QRect currentRect( animations().menuBarEngine().currentRect( widget, r.topLeft() ) );

            const bool current( currentRect.contains( r.topLeft() ) );

            if (active || animated )
            {
                QColor color = pal.color(QPalette::Window);
                if (OxygenStyleConfigData::menuHighlightMode() != OxygenStyleConfigData::MM_DARK)
                {

                    if(flags & State_Sunken)
                    {

                        if (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG) color = pal.color(QPalette::Highlight);
                        else color = KColorUtils::mix(color, KColorUtils::tint(color, pal.color(QPalette::Highlight), 0.6));

                    } else {

                        if (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG) color = KColorUtils::tint(color, _viewHoverBrush.brush(pal).color());
                        else color = KColorUtils::mix(color, KColorUtils::tint(color, _viewHoverBrush.brush(pal).color()));
                    }

                } else color = _helper.calcMidColor(color);

                // drawing
                if( animated && current ) {

                    color = KColorUtils::mix( pal.color(QPalette::Window), color, opacity );
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
            if (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG && (flags & State_Sunken) && (flags & State_Enabled) )
            { role = QPalette::HighlightedText; }

            drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled, textOpts->text, role);
            return true;
        }

        default: return false;
    }

}

//______________________________________________________
bool OxygenStyle::drawMenuPrimitive(
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
            bool animated( animations().menuEngine().isAnimated(widget, Oxygen::Previous ) );
            QRect previousRect( animations().menuEngine().currentRect( widget, Oxygen::Previous ) );
            qreal opacity(  animations().menuEngine().opacity( widget, Oxygen::Previous ) );

            if( animated && previousRect.intersects( r ) )
            {
                QColor color( _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, previousRect.center() ) );
                renderMenuItemRect( opt, previousRect, color, pal, p, opacity );
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
bool OxygenStyle::drawMenuItemPrimitive(
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

                        toolbuttonOpt.text = QFontMetrics( toolbuttonOpt.font )
                            .elidedText( menuItemOption->text, Qt::ElideRight, width );
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

            bool animated( animations().menuEngine().isAnimated(widget, Oxygen::Current ) );
            QRect currentRect( animations().menuEngine().currentRect( widget, Oxygen::Current ) );
            const bool intersected( currentRect.contains( r.topLeft() ) );

            if (enabled)
            {

                QColor color( _helper.menuBackgroundColor( pal.color( QPalette::Window ), widget, r.center() ) );
                if( animated && intersected ) renderMenuItemRect( opt, r, color, pal, p, animations().menuEngine().opacity( widget, Oxygen::Current ) );
                else renderMenuItemRect( opt, r, color, pal, p );

            } else drawKStylePrimitive(WT_Generic, Generic::FocusIndicator, opt, r, pal, flags, p, widget, kOpt);

            return true;
        }

        case Generic::Text:
        {
            KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
            QPalette::ColorRole role( QPalette::WindowText );
            if (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG && (flags & State_Selected) && (flags & State_Enabled) )
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
bool OxygenStyle::drawDockWidgetPrimitive(
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
            if (!dwOpt) return true;
            const QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(opt);
            bool verticalTitleBar = v2 ? v2->verticalTitleBar : false;

            QRect btnr = subElementRect(dwOpt->floatable ? SE_DockWidgetFloatButton : SE_DockWidgetCloseButton, opt, widget);
            int fw = widgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, opt, widget);
            QRect r = dwOpt->rect.adjusted(fw, fw, -fw, -fw);
            if (verticalTitleBar) {

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
            int th = dwOpt->fontMetrics.height();
            int width = verticalTitleBar ? r.height() : r.width();
            if (width < tw) title = dwOpt->fontMetrics.elidedText(title, Qt::ElideRight, width, Qt::TextShowMnemonic);

            if (verticalTitleBar)
            {

                // one should properly rotate/translate painter rather than using QImage
                QRect br(dwOpt->fontMetrics.boundingRect(title));
                QImage textImage(br.size(), QImage::Format_ARGB32_Premultiplied);
                textImage.fill(0x00000000);
                QPainter painter(&textImage);
                drawItemText(&painter, QRect(0, 0, br.width(), br.height()), Qt::AlignLeft|Qt::AlignTop|Qt::TextShowMnemonic, dwOpt->palette, dwOpt->state & State_Enabled, title, QPalette::WindowText);
                painter.end();
                textImage = textImage.transformed(QMatrix().rotate(-90));

                p->drawPixmap(r.x()+(r.width()-th)/2, r.y()+r.height()-textImage.height(), QPixmap::fromImage(textImage));

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
        if (flags&State_Horizontal) drawKStylePrimitive(WT_Splitter, Splitter::HandleVert, opt, r, pal, flags, p, widget);
        else drawKStylePrimitive(WT_Splitter, Splitter::HandleHor, opt, r, pal, flags, p, widget);
        return true;

        default: return false;

    }

}

//______________________________________________________
bool OxygenStyle::drawStatusBarPrimitive(
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
bool OxygenStyle::drawCheckBoxPrimitive(
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

            animations().widgetStateEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
            animations().widgetStateEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

            if( enabled && animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationHover ) )
            {

                qreal opacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationHover ) );
                renderCheckBox(p, r, pal, enabled, hasFocus, mouseOver, primitive, false, opacity, Oxygen::AnimationHover );

            } else if( enabled && !hasFocus && animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationFocus ) ) {

                qreal opacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationFocus ) );
                renderCheckBox(p, r, pal, enabled, hasFocus, mouseOver, primitive, false, opacity, Oxygen::AnimationFocus );

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
bool OxygenStyle::drawRadioButtonPrimitive(
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
            animations().widgetStateEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
            animations().widgetStateEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );
            if( enabled && animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationHover ) )
            {

                qreal opacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationHover ) );
                renderRadioButton(p, r, pal, enabled, hasFocus, mouseOver, primitive, true, opacity, Oxygen::AnimationHover );

            } else if(  enabled && animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationFocus ) ) {

                qreal opacity( animations().widgetStateEngine().opacity( widget, Oxygen::AnimationFocus ) );
                renderRadioButton(p, r, pal, enabled, hasFocus, mouseOver, primitive, true, opacity, Oxygen::AnimationFocus );

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
bool OxygenStyle::drawScrollBarPrimitive(
    int primitive,
    const QStyleOption* opt,
    const QRect &r, const QPalette &pal,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{

    Q_UNUSED( widget );
    Q_UNUSED( kOpt );

    const bool reverseLayout = opt->direction == Qt::RightToLeft;
    switch (primitive)
    {
        case ScrollBar::DoubleButtonHor:

        if (reverseLayout) renderScrollBarHole(p, QRect(r.right()+1, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Bottom | TileSet::Left);
        else renderScrollBarHole(p, QRect(r.left()-5, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Right | TileSet::Bottom);
        return false;

        case ScrollBar::DoubleButtonVert:
        renderScrollBarHole(p, QRect(0, r.top()-5, r.width(), 5), pal.color(QPalette::Window), Qt::Vertical, TileSet::Bottom | TileSet::Left | TileSet::Right);
        return false;

        case ScrollBar::SingleButtonHor:
        if (reverseLayout) renderScrollBarHole(p, QRect(r.left()-5, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Right | TileSet::Bottom);
        else renderScrollBarHole(p, QRect(r.right()+1, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal, TileSet::Top | TileSet::Left | TileSet::Bottom);
        return false;

        case ScrollBar::SingleButtonVert:
        renderScrollBarHole(p, QRect(0, r.bottom()+3, r.width(), 5), pal.color(QPalette::Window), Qt::Vertical, TileSet::Top | TileSet::Left | TileSet::Right);
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
            const bool enabled = flags & State_Enabled;
            bool animated( animations().scrollBarEngine().isAnimated( widget, SC_ScrollBarSlider ) );
            if( animated && enabled ) renderScrollBarHandle(p, r, pal, Qt::Horizontal, flags & State_MouseOver && flags & State_Enabled, animations().scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
            else renderScrollBarHandle(p, r, pal, Qt::Horizontal, (flags & State_MouseOver) && enabled );
            return true;
        }

        case ScrollBar::SliderVert:
        {
            const bool enabled = flags & State_Enabled;
            bool animated( animations().scrollBarEngine().isAnimated( widget, SC_ScrollBarSlider ) );
            if( animated && enabled ) renderScrollBarHandle(p, r, pal, Qt::Vertical, flags & State_MouseOver && flags & State_Enabled, animations().scrollBarEngine().opacity( widget, SC_ScrollBarSlider ) );
            else renderScrollBarHandle(p, r, pal, Qt::Vertical, (flags & State_MouseOver) && enabled );
            return true;
        }

        default: return false;

    }
}


//______________________________________________________
bool OxygenStyle::drawTabBarPrimitive(
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
            if (!tabOpt) return false;

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

            switch(option->shape)
            {

                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
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

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
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

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
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

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
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
            if (vertical) h = gr.height();
            else w = gr.width();

            QLinearGradient grad;
            if( reverseLayout && !vertical ) grad = QLinearGradient( 0, 0, w, h );
            else grad = QLinearGradient(w, h, 0, 0);

            grad.setColorAt(0, Qt::transparent );
            grad.setColorAt(0.6, Qt::black);
            grad.setColorAt(1, Qt::black);

            _helper.renderWindowBackground(&pp, pm.rect(), widget, pal);
            pp.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
            pp.fillRect(pm.rect(), QBrush(grad));
            pp.end();
            p->drawPixmap(gr.topLeft(),pm);

            if( !(documentMode && flags&State_Selected) )
            {
                // clipping is done by drawing the
                // window background over the requested rect
                if( clip.isValid() )
                {
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
            if (!tabOpt->tabBarRect.isValid() && !tabWidget) return true;

            const QWidget* leftWidget = ( tabWidget && widget->isVisible() && tabWidget->cornerWidget(Qt::TopLeftCorner) ) ? tabWidget->cornerWidget(Qt::TopLeftCorner):0;
            const QWidget* rightWidget = ( tabWidget && widget->isVisible() && tabWidget->cornerWidget(Qt::TopRightCorner) ) ? tabWidget->cornerWidget(Qt::TopRightCorner):0;

            switch(tabOpt->shape)
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                {

                    if (r.left() < tabOpt->tabBarRect.left())
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

                    if (tabOpt->tabBarRect.right() < r.right())
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

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                {

                    if (r.left() < tabOpt->tabBarRect.left())
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

                    if (tabOpt->tabBarRect.right() < r.right())
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

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
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

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
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
bool OxygenStyle::drawTabWidgetPrimitive(
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

            switch(tabOpt->shape)
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
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
                        if (lw > 0) renderSlab(p, QRect(r.right() - lw-7+gw, r.y()-gw, lw+7, 7), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);

                    } else {

                        // left side
                        if (lw > 0) renderSlab(p, QRect(-gw, r.y()-gw, lw+11, 7), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);

                        // right side
                        QRect slabRect( w+lw-7, r.y()-gw, r.width() - w - lw+7+gw, 7);
                        if( tw ) slabRect.setLeft( qMin( slabRect.left(), tw->rect().right() - rw - 7 ) );
                        renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);

                    }

                } else {

                    renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Ring);

                }

                return true;

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
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

                        if (lw > 0) renderSlab(p, QRect(r.right() - lw-7+gw, r.bottom()-7+gw, lw+7, 7), pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);

                    } else {

                        if (lw > 0) renderSlab(p, QRect(-gw, r.bottom()-7+gw, lw+7+gw, 7), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);

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

                } else {

                    renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Ring);

                }
                return true;

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                if( h+lh > 0 )
                {

                    renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right | TileSet::Bottom);

                    if (lh > 0) renderSlab(p, QRect(r.x()-gw, r.y()-gw, 7, lh+7 + 1), pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);
                    QRect slabRect( r.x()-gw, r.y()+h+lh-7, 7, r.height() - h - lh+7+gw);
                    if( tw ) slabRect.setTop( qMin( slabRect.top(), tw->rect().bottom() - rh -7 ) );
                    renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);

                } else {

                    renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Ring);

                }

                return true;

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                if( h+lh > 0 )
                {
                    renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left | TileSet::Bottom);

                    if (lh > 0) renderSlab(p, QRect(r.right()+1-7+gw, r.y()-gw, 7, lh+7+gw), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);

                    QRect slabRect(r.right()+1-7+gw, r.y()+h+lh-7, 7, r.height() - h - lh+7+gw );
                    if( tw ) slabRect.setTop( qMin( slabRect.top(), tw->rect().bottom() - rh -7 ) );
                    renderSlab(p, slabRect, pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);

                } else {

                    renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill, TileSet::Ring);

                }

                return true;

                default: return true;
            }
        }

        default: return false;

    }
}

//_________________________________________________________
bool OxygenStyle::drawWindowPrimitive(
    int primitive,
    const QStyleOption* opt,
    const QRect &r, const QPalette &pal,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{
    Q_UNUSED( widget );

    switch (primitive)
    {
        case Generic::Frame:
        {
            _helper.drawFloatFrame(p, r, pal.window().color());
            return true;
        }

        case Generic::Text:
        {

            const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt);
            bool active = (tb->titleBarState & Qt::WindowActive );
            KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);
            QPalette local( pal );
            local.setCurrentColorGroup( active ? QPalette::Active: QPalette::Disabled );
            drawItemText( p, r, Qt::AlignVCenter | textOpts->hAlign, local, active, textOpts->text, QPalette::WindowText);

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
            State bflags = flags;
            bflags &= ~State_Sunken;
            p->save();
            p->drawPixmap(r.topLeft(), _helper.windecoButton(pal.window().color(), tbkOpts->active,  r.height()));
            p->setRenderHints(QPainter::Antialiasing);
            p->setBrush(Qt::NoBrush);

            const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt);
            bool active = (tb->titleBarState & Qt::WindowActive );
            QColor color( pal.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText ) );

            {
                // contrast pixel is achieved by translating
                // down the icon and painting it with white color
                qreal width( 1.1 );
                p->translate(0, 0.5);
                p->setPen(QPen( _helper.calcLightColor( color ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                renderWindowIcon(p, QRectF(r).adjusted(-2.5,-2.5,0,0), primitive);
            }

            {
                // main icon painting
                qreal width( 1.1 );
                p->translate(0,-1);
                p->setPen(QPen( color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                renderWindowIcon(p, QRectF(r).adjusted(-2.5,-2.5,0,0), primitive);
            }

            p->restore();
            return true;
        }

        default: return false;
    }
}

//_________________________________________________________
bool OxygenStyle::drawSplitterPrimitive(
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
    qreal opacity( Oxygen::AnimationData::OpacityInvalid );

    // try retrieve QSplitterHandle, from painter device.
    if( enabled )
    {
        if( const QSplitterHandle* handle = dynamic_cast<const QSplitterHandle*>(p->device()) )
        {

            animations().widgetStateEngine().updateState( handle, Oxygen::AnimationHover, mouseOver );
            animated = animations().widgetStateEngine().isAnimated( handle, Oxygen::AnimationHover );
            opacity = animations().widgetStateEngine().opacity( handle, Oxygen::AnimationHover );

        } else if( widget && widget->inherits( "QMainWindow" ) ) {

            animations().dockSeparatorEngine().updateRect( widget, r, mouseOver );
            animated = animations().dockSeparatorEngine().isAnimated( widget, r );
            opacity = animated ? animations().dockSeparatorEngine().opacity( widget ) : Oxygen::AnimationData::OpacityInvalid;

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
bool OxygenStyle::drawSliderPrimitive(
    int primitive,
    const QStyleOption* opt,
    const QRect &r, const QPalette &pal,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{
    Q_UNUSED( widget );
    Q_UNUSED( kOpt );

    const bool enabled = flags & State_Enabled;
    const bool mouseOver(enabled && (flags & State_MouseOver));
    switch (primitive)
    {
        case Slider::HandleHor:
        case Slider::HandleVert:
        {
            StyleOptions opts = (flags & State_HasFocus ? Focus : StyleOption());
            if( enabled &&  animations().sliderEngine().isAnimated( widget ) )
            {

                renderSlab(p, r, pal.color(QPalette::Button), opts,  animations().sliderEngine().opacity( widget ), Oxygen::AnimationHover, TileSet::Ring );

            } else {

                if(const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt))
                { if( (slider->activeSubControls & SC_SliderHandle) && mouseOver ) opts |= Hover; }

                renderSlab(p, r, pal.color(QPalette::Button), opts);

            }

            return true;
        }

        case Slider::GrooveHor:
        case Slider::GrooveVert:
        {

            bool horizontal = primitive == Slider::GrooveHor;

            if (horizontal) {
                int center = r.y()+r.height()/2;
                _helper.groove(pal.color(QPalette::Window), 0.0)->render( QRect(r.left()+4, center-2, r.width()-8, 5), p);
            } else {
                int center = r.x()+r.width()/2;
                _helper.groove(pal.color(QPalette::Window), 0.0)->render(  QRect(center-2, r.top()+4, 5, r.height()-8), p);

            }

            return true;
        }

        default: return false;

    }

}

//_________________________________________________________
bool OxygenStyle::drawSpinBoxPrimitive(
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
            QRect fr = r.adjusted(2,2,-2,-2);
            p->save();
            p->setRenderHint(QPainter::Antialiasing);
            p->setPen(Qt::NoPen);
            p->setBrush(inputColor);


            const QStyleOptionSpinBox* sbOpt( qstyleoption_cast<const QStyleOptionSpinBox*>( opt ) );
            if( sbOpt && !sbOpt->frame )
            {
                // frameless spinbox
                // frame is adjusted to have the same dimensions as a frameless editor
                p->fillRect(fr.adjusted(4,4,-4,-4), inputColor);
                p->restore();

            } else {

                // normal spinbox
                #ifdef HOLE_NO_EDGE_FILL
                p->fillRect(fr.adjusted(3,3,-3,-3), inputColor);
                #else
                _helper.fillHole(*p, r.adjusted( 1, 0, -1, -1 ) );
                #endif

                p->restore();

                // TODO use widget background role?
                // We really need the color of the widget behind to be "right",
                // but the shadow needs to be colored as the inner widget; needs
                // changes in helper.

                #ifdef HOLE_COLOR_OUTSIDE
                QColor local( pal.color(QPalette::Window) );
                #else
                QColor local( inputColor );
                #endif

                animations().lineEditEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
                animations().lineEditEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );
                if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationFocus ) )
                {

                    renderHole(p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, Oxygen::AnimationFocus ), Oxygen::AnimationFocus, TileSet::Ring);

                } else if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationHover ) ) {

                    renderHole(p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, Oxygen::AnimationHover ), Oxygen::AnimationHover, TileSet::Ring);

                } else {

                    renderHole(p, local, fr, hasFocus, mouseOver);

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
bool OxygenStyle::drawComboBoxPrimitive(
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

    if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) )
    { editable = cb->editable; }

    bool hasFocus = flags & State_HasFocus;
    StyleOptions opts = (flags & State_HasFocus ? Focus : StyleOption());
    if (mouseOver) opts |= Hover;

    const QColor inputColor = enabled ? pal.color(QPalette::Base) : pal.color(QPalette::Window);
    QRect editField = subControlRect(CC_ComboBox, qstyleoption_cast<const QStyleOptionComplex*>(opt), SC_ComboBoxEditField, widget);

    animations().lineEditEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
    animations().lineEditEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

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

                } else if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationHover ) ) {

                    qreal opacity( animations().lineEditEngine().opacity( widget, Oxygen::AnimationHover ) );
                    renderButtonSlab( p, r, pal.color(QPalette::Button), opts, opacity, Oxygen::AnimationHover, TileSet::Ring );

                } else if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationFocus ) ) {

                    qreal opacity( animations().lineEditEngine().opacity( widget, Oxygen::AnimationFocus ) );
                    renderButtonSlab( p, r, pal.color(QPalette::Button), opts, opacity, Oxygen::AnimationFocus, TileSet::Ring );

                } else {

                    renderButtonSlab(p, r, pal.color(QPalette::Button), opts);

                }

            } else {

                QRect fr = r.adjusted(2,2,-2,-2);

                // input area
                p->save();
                p->setRenderHint(QPainter::Antialiasing);
                p->setPen(Qt::NoPen);
                p->setBrush(inputColor);

                if( cbOpt && !cbOpt->frame )
                {

                    // adjust rect to match frameLess editors
                    p->fillRect(fr.adjusted( 4, 4, -4, -4 ), inputColor);
                    p->restore();

                } else {

                    #ifdef HOLE_NO_EDGE_FILL
                    p->fillRect(fr.adjusted(3,3,-3,-3), inputColor);
                    #else
                    _helper.fillHole(*p, r.adjusted(1,0,-1,-1));
                    #endif

                    p->restore();

                    #ifdef HOLE_COLOR_OUTSIDE
                    QColor local( pal.color(QPalette::Window) );
                    #else
                    QColor local( inputColor );
                    #endif

                    if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationFocus ) )
                    {

                        renderHole(p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, Oxygen::AnimationFocus ), Oxygen::AnimationFocus, TileSet::Ring);

                    } else if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationHover ) ) {

                        renderHole(p, local, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, Oxygen::AnimationHover ), Oxygen::AnimationHover, TileSet::Ring);

                    } else {

                        renderHole(p, local, fr, hasFocus && enabled, mouseOver);

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
            drawKStylePrimitive(WT_Generic, Generic::ArrowDown, opt, r, pal, flags, p, widget, colorOpt );
            return true;
        }

        default: return false;

    }

}

//_________________________________________________________
bool OxygenStyle::drawHeaderPrimitive(
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
        case Header::SectionHor:
        case Header::SectionVert:
        {
            if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt))
            {

                bool isFirst = (primitive==Header::SectionHor)&&(header->position == QStyleOptionHeader::Beginning);
                p->save();
                p->setPen(pal.color(QPalette::Text));

                QColor color = pal.color(QPalette::Button);
                QColor dark  = _helper.calcDarkColor(color);
                QColor light = _helper.calcLightColor(color);

                QRect rect(r);

                p->fillRect(r, color);
                if(primitive == Header::SectionHor)
                {

                    if(header->section != 0 || isFirst)
                    {
                        int center = r.center().y();
                        int pos = (reverseLayout)? r.left()+1 : r.right()-1;
                        renderDot(p, QPointF(pos, center-3), color);
                        renderDot(p, QPointF(pos, center), color);
                        renderDot(p, QPointF(pos, center+3), color);
                    }
                    p->setPen(dark);
                    p->drawLine(rect.bottomLeft(), rect.bottomRight());
                    rect.adjust(0,0,0,-1);
                    p->setPen(light);
                    p->drawLine(rect.bottomLeft(), rect.bottomRight());

                } else {

                    int center = r.center().x();
                    int pos = r.bottom()-1;
                    renderDot(p, QPointF(center-3, pos), color);
                    renderDot(p, QPointF(center, pos), color);
                    renderDot(p, QPointF(center+3, pos), color);

                    if (reverseLayout)
                    {
                        p->setPen(dark); p->drawLine(rect.topLeft(), rect.bottomLeft());
                        rect.adjust(1,0,0,0);
                        p->setPen(light); p->drawLine(rect.topLeft(), rect.bottomLeft());
                    } else {
                        p->setPen(dark); p->drawLine(rect.topRight(), rect.bottomRight());
                        rect.adjust(0,0,-1,0);
                        p->setPen(light); p->drawLine(rect.topRight(), rect.bottomRight());
                    }
                }

                p->restore();
            }

            return true;
        }

        default: return false;
    }
}

//_________________________________________________________
bool OxygenStyle::drawTreePrimitive(
    int primitive,
    const QStyleOption* opt,
    const QRect &r, const QPalette &pal,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{

    const bool reverseLayout = opt->direction == Qt::RightToLeft;
    switch (primitive)
    {
        case Tree::VerticalBranch:
        case Tree::HorizontalBranch:
        {
            if (OxygenStyleConfigData::viewDrawTreeBranchLines())
            {
                QBrush brush(Qt::Dense4Pattern);
                QColor lineColor = pal.text().color();
                lineColor.setAlphaF(0.3);
                brush.setColor(lineColor);
                p->fillRect(r, brush);
            }
            return true;
        }

        case Tree::ExpanderOpen:
        case Tree::ExpanderClosed:
        {
            int radius = (r.width() - 4) / 2;
            int centerx = r.x() + r.width()/2;
            int centery = r.y() + r.height()/2;

            if(!OxygenStyleConfigData::viewDrawTriangularExpander())
            {
                // plus or minus
                p->save();
                p->setPen( pal.text().color() );
                p->drawLine( centerx - radius, centery, centerx + radius, centery );

                // Collapsed = On
                if (primitive == Tree::ExpanderClosed)
                { p->drawLine( centerx, centery - radius, centerx, centery + radius ); }

                p->restore();

            } else {

                KStyle::ColorOption* colorOpt   = extractOption<KStyle::ColorOption*>(kOpt);
                colorOpt->color = ColorMode( QPalette::Text );
                if(primitive == Tree::ExpanderClosed)
                {
                    drawKStylePrimitive(WT_Generic, reverseLayout? Generic::ArrowLeft : Generic::ArrowRight, opt, QRect(r.x()+1,r.y()+1,r.width(),r.height()), pal, flags, p, widget, colorOpt );
                } else {
                    drawKStylePrimitive(WT_Generic, Generic::ArrowDown, opt, QRect(r.x()+1,r.y()+1,r.width(),r.height()), pal, flags, p, widget, colorOpt );
                }
            }

            return true;
        }

        default: return false;

    }
}

//_________________________________________________________
bool OxygenStyle::drawLineEditPrimitive(
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

    switch (primitive)
    {
        case Generic::Frame:
        {
            const bool isReadOnly = flags & State_ReadOnly;
            const bool hasFocus = flags & State_HasFocus;

            #ifdef HOLE_COLOR_OUTSIDE
            const QColor inputColor =  pal.color(QPalette::Window);
            #else
            const QColor inputColor = enabled?pal.color(QPalette::Base):pal.color(QPalette::Window);
            #endif

            QRect fr( r.adjusted(2,2,-2,-2) );

            animations().lineEditEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
            animations().lineEditEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

            if( enabled && (!isReadOnly) && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationFocus ) )
            {

                renderHole(p, inputColor, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, Oxygen::AnimationFocus ), Oxygen::AnimationFocus, TileSet::Ring);

            } else if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationHover ) ) {

                renderHole(p, inputColor, fr, hasFocus, mouseOver, animations().lineEditEngine().opacity( widget, Oxygen::AnimationHover ), Oxygen::AnimationHover, TileSet::Ring);

            } else {

                renderHole(p, inputColor, fr, hasFocus, mouseOver);

            }
            return true;
        }

        case LineEdit::Panel:
        {
            if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
            {

                const QBrush inputBrush = enabled?panel->palette.base():panel->palette.window();
                const int lineWidth(panel->lineWidth);

                if (lineWidth > 0)
                {
                    p->save();
                    p->setRenderHint(QPainter::Antialiasing);
                    p->setPen(Qt::NoPen);
                    p->setBrush(inputBrush);

                    #ifdef HOLE_NO_EDGE_FILL
                    p->fillRect(r.adjusted(5,5,-5,-5), inputBrush);
                    #else
                    _helper.fillHole(*p, r.adjusted(1,0,-1,-1));
                    #endif

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
bool OxygenStyle::drawGroupBoxPrimitive(
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
            slopeTileSet->render(r, p);

            p->restore();

            return true;
        }

        case GroupBox::FlatFrame: return true;
        default: return false;

    }

}

//_________________________________________________________
bool OxygenStyle::drawToolBarPrimitive(
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
        return true;

        default: return false;
    }
}

//_________________________________________________________
bool OxygenStyle::drawToolButtonPrimitive(
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

            // toolbutton engine
            bool isInToolBar( widget && widget->parent() && widget->parent()->inherits( "QToolBar" ) );
            if( isInToolBar )
            {

              animations().toolBarEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );

            } else {

              animations().widgetStateEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
              animations().widgetStateEngine().updateState( widget, Oxygen::AnimationFocus, hasFocus );

            }

            bool hoverAnimated(
                isInToolBar ?
                animations().toolBarEngine().isAnimated( widget, Oxygen::AnimationHover ):
                animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationHover ) );

            bool focusAnimated(
                isInToolBar ?
                animations().toolBarEngine().isAnimated( widget, Oxygen::AnimationFocus ):
                animations().widgetStateEngine().isAnimated( widget, Oxygen::AnimationFocus ) );

            qreal hoverOpacity(
                isInToolBar ?
                animations().toolBarEngine().opacity( widget, Oxygen::AnimationHover ):
                animations().widgetStateEngine().opacity( widget, Oxygen::AnimationHover ) );

             qreal focusOpacity(
                isInToolBar ?
                animations().toolBarEngine().opacity( widget, Oxygen::AnimationFocus ):
                animations().widgetStateEngine().opacity( widget, Oxygen::AnimationFocus ) );

            // hover rect
            QRect slitRect = r;

            // cast
            const QToolButton* t=qobject_cast<const QToolButton*>(widget);
            if (t && !t->autoRaise())
            {
                StyleOptions opts = 0;

                // check if parent is tabbar
                if (const QTabBar *tb =  qobject_cast<const QTabBar*>(t->parent()))
                {

                    QPalette::ColorGroup colorGroup = tb->palette().currentColorGroup();
                    QTabWidget* tw( qobject_cast<QTabWidget*>(tb->parent() ) );
                    const bool documentMode( tb->documentMode() || !tw );

                    // get corner widgets if any
                    const QWidget* leftWidget( tw ? tw->cornerWidget( Qt::TopLeftCorner ):0 );
                    const QWidget* rightWidget( tw ? tw->cornerWidget( Qt::TopRightCorner ):0 );
                    //if( leftWidget && !leftWidget->isVisible() ) leftWidget = 0;
                    //if( rightWidget && !rightWidget->isVisible() ) rightWidget = 0;

                    // prepare painting, clipping and tiles
                    TileSet::Tiles tiles = 0;
                    QRect slabRect;
                    QRect clipRect;

                    // paint relevant region depending on tabbar shape and whether widget is on the edge
                    switch(tb->shape())
                    {
                        case QTabBar::RoundedNorth:
                        case QTabBar::TriangularNorth:
                        {

                            // need to swap left and right widgets in reverse mode
                            if( reverseLayout ) qSwap( leftWidget, rightWidget );

                            slitRect.adjust(0,3,0,-3-gw);
                            clipRect = r.adjusted(0,2-gw,0,-3);
                            tiles = TileSet::Top;

                            // check border right
                            if( !documentMode && !rightWidget && t->geometry().right() >= tb->rect().right() )
                            {

                                tiles |= TileSet::Right;
                                slabRect = QRect(r.left()-7, r.bottom()-6-gw, r.width()+7+1, 5);

                            } else if( !documentMode && !leftWidget && t->geometry().left() <= tb->rect().left() ) {

                                tiles |= TileSet::Left;
                                slabRect = QRect(r.left()-1, r.bottom()-6-gw, r.width()+7+1, 5);

                            } else {

                                slabRect = QRect(r.left()-7, r.bottom()-6-gw, r.width()+14, 5);

                            }

                            break;
                        }

                        case QTabBar::RoundedSouth:
                        case QTabBar::TriangularSouth:
                        {

                            // need to swap left and right widgets in reverse mode
                            if( reverseLayout ) qSwap( leftWidget, rightWidget );

                            slitRect.adjust(0,3+gw,0,-3);
                            tiles = TileSet::Bottom;
                            clipRect = r.adjusted(0,2+gw,0,0);

                            if( !documentMode && !rightWidget && t->geometry().right() >= tb->rect().right() )
                            {

                                tiles |= TileSet::Right;
                                slabRect = QRect(r.left()-7, r.top()+gw+1, r.width()+7+1, 5);

                            } else if( !documentMode && !leftWidget && t->geometry().left() <= tb->rect().left() ) {

                                tiles |= TileSet::Left;
                                slabRect = QRect(r.left()-1, r.top()+gw+1, r.width()+7+1, 5);

                            } else {

                                slabRect = QRect(r.left()-7, r.top()+gw+1, r.width()+14, 5);

                            }

                            break;
                        }

                        case QTabBar::RoundedEast:
                        case QTabBar::TriangularEast:
                        {

                            slitRect.adjust(3+gw,0,-3-gw,0);
                            tiles = TileSet::Right;
                            clipRect = r.adjusted(3+gw,0,-2,0);
                            if( !documentMode && !rightWidget && t->geometry().bottom() >= tb->rect().bottom() )
                            {
                                tiles |= TileSet::Bottom;
                                slabRect = QRect(r.left()+gw+3, r.top()-7, 4, r.height()+7+1);

                            } else {

                                slabRect = QRect(r.left()+gw+3, r.top()-7, 4, r.height()+14);

                            }

                            break;
                        }


                        case QTabBar::RoundedWest:
                        case QTabBar::TriangularWest:
                        {

                            // west
                            slitRect.adjust(3+gw,0,-3-gw,0);
                            tiles |= TileSet::Left;
                            clipRect = r.adjusted(2-gw,0,-3, 0);

                            if( !documentMode && !rightWidget && t->geometry().bottom() >= tb->rect().bottom() )
                            {

                                tiles |= TileSet::Bottom;
                                slabRect = QRect(r.right()-6-gw, r.top()-7, 5, r.height()+7+1);

                            } else {

                                slabRect = QRect(r.right()-6-gw, r.top()-7, 5, r.height()+14);

                            }

                            break;
                        }

                        default:
                        break;
                    }

                    if( clipRect.isValid() )
                    {
                        QPalette local( t->parentWidget() ? t->parentWidget()->palette() : pal );

                        // check whether parent has autofill background flag
                        if( const QWidget* parent = checkAutoFillBackground( t ) ) p->fillRect( clipRect, parent->palette().color( parent->backgroundRole() ) );
                        else _helper.renderWindowBackground(p, clipRect, t, local);

                    }

                    if( slabRect.isValid() )
                    {
                        p->save();
                        p->setClipRect( slabRect );
                        renderSlab(p, slabRect, pal.color(colorGroup, QPalette::Window), NoFill, tiles );
                        p->restore();
                    }

                } else {

                    // "normal" parent, and non "autoraised" (that is: always raised) buttons
                    if ((flags & State_On) || (flags & State_Sunken)) opts |= Sunken;
                    if (flags & State_HasFocus) opts |= Focus;
                    if (enabled && (flags & State_MouseOver)) opts |= Hover;

                    if (t->popupMode()==QToolButton::MenuButtonPopup) {

                        renderButtonSlab(p, r.adjusted(0,0,4,0), pal.color(QPalette::Button), opts, TileSet::Bottom | TileSet::Top | TileSet::Left);

                    } else if( enabled && hoverAnimated ) {

                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts, hoverOpacity, Oxygen::AnimationHover, TileSet::Ring );

                    } else if( enabled && !hasFocus && focusAnimated ) {

                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts, focusOpacity, Oxygen::AnimationFocus, TileSet::Ring );

                    } else {

                        renderButtonSlab( p, r, pal.color(QPalette::Button), opts);

                    }

                    return true;

                }
            }

            if( widget && widget->inherits("QDockWidgetTitleButton" ) )
            { slitRect.adjust( 1, 0, 0, 0 ); }

            // normal (auto-raised) toolbuttons
            bool hasFocus = flags & State_HasFocus;

            if((flags & State_Sunken) || (flags & State_On) )
            {

                if( enabled && hoverAnimated )
                {

                    renderHole(p, pal.color(QPalette::Window), slitRect, hasFocus, mouseOver, hoverOpacity, Oxygen::AnimationHover, TileSet::Ring );

                } else {

                    renderHole(p, pal.color(QPalette::Window), slitRect, hasFocus, mouseOver);

                }

            } else {

                if( enabled && hoverAnimated ) {

                    QColor glow( _helper.alphaColor( _viewFocusBrush.brush(QPalette::Active).color(), hoverOpacity ) );
                    _helper.slitFocused( glow )->render(slitRect, p);

                } else if (hasFocus || mouseOver) {

                    _helper.slitFocused(_viewFocusBrush.brush(QPalette::Active).color())->render(slitRect, p);

                }

            }

            return true;
        }

        default: return false;

    }

}


//_________________________________________________________
bool OxygenStyle::drawGenericPrimitive(
    WidgetType widgetType,
    int primitive,
    const QStyleOption* opt,
    const QRect &r, const QPalette &pal,
    State flags, QPainter* p,
    const QWidget* widget,
    KStyle::Option* kOpt) const
{

    const bool enabled = flags & State_Enabled;
    const bool mouseOver(enabled && (flags & State_MouseOver));

    StyleOptions opts = 0;
    switch (primitive)
    {

        case Generic::ArrowUp:
        case Generic::ArrowDown:
        case Generic::ArrowLeft:
        case Generic::ArrowRight:
        {

            p->save();

            // define gradient and polygon for drawing arrow
            QPolygonF a;
            switch (primitive)
            {
                case Generic::ArrowUp: {
                    a << QPointF( -3,2.5) << QPointF(0.5, -1.5) << QPointF(4,2.5);
                    break;
                }

                case Generic::ArrowDown: {
                    a << QPointF( -3,-2.5) << QPointF(0.5, 1.5) << QPointF(4,-2.5);
                    break;
                }

                case Generic::ArrowLeft: {
                    a << QPointF(2.5,-3) << QPointF(-1.5, 0.5) << QPointF(2.5,4);
                    break;
                }

                case Generic::ArrowRight: {
                    a << QPointF(-2.5,-3) << QPointF(1.5, 0.5) << QPointF(-2.5,4);
                    break;
                }

                default: break;

            }

            qreal penThickness = 1.6;
            bool drawContrast = true;
            KStyle::ColorOption* colorOpt = extractOption<KStyle::ColorOption*>(kOpt);
            QColor color = colorOpt->color.color(pal);
            QColor background = pal.color(QPalette::Window);

            // customize color depending on widget
            if( widgetType == WT_SpinBox )
            {
                // spinBox
                color = pal.color( QPalette::Text );
                background = pal.color( QPalette::Background );
                drawContrast = false;

            } else if( widgetType == WT_ComboBox ) {

                // combobox
                if( const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) )
                {

                    if( cb->editable )
                    {

                        color = pal.color( QPalette::Text );
                        background = pal.color( QPalette::Background );
                        if( enabled ) drawContrast = false;

                    } else {

                        color = pal.color( QPalette::ButtonText );
                        background = pal.color( QPalette::Button );

                    }

                }

            } else if(const QScrollBar* scrollbar = qobject_cast<const QScrollBar*>(widget) ) {


                // handle scrollbar arrow hover
                // first get relevant subcontrol type matching arrow
                SubControl subcontrol( SC_None );
                if( scrollbar->orientation() == Qt::Vertical )  subcontrol = (primitive == Generic::ArrowDown) ? SC_ScrollBarAddLine:SC_ScrollBarSubLine;
                else if( opt->direction == Qt::LeftToRight ) subcontrol = (primitive == Generic::ArrowLeft) ? SC_ScrollBarSubLine:SC_ScrollBarAddLine;
                else subcontrol = (primitive == Generic::ArrowLeft) ? SC_ScrollBarAddLine:SC_ScrollBarSubLine;

                if( enabled )
                {

                    bool hover( animations().scrollBarEngine().isHovered( widget, subcontrol ) );
                    bool animated( animations().scrollBarEngine().isAnimated( widget, subcontrol ) );
                    qreal opacity( animations().scrollBarEngine().opacity( widget, subcontrol ) );

                    QPoint position( hover ? scrollbar->mapFromGlobal( QCursor::pos() ) : QPoint( -1, -1 ) );
                    if( hover && r.contains( position ) )
                    {
                        // we need to update the arrow subcontrolRect on fly because there is no
                        // way to get it from the styles directly, outside of repaint events
                        animations().scrollBarEngine().setSubControlRect( widget, subcontrol, r );
                    }

                    QColor highlight = KColorScheme(pal.currentColorGroup()).decoration(KColorScheme::HoverColor).color();
                    if( r.intersects(  animations().scrollBarEngine().subControlRect( widget, subcontrol ) ) )
                    {

                        if( animated )
                        {
                            color = KColorUtils::mix( color, highlight, opacity );

                        } else if( hover ) {

                            color = highlight;

                        }

                    }

                }

            } else if (const QToolButton *tool = qobject_cast<const QToolButton *>(widget)) {

                // toolbutton animation
                animations().toolBarEngine().updateState( widget, Oxygen::AnimationHover, mouseOver );
                bool animated( animations().toolBarEngine().isAnimated( widget, Oxygen::AnimationHover ) );
                qreal opacity( animations().toolBarEngine().opacity( widget, Oxygen::AnimationHover ) );

                QColor highlight = KColorScheme(pal.currentColorGroup()).decoration(KColorScheme::HoverColor).color();
                color = pal.color( QPalette::WindowText );

                // toolbuttons
                if (tool->popupMode()==QToolButton::MenuButtonPopup)
                {

                    if(!tool->autoRaise())
                    {

                        color = pal.color( QPalette::ButtonText );
                        background = pal.color( QPalette::Button );
                        if( (flags & State_On) || (flags & State_Sunken) ) opts |= Sunken;
                        if( flags & State_HasFocus ) opts |= Focus;
                        if( mouseOver ) opts |= Hover;
                        renderSlab(p, r.adjusted(-10,0,0,0), pal.color(QPalette::Button), opts, TileSet::Bottom | TileSet::Top | TileSet::Right);

                        a.translate(-3,1);

                        //Draw the dividing line
                        QColor color = pal.color(QPalette::Window);
                        QColor light = _helper.calcLightColor(color);
                        QColor dark = _helper.calcDarkColor(color);
                        dark.setAlpha(200);
                        light.setAlpha(150);
                        p->setPen(QPen(light,1));
                        p->drawLine(r.x()-5, r.y()+3, r.x()-5, r.bottom()-4);
                        p->drawLine(r.x()-3, r.y()+3, r.x()-3, r.bottom()-3);
                        p->setPen(QPen(dark,1));
                        p->drawLine(r.x()-4, r.y()+4, r.x()-4, r.bottom()-3);

                    } else {

                        // this does not really work
                        // in case of menu tool-buttons, one should animate the
                        // menu arrow independently from the button itself
                        if( animated ) color = KColorUtils::mix( color, highlight, opacity );
                        else if( mouseOver ) color = highlight;
                        else color = pal.color( QPalette::WindowText );

                    }

                } else {

                    if( animated ) color = KColorUtils::mix( color, highlight, opacity );
                    else if( mouseOver ) color = highlight;
                    else color = pal.color( QPalette::WindowText );

                    // smaller down arrow for menu indication on toolbuttons
                    penThickness = 1.4;
                    a.clear();

                    switch (primitive)
                    {
                        case Generic::ArrowUp: {
                            a << QPointF( -2,1.5) << QPointF(0.5, -1.5) << QPointF(3,1.5);
                            break;
                        }
                        case Generic::ArrowDown: {
                            a << QPointF( -2,-1.5) << QPointF(0.5, 1.5) << QPointF(3,-1.5);
                            break;
                        }
                        case Generic::ArrowLeft: {
                            a << QPointF(1.5,-2) << QPointF(-1.5, 0.5) << QPointF(1.5,3);
                            break;
                        }
                        case Generic::ArrowRight: {
                            a << QPointF(-1.5,-2) << QPointF(1.5, 0.5) << QPointF(-1.5,3);
                            break;
                        }

                        default: break;
                    }
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

        case Generic::Frame:
        {

            // WT_Generic and other fallen-through frames...
            // QFrame, Qt item views, etc.: sunken..
            bool hoverHighlight = flags&State_MouseOver;
            bool focusHighlight = flags&State_HasFocus;

            animations().lineEditEngine().updateState( widget, Oxygen::AnimationHover, hoverHighlight );
            animations().lineEditEngine().updateState( widget, Oxygen::AnimationFocus, focusHighlight );
            if (flags & State_Sunken)
            {

                if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationFocus ) )
                {

                    renderHole(p, pal.color(QPalette::Window), r, focusHighlight, hoverHighlight,
                        animations().lineEditEngine().opacity( widget, Oxygen::AnimationFocus ),
                        Oxygen::AnimationFocus, TileSet::Ring );

                } else if( enabled && animations().lineEditEngine().isAnimated( widget, Oxygen::AnimationHover ) ) {

                    renderHole(p, pal.color(QPalette::Window), r, focusHighlight, hoverHighlight,
                        animations().lineEditEngine().opacity( widget, Oxygen::AnimationHover ),
                        Oxygen::AnimationHover, TileSet::Ring );

                } else {

                    renderHole(p, pal.color(QPalette::Window), r, focusHighlight, hoverHighlight);

                }

            } else if(widgetType == WT_Generic && (flags & State_Raised)) {

                renderSlab(p, r.adjusted(-2, -2, 2, 2), pal.color(QPalette::Background), NoFill);

            }

            return false;
        }

        case Generic::FocusIndicator:
        {

            if( const QAbstractItemView *aiv = qobject_cast<const QAbstractItemView*>(widget) )
            {
                if( OxygenStyleConfigData::viewDrawFocusIndicator() &&
                  aiv->selectionMode() != QAbstractItemView::SingleSelection &&
                  aiv->selectionMode() != QAbstractItemView::NoSelection)
                {
                    QLinearGradient lg(r.adjusted(2,0,0,-2).bottomLeft(), r.adjusted(0,0,-2,-2).bottomRight());
                    lg.setColorAt(0.0, Qt::transparent);

                    if (flags & State_Selected) {

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
                    p->drawLine(r.adjusted(2,0,0,-2).bottomLeft(), r.adjusted(0,0,-2,-2).bottomRight());
                    p->restore();

                  }
            }

            // we don't want the stippled focus indicator in oxygen
            if( !( widget && widget->inherits("Q3ListView") ) ) return true;
            else return false;
        }

        default: return false;
    }

}

//______________________________________________________________
void OxygenStyle::drawCapacityBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const
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
void OxygenStyle::registerScrollArea( QAbstractScrollArea* scrollArea ) const
{

    if( !scrollArea ) return;

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
void OxygenStyle::polish(QWidget* widget)
{
    if (!widget) return;

    // register widget to animations
    animations().registerWidget( widget );
    transitions().registerWidget( widget );

    if( widget->inherits( "QAbstractScrollArea" ) )
    { registerScrollArea( qobject_cast<QAbstractScrollArea*>(widget) ); }

    // adjust flags
    switch (widget->windowFlags() & Qt::WindowType_Mask)
    {

        case Qt::Window:
        case Qt::Dialog:
            widget->installEventFilter(this);
            widget->setAttribute(Qt::WA_StyledBackground);
            break;
        case Qt::Popup: // we currently don't want that kind of gradient on menus etc
        case Qt::Tool: // this we exclude as it is used for dragging of icons etc
        default: break;

    }

    if (
        qobject_cast<QAbstractItemView*>(widget)
        || qobject_cast<QAbstractSpinBox*>(widget)
        || qobject_cast<QCheckBox*>(widget)
        || qobject_cast<QComboBox*>(widget)
        || qobject_cast<QLineEdit*>(widget)
        || qobject_cast<QPushButton*>(widget)
        || qobject_cast<QRadioButton*>(widget)
        || qobject_cast<QScrollBar*>(widget)
        || qobject_cast<QSlider*>(widget)
        || qobject_cast<QTabBar*>(widget)
        || qobject_cast<QTextEdit*>(widget)
        || qobject_cast<QToolButton*>(widget)
        || qobject_cast<QDial*>(widget)
        || qobject_cast<QSplitterHandle*>(widget)
        )
    { widget->setAttribute(Qt::WA_Hover); }

    if( qobject_cast<QAbstractButton*>(widget) && qobject_cast<QDockWidget*>( widget->parent() ) )
    { widget->setAttribute(Qt::WA_Hover); }

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

    }

    if (qobject_cast<QMenuBar*>(widget))
    {

        widget->setBackgroundRole(QPalette::NoRole);

    } else if (widget->inherits("Q3ToolBar")
        || qobject_cast<QToolBar*>(widget)
        || qobject_cast<QToolBar *>(widget->parent())) {

        widget->setBackgroundRole(QPalette::NoRole);
        widget->setAttribute(Qt::WA_TranslucentBackground);
        widget->setContentsMargins(0,0,0,1);
        widget->installEventFilter(this);
#ifdef Q_WS_WIN
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint); //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
#endif

    } else if (qobject_cast<QScrollBar*>(widget) ) {

        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);

    } else if (qobject_cast<QDockWidget*>(widget)) {

        widget->setBackgroundRole(QPalette::NoRole);
        widget->setAttribute(Qt::WA_TranslucentBackground);
        widget->setContentsMargins(3,3,3,3);
        widget->installEventFilter(this);

    } else if (qobject_cast<QToolBox*>(widget)) {

        widget->setBackgroundRole(QPalette::NoRole);
        widget->setAutoFillBackground(false);
        widget->setContentsMargins(5,5,5,5);
        widget->installEventFilter(this);

    } else if (widget->parentWidget() && widget->parentWidget()->parentWidget() && qobject_cast<QToolBox*>(widget->parentWidget()->parentWidget()->parentWidget())) {

        widget->setBackgroundRole(QPalette::NoRole);
        widget->setAutoFillBackground(false);
        widget->parentWidget()->setAutoFillBackground(false);

    } else if (qobject_cast<QMenu*>(widget) ) {

        widget->setAttribute(Qt::WA_TranslucentBackground);
        #ifdef Q_WS_WIN
        //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
        #endif

    } else if (widget->inherits("QComboBoxPrivateContainer")) {

        widget->installEventFilter(this);
        widget->setAttribute(Qt::WA_TranslucentBackground);
#ifdef Q_WS_WIN
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint); //FramelessWindowHint is needed on windows to make WA_TranslucentBackground work properly
#endif

    } else if ( qobject_cast<QFrame*>(widget) ) {

        if (qobject_cast<KTitleWidget*>(widget->parentWidget()))
        {
            widget->setBackgroundRole( QPalette::Window );
        }

        widget->installEventFilter(this);

    }

    // base class polishing
    KStyle::polish(widget);

}

//_______________________________________________________________
void OxygenStyle::unpolish(QWidget* widget)
{

    // register widget to animations
    animations().unregisterWidget( widget );
    transitions().unregisterWidget( widget );

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

    // hover flags
    if (qobject_cast<QPushButton*>(widget)
        || qobject_cast<QComboBox*>(widget)
        || qobject_cast<QAbstractSpinBox*>(widget)
        || qobject_cast<QCheckBox*>(widget)
        || qobject_cast<QRadioButton*>(widget)
        || qobject_cast<QScrollBar*>(widget)
        || qobject_cast<QSlider*>(widget)
        || qobject_cast<QLineEdit*>(widget)
    ) { widget->setAttribute(Qt::WA_Hover, false); }

    if (qobject_cast<QMenuBar*>(widget)
        || (widget && widget->inherits("Q3ToolBar"))
        || qobject_cast<QToolBar*>(widget)
        || (widget && qobject_cast<QToolBar *>(widget->parent()))
        || qobject_cast<QToolBox*>(widget))
    {
        widget->setBackgroundRole(QPalette::Button);
        widget->removeEventFilter(this);
        widget->clearMask();
    }

    if (qobject_cast<QScrollBar*>(widget))
    {

        widget->setAttribute(Qt::WA_OpaquePaintEvent);

    } else if (qobject_cast<QDockWidget*>(widget)) {

        widget->setContentsMargins(0,0,0,0);
        widget->clearMask();

    } else if (qobject_cast<QToolBox*>(widget)) {

        widget->setBackgroundRole(QPalette::Button);
        widget->setContentsMargins(0,0,0,0);
        widget->removeEventFilter(this);

    } else if (qobject_cast<QMenu*>(widget)) {

        widget->setAttribute(Qt::WA_PaintOnScreen, false);
        widget->setAttribute(Qt::WA_NoSystemBackground, false);
        widget->clearMask();

    } else if (widget->inherits("QComboBoxPrivateContainer")) widget->removeEventFilter(this);
    else if (qobject_cast<QFrame*>(widget)) widget->removeEventFilter(this);

    KStyle::unpolish(widget);

}

//_____________________________________________________________________
void OxygenStyle::globalSettingsChange(int type, int /*arg*/)
{
    if (type == KGlobalSettings::PaletteChanged) {
        _helper.reloadConfig();
        _viewFocusBrush = KStatefulBrush( KColorScheme::View, KColorScheme::FocusColor, _sharedConfig );
        _viewHoverBrush = KStatefulBrush( KColorScheme::View, KColorScheme::HoverColor, _sharedConfig );
    }

    // need to update animated timers
    OxygenStyleConfigData::self()->readConfig();
    animations().setupEngines();
    transitions().setupEngines();

}

//__________________________________________________________________________
void OxygenStyle::renderMenuItemRect( const QStyleOption* opt, const QRect& r, const QColor& base, const QPalette& pal, QPainter* p, qreal opacity ) const
{

    if( opacity == 0 ) return;

    QPixmap pm(r.size());
    pm.fill(Qt::transparent);
    QPainter pp(&pm);
    QRect rr(QPoint(0,0), r.size());

    QColor color(base);
    if (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG)
    {

        color = pal.color(QPalette::Highlight);

    } else if (OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE) {

        color = KColorUtils::mix(color, KColorUtils::tint(color, pal.color(QPalette::Highlight), 0.6));

    } else  color = _helper.calcDarkColor( color );

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

}

//____________________________________________________________________________________
QColor OxygenStyle::slabShadowColor( QColor color, StyleOptions opts, qreal opacity, Oxygen::AnimationMode mode ) const
{

    QColor glow;
    if( mode == Oxygen::AnimationNone || opacity < 0 )
    {

        if( opts & Hover ) glow = _viewHoverBrush.brush(QPalette::Active).color();
        else if( opts & Focus ) glow = _viewFocusBrush.brush(QPalette::Active).color();
        else if( opts & SubtleShadow ) glow = _helper.alphaColor(_helper.calcShadowColor(color), 0.15 );


    } else if( mode == Oxygen::AnimationHover ) {

        // animated color, hover
        if( opts&Focus ) glow = _viewFocusBrush.brush(QPalette::Active).color();
        else if( opts&SubtleShadow ) glow = _helper.alphaColor(_helper.calcShadowColor(color), 0.15 );

        if( glow.isValid() ) glow = KColorUtils::mix( glow,  _viewHoverBrush.brush(QPalette::Active).color(), opacity );
        else glow = _helper.alphaColor(  _viewHoverBrush.brush(QPalette::Active).color(), opacity );

    } else if( mode == Oxygen::AnimationFocus ) {

        if( opts&Hover ) glow = _viewHoverBrush.brush(QPalette::Active).color();
        else if( opts&SubtleShadow ) glow = _helper.alphaColor(_helper.calcShadowColor(color), 0.15 );

        if( glow.isValid() ) glow = KColorUtils::mix( glow,  _viewFocusBrush.brush(QPalette::Active).color(), opacity );
        else glow = _helper.alphaColor(  _viewFocusBrush.brush(QPalette::Active).color(), opacity );

    }

    return glow;
}

//___________________________________________________________________________________
void OxygenStyle::renderDialSlab( QPainter *painter, QRect rect, const QColor &color, const QStyleOption *option, StyleOptions opts, qreal opacity, Oxygen::AnimationMode mode) const
{

    // cast option
    const QStyleOptionSlider* sliderOption( qstyleoption_cast<const QStyleOptionSlider*>( option ) );
    if( !sliderOption ) return;

    // calculate glow color
    QColor glow = slabShadowColor( color, opts, opacity, mode );

    // get main slab
    QPixmap pix( glow.isValid() ? _helper.dialSlabFocused( color, glow, 0.0, rect.width()) : _helper.dialSlab( color, 0.0, rect.width() ));
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

    QPointF center = rect.center();
    const int sliderWidth = qMin( 2*rect.width()/5, 18 );
    const qreal radius( 0.5*( rect.width() - 2*sliderWidth ) );
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
void OxygenStyle::renderButtonSlab(QPainter *p, QRect r, const QColor &color, StyleOptions opts, qreal opacity, Oxygen::AnimationMode mode, TileSet::Tiles tiles) const
{
    if ((r.width() <= 0) || (r.height() <= 0)) return;

    if (opts & Sunken) r.adjust(-1,0,1,2);

    // fill
    if (!(opts & NoFill))
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);

        if (_helper.calcShadowColor(color).value() > color.value() && (opts & Sunken) )
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
    if (opts & Sunken)
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
void OxygenStyle::renderSlab(QPainter *p, QRect r, const QColor &color, StyleOptions opts, qreal opacity,
    Oxygen::AnimationMode mode,
    TileSet::Tiles tiles) const
{
    if ((r.width() <= 0) || (r.height() <= 0)) return;

    if (opts & Sunken) r.adjust(-1,0,1,2);

    // fill
    if (!(opts & NoFill))
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);

        if (_helper.calcShadowColor(color).value() > color.value() && (opts & Sunken) )
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
    if (opts & Sunken)
    {
        tile = _helper.slabSunken(color, 0.0);

    } else {

        // calculate proper glow color based on current settings and opacity
        QColor glow( slabShadowColor( color, opts, opacity, mode ) );
        tile = glow.isValid() ? _helper.slabFocused(color, glow , 0.0) : _helper.slab(color, 0.0);

    }

    tile->render(r, p, tiles);

}

//____________________________________________________________________________________
void OxygenStyle::renderHole(QPainter *p, const QColor &base, const QRect &r, bool focus, bool hover, qreal opacity, Oxygen::AnimationMode animationMode,  TileSet::Tiles tiles) const
{
    if((r.width() <= 0)||(r.height() <= 0))
        return;

    if( opacity >= 0 && ( animationMode & Oxygen::AnimationFocus ) )
    {

        // calculate proper glow color based on current settings and opacity
        QColor glow = hover ?
            KColorUtils::mix( _viewHoverBrush.brush(QPalette::Active).color(), _viewFocusBrush.brush(QPalette::Active).color(), opacity ):
            _helper.alphaColor(  _viewFocusBrush.brush(QPalette::Active).color(), opacity );

        _helper.holeFocused(base, glow, 0.0)->render(r, p, tiles);

    } else if (focus) {

        _helper.holeFocused(base, _viewFocusBrush.brush(QPalette::Active).color(), 0.0)->render(r, p, tiles);

    } else if( opacity >= 0 && ( animationMode & Oxygen::AnimationHover ) ) {

        // calculate proper glow color based on current settings and opacity
        QColor glow = _helper.alphaColor(  _viewHoverBrush.brush(QPalette::Active).color(), opacity );
        _helper.holeFocused(base, glow, 0.0)->render(r, p, tiles);

    } else if (hover) {

        _helper.holeFocused(base, _viewHoverBrush.brush(QPalette::Active).color(), 0.0)->render(r, p, tiles);

    } else {

        _helper.hole(base, 0.0)->render(r, p, tiles);

    }

}

//______________________________________________________________________________
void OxygenStyle::renderScrollBarHole(QPainter *p, const QRect &r, const QColor &color,
                                   Qt::Orientation orientation, TileSet::Tiles tiles) const
{
    if (r.isValid())
    {

        // one need to make smaller shadow
        // (notably on the size when rect height is too high)
        bool smallShadow = r.height() < 10;
        _helper.scrollHole( color, orientation, smallShadow)->render(r, p, tiles);

    }

}

//______________________________________________________________________________
void OxygenStyle::renderScrollBarHandle(
    QPainter *p, const QRect &r, const QPalette &pal,
    Qt::Orientation orientation, bool hover, qreal opacity ) const
{
    if (!r.isValid()) return;
    p->save();
    p->setRenderHints(QPainter::Antialiasing);
    QColor color = pal.color(QPalette::Button);
    QColor light = _helper.calcLightColor(color);
    QColor mid = _helper.calcMidColor(color);
    QColor dark = _helper.calcDarkColor(color);
    QColor shadow = _helper.calcShadowColor(color);
    bool horizontal = orientation == Qt::Horizontal;

    // draw the hole as background
    const QRect holeRect = horizontal ? r.adjusted(-4,0,4,0) : r.adjusted(0,-3,0,4);
    renderScrollBarHole(p, holeRect,
            pal.color(QPalette::Window), orientation,
            horizontal ? TileSet::Top | TileSet::Bottom | TileSet::Center
                       : TileSet::Left | TileSet::Right | TileSet::Center);

    // draw the slider itself
    QRectF rect = r.adjusted(3, horizontal ? 2 : 4, -3, -3);
    if (!rect.isValid()) { // e.g. not enough height
        p->restore();
        return;
    }

    // draw the slider
    QColor glowColor;
    if (!OxygenStyleConfigData::scrollBarColored())
    {
        QColor base = KColorUtils::mix(dark, shadow, 0.5);
        QColor hovered = _viewHoverBrush.brush(QPalette::Active).color();

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
    if (horizontal) p->drawRoundedRect(rect.adjusted(-1.2,-0.8,1.2,0.8), 3, 3);
    else p->drawRoundedRect(rect.adjusted(-0.8,-1.2,0.8,1.2), 3, 3);

    // colored background
    p->setPen(Qt::NoPen);
    if (OxygenStyleConfigData::scrollBarColored())
    {

        if( opacity >= 0 ) p->setBrush( KColorUtils::mix( color, pal.color(QPalette::Highlight), opacity ) );
        else if( hover ) p->setBrush(  pal.color(QPalette::Highlight) );
        else p->setBrush( color );
        p->drawRoundedRect(rect, 2, 2);

    }

    // slider gradient
    {
        QLinearGradient sliderGradient( rect.topLeft(), horizontal ? rect.bottomLeft() : rect.topRight());
        if (!OxygenStyleConfigData::scrollBarColored()) {
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
        if (!OxygenStyleConfigData::scrollBarColored()) {
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

    if (OxygenStyleConfigData::scrollBarColored()) {
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

//________________________________________________________________________
void OxygenStyle::renderCheckBox(QPainter *p, const QRect &rect, const QPalette &pal,
    bool enabled, bool hasFocus, bool mouseOver, int primitive,
    bool sunken,
    qreal opacity,
    Oxygen::AnimationMode mode ) const
{
    Q_UNUSED(enabled);

    int s = qMin(rect.width(), rect.height());
    QRect r = centerRect(rect, s, s);

    StyleOptions opts;
    if (hasFocus) opts |= Focus;
    if (mouseOver) opts |= Hover;

    if(sunken) _helper.holeFlat(pal.color(QPalette::Window), 0.0)->render(r, p, TileSet::Full);
    else renderSlab(p, r, pal.color(QPalette::Button), opts, opacity, mode, TileSet::Ring );

    // check mark
    double x = r.center().x() - 3.5, y = r.center().y() - 2.5;

    if (primitive != CheckBox::CheckOff)
    {
        qreal penThickness = 2.0;

        QColor color =  (sunken) ? pal.color(QPalette::WindowText): pal.color(QPalette::ButtonText);
        QColor background =  (sunken) ? pal.color(QPalette::Window): pal.color(QPalette::Button);

        QPen pen( _helper.decoColor( background, color ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        QPen contrastPen( _helper.calcLightColor( background ), penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        pen.setCapStyle(Qt::RoundCap);
        if (primitive == CheckBox::CheckTriState)
        {
            QVector<qreal> dashes;
            if (OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_CHECK)
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
        p->setRenderHint(QPainter::Antialiasing);
        qreal offset( qMin( penThickness, qreal(1.0) ) );
        if (OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_CHECK)
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

            if (sunken)
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
void OxygenStyle::renderRadioButton(QPainter *p, const QRect &r, const QPalette &pal,
    bool enabled, bool hasFocus, bool mouseOver, int prim,
    bool drawButton, qreal opacity,
    Oxygen::AnimationMode mode ) const
{
    Q_UNUSED(enabled);

    int s = widgetLayoutProp(WT_RadioButton, RadioButton::Size);
    QRect r2(r.x() + (r.width()-s)/2, r.y() + (r.height()-s)/2, s, s);
    int x = r2.x();
    int y = r2.y();

    if( drawButton )
    {

        StyleOptions opts;
        if (hasFocus) opts |= Focus;
        if (mouseOver) opts |= Hover;
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

void OxygenStyle::renderDot(QPainter *p, const QPointF &point, const QColor &baseColor) const
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

static TileSet::Tiles tilesByShape(QTabBar::Shape shape)
{
    switch (shape) {
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
            qDebug() << "tilesByShape: unknown shape";
            return TileSet::Ring;
    }
}

//_____________________________________________________________________
void OxygenStyle::renderTab(
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

    const bool northAlignment = tabOpt->shape == QTabBar::RoundedNorth || tabOpt->shape == QTabBar::TriangularNorth;
    const bool southAlignment = tabOpt->shape == QTabBar::RoundedSouth || tabOpt->shape == QTabBar::TriangularSouth;
    const bool westAlignment = tabOpt->shape == QTabBar::RoundedWest || tabOpt->shape == QTabBar::TriangularWest;
    const bool eastAlignment = tabOpt->shape == QTabBar::RoundedEast || tabOpt->shape == QTabBar::TriangularEast;
    const bool horizontal = (northAlignment || southAlignment);

    const bool leftCornerWidget = reverseLayout ?
        (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget) :
        (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget);

    const bool rightCornerWidget = reverseLayout ?
        (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget) :
        (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget);

    const bool isFirst = pos == QStyleOptionTab::Beginning || pos == QStyleOptionTab::OnlyOneTab/* (pos == First) || (pos == Single)*/;

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

    TileSet::Tiles frameTiles = (horizontal) ?
        (northAlignment ? TileSet::Top : TileSet::Bottom):
        (westAlignment ? TileSet::Left : TileSet::Right);


    switch (OxygenStyleConfigData::tabStyle())
    {

        case OxygenStyleConfigData::TS_SINGLE:
        {
            QRect tabRect = r;

            // tabRect defines the position of the tab
            if (horizontal)
            {

                // selected tabs are taller
                if (selected)
                {

                    if (northAlignment) tabRect.adjust(0,-1,0,2);
                    else tabRect.adjust(0,-2,0,1);

                } else {

                    // deselected
                    if (northAlignment) tabRect.adjust(0,1,0,2);
                    else tabRect.adjust(0,-2,0,-1);

                }

                // reduces the space between tabs
                if (!isLeftMost) tabRect.adjust(-gw,0,0,0);
                if (!isRightMost) tabRect.adjust(0,0,gw,0);

            } else {

                // east and west tabs
                // selected tabs are taller
                if (selected)
                {

                    if (westAlignment) tabRect.adjust(-1,0,2,0);
                    else tabRect.adjust(-2,0,1,0);

                } else {

                    // deselected
                    if (westAlignment) tabRect.adjust(1,0,2,0);
                    else tabRect.adjust(-2,0,-1,0);

                }

                // reduces the space between tabs
                if( !isTopMost ) tabRect.adjust(0,-gw,0,0);
                if( !isBottomMost ) tabRect.adjust(0,0,0,gw);
            }

            // frameRect defines the part of the frame which
            // holds the content and is connected to the tab
            QRect frameRect;
            if (horizontal)
            {

                if (northAlignment) frameRect = r.adjusted(-7, r.height()-gw-7, 7, 0);
                else frameRect = r.adjusted(-7, 0, 7, -r.height()+gw+7);

            } else {

                // vertical
                if (westAlignment) frameRect = r.adjusted(r.width()-gw-7, -7, 0, 7);
                else frameRect = r.adjusted(0, -7, -r.width()+gw+7, 7);

            }

            // HACK: Workaround for misplaced tab
            if (southAlignment) {
                frameRect.translate( 0, -1 );
                if (selected) tabRect.translate( 0, -1 );
                else tabRect.adjust(0,0,0,-1);
            }

            // handle the rightmost and leftmost tabs
            // if document mode is not enabled, draw the rounded frame corner (which is visible if the tab is not selected)
            // also fill the small gap between that corner and the actial frame
            if (horizontal)
            {

                if ((isLeftMost && !reverseLayout) || (isRightMost && reverseLayout))
                {

                    if (!reverseLayout)
                    {
                        if( isFrameAligned )
                        {
                            if (!selected)
                            {
                                frameRect.adjust(-gw+7,0,0,0);
                                frameTiles |= TileSet::Left;
                            }
                            if (northAlignment) renderSlab(p, QRect(r.x()-gw, r.bottom()-11, 2, 18), color, NoFill, TileSet::Left);
                            else if( selected ) renderSlab(p, QRect(r.x()-gw, r.top()-6, 2, 17), color, NoFill, TileSet::Left);
                            else renderSlab(p, QRect(r.x()-gw, r.top()-6, 2, 18), color, NoFill, TileSet::Left);
                        }
                        tabRect.adjust(-gw,0,0,0);

                    } else {

                        // reverseLayout
                        if( isFrameAligned )
                        {
                            if (!selected)
                            {
                                frameRect.adjust(0,0,gw-7,0);
                                frameTiles |= TileSet::Right;
                            }

                            if (northAlignment) renderSlab(p, QRect(r.right(), r.bottom()-11, 2, 18), color, NoFill, TileSet::Right);
                            else if( selected ) renderSlab(p, QRect(r.right(), r.top()-6, 2, 17), color, NoFill, TileSet::Right);
                            else renderSlab(p, QRect(r.right(), r.top()-6, 2, 18), color, NoFill, TileSet::Right);

                        }
                        tabRect.adjust(0,0,gw,0);
                    }
                }

            } else {

                // vertical
                if( isTopMost && isFrameAligned )
                {

                    if (!selected)
                    {
                        frameRect.adjust(0,-gw+7,0,0);
                        frameTiles |= TileSet::Top;
                    }

                    if (westAlignment) renderSlab(p, QRect(r.right()-11, r.y()-gw, 18, 2), color, NoFill, TileSet::Top);
                    else renderSlab(p, QRect(r.x()-11, r.y()-gw, 23, 2), color, NoFill, TileSet::Top);

                }
                tabRect.adjust(0,-gw,0,0);
            }

            p->save();

            // draw the remaining parts of the frame
            if (!selected) {

                renderSlab(p, frameRect, color, NoFill, frameTiles);

            } else {

                // when selected only draw parts of the frame to appear connected to the content
                QRegion clipRegion;
                if (horizontal && !(isLeftMost && !reverseLayout))
                {
                    QRegion frameRegionLeft = QRegion(QRect(frameRect.x(), frameRect.y(), tabRect.x()-frameRect.x() + 2, frameRect.height()));
                    clipRegion += frameRegionLeft;
                }

                if (horizontal && !(isRightMost && reverseLayout))
                {
                    QRegion frameRegionRight = QRegion(QRect(tabRect.right() - gw, frameRect.y(), frameRect.right()-tabRect.right(), frameRect.height()));
                    clipRegion += frameRegionRight;
                }

                if (!horizontal && !isTopMost)
                {
                    QRegion frameRegionTop = QRegion(QRect(frameRect.x(), frameRect.y(), frameRect.width(), tabRect.y() - frameRect.y() + 3));
                    clipRegion += frameRegionTop;
                }

                if (!horizontal /* && !isBottomMost */)
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
                if (northAlignment) {

                    // don't draw the connection for a frame aligned tab
                    // except for RTL-layout
                    if (!isFrameAligned || reverseLayout) renderSlab( p, QRect(tabRect.x() - 6, tabRect.bottom()-9, 17, 7), color, NoFill, TileSet::Top );
                    if (!isFrameAligned || !reverseLayout) renderSlab( p, QRect( tabRect.right()-10, tabRect.bottom()-9, 17, 7), color, NoFill, TileSet::Top );

                } else if (southAlignment) {

                    if (!isFrameAligned || reverseLayout)
                    {
                        if( isLeftMost && !reverseLayout ) renderSlab( p, QRect( tabRect.x()-6, tabRect.y()+3, 17, 7), color, NoFill, TileSet::Bottom );
                        else renderSlab( p, QRect( tabRect.x()-5, tabRect.y()+3, 16, 7), color, NoFill, TileSet::Bottom );
                    }

                    if (!isFrameAligned || !reverseLayout)
                    {
                        if( isRightMost && reverseLayout ) renderSlab( p, QRect( tabRect.right()-10, tabRect.y()+3, 17, 7), color, NoFill, TileSet::Bottom );
                        else renderSlab( p, QRect( tabRect.right()-10, tabRect.y()+3, 16, 7), color, NoFill, TileSet::Bottom );
                    }

                } else if (eastAlignment) {

                    if (!isFrameAligned) renderSlab( p, QRect( tabRect.x() + 3, tabRect.y() - 7, 7, 18 ), color, NoFill, TileSet::Right );
                    renderSlab( p, QRect( tabRect.x() + 3, tabRect.bottom()-10, 7, 16 ), color, NoFill, TileSet::Right );

                } else {

                    // west aligned
                    if (!isFrameAligned) renderSlab( p, QRect( tabRect.right()-9, tabRect.y()-7, 7, 18 ), color, NoFill, TileSet::Left );
                    renderSlab( p, QRect( tabRect.right()-9, tabRect.bottom()-10, 7, 16 ), color, NoFill, TileSet::Left );

                }
            }

            p->setClipRect(r);

            // get timeLine
            if( !selected && enabled && animations().tabBarEngine().isAnimated( widget, r.topLeft() ) )
            {

                renderSlab(p, tabRect, color, mouseOver ? hoverTabOpts : deselectedTabOpts,
                    animations().tabBarEngine().opacity( widget, r.topLeft() ), Oxygen::AnimationHover,
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
                _helper.renderWindowBackground(p, fillRect, widget,widget->window()->palette());
            }

            fillTab(p, tabRect, color, orientation, selected, inverted );

            p->restore();
            return;

        } // OxygenStyleConfigData::TS_SINGLE

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
                        _helper.renderWindowBackground(p, fillRect, widget,widget->window()->palette());
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
                            path.moveTo(x+2.5, y+h-2-( isFrameAligned ? 0 : 2 ));
                            path.lineTo(x+2.5, y+2.5); // left border
                            path.arcTo(QRectF(x+2.5, y+0.5, 9, 9), 180, -90); // top-left corner
                            path.lineTo(QPointF( x + w - 1.5 - 4.5, y+0.5)); // top border
                            path.arcTo( QRectF( x+w - 1.5 - 9, y+0.5, 9, 9 ), 90, -90 );
                            path.lineTo(QPointF( x+w - 1.5, y+h-2-( isFrameAligned ? 0 : 2 ) )); // to complete the path.
                            p->drawPath(path);

                        } else if( isLeftMost ) {

                            QPainterPath path;
                            x-=gw;
                            w+=gw;
                            path.moveTo(x+2.5, y+h-2-( isFrameAligned ? 0 : 2 ));
                            path.lineTo(x+2.5, y+2.5); // left border
                            path.arcTo(QRectF(x+2.5, y+0.5, 9, 9), 180, -90); // top-left corner
                            path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+0.5)); // top border
                            path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+h-4)); // to complete the path.
                            p->drawPath(path);

                        } else if( isRightMost ) {

                            QPainterPath path;
                            w+=gw;
                            path.moveTo(x+w-2.5, y+h-2- ( isFrameAligned ? 0 : 2 ) );
                            path.lineTo(x+w-2.5, y+2.5); // right border
                            path.arcTo(QRectF(x+w-9-2.5, y+0.5, 9, 9), 0, 90); // top-right corner
                            path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+0.5)); // top border
                            path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+h-4)); // to complete the path.
                            p->drawPath(path);

                        } else {

                            // top border
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
                            path.lineTo(x+2.5, y+h-2.5); // left border
                            path.arcTo(QRectF(x+2.5, y+h-9.5, 9, 9), 180, 90); // bottom-left corner
                            path.lineTo(QPointF(x+w - 1.5 -4.5, y+h-0.5)); // bottom border
                            path.arcTo( QRectF( x+w - 1.5 - 9, y+h-0.5 - 9, 9, 9 ), -90, 90 );
                            path.lineTo(QPointF(x+w-1.5, y+4 ) ); // to complete the path
                            p->drawPath(path);

                        } else if(isLeftMost) {

                            QPainterPath path;
                            x-=gw;
                            w+=gw;
                            path.moveTo(x+2.5, y+2+( isFrameAligned ? 0 : 2));
                            path.lineTo(x+2.5, y+h-2.5); // left border
                            path.arcTo(QRectF(x+2.5, y+h-9.5, 9, 9), 180, 90); // bottom-left corner
                            path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+h-0.5)); // bottom border
                            path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+4)); // to complete the path.
                            p->drawPath(path);

                        } else if(isRightMost) {

                            QPainterPath path;
                            w+=gw;
                            path.moveTo(x+w-2.5, y+2+( isFrameAligned ? 0 : 2));
                            path.lineTo(x+w-2.5, y+h-2.5); // right border
                            path.arcTo(QRectF(x+w-9-2.5, y+h-9.5, 9, 9), 0, -90); // bottom-right corner
                            path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+h-0.5)); // bottom border
                            path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+4)); // to complete the path.
                            p->drawPath(path);

                        } else {

                            // bottom border
                            p->drawLine(QPointF(x-(isRightOfSelected?2:0), y+h-0.5), QPointF(x+w+(isRightOfSelected ?2:0)+(isLeftOfSelected ?2:0), y+h-0.5));
                            if(!isLeftOfSelected) p->drawLine(QPointF(x+w+0.5, y+2.5), QPointF(x+w+0.5, y+h-4));
                            p->fillRect(x, y+2, w+(isLeftOfSelected ?2:0), h-2, midColor);

                        }
                    }
                    p->restore();

                    // bottom line
                    TileSet::Tiles tiles = southAlignment?TileSet::Bottom:TileSet::Top;
                    QRect Ractual(Rb.left(), Rb.y(), Rb.width(), 6);

                    if(isLeftMost)
                    {

                        if( isFrameAligned ) tiles |= TileSet::Left;
                        if( reverseLayout || documentMode || !isFrameAligned )
                        { Ractual.adjust( -6, 0, 0, 0); }

                    } else if( isRightOfSelected ) Ractual.adjust(-10+gw,0,0,0);
                    else Ractual.adjust(-7+gw,0,0,0);

                    if(isRightMost)
                    {

                        if( isFrameAligned ) tiles |= TileSet::Right;
                        else Ractual.adjust(0,0,6,0);

                    } else if( isLeftOfSelected ) Ractual.adjust(0,0,10-gw,0);
                    else Ractual.adjust(0,0,7-gw,0);

                    if( animations().tabBarEngine().isAnimated( widget, r.topLeft() ) )
                    {

                        renderSlab(p, Ractual, color, NoFill| Hover,
                            animations().tabBarEngine().opacity( widget, r.topLeft() ),
                            Oxygen::AnimationHover,
                            tiles );

                    } else if (mouseOver) renderSlab(p, Ractual, color, NoFill| Hover, tiles);
                    else renderSlab(p, Ractual, color, NoFill, tiles);

                }

            } else {

                // westAlignment and eastAlignment
                // the tab part of the tab - ie subtracted the fairing to the frame
                QRect Rc = eastAlignment ? r.adjusted(7+gw,-gw,gw,gw) : r.adjusted(-gw,-gw,-7-gw,gw);

                // the area where the fairing should appear
                const QRect Rb(eastAlignment ? r.x()+gw: Rc.right()+1, Rc.top(), r.width()-Rc.width(), Rc.height() );

                if (selected)
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
                        _helper.renderWindowBackground(p, fillRect, widget,widget->window()->palette());
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

                    if (westAlignment)
                    {
                        // west alignment
                        r.adjusted(5-gw,0,-5-gw,0).getRect(&x, &y, &w, &h);

                        if( isLeftMost && isRightMost ) {

                            // at top
                            QPainterPath path;
                            y = y + 1.5;

                            path.moveTo(x+w+0.5 + ( isFrameAligned ? 2 : 0), y+0.5 );
                            path.lineTo(x+5.0, y+0.5); // top border
                            path.arcTo(QRectF(x+0.5, y+0.5, 9.5, 9.5), 90, 90); // top-left corner
                            path.lineTo(x+0.5, y+h-2.5-4.5); // left border
                            path.arcTo( QRectF( x+0.5, y+h-2.5-9, 9, 9 ), 180, 90 );
                            path.lineTo(x+w+( 0.5 ), y+h-2.5); // complete the path
                            p->drawPath(path);

                        } else if( isLeftMost ) {

                            // at top
                            QPainterPath path;
                            y += 1.5;

                            path.moveTo(x+w+0.5 + ( isFrameAligned ? 2 : 0), y+0.5);
                            path.lineTo(x+5.0, y+0.5); // top border
                            path.arcTo(QRectF(x+0.5, y+0.5, 9.5, 9.5), 90, 90); // top-left corner
                            path.lineTo(x+0.5, y+h+0.5); // left border
                            path.lineTo(x+w+1.0, y+h+0.5); // complete the path
                            p->drawPath(path);

                        } else if( isRightMost ) {

                            // at bottom
                            QPainterPath path;

                            path.moveTo(x+w+0.5, y+h-0.5);
                            path.lineTo(x+5.0, y+h-0.5); // bottom border
                            path.arcTo(QRectF(x+0.5, y+h-0.5-9.5, 9.5, 9.5), 270, -90); // bottom-left corner
                            path.lineTo(x+0.5, y-0.5); // left border
                            path.lineTo(x+w+0.5, y-0.5); // complete the path
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
                            path.lineTo(x+w-5.0, y+0.5); // top line
                            path.arcTo(QRectF(x+w-0.5-9.5, y+0.5, 9.5, 9.5), 90, -90); // top-right corner
                            path.lineTo(x+w-0.5, y+h-2.5 -4.5 ); // right line
                            path.arcTo( QRectF( x+w-0.5-9, y+h-2.5-9, 9, 9 ), 0, -90 );
                            path.lineTo(x-0.5, y+h-2.5); // complete path
                            p->drawPath(path);

                        } else if (isLeftMost) {

                            // at top
                            QPainterPath path;
                            y = y + 1.5;

                            path.moveTo(x-0.5 - ( isFrameAligned ? 2:0 ), y+0.5 );
                            path.lineTo(x+w-5.0, y+0.5); // top line
                            path.arcTo(QRectF(x+w-0.5-9.5, y+0.5, 9.5, 9.5), 90, -90); // top-right corner
                            path.lineTo(x+w-0.5, y+h+0.5); // right line
                            path.lineTo(x-0.5, y+h+0.5); // complete path
                            p->drawPath(path);

                        } else if( isRightMost ) {

                            // at bottom
                            QPainterPath path;

                            path.moveTo(x-0.5, y+h-0.5);
                            path.lineTo(x+w-5.0, y+h-0.5); // bottom line
                            path.arcTo(QRectF(x+w-0.5-9.5, y+h-0.5-9.5, 9.5, 9.5), -90, 90); // bottom-right corner
                            path.lineTo(x+w-0.5, y-0.5); // right line
                            path.lineTo(x-0.5, y-0.5); // complete path
                            p->drawPath(path);

                        } else {

                            // right line
                            p->drawLine(QPointF(x+w-0.5, y - (isRightOfSelected ? 2:0) ), QPointF(x+w-0.5, y+h-0.5 + (isLeftOfSelected ? 2:0)));
                            if( !isLeftOfSelected ) p->drawLine(QPointF(x+0.5, y+h-0.5), QPointF(x+w-1.5, y+h-0.5));
                            p->fillRect(x, y - (isRightOfSelected ? 2:0), w, h + (isRightOfSelected ? 2:0) + (isLeftOfSelected ? 2:0), midColor);

                        }
                    }
                    p->restore();

                    TileSet::Tiles tiles = eastAlignment ? TileSet::Right : TileSet::Left;
                    QRect Ractual(Rb.left(), Rb.y(), 7, Rb.height());

                    if(isLeftMost)
                    {

                        // at top
                        if( isFrameAligned ) tiles |= TileSet::Top;
                        else {
                            renderSlab(p, QRect(Ractual.left(), Ractual.y()-7, Ractual.width(), 2+14), color, NoFill, tiles);
                            Ractual.adjust(0,-5,0,0);
                        }

                    } else if( isRightOfSelected ) Ractual.adjust(0,-10+gw,0,0);
                    else  Ractual.adjust(0,-7+gw,0,0);

                    if(isRightMost)
                    {

                        // at bottom
                        if( isFrameAligned && !reverseLayout) tiles |= TileSet::Top;
                        Ractual.adjust(0,0,0,7);

                    } else if( isLeftOfSelected )  Ractual.adjust(0,0,0,10-gw);
                    else Ractual.adjust(0,0,0,7-gw);

                    if( animations().tabBarEngine().isAnimated( widget, r.topLeft() ) ) {

                        renderSlab(p, Ractual, color, NoFill| Hover,
                            animations().tabBarEngine().opacity( widget, r.topLeft() ),
                            Oxygen::AnimationHover,
                            tiles );

                    } else if (mouseOver) renderSlab(p, Ractual, color, NoFill| Hover, tiles);
                    else renderSlab(p, Ractual, color, NoFill, tiles);

                }

            }
        } // OxygenStyleConfigData::TS_PLAIN
    }
}

//______________________________________________________________________________________________________________________________
void OxygenStyle::fillTab(QPainter *p, const QRect &r, const QColor &color, Qt::Orientation orientation, bool active, bool inverted) const
{
    QColor dark = _helper.calcDarkColor(color);
    QColor shadow = _helper.calcShadowColor(color);
    QColor light = _helper.calcLightColor(color);
    QColor hl = _viewFocusBrush.brush(QPalette::Active).color();

    QRect fillRect = r.adjusted(4,(orientation == Qt::Horizontal && !inverted) ? 3 : 4,-4,-4);

    QLinearGradient highlight;
    if (orientation == Qt::Horizontal)
    {

        if (!inverted)
        {

            highlight = QLinearGradient(fillRect.topLeft(), fillRect.bottomLeft());

        } else {

            // inverted
            highlight = QLinearGradient(fillRect.bottomLeft(), fillRect.topLeft());

        }

    } else {

        // vertical tab fill
        if (!inverted)
        {

            highlight = QLinearGradient(fillRect.topLeft(), fillRect.topRight());

        } else {

            // inverted
            highlight = QLinearGradient(fillRect.topRight(), fillRect.topLeft());

        }
    }

    if (active) {

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
int OxygenStyle::styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget, QStyleHintReturn * returnData) const
{
    switch (hint) {

        case SH_ComboBox_ListMouseTracking:
            return true;
        case SH_Menu_SubMenuPopupDelay:
            return 96; // Motif-like delay...

        case SH_ScrollView_FrameOnlyAroundContents:
            return true;

        case SH_ItemView_ShowDecorationSelected:
            return false;

        case SH_RubberBand_Mask:
        {
            const QStyleOptionRubberBand *opt = qstyleoption_cast<const QStyleOptionRubberBand *>(option);
            if (!opt) return false;
            if (QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask*>(returnData))
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

        case SH_WindowFrame_Mask:
        {

            const QStyleOptionTitleBar *opt = qstyleoption_cast<const QStyleOptionTitleBar *>(option);
            if (!opt) return true;
            if (QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData)) {
                if (opt->titleBarState & Qt::WindowMaximized) mask->region = option->rect;
                else mask->region = _helper.roundedMask( option->rect );
            }
            return true;

        }

        case SH_Menu_Mask:
        {

            // mask should be returned only if composite in disabled
            if( !compositingActive() )
            {
                if (QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask *>(returnData))
                { mask->region = _helper.roundedMask( option->rect ); }
                return true;

            } else {

                return KStyle::styleHint(hint, option, widget, returnData);

            }
        }

        case SH_ItemView_ArrowKeysNavigateIntoChildren:
        return true;

        default: return KStyle::styleHint(hint, option, widget, returnData);
    }
}

//______________________________________________________________________________________________________________________________
int OxygenStyle::pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const
{
    switch(m) {
        case PM_DefaultTopLevelMargin: return 11;
        case PM_DefaultChildMargin: return 4; // qcommon is 9;
        case PM_DefaultLayoutSpacing: return 4; // qcommon is 6
        case PM_ButtonMargin: return 5;

        case PM_DefaultFrameWidth:
        if (qobject_cast<const QLineEdit*>(widget)) return 4;
        if (qobject_cast<const QFrame*>(widget) ||  qobject_cast<const QComboBox*>(widget)) return 3;
        //else fall through

        // spacing between widget and scrollbars
        case PM_ScrollView_ScrollBarSpacing: return 1;

        default: return KStyle::pixelMetric(m,opt,widget);
    }
}

//______________________________________________________________________________________________________________________________
QSize OxygenStyle::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const
{
    switch(type)
    {
        case CT_GroupBox:
        {
            // adjust groupbox width to bold label font
            if (const QStyleOptionGroupBox* gbOpt = qstyleoption_cast<const QStyleOptionGroupBox*>(option))
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

            if (const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option))
            {

                if ((!tbOpt->icon.isNull()) && (!tbOpt->text.isEmpty()) && tbOpt->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
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
            if (const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option))
            {

                if (tbOpt->features & QStyleOptionToolButton::MenuButtonPopup)
                {

                    menuAreaWidth = pixelMetric(QStyle::PM_MenuButtonIndicator, option, widget);

                } else if (tbOpt->features & QStyleOptionToolButton::HasMenu) {

                    size.setWidth(size.width() + widgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorSize, tbOpt, widget));

                }

            }
            size.setWidth(size.width() - menuAreaWidth);
            if (size.width() < size.height())
                size.setWidth(size.height());
            size.setWidth(size.width() + menuAreaWidth);

            const QToolButton* t=qobject_cast<const QToolButton*>(widget);
            if (t && t->autoRaise()==true)
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
QRect OxygenStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    QRect r = option->rect;

    switch (control)
    {
        case CC_GroupBox:
        {
            const QStyleOptionGroupBox *gbOpt = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
            if (!gbOpt) break;

            bool isFlat = gbOpt->features & QStyleOptionFrameV2::Flat;

            switch (subControl)
            {

                case SC_GroupBoxFrame: return r;

                case SC_GroupBoxContents:
                {
                    int th = gbOpt->fontMetrics.height() + 8;
                    QRect cr = subElementRect(SE_CheckBoxIndicator, option, widget);
                    int fw = widgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, option, widget);

                    bool checkable = gbOpt->subControls & QStyle::SC_GroupBoxCheckBox;
                    bool emptyText = gbOpt->text.isEmpty();

                    r.adjust(fw, fw, -fw, -fw);
                    if (checkable && !emptyText) r.adjust(0, qMax(th, cr.height()), 0, 0);
                    else if (checkable) r.adjust(0, cr.height(), 0, 0);
                    else if (!emptyText) r.adjust(0, th, 0, 0);

                    // add additional indentation to flat group boxes
                    if (isFlat)
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

                    // translate down by 8 pixels in non flat mode,
                    // to avoid collision with groupbox frame
                    if( !isFlat ) r.moveTop(8);

                    QRect cr;
                    if(gbOpt->subControls & QStyle::SC_GroupBoxCheckBox)
                    {
                        cr = subElementRect(SE_CheckBoxIndicator, option, widget);
                        QRect gcr((gbOpt->rect.width() - tw -cr.width())/2 , (h-cr.height())/2+r.y(), cr.width(), cr.height());
                        if(subControl == SC_GroupBoxCheckBox)
                        {
                            if (!isFlat) return visualRect(option->direction, option->rect, gcr);
                            else return visualRect(option->direction, option->rect, QRect(0,0,cr.width(),cr.height()));
                        }
                    }

                    // left align labels in flat group boxes, center align labels in framed group boxes
                    if (isFlat) r = QRect(cr.width(),r.y(),tw,r.height());
                    else r = QRect((gbOpt->rect.width() - tw - cr.width())/2 + cr.width(), r.y(), tw, r.height());

                    return visualRect(option->direction, option->rect, r);
                }

                default: break;

            }
            break;
        }
        case CC_ComboBox:
        // add the same width as we do in eventFilter
        if(subControl == SC_ComboBoxListBoxPopup)
        return r.adjusted(0,0,8,0);

        default:
        break;
    }

    return KStyle::subControlRect(control, option, subControl, widget);
}

//______________________________________________________________________________________________________________________________
QRect OxygenStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;

    switch (sr)
    {

        case SE_TabBarTabText:
        {

            // bypass KStyle entirely because it makes it completely impossible
            // to handle both KDE and Qt applications at the same time
            int voffset(0);
            if( const QStyleOptionTabV3* tov3 = qstyleoption_cast<const QStyleOptionTabV3*>(opt) )
            {

                switch( tov3->shape )
                {

                    case QTabBar::RoundedEast:
                    case QTabBar::TriangularEast:
                    case QTabBar::RoundedWest:
                    case QTabBar::TriangularWest:
                    case QTabBar::RoundedNorth:
                    case QTabBar::TriangularNorth:
                    voffset = ( opt->state & State_Selected ) ? -1:0;
                    break;

                    case QTabBar::RoundedSouth:
                    case QTabBar::TriangularSouth:
                    voffset = ( opt->state & State_Selected ) ? 1:0;
                    break;

                    default: break;

                }

            }

            return  QCommonStyle::subElementRect(sr, opt, widget).adjusted( 6, 0, -6, 0 ).translated( 0, voffset );

        }

        case SE_TabWidgetTabContents:
        {
            QRect r = KStyle::subElementRect(sr, opt, widget);
            if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt))
            {
                bool tabBarVisible( !twf->tabBarSize.isEmpty() );
                switch (twf->shape)
                {
                    case QTabBar::RoundedNorth:
                    case QTabBar::TriangularNorth:
                    if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( 0, -3, 0, 0 );
                    break;

                    case QTabBar::RoundedSouth:
                    case QTabBar::TriangularSouth:
                    if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( 0, 0, 0, 2 );
                    else if( tabBarVisible ) r.adjust( 0, 0, 0, -1 );
                    break;

                    case QTabBar::RoundedEast:
                    case QTabBar::TriangularEast:
                    if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( 0, 0, 3, 0 );
                    break;

                    case QTabBar::RoundedWest:
                    case QTabBar::TriangularWest:
                    if( twf->lineWidth == 0 && tabBarVisible ) r.adjust( -3, 0, 0, 0 );
                    break;

                    default:
                    break;

                }

            }

            return r;
        }

        case SE_TabWidgetTabPane:
        {
            if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt))
            {
                QStyleOptionTab tabopt;
                tabopt.shape = twf->shape;
                int overlap = pixelMetric(PM_TabBarBaseOverlap, &tabopt, widget);

                switch (twf->shape)
                {
                    case QTabBar::RoundedNorth:
                    case QTabBar::TriangularNorth:

                    // this line is what causes the differences between drawing corner widgets in KStyle and drawing them in Qt
                    // TODO: identify where the lineWidth difference come from
                    if (twf->lineWidth == 0) overlap -= 1;

                    r = QRect(QPoint(0,qMax(twf->tabBarSize.height() - overlap, 0)),
                    QSize(twf->rect.width(), qMin(twf->rect.height() - twf->tabBarSize.height() + overlap, twf->rect.height())));
                    break;

                    case QTabBar::RoundedSouth:
                    case QTabBar::TriangularSouth:
                    r = QRect(QPoint(0,0), QSize(twf->rect.width(), qMin(twf->rect.height() - twf->tabBarSize.height() + overlap, twf->rect.height())));
                    break;

                    case QTabBar::RoundedEast:
                    case QTabBar::TriangularEast:
                    r = QRect(QPoint(0, 0), QSize(qMin(twf->rect.width() - twf->tabBarSize.width() + overlap, twf->rect.width()), twf->rect.height()));
                    break;

                    case QTabBar::RoundedWest:
                    case QTabBar::TriangularWest:
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

            switch (twf->shape)
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                {
                    r.setWidth(qMin(r.width(), twf->rect.width()
                        - twf->leftCornerWidgetSize.width()
                        - twf->rightCornerWidgetSize.width()));
                    r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(), 0));
                    r = visualRect(twf->direction, twf->rect, r);
                    break;
                }

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                {
                    r.setWidth(qMin(r.width(), twf->rect.width()
                        - twf->leftCornerWidgetSize.width()
                        - twf->rightCornerWidgetSize.width()));
                    r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(),
                        twf->rect.height() - twf->tabBarSize.height()));
                    r = visualRect(twf->direction, twf->rect, r);
                    break;
                }

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
                {
                    r.setHeight(qMin(r.height(), twf->rect.height()
                        - twf->leftCornerWidgetSize.height()
                        - twf->rightCornerWidgetSize.height()));
                    r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width(),
                        twf->leftCornerWidgetSize.height()));
                    break;
                }

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
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
            switch (twf->shape)
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                r = QRect(QPoint(paneRect.x(), paneRect.y() - twf->leftCornerWidgetSize.height() + (tb && tb->documentMode() ? 0 : gw)), twf->leftCornerWidgetSize);
                r = visualRect(twf->direction, twf->rect, r);
                r.translate( 0, 2 );
                break;

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                r = QRect(QPoint(paneRect.x(), paneRect.height() ), twf->leftCornerWidgetSize);
                r = visualRect(twf->direction, twf->rect, r);
                r.translate( 0, -4 );
                break;

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                r = QRect(QPoint(paneRect.x() - twf->leftCornerWidgetSize.width(), paneRect.y()), twf->leftCornerWidgetSize);
                r.translate( 4, 0 );
                break;

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
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
            switch (twf->shape)
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.y() - twf->rightCornerWidgetSize.height() + (tb && tb->documentMode() ? 0 : gw)), twf->rightCornerWidgetSize);
                r = visualRect(twf->direction, twf->rect, r);
                r.translate( 0, 2 );
                break;

                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.height()), twf->rightCornerWidgetSize);
                r = visualRect(twf->direction, twf->rect, r);
                r.translate( 0, -4 );
                break;

                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                r = QRect(QPoint(paneRect.x() - twf->rightCornerWidgetSize.width(), paneRect.y() + paneRect.height() - twf->rightCornerWidgetSize.height()), twf->rightCornerWidgetSize);
                r.translate( 4, 0 );
                break;

                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
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

            switch (option->shape)
            {
                case QTabBar::RoundedNorth:
                case QTabBar::TriangularNorth:
                case QTabBar::RoundedSouth:
                case QTabBar::TriangularSouth:
                r.setRect(option->rect.left(), option->rect.top(), 8, option->rect.height());
                break;
                case QTabBar::RoundedWest:
                case QTabBar::TriangularWest:
                case QTabBar::RoundedEast:
                case QTabBar::TriangularEast:
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

void OxygenStyle::renderWindowIcon(QPainter *p, const QRectF &r, int &type) const
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
bool OxygenStyle::eventFilter(QObject *obj, QEvent *ev)
{
    if (KStyle::eventFilter(obj, ev) ) return true;

    // toolbars
    if (QToolBar *t = qobject_cast<QToolBar*>(obj))
    {
        switch(ev->type())
        {
            case QEvent::Show:
            case QEvent::Resize:
            {
                // make sure mask appropriate
                if( !compositingActive() )
                {

                    QRegion mask( _helper.roundedMask( t->rect() ) );
                    if(t->mask() != mask) t->setMask(mask);

                } else if( t->mask() != QRegion() ) {

                    t->clearMask();

                }

                return false;
            }

            case QEvent::Paint:
            {

                // default painting when not floating
                if( !t->isFloating() ) return false;

                QPainter p(t);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());

                QRect r = t->rect();
                QColor color = t->palette().window().color();

                if( compositingActive() )
                {
                    TileSet *tileSet( _helper.roundCorner(color) );
                    tileSet->render( r, &p );
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
                if( compositingActive() ) p.setClipping( false );
                _helper.drawFloatFrame( &p, r, color );

                return true;

            }
            default: return false;
        }
    }

    // combobox container
    QWidget *widget = static_cast<QWidget*>(obj);
    if (widget->inherits("QComboBoxPrivateContainer"))
    {
        switch(ev->type())
        {
            case QEvent::Show:
            case QEvent::Resize:
            {

                // make sure mask is appropriate
                if( compositingActive() )
                {
                    if( widget->mask() != QRegion() )
                    { widget->clearMask(); }

                } else if( widget->mask() == QRegion() ) {

                    widget->setMask( _helper.roundedMask( widget->rect() ) );

                }

                return false;
            }

            case QEvent::Paint:
            {

                QPainter p(widget);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());

                QRect r = widget->rect();
                QColor color = widget->palette().window().color();

                if( compositingActive() )
                {
                    TileSet *tileSet( _helper.roundCorner(color) );
                    tileSet->render( r, &p );

                    // set clip region
                    p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );

                }

                // background
                int splitY = qMin(200, 3*r.height()/4);

                QRect upperRect = QRect(0, 0, r.width(), splitY);
                QPixmap tile = _helper.verticalGradient(color, splitY);
                p.drawTiledPixmap(upperRect, tile);

                QRect lowerRect = QRect(0,splitY, r.width(), r.height() - splitY);
                p.fillRect(lowerRect, _helper.backgroundBottomColor(color));

                // frame
                if( compositingActive() ) p.setClipping( false );
                _helper.drawFloatFrame( &p, r, color );
                return false;

            }
            default: return false;
        }

    }

    // window painting
    if (widget->isWindow() && widget->isVisible())
    {
        if (ev->type() == QEvent::Paint)
        {
            QBrush brush = widget->palette().brush(widget->backgroundRole());

            // don't use our background if the app requested something else,
            // e.g. a pixmap
            // TODO - draw our light effects over an arbitrary fill?
            if (brush.style() == Qt::SolidPattern) {}

            if(widget->testAttribute(Qt::WA_StyledBackground) && !widget->testAttribute(Qt::WA_NoSystemBackground))
            {
                QPainter p(widget);
                _helper.renderWindowBackground(&p, widget->rect(), widget,widget->window()->palette());
            }
        }
    }

    // dock widgets
    if (QDockWidget*dw = qobject_cast<QDockWidget*>(obj))
    {
        switch( ev->type() )
        {
            case QEvent::Show:
            case QEvent::Resize:
            {

                if( dw->isFloating() && !compositingActive() )
                {

                    QRegion mask( _helper.roundedMask( dw->rect() ) );
                    if(dw->mask() != mask) dw->setMask( mask );

                } else if (dw->mask() != QRegion()) {
                    // remove the mask in docked state to prevent it
                    // from intefering with the dock frame
                    dw->clearMask();
                }
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
                    if( compositingActive() )
                    {
                        TileSet *tileSet( _helper.roundCorner(color) );
                        tileSet->render( r, &p );

                        // set clip region
                        p.setClipRegion( _helper.roundedRegion( r.adjusted( 1, 1, -1, -1 ) ), Qt::IntersectClip );
                    }
#endif

                    _helper.renderWindowBackground(&p, r, dw, color);

#ifndef Q_WS_WIN
                    if( compositingActive() ) p.setClipping( false );
#endif

                    _helper.drawFloatFrame(&p, r, color);

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

    // toolboxes
    if (QToolBox *tb = qobject_cast<QToolBox*>(obj))
    {
        if (ev->type() == QEvent::Paint)
        {
            if(tb->frameShape() != QFrame::NoFrame) {
                QRect r = tb->rect();
                StyleOptions opts = NoFill;

                QPainter p(tb);
                p.setClipRegion(((QPaintEvent*)ev)->region());
                renderSlab(&p, r, tb->palette().color(QPalette::Button), opts);
            }
        }
        return false;
    }

    // frames
    // style HLines/VLines here, as Qt doesn't make them stylable as primitives.
    // Qt bug is filed.
    if (QFrame *f = qobject_cast<QFrame*>(obj))
    {
        if (ev->type() == QEvent::Paint) {
            if (qobject_cast<KTitleWidget*>(f->parentWidget())) {
                QPainter p(f);
                _helper.renderWindowBackground(&p, f->rect(), f, f->window()->palette());
            } else {
                QRect r = f->rect();
                QPainter p(f);
                p.setClipRegion(((QPaintEvent*)ev)->region());
                p.setClipping(false);
                Qt::Orientation o;
                switch(f->frameShape())
                {
                    case QFrame::HLine: { o = Qt::Horizontal; break; }
                    case QFrame::VLine: { o = Qt::Vertical; break; }
                    default: { return false; }
                }
                _helper.drawSeparator(&p, r, f->palette().color(QPalette::Window), o);
                return true;
            }
        }
        return false;
    }

    return false;
}

//____________________________________________________________________
bool OxygenStyle::compositingActive( void ) const
{
    return KWindowSystem::compositingActive();
}

//____________________________________________________________________
const QWidget* OxygenStyle::checkAutoFillBackground( const QWidget* w ) const
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
QIcon OxygenStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
                                               const QWidget *widget) const
{
    // get button color (unfortunately option and widget might not be set)
    QColor buttonColor;
    QColor iconColor;
    if (option) {
        buttonColor = option->palette.window().color();
        iconColor   = option->palette.windowText().color();
    } else if (widget) {
        buttonColor = widget->palette().window().color();
        iconColor   = widget->palette().windowText().color();
    } else if (qApp) {
        // might not have a QApplication
        buttonColor = qApp->palette().window().color();
        iconColor   = qApp->palette().windowText().color();
    } else {// KCS is always safe
        buttonColor = KColorScheme(QPalette::Active, KColorScheme::Window,
                                   _sharedConfig).background().color();
        iconColor   = KColorScheme(QPalette::Active, KColorScheme::Window,
                                   _sharedConfig).foreground().color();
    }

    switch (standardIcon) {
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
            {
                qreal width( 1.1 );
                painter.translate(0, 0.5);
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
                painter.drawLine( QPointF(8.75,8.75), QPointF(6.5,11.0) );

            }

            {
                qreal width( 1.1 );
                painter.translate(0,-1);
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

                painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
                painter.drawLine( QPointF(8.75,8.75), QPointF(6.5,11.0) );
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
            {
                qreal width( 1.1 );
                painter.translate(0, 0.5);
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen( _helper.calcLightColor( buttonColor ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
                painter.drawLine( QPointF(8.75,8.75), QPointF(11.0,6.5) );
            }

            {
                qreal width( 1.1 );
                painter.translate(0,-1);
                painter.setBrush(Qt::NoBrush);
                painter.setPen(QPen( iconColor, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

                painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
                painter.drawLine( QPointF(8.75,8.75), QPointF(11.0,6.5) );
            }

            return QIcon( realpm );
        }

        default:
        return KStyle::standardIconImplementation(standardIcon, option, widget);
    }
}

QPoint OxygenStyle::handleRTL(const QStyleOption* opt, const QPoint& pos) const
{
    return visualPos(opt->direction, opt->rect, pos);
}

QRect OxygenStyle::handleRTL(const QStyleOption* opt, const QRect& subRect) const
{
    return visualRect(opt->direction, opt->rect, subRect);
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
