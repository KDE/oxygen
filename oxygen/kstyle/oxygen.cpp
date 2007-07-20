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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QToolBar>
#include <QtGui/QScrollBar>

#include <KGlobal>
#include <KSharedConfig>
#include <KColorUtils>
#include <KColorScheme>

#include "helper.h"
#include "tileset.h"

K_EXPORT_STYLE("Oxygen", OxygenStyle)

K_GLOBAL_STATIC_WITH_ARGS(OxygenStyleHelper, globalHelper, ("OxygenStyle"))

// some bitmaps for the radio button so it's easier to handle the circle stuff...
// 13x13
static const unsigned char radiobutton_mask_bits[] = {
   0xf8, 0x03, 0xfc, 0x07, 0xfe, 0x0f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f,
   0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xff, 0x1f, 0xfe, 0x0f, 0xfc, 0x07,
   0xf8, 0x03};
static const unsigned char radiobutton_contour_bits[] = {
   0xf0, 0x01, 0x0c, 0x06, 0x02, 0x08, 0x02, 0x08, 0x01, 0x10, 0x01, 0x10,
   0x01, 0x10, 0x01, 0x10, 0x01, 0x10, 0x02, 0x08, 0x02, 0x08, 0x0c, 0x06,
   0xf0, 0x01};
static const unsigned char radiobutton_aa_inside_bits[] = {
   0x00, 0x00, 0x10, 0x01, 0x04, 0x04, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x02, 0x08, 0x00, 0x00, 0x04, 0x04, 0x10, 0x01,
   0x00, 0x00};
static const unsigned char radiobutton_aa_outside_bits[] = {
   0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00,
   0x08, 0x02};
static const unsigned char radiobutton_highlight1_bits[] = {
   0x00, 0x00, 0xf0, 0x01, 0x1c, 0x07, 0x04, 0x04, 0x06, 0x0c, 0x02, 0x08,
   0x02, 0x08, 0x02, 0x08, 0x06, 0x0c, 0x04, 0x04, 0x1c, 0x07, 0xf0, 0x01,
   0x00, 0x00};
static const unsigned char radiobutton_highlight2_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x18, 0x03, 0x08, 0x02, 0x04, 0x04,
   0x04, 0x04, 0x04, 0x04, 0x08, 0x02, 0x18, 0x03, 0xe0, 0x00, 0x00, 0x00,
   0x00, 0x00};
// check mark
const uint CHECKMARKSIZE = 9; // 9x9
static const unsigned char checkmark_aa_bits[] = {
   0x45, 0x01, 0x28, 0x00, 0x11, 0x01, 0x82, 0x00, 0x44, 0x00, 0x82, 0x00,
   0x11, 0x01, 0x28, 0x00, 0x45, 0x01};
static const unsigned char checkmark_dark_bits[] = {
   0x82, 0x00, 0x45, 0x01, 0xaa, 0x00, 0x54, 0x00, 0x28, 0x00, 0x74, 0x00,
   0xea, 0x00, 0xc5, 0x01, 0x82, 0x00};
static const unsigned char checkmark_light_bits[] = {
   0x00, 0xfe, 0x82, 0xfe, 0x44, 0xfe, 0x28, 0xfe, 0x10, 0xfe, 0x08, 0xfe,
   0x04, 0xfe, 0x02, 0xfe, 0x00, 0xfe};
static const unsigned char checkmark_tristate_bits[] = {
   0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01,
   0x00, 0x00, 0xff, 0x01, 0x00, 0x00};
// radio mark
const uint RADIOMARKSIZE = 9; // 9x9
static const unsigned char radiomark_aa_bits[] = {
   0x00, 0x00, 0x44, 0x00, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x82, 0x00, 0x44, 0x00, 0x00, 0x00};
static const unsigned char radiomark_dark_bits[] = {
   0x00, 0x00, 0x38, 0x00, 0x44, 0x00, 0xf2, 0x00, 0xfa, 0x00, 0xfa, 0x00,
   0x7c, 0x00, 0x38, 0x00, 0x00, 0x00};
static const unsigned char radiomark_light_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x0c, 0x00, 0x04, 0x00, 0x04, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


OxygenStyle::OxygenStyle() :
//     kickerMode(false),
//     kornMode(false),
    flatMode(false),
    _helper(*globalHelper)
{
    _config = _helper.config();

    setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth, 2);

    // TODO: change this when double buttons are implemented
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleBotButton, true);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::MinimumSliderHeight, 21);

    setWidgetLayoutProp(WT_PushButton, PushButton::DefaultIndicatorMargin, 1);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Left, 4);
    setWidgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Right, 4);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin, 3);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Left, 2);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Right, 2);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Top, 2);
    setWidgetLayoutProp(WT_PushButton, PushButton::FocusMargin + Bot, 2);
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftHorizontal, 1);
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftVertical,   1);

    setWidgetLayoutProp(WT_Splitter, Splitter::Width, 6);

    setWidgetLayoutProp(WT_CheckBox, CheckBox::Size, 13);
    setWidgetLayoutProp(WT_RadioButton, RadioButton::Size, 13);

    setWidgetLayoutProp(WT_MenuBar, MenuBar::ItemSpacing, 6);

    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin, 3);
    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin+Left, 3);
    setWidgetLayoutProp(WT_MenuBarItem, MenuBarItem::Margin+Right, 3);

    setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckAlongsideIcon, 1);
    setWidgetLayoutProp(WT_MenuItem, MenuItem::CheckWidth, 13);

    setWidgetLayoutProp(WT_ProgressBar, ProgressBar::BusyIndicatorSize, 10);

    setWidgetLayoutProp(WT_TabBar, TabBar::TabOverlap, 1);

    setWidgetLayoutProp(WT_TabWidget, TabWidget::ContentsMargin, 2);

    setWidgetLayoutProp(WT_Slider, Slider::HandleThickness, 20/*15*/);
    setWidgetLayoutProp(WT_Slider, Slider::HandleLength, 11);

    setWidgetLayoutProp(WT_SpinBox, SpinBox::FrameWidth, 2);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonWidth, 2+16+1);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonSpacing, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Left, 0);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Right, 3);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Top, 3);
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ButtonMargin+Bot, 3);

    setWidgetLayoutProp(WT_ComboBox, ComboBox::FrameWidth, 2);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonWidth, 2+16+1);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Left, 0);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Right, 3);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Top, 3);
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ButtonMargin+Bot, 3);

    setWidgetLayoutProp(WT_ToolBar, ToolBar::FrameWidth, 0);
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemSpacing, 1);
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemMargin, 0);

    setWidgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin, 4);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::FocusMargin,    3);

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
    _customCheckMarkColor = settings.value("/customCheckMarkColor", false).toBool();
    _checkMarkColor.setNamedColor( settings.value("/checkMarkColor", "black").toString() );
    // FIXME below this line to be deleted (and can we not use QSettings? KConfig* is safe now)
    _customOverHighlightColor = true;
    _customFocusHighlightColor = true;
    // do next two lines in polish()?
    KColorScheme schemeView( KColorScheme::View, _config );
    _viewHoverColor = _overHighlightColor = schemeView.decoration( KColorScheme::HoverColor ).color();
    _viewFocusColor = _focusHighlightColor = schemeView.decoration( KColorScheme::FocusColor ).color();
    settings.endGroup();

    // setup pixmap cache...
    pixmapCache = new QCache<int, CacheEntry>(327680);

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
            iter.value() = (iter.value() + 1) % 20;
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
    delete pixmapCache;
}


void OxygenStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{
    switch (element)
    {
        case PE_Widget:
        {
            if (!widget || !widget->isWindow())
                return;

            QColor color = option->palette.color(widget->backgroundRole());
            int splitY = qMin(300, 3*option->rect.height()/4);

            QRect upperRect = QRect(0, 0, option->rect.width(), splitY);
            QPixmap tile = _helper.verticalGradient(color, splitY);
            painter->drawTiledPixmap(upperRect, tile);

            QRect lowerRect = QRect(0,splitY, option->rect.width(), option->rect.height() - splitY);
            painter->fillRect(lowerRect, _helper.backgroundBottomColor(color));

            int radialW = qMin(600, option->rect.width());
            tile = _helper.radialGradient(color, radialW);
            QRect radialRect = QRect((option->rect.width() - radialW) / 2, 0, radialW, 64);
            painter->drawPixmap(radialRect, tile);
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
                    bool sunken   = (flags & State_On) || (flags & State_Sunken);

                    renderButton(p, r, pal, sunken,
                                 mouseOver/*,
                                         bool horizontal,
                                         bool enabled,
                                         bool khtmlMode*/);

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
                    bool sunken   = (flags & State_On) || (flags & State_Sunken) || (flags & State_Selected);

                    renderButton(p, r, pal, sunken, mouseOver);

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
                    QColor bg = enabled?pal.color(QPalette::Base):pal.color(QPalette::Background); // background

                    p->setPen(bg.dark(105) );
                    p->drawLine(r.left()+2, r.top()+1, r.right()-2, r.top()+1 );
                    p->drawLine(r.left()+1, r.top()+2, r.left()+1, r.bottom()-2);
                    p->setPen(bg.light(105) );
                    p->drawLine(r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1 );
                    p->drawLine(r.right()-1, r.top()+2, r.right()-1, r.bottom()-2);

            // fill background
                    p->fillRect(r.adjusted(2,2,-2,-2), bg );

                    return;
                }

                case ProgressBar::BusyIndicator:
                {
                    renderSurface(p, QRect( r.x()+/*progress+*/1, r.y()+1, r.width()-2, r.height()-2 ),
                                  bg, fg, pal.color(QPalette::Highlight),
                                  2*(_contrast/3),
                                  Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                          Round_UpperRight|Round_BottomRight|
                                          Round_UpperLeft|Round_BottomLeft|Is_Horizontal);

                    return;
                }

                case ProgressBar::Indicator:
                {
                    QRect Rcontour = r;
                    QRect Rsurface(Rcontour.left()+1, Rcontour.top()+1, Rcontour.width()-2, Rcontour.height()-2);

                    QRegion mask(Rsurface);
                    if(reverseLayout) {
                        mask -= QRegion(Rsurface.left(), Rsurface.top(), 1, 1);
                        mask -= QRegion(Rsurface.left(), Rsurface.bottom(), 1, 1);
                    } else {
                        mask -= QRegion(Rsurface.right(), Rsurface.top(), 1, 1);
                        mask -= QRegion(Rsurface.right(), Rsurface.bottom(), 1, 1);
                    }
                    p->setClipRegion(mask);
                    int counter = 0;
                    QPixmap surfaceTile(21, r.height()-2);
                    QPainter surfacePainter(&surfaceTile);
                    // - 21 pixel -
                    //  __________
                    // |    `    `| <- 3
                    // | 1   | 2  |
                    // |____,____,| <- 3
                    // 1 = light, 11 pixel, 1 pixel overlapping with 2
                    // 2 = dark, 11 pixel, 1 pixel overlapping with 3
                    // 3 = light edges
                    const int tileHeight = surfaceTile.height();
                    // 3
                    renderSurface(&surfacePainter,
                                    QRect(20, 0, 11, tileHeight),
                                    fg.light(105), fg, pal.color(QPalette::Highlight), 2*(_contrast/3),
                                    reverseLayout ? Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                            Round_UpperLeft|Round_BottomLeft|Is_Horizontal
                                    : Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                            Round_UpperRight|Round_BottomRight|Is_Horizontal);
                    // 2
                    renderSurface(&surfacePainter,
                                    QRect(10, 0, 11, tileHeight),
                                    fg, fg.light(105), pal.color(QPalette::Highlight), 2*(_contrast/3),
                                    reverseLayout ? Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                            Round_UpperLeft|Round_BottomLeft|Is_Horizontal
                                    : Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                            Round_UpperRight|Round_BottomRight|Is_Horizontal);
                    // 1
                    renderSurface(&surfacePainter,
                                    QRect(0, 0, 11, tileHeight),
                                    fg.light(105), fg, pal.color(QPalette::Highlight), 2*(_contrast/3),
                                    reverseLayout ? Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                            Round_UpperLeft|Round_BottomLeft|Is_Horizontal
                                    : Draw_Right|Draw_Left|Draw_Top|Draw_Bottom|
                                            Round_UpperRight|Round_BottomRight|Is_Horizontal);

                    surfacePainter.end();
                    int staticShift = 0;
                    int animShift = 0;
                    if (!_animateProgressBar) {
                        staticShift = (reverseLayout ? Rsurface.left() : Rsurface.right()) % 40 - 40;
                    } else {
                        // find the animation Offset for the current Widget
                        QWidget* nonConstWidget = const_cast<QWidget*>(widget);
                        QMap<QWidget*, int>::const_iterator iter = progAnimWidgets.find(nonConstWidget);
                        if (iter != progAnimWidgets.end())
                            animShift = iter.value();
                    }
                    while((counter*10) < (Rsurface.width()+20)) {
                        counter++;
                        if (reverseLayout) {
                            // from right to left, overlap 1 pixel with the previously drawn tile
                            p->drawPixmap(Rsurface.right()-counter*20-animShift+40+staticShift, r.top()+1,
                                        surfaceTile);
                        } else {
                            // from left to right, overlap 1 pixel with the previously drawn tile
                            p->drawPixmap(Rsurface.left()+counter*20+animShift-40+staticShift, r.top()+1,
                                        surfaceTile);
                        }
                    }

                    p->setClipping(false);

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
                    bool down = flags & State_Sunken;

                    if (active && focused) {
                        renderButton(p, r, pal, down, mouseOver, true);
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
                        renderSurface(p, r, pal.color(QPalette::Background), pal.color(QPalette::Highlight), pal.color(QPalette::Highlight),
                                _contrast+3, Draw_Top|Draw_Bottom|Is_Horizontal);
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
                    renderCheckBox(p, r, pal, enabled, mouseOver, CheckBox::CheckOn);
                    return;
                }

                case MenuItem::CheckOff:
                {
                    renderCheckBox(p, r, pal, enabled, mouseOver, CheckBox::CheckOff);
                    return;
                }

                case MenuItem::RadioOn:
                {
                    renderRadioButton(p, r, pal, enabled, mouseOver, RadioButton::RadioOn);
                    return;
                }

                case MenuItem::RadioOff:
                {
                    renderRadioButton(p, r, pal, enabled, mouseOver, RadioButton::RadioOff);
                    return;
                }

                case MenuItem::CheckIcon:
                {
                    // TODO
                    renderButton(p, r, pal, true /*sunken*/);
                    return;
                }
            }
        }
        break;

        case WT_DockWidget:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    //FIXME CBRrenderContour(p, r, pal.color( QPalette::Background ),                                  pal.color( QPalette::Background ).dark(160),                                  Draw_Left|Draw_Right|Draw_Top|Draw_Bottom);

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
                    renderCheckBox(p, r, pal, enabled, mouseOver, primitive);
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
                {
                    renderHole(p, QRect(r.left(),0, 3, r.height()), false,false, TileSet::Top | TileSet::Right | TileSet::Bottom);
                    return;
                }
                break;

                case ScrollBar::DoubleButtonVert:
                {
                    renderHole(p, QRect(0,r.top(), r.width(), 3), false,false, TileSet::Bottom | TileSet::Left | TileSet::Right);
                    return;
                }
                break;

                case ScrollBar::SingleButtonHor:
                {
                    renderHole(p, QRect(r.right()-2,0, 3, r.height()), false,false, TileSet::Top | TileSet::Left | TileSet::Bottom);
                    return;
                }
                break;

                case ScrollBar::SingleButtonVert:
                {
                    renderHole(p, QRect(0,r.bottom()-2, r.width(), 3), false,false, TileSet::Top | TileSet::Left | TileSet::Right);
                    return;
                }
                break;

                case ScrollBar::GrooveAreaVert:
                {
                    renderHole(p, r, false,false, TileSet::Left | TileSet::Right);
                    return;
                }
                case ScrollBar::GrooveAreaHor:
                {
                    renderHole(p, r, false,false, TileSet::Top | TileSet::Bottom);
                    return;
                }

                case ScrollBar::SliderVert:
                {
                    renderHole(p, r, false,false, TileSet::Left | TileSet::Right);
                    _helper.verticalScrollBar(QColor(0,116,0), r.adjusted(1,0,-1,0))->render(r.adjusted(1,0,-1,0), p, TileSet::Full);
                    return;
                }

                case ScrollBar::SliderHor:
                {
                    renderHole(p, r, false,false, TileSet::Top | TileSet::Bottom);
                    p->fillRect(r.adjusted(2, 2, -2, -2), QColor(Qt::red));
                    return;
                }

            }

        }
        break;

        case WT_TabBar:
        {
            const QStyleOptionTab* tabOpt = qstyleoption_cast<const QStyleOptionTab*>(opt);
            if (!tabOpt) break;

            switch (primitive)
            {
                case TabBar::NorthTab:
                case TabBar::SouthTab:
                {
                    QStyleOptionTab::TabPosition pos = tabOpt->position;
                    bool bottom = primitive == TabBar::SouthTab;
                    bool cornerWidget = reverseLayout ?
                            (tabOpt->cornerWidgets&QStyleOptionTab::LeftCornerWidget) :
                            (tabOpt->cornerWidgets&QStyleOptionTab::RightCornerWidget);

                    // TODO kstyle helper for triangular tabs...? TabOpt...?
                    bool triangular = (tabOpt->shape==QTabBar::TriangularNorth) ||
                            (tabOpt->shape==QTabBar::TriangularSouth);

                    // TODO: tab painting needs a lot of work in order to handle east and west tabs.
                    renderTab(p, r, pal, mouseOver, flags&State_Selected, bottom, pos, triangular, cornerWidget, reverseLayout);

                    return;
                }

                // TODO: TabBar::EastTab, TabBar::WestTab, TabBar::BaseFrame, TabBar::ScrollButton

            }

        }
        break;

        case WT_TabWidget:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    renderPanel(p, r, pal, true, flags&State_Sunken);
                    return;
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
                    renderPanel(p, r, pal, true, flags&State_Sunken);
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
                        renderDot(p, QPointF(r.left()+3, center-5), color);
                        renderDot(p, QPointF(r.left()+3, center), color);
                        renderDot(p, QPointF(r.left()+3, center+5), color);
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
                        renderDot(p, QPointF(center-5, r.top()+3), color);
                        renderDot(p, QPointF(center, r.top()+3), color);
                        renderDot(p, QPointF(center+5, r.top()+3), color);
                    }
                    return;
                }
            }
        }
        break;

        case WT_Slider:
        {
            switch (primitive)
            {
                case Slider::HandleHor:
                case Slider::HandleVert:
                {

                    bool horizontal = primitive == Slider::HandleHor;

                    const bool pressed = (flags&State_Sunken);
                    const WidgetState s = enabled?(pressed?IsPressed:IsEnabled):IsDisabled;
                    const QColor contour = getColor(pal,DragButtonContour,s),
                                surface = getColor(pal,DragButtonSurface,s);

                    int xcenter = (r.left()+r.right()) / 2;
                    int ycenter = (r.top()+r.bottom()) / 2;

                    if (horizontal) {
                        // manual contour: vertex
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 50) );
                        p->drawPoint(xcenter-5+1, ycenter+4);
                        p->drawPoint(xcenter+5-1, ycenter+4);
                        p->drawPoint(xcenter-5+2, ycenter+5);
                        p->drawPoint(xcenter+5-2, ycenter+5);
                        p->drawPoint(xcenter-5+3, ycenter+6);
                        p->drawPoint(xcenter+5-3, ycenter+6);
                        p->drawPoint(xcenter-5+4, ycenter+7);
                        p->drawPoint(xcenter+5-4, ycenter+7);
                        // anti-aliasing of the contour... sort of. :)
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 80) );
                        p->drawPoint(xcenter, ycenter+8);
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 150) );
                        p->drawPoint(xcenter-5, ycenter+4);
                        p->drawPoint(xcenter+5, ycenter+4);
                        p->drawPoint(xcenter-5+1, ycenter+5);
                        p->drawPoint(xcenter+5-1, ycenter+5);
                        p->drawPoint(xcenter-5+2, ycenter+6);
                        p->drawPoint(xcenter+5-2, ycenter+6);
                        p->drawPoint(xcenter-5+3, ycenter+7);
                        p->drawPoint(xcenter+5-3, ycenter+7);
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 190) );
                        p->drawPoint(xcenter-5+4, ycenter+8);
                        p->drawPoint(xcenter+5-4, ycenter+8);


                        QRegion mask(xcenter-4, ycenter-5, 9, 13);
                        mask -= QRegion(xcenter-4, ycenter+4, 1, 4);
                        mask -= QRegion(xcenter-3, ycenter+5, 1, 3);
                        mask -= QRegion(xcenter-2, ycenter+6, 1, 2);
                        mask -= QRegion(xcenter-1, ycenter+7, 1, 1);
                        mask -= QRegion(xcenter+1, ycenter+7, 1, 1);
                        mask -= QRegion(xcenter+2, ycenter+6, 1, 2);
                        mask -= QRegion(xcenter+3, ycenter+5, 1, 3);
                        mask -= QRegion(xcenter+4, ycenter+4, 1, 4);
                        p->setClipRegion(mask);
                        uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Round_UpperLeft|Round_UpperRight|Is_Horizontal;
                        if(!enabled)
                            surfaceFlags |= Is_Disabled;
                        renderSurface(p, QRect(xcenter-4, ycenter-5, 9, 13),
                                      pal.color(QPalette::Background), surface, getColor(pal,MouseOverHighlight),
                                    _contrast+3, surfaceFlags);
                        renderDot(p, QPointF(xcenter-3, ycenter-3), surface);
                        renderDot(p, QPointF(xcenter+2,   ycenter-3), surface);
                        p->setClipping(false);
                    } else {
                        // manual contour: vertex
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 50) );
                        p->drawPoint(xcenter+4, ycenter-5+1);
                        p->drawPoint(xcenter+4, ycenter+5-1);
                        p->drawPoint(xcenter+5, ycenter-5+2);
                        p->drawPoint(xcenter+5, ycenter+5-2);
                        p->drawPoint(xcenter+6, ycenter-5+3);
                        p->drawPoint(xcenter+6, ycenter+5-3);
                        p->drawPoint(xcenter+7, ycenter-5+4);
                        p->drawPoint(xcenter+7, ycenter+5-4);
                        // anti-aliasing. ...sort of :)
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 80) );
                        p->drawPoint(xcenter+8, ycenter);
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 150) );
                        p->drawPoint(xcenter+4, ycenter-5);
                        p->drawPoint(xcenter+4, ycenter+5);
                        p->drawPoint(xcenter+5, ycenter-5+1);
                        p->drawPoint(xcenter+5, ycenter+5-1);
                        p->drawPoint(xcenter+6, ycenter-5+2);
                        p->drawPoint(xcenter+6, ycenter+5-2);
                        p->drawPoint(xcenter+7, ycenter-5+3);
                        p->drawPoint(xcenter+7, ycenter+5-3);
                        p->setPen(alphaBlendColors(pal.color(QPalette::Background), contour, 190) );
                        p->drawPoint(xcenter+8, ycenter-5+4);
                        p->drawPoint(xcenter+8, ycenter+5-4);

                        QRegion mask(xcenter-5, ycenter-4, 13, 9);
                        mask -= QRegion(xcenter+4, ycenter-4, 4, 1);
                        mask -= QRegion(xcenter+5, ycenter-3, 3, 1);
                        mask -= QRegion(xcenter+6, ycenter-2, 2, 1);
                        mask -= QRegion(xcenter+7, ycenter-1, 1, 1);
                        mask -= QRegion(xcenter+7, ycenter+1, 1, 1);
                        mask -= QRegion(xcenter+6, ycenter+2, 2, 1);
                        mask -= QRegion(xcenter+5, ycenter+3, 3, 1);
                        mask -= QRegion(xcenter+4, ycenter+4, 4, 1);
                        p->setClipRegion(mask);
                        uint surfaceFlags = Draw_Left|Draw_Top|Draw_Bottom|Round_UpperLeft|Round_BottomLeft|
                                        Round_UpperRight|Is_Horizontal;
                        if(!enabled)
                            surfaceFlags |= Is_Disabled;
                        renderSurface(p, QRect(xcenter-5, ycenter-4, 13, 9),
                                      pal.color(QPalette::Background), surface, getColor(pal,MouseOverHighlight),
                                    _contrast+3, surfaceFlags);
                        renderDot(p, QPoint(xcenter-3, ycenter-3), surface);
                        renderDot(p, QPoint(xcenter-3,   ycenter+2), surface);
                        p->setClipping(false);
                    }

                    return;
                }

                case Slider::GrooveHor:
                case Slider::GrooveVert:
                {

                    bool horizontal = primitive == Slider::GrooveHor;

                    if (horizontal) {
                        int center = r.y()+r.height()/2;
                        renderHole(p, QRect(r.left(), center-2, r.width(), 4));
                    } else {
                        int center = r.x()+r.width()/2;
                        renderHole(p, QRect(center-2, r.top(), 4, r.height()));
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
                    p->fillRect(opt->rect.adjusted(1,1,-1,-1), inputColor );
                    renderHole(p, r, hasFocus, mouseOver);
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

            const QColor buttonColor = enabled?pal.color(QPalette::Button):pal.color(QPalette::Background);
            const QColor inputColor = enabled ? pal.color(QPalette::Base) : pal.color(QPalette::Background);
            QRect editField = subControlRect(CC_ComboBox, qstyleoption_cast<const QStyleOptionComplex*>(opt), SC_ComboBoxEditField, widget);

            switch (primitive)
            {
                case Generic::Frame:
                {
                    // TODO: pressed state
                    if(!editable) {
                        int surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                        if(reverseLayout) {
                            surfaceFlags |= Round_UpperRight|Round_BottomRight;
                        } else {
                            surfaceFlags |= Round_UpperLeft|Round_BottomLeft;
                        }

                        if (mouseOver) {
                            surfaceFlags |= Is_Highlight;
                            surfaceFlags |= Highlight_Top|Highlight_Bottom;
                        }
                        renderSurface(p, r,
                                    pal.color(QPalette::Background), buttonColor, getColor(pal,MouseOverHighlight), enabled?_contrast+3:(_contrast/2),
                                    surfaceFlags);
                    } else {
                        // input area
                        p->fillRect(r.adjusted(1,1,-1,-1), inputColor );

                        if (_inputFocusHighlight && hasFocus && enabled)
                        {
                            renderHole(p, r, true, mouseOver);
                        }
                        else
                        {
                            renderHole(p, r);
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

                        uint contourFlags = Draw_Right|Draw_Top|Draw_Bottom;
                        if (isFirst)
                            contourFlags |= Draw_Left;
                        if(!enabled) contourFlags|=Is_Disabled;
                        //renderContour(p, r, pal.color(QPalette::Background), getColor(pal,ButtonContour),                                        contourFlags);

                        uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
                        if(!enabled) surfaceFlags|=Is_Disabled;
                        else {
                            if(flags&State_On || flags&State_Sunken) surfaceFlags|=Is_Sunken;
                            else {
                                if(mouseOver) {
                                    surfaceFlags|=Is_Highlight|Highlight_Top|Highlight_Bottom;
                                }
                            }
                        }
                        renderSurface(p, QRect(isFirst?r.left()+1:r.left(), r.top()+1, isFirst?r.width()-2:r.width()-1, r.height()-2),
                                    pal.color(QPalette::Background), pal.color(QPalette::Button), getColor(pal,MouseOverHighlight), _contrast,
                                        surfaceFlags);
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

                    if ( _inputFocusHighlight && hasFocus && !isReadOnly && isEnabled)
                    {
                        renderHole(p, r, true, mouseOver);
                    }
                    else
                    {
                        renderHole(p, r, false, mouseOver);
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
                            p->fillRect(r.adjusted(lineWidth,lineWidth,-lineWidth,-lineWidth), inputColor);
                            drawPrimitive(PE_FrameLineEdit, panel, p, widget);
                        }
                        else
                            p->fillRect(r.adjusted(2,2,-2,-2), inputColor);
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
                    renderPanel(p, r, pal, false);

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
                        if(flags & State_Horizontal) {
                            int center = r.left()+r.width()/2;
                            p->setPen( getColor(pal, PanelDark) );
                            p->drawLine( center-1, r.top()+3, center-1, r.bottom()-3 );
                            p->setPen( getColor(pal, PanelLight) );
                            p->drawLine( center, r.top()+3, center, r.bottom()-3 );
                        } else {
                            int center = r.top()+r.height()/2;
                            p->setPen( getColor(pal, PanelDark) );
                            p->drawLine( r.x()+3, center-1, r.right()-3, center-1 );
                            p->setPen( getColor(pal, PanelLight) );
                            p->drawLine( r.x()+3, center, r.right()-3, center );
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
                    renderButton(p, r, pal, flags&State_Sunken||flags&State_On,
                                 false, true, flags&State_Enabled);

                    return;
                }
            }

        }
        break;

    }


    // Arrows
    if (primitive >= Generic::ArrowUp && primitive <= Generic::ArrowLeft) {
        QPolygon a;

        switch (primitive) {
            case Generic::ArrowUp: {
                a.setPoints(7, u_arrow);
                break;
            }
            case Generic::ArrowDown: {
                a.setPoints(7, d_arrow);
                break;
            }
            case Generic::ArrowLeft: {
                a.setPoints(7, l_arrow);
                break;
            }
            case Generic::ArrowRight: {
                a.setPoints(7, r_arrow);
                break;
            }
//                         default: {
//                             if (flags & Style_Up) {
//                                 a.setPoints(7, u_arrow);
//                             } else {
//                                 a.setPoints(7, d_arrow);
//                             }
//                         }
        }

                const QMatrix oldMatrix( p->matrix() );

//                     if (flags & Style_Down) {
//                         p->translate(pixelMetric(PM_ButtonShiftHorizontal),
//                                         pixelMetric(PM_ButtonShiftVertical));
//                     }

        a.translate((r.x()+r.width()/2), (r.y()+r.height()/2));
//                     // extra-pixel-shift, correcting some visual tics...
//                     switch(pe) {
//                         case Generic::ArrowLeft:
//                         case Generic::ArrowRight:
//                             a.translate(0, -1);
//                             break;
//                         case PE_SpinWidgetUp:
//                         case PE_SpinWidgetDown:
//                             a.translate(+1, 0);
//                             break;
//                         default:
//                             a.translate(0, 0);
//                     }

        KStyle::ColorOption* colorOpt   = extractOption<KStyle::ColorOption*>(kOpt);
        QColor               arrowColor = colorOpt->color.color(pal);

        p->setPen(arrowColor);

        p->drawLines(a.constData(), 3);
        p->drawPoint(a[6]);

        p->setMatrix( oldMatrix );

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
                renderHole(p, r, focusHighlight);
            } else if (flags & State_Raised) {
                renderPanel(p, r, pal, true, false, focusHighlight);
            } else {
                renderPanel(p, r, pal, false);
            }

            return;
        }
    }

    // default fallback
    KStyle::drawKStylePrimitive(widgetType, primitive, opt,
                                r, pal, flags, p, widget, kOpt);
}

void OxygenStyle::polish(QWidget* widget)
{
    if (widget->isWindow())
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
        ) {
        widget->setAttribute(Qt::WA_Hover);
    }

    if (qobject_cast<QMenuBar*>(widget)
        || widget->inherits("Q3ToolBar")
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
        || qobject_cast<QRadioButton*>(widget)) {
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

void OxygenStyle::renderHole(QPainter *p, const QRect &r, bool focus, bool hover, TileSet::PosFlags posFlags) const
{
    if((r.width() <= 0)||(r.height() <= 0))
        return;

    TileSet *tile;
    QColor base = QColor(Qt::white);
    // for holes, focus takes precedence over hover (other way around for buttons)
    if (focus)
        tile = _helper.holeFocused(base, _viewFocusColor);
    else if (hover)
        tile = _helper.holeFocused(base, _viewHoverColor);
    else
        tile = _helper.hole(base);
    tile->render(r, p, posFlags);
}


void OxygenStyle::renderSurface(QPainter *p,
                                 const QRect &r,
                                 const QColor &backgroundColor,
                                 const QColor &buttonColor,
                                 const QColor &highlightColor,
                                 int intensity,
                                 const uint flags) const
{
    if((r.width() <= 0)||(r.height() <= 0))
        return;

    const bool disabled = flags&Is_Disabled;

    const bool drawLeft = flags&Draw_Left;
    const bool drawRight = flags&Draw_Right;
    const bool drawTop = flags&Draw_Top;
    const bool drawBottom = flags&Draw_Bottom;
    const bool roundUpperLeft = flags&Round_UpperLeft;
    const bool roundUpperRight = flags&Round_UpperRight;
    const bool roundBottomLeft = flags&Round_BottomLeft;
    const bool roundBottomRight = flags&Round_BottomRight;
    const bool sunken = flags&Is_Sunken;
    const bool horizontal = flags&Is_Horizontal;
    bool highlight = false,
        highlightLeft = false,
        highlightRight = false,
        highlightTop = false,
        highlightBottom = false;
    // only highlight if not sunken & not disabled...
    if(!sunken && !disabled) {
        highlight = (flags&Is_Highlight);
        highlightLeft = (flags&Highlight_Left);
        highlightRight = (flags&Highlight_Right);
        highlightTop = (flags&Highlight_Top);
        highlightBottom = (flags&Highlight_Bottom);
    }

    QColor baseColor = alphaBlendColors(backgroundColor, disabled?backgroundColor:buttonColor, 10);
    if (disabled) {
        intensity = 2;
    } else if (highlight) {
        // blend this _slightly_ with the background
        baseColor = alphaBlendColors(baseColor, highlightColor, 240);
    } else if (sunken) {
        // enforce a common sunken-style...
        baseColor = baseColor.dark(110+intensity);
        intensity = _contrast/2;
    }
// some often needed colors...
    // 1 more intensive than 2 and 3.
    const QColor colorTop1 = alphaBlendColors(baseColor,
                    sunken?baseColor.dark(100+intensity*2):baseColor.light(100+intensity*2), 80);
    const QColor colorTop2 = alphaBlendColors(baseColor,
                    sunken?baseColor.dark(100+intensity):baseColor.light(100+intensity), 80);
    const QColor colorBottom1 = alphaBlendColors(baseColor,
                        sunken?baseColor.light(100+intensity*2):baseColor.dark(100+intensity*2), 80);
    const QColor colorBottom2 = alphaBlendColors(baseColor,
                        sunken?baseColor.light(100+intensity):baseColor.dark(100+intensity), 80);

// sides
    if (drawLeft) {
        if (horizontal) {
            int height = r.height();
            if (roundUpperLeft || !drawTop) height--;
            if (roundBottomLeft || !drawBottom) height--;
            renderGradient(p, QRect(r.left(), (roundUpperLeft&&drawTop)?r.top()+1:r.top(), 1, height),
                            colorTop1, baseColor);
        } else {
            p->setPen(colorTop1 );
            p->drawLine(r.left(), (roundUpperLeft&&drawTop)?r.top()+1:r.top(),
                        r.left(), (roundBottomLeft&&drawBottom)?r.bottom()-1:r.bottom() );
        }
    }
    if (drawRight) {
        if (horizontal) {
            int height = r.height();
            // TODO: there's still a bogus in it: when edge4 is Thick
            //       and we don't whant to draw the Top, we have a unpainted area
            if (roundUpperRight || !drawTop) height--;
            if (roundBottomRight || !drawBottom) height--;
            renderGradient(p, QRect(r.right(), (roundUpperRight&&drawTop)?r.top()+1:r.top(), 1, height),
                            baseColor, colorBottom1);
        } else {
            p->setPen(colorBottom1 );
            p->drawLine(r.right(), (roundUpperRight&&drawTop)?r.top()+1:r.top(),
                        r.right(), (roundBottomRight&&drawBottom)?r.bottom()-1:r.bottom() );
        }
    }
    if (drawTop) {
        if (horizontal) {
            p->setPen(colorTop1 );
            p->drawLine((roundUpperLeft&&drawLeft)?r.left()+1:r.left(), r.top(),
                        (roundUpperRight&&drawRight)?r.right()-1:r.right(), r.top() );
        } else {
            int width = r.width();
            if (roundUpperLeft || !drawLeft) width--;
            if (roundUpperRight || !drawRight) width--;
            renderGradient(p, QRect((roundUpperLeft&&drawLeft)?r.left()+1:r.left(), r.top(), width, 1),
                            colorTop1, colorTop2);
        }
    }
    if (drawBottom) {
        if (horizontal) {
            p->setPen(colorBottom1 );
            p->drawLine((roundBottomLeft&&drawLeft)?r.left()+1:r.left(), r.bottom(),
                        (roundBottomRight&&drawRight)?r.right()-1:r.right(), r.bottom() );
        } else {
            int width = r.width();
            if (roundBottomLeft || !drawLeft) width--;
            if (roundBottomRight || !drawRight) width--;
            renderGradient(p, QRect((roundBottomLeft&&drawLeft)?r.left()+1:r.left(), r.bottom(), width, 1),
                            colorBottom2, colorBottom1);
        }
    }

// button area...
    int width = r.width();
    int height = r.height();
    if (drawLeft) width--;
    if (drawRight) width--;
    if (drawTop) height--;
    if (drawBottom) height--;
    renderGradient(p, QRect(drawLeft?r.left()+1:r.left(), drawTop?r.top()+1:r.top(), width, height),
                    colorTop2, colorBottom2, horizontal);


// highlighting...
    QColor hl = highlightColor;
    hl.setAlphaF(0.6);
    p->setPen(hl);
    if(highlightTop) {
        p->drawLine((roundUpperLeft&&drawLeft)?r.left()+1:r.left(), r.top(),
                    (roundUpperRight&&drawRight)?r.right()-1:r.right(), r.top() );
    }
    if(highlightBottom) {
        p->drawLine((roundBottomLeft&&drawLeft)?r.left()+1:r.left(), r.bottom(),
                    (roundBottomRight&&drawRight)?r.right()-1:r.right(), r.bottom() );
    }
    if(highlightLeft) {
        p->drawLine(r.left(), (roundUpperLeft&&drawTop)?r.top()+1:r.top(),
                    r.left(), (roundBottomLeft&&drawBottom)?r.bottom()-1:r.bottom() );
    }
    if(highlightRight) {
        p->drawLine(r.right(), (roundUpperRight&&drawTop)?r.top()+1:r.top(),
                    r.right(), (roundBottomRight&&drawBottom)?r.bottom()-1:r.bottom() );
    }
    hl.setAlphaF(0.3);
    p->setPen(hl);
    if(highlightTop) {
        p->drawLine(highlightLeft?r.left()+1:r.left(), r.top()+1,
                    highlightRight?r.right()-1:r.right(), r.top()+1 );
    }
    if(highlightBottom) {
        p->drawLine(highlightLeft?r.left()+1:r.left(), r.bottom()-1,
                    highlightRight?r.right()-1:r.right(), r.bottom()-1 );
    }
    if(highlightLeft) {
        p->drawLine(r.left()+1, highlightTop?r.top()+1:r.top(),
                    r.left()+1, highlightBottom?r.bottom()-1:r.bottom() );
    }
    if(highlightRight) {
        p->drawLine(r.right()-1, highlightTop?r.top()+1:r.top(),
                    r.right()-1, highlightBottom?r.bottom()-1:r.bottom() );
    }
}

void OxygenStyle::renderButton(QPainter *p,
                               const QRect &r,
                               const QPalette &pal,
                               bool sunken,
                               bool mouseOver,
                               bool horizontal,
                               bool enabled,
                               bool khtmlMode) const
{
    const QPen oldPen( p->pen() );

    uint contourFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
    if(!enabled) contourFlags|=Is_Disabled;
    if(khtmlMode) contourFlags|=Draw_AlphaBlend;

    uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom;
    if(horizontal) surfaceFlags|=Is_Horizontal;
    if(!enabled) surfaceFlags|=Is_Disabled;
    else {
        if(sunken) surfaceFlags|=Is_Sunken;
        else {
            if(mouseOver) {
                surfaceFlags|=Is_Highlight;
                if(horizontal) {
                    surfaceFlags|=Highlight_Top;
                    surfaceFlags|=Highlight_Bottom;
                } else {
                    surfaceFlags|=Highlight_Left;
                    surfaceFlags|=Highlight_Right;
                }
            }
        }
    }

    if (!flatMode) {
        surfaceFlags |= Round_UpperLeft|Round_UpperRight|Round_BottomLeft|Round_BottomRight;

        renderSurface(p, QRect(r.left()+1, r.top()+1, r.width()-2, r.height()-2),
                      pal.color(QPalette::Background), pal.color(QPalette::Button), getColor(pal,MouseOverHighlight), _contrast, surfaceFlags);
    } else {
        renderSurface(p, QRect(r.left()+1, r.top()+1, r.width()-2, r.height()-2),
                      pal.color(QPalette::Background), pal.color(QPalette::Button), getColor(pal,MouseOverHighlight), _contrast/2, surfaceFlags);

        flatMode = false;
    }

    p->setPen(oldPen);
}

void OxygenStyle::renderCheckBox(QPainter *p, const QRect &rect, const QPalette &pal,
                                  bool enabled, bool mouseOver, int primitive) const
{
    QColor contentColor = enabled?pal.color(QPalette::Base):pal.color(QPalette::Background);

    int s = qMin(rect.width(), rect.height());
    QRect r = centerRect(rect, s, s);

    // surface
    uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
    if(!enabled) {
        surfaceFlags |= Is_Disabled;
    } else if(mouseOver) {
        contentColor = alphaBlendColors(contentColor, getColor(pal,MouseOverHighlight), 240);
        surfaceFlags |= Is_Highlight;
        surfaceFlags |= Highlight_Left|Highlight_Right|
                Highlight_Top|Highlight_Bottom;
    }
    renderSurface(p, QRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2),
                  pal.color(QPalette::Background), contentColor, getColor(pal,MouseOverHighlight), enabled?_contrast+3:(_contrast/2), surfaceFlags);

            // check mark
    QColor checkmarkColor = enabled?getColor(pal,CheckMark):pal.color(QPalette::Background);
            // TODO: check mouse pressed Style_Down equivalent for kstyle4
    if(false/*flags & Style_Down*/) {
        checkmarkColor = alphaBlendColors(contentColor, checkmarkColor, 150);
    }
    int x = r.center().x() - 4, y = r.center().y() - 4;

    QBitmap bmp;

    switch (primitive)
    {
        case CheckBox::CheckOn:
        {
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), checkmark_dark_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(150), 50) );
            p->drawPixmap(x, y, bmp);
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), checkmark_light_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(125), 50) );
            p->drawPixmap(x, y, bmp);
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), checkmark_aa_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(150), 150) );
            p->drawPixmap(x, y, bmp);

            return;
        }

        case CheckBox::CheckOff:
        {
                    // empty
            return;
        }

        case CheckBox::CheckTriState:
        {
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), checkmark_tristate_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(150), 50) );
            p->drawPixmap(x, y, bmp);

            return;
        }
    }
}

void OxygenStyle::renderRadioButton(QPainter *p, const QRect &r, const QPalette &pal,
                                        bool enabled, bool mouseOver, int prim) const
{


    int x = r.x();
    int y = r.y();

    const QColor contourColor = getColor(pal, ButtonContour, enabled);
    QColor contentColor = enabled?pal.color(QPalette::Base):pal.color(QPalette::Background);

    QBitmap bmp;
    bmp = QBitmap::fromData(QSize( 13, 13 ), radiobutton_mask_bits);
            // first the surface...
    uint surfaceFlags = Draw_Left|Draw_Right|Draw_Top|Draw_Bottom|Is_Horizontal;
    if(!enabled) {
        surfaceFlags |= Is_Disabled;
    } else if (mouseOver) {
        contentColor = alphaBlendColors(contentColor, getColor(pal,MouseOverHighlight), 240);
    }
    p->setClipRegion(bmp);
    renderSurface(p, r,
                  pal.color(QPalette::Background), contentColor, getColor(pal,MouseOverHighlight), enabled?_contrast+3:(_contrast/2), surfaceFlags);
    p->setClipping(false);

            // ...then contour, anti-alias, mouseOver...
            // contour
    bmp = QBitmap::fromData(QSize( 13, 13 ), radiobutton_contour_bits);
    bmp.setMask(bmp);
    p->setPen(alphaBlendColors(pal.color(QPalette::Background), contourColor, 50) );
    p->drawPixmap(x, y, bmp);
            // anti-alias outside
    bmp = QBitmap::fromData(QSize( 13, 13 ), radiobutton_aa_outside_bits);
    bmp.setMask(bmp);
    p->setPen(alphaBlendColors(pal.color(QPalette::Background), contourColor, 150) );
    p->drawPixmap(x, y, bmp);
            // highlighting...
    if(mouseOver) {
        bmp = QBitmap::fromData(QSize( 13, 13 ), radiobutton_highlight1_bits);
        bmp.setMask(bmp);
        p->setPen(alphaBlendColors(contentColor, getColor(pal,MouseOverHighlight), 80) );
        p->drawPixmap(x, y, bmp);
        bmp = QBitmap::fromData(QSize( 13, 13 ), radiobutton_highlight2_bits);
        bmp.setMask(bmp);
        p->setPen(alphaBlendColors(contentColor, getColor(pal,MouseOverHighlight), 150) );
        p->drawPixmap(x, y, bmp);
    }
            // anti-alias inside, "above" the higlighting!
    bmp = QBitmap::fromData(QSize( 13, 13 ), radiobutton_aa_inside_bits);
    bmp.setMask(bmp);
    if(mouseOver) {
        p->setPen(alphaBlendColors(getColor(pal,MouseOverHighlight), contourColor, 180) );
    } else {
        p->setPen(alphaBlendColors(contentColor, contourColor, 180) );
    }
    p->drawPixmap(x, y, bmp);


    QColor checkmarkColor = enabled?getColor(pal,CheckMark):pal.color(QPalette::Background);
            // TODO: implement pressed state with Style_Down equivalent
    if(false /*flags & Style_Down*/) {
        checkmarkColor = alphaBlendColors(contentColor, checkmarkColor, 150);
    }

            // draw the radio mark
    switch (prim)
    {
        case RadioButton::RadioOn:
        {
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), radiomark_dark_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(150), 50) );
            p->drawPixmap(x+2, y+2, bmp);
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), radiomark_light_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(125), 50) );
            p->drawPixmap(x+2, y+2, bmp);
            bmp = QBitmap::fromData(QSize( CHECKMARKSIZE, CHECKMARKSIZE ), radiomark_aa_bits);
            bmp.setMask(bmp);
            p->setPen(alphaBlendColors(contentColor, checkmarkColor.dark(150), 150) );
            p->drawPixmap(x+2, y+2, bmp);

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
    const qreal diameter = 2.5;
    p->setRenderHint(QPainter::Antialiasing);
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(0, 0, 0, 66));
    p->drawEllipse(QRectF(point.x()-diameter/2+0.5, point.y()-diameter/2+0.5, diameter, diameter));
    p->setRenderHint(QPainter::Antialiasing, false);
}

void OxygenStyle::renderGradient(QPainter *painter,
                                  const QRect &rect,
                                  const QColor &c1,
                                  const QColor &c2,
                                  bool horizontal) const
{
    if((rect.width() <= 0)||(rect.height() <= 0))
        return;

    // generate a quite unique key for this surface.
    CacheEntry search(cGradientTile,
                      horizontal ? 0 : rect.width(),
                      horizontal ? rect.height() : 0,
                      c1.rgb(), c2.rgb(), horizontal );
    int key = search.key();

    CacheEntry *cacheEntry;
    if( (cacheEntry = pixmapCache->object(key)) ) {
        if( search == *cacheEntry ) { // match! we can draw now...
            if(cacheEntry->pixmap) {
                painter->drawTiledPixmap(rect, *(cacheEntry->pixmap) );
            }
            return;
        } else {
            // Remove old entry in case of a conflict!
            // This shouldn't happen very often, see comment in CacheEntry.
            pixmapCache->remove(key);
        }
    }

    // there wasn't anything matching in the cache, create the pixmap now...
    QPixmap *result = new QPixmap(horizontal ? 10 : rect.width(),
                                  horizontal ? rect.height() : 10);
    QPainter p(result);

    int r_w = result->rect().width();
    int r_h = result->rect().height();
    int r_x, r_y, r_x2, r_y2;
    result->rect().getCoords(&r_x, &r_y, &r_x2, &r_y2);

    int rDiff, gDiff, bDiff;
    int rc, gc, bc;

    register int x, y;

    rDiff = ( c2.red())   - (rc = c1.red());
    gDiff = ( c2.green()) - (gc = c1.green());
    bDiff = ( c2.blue())  - (bc = c1.blue());

    register int rl = rc << 16;
    register int gl = gc << 16;
    register int bl = bc << 16;

    int rdelta = ((1<<16) / (horizontal ? r_h : r_w)) * rDiff;
    int gdelta = ((1<<16) / (horizontal ? r_h : r_w)) * gDiff;
    int bdelta = ((1<<16) / (horizontal ? r_h : r_w)) * bDiff;

    // these for-loops could be merged, but the if's in the inner loop
    // would make it slow
    if(horizontal) {
        for ( y = 0; y < r_h; y++ ) {
            rl += rdelta;
            gl += gdelta;
            bl += bdelta;

            p.setPen(QColor(rl>>16, gl>>16, bl>>16));
            p.drawLine(r_x, r_y+y, r_x2, r_y+y);
        }
    } else {
        for( x = 0; x < r_w; x++) {
            rl += rdelta;
            gl += gdelta;
            bl += bdelta;

            p.setPen(QColor(rl>>16, gl>>16, bl>>16));
            p.drawLine(r_x+x, r_y, r_x+x, r_y2);
        }
    }

    p.end();

    // draw the result...
    painter->drawTiledPixmap(rect, *result);

    // insert into cache using the previously created key.
    CacheEntry *toAdd = new CacheEntry(search);
    toAdd->pixmap = result;
    bool insertOk = pixmapCache->insert( key, toAdd, result->width()*result->height()*result->depth()/8 );

    if(!insertOk)
        delete result;
}

void OxygenStyle::renderPanel(QPainter *p,
                              const QRect &r,
                              const QPalette &pal,
                              const bool pseudo3d,
                              const bool sunken,
                              const bool focusHighlight) const
{
    int x, x2, y, y2, w, h;
    r.getRect(&x,&y,&w,&h);
    r.getCoords(&x, &y, &x2, &y2);

        if(pseudo3d) {
            QColor dark = focusHighlight ?
                    getColor(pal,FocusHighlight).dark(130) : getColor(pal, PanelDark);
            QColor light = focusHighlight ?
                    getColor(pal,FocusHighlight).light(130) : getColor(pal, PanelLight);
            if (sunken) {
                p->setPen(dark);
            } else {
                p->setPen(light);
            }
            p->drawLine(r.left()+2, r.top()+1, r.right()-2, r.top()+1);
            p->drawLine(r.left()+1, r.top()+2, r.left()+1, r.bottom()-2);
            if (sunken) {
                p->setPen(light);
            } else {
                p->setPen(dark);
            }
            p->drawLine(r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1);
            p->drawLine(r.right()-1, r.top()+2, r.right()-1, r.bottom()-2);
        }
}


void OxygenStyle::renderTab(QPainter *p,
                            const QRect &r,
                            const QPalette &pal,
                            bool mouseOver,
                            const bool selected,
                            const bool bottom,
                            const QStyleOptionTab::TabPosition pos/*const TabPosition pos*/,
                            const bool triangular,
                            const bool cornerWidget,
                            const bool reverseLayout) const
{
    const bool isFirst = pos == QStyleOptionTab::Beginning || pos == QStyleOptionTab::OnlyOneTab/* (pos == First) || (pos == Single)*/;
    const bool isLast = pos == QStyleOptionTab::End /*(pos == Last)*/;
    const bool isSingle = pos == QStyleOptionTab::OnlyOneTab /*(pos == Single)*/;

    if (selected) {
    // is selected

    // the top part of the tab which is nearly the same for all positions
        QRect Rc; // contour
        if (!bottom) {
            if (isFirst && !cornerWidget && !reverseLayout) {
                Rc = QRect(r.x(), r.y(), r.width()-1, r.height()-3);
            } else if (isFirst && !cornerWidget && reverseLayout) {
                Rc = QRect(r.x()+1, r.y(), r.width()-1, r.height()-3);
            } else {
                Rc = QRect(r.x()+1, r.y(), r.width()-2, r.height()-3);
            }
        } else {
            if (isFirst && !cornerWidget && !reverseLayout) {
                Rc = QRect(r.x(), r.y()+3, r.width()-1, r.height()-3);
            } else if (isFirst && !cornerWidget && reverseLayout) {
                Rc = QRect(r.x()+1, r.y()+3, r.width()-1, r.height()-3);
            } else {
                Rc = QRect(r.x()+1, r.y()+3, r.width()-2, r.height()-3);
            }
        }
        const QRect Rs(Rc.x()+1, bottom?Rc.y():Rc.y()+1, Rc.width()-2, Rc.height()-1); // the resulting surface
        // the area where the fake border shoudl appear
        const QRect Rb(r.x(), bottom?r.top():Rc.bottom()+1, r.width(), r.height()-Rc.height() );

        // surface
        if(!bottom) {
            p->setPen(getColor(pal,PanelLight) );
            p->drawLine(Rs.x()+1, Rs.y(), Rs.right()-1, Rs.y() );
            renderGradient(p, QRect(Rs.x(), Rs.y()+1, 1, Rs.height()-1),
                           getColor(pal,PanelLight), getColor(pal,PanelLight2));
            renderGradient(p, QRect(Rs.right(), Rs.y()+1, 1, Rs.height()-1),
                            getColor(pal,PanelDark), getColor(pal,PanelDark2));
        } else {
            p->setPen(alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).dark(160), 100) );
            p->drawLine(Rs.x()+1, Rs.bottom(), Rs.right()-1, Rs.bottom() );
            renderGradient(p, QRect(Rs.x(), Rs.y(), 1, Rs.height()-1),
                            getColor(pal,PanelLight), getColor(pal,PanelLight2));
            renderGradient(p, QRect(Rs.right(), Rs.y(), 1, Rs.height()-1),
                            getColor(pal,PanelDark), getColor(pal,PanelDark2));
        }

    // some "position specific" paintings...
        // draw parts of the inactive tabs around...
        if(!isSingle) {
            p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal, ButtonContour), 50) );
            if( (!isFirst&&!reverseLayout) || (!isLast&&reverseLayout) ) {
                p->drawPoint(r.left(), bottom?(triangular?r.bottom()-2:r.bottom()-3):(triangular?r.top()+2:r.top()+3) );
                renderSurface(p, QRect(r.left(), bottom?r.top()+3:(triangular?r.top()+3:r.top()+4), 1, (triangular?r.height()-6:r.height()-7) ),
                            pal.color( QPalette::Background ), pal.color( QPalette::Button ), getColor(pal,MouseOverHighlight), _contrast,
                            Draw_Top|Draw_Bottom|Is_Horizontal);
            }
            if( (!isLast&&!reverseLayout) || (!isFirst&&reverseLayout) ) {
                p->drawPoint(r.right(), bottom?(triangular?r.bottom()-2:r.bottom()-3):(triangular?r.top()+2:r.top()+3) );
                renderSurface(p, QRect(r.right(), bottom?r.top()+3:(triangular?r.top()+3:r.top()+4), 1, (triangular?r.height()-6:r.height()-7) ),
                            pal.color( QPalette::Background ), pal.color( QPalette::Button ), getColor(pal,MouseOverHighlight), _contrast,
                            Draw_Top|Draw_Bottom|Is_Horizontal);
            }
        }
        // left connection from the panel border to the tab. :)
        if(isFirst && !reverseLayout && !cornerWidget) {
            p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
            p->drawLine(Rb.x(), Rb.y(), Rb.x(), Rb.bottom() );
            p->setPen(getColor(pal,PanelLight) );
            p->drawLine(Rb.x()+1, Rb.y(), Rb.x()+1, Rb.bottom() );
        } else if(isFirst && reverseLayout && !cornerWidget) {
            p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
            p->drawLine(Rb.right(), Rb.y(), Rb.right(), Rb.bottom() );
            p->setPen(getColor(pal,PanelDark) );
            p->drawLine(Rb.right()-1, Rb.y(), Rb.right()-1, Rb.bottom() );
        }
        // rounded connections to the panel...
        if(!bottom) {
            // left
            if( (!isFirst && !reverseLayout) || (reverseLayout) || (isFirst && !reverseLayout && cornerWidget) ) {
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.x(), Rb.y());
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.x(), Rb.y()+1);
                p->drawPoint(Rb.x()+1, Rb.y());
            }
            // right
            if( (!reverseLayout) || (!isFirst && reverseLayout) || (isFirst && reverseLayout && cornerWidget) ) {
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.right(), Rb.y());
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.right(), Rb.y()+1);
                p->drawPoint(Rb.right()-1, Rb.y());
            }
        } else {
            // left
            if( (!isFirst && !reverseLayout) || (reverseLayout) || (isFirst && !reverseLayout && cornerWidget) ) {
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.x(), Rb.bottom());
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.x(), Rb.bottom()-1);
                p->drawPoint(Rb.x()+1, Rb.bottom());
            }
            // right
            if( (!reverseLayout) || (!isFirst && reverseLayout) || (isFirst && reverseLayout && cornerWidget) ) {
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.right(), Rb.bottom());
                p->setPen( alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.right(), Rb.bottom()-1);
                p->drawPoint(Rb.right()-1, Rb.bottom());
            }
        }

    } else {
    // inactive tabs

    // the top part of the tab which is nearly the same for all positions
        QRect Rc; // contour
        if (isFirst&&reverseLayout ) {
            Rc = QRect(r.x()+1, (bottom?r.y()+2:(triangular?r.y()+2:r.y()+3)), r.width()-2, (triangular?r.height()-4:r.height()-5) );
        } else {
            Rc = QRect(r.x()+1, (bottom?r.y()+2:(triangular?r.y()+2:r.y()+3)), r.width()-1, (triangular?r.height()-4:r.height()-5) );
        }
        QRect Rs; // the resulting surface
        if ( (isFirst&&!reverseLayout) || (isLast&&reverseLayout) ) {
            Rs = QRect(Rc.x()+1, bottom?Rc.y():Rc.y()+1, Rc.width()-2, Rc.height()-1);
        } else {
            Rs = QRect(Rc.x(), bottom?Rc.y():Rc.y()+1, Rc.width()-1, Rc.height()-1);
        }
        // the area where the fake border shoudl appear
        const QRect Rb(r.x(), bottom?r.y():Rc.bottom()+1, r.width(), 2 );

        uint contourFlags;
        if(!bottom) {
            if ( (isFirst&&!reverseLayout) || (isLast&&reverseLayout) ) {
                contourFlags = Draw_Left|Draw_Right|Draw_Top|Round_UpperLeft;
            } else if ( (isLast&&!reverseLayout) || (isFirst&&reverseLayout) ) {
                contourFlags = Draw_Right|Draw_Top|Round_UpperRight;
            } else {
                contourFlags = Draw_Right|Draw_Top;
            }
        } else {
            if ( (isFirst&&!reverseLayout) || (isLast&&reverseLayout) ) {
                contourFlags = Draw_Left|Draw_Right|Draw_Bottom|Round_BottomLeft;
            } else if ( (isLast&&!reverseLayout) || (isFirst&&reverseLayout) ) {
                contourFlags = Draw_Right|Draw_Bottom|Round_BottomRight;
            } else {
                contourFlags = Draw_Right|Draw_Bottom;
            }
        }
//FIXME CBR        renderContour(p, Rc,                        pal.color( QPalette::Background ), getColor(pal, ButtonContour),                        contourFlags);

        uint surfaceFlags = Is_Horizontal;
        if(mouseOver) {
            surfaceFlags |= (bottom?Highlight_Bottom:Highlight_Top);
            surfaceFlags |= Is_Highlight;
        }
        if ( (isFirst&&!reverseLayout) || (isLast&&reverseLayout) ) {
            if(!bottom)
                surfaceFlags |= Draw_Left|Draw_Top|Draw_Bottom|Round_UpperLeft;
            else
                surfaceFlags |= Draw_Left|Draw_Top|Draw_Bottom|Round_BottomLeft;
        } else if ( (isLast&&!reverseLayout) || (isFirst&&reverseLayout) ) {
            if(!bottom)
                surfaceFlags |= Draw_Right|Draw_Top|Draw_Bottom|Round_UpperRight;
            else
                surfaceFlags |= Draw_Right|Draw_Top|Draw_Bottom|Round_BottomRight;
        } else {
            surfaceFlags |= Draw_Top|Draw_Bottom;
        }
        renderSurface(p, Rs,
                        pal.color(QPalette::Background), pal.color( QPalette::Button ), getColor(pal,MouseOverHighlight), _contrast,
                        surfaceFlags);

    // some "position specific" paintings...
        // fake parts of the panel border
        if(!bottom) {
            p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
            p->drawLine(Rb.x(), Rb.y(), ((isLast&&!reverseLayout)||(isFirst&&reverseLayout&&cornerWidget))?Rb.right():Rb.right()-1, Rb.y());
            p->setPen(getColor(pal,PanelLight) );
            p->drawLine(Rb.x(), Rb.y()+1, ((isLast&&!reverseLayout)||(isFirst&&reverseLayout&&cornerWidget))?Rb.right():Rb.right()-1, Rb.y()+1 );
        } else {
            p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
            p->drawLine(Rb.x(), Rb.bottom(), ((isLast&&!reverseLayout)||(isFirst&&reverseLayout&&cornerWidget))?Rb.right():Rb.right()-1, Rb.bottom());
            p->setPen(getColor(pal,PanelDark) );
            p->drawLine(Rb.x(), Rb.bottom()-1, ((isLast&&!reverseLayout)||(isFirst&&reverseLayout&&cornerWidget))?Rb.right():Rb.right()-1, Rb.bottom()-1 );
        }
        // fake the panel border edge for tabs which are aligned left-most
        // (i.e. only if there is no widget in the corner of the tabwidget!)
        if(isFirst&&!reverseLayout&&!cornerWidget)
        // normal layout
        {
            if (!bottom) {
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.x()+1, Rb.y()+1 );
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.x(), Rb.y()+1 );
                p->setPen(pal.color(QPalette::Background) );
                p->drawPoint(Rb.x(), Rb.y() );
                p->setPen(alphaBlendColors( alphaBlendColors(pal.color(QPalette::Background), getColor(pal, ButtonContour), 50), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.x()+1, Rb.y() );
            } else {
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.x()+1, Rb.bottom()-1 );
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.x(), Rb.bottom()-1 );
                p->setPen(pal.color(QPalette::Background) );
                p->drawPoint(Rb.x(), Rb.bottom() );
                p->setPen(alphaBlendColors( alphaBlendColors(pal.color(QPalette::Background), getColor(pal, ButtonContour), 50), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.x()+1, Rb.bottom() );
            }
        } else if(isFirst&&reverseLayout&&!cornerWidget)
        // reverse layout
        {
            if (!bottom) {
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.right()-1, Rb.y()+1 );
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.right(), Rb.y()+1 );
                p->setPen(pal.color(QPalette::Background) );
                p->drawPoint(Rb.right(), Rb.y() );
                p->setPen(alphaBlendColors( alphaBlendColors(pal.color(QPalette::Background), getColor(pal, ButtonContour), 50), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.right()-1, Rb.y() );
            } else {
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 50) );
                p->drawPoint(Rb.right()-1, Rb.bottom()-1 );
                p->setPen(alphaBlendColors(pal.color(QPalette::Background), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.right(), Rb.bottom()-1 );
                p->setPen(pal.color(QPalette::Background) );
                p->drawPoint(Rb.right(), Rb.bottom() );
                p->setPen(alphaBlendColors( alphaBlendColors(pal.color(QPalette::Background), getColor(pal, ButtonContour), 50), getColor(pal,PanelContour), 150) );
                p->drawPoint(Rb.right()-1, Rb.bottom() );
            }
        }
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

        default:
            return KStyle::styleHint(hint, option, widget, returnData);
    }
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

QColor OxygenStyle::getColor(const QPalette &pal, const ColorType t, const bool enabled)const
{
    return getColor(pal, t, enabled?IsEnabled:IsDisabled);
}

QColor OxygenStyle::getColor(const QPalette &pal, const ColorType t, const WidgetState s)const
{
    const bool enabled = (s != IsDisabled) &&
            ((s == IsEnabled) || (s == IsPressed) || (s == IsHighlighted));
    const bool pressed = (s == IsPressed);
    const bool highlighted = (s == IsHighlighted);
    switch(t) {
        case ButtonContour:
            return enabled ? pal.color(QPalette::Button).dark(130+_contrast*8)
            : pal.color(QPalette::Background).dark(120+_contrast*8);
        case DragButtonContour: {
            if(enabled) {
                if(pressed)
                    return pal.color(QPalette::Button).dark(130+_contrast*6); // bright
                else if(highlighted)
                    return pal.color(QPalette::Button).dark(130+_contrast*9); // dark
                else
                    return pal.color(QPalette::Button).dark(130+_contrast*8); // normal
            } else {
                return pal.color(QPalette::Background).dark(120+_contrast*8);
            }
        }
        case DragButtonSurface: {
            if(enabled) {
                if(pressed)
                    return pal.color(QPalette::Button).dark(100-_contrast);  // bright
                else if(highlighted)
                    return pal.color(QPalette::Button).light(100+_contrast); // dark
                else
                    return pal.color(QPalette::Button);                      // normal
            } else {
                return pal.color(QPalette::Background);
            }
        }
        case PanelContour:
            return pal.color(QPalette::Background).dark(160+_contrast*8);
        case PanelDark:
            return alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).dark(120+_contrast*5), 110);
        case PanelDark2:
            return alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).dark(110+_contrast*5), 110);
        case PanelLight:
            return alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).light(120+_contrast*5), 110);
        case PanelLight2:
            return alphaBlendColors(pal.color(QPalette::Background), pal.color(QPalette::Background).light(110+_contrast*5), 110);
        case MouseOverHighlight:
            if( _customOverHighlightColor )
                return _overHighlightColor;
            else
                return pal.color( QPalette::Highlight );
        case FocusHighlight:
            if( _customFocusHighlightColor )
                return _focusHighlightColor;
            else
                return pal.color( QPalette::Highlight );
        case CheckMark:
            if( _customCheckMarkColor )
                return _checkMarkColor;
            else
                return pal.color( QPalette::Foreground );
        default:
            return pal.color(QPalette::Background);
    }
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;

