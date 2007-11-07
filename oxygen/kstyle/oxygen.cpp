/* Oxygen widget style for KDE 4
   Copyright (C) 2007 Casper Boemann <cbr@boemann.dk>

    based on the plastik style which is:

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

#include "oxygen.h"
#include "oxygen.moc"

#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <QtCore/QSettings>
#include <QtGui/QStyleOption>
#include <QtGui/QApplication>

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QToolButton>
#include <QtGui/QToolBar>
#include <QtGui/QScrollBar>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QDockWidget>
#include <QStyleOptionDockWidget>

#include <QtDBus/QtDBus>

#include <KGlobal>
#include <KGlobalSettings>
#include <KColorUtils>

#include <math.h>

#include "helper.h"
#include "tileset.h"

K_EXPORT_STYLE("Oxygen", OxygenStyle)

K_GLOBAL_STATIC_WITH_ARGS(OxygenStyleHelper, globalHelper, ("OxygenStyle"))

OxygenStyle::OxygenStyle() :
//     kickerMode(false),
//     kornMode(false),
    flatMode(false),
    _helper(*globalHelper)
{
    _config = _helper.config();

    // connect to KGlobalSettings signals so we will be notified when the
    // system palette (in particular, the contrast) is changed
    QDBusConnection::sessionBus().connect( QString(), "/KGlobalSettings",
                                           "org.kde.KGlobalSettings",
                                           "notifyChange", this,
                                           SLOT(globalSettingsChange(int,int))
                                         );

    // call the slot directly; this initial call will set up things that also
    // need to be reset when the system palette changes
    globalSettingsChange(KGlobalSettings::PaletteChanged, 0);

    setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth, 2);

    // TODO: change this when double buttons are implemented
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleBotButton, true);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::MinimumSliderHeight, 21);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::BarWidth, 15); // size*2+1
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::ArrowColor,QPalette::ButtonText);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::ActiveArrowColor,QPalette::ButtonText);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::SingleButtonHeight, 14);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleButtonHeight, 28);

    setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Left, 16);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Right, 16);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Top, 1);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Bot, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Left, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Right, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Top, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Bot, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftHorizontal, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftVertical,   0);

    setWidgetLayoutProp(WT_Splitter, Splitter::Width, 6);

    setWidgetLayoutProp(WT_CheckBox, CheckBox::Size, 23);
    setWidgetLayoutProp(WT_RadioButton, RadioButton::Size, 25);

    setWidgetLayoutProp(WT_DockWidget, DockWidget::TitleTextColor, QPalette::WindowText);
    setWidgetLayoutProp(WT_DockWidget, DockWidget::FrameWidth, 1);
    setWidgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, 2);

    setWidgetLayoutProp(WT_MenuBar, MenuBar::ItemSpacing, 6);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin,        0);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Left,  6);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Right, 6);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Top, 0);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Bot, 4);

    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin, 3);
    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin+Left, 3);
    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin+Right, 3);

    setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckAlongsideIcon, 1);
    setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckWidth, 13);

    setWidgetLayoutProp(WT_ProgressBar, ProgressBar::BusyIndicatorSize, 10);

    setWidgetLayoutProp(WT_TabBar, TabBar::TabOverlap, 1);
    setWidgetLayoutProp(WT_TabBar, TabBar::BaseOverlap, 7);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Left, 8);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Right, 8);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Top, 2);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Bot, 2);

    setWidgetLayoutProp(WT_TabWidget, TabWidget::ContentsMargin, 6);

    setWidgetLayoutProp(WT_Slider, Slider::HandleThickness, 25);
    setWidgetLayoutProp(WT_Slider, Slider::HandleLength, 19);

    setWidgetLayoutProp(WT_SpinBox, SpinBox::FrameWidth, 6);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Left, 3);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Top, -2);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Bot, -1);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonWidth, 19);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonSpacing, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Left, 2);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Right, 9);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Top, 5);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Bot, 4);

    setWidgetLayoutProp(WT_ComboBox, ComboBox::FrameWidth, 6);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Left, 3);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Top, -1);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Bot, -1);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonWidth, 19);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Left, 2);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Right, 9);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Top, 6);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Bot, 3);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::FocusMargin, 0);

    setWidgetLayoutProp(WT_ToolBar, ToolBar::FrameWidth, 0);
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemSpacing, 1);
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemMargin, 0);

    setWidgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin, 4);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::FocusMargin,    0);

    setWidgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, 5);

    QSettings settings;
    // TODO get from KGlobalSettings::contrastF or expose in OxygenHelper
    _contrast = settings.value("/Qt/KDE/contrast", 6).toInt();
    settings.beginGroup("/oxygenstyle/Settings");
    _scrollBarLines = settings.value("/scrollBarLines", false).toBool();
    _animateProgressBar = settings.value("/animateProgressBar", true).toBool();
    _drawToolBarItemSeparator = settings.value("/drawToolBarItemSeparator", true).toBool();
    _drawFocusRect = settings.value("/drawFocusRect", true).toBool();
    _drawTriangularExpander = settings.value("/drawTriangularExpander", false).toBool();
    _inputFocusHighlight = settings.value("/inputFocusHighlight", true).toBool();
    settings.endGroup();

    // FIXME below this line to be deleted (and can we not use QSettings? KConfig* is safe now)
    _customOverHighlightColor = true;
    _customFocusHighlightColor = true;

    if ( _animateProgressBar )
    {
        animationTimer = new QTimer( this );
        connect( animationTimer, SIGNAL(timeout()), this, SLOT(updateProgressPos()) );
    }

}


void OxygenStyle::updateProgressPos()
{
    QProgressBar* pb;
    //Update the registered progressbars.
    QMap<QWidget*, int>::iterator iter;
    bool visible = false;
    for (iter = progAnimWidgets.begin(); iter != progAnimWidgets.end(); ++iter)
    {
        pb = dynamic_cast<QProgressBar*>(iter.key());

        if ( !pb )
            continue;

        if ( iter.key() -> isEnabled() &&
             pb->value() != pb->maximum() )
        {
            // update animation Offset of the current Widget
            iter.value() = (iter.value() + 1) % 32;
            iter.key()->update();
        }
        if (iter.key()->isVisible())
            visible = true;
    }
    if (!visible)
        animationTimer->stop();
}


OxygenStyle::~OxygenStyle()
{
}


void OxygenStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{
    switch (element)
    {
        case PE_Widget:
        {
            if (!widget)
                return;

            if (widget->isWindow()) {
            QColor color = option->palette.color(widget->backgroundRole());
            int splitY = qMin(300, 3*option->rect.height()/4);

            QRect upperRect = QRect(0, 0, option->rect.width(), splitY);
            QPixmap tile = _helper.verticalGradient(color, splitY);
            painter->drawTiledPixmap(upperRect, tile);

            QRect lowerRect = QRect(0,splitY, option->rect.width(), option->rect.height() - splitY);
            painter->fillRect(lowerRect, _helper.backgroundBottomColor(color));

            int radialW = qMin(600, option->rect.width());
            int frameH = widget->geometry().y() - widget->y();
            tile = _helper.radialGradient(color, radialW);
            QRect radialRect = QRect((option->rect.width() - radialW) / 2, 0, radialW, 64-frameH);
            painter->drawPixmap(radialRect, tile, QRect(0, frameH, radialW, 64-frameH));
            }

            break;
        }

        default:
            KStyle::drawPrimitive(element, option, painter, widget);
    }
}


void OxygenStyle::drawKStylePrimitive(WidgetType widgetType, int primitive,
                                       const QStyleOption* opt,
                                       const QRect &r, const QPalette &pal,
                                       State flags, QPainter* p,
                                       const QWidget* widget,
                                       KStyle::Option* kOpt) const
{
    StyleOptions opts = 0;
    const bool reverseLayout = opt->direction == Qt::RightToLeft;

    const bool enabled = flags & State_Enabled;
    const bool mouseOver(enabled && (flags & State_MouseOver));

    switch (widgetType)
    {
        case WT_PushButton:
        {
            switch (primitive)
            {
                case PushButton::Panel:
                {
                    if ((flags & State_On) || (flags & State_Sunken))
                        opts |= Sunken;
                    if (flags & State_HasFocus)
                        opts |= Focus;
                    if (enabled && (flags & State_MouseOver))
                        opts |= Hover;

                    renderSlab(p, r, pal.color(QPalette::Button), opts);
                    return;
                }

                case PushButton::DefaultButtonFrame:
                {
                    return;
                }
            }
        }
        break;

        case WT_ToolBoxTab:
        {
            switch (primitive)
            {
                case ToolBoxTab::Panel:
                {
//                    bool sunken   = (flags & State_On) || (flags & State_Sunken) || (flags & State_Selected);

//                    renderButton(p, r, pal, sunken, mouseOver);

                    return;
                }
            }
        }
        break;

        case WT_ProgressBar:
        {
//             const Q3ProgressBar *pb = dynamic_cast<const Q3ProgressBar*>(widget);
//             int steps = pb->totalSteps();

            QColor bg = enabled?pal.color(QPalette::Base):pal.color(QPalette::Background); // background
            QColor fg = enabled?pal.color(QPalette::Highlight):pal.color(QPalette::Background).dark(110); // foreground


            switch (primitive)
            {
                case ProgressBar::Groove:
                {
                    QColor color = pal.color(QPalette::Button);
                    QRect rect = r.adjusted(2,0,-2,0);

                    TileSet *tiles1 = _helper.horizontalScrollBar(color, rect.height(), r.width());

                    p->save();
                    p->setClipRect(rect.adjusted(-32,0,32,0));
                    tiles1->render(rect, p, TileSet::Left | TileSet::Vertical | TileSet::Right);
                    p->restore();
                    return;
                }

                case ProgressBar::BusyIndicator:
                {
                    QColor color = _viewHoverBrush.brush(pal).color();
                    QRect rect = r.adjusted(0,-2,0,2);

                    TileSet *tiles1 = _helper.horizontalScrollBar(color, rect.height(), r.width());

                    p->save();
                    p->setClipRect(rect.adjusted(-32,0,32,0));
                    tiles1->render(rect, p, TileSet::Left | TileSet::Vertical | TileSet::Right);
                    p->restore();
                    return;
                }

                case ProgressBar::Indicator:
                {
                    QColor color = _viewHoverBrush.brush(pal).color();
                    QRect rect = r.adjusted(0,-2,2+r.width() / 300,2); // right pos: hackish, but neccessary...

                    int staticShift = 0;
                    int animShift = 0;
                    if (_animateProgressBar) {
                        // find the animation Offset for the current Widget
                        QWidget* nonConstWidget = const_cast<QWidget*>(widget);
                        QMap<QWidget*, int>::const_iterator iter = progAnimWidgets.find(nonConstWidget);
                        if (iter != progAnimWidgets.end())
                            animShift = iter.value();
                    }
                    TileSet *tiles1 = _helper.horizontalScrollBar(color, rect.height(), r.width()-animShift);

                    p->save();
                    p->setClipRect(rect.adjusted(-32,0,32,0));
                    tiles1->render(rect, p, TileSet::Left | TileSet::Vertical | TileSet::Right);
                    p->restore();
                    return;
                }
            }
        }
        break;

        case WT_MenuBar:
        {
            switch (primitive)
            {
                case MenuBar::EmptyArea:
                {
                    return;
                }
            }
        }
        break;

        case WT_MenuBarItem:
        {
            switch (primitive)
            {
                case MenuBarItem::Panel:
                {
                    bool active  = flags & State_Selected;
                    bool focused = flags & State_HasFocus;
//                     bool down = flags & State_Sunken;

                    if (active && focused) {
//                        renderButton(p, r, pal, down, mouseOver, true);
                    }

                    return;
                }
            }
        }
        break;

        case WT_Menu:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    //FIXME CBR renderContour(p, r, pal.color(QPalette::Background), pal.color(QPalette::Background).dark(200),                          Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);
                    return;
                }

                case Menu::Background:
                {
                    p->fillRect( r, pal.color(QPalette::Background).light( 105 ) );
                    return;
                }

                case Menu::TearOff:
                {
                    // TODO: See Keramik...

                    return;
                }

                case Menu::Scroller:
                {
                    // TODO
                    return;
                }
            }
        }
        break;

        case WT_MenuItem:
        {
            switch (primitive)
            {
                case MenuItem::Separator:
                {
                    p->setPen( pal.mid().color() );
                    p->drawLine( r.x()+5, r.y() /*+ 1*/, r.right()-5, r.y() );
                    p->setPen( pal.color( QPalette::Light ) );
                    p->drawLine( r.x()+5, r.y() + 1, r.right()-5 , r.y() + 1 );

                    return;
                }

                case MenuItem::ItemIndicator:
                {
                    if (enabled) {
                        p->fillRect(r, pal.color(QPalette::Highlight));
                    }
                    else {
                        drawKStylePrimitive(WT_Generic, Generic::FocusIndicator, opt, r, pal, flags, p, widget, kOpt);
                    }
                    return;
                }

                case MenuItem::CheckColumn:
                {
                    // empty
                    return;
                }

                case MenuItem::CheckOn:
                {
                    renderCheckBox(p, r.adjusted(-2,-3,3,3), pal, enabled, false, mouseOver, CheckBox::CheckOn, true);
                    return;
                }

                case MenuItem::CheckOff:
                {
                    renderCheckBox(p, r.adjusted(-2,-3,3,3), pal, enabled, false, mouseOver, CheckBox::CheckOff, true);
                    return;
                }

                case MenuItem::RadioOn:
                {
                    renderRadioButton(p, r, pal, enabled, mouseOver, RadioButton::RadioOn, true);
                    return;
                }

                case MenuItem::RadioOff:
                {
                    renderRadioButton(p, r, pal, enabled, mouseOver, RadioButton::RadioOff, true);
                    return;
                }

                case MenuItem::CheckIcon:
                {
                    // TODO
                    return;
                }
            }
        }
        break;

        case WT_DockWidget:
        {
            switch (primitive)
            {
//                case Generic::Text:
                case Generic::Frame:
                {
                    // shadows of the frame
                    int x,y,w,h;

                    r.getRect(&x, &y, &w, &h);

                    p->setBrush(Qt::NoBrush);
                    QLinearGradient lg(0, 0, 0, 10);
                    QGradientStops stops;
                    stops << QGradientStop( 0, QColor(255,255,255, 110) )
                        << QGradientStop( 1, QColor(128,128,128, 60) );
                    lg.setStops(stops);
                    p->setPen(QPen(QBrush(lg),1));
                    p->drawLine(QPointF(6.3, 0.5), QPointF(w-6.3, 0.5));
                    p->drawArc(QRectF(0.5, 0.5, 9.5, 9.5),90*16, 90*16);
                    p->drawArc(QRectF(w-9.5-0.5, 0.5, 9.5, 9.5), 0, 90*16);

                    p->setPen(QColor(128,128,128, 60));
                    p->drawLine(QPointF(0.5, 6.3), QPointF(0.5, h-6.3));
                    p->drawLine(QPointF(w-0.5, 6.3), QPointF(w-0.5, h-6.3));

                    lg = QLinearGradient(0, h-10, 0, h);
                    stops.clear();
                    stops << QGradientStop( 0, QColor(128,128,128, 60) )
                        << QGradientStop( 1, QColor(0,0,0, 50) );
                    lg.setStops(stops);
                    p->setPen(QPen(QBrush(lg),1));
                    p->drawArc(QRectF(0.5, h-9.5-0.5, 9.5, 9.5),180*16, 90*16);
                    p->drawArc(QRectF(w-9.5-0.5, h-9.5-0.5, 9.5, 9.5), 270*16, 90*16);
                    p->drawLine(QPointF(6.3, h-0.5), QPointF(w-6.3, h-0.5));
                    return;
                }

                case DockWidget::TitlePanel:
                {
                    const QStyleOptionDockWidget* dwOpt = ::qstyleoption_cast<const QStyleOptionDockWidget*>(opt);
                    const QDockWidget *dw = qobject_cast<const QDockWidget*>(widget);
                    if (!dw) return; // widget is not even guarenteed to be non-NULL, let alone for it to be a QDockWidget
                    if (!dwOpt) return;
                    if (dw->isFloating()) return;

                    int x,y,w,h;

                    dw->rect().getRect(&x, &y, &w, &h);
                    h--;
                    p->setPen(QColor(0,0,0, 30));
                    p->drawLine(QPointF(6.3, 0.5), QPointF(w-6.3, 0.5));
                    p->drawArc(QRectF(0.5, 0.5, 9.5, 9.5),90*16, 90*16);
                    p->drawArc(QRectF(w-9.5-0.5, 0.5, 9.5, 9.5), 0, 90*16);
                    p->drawLine(QPointF(0.5, 6.3), QPointF(0.5, h-6.3));
                    p->drawLine(QPointF(w-0.5, 6.3), QPointF(w-0.5, h-6.3));
                    p->drawArc(QRectF(0.5, h-9.5-0.5, 9.5, 9.5),180*16, 90*16);
                    p->drawArc(QRectF(w-9.5-0.5, h-9.5-0.5, 9.5, 9.5), 270*16, 90*16);
                    p->drawLine(QPointF(6.3, h-0.5), QPointF(w-6.3, h-0.5));
                    return;
                }

                case DockWidget::SeparatorHandle:
                    if (flags&State_Horizontal)
                        drawKStylePrimitive(WT_Splitter, Splitter::HandleVert, opt, r, pal, flags, p, widget);
                    else
                        drawKStylePrimitive(WT_Splitter, Splitter::HandleHor, opt, r, pal, flags, p, widget);
                    return;
            }
        }
        break;

        case WT_StatusBar:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    return;
                }
            }
        }
        break;

        case WT_CheckBox:
        {
            switch(primitive)
            {
                case CheckBox::CheckOn:
                case CheckBox::CheckOff:
                case CheckBox::CheckTriState:
                {
                    bool hasFocus = flags & State_HasFocus;

                    renderCheckBox(p, r, pal, enabled, hasFocus, mouseOver, primitive);
                    return;
                }
            }
        }
        break;

        case WT_RadioButton:
        {
            switch(primitive)
            {
                case RadioButton::RadioOn:
                case RadioButton::RadioOff:
                {
                    renderRadioButton(p, r, pal, enabled, mouseOver, primitive);
                    return;
                }
            }

        }
        break;

        case WT_ScrollBar:
        {
            switch (primitive)
            {
                case ScrollBar::DoubleButtonHor:
                    renderHole(p, pal.color(QPalette::Window), QRect(r.left()-5, 0, 5, r.height()),
                               false, false, TileSet::Top | TileSet::Bottom | TileSet::Right);
                    break;

                case ScrollBar::DoubleButtonVert:
                    renderHole(p, pal.color(QPalette::Window), QRect(0, r.top()-5, r.width(), 5),
                               false, false, TileSet::Left | TileSet::Bottom | TileSet::Right);
                    break;

                case ScrollBar::SingleButtonHor:
                    renderHole(p, pal.color(QPalette::Window), QRect(r.right()+3, 0, 5, r.height()),
                               false, false, TileSet::Top | TileSet::Left | TileSet::Bottom);
                    break;

                case ScrollBar::SingleButtonVert:
                    renderHole(p, pal.color(QPalette::Window), QRect(0, r.bottom()+3, r.width(), 5),
                               false, false, TileSet::Top | TileSet::Left | TileSet::Right);
                    break;

                case ScrollBar::GrooveAreaVertTop:
                {
                    renderHole(p, pal.color(QPalette::Window), r.adjusted(0,2,0,12),
                               false, false, TileSet::Left | TileSet::Right);
                    return;
                }

                case ScrollBar::GrooveAreaVertBottom:
                {
                    renderHole(p, pal.color(QPalette::Window), r.adjusted(0,-10,0,0), false, false,
                               TileSet::Left | TileSet::Right);
                    return;
                }

                case ScrollBar::GrooveAreaHorLeft:
                {
                    renderHole(p, pal.color(QPalette::Window), r.adjusted(2,0,12,0), false, false,
                               TileSet::Top | TileSet::Bottom);
                    return;
                }

                case ScrollBar::GrooveAreaHorRight:
                {
                    renderHole(p, pal.color(QPalette::Window), r.adjusted(-10,0,0,0), false, false,
                               TileSet::Top | TileSet::Bottom);
                    return;
                }

                case ScrollBar::SliderVert:
                {
                    QColor color = pal.color(QPalette::Button);
                    if (mouseOver || (flags & State_Sunken)) // TODO not when disabled ((flags & State_Enabled) doesn't work?)
                        color = _viewHoverBrush.brush(pal).color();
                    QRect rect = r.adjusted(1,3,-1,0);

                    renderHole(p, pal.color(QPalette::Window), rect.adjusted(-1,-1,1,0), false, false,
                               TileSet::Left | TileSet::Right);

                    int offset = rect.top()/2; // divide by 2 to make the "lightplay" move half speed of the handle
                    int remainder = qMin(12, rect.height()/2);

                    // Draw the handle in two parts, the top, and the bottom with calculated offset
                    TileSet *tiles1 = _helper.verticalScrollBar(color, rect.width(), offset);
                    TileSet *tiles2 = _helper.verticalScrollBar(color, rect.width(), offset+rect.height()+8);

                    p->save();
                    p->setClipRect(rect.adjusted(0,0,0,-remainder-1));
                    tiles1->render(rect, p, TileSet::Top | TileSet::Horizontal);
                    p->setClipRect( QRect(rect.left(), rect.bottom()-remainder, rect.width(), remainder));
                    tiles2->render( QRect(rect.left(), rect.bottom()-32, rect.width(),32),
                                    p, TileSet::Bottom | TileSet::Horizontal);
                    p->restore();
                    return;
                }

                case ScrollBar::SliderHor:
                {
                    QColor color = pal.color(QPalette::Button);
                    if (mouseOver || (flags & State_Sunken)) // TODO not when disabled ((flags & State_Enabled) doesn't work?)
                        color = _viewHoverBrush.brush(pal).color();
                    QRect rect = r.adjusted(3,1,0,-1);

                    renderHole(p, pal.color(QPalette::Window), rect.adjusted(-1,-1,0,1), false, false,
                               TileSet::Top | TileSet::Bottom);

                    int offset = r.left()/2; // divide by 2 to make the "lightplay" move half speed of the handle
                    int remainder = qMin(12, rect.width()/2);

                    // Draw the handle in two parts, the top, and the bottom with calculated offset
                    TileSet *tiles1 = _helper.horizontalScrollBar(color, rect.height(), offset);
                    TileSet *tiles2 = _helper.horizontalScrollBar(color, rect.height(), offset+rect.width()+8);

                    p->save();
                    p->setClipRect(rect.adjusted(0,0,-remainder-1,0));
                    tiles1->render(rect, p, TileSet::Left | TileSet::Vertical);
                    p->setClipRect( QRect(rect.right()-remainder, rect.top(), remainder, rect.height()) );
                    tiles2->render( QRect(rect.right()-32, rect.top(), 32, rect.height()),
                                    p, TileSet::Right | TileSet::Vertical);
                    p->restore();
                    return;
                }

            }

        }
        break;

        case WT_TabBar:
        {
            const QStyleOptionTabV2* tabOpt = qstyleoption_cast<const QStyleOptionTabV2*>(opt);

            switch (primitive)
            {
                case TabBar::NorthTab:
                case TabBar::SouthTab:
                {
                    if (!tabOpt) break;

                    QStyleOptionTab::TabPosition pos = tabOpt->position;
                    bool bottom = primitive == TabBar::SouthTab;

                    // TODO: tab painting needs a lot of work in order to handle east and west tabs.
                    renderTab(p, r, pal, mouseOver, flags&State_Selected, bottom, tabOpt,
reverseLayout);

                    return;
                }
                case TabBar::BaseFrame:
                    //p->fillRect(r,QColor(Qt::red));
                    return;

                // TODO: TabBar::EastTab, TabBar::WestTab, TabBar::ScrollButton
            }

        }
        break;

        case WT_TabWidget:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    // FIXME!!
                    const QStyleOptionTabWidgetFrame* tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(opt);
                    int w = tabOpt->tabBarSize.width();
                    int lw = tabOpt->leftCornerWidgetSize.width();
                    //int h = tabOpt->tabBarSize.height();
                    switch(tabOpt->shape)
                    {
                        case QTabBar::RoundedNorth:
                        case QTabBar::TriangularNorth:
                            renderSlab(p, r, pal.color(QPalette::Window), NoFill,
                                       TileSet::Left | TileSet::Bottom | TileSet::Right);
                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode

                                if (w+lw >0)
                                    renderSlab(p, QRect(0, r.y(), r.width() - w - lw+7, 7),
                                        pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);
                                else
                                    renderSlab(p, QRect(0, r.y(), r.width(), 7), pal.color(QPalette::Window), NoFill,
                                            TileSet::Left | TileSet::Top | TileSet::Right);

                                if (lw > 0)
                                    renderSlab(p, QRect(r.right() - lw-7, r.y(), lw+7, 7),
                                             pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);
                            }
                            else
                            {
                                if (lw > 0)
                                    renderSlab(p, QRect(0, r.y(), lw+7, 7), pal.color(QPalette::Window), NoFill,
                                        TileSet::Left | TileSet::Top);

                                if (w+lw >0)
                                    renderSlab(p, QRect(w+lw-7, r.y(), r.width() - w - lw+7, 7), pal.color(QPalette::Window), NoFill,
                                            TileSet::Top | TileSet::Right);
                                else
                                    renderSlab(p, QRect(0, r.y(), r.width(), 7), pal.color(QPalette::Window), NoFill,
                                            TileSet::Left | TileSet::Top | TileSet::Right);

                            }
                            return;

                        case QTabBar::RoundedSouth:
                        case QTabBar::TriangularSouth:
                            renderSlab(p, r, pal.color(QPalette::Window), NoFill,
                                       TileSet::Left | TileSet::Top | TileSet::Right);
                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode

                                if (w+lw >0)
                                    renderSlab(p, QRect(0, r.bottom()-7, r.width() - w - lw + 7, 7),
                                        pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);
                                else
                                    renderSlab(p, QRect(0, r.bottom()-7, r.width(), 7), pal.color(QPalette::Window),
                                        NoFill, TileSet::Left | TileSet::Bottom | TileSet::Right);

                                if (lw > 0)
                                    renderSlab(p, QRect(r.right() - lw-7, r.bottom()-7, lw+7, 7),
                                        pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                            }
                            else
                            {
                                if (lw > 0)
                                    renderSlab(p, QRect(0, r.bottom()-7, lw+7, 7),
                                            pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);

                                if (w+lw >0)
                                    renderSlab(p, QRect(w+lw-7, r.bottom()-7, r.width() - w - lw+7, 7),
                                            pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                                else
                                    renderSlab(p, QRect(0, r.bottom()-7, r.width(), 7), pal.color(QPalette::Window),
                                        NoFill, TileSet::Left | TileSet::Bottom | TileSet::Right);

                            }
                            return;

                        case QTabBar::RoundedWest:
                        case QTabBar::TriangularWest:
                            renderSlab(p, r, pal.color(QPalette::Window), NoFill,
                                       TileSet::Top | TileSet::Right | TileSet::Bottom);
                            return;

                        case QTabBar::RoundedEast:
                        case QTabBar::TriangularEast:
                            renderSlab(p, r, pal.color(QPalette::Window), NoFill,
                                       TileSet::Top | TileSet::Left | TileSet::Bottom);
                            return;
                        default:
                            return;
                    }
                }

            }
        }
        break;

        case WT_Window:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    return;
                }

                case Window::TitlePanel:
                    p->fillRect(r, QColor(Qt::green) );
                    return;

                case Window::ButtonMin:
                case Window::ButtonMax:
                case Window::ButtonRestore:
                case Window::ButtonClose:
                case Window::ButtonShade:
                case Window::ButtonUnshade:
                case Window::ButtonHelp:
                {
                    KStyle::TitleButtonOption* tbkOpts =
                            extractOption<KStyle::TitleButtonOption*>(kOpt);
                    State bflags = flags;
                    bflags &= ~State_Sunken;
                    if (tbkOpts->active)
                        bflags |= State_Sunken;
                    drawKStylePrimitive(WT_ToolButton, ToolButton::Panel, opt, r, pal, bflags, p, widget);
                    return;
                }

            }
        }
        break;

        case WT_Splitter:
        {
            switch (primitive)
            {
                case Splitter::HandleHor:
                {
                    int h = r.height();
                    QColor color = pal.color(QPalette::Background);

                    int ngroups = qMax(1,h / 250);
                    int center = (h - (ngroups-1) * 250) /2 + r.top();
                    for(int k = 0; k < ngroups; k++, center += 250) {
                        renderDot(p, QPointF(r.left()+3, center-3), color);
                        renderDot(p, QPointF(r.left()+3, center), color);
                        renderDot(p, QPointF(r.left()+3, center+3), color);
                    }
                    return;
                }
                case Splitter::HandleVert:
                {
                    int w = r.width();
                    QColor color = pal.color(QPalette::Background);

                    int ngroups = qMax(1, w / 250);
                    int center = (w - (ngroups-1) * 250) /2 + r.left();
                    for(int k = 0; k < ngroups; k++, center += 250) {
                        renderDot(p, QPointF(center-3, r.top()+3), color);
                        renderDot(p, QPointF(center, r.top()+3), color);
                        renderDot(p, QPointF(center+3, r.top()+3), color);
                    }
                    return;
                }
            }
        }
        break;

        case WT_Slider:
        {
            // TODO
            switch (primitive)
            {
                case Slider::HandleHor:
                case Slider::HandleVert:
                {
                    StyleOptions opts = (flags & State_HasFocus ? Focus : StyleOption());
                    if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt))
                        if(slider->activeSubControls & SC_SliderHandle)
                            if (mouseOver) opts |= Hover;

                    renderSlab(p, r, pal.color(QPalette::Button), opts);
                    return;
                }

                case Slider::GrooveHor:
                case Slider::GrooveVert:
                {

                    bool horizontal = primitive == Slider::GrooveHor;

                    if (horizontal) {
                        int center = r.y()+r.height()/2;
                        renderHole(p, pal.color(QPalette::Window), QRect(r.left()+4, center-2, r.width()-8, 5));
                    } else {
                        int center = r.x()+r.width()/2;
                        renderHole(p, pal.color(QPalette::Window), QRect(center-2, r.top()+4, 5, r.height()-8));
                    }

                    return;
                }
            }

        }
        break;

        case WT_SpinBox:
        {
            bool hasFocus = flags & State_HasFocus;

            const QColor inputColor = enabled?pal.color(QPalette::Base):pal.color(QPalette::Background);

            switch (primitive)
            {
                case Generic::Frame:
                {
                    QRect fr = r.adjusted(2,2,-2,-2);
                     p->save();
                     p->setRenderHint(QPainter::Antialiasing);
                     p->setPen(Qt::NoPen);
                     p->setBrush(inputColor);

                     _helper.fillHole(*p, r);

                     p->restore();
                    // TODO use widget background role?
                    renderHole(p, inputColor, fr, hasFocus, mouseOver);
                    return;
                }
                case SpinBox::EditField:
                case SpinBox::ButtonArea:
                case SpinBox::UpButton:
                case SpinBox::DownButton:
                {
                    return;
                }

            }

        }
        break;

        case WT_ComboBox:
        {
            bool editable = false;
            if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt) )
                editable = cb->editable;

            bool hasFocus = flags & State_HasFocus;
            StyleOptions opts = (flags & State_HasFocus ? Focus : StyleOption());
            if (mouseOver) opts |= Hover;

            const QColor buttonColor = enabled?pal.color(QPalette::Button):pal.color(QPalette::Background);
            const QColor inputColor = enabled ? pal.color(QPalette::Base) : pal.color(QPalette::Background);
            QRect editField = subControlRect(CC_ComboBox, qstyleoption_cast<const QStyleOptionComplex*>(opt), SC_ComboBoxEditField, widget);

            switch (primitive)
            {
                case Generic::Frame:
                {
                    // TODO: pressed state
                    if(!editable) {
                        renderSlab(p, r, pal.color(QPalette::Button), opts);
                    } else {
                        QRect fr = r.adjusted(2,2,-2,-2);
                        // input area
                        p->save();
                        p->setRenderHint(QPainter::Antialiasing);
                        p->setPen(Qt::NoPen);
                        p->setBrush(inputColor);

                        _helper.fillHole(*p, r.adjusted(0,0,0,-1));

                        p->restore();

                        if (_inputFocusHighlight && hasFocus && enabled)
                        {
                            renderHole(p, inputColor, fr, true, mouseOver);
                        }
                        else
                        {
                            renderHole(p, inputColor, fr, false, mouseOver);
                        }
                    }

                    return;
                }

                case ComboBox::EditField:
                {
                    // empty
                    return;
                }

                case ComboBox::Button:
                {
                    return;
                }
            }

        }
        break;

        case WT_Header:
        {
            switch (primitive)
            {
                case Header::SectionHor:
                case Header::SectionVert:
                {
                    if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
                        bool isFirst = (primitive==Header::SectionHor)&&(header->position == QStyleOptionHeader::Beginning);

                        p->setPen(pal.color(QPalette::Text));

                        QColor color = pal.color(QPalette::Button);
                        p->fillRect(r, color);
                        if(primitive == Header::SectionHor) {
                            if(header->section != 0 || isFirst) {
                                int center = r.center().y();
                                renderDot(p, QPointF(r.right()-1, center-3), color);
                                renderDot(p, QPointF(r.right()-1, center), color);
                                renderDot(p, QPointF(r.right()-1, center+3), color);
                            }
                        }
                        else
                            p->drawLine(r.bottomLeft(),r.bottomRight());
                    }

                    return;
                }
            }
        }
        break;

        case WT_Tree:
        {
            switch (primitive)
            {
                case Tree::VerticalBranch:
                case Tree::HorizontalBranch:
                {
                //### FIXME: set sane color.
                    QBrush brush(Qt::Dense4Pattern);
                    brush.setColor(pal.mid().color() );
                    p->fillRect(r, brush);
                    return;
                }
                case Tree::ExpanderOpen:
                case Tree::ExpanderClosed:
                {
                    int radius = (r.width() - 4) / 2;
                    int centerx = r.x() + r.width()/2;
                    int centery = r.y() + r.height()/2;

                    p->setPen( pal.text().color() );
                    if(!_drawTriangularExpander)
                    {
                        // plus or minus
                        p->drawLine( centerx - radius, centery, centerx + radius, centery );
                        if (primitive == Tree::ExpanderClosed) // Collapsed = On
                            p->drawLine( centerx, centery - radius, centerx, centery + radius );
                    } else {
                        if(primitive == Tree::ExpanderClosed)
                            drawKStylePrimitive(WT_Generic, Generic::ArrowRight, opt, QRect(r.x()+1,r.y()+1,r.width(),r.height()), pal, flags, p, widget);
                        else
                            drawKStylePrimitive(WT_Generic, Generic::ArrowDown, opt, QRect(r.x()+1,r.y()+1,r.width(),r.height()), pal, flags, p, widget);
                    }

                    return;
                }
                default:
                    break;
            }
        }
        break;

        case WT_LineEdit:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    const bool isReadOnly = flags & State_ReadOnly;
                    const bool isEnabled = flags & State_Enabled;
                    const bool hasFocus = flags & State_HasFocus;
                    const QColor inputColor =
                                enabled?pal.color(QPalette::Base):pal.color(QPalette::Background);

                    if ( _inputFocusHighlight && hasFocus && !isReadOnly && isEnabled)
                    {
                        renderHole(p, inputColor, r.adjusted(2,2,-2,-3), true, mouseOver);
                    }
                    else
                    {
                        renderHole(p, inputColor, r.adjusted(2,2,-2,-3), false, mouseOver);
                    }
                    return;
                }

                case LineEdit::Panel:
                {
                    const QColor inputColor =
                                enabled?pal.color(QPalette::Base):pal.color(QPalette::Background);

                    if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
                    {
                        const int lineWidth(panel->lineWidth);

                        if (lineWidth > 0)
                        {
                            int lw = lineWidth - 5;

                            p->save();
                            p->setRenderHint(QPainter::Antialiasing);
                            p->setPen(Qt::NoPen);
                            p->setBrush(inputColor);

                            _helper.fillHole(*p, r.adjusted(lw,lw,-lw,-lw-1));
                            drawPrimitive(PE_FrameLineEdit, panel, p, widget);

                            p->restore();
                        }
                        else
                        {
                            p->fillRect(r.adjusted(2,2,-2,-1), inputColor);
                        }
                    }
                }
            }

        }
        break;

        case WT_GroupBox:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    QColor color = pal.color(QPalette::Window);

                    p->save();
                    p->setRenderHint(QPainter::Antialiasing);
                    p->setPen(Qt::NoPen);

                    QLinearGradient innerGradient(0, r.top()-r.height()+12, 0, r.bottom()+r.height()-19);
                    QColor light = _helper.calcLightColor(color); //KColorUtils::shade(calcLightColor(color), shade));
                    light.setAlphaF(0.4);
                    innerGradient.setColorAt(0.0, light);
                    color.setAlphaF(0.4);
                    innerGradient.setColorAt(1.0, color);
                    p->setBrush(innerGradient);
                    p->setClipRect(r.adjusted(0, 0, 0, -19));
                    _helper.fillSlab(*p, r);

                    TileSet *slopeTileSet = _helper.slope(pal.color(QPalette::Window), 0.0);
                    p->setClipping(false);
                    slopeTileSet->render(r, p);

                    p->restore();

                    return;
                }
            }

        }
        break;

        case WT_ToolBar:
        {
            switch (primitive)
            {
                case ToolBar::HandleHor:
                {
                    int counter = 1;

                        int center = r.left()+r.width()/2;
                        for(int j = r.top()+2; j <= r.bottom()-3; j+=3) {
                            if(counter%2 == 0) {
                                renderDot(p, QPoint(center+1, j), pal.color(QPalette::Background));
                            } else {
                                renderDot(p, QPoint(center-2, j), pal.color(QPalette::Background));
                            }
                            counter++;
                        }
                    return;
                }
                case ToolBar::HandleVert:
                {
                    int counter = 1;

                        int center = r.top()+r.height()/2;
                        for(int j = r.left()+2; j <= r.right()-3; j+=3) {
                            if(counter%2 == 0) {
                                renderDot(p, QPoint(j, center+1), pal.color(QPalette::Background));
                            } else {
                                renderDot(p, QPoint(j, center-2), pal.color(QPalette::Background));
                            }
                            counter++;
                        }

                    return;
                }

                case ToolBar::Separator:
                {
                    if(_drawToolBarItemSeparator) {
                        // TODO leftover from plastik, probably should be redone
                        p->setPen(_helper.calcDarkColor(pal.color(QPalette::Background)));
                        if(flags & State_Horizontal) {
                            int center = r.left()+r.width()/2;
                            p->drawLine( center-1, r.top()+3, center-1, r.bottom()-3 );
                        } else {
                            int center = r.top()+r.height()/2;
                            p->drawLine( r.x()+3, center-1, r.right()-3, center-1 );
                        }
                    }

                    return;
                }
            }
        }
        break;

        case WT_ToolButton:
        {
            switch (primitive)
            {
                case ToolButton::Panel:
                {
                    bool hasFocus = flags & State_HasFocus;

                    if((flags & State_Sunken) || (flags & State_On) )
                    {
                        renderHole(p, pal.color(QPalette::Window), r, hasFocus, mouseOver);
                    }
                    else if (hasFocus || mouseOver)
                    {
                        TileSet *tile;

                        if(mouseOver)
                            tile = _helper.slitFocused(_viewHoverBrush.brush(QPalette::Active).color());
                        else
                             tile = _helper.slitFocused(_viewFocusBrush.brush(QPalette::Active).color());
                       tile->render(r, p);
                    }
                    return;
                }
            }

        }
        break;

    }


    // Arrows
    if (primitive >= Generic::ArrowUp && primitive <= Generic::ArrowLeft) {
        QPolygonF a;

        switch (primitive) {
            case Generic::ArrowUp: {
                a.clear();
                a << QPointF( -3,2.5) << QPointF(0.5, -1.5) << QPointF(4,2.5);
                break;
            }
            case Generic::ArrowDown: {
                a.clear();
                a << QPointF( -3,-2.5) << QPointF(0.5, 1.5) << QPointF(4,-2.5);
              break;
            }
            case Generic::ArrowLeft: {
                a.clear();
                a << QPointF(2.5,-3) << QPointF(-1.5, 0.5) << QPointF(2.5,4);
                break;
            }
            case Generic::ArrowRight: {
                a.clear();
                a << QPointF(-2.5,-3) << QPointF(1.5, 0.5) << QPointF(-2.5,4);
                break;
            }
        }

        a.translate(int(r.x()+r.width()/2), int(r.y()+r.height()/2));
        KStyle::ColorOption* colorOpt   = extractOption<KStyle::ColorOption*>(kOpt);
        QColor               arrowColor = colorOpt->color.color(pal);

        QPen oldPen(p->pen()); // important to save the pen as combobox assumes we don't touch
        p->setPen(QPen(arrowColor, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p->setRenderHint(QPainter::Antialiasing);
        p->drawPolyline(a);
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setPen(oldPen);
        return;
    }

    switch (primitive)
    {
        case Generic::Frame:
        {
            // WT_Generic and other fallen-through frames...
            // QFrame, Qt item views, etc.: sunken..
            bool focusHighlight = flags&State_HasFocus/* && flags&State_Enabled*/;
            if (flags & State_Sunken) {
                // TODO use widget background role?
                renderHole(p, pal.color(QPalette::Window), r, focusHighlight);
            } else
                break; // do the default thing
        }

        case Generic::FocusIndicator:
            // we don't want the stippled focus indicator in oxygen
            return;

        default:
            break;
    }

    // default fallback
    KStyle::drawKStylePrimitive(widgetType, primitive, opt,
                                r, pal, flags, p, widget, kOpt);
}

void OxygenStyle::polish(QWidget* widget)
{
    switch (widget->windowFlags() & Qt::WindowType_Mask) {
        case Qt::Window:
        case Qt::Dialog:
        case Qt::Popup:
            widget->setAttribute(Qt::WA_StyledBackground);
        case Qt::Tool: // this we exclude as it is used for dragging of icons etc
        default:
            break;
    }

    if (qobject_cast<const QGroupBox*>(widget))
        widget->setAttribute(Qt::WA_StyledBackground);

    if( _animateProgressBar && qobject_cast<QProgressBar*>(widget) )
    {
        widget->installEventFilter(this);
        progAnimWidgets[widget] = 0;
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressBarDestroyed(QObject*)));
        if (!animationTimer->isActive()) {
            animationTimer->setSingleShot( false );
            animationTimer->start( 50 );
        }
    }

    if (qobject_cast<QPushButton*>(widget)
        || qobject_cast<QComboBox*>(widget)
        || qobject_cast<QAbstractSpinBox*>(widget)
        || qobject_cast<QCheckBox*>(widget)
        || qobject_cast<QRadioButton*>(widget)
        || qobject_cast<QTabBar*>(widget)
        || qobject_cast<QScrollBar*>(widget)
        || qobject_cast<QSlider*>(widget)
        || qobject_cast<QToolButton*>(widget)
        ) {
        widget->setAttribute(Qt::WA_Hover);
    }

    if (qobject_cast<QMenuBar*>(widget)
        || (widget && widget->inherits("Q3ToolBar"))
        || qobject_cast<QToolBar*>(widget)
        || (widget && qobject_cast<QToolBar *>(widget->parent())) )
    {
        widget->setBackgroundRole(QPalette::Background);
    }

    if (qobject_cast<QScrollBar*>(widget))
    {
        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    KStyle::polish(widget);
}

void OxygenStyle::unpolish(QWidget* widget)
{
    if ( qobject_cast<QProgressBar*>(widget) )
    {
        progAnimWidgets.remove(widget);
    }

    if (qobject_cast<QPushButton*>(widget)
        || qobject_cast<QComboBox*>(widget)
        || qobject_cast<QAbstractSpinBox*>(widget)
        || qobject_cast<QCheckBox*>(widget)
        || qobject_cast<QRadioButton*>(widget)
        || qobject_cast<QScrollBar*>(widget)
        || qobject_cast<QSlider*>(widget)
    ) {
        widget->setAttribute(Qt::WA_Hover, false);
    }

    if (qobject_cast<QMenuBar*>(widget)
        || (widget && widget->inherits("Q3ToolBar"))
        || qobject_cast<QToolBar*>(widget)
        || (widget && qobject_cast<QToolBar *>(widget->parent())) )
    {
        widget->setBackgroundRole(QPalette::Button);
    }

    if (qobject_cast<QScrollBar*>(widget))
    {
        widget->setAttribute(Qt::WA_OpaquePaintEvent);
    }

    KStyle::unpolish(widget);
}

void OxygenStyle::progressBarDestroyed(QObject* obj)
{
    progAnimWidgets.remove(static_cast<QWidget*>(obj));
}

void OxygenStyle::globalSettingsChange(int type, int arg)
{
    if (type == KGlobalSettings::PaletteChanged) {
        _helper.reloadConfig();
        _viewFocusBrush = KStatefulBrush( KColorScheme::View, KColorScheme::FocusColor, _config );
        _viewHoverBrush = KStatefulBrush( KColorScheme::View, KColorScheme::HoverColor, _config );
    }
}

void OxygenStyle::renderSlab(QPainter *p, const QRect &r, const QColor &color, StyleOptions opts, TileSet::Tiles tiles) const
{
    if ((r.width() <= 0) || (r.height() <= 0))
        return;

    TileSet *tile;

    // fill
    if (!(opts & NoFill))
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);

        QLinearGradient innerGradient(0, r.top() - r.height(), 0, r.bottom());
        innerGradient.setColorAt(0.0, _helper.calcLightColor(color)); //KColorUtils::shade(calcLightColor(color), shade));
        innerGradient.setColorAt(1.0, color);
        p->setBrush(innerGradient);
        _helper.fillSlab(*p, r);

        p->restore();
    }

    // edges
    // for slabs, hover takes precedence over focus (other way around for holes)
    // but in any case if the button is sunken we don't show focus nor hover
    if (opts & Sunken)
        tile = _helper.slabSunken(color, 0.0);
    else if (opts & Hover)
        tile = _helper.slabFocused(color, _viewHoverBrush.brush(QPalette::Active).color(), 0.0); // FIXME need state
    else if (opts & Focus)
        tile = _helper.slabFocused(color, _viewFocusBrush.brush(QPalette::Active).color(), 0.0); // FIXME need state
    else
    {
        tile = _helper.slab(color, 0.0);
        tile->render(r, p, tiles);
        return;
    }
    tile->render(r, p, tiles);
}

void OxygenStyle::renderHole(QPainter *p, const QColor &base, const QRect &r, bool focus, bool hover, TileSet::Tiles posFlags) const
{
    if((r.width() <= 0)||(r.height() <= 0))
        return;

    TileSet *tile;
    // for holes, focus takes precedence over hover (other way around for buttons)
    if (focus)
        tile = _helper.holeFocused(base, _viewFocusBrush.brush(QPalette::Active).color(), 0.0); // FIXME need state
    else if (hover)
        tile = _helper.holeFocused(base, _viewHoverBrush.brush(QPalette::Active).color(), 0.0); // FIXME need state
    else
        tile = _helper.hole(base, 0.0);
    tile->render(r, p, posFlags);
}

// TODO take StyleOptions instead of ugly bools
void OxygenStyle::renderCheckBox(QPainter *p, const QRect &rect, const QPalette &pal,
                                 bool enabled, bool hasFocus, bool mouseOver, int primitive,
                                 bool sunken) const
{
    int s = qMin(rect.width(), rect.height());
    QRect r = centerRect(rect, s, s);

    StyleOptions opts;
    if (hasFocus) opts |= Focus;
    if (mouseOver) opts |= Hover;

    if(sunken)
    {
        QColor color = pal.color(QPalette::Window);

        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(Qt::NoPen);

        p->setBrush(color);
        _helper.fillHole(*p, r.adjusted(-2,-2,2,2));

        p->restore();

        _helper.holeFlat(color, 0.0)->render(r, p);
    }
    else
    {
        renderSlab(p, r, pal.color(QPalette::Button), opts);
    }

    // check mark
    double x = r.center().x() - 3.5, y = r.center().y() - 2.5;

    if (primitive != CheckBox::CheckOff)
    {
        QBrush brush = _helper.decoGradient(rect.adjusted(2,2,-2,-2), pal.color(QPalette::ButtonText));
        QPen pen(brush, 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        pen.setCapStyle(Qt::RoundCap);
        if (primitive == CheckBox::CheckTriState) {
#if 1 // check
            QVector<qreal> dashes;
            dashes << 1.0 << 2.0;
            pen.setWidthF(1.3);
            pen.setDashPattern(dashes);
#else // 'X'
            QVector<qreal> dashes;
            dashes << 0.4 << 2.0;
            pen.setDashPattern(dashes);
#endif
        }

        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(pen);
#if 1 // check
        p->drawLine(QPointF(x+9, y), QPointF(x+3,y+7));
        p->drawLine(QPointF(x, y+4), QPointF(x+3,y+7));
#else // 'X'
        p->drawLine(QPointF(x+7, y), QPointF(x+1,y+6));
        p->drawLine(QPointF(x+7, y+6), QPointF(x+1,y));
#endif
        p->setRenderHint(QPainter::Antialiasing, false);
    }
}

void OxygenStyle::renderRadioButton(QPainter *p, const QRect &r, const QPalette &pal,
                                        bool enabled, bool mouseOver, int prim,
                                   bool drawButton) const
{
    QRect r2(r.x() + (r.width()-21)/2, r.y() + (r.height()-21)/2, 21, 21);
    int x = r2.x();
    int y = r2.y();

    // TODO focus?
    if(mouseOver)
    {
        QPixmap slabPixmap = _helper.roundSlabFocused(pal.color(QPalette::Button),_viewHoverBrush.brush(QPalette::Active).color(), 0.0);
        if(drawButton)
            p->drawPixmap(x, y, slabPixmap);
    }
    else
    {
        QPixmap slabPixmap = _helper.roundSlab(pal.color(QPalette::Button), 0.0);
        if(drawButton)
            p->drawPixmap(x, y, slabPixmap);
    }

    // draw the radio mark
    switch (prim)
    {
        case RadioButton::RadioOn:
        {
            const double radius = 3.0;
            double dx = r2.width() * 0.5 - radius;
            double dy = r2.height() * 0.5 - radius;
            p->save();
            p->setRenderHints(QPainter::Antialiasing);
            p->setPen(Qt::NoPen);
            p->setBrush(_helper.decoGradient(r2.adjusted(2,2,-2,-2), pal.color(QPalette::ButtonText)));
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
    p->setRenderHint(QPainter::Antialiasing);
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(0, 0, 0, 66));
    p->drawEllipse(QRectF(point.x()-diameter/2+0.5, point.y()-diameter/2+0.5, diameter, diameter));
    p->setRenderHint(QPainter::Antialiasing, false);
}

void OxygenStyle::renderTab(QPainter *p,
                            const QRect &r,
                            const QPalette &pal,
                            bool mouseOver,
                            const bool selected,
                            const bool bottom,
                            const QStyleOptionTabV2 *tabOpt,
                            const bool reverseLayout) const
{
    const QStyleOptionTab::TabPosition pos = tabOpt->position;
    const bool leftCornerWidget = reverseLayout ?
                            (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget) :
                            (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget);
    const bool rightCornerWidget = reverseLayout ?
                            (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget) :
                            (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget);
    const bool isFirst = pos == QStyleOptionTab::Beginning || pos == QStyleOptionTab::OnlyOneTab/* (pos == First) || (pos == Single)*/;
    const bool isLast = pos == QStyleOptionTab::End /*(pos == Last)*/;
    const bool isSingle = pos == QStyleOptionTab::OnlyOneTab /*(pos == Single)*/;
    const bool isLeftOfSelected =  reverseLayout ?
                            (tabOpt->selectedPosition == QStyleOptionTab::PreviousIsSelected) :
                            (tabOpt->selectedPosition == QStyleOptionTab::NextIsSelected);
    const bool isRightOfSelected =  reverseLayout ?
                            (tabOpt->selectedPosition == QStyleOptionTab::NextIsSelected) :
                            (tabOpt->selectedPosition == QStyleOptionTab::PreviousIsSelected);
    const bool isLeftMost =  (reverseLayout ?
                            (tabOpt->position == QStyleOptionTab::End) :
                            (tabOpt->position == QStyleOptionTab::Beginning)) ||
                                tabOpt->position == QStyleOptionTab::OnlyOneTab;
    const bool isRightMost =  reverseLayout ?
                            (tabOpt->position == QStyleOptionTab::Beginning) :
                            (tabOpt->position == QStyleOptionTab::End) ||
                                tabOpt->position == QStyleOptionTab::OnlyOneTab;
    const bool isFrameAligned =  reverseLayout ?
                            (isRightMost && ! (tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget)) :
                            (isLeftMost && ! (tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget));

    // the tab part of the tab - ie subtracted the fairing to the frame
    QRect Rc = bottom ? r.adjusted(0,6,0,0) : r.adjusted(0,0,0,-7);

    // the area where the fairing should appear
    const QRect Rb(r.x(), bottom?r.top():Rc.bottom()+1, r.width(), r.height()-Rc.height() );

    // FIXME - maybe going to redo tabs, also are broken ATM
    if (selected) {
        if(bottom)
            renderSlab(p, Rc.adjusted(0,-7,0,0), pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Left | TileSet::Right);
        else
            renderSlab(p, Rc.adjusted(0,0,0,7), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left | TileSet::Right);

        // some "position specific" paintings...
        // First draw the left connection from the panel border to the tab
        if(isFirst && !reverseLayout && !leftCornerWidget) {
            renderSlab(p, Rb.adjusted(0,-7,0,7), pal.color(QPalette::Window), NoFill, TileSet::Left);
        } else {
//            renderHole(p, QRect(Rb.left(), Rb.top(),4,5), false, false, TileSet::Right | TileSet::Bottom);
            TileSet *tile = _helper.slabInverted(pal.color(QPalette::Window), 0.0);
            if(bottom)
                tile->render(QRect(Rb.left()-5, Rb.top()-1,12,13), p, TileSet::Right | TileSet::Top);
            else
                tile->render(QRect(Rb.left()-5, Rb.top()-5,12,12), p, TileSet::Right | TileSet::Bottom);
        }

        // Now draw the right connection from the panel border to the tab
        if(isFirst && reverseLayout && !rightCornerWidget) {
            renderSlab(p, Rb.adjusted(0,-7,0,7), pal.color(QPalette::Window), NoFill, TileSet::Right);
        } else {
            TileSet *tile = _helper.slabInverted(pal.color(QPalette::Window), 0.0);
            //renderHole(p, QRect(Rb.right()-3, Rb.top(),3,5), false, false, TileSet::Left | TileSet::Bottom);
            if(bottom)
                tile->render(QRect(Rb.right()-6, Rb.top()-1,12,13), p, TileSet::Left | TileSet::Top);
            else
                tile->render(QRect(Rb.right()-6, Rb.top()-5,12,12), p, TileSet::Left | TileSet::Bottom);
        }
    } else {
        // inactive tabs
        int x,y,w,h;
        r.adjusted(0,4,0,0).getRect(&x, &y, &w, &h);
        p->setPen(QColor(0,0,0, 30));
        if (!bottom) {
            if(isLeftMost) {
                p->drawArc(QRectF(x+2.5, y+0.5, 9.5, 9.5),90*16, 90*16);
                if(isFrameAligned)
                    p->drawLine(QPointF(x+2.5, y+6.3), QPointF(x+2.5, y+h-0.5));
                else
                    p->drawLine(QPointF(x+0.5, y+6.3), QPointF(x+0.5, y+h-6.3));
                p->drawLine(QPointF(x+8.8, y+0.5), QPointF(x+w+2.5, y+0.5));
                if(!isLeftOfSelected)
                    p->drawLine(QPointF(x+w-0.5, y+1.5), QPointF(x+w-0.5, y+h-6.3));
                p->fillRect(x-0.5, y+1.5, w-1 , h-5,QColor(0,0,0,10));
            } else  if(isRightMost) {
                p->drawArc(QRectF(x+w-9.5-2.5, y+0.5, 9.5, 9.5), 0, 90*16);
                if(isFrameAligned)
                    p->drawLine(QPointF(x+w-2.5, y+6.3), QPointF(x+w-2.5, y+h+0.5));
                else
                    p->drawLine(QPointF(x+w-2.5, y+6.3), QPointF(x+w-2.5, y+h-6.3));
                p->drawLine(QPointF(isRightOfSelected ? x -0.5 : x+3.5+1, y+0.5), QPointF(x+w-8.8, y+0.5));
                p->fillRect(x-0.5, y+1.5, w-1 , h-5,QColor(0,0,0,10));
            } else {
                p->drawLine(QPointF(isRightOfSelected ? x -0.5 : x+3.5+1, y+0.5), QPointF(x+w+2.5, y+0.5));
                if(!isLeftOfSelected)
                    p->drawLine(QPointF(x+w-0.5, y+1.5), QPointF(x+w-0.5, y+h-6.3));
                p->fillRect(x-0.5, y+1.5, w-1 , h-5,QColor(0,0,0,10));
            }
        }
        TileSet::Tiles posFlag = bottom?TileSet::Bottom:TileSet::Top;
        QRect Ractual(Rb.left(), Rb.y(), Rb.width(), 6);

        if(isLeftMost && isFrameAligned)
            posFlag |= TileSet::Left;
        else
            Ractual.adjust(-7,0,0,0); // fixme: hovering rightmost tab causes green artifact

        if(isRightMost && isFrameAligned)
            posFlag |= TileSet::Right;
        else
            Ractual.adjust(0,0,7,0); // fixme: hovering rightmost tab causes green artifact

        if (mouseOver)
            renderSlab(p, Ractual, pal.color(QPalette::Window), NoFill| Hover, posFlag);
        else
            renderSlab(p, Ractual, pal.color(QPalette::Window), NoFill, posFlag);

        // TODO mouseover effects
    }
}

int OxygenStyle::styleHint(StyleHint hint, const QStyleOption * option,
                            const QWidget * widget, QStyleHintReturn * returnData) const
{
    switch (hint) {
        case SH_Menu_SubMenuPopupDelay:
            return 96; // Motif-like delay...

        case SH_ScrollView_FrameOnlyAroundContents:
            return true;

        case SH_ItemView_ShowDecorationSelected:
            return true;

        default:
            return KStyle::styleHint(hint, option, widget, returnData);
    }
}

int OxygenStyle::pixelMetric(PixelMetric m, const QStyleOption *opt, const QWidget *widget) const
{
    switch(m) {
        case PM_DefaultTopLevelMargin:
            return 11;

        case PM_DefaultChildMargin:
            return 4; // qcommon is 9;

        case PM_DefaultLayoutSpacing:
            return 4; // qcommon is 6

        case PM_DefaultFrameWidth:
            if (qobject_cast<const QLineEdit*>(widget))
                return 5;
            //else fall through
        default:
            return KStyle::pixelMetric(m,opt,widget);
    }
}


QSize OxygenStyle::sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget) const
{
    switch(type)
    {
        case CT_ToolButton:
        {
            // We want to avoid super-skiny buttons, for things like "up" when icons + text
            // For this, we would like to make width >= height.
            // However, once we get here, QToolButton may have already put in the menu area
            // (PM_MenuButtonIndicator) into the width. So we may have to take it out, fix things
            // up, and add it back in. So much for class-independent rendering...
            QSize size = contentsSize;

            if (const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option)) {
                if ((!tbOpt->icon.isNull()) && (!tbOpt->text.isEmpty()) && tbOpt->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                    size.setHeight(size.height()-9);
            }
            return KStyle::sizeFromContents(type, option, size, widget);
        }
        default:
            break;
    }
    return KStyle::sizeFromContents(type, option, contentsSize, widget);
}


QRect OxygenStyle::subControlRect(ComplexControl control, const QStyleOptionComplex* option,
                                SubControl subControl, const QWidget* widget) const
{
    QRect r = option->rect;

    switch (control)
    {
        case CC_GroupBox:
            switch (subControl)
            {
                case SC_GroupBoxFrame:
                    return r;

                case SC_GroupBoxLabel:
                {
                    if (const QStyleOptionGroupBox *gbOpt = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
                        QFontMetrics fontMetrics = gbOpt->fontMetrics;
                        int h = fontMetrics.height();
                        int tw = fontMetrics.size(Qt::TextShowMnemonic, gbOpt->text + QLatin1String("  ")).width();
                        r.setHeight(h);
                        r.moveTop(8);

                        return alignedRect(gbOpt->direction, Qt::AlignHCenter, QSize(tw, h), r);
                    }
                }

                default:
                    break;
            }
        default:
            break;
    }

    return KStyle::subControlRect(control, option, subControl, widget);
}

bool OxygenStyle::eventFilter(QObject *obj, QEvent *ev)
{
    if (KStyle::eventFilter(obj, ev) )
        return true;

    // Track show events for progress bars
    if ( _animateProgressBar && qobject_cast<QProgressBar*>(obj) )
    {
        if ((ev->type() == QEvent::Show) && !animationTimer->isActive())
        {
            animationTimer->start( 50 );
        }
    }

    return false;
}

QIcon OxygenStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
                                               const QWidget *widget) const
{
    // get button color (unfortunately option and widget might not be set)
    QColor buttonColor;
    if (option)
        buttonColor = option->palette.button().color();
    else if (widget)
        buttonColor = widget->palette().button().color();
    else if (qApp) // might not have a QApplication
        buttonColor = qApp->palette().button().color();
    else // KCS is always safe
        buttonColor = KColorScheme(QPalette::Active, KColorScheme::Button,
                                   _config).background().color();

    switch (standardIcon) {
        case SP_TitleBarNormalButton:
        {
            QPixmap pm = _helper.windecoButton(buttonColor,6);
            QPainter painter(&pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg(0, 6, 0, 12);
            lg.setColorAt(0.45, QColor(0,0,0,150));
            lg.setColorAt(0.80, QColor(0,0,0,80));
            painter.setPen(QPen(lg,2));
            painter.setBrush(lg);
            QPoint points[4] = {QPoint(9, 6), QPoint(12, 9), QPoint(9, 12), QPoint(6, 9)};
            painter.drawPolygon(points, 4);

            return QIcon(pm);
        }

        case SP_TitleBarCloseButton:
        case SP_DockWidgetCloseButton:
        {
            QPixmap pm = _helper.windecoButton(buttonColor,6);
            QPainter painter(&pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg(0, 6, 0, 12);
            lg.setColorAt(0.45, QColor(0,0,0,150));
            lg.setColorAt(0.80, QColor(0,0,0,80));
            painter.setPen(QPen(lg,2));
            painter.drawLine(6,6,12,12);
            painter.drawLine(12,6,6,12);

            return QIcon(pm);
        }
        default:
            return KStyle::standardPixmap(standardIcon, option, widget);
    }
}
// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
