/* Oxygen widget style for KDE 4
   Copyright (C) 2007-2008 Casper Boemann <cbr@boemann.dk>
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
#include <QtGui/QToolBox>
#include <QtGui/QScrollBar>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QDockWidget>
#include <QtGui/QMdiSubWindow>
#include <QStyleOptionDockWidget>
#include <QPaintEvent>
#include <QToolBox>
#include <QAbstractScrollArea>
#include <QAbstractItemView>
#include <KTitleWidget>

#include <QtDBus/QtDBus>

#include <KGlobal>
#include <KGlobalSettings>
#include <KConfigGroup>
#include <KColorUtils>
#include <KIconEffect>
#include <kdebug.h>

#include <math.h>

#include "helper.h"
#include "tileset.h"

// We need better holes! Bevel color and shadow color are currently based on
// only one color, even though they are different things; also, we don't really
// know what bevel color should be based on... (and shadow color for white
// views looks rather bad). For now at least, just using QPalette::Window
// everywhere seems best...
#define HOLE_COLOR_OUTSIDE

K_EXPORT_STYLE("Oxygen", OxygenStyle)

K_GLOBAL_STATIC_WITH_ARGS(OxygenStyleHelper, globalHelper, ("oxygen"))

static const int gw = 2; // ie glowwidth which we want to un-reserve space for in the tabs

static void cleanupBefore()
{
    OxygenStyleHelper *h = globalHelper;
    h->cleanupBeforeDelete();
}

OxygenStyle::OxygenStyle() :
    KStyle(),
    _helper(*globalHelper)
{
    _config = _helper.config();

    qAddPostRoutine(cleanupBefore);

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

    setWidgetLayoutProp(WT_Generic, Generic::DefaultFrameWidth, 1);

    // TODO: change this when double buttons are implemented
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleBotButton, true);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::MinimumSliderHeight, 21);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::BarWidth, 15); // size*2+1
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::ArrowColor,QPalette::ButtonText);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::ActiveArrowColor,QPalette::ButtonText);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::SingleButtonHeight, 14);
    setWidgetLayoutProp(WT_ScrollBar, ScrollBar::DoubleButtonHeight, 28);

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
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftHorizontal, 0);
    setWidgetLayoutProp(WT_PushButton, PushButton::PressedShiftVertical,   0);

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
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin,        0);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Left,  0);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Right, 0);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Top, 0);
    setWidgetLayoutProp(WT_MenuBar, MenuBar::Margin + Bot, 2);

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
    setWidgetLayoutProp(WT_SpinBox, SpinBox::ContentsMargin + Right, 4);
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
    setWidgetLayoutProp(WT_ComboBox, ComboBox::ContentsMargin + Right, 4);
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
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemMargin, 2);

    setWidgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin, 4);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::FocusMargin,    0);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorSize, 8);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorXOff, -11);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorYOff, -10);

    setWidgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, 5);
    setWidgetLayoutProp(WT_GroupBox, GroupBox::TitleTextColor, ColorMode(QPalette::WindowText));

    setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin, 5);

    setWidgetLayoutProp(WT_Window, Window::TitleTextColor, QPalette::WindowText);

    KConfigGroup cfg(_config, "Style");
    switch (cfg.readEntry("MenuHighlight", (int)MM_DARK)) {
        case MM_STRONG:
            _menuHighlightMode = MM_STRONG;
            break;
        case MM_SUBTLE:
            _menuHighlightMode = MM_SUBTLE;
            break;
        default:
            _menuHighlightMode = MM_DARK;
    }
    _checkCheck = (cfg.readEntry("CheckStyle", 0) == 0);
    _animateProgressBar = cfg.readEntry("AnimateProgressBar", true);
    _drawToolBarItemSeparator = cfg.readEntry("DrawToolBarItemSeparator", true);
    _drawTriangularExpander = cfg.readEntry("DrawTriangularExpander", false);

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
            // don't update right now
            // iter.key()->update();
        }
        if ((pb->minimum() == 0 && pb->maximum() == 0))
        {
          pb->setValue(pb->value()+1);
          pb->update();
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

void OxygenStyle::drawComplexControl(ComplexControl control,const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
	switch (control)
	{
		case CC_GroupBox:
		{
			if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option))
			{
				bool isFlat = groupBox->features & QStyleOptionFrameV2::Flat;

				if (isFlat)
				{
					QFont font = painter->font();
					font.setBold(true);
					painter->setFont(font);
				}
			}
		}
		break;
		default:
			break;
	}

	return KStyle::drawComplexControl(control,option,painter,widget);
}

void OxygenStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    switch (element)
    {
        case CE_RubberBand:
        {
            if (const QStyleOptionRubberBand *rbOpt = qstyleoption_cast<const QStyleOptionRubberBand *>(option))
            {
                p->save();
                QColor color = rbOpt->palette.color(QPalette::Highlight);
                color.setAlpha(50);
                p->setBrush(color);
                color = KColorUtils::mix(color, rbOpt->palette.color(QPalette::Active, QPalette::WindowText));
                p->setPen(color);
                p->setClipRegion(rbOpt->rect);
                p->drawRect(rbOpt->rect.adjusted(0,0,-1,-1));
                p->restore();
            }
            break;
        }
    case CE_ComboBoxLabel: //same as CommonStyle, except for fiilling behind icon
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
            QRect editRect = subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField, widget);
            p->save();
            p->setClipRect(editRect);
            if (!cb->currentIcon.isNull()) {
                QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal
                                                             : QIcon::Disabled;
                QPixmap pixmap = cb->currentIcon.pixmap(cb->iconSize, mode);
                QRect iconRect(editRect);
                iconRect.setWidth(cb->iconSize.width() + 4);
                iconRect = alignedRect(cb->direction,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       iconRect.size(), editRect);

                drawItemPixmap(p, iconRect, Qt::AlignCenter, pixmap);

                if (cb->direction == Qt::RightToLeft)
                    editRect.translate(-4 - cb->iconSize.width(), 0);
                else
                    editRect.translate(cb->iconSize.width() + 4, 0);
            }
            if (!cb->currentText.isEmpty() && !cb->editable) {
                drawItemText(p, editRect.adjusted(1, 0, -1, 0),
                             visualAlignment(cb->direction, Qt::AlignLeft | Qt::AlignVCenter),
                             cb->palette, cb->state & State_Enabled, cb->currentText);
            }
            p->restore();
        }
        break;

        default:
            KStyle::drawControl(element, option, p, widget);
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
                    const QStyleOptionToolBox *option = qstyleoption_cast<const QStyleOptionToolBox *>(opt);
                    if(!(option && widget)) return;

                    const QStyleOptionToolBoxV2 *v2 = qstyleoption_cast<const QStyleOptionToolBoxV2 *>(opt);

                    p->save();
                    if (v2 && v2->position == QStyleOptionToolBoxV2::Beginning)
                    {
                        p->restore();
                        return;
                    }

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
            const QStyleOptionProgressBarV2 *pbOpt = qstyleoption_cast<const QStyleOptionProgressBarV2 *>(opt);
            Qt::Orientation orientation = pbOpt? pbOpt->orientation : Qt::Horizontal;

            // we want small (16px) and centered progressbars
            QRect rect = r;
            /*if (orientation == Qt::Horizontal) {
                if (r.height() > 17)
                    rect = QRect(r.x(), r.y() + (r.height()-17)/2, r.width(), 18);
            } else {
                if (r.width() > 17)
                    rect = QRect(r.x() + (r.width()-17)/2, r.y(), 18, r.height());
            }*/

            switch (primitive)
            {
                case ProgressBar::Groove:
                {
                    QColor color = pal.color(QPalette::Window);

                    TileSet *tiles1 = _helper.progressBar(color, rect, orientation);
                    if (orientation == Qt::Horizontal)
                        tiles1->render(rect, p, TileSet::Left | TileSet::Vertical | TileSet::Right);
                    else
                        tiles1->render(rect, p, TileSet::Top | TileSet::Horizontal | TileSet::Bottom);

                    return;
                }

                case ProgressBar::BusyIndicator:
                case ProgressBar::Indicator:
                {
                    rect.adjust(-2,-2,2,2);

                    QColor color = pal.color(QPalette::Active, QPalette::Highlight);

                    if (rect.width() > 3) // doesn't look too good in a very small rect
                    {
                        // TODO: make kstyle make vertical progress bar grow from bottom to top
                        TileSet *tiles1 = _helper.progressBar(_helper.alphaColor(color,0.8), rect, orientation);

                        QPixmap pm(rect.width(),rect.height());
                        pm.fill(Qt::transparent);
                        QPainter pp(&pm);
                        pp.setRenderHints(QPainter::Antialiasing);

                        QLinearGradient lg(rect.topLeft(),rect.topRight());
                        lg.setColorAt(0.0, _helper.alphaColor(color,0.8));
                        lg.setColorAt(1.0, color);
                        pp.setPen(Qt::NoPen);
                        pp.setBrush(lg);
                        pp.drawRoundedRect(pm.rect().adjusted(2,2,-2,-3),3,3);
                        // only draw the inner part of the scrollbar
                        pp.setCompositionMode(QPainter::CompositionMode_SourceAtop);

                        if (orientation == Qt::Horizontal)
                            tiles1->render(pm.rect(), &pp, TileSet::Horizontal);
                        else
                            tiles1->render(pm.rect(), &pp, TileSet::Vertical);

                        pp.end();
                        p->drawPixmap(rect.topLeft(),pm);
                    }
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

                    if (active) {
                        QColor color = pal.color(QPalette::Window);
                        if (_menuHighlightMode != MM_DARK) {
                            if(flags & State_Sunken) {
                                if (_menuHighlightMode == MM_STRONG)
                                    color = pal.color(QPalette::Highlight);
                                else
                                    color = KColorUtils::mix(color, KColorUtils::tint(color, pal.color(QPalette::Highlight), 0.6));
                            }
                            else {
                                if (_menuHighlightMode == MM_STRONG)
                                    color = KColorUtils::tint(color, _viewHoverBrush.brush(pal).color());
                                else
                                    color = KColorUtils::mix(color, KColorUtils::tint(color, _viewHoverBrush.brush(pal).color()));
                            }
                        }
                        else {
                            color = _helper.calcMidColor(color);
                        }

                        _helper.holeFlat(color, 0.0)->render(r.adjusted(2,2,-2,-2), p, TileSet::Full);
                    }

                    return;
                }

                case Generic::Text:
                {
                    KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);

                    QPen   old = p->pen();
                    if (_menuHighlightMode == MM_STRONG && flags & State_Sunken)
                        p->setPen(pal.color(QPalette::HighlightedText));
                    else
                        p->setPen(pal.color(QPalette::WindowText));
                    drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled,
                                 textOpts->text);
                    p->setPen(old);

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
                    _helper.drawFloatFrame(p, r, pal.window().color());
                    return;
                }

                case Menu::Background:
                {
                    // we paint in the eventFilter instead so we can paint in the border too
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
                    _helper.drawSeparator(p, r, pal.color(QPalette::Window), Qt::Horizontal);
                    return;
                }

                case MenuItem::ItemIndicator:
                {
                    if (enabled) {
                        QPixmap pm(r.size());
                        pm.fill(Qt::transparent);
                        QPainter pp(&pm);
                        QRect rr(QPoint(0,0), r.size());

                        QColor color = pal.color(QPalette::Window);
                        if (_menuHighlightMode == MM_STRONG)
                            color = pal.color(QPalette::Highlight);
                        else if (_menuHighlightMode == MM_SUBTLE)
                            color = KColorUtils::mix(color, KColorUtils::tint(color, pal.color(QPalette::Highlight), 0.6));
                        else
                            color = _helper.calcMidColor(color);
                        pp.setRenderHint(QPainter::Antialiasing);
                        pp.setPen(Qt::NoPen);

                        pp.setBrush(color);
                        _helper.fillHole(pp, rr);

                        _helper.holeFlat(color, 0.0)->render(rr.adjusted(2,2,-2,-2), &pp);

                        QRect maskr( visualRect(opt->direction, rr, QRect(rr.width()-40, 0, 40,rr.height())) );
                        QLinearGradient gradient(
                                visualPos(opt->direction, maskr, QPoint(maskr.left(), 0)),
                                visualPos(opt->direction, maskr, QPoint(maskr.right()-4, 0)));
                        gradient.setColorAt(0.0, QColor(0,0,0,255));
                        gradient.setColorAt(1.0, QColor(0,0,0,0));
                        pp.setBrush(gradient);
                        pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                        pp.drawRect(maskr);

                        p->drawPixmap(handleRTL(opt, r), pm);
                    }
                    else {
                        drawKStylePrimitive(WT_Generic, Generic::FocusIndicator, opt, r, pal, flags, p, widget, kOpt);
                    }

                    return;
                }

                case Generic::Text:
                {
                    KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);

                    QPen   old = p->pen();
                    if (_menuHighlightMode == MM_STRONG && flags & State_Selected)
                        p->setPen(pal.color(QPalette::HighlightedText));
                    else
                        p->setPen(pal.color(QPalette::WindowText));
                    drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled,
                                 textOpts->text);
                    p->setPen(old);

                    return;
                }

                case Generic::ArrowRight:
                case Generic::ArrowLeft:
                {
                    // always draw in window text color due to fade-out
                    extractOption<KStyle::ColorOption*>(kOpt)->color = QPalette::WindowText;
                    // fall through to lower handler
                    break;
                }

                case MenuItem::CheckColumn:
                {
                    // empty
                    return;
                }

                case MenuItem::CheckOn:
                {
                    renderCheckBox(p, r.adjusted(2,-2,2,2), pal, enabled, false, mouseOver, CheckBox::CheckOn, true);
                    return;
                }

                case MenuItem::CheckOff:
                {
                    renderCheckBox(p, r.adjusted(2,-2,2,2), pal, enabled, false, mouseOver, CheckBox::CheckOff, true);
                    return;
                }

                case MenuItem::RadioOn:
                {
                    renderRadioButton(p, r, pal, enabled, false, mouseOver, RadioButton::RadioOn, true);
                    return;
                }

                case MenuItem::RadioOff:
                {
                    renderRadioButton(p, r, pal, enabled, false, mouseOver, RadioButton::RadioOff, true);
                    return;
                }

                case MenuItem::CheckIcon:
                {
                    // TODO
                    return;
                }

                case Generic::Icon:
                {
                    p->save();
                    KStyle::IconOption* iconOpts = extractOption<KStyle::IconOption*>(kOpt);
                    QSize size = iconOpts->size;
                    if(!size.isValid()) {
                        size = QSize(pixelMetric(PM_SmallIconSize, opt, widget),
                                     pixelMetric(PM_SmallIconSize, opt, widget));
                    }
                    QImage icon;
                    if (flags & State_Enabled) {
                        if (iconOpts->active) {
                            icon = iconOpts->icon.pixmap(size, QIcon::Active).toImage();
                        } else {
                            icon = iconOpts->icon.pixmap(size, QIcon::Normal).toImage();
                        }
                    } else {
                        icon = iconOpts->icon.pixmap(size).toImage();
                        KIconEffect::deSaturate(icon, 0.8);
                        p->setOpacity(0.7);
                    }
                    p->drawImage(centerRect(r, icon.size()), icon);
                    p->restore();
                    return;
                }
            }
        }
        break;

        case WT_DockWidget:
        {
            switch (primitive)
            {
                case Generic::Text:
                {
                    const QStyleOptionDockWidget* dwOpt = ::qstyleoption_cast<const QStyleOptionDockWidget*>(opt);
                    if (!dwOpt) return;
                    const QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(opt);
                    bool verticalTitleBar = v2 ? v2->verticalTitleBar : false;

                    QRect btnr = subElementRect(dwOpt->floatable ? SE_DockWidgetFloatButton : SE_DockWidgetCloseButton, opt, widget);
                    int fw = widgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, opt, widget);
                    QRect r = dwOpt->rect.adjusted(fw, fw, -fw, -fw);
                    if (verticalTitleBar) {
                        if(btnr.isValid())
                            r.setY(btnr.y()+btnr.height());
                    }
                    else if(reverseLayout) {
                        if(btnr.isValid())
                            r.setLeft(btnr.x()+btnr.width());
                        r.adjust(0,0,-4,0);
                    } else {
                        if(btnr.isValid())
                            r.setRight(btnr.x());
                        r.adjust(4,0,0,0);
                    }

                    QString title = dwOpt->title;
                    QString tmpTitle = title;
                    if(tmpTitle.contains("&"))
                    {
                        int pos = tmpTitle.indexOf("&");
                        if(!(tmpTitle.size()-1 > pos && tmpTitle.at(pos+1) == QChar('&')))
                            tmpTitle.remove(pos, 1);
                    }
                    int tw = dwOpt->fontMetrics.width(tmpTitle);
                    int th = dwOpt->fontMetrics.height();
                    int width = verticalTitleBar ? r.height() : r.width();
                    if (width < tw)
                        title = dwOpt->fontMetrics.elidedText(title, Qt::ElideRight, width, Qt::TextShowMnemonic);

                    if (verticalTitleBar)
                    {
                        QRect br(dwOpt->fontMetrics.boundingRect(title));
                        QImage textImage(br.size(), QImage::Format_ARGB32_Premultiplied);
                        textImage.fill(0x00000000);
                        QPainter painter(&textImage);
                        drawItemText(&painter, QRect(0, 0, br.width(), br.height()), Qt::AlignLeft|Qt::AlignTop|Qt::TextShowMnemonic, dwOpt->palette, dwOpt->state & State_Enabled, title, QPalette::WindowText);
                        painter.end();
                        textImage = textImage.transformed(QMatrix().rotate(-90));

                        p->drawPixmap(r.x()+(r.width()-th)/2, r.y()+r.height()-textImage.height(), QPixmap::fromImage(textImage));
                    }
                    else
                    {
                        drawItemText(p, r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, dwOpt->palette, dwOpt->state & State_Enabled, title, QPalette::WindowText);
                    }
                    return;
                }
                case Generic::Frame:
                {
                    // Don't do anything here as it interferes with custom titlewidgets
                    return;
                }

                case DockWidget::TitlePanel:
                {
                    // The frame is draw in the eventfilter
                    // This is because when a dockwidget has a titlebarwidget, then we can not
                    //  paint on the dockwidget prober here
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
                case Generic::Text:
                {
                    KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);

                    QPen old = p->pen();
                    p->setPen(pal.color(QPalette::WindowText));
                    drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled,
                                 textOpts->text);
                    p->setPen(old);
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
                    bool hasFocus = flags & State_HasFocus;

                    renderRadioButton(p, r, pal, enabled, hasFocus, mouseOver, primitive);
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

                    if (reverseLayout)
                        renderScrollBarHole(p, QRect(r.right()+1, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal,
                                   TileSet::Top | TileSet::Bottom | TileSet::Left);
                    else
                        renderScrollBarHole(p, QRect(r.left()-5, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal,
                                   TileSet::Top | TileSet::Right | TileSet::Bottom);
                    break;

                case ScrollBar::DoubleButtonVert:
                    renderScrollBarHole(p, QRect(0, r.top()-5, r.width(), 5), pal.color(QPalette::Window), Qt::Vertical,
                               TileSet::Bottom | TileSet::Left | TileSet::Right);
                    break;

                case ScrollBar::SingleButtonHor:
                    if (reverseLayout)
                        renderScrollBarHole(p, QRect(r.left()-5, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal,
                                   TileSet::Top | TileSet::Right | TileSet::Bottom);
                    else
                        renderScrollBarHole(p, QRect(r.right()+1, 0, 5, r.height()), pal.color(QPalette::Window), Qt::Horizontal,
                                   TileSet::Top | TileSet::Left | TileSet::Bottom);
                    break;

                case ScrollBar::SingleButtonVert:
                    renderScrollBarHole(p, QRect(0, r.bottom()+3, r.width(), 5), pal.color(QPalette::Window), Qt::Vertical,
                               TileSet::Top | TileSet::Left | TileSet::Right);
                    break;

                case ScrollBar::GrooveAreaVertTop:
                {
                    renderScrollBarHole(p, r.adjusted(0,2,0,12), pal.color(QPalette::Window), Qt::Vertical, 
                            TileSet::Left | TileSet::Right | TileSet::Center | TileSet::Top);
                    return;
                }

                case ScrollBar::GrooveAreaVertBottom:
                {
                    renderScrollBarHole(p, r.adjusted(0,-10,0,0), pal.color(QPalette::Window), Qt::Vertical,
                            TileSet::Left | TileSet::Right | TileSet::Center | TileSet::Bottom);
                    return;
                }

                case ScrollBar::GrooveAreaHorLeft:
                {
                    QRect rect = (reverseLayout) ? r.adjusted(0,0,10,0) : r.adjusted(0,0,12,0);
                    renderScrollBarHole(p, rect, pal.color(QPalette::Window), Qt::Horizontal,
                            TileSet::Left | TileSet::Center | TileSet::Top | TileSet::Bottom);
                    return;
                }

                case ScrollBar::GrooveAreaHorRight:
                {
                    QRect rect = (reverseLayout) ? r.adjusted(-12,0,0,0) : r.adjusted(-10,0,0,0);
                    renderScrollBarHole(p, rect, pal.color(QPalette::Window), Qt::Horizontal,
                            TileSet::Right | TileSet::Center | TileSet::Top | TileSet::Bottom);
                    return;
                }
                case ScrollBar::SliderHor:
                {
                    renderScrollBarHandle(p, r, pal, Qt::Horizontal,
                            flags & State_MouseOver && flags & State_Enabled);
                    return;
                }
                case ScrollBar::SliderVert:
                {
                    renderScrollBarHandle(p, r, pal, Qt::Vertical,
                            flags & State_MouseOver && flags & State_Enabled);
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
                case TabBar::WestTab:
                case TabBar::EastTab:
                {
                    if (!tabOpt) break;

                    renderTab(p, r, pal, mouseOver, flags&State_Selected, tabOpt, reverseLayout);

                    return;
                }
                case TabBar::WestText:
                case TabBar::EastText:
                {
                    QImage img(r.height(), r.width(), QImage::Format_ARGB32_Premultiplied);
                    img.fill(0x00000000);
                    QPainter painter(&img);
                    drawItemText(&painter, img.rect(), (reverseLayout ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter | Qt::TextShowMnemonic, tabOpt->palette, tabOpt->state & State_Enabled, tabOpt->text, QPalette::WindowText);
                    painter.end();
                    img = img.transformed(QMatrix().rotate(primitive == TabBar::WestText ? -90 : 90));
                    p->drawImage(r.x(), r.y(), img);
                    return;
                }
                case TabBar::IndicatorTear:
                {
                    const QStyleOptionTab* option = qstyleoption_cast<const QStyleOptionTab*>(opt);
                    if(!option) return;

                    TileSet::Tiles flag;
                    QRect rect;
                    QRect br = r;
                    QRect gr = r; // fade the tab there
                    bool vertical = false;
                    QPainter::CompositionMode slabCompMode = QPainter::CompositionMode_Source;

                    switch(option->shape) {
                    case QTabBar::RoundedNorth:
                    case QTabBar::TriangularNorth:
                        if(!option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) {
                            flag = reverseLayout ? TileSet::Right : TileSet::Left;
                            rect = QRect(r.x(), r.y()+r.height()-4-7, 14+7, 4+14);
                        }
                        else {
                            flag = TileSet::Top;
                            rect = QRect(r.x()-7, r.y()+r.height()-7, 14+7, 7);
                            slabCompMode = QPainter::CompositionMode_SourceOver;
                        }
                        rect.translate(-gw,0);
                        rect = visualRect(option->direction, r, rect);
                        gr.translate(-gw,0);
                        break;
                    case QTabBar::RoundedSouth:
                    case QTabBar::TriangularSouth:
                        if(!option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) {
                            flag = reverseLayout ? TileSet::Right : TileSet::Left;
                            rect = QRect(r.x(), r.y()-7, 14+7, 2+14);
                        }
                        else {
                            flag = TileSet::Bottom;
                            rect = reverseLayout ? QRect(r.x()-7+4, r.y(), 14+3, 6) : QRect(r.x()-7, r.y()-1, 14+6, 7);
                        }
                        rect.translate(-gw,0);
                        rect = visualRect(option->direction, r, rect);
                        gr.translate(-gw,0);
                        break;
                    case QTabBar::RoundedWest:
                    case QTabBar::TriangularWest:
                        if(!option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) {
                            flag = TileSet::Top;
                            rect = QRect(r.x()+r.width()-4-7, r.y(), 4+14, 7);
                        }
                        else {
                            flag = TileSet::Left;
                            rect = QRect(r.x()+r.width()-7, r.y()-7, 7, 4+14);
                            br.adjust(0,0,-5,0);
                        }
                        vertical = true;
                        rect.translate(0,-gw);
                        gr.translate(0,-gw);
                        break;
                    case QTabBar::RoundedEast:
                    case QTabBar::TriangularEast:
                        if(!option->cornerWidgets & QStyleOptionTab::LeftCornerWidget) {
                            flag = TileSet::Top;
                            rect = QRect(r.x()-7, r.y(), 4+14, 7);
                        }
                        else {
                            flag = TileSet::Right;
                            rect = QRect(r.x(), r.y()-7, 7, 4+14);
                            br.adjust(5,0,0,0);
                        }
                        vertical = true;
                        rect.translate(0,-gw);
                        gr.translate(0,-gw);
                        break;
                    default:
                        return;
                    }

                    if(!vertical && reverseLayout)
                    {
                        if(!option->cornerWidgets & QStyleOptionTab::LeftCornerWidget)
                            gr.adjust(-4,-gr.y(),+gr.x()-4,0);
                        else
                            gr.adjust(0,-gr.y(),gr.x(),0);
                    }

                    // fade tabbar
                    QPixmap pm(gr.width(),gr.height());
                    pm.fill(Qt::transparent);
                    QPainter pp(&pm);

                    int w = 0, h = 0;
                    if (vertical) {
                        h = gr.height();
                    } else {
                        w = gr.width();
                    }
                    QLinearGradient grad(w, h, 0, 0);
                    grad.setColorAt(0, Qt::transparent);
                    grad.setColorAt(0.2, Qt::transparent);
                    grad.setColorAt(1, Qt::black);

                    _helper.renderWindowBackground(&pp, pm.rect(), widget, pal);
                    pp.setCompositionMode(QPainter::CompositionMode_DestinationAtop);
                    pp.fillRect(pm.rect(), QBrush(grad));
                    p->setCompositionMode(QPainter::CompositionMode_SourceOver);
                    p->drawPixmap(gr.topLeft(),pm);

                    renderSlab(p, rect, opt->palette.color(QPalette::Window), NoFill, flag);

                    return;
                }
                case TabBar::BaseFrame:
                {
                   const QStyleOptionTabBarBase* tabOpt = qstyleoption_cast<const QStyleOptionTabBarBase*>(opt);

                    switch(tabOpt->shape)
                    {
                        case QTabBar::RoundedNorth:
                        case QTabBar::TriangularNorth:
                        {

                            if (r.left() < tabOpt->tabBarRect.left())
                            {
                                QRect fr = r;
                                fr.setRight(tabOpt->tabBarRect.left());
                                fr.adjust(-7,-gw,7,-1-gw);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Top);
                            }
                            if (tabOpt->tabBarRect.right() < r.right())
                            {
                                QRect fr = r;
                                fr.setLeft(tabOpt->tabBarRect.right());
                                fr.adjust(-7,-gw,7,-1-gw);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Top);
                            }
                            return;
                        }
                        case QTabBar::RoundedSouth:
                        case QTabBar::TriangularSouth:
                        {
                            if (r.left() < tabOpt->tabBarRect.left())
                            {
                                QRect fr = r;
                                fr.setRight(tabOpt->tabBarRect.left());
                                fr.adjust(-7,gw,7,-1+gw);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                            }
                            if (tabOpt->tabBarRect.right() < r.right())
                            {
                                QRect fr = r;
                                fr.setLeft(tabOpt->tabBarRect.right());
                                fr.adjust(-6,gw,7,-1+gw);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                            }
                            return;
                        }
                        default:
                            break;
                    }
                    return;
                }
                case Generic::Text:
                {
                    KStyle::TextOption* textOpts = extractOption<KStyle::TextOption*>(kOpt);

                    QPen old = p->pen();
                    p->setPen(pal.color(QPalette::WindowText));
                    drawItemText(p, r, Qt::AlignVCenter | Qt::TextShowMnemonic | textOpts->hAlign, pal, flags & State_Enabled,
                                 textOpts->text);
                    p->setPen(old);
                    return;
                }
            }

        }
        break;

        case WT_TabWidget:
        {
            switch (primitive)
            {
                case Generic::Frame:
                {
                    const QStyleOptionTabWidgetFrame* tabOpt = qstyleoption_cast<const QStyleOptionTabWidgetFrame*>(opt);
                    // FIXME: tabOpt->tabBarSize may be bigger than the tab widget's size
                    int w = tabOpt->tabBarSize.width();
                    int h = tabOpt->tabBarSize.height();
                    int lw = tabOpt->leftCornerWidgetSize.width();
                    int lh = tabOpt->leftCornerWidgetSize.height();

                    switch(tabOpt->shape)
                    {
                        case QTabBar::RoundedNorth:
                        case QTabBar::TriangularNorth:
                            renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill,
                                       TileSet::Left | TileSet::Bottom | TileSet::Right);
                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode

                                if (w+lw >0)
                                    renderSlab(p, QRect(-gw, r.y()-gw, r.width() - w - lw+7+gw, 7),
                                        pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Top);
                                else
                                    renderSlab(p, QRect(-gw, r.y()-gw, r.width()+2*gw, 7), pal.color(QPalette::Window), NoFill,
                                            TileSet::Left | TileSet::Top | TileSet::Right);

                                if (lw > 0)
                                    renderSlab(p, QRect(r.right() - lw-7+gw, r.y()-gw, lw+7, 7),
                                             pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);
                            }
                            else
                            {
                                if (lw > 0)
                                    renderSlab(p, QRect(-gw, r.y()-gw, lw+7, 7), pal.color(QPalette::Window), NoFill,
                                        TileSet::Left | TileSet::Top);

                                if (w+lw >0)
                                    renderSlab(p, QRect(w+lw-7, r.y()-gw, r.width() - w - lw+7+gw, 7), pal.color(QPalette::Window), NoFill,
                                            TileSet::Top | TileSet::Right);
                                else
                                    renderSlab(p, QRect(-gw, r.y(), r.width()+2*gw, 7), pal.color(QPalette::Window), NoFill,
                                            TileSet::Left | TileSet::Top | TileSet::Right);

                            }
                            return;

                        case QTabBar::RoundedSouth:
                        case QTabBar::TriangularSouth:
                            renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill,
                                       TileSet::Left | TileSet::Top | TileSet::Right);
                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode

                                if (w+lw >0)
                                    renderSlab(p, QRect(-gw, r.bottom()-7+gw, r.width() - w - lw + 7+gw, 7),
                                        pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);
                                else
                                    renderSlab(p, QRect(-gw, r.bottom()-7+gw, r.width()+2*gw, 7), pal.color(QPalette::Window),
                                        NoFill, TileSet::Left | TileSet::Bottom | TileSet::Right);

                                if (lw > 0)
                                    renderSlab(p, QRect(r.right() - lw-7+gw, r.bottom()-7+gw, lw+7, 7),
                                        pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                            }
                            else
                            {
                                if (lw > 0)
                                    renderSlab(p, QRect(-gw, r.bottom()-7+gw, lw+7+gw, 7),
                                            pal.color(QPalette::Window), NoFill, TileSet::Left | TileSet::Bottom);

                                if (w+lw >0)
                                    renderSlab(p, QRect(w+lw-7, r.bottom()-7+gw, r.width() - w - lw+7+gw, 7),
                                            pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                                else
                                    renderSlab(p, QRect(-gw, r.bottom()-7, r.width()+2*gw, 7), pal.color(QPalette::Window),
                                        NoFill, TileSet::Left | TileSet::Bottom | TileSet::Right);

                            }
                            return;

                        case QTabBar::RoundedWest:
                        case QTabBar::TriangularWest:
                            renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill,
                                       TileSet::Top | TileSet::Right | TileSet::Bottom);

                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode
                                if (h+lh >0)
                                    renderSlab(p, QRect(r.x()-gw,  h + lh - 7, 7, r.height() - h - lh+7+gw),
                                               pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Left);
                                else
                                    renderSlab(p, QRect(r.x()-gw, r.y()-gw, r.width()+2*gw, 7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Top | TileSet::Right);

                                if (lh > 0)
                                    renderSlab(p, QRect(r.x()-gw, r.y()+gw , 7, lh+7),
                                               pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left);
                            }
                            else
                            {
                                if (lh > 0)
                                    renderSlab(p, QRect(r.x()-gw, r.y()-gw, 7, lh+7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Top);

                                if (h+lh >0)
                                    renderSlab(p, QRect(r.x()-gw, r.y()+h+lh-7, 7, r.height() - h - lh+7+gw), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Bottom);
                                else
                                    renderSlab(p, QRect(r.x()-gw, r.y()-gw, 7, r.height()+2*gw), pal.color(QPalette::Window), NoFill,
                                               TileSet::Top | TileSet::Left | TileSet::Bottom);
                            }

                            return;

                        case QTabBar::RoundedEast:
                        case QTabBar::TriangularEast:
                            renderSlab(p, r.adjusted(-gw,-gw,gw,gw), pal.color(QPalette::Window), NoFill,
                                       TileSet::Top | TileSet::Left | TileSet::Bottom);
                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode
                                if (h+lh >0)
                                    renderSlab(p, QRect(r.right()+1-7+gw,  h + lh - 7, 7, r.height() - h - lh+7+gw),
                                               pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                                else
                                    renderSlab(p, QRect(r.right()+1-7+gw, r.y()-gw, r.width()+2*gw, 7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Top | TileSet::Right);

                                if (lh > 0)
                                    renderSlab(p, QRect(r.right()+1-7+gw, r.y()+gw , 7, lh+7),
                                               pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);
                            }
                            else
                            {
                                if (lh > 0)
                                    renderSlab(p, QRect(r.right()+1-7+gw, r.y()-gw, 7, lh+7+gw), pal.color(QPalette::Window), NoFill,
                                               TileSet::Top | TileSet::Right);

                                if (h+lh >0)
                                    renderSlab(p, QRect(r.right()+1-7+gw, r.y()+h+lh-7, 7, r.height() - h - lh+7+gw), pal.color(QPalette::Window), NoFill,
                                               TileSet::Bottom | TileSet::Right);
                                else
                                    renderSlab(p, QRect(r.x()-gw, r.y()-gw, 7, r.height()+2*gw), pal.color(QPalette::Window), NoFill,
                                               TileSet::Top | TileSet::Right | TileSet::Bottom);
                            }

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
                    _helper.drawFloatFrame(p, r, pal.window().color());
                    return;
                }

                case Window::TitlePanel:
                {
                    return;
                }

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
                    //if (tbkOpts->active)
                    //    bflags |= State_Sunken;
                    //drawKStylePrimitive(WT_ToolButton, ToolButton::Panel, opt, r, pal, bflags, p, widget);
                    p->drawPixmap(r.topLeft(), _helper.windecoButton(pal.button().color(), tbkOpts->active,  r.height()));
                    p->setRenderHints(QPainter::Antialiasing);
                    p->setBrush(Qt::NoBrush);
                    QLinearGradient lg = _helper.decoGradient(QRect(3,3,11,11), QColor(0,0,0));
                    p->setPen(QPen(lg, 1.4));
                    renderWindowIcon(p, QRectF(r).adjusted(-2.5,-2.5,0,0), primitive);

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
                        renderDot(p, QPointF(r.left()+1, center-3), color);
                        renderDot(p, QPointF(r.left()+1, center), color);
                        renderDot(p, QPointF(r.left()+1, center+3), color);
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
                        renderDot(p, QPointF(center-3, r.top()+1), color);
                        renderDot(p, QPointF(center, r.top()+1), color);
                        renderDot(p, QPointF(center+3, r.top()+1), color);
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
                        _helper.groove(pal.color(QPalette::Window), 0.0)->render(
                                    QRect(r.left()+4, center-2, r.width()-8, 5), p);
                    } else {
                        int center = r.x()+r.width()/2;
                        _helper.groove(pal.color(QPalette::Window), 0.0)->render(
                                    QRect(center-2, r.top()+4, 5, r.height()-8), p);

                    }

                    return;
                }
            }

        }
        break;

        case WT_SpinBox:
        {
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

#ifdef HOLE_NO_EDGE_FILL
                    p->fillRect(fr.adjusted(3,3,-3,-3), inputColor);
#else
                    _helper.fillHole(*p, r);
#endif

                    p->restore();
                    // TODO use widget background role?
                    // We really need the color of the widget behind to be "right",
                    // but the shadow needs to be colored as the inner widget; needs
                    // changes in helper.
#ifdef HOLE_COLOR_OUTSIDE
                    renderHole(p, pal.color(QPalette::Window), fr, hasFocus, mouseOver);
#else
                    renderHole(p, inputColor, fr, hasFocus, mouseOver);
#endif
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

            const QColor buttonColor = enabled?pal.color(QPalette::Button):pal.color(QPalette::Window);
            const QColor inputColor = enabled ? pal.color(QPalette::Base) : pal.color(QPalette::Window);
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

#ifdef HOLE_NO_EDGE_FILL
                        p->fillRect(fr.adjusted(3,3,-3,-3), inputColor);
#else
                        _helper.fillHole(*p, r.adjusted(0,0,0,-1));
#endif

                        p->restore();

#ifdef HOLE_COLOR_OUTSIDE
                        if (hasFocus && enabled)
                        {
                            renderHole(p, pal.color(QPalette::Window), fr, true, mouseOver);
                        }
                        else
                        {
                            renderHole(p, pal.color(QPalette::Window), fr, false, mouseOver);
                        }
#else
                        if (hasFocus && enabled)
                        {
                            renderHole(p, inputColor, fr, true, mouseOver);
                        }
                        else
                        {
                            renderHole(p, inputColor, fr, false, mouseOver);
                        }
#endif
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
                                int pos = (reverseLayout)? r.left()+1 : r.right()-1;
                                renderDot(p, QPointF(pos, center-3), color);
                                renderDot(p, QPointF(pos, center), color);
                                renderDot(p, QPointF(pos, center+3), color);
                            }
                        }
                        else
                        {
                            int center = r.center().x();
                            int pos = r.bottom()-1;
                            renderDot(p, QPointF(center-3, pos), color);
                            renderDot(p, QPointF(center, pos), color);
                            renderDot(p, QPointF(center+3, pos), color);
                        }
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
                            drawKStylePrimitive(WT_Generic, reverseLayout? Generic::ArrowLeft : Generic::ArrowRight, opt, QRect(r.x()+1,r.y()+1,r.width(),r.height()), pal, flags, p, widget);
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
#ifdef HOLE_COLOR_OUTSIDE
                    const QColor inputColor =  pal.color(QPalette::Window);
#else
                    const QColor inputColor = enabled?pal.color(QPalette::Base):pal.color(QPalette::Window);
#endif
                    if (hasFocus && !isReadOnly && isEnabled)
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
                            _helper.fillHole(*p, r.adjusted(0,0,-0,-1));
#endif
                            drawPrimitive(PE_FrameLineEdit, panel, p, widget);

                            p->restore();
                        }
                        else
                        {
                            p->fillRect(r.adjusted(2,2,-2,-1), inputBrush);
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
				case GroupBox::FlatFrame:
				{
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
                        QColor color = pal.color(QPalette::Window);
                        if(flags & State_Horizontal)
                            _helper.drawSeparator(p, r, color, Qt::Vertical);
                        else
                            _helper.drawSeparator(p, r, color, Qt::Horizontal);
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
                    QRect slitRect = r;
                    const QToolButton* t=dynamic_cast<const QToolButton*>(widget);
                    if (t && !t->autoRaise())
                    {
                        StyleOptions opts = 0;

                        if (const QTabBar *tb =  dynamic_cast<const QTabBar*>(t->parent()))
                        {
                            bool horizontal = true;
                            bool northOrEast = true;
                            switch(tb->shape())
                            {
                                case QTabBar::RoundedNorth:
                                case QTabBar::TriangularNorth:
                                    break;
                                case QTabBar::RoundedSouth:
                                case QTabBar::TriangularSouth:
                                    northOrEast = false;
                                    break;
                                case QTabBar::RoundedEast:
                                case QTabBar::TriangularEast:
                                    horizontal = false;
                                    break;
                                case QTabBar::RoundedWest:
                                case QTabBar::TriangularWest:
                                    northOrEast = false;
                                    horizontal = false;
                                    break;
                                default:
                                    break;
                            }
                            int gw=2;
                            if (horizontal)
                            {
                                if (northOrEast) // north
                                {
                                    slitRect.adjust(0,3,0,-3-gw);
                                    _helper.renderWindowBackground(p, r.adjusted(0,2-gw,0,-1), t, t->window()->palette());
                                    renderSlab(p, QRect(r.left()-7, r.bottom()-6-gw, r.width()+14, 2), pal.color(QPalette::Window), NoFill, TileSet::Top);
                                }
                                else // south
                                {
                                    slitRect.adjust(0,3+gw,0,-3);
                                    _helper.renderWindowBackground(p, r.adjusted(0,2+gw,0,0), t, t->window()->palette());
                                    renderSlab(p, QRect(r.left()-7, r.top()+4+gw, r.width()+14, 2), pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                                }
                            }
                            else
                            {
                                if (northOrEast) // east
                                {
                                    slitRect.adjust(3+gw,0,-3-gw,0);
                                    _helper.renderWindowBackground(p, r.adjusted(2+gw,0,-2,0), t, t->window()->palette());
                                    renderSlab(p, QRect(r.left()+5+gw, r.top()-7, 2, r.height()+14), pal.color(QPalette::Window), NoFill, TileSet::Right);
                                }
                                else // west
                                {
                                    slitRect.adjust(3+gw,0,-3-gw,0);
                                    _helper.renderWindowBackground(p, r.adjusted(2-gw,0,-2,0), t, t->window()->palette());
                                    renderSlab(p, QRect(r.right()-6-gw, r.top()-7, 2, r.height()+14), pal.color(QPalette::Window), NoFill, TileSet::Left);
                                }
                            }
                            // continue drawing the slit
                        }
                        else
                        {
                            if ((flags & State_On) || (flags & State_Sunken))
                                opts |= Sunken;
                            if (flags & State_HasFocus)
                                opts |= Focus;
                            if (enabled && (flags & State_MouseOver))
                                opts |= Hover;
                            
                            if (t->popupMode()==QToolButton::MenuButtonPopup) {
                                renderSlab(p, r.adjusted(0,0,4,0), pal.color(QPalette::Button), opts, TileSet::Bottom | TileSet::Top | TileSet::Left);
                            } else
                                renderSlab(p, r, pal.color(QPalette::Button), opts);
                            return;
                        }
                    }

                    bool hasFocus = flags & State_HasFocus;

                    if((flags & State_Sunken) || (flags & State_On) )
                    {
                        renderHole(p, pal.color(QPalette::Window), slitRect, hasFocus, mouseOver);
                    }
                    else if (hasFocus || mouseOver)
                    {
                        TileSet *tile;
                        tile = _helper.slitFocused(_viewFocusBrush.brush(QPalette::Active).color());
                        tile->render(slitRect, p);
                    }
                    return;
                }
            }

        }
        break;

        case WT_Limit: //max value for the enum, only here to silence the compiler
        case WT_Generic: // handled below since the primitives arevalid for all WT_ types
            break;
    }


    // Arrows
    if (primitive >= Generic::ArrowUp && primitive <= Generic::ArrowLeft) {
        QPolygonF a;
        QPen oldPen(p->pen()); // important to save the pen as combobox assumes we don't touch
        QLinearGradient arrowGradient;

        switch (primitive) {
            case Generic::ArrowUp: {
                a << QPointF( -3,2.5) << QPointF(0.5, -1.5) << QPointF(4,2.5);
                arrowGradient = QLinearGradient(QPoint(0,-1.5),QPoint(0,2.5));
                break;
            }
            case Generic::ArrowDown: {
                a << QPointF( -3,-2.5) << QPointF(0.5, 1.5) << QPointF(4,-2.5);
                arrowGradient = QLinearGradient(QPoint(0,-1.5),QPoint(0,2.5));
              break;
            }
            case Generic::ArrowLeft: {
                a << QPointF(2.5,-3) << QPointF(-1.5, 0.5) << QPointF(2.5,4);
                arrowGradient = QLinearGradient(QPoint(0,-3),QPoint(0,4));
                break;
            }
            case Generic::ArrowRight: {
                a << QPointF(-2.5,-3) << QPointF(1.5, 0.5) << QPointF(-2.5,4);
                arrowGradient = QLinearGradient(QPoint(0,-3),QPoint(0,4));
                break;
            }
        }
        qreal penThickness = 2.2;

        if (const QToolButton *tool = dynamic_cast<const QToolButton *>(widget)) {
            if (tool->popupMode()==QToolButton::MenuButtonPopup) {
                if(!tool->autoRaise()) {
                    if ((flags & State_On) || (flags & State_Sunken))
                        opts |= Sunken;
                    if (flags & State_HasFocus)
                        opts |= Focus;
                    if (enabled && (flags & State_MouseOver))
                        opts |= Hover;
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
                }
            }
            else {
                // smaller down arrow for menu indication on toolbuttons
                penThickness = 1.7;
                a.clear();
                // NOTE: is there any smarter solution than this?
                switch (primitive)
                {
                    case Generic::ArrowUp: {
                        a << QPointF( -2,1.5) << QPointF(0.5, -1.5) << QPointF(3,1.5);
                        arrowGradient = QLinearGradient(QPoint(0,-1.5),QPoint(0,1.5));
                        break;
                    }
                    case Generic::ArrowDown: {
                        a << QPointF( -2,-1.5) << QPointF(0.5, 1.5) << QPointF(3,-1.5);
                        arrowGradient = QLinearGradient(QPoint(0,-1.5),QPoint(0,1.5));
                        break;
                    }
                    case Generic::ArrowLeft: {
                        a << QPointF(1.5,-2) << QPointF(-1.5, 0.5) << QPointF(1.5,3);
                        arrowGradient = QLinearGradient(QPoint(0,-2),QPoint(0,3));
                        break;
                    }
                    case Generic::ArrowRight: {
                        a << QPointF(-1.5,-2) << QPointF(1.5, 0.5) << QPointF(-1.5,3);
                        arrowGradient = QLinearGradient(QPoint(0,-2),QPoint(0,3));
                        break;
                    }
                }
            }
        }

        p->translate(int(r.x()+r.width()/2), int(r.y()+r.height()/2));

        KStyle::ColorOption* colorOpt   = extractOption<KStyle::ColorOption*>(kOpt);
        QColor  arrowColor = colorOpt->color.color(pal);

        arrowGradient.setColorAt(0.0, arrowColor);
        arrowGradient.setColorAt(0.8, KColorUtils::mix(pal.color(QPalette::Window), arrowColor, 0.6));

        // white reflection
        p->translate(0,1);
        p->setPen(QPen(_helper.calcLightColor(pal.color(QPalette::Window)),
                       penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p->setRenderHint(QPainter::Antialiasing);
        p->drawPolyline(a);
        p->translate(0,-1);

        p->setPen(QPen(arrowGradient, penThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p->drawPolyline(a);

        p->setRenderHint(QPainter::Antialiasing, false);
        p->setPen(oldPen);
        p->translate(-int(r.x()+r.width()/2), -int(r.y()+r.height()/2));
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
                // TODO use widget background role? - probably not
                //renderHole(p, pal.color(widget->backgroundRole()), r, focusHighlight);
                renderHole(p, pal.color(QPalette::Window), r, focusHighlight);
            } else
                if(widgetType == WT_Generic && (flags & State_Raised)) {
                    renderSlab(p, r.adjusted(-2, -2, 2, 2), pal.color(QPalette::Background), NoFill);
                }
                break; // do the default thing
        }

        case Generic::FocusIndicator:
        {
            const QAbstractItemView *aiv = qobject_cast<const QAbstractItemView*>(widget);
            if (aiv && opt && (opt->state & QStyle::State_Item)
                         && (aiv->selectionMode() != QAbstractItemView::SingleSelection))
            {
                QPen pen(_viewFocusBrush.brush(QPalette::Active).color());
                pen.setWidth(0);
                pen.setStyle(Qt::DotLine);
                p->setPen(pal.color(QPalette::Base));
                p->drawRect(r.adjusted(0,0,-1,-1));
                p->setPen(pen);
                p->drawRect(r.adjusted(0,0,-1,-1));
            }
            // we don't want the stippled focus indicator in oxygen
            return;
        }

        default:
            break;
    }

    // default fallback
    KStyle::drawKStylePrimitive(widgetType, primitive, opt,
                                r, pal, flags, p, widget, kOpt);
}

void OxygenStyle::polish(QWidget* widget)
{
    if (!widget) return;

    switch (widget->windowFlags() & Qt::WindowType_Mask) {
        case Qt::Window:
        case Qt::Dialog:
            widget->installEventFilter(this);
            widget->setAttribute(Qt::WA_StyledBackground);
            break;
        case Qt::Popup: // we currently don't want that kind of gradient on menus etc
        case Qt::Tool: // this we exclude as it is used for dragging of icons etc
        default:
            break;
    }

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
        || qobject_cast<QLineEdit*>(widget)
        ) {
        widget->setAttribute(Qt::WA_Hover);
    }

    if (qobject_cast<QMenuBar*>(widget))
    {
        widget->setBackgroundRole(QPalette::NoRole);
    }
    else if (widget->inherits("Q3ToolBar")
        || qobject_cast<QToolBar*>(widget)
        || qobject_cast<QToolBar *>(widget->parent()))
    {
        widget->setBackgroundRole(QPalette::NoRole);
        widget->setContentsMargins(0,0,0,2);
        widget->installEventFilter(this);
    }
    else if (qobject_cast<QScrollBar*>(widget) )
    {
        widget->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }
    else if (qobject_cast<QDockWidget*>(widget))
    {
        widget->setContentsMargins(4,3,4,4);
        widget->installEventFilter(this);
    }
    else if (qobject_cast<QToolBox*>(widget))
    {
        widget->setBackgroundRole(QPalette::NoRole);
        widget->setAutoFillBackground(false);
        widget->setContentsMargins(5,5,5,5);
        widget->installEventFilter(this);
    }
    else if (widget->parentWidget() && widget->parentWidget()->parentWidget() && qobject_cast<QToolBox*>(widget->parentWidget()->parentWidget()->parentWidget()))
    {
        widget->setBackgroundRole(QPalette::NoRole);
        widget->setAutoFillBackground(false);
        widget->parentWidget()->setAutoFillBackground(false);
    }
    else if (qobject_cast<QMenu*>(widget) 
            || qobject_cast<QFrame*>(widget) 
            || qobject_cast<QMdiSubWindow*>(widget))
    {
        widget->installEventFilter(this);
    }
    else if (widget->inherits("QComboBoxPrivateContainer"))
    {
        widget->installEventFilter(this);
        // note, it doesn't help to do a setContentsMargin()
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
        || qobject_cast<QLineEdit*>(widget)
    ) {
        widget->setAttribute(Qt::WA_Hover, false);
    }

    if (qobject_cast<QMenuBar*>(widget)
        || (widget && widget->inherits("Q3ToolBar"))
        || qobject_cast<QToolBar*>(widget)
        || (widget && qobject_cast<QToolBar *>(widget->parent()))
        || qobject_cast<QToolBox*>(widget))
    {
        widget->setBackgroundRole(QPalette::Button);
    }

    if (qobject_cast<QScrollBar*>(widget))
    {
        widget->setAttribute(Qt::WA_OpaquePaintEvent);
    }
    else if (qobject_cast<QDockWidget*>(widget))
    {
        widget->setContentsMargins(0,0,0,0);
    }
    else if (qobject_cast<QToolBox*>(widget))
    {
        widget->setBackgroundRole(QPalette::Button);
        widget->setContentsMargins(0,0,0,0);
        widget->removeEventFilter(this);
    }
    else if (qobject_cast<QMenu*>(widget))
    {
        widget->setAttribute(Qt::WA_PaintOnScreen, false);
        widget->setAttribute(Qt::WA_NoSystemBackground, false);
        widget->removeEventFilter(this);
    }
    else if (qobject_cast<QFrame*>(widget)
            || qobject_cast<QMdiSubWindow*>(widget))
    {
        widget->removeEventFilter(this);
    }
    else if (widget->inherits("QComboBoxPrivateContainer"))
    {
        widget->removeEventFilter(this);
    }
    KStyle::unpolish(widget);
}

void OxygenStyle::progressBarDestroyed(QObject* obj)
{
    progAnimWidgets.remove(static_cast<QWidget*>(obj));
    //the timer updates will stop next time if this was the last visible one
}

void OxygenStyle::globalSettingsChange(int type, int /*arg*/)
{
    if (type == KGlobalSettings::PaletteChanged) {
        _helper.reloadConfig();
        _viewFocusBrush = KStatefulBrush( KColorScheme::View, KColorScheme::FocusColor, _config );
        _viewHoverBrush = KStatefulBrush( KColorScheme::View, KColorScheme::HoverColor, _config );
    }
}

void OxygenStyle::renderSlab(QPainter *p, QRect r, const QColor &color, StyleOptions opts, TileSet::Tiles tiles) const
{
    if ((r.width() <= 0) || (r.height() <= 0))
        return;

    TileSet *tile;

    if (opts & Sunken)
        r.adjust(-1,0,1,2); // the tiles of sunken slabs look different so this is needed (also for the fill)

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

void OxygenStyle::renderScrollBarHole(QPainter *p, const QRect &r, const QColor &color,
                                   Qt::Orientation orientation, TileSet::Tiles tiles) const
{
    _helper.scrollHole(
            color,
            orientation)->render(r, p, tiles);
}

void OxygenStyle::renderScrollBarHandle(QPainter *p, const QRect &r, const QPalette &pal,
                               Qt::Orientation orientation, bool hover) const
{
    p->setRenderHints(QPainter::Antialiasing);
    QColor color = pal.color(QPalette::Button);
    QColor light = _helper.calcLightColor(color);
    QColor mid = _helper.calcMidColor(color);
    QColor dark = _helper.calcDarkColor(color);
    QColor shadow = _helper.calcShadowColor(color);
    bool horizontal = orientation == Qt::Horizontal;

    // draw the hole as background
    renderScrollBarHole(p, (orientation == Qt::Horizontal) ? r.adjusted(-4,0,4,0) : r.adjusted(0,-3,0,4),
            pal.color(QPalette::Window), orientation, 
            horizontal ? TileSet::Top | TileSet::Bottom | TileSet::Center
                       : TileSet::Left | TileSet::Right | TileSet::Center);

    // draw the slider itself
    QRectF rect = r.adjusted(3, horizontal ? 2 : 4, -3, -3);

    // gradients
    QLinearGradient sliderGradient( rect.topLeft(), horizontal ? rect.bottomLeft() : rect.topRight());
    sliderGradient.setColorAt(0.0, color);
    sliderGradient.setColorAt(1.0, mid);

    QLinearGradient bevelGradient( rect.topLeft(), horizontal ? rect.topRight() : rect.bottomLeft());
    bevelGradient.setColorAt(0.0, Qt::transparent);
    bevelGradient.setColorAt(0.5, light);
    bevelGradient.setColorAt(1.0, Qt::transparent);

    QPoint offset = horizontal ? QPoint(-rect.left(), 0) : QPoint(0, -rect.top()); // don't let the pattern move
    QPoint periodEnd = offset + (horizontal ? QPoint(30, 0) : QPoint(0, 30));
    QLinearGradient patternGradient(rect.topLeft()+offset, rect.topLeft()+periodEnd);
    patternGradient.setColorAt(0.0, _helper.alphaColor(shadow, 0.1));
    patternGradient.setColorAt(1.0, _helper.alphaColor(light, 0.1));
    patternGradient.setSpread(QGradient::ReflectSpread);

    // draw the slider
    QColor glowColor = hover? _viewHoverBrush.brush(QPalette::Active).color()
                            : KColorUtils::mix(dark, shadow, 0.5);
    // glow / shadow
    p->setPen(Qt::NoPen);
    p->setBrush(_helper.alphaColor(glowColor, 0.6));
    p->drawRoundedRect(rect.adjusted(-0.8,-0.8,0.8,0.8), 3, 3);
    p->setPen(QPen(
                _helper.alphaColor(glowColor, 0.3),
                1.5));
    if (horizontal)
        p->drawRoundedRect(rect.adjusted(-1.2,-0.8,1.2,0.8), 3, 3);
    else
        p->drawRoundedRect(rect.adjusted(-0.8,-1.2,0.8,1.2), 3, 3);

    // slider
    p->setPen(Qt::NoPen);
    p->setBrush(sliderGradient);
    p->drawRoundedRect(rect, 2, 2);

    // pattern
    p->setBrush(patternGradient);
    p->drawRoundedRect(rect, 2, 2);

    // bevel
    rect.adjust(0.5, 0.5, -0.5, -0.5); // for sharper lines
    p->setPen(QPen(bevelGradient, 1.0));
    p->drawLine(rect.topLeft(), horizontal ? rect.topRight() : rect.bottomLeft());
    p->drawLine(rect.bottomRight(), horizontal ? rect.bottomLeft() : rect.topRight());
}

// TODO take StyleOptions instead of ugly bools
void OxygenStyle::renderCheckBox(QPainter *p, const QRect &rect, const QPalette &pal,
                                 bool enabled, bool hasFocus, bool mouseOver, int primitive,
                                 bool sunken) const
{
    Q_UNUSED(enabled);

    int s = qMin(rect.width(), rect.height());
    QRect r = centerRect(rect, s, s);

    StyleOptions opts;
    if (hasFocus) opts |= Focus;
    if (mouseOver) opts |= Hover;

    if(sunken)
    {
        QColor color = pal.color(QPalette::Window);
        _helper.holeFlat(color, 0.0)->render(r, p, TileSet::Full);
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
            QVector<qreal> dashes;
            if (_checkCheck) {
                dashes << 1.0 << 2.0;
                pen.setWidthF(1.3);
            }
            else {
                dashes << 0.4 << 2.0;
            }
            pen.setDashPattern(dashes);
        }

        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(pen);
        if (_checkCheck) {
            p->drawLine(QPointF(x+9, y), QPointF(x+3,y+7));
            p->drawLine(QPointF(x, y+4), QPointF(x+3,y+7));
        }
        else {
            if (sunken) {
                p->drawLine(QPointF(x+8, y), QPointF(x+1,y+7));
                p->drawLine(QPointF(x+8, y+7), QPointF(x+1,y));
            }
            else {
                p->drawLine(QPointF(x+8, y-1), QPointF(x,y+7));
                p->drawLine(QPointF(x+8, y+7), QPointF(x,y-1));
            }
        }
        p->setRenderHint(QPainter::Antialiasing, false);
    }
}

void OxygenStyle::renderRadioButton(QPainter *p, const QRect &r, const QPalette &pal,
                                        bool enabled, bool hasFocus, bool mouseOver, int prim,
                                   bool drawButton) const
{
    Q_UNUSED(enabled);

    int s = widgetLayoutProp(WT_RadioButton, RadioButton::Size);
    QRect r2(r.x() + (r.width()-s)/2, r.y() + (r.height()-s)/2, s, s);
    int x = r2.x();
    int y = r2.y();

    if(mouseOver || hasFocus)
    {
        QPixmap slabPixmap = _helper.roundSlabFocused(pal.color(QPalette::Button),
                    (mouseOver ? _viewHoverBrush : _viewFocusBrush).brush(QPalette::Active).color(), 0.0);
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
                            const QStyleOptionTabV2 *tabOpt,
                            const bool reverseLayout) const
{
    const QStyleOptionTab::TabPosition pos = tabOpt->position;
    const bool northAlignment = tabOpt->shape == QTabBar::RoundedNorth || tabOpt->shape == QTabBar::TriangularNorth;
    const bool southAlignment = tabOpt->shape == QTabBar::RoundedSouth || tabOpt->shape == QTabBar::TriangularSouth;
    const bool westAlignment = tabOpt->shape == QTabBar::RoundedWest || tabOpt->shape == QTabBar::TriangularWest;
    const bool eastAlignment = tabOpt->shape == QTabBar::RoundedEast || tabOpt->shape == QTabBar::TriangularEast;
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
    const bool isLeftMost =  (reverseLayout && !(westAlignment || eastAlignment) ?
                            (tabOpt->position == QStyleOptionTab::End) :
                            (tabOpt->position == QStyleOptionTab::Beginning)) ||
                                tabOpt->position == QStyleOptionTab::OnlyOneTab;
    const bool isRightMost =  reverseLayout && !(westAlignment || eastAlignment) ?
                            (tabOpt->position == QStyleOptionTab::Beginning) :
                            (tabOpt->position == QStyleOptionTab::End) ||
                                tabOpt->position == QStyleOptionTab::OnlyOneTab;
    const bool isFrameAligned =  reverseLayout && !(westAlignment || eastAlignment) ?
        (isRightMost && ! (tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget)) :
        (isLeftMost && ! (tabOpt->cornerWidgets & QStyleOptionTab::LeftCornerWidget));
    const QColor midColor = _helper.alphaColor(_helper.calcDarkColor(pal.color(QPalette::Window)), 0.4);
    const QColor darkColor = _helper.alphaColor(_helper.calcDarkColor(pal.color(QPalette::Window)), 0.6);

    if(northAlignment || southAlignment) {
        // the tab part of the tab - ie subtracted the fairing to the frame
        QRect Rc = southAlignment ? r.adjusted(-gw,6+gw,gw,gw) : r.adjusted(-gw,-gw,gw,-7-gw);

        // the area where the fairing should appear
        QRect Rb(Rc.x(), southAlignment?r.top()+gw:Rc.bottom()+1, Rc.width(), r.height()-Rc.height() );


        // FIXME - maybe going to redo tabs
        if (selected) {
            int x,y,w,h;
            r.getRect(&x, &y, &w, &h);

            if(southAlignment)
                renderSlab(p, Rc.adjusted(0,-7,0,0), pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Left | TileSet::Right);
            else
                renderSlab(p, Rc.adjusted(0,0,0,7), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left | TileSet::Right);

            // some "position specific" paintings...
            // First draw the left connection from the panel border to the tab
            if(isFirst && !reverseLayout && !leftCornerWidget) {
                renderSlab(p, Rb.adjusted(0,-7,0,7), pal.color(QPalette::Window), NoFill, TileSet::Left);
            } else {
                TileSet *tile = _helper.slabInverted(pal.color(QPalette::Window), 0.0);
                if(southAlignment)
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
                if(southAlignment)
                    tile->render(QRect(Rb.right()-6, Rb.top()-1,12,13), p, TileSet::Left | TileSet::Top);
                else
                    tile->render(QRect(Rb.right()-6, Rb.top()-5,12,12), p, TileSet::Left | TileSet::Bottom);
            }
        } else {
            
            // inactive tabs
            int x,y,w,h;
            p->save(); // we only use the clipping and AA for inactive tabs
            p->setPen(darkColor);
            p->setBrush(midColor);
            p->setRenderHints(QPainter::Antialiasing);

            if (northAlignment) {
                r.adjusted(0,5-gw,0,-gw).getRect(&x, &y, &w, &h);
                p->setClipRect(x-4, y, w+8, h-5); // don't intersect the translucent border of the slab
                p->setClipRect(x, y, w, h, Qt::UniteClip);
                if(isLeftMost) {
                    QPainterPath path;
                    x-=gw;
                    w+=gw;
                    path.moveTo(x+2.5, y+h-2-(isFrameAligned ? 0 : 2));
                    path.lineTo(x+2.5, y+2.5); // left border
                    path.arcTo(QRectF(x+2.5, y+0.5, 9, 9), 180, -90); // top-left corner
                    path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+0.5)); // top border
                    path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+h-4)); // to complete the path.
                    p->drawPath(path);
                } else if(isRightMost) {
                    QPainterPath path;
                    w+=gw;
                    path.moveTo(x+w-2.5, y+h-2-(isFrameAligned?0:2));
                    path.lineTo(x+w-2.5, y+2.5); // right border
                    path.arcTo(QRectF(x+w-9-2.5, y+0.5, 9, 9), 0, 90); // top-right corner
                    path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+0.5)); // top border
                    path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+h-4)); // to complete the path.
                    p->drawPath(path);
                } else {
                    // top border
                    p->drawLine(QPointF(x-(isRightOfSelected?2:0), y+0.5), QPointF(x+w+(isRightOfSelected?2:0)+(isLeftOfSelected?2:0), y+0.5));
                    if(!isLeftOfSelected)
                        p->drawLine(QPointF(x+w+0.5, y+1.5), QPointF(x+w+0.5, y+h-4));
                    p->fillRect(x-(isRightOfSelected ? 2 : 0), y+1, w+(isLeftOfSelected||isRightOfSelected ? (isRightOfSelected ? 3 : 3-gw) : 0), h-5, midColor);
                }
            }
            else { // southAlignment
                r.adjusted(0,gw,0,-5+gw).getRect(&x, &y, &w, &h);
                if(isLeftMost) {
                    QPainterPath path;
                    x-=gw;
                    w+=gw;
                    path.moveTo(x+2.5, y+2+(isFrameAligned ? 0 : 2));
                    path.lineTo(x+2.5, y+h-2.5); // left border
                    path.arcTo(QRectF(x+2.5, y+h-9.5, 9, 9), 180, 90); // bottom-left corner
                    path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+h-0.5)); // bottom border
                    path.lineTo(QPointF(x+w-0.5+(isLeftOfSelected?4-gw:0), y+4)); // to complete the path.
                    p->drawPath(path);
                } else if(isRightMost) {
                    QPainterPath path;
                    w+=gw;
                    path.moveTo(x+w-2.5, y+2+(isFrameAligned ?0:2));
                    path.lineTo(x+w-2.5, y+h-2.5); // right border
                    path.arcTo(QRectF(x+w-9-2.5, y+h-9.5, 9, 9), 0, -90); // bottom-right corner
                    path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+h-0.5)); // bottom border
                    path.lineTo(QPointF(x+0.5-(isRightOfSelected?4-gw:0), y+4)); // to complete the path.
                    p->drawPath(path);
                } else {
                    // bottom border
                    p->drawLine(QPointF(x-(isRightOfSelected?2:0), y+h-0.5), QPointF(x+w+(isRightOfSelected ?2:0)+(isLeftOfSelected ?2:0), y+h-0.5));
                    if(!isLeftOfSelected)
                        p->drawLine(QPointF(x+w+0.5, y+1.5), QPointF(x+w+0.5, y+h-4));
                    p->fillRect(x, y+1, w, h-2, midColor);
                }
            }
            p->restore();

            TileSet::Tiles posFlag = southAlignment?TileSet::Bottom:TileSet::Top;
            QRect Ractual(Rb.left(), Rb.y(), Rb.width(), 6);

            if(isLeftMost) {
                if(isFrameAligned)
                    posFlag |= TileSet::Left;
                // fix, to keep the mouseover line within the tabs (drawn) boundary
                if(reverseLayout || !isFrameAligned) {
                    renderSlab(p, QRect(Ractual.left()-7, Ractual.y(), 2+14, Ractual.height()), pal.color(QPalette::Window), NoFill, posFlag);
                    Ractual.adjust(-5,0,0,0);
                }
            }
            else
                Ractual.adjust(-7+gw,0,0,0);

            if(isRightMost) {
                if(isFrameAligned)
                    posFlag |= TileSet::Right;
                // fix, to keep the mouseover line within the tabs (drawn) boundary
                if(reverseLayout && !isFrameAligned) {
                    renderSlab(p, QRect(Ractual.left()+Ractual.width()-2-7, Ractual.y(), 1+14, Ractual.height()), pal.color(QPalette::Window), NoFill, posFlag);
                    Ractual.adjust(0,0,5,0);
                }
                else if(!isFrameAligned) {
                    renderSlab(p, QRect(Ractual.left()+Ractual.width()-2-7, Ractual.y(), 2+14, Ractual.height()), pal.color(QPalette::Window), NoFill, posFlag);
                    Ractual.adjust(0,0,5,0);
                }
            }
            else
                Ractual.adjust(0,0,7-gw,0);

            if (mouseOver)
                renderSlab(p, Ractual, pal.color(QPalette::Window), NoFill| Hover, posFlag);
            else
                renderSlab(p, Ractual, pal.color(QPalette::Window), NoFill, posFlag);


            // TODO mouseover effects
        }
    }
     // westAlignment and eastAlignment
    else {
        // the tab part of the tab - ie subtracted the fairing to the frame
        QRect Rc = eastAlignment ? r.adjusted(7+gw,-gw,gw,gw) : r.adjusted(-gw,-gw,-7-gw,gw);
        // the area where the fairing should appear
        const QRect Rb(eastAlignment ? r.x()+gw: Rc.right()+1, Rc.top(), r.width()-Rc.width(), Rc.height() );

        if (selected) {
            int x,y,w,h;
            r.getRect(&x, &y, &w, &h);

            // parts of the adjacent tabs
            if(!isSingle && ((!reverseLayout && !isFirst) || (reverseLayout && !isFirst))) {
                p->setPen(darkColor);
                if(eastAlignment) {
                    p->fillRect(x+5, y, w-10, 2, midColor);
                    p->drawLine(QPointF(x+w-5-1, y), QPointF(x+w-5-1, y+2));
                }
                else {
                    p->fillRect(x+5, y, w-10, 2, midColor);
                    p->drawLine(QPointF(x+5, y), QPointF(x+5, y+2));
                }
            }
            if(!isSingle && ((!reverseLayout && !isLast) || (reverseLayout && !isLast))) {
                p->setPen(darkColor);
                if(eastAlignment) {
                    p->fillRect(x+5, y+h-2, w-10, 2, midColor);
                    p->drawLine(QPointF(x+w-5-1, y+h-2), QPointF(x+w-5-1, y+h-1));
                }
                else {
                    p->fillRect(x+5, y+h-2, w-10, 2, midColor);
                    p->drawLine(QPointF(x+5, y+h-2-1), QPointF(x+5, y+h-1));
                }
            }

            if(eastAlignment)
                renderSlab(p, Rc.adjusted(-7,0,0,0), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right | TileSet::Bottom);
            else
                renderSlab(p, Rc.adjusted(0,0,7,0), pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left | TileSet::Bottom);

            // some "position specific" paintings...
            // First draw the top connection from the panel border to the tab
            if(isFirst && !leftCornerWidget) {
                renderSlab(p, Rb.adjusted(-7,0,7,0), pal.color(QPalette::Window), NoFill, TileSet::Top);
            } else {
                TileSet *tile = _helper.slabInverted(pal.color(QPalette::Window), 0.0);
                if(eastAlignment)
                    tile->render(QRect(Rb.left(), Rb.top()-6,12,13), p, TileSet::Left | TileSet::Bottom);
                else
                    tile->render(QRect(Rb.left()-5, Rb.top()-5,12,12), p, TileSet::Right | TileSet::Bottom);
            }

            // Now draw the bottom connection from the panel border to the tab
            TileSet *tile = _helper.slabInverted(pal.color(QPalette::Window), 0.0);
            if(eastAlignment)
                tile->render(QRect(Rb.right()-6, Rb.bottom()-6,12,13), p, TileSet::Left | TileSet::Top);
            else
                tile->render(QRect(Rb.right()-5-6, Rb.bottom()-6,12,12), p, TileSet::Right | TileSet::Top);

        }
        else {
            // inactive tabs
            int x,y,w,h;
            p->save(); // we only use the clipping and AA for inactive tabs
            p->setPen(darkColor);
            p->setBrush(midColor);
            p->setRenderHints(QPainter::Antialiasing);

            if (westAlignment) {
                r.adjusted(5-gw,0,-5-gw,0).getRect(&x, &y, &w, &h);

                if (isLeftMost) { // at top
                    QPainterPath path;

                    path.moveTo(x+w+3.0, y);
                    path.lineTo(x+5.0, y); // top border
                    path.arcTo(QRectF(x+0.5, y+0.5, 9.5, 9.5), 90, 90); // top-left corner
                    path.lineTo(x+0.5, y+h+0.5); // left border
                    path.lineTo(x+w+1.0, y+h+0.5); // complete the path
                    p->drawPath(path);
                } else if (isRightMost) { // at bottom
                    QPainterPath path;

                    path.moveTo(x+w+0.5, y+h-0.5);
                    path.lineTo(x+5.0, y+h-0.5); // bottom border
                    path.arcTo(QRectF(x+0.5, y+h-0.5-9.5, 9.5, 9.5), 270, -90); // bottom-left corner
                    path.lineTo(x+0.5, y-0.5); // left border
                    path.lineTo(x+w+0.5, y-0.5); // complete the path
                    p->drawPath(path);
                } else {
                    // leftline
                    p->drawLine(QPointF(x+0.5, y-0.5), QPointF(x+0.5, y+h-0.5));
                    if((!reverseLayout && !isLeftOfSelected) || (reverseLayout && !isRightOfSelected))
                        p->drawLine(QPointF(x+0.5, y+h-0.5), QPointF(x+w-0.5, y+h-0.5));
                    p->fillRect(x, y, w, h, midColor);
                }
            } else { // eastAlignment
                r.adjusted(5+gw,0,-5+gw,0).getRect(&x, &y, &w, &h);
                if (isLeftMost) { // at top
                    QPainterPath path;

                    path.moveTo(x-3.0, y+0.5);
                    path.lineTo(x+w-5.0, y+0.5); // top line
                    path.arcTo(QRectF(x+w-0.5-9.5, y+0.5, 9.5, 9.5), 90, -90); // top-right corner
                    path.lineTo(x+w-0.5, y+h+0.5); // right line
                    path.lineTo(x-0.5, y+h+0.5); // complete path
                    p->drawPath(path);
                } else if (isRightMost) { // at bottom
                    QPainterPath path;

                    path.moveTo(x-0.5, y+h-0.5);
                    path.lineTo(x+w-5.0, y+h-0.5); // bottom line
                    path.arcTo(QRectF(x+w-0.5-9.5, y+h-0.5-9.5, 9.5, 9.5), -90, 90); // bottom-right corner
                    path.lineTo(x+w-0.5, y-0.5); // right line
                    path.lineTo(x-0.5, y-0.5); // complete path
                    p->drawPath(path);
                } else {
                    // right line
                    p->drawLine(QPointF(x+w-0.5, y), QPointF(x+w-0.5, y+h-0.5));
                    if((!reverseLayout && !isLeftOfSelected) || (reverseLayout && !isRightOfSelected))
                        p->drawLine(QPointF(x+0.5, y+h-0.5), QPointF(x+w-1.5, y+h-0.5));
                    p->fillRect(x, y, w, h, midColor);
                }
            }
            p->restore();

            TileSet::Tiles posFlag = eastAlignment ? TileSet::Right : TileSet::Left;
            QRect Ractual(Rb.left(), Rb.y(), 7, Rb.height());

            if(isLeftMost) { // at top
                if(isFrameAligned)
                    posFlag |= TileSet::Top;
                else {
                    renderSlab(p, QRect(Ractual.left(), Ractual.y()-7, Ractual.width(), 2+14), pal.color(QPalette::Window), NoFill, posFlag);
                    Ractual.adjust(0,-5,0,0);
                }
            }
            else
                Ractual.adjust(0,-7+gw,0,0);

            if(isRightMost) { // at bottom
                if(isFrameAligned && !reverseLayout)
                    posFlag |= TileSet::Top;
                Ractual.adjust(0,0,0,7);
            }
            else
                Ractual.adjust(0,0,0,7-gw);

            if (mouseOver)
                renderSlab(p, Ractual, pal.color(QPalette::Window), NoFill| Hover, posFlag);
            else
                renderSlab(p, Ractual, pal.color(QPalette::Window), NoFill, posFlag);

        // TODO mouseover effects
        }

    }
}

int OxygenStyle::styleHint(StyleHint hint, const QStyleOption * option,
                            const QWidget * widget, QStyleHintReturn * returnData) const
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
            if (!opt)
                return true;
            if (QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask*>(returnData))
                mask->region = option->rect;
            return true;
        }
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
                return 4;
            if (qobject_cast<const QFrame*>(widget) ||  qobject_cast<const QComboBox*>(widget))
                return 3;
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
            QSize size = contentsSize;

            if (const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option)) {
                if ((!tbOpt->icon.isNull()) && (!tbOpt->text.isEmpty()) && tbOpt->toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                    size.setHeight(size.height()-9);
            }

            // We want to avoid super-skiny buttons, for things like "up" when icons + text
            // For this, we would like to make width >= height.
            // However, once we get here, QToolButton may have already put in the menu area
            // (PM_MenuButtonIndicator) into the width. So we may have to take it out, fix things
            // up, and add it back in. So much for class-independent rendering...
            int   menuAreaWidth = 0;
            if (const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(option)) {
                if (tbOpt->features & QStyleOptionToolButton::MenuButtonPopup)
                    menuAreaWidth = pixelMetric(QStyle::PM_MenuButtonIndicator, option, widget);
                else if (tbOpt->features & QStyleOptionToolButton::HasMenu)
                    size.setWidth(size.width() + widgetLayoutProp(WT_ToolButton, ToolButton::InlineMenuIndicatorSize, tbOpt, widget));
            }
            size.setWidth(size.width() - menuAreaWidth);
            if (size.width() < size.height())
                size.setWidth(size.height());
            size.setWidth(size.width() + menuAreaWidth);

            const QToolButton* t=dynamic_cast<const QToolButton*>(widget);
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
            }
            else
            {
                int width = size.width() +
                        2*widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + MainMargin, option, widget);

                int height = size.height() +
                        2*widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + MainMargin, option, widget)
                        + widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Top, option, widget)
                        + widgetLayoutProp(WT_PushButton, PushButton::ContentsMargin + Bot, option, widget);

                return QSize(width, height);
            }
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
        {
            const QStyleOptionGroupBox *gbOpt = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
            if (!gbOpt)
                break;

            bool isFlat = gbOpt->features & QStyleOptionFrameV2::Flat;

            switch (subControl)
            {
                case SC_GroupBoxFrame:
                    return r;
                case SC_GroupBoxContents:
                {
                    int th = gbOpt->fontMetrics.height() + 8;
                    QRect cr = subElementRect(SE_CheckBoxIndicator, option, widget);
                    int fw = widgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, option, widget);

                    bool checkable = gbOpt->subControls & QStyle::SC_GroupBoxCheckBox;
                    bool emptyText = gbOpt->text.isEmpty();
                    if (emptyText && !checkable) r.adjust(fw, fw, -fw, -fw);
                    else if (checkable) r.adjust(fw, fw + cr.height(), -fw, -fw);
                    else if (!emptyText) r.adjust(fw, fw + th, -fw, -fw);
                    else r.adjust(fw, fw + qMax(th, cr.height()), -fw, -fw);

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
                    if (isFlat)
                        font.setBold(true);

                    QFontMetrics fontMetrics = QFontMetrics(font);
                    int h = fontMetrics.height();
                    int tw = fontMetrics.size(Qt::TextShowMnemonic, gbOpt->text + QLatin1String("  ")).width();
                    r.setHeight(h);
                    r.moveTop(8);
                    QRect cr;
                    if(gbOpt->subControls & QStyle::SC_GroupBoxCheckBox)
                    {
                        cr = subElementRect(SE_CheckBoxIndicator, option, widget);
                        QRect gcr((gbOpt->rect.width() - tw -cr.width())/2 , (h-cr.height())/2+r.y(), cr.width(), cr.height());
                        if(subControl == SC_GroupBoxCheckBox)
			{
			    if (!isFlat)
				return visualRect(option->direction, option->rect, gcr);
			    else
				return visualRect(option->direction, option->rect, QRect(0,0,cr.width(),cr.height()));
			}
                    }

                    // left align labels in flat group boxes, center align labels in framed group boxes
                    if (isFlat)
                        r = QRect(cr.width(),r.y(),tw,r.height());
                    else
                        r = QRect((gbOpt->rect.width() - tw - cr.width())/2 + cr.width(), r.y(), tw, r.height());

                    return visualRect(option->direction, option->rect, r);
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    return KStyle::subControlRect(control, option, subControl, widget);
}

QRect OxygenStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;

    switch (sr) {
    case SE_TabWidgetTabBar: {
        const QStyleOptionTabWidgetFrame *twf  = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
        if(!twf) return QRect();
        r = QRect(QPoint(0,0), twf->tabBarSize);

        switch (twf->shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth: {
            r.setWidth(qMin(r.width(), twf->rect.width()
                            - twf->leftCornerWidgetSize.width()
                            - twf->rightCornerWidgetSize.width()));
            r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(), 0));
            r = visualRect(twf->direction, twf->rect, r);
            break;
        }
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth: {
            r.setWidth(qMin(r.width(), twf->rect.width()
                            - twf->leftCornerWidgetSize.width()
                            - twf->rightCornerWidgetSize.width()));
            r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(),
                                     twf->rect.height() - twf->tabBarSize.height()));
            r = visualRect(twf->direction, twf->rect, r);
            break;
        }
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast: {
            r.setHeight(qMin(r.height(), twf->rect.height()
                             - twf->leftCornerWidgetSize.height()
                             - twf->rightCornerWidgetSize.height()));
            r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width(),
                                     twf->leftCornerWidgetSize.height()));
            break;
        }
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest: {
            r.setHeight(qMin(r.height(), twf->rect.height()
                             - twf->leftCornerWidgetSize.height()
                             - twf->rightCornerWidgetSize.height()));
            r.moveTopLeft(QPoint(0, twf->leftCornerWidgetSize.height()));
            }
            break;
        }
        return r;

    }
    case SE_TabWidgetLeftCorner: {
        const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
        if(!twf) return QRect();

        QRect paneRect = subElementRect(SE_TabWidgetTabPane, twf, widget);
        switch (twf->shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            r = QRect(QPoint(paneRect.x(), paneRect.y() - twf->leftCornerWidgetSize.height() + gw), twf->leftCornerWidgetSize);
            r = visualRect(twf->direction, twf->rect, r);
            break;
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            r = QRect(QPoint(paneRect.x(), paneRect.height()), twf->leftCornerWidgetSize);
            r = visualRect(twf->direction, twf->rect, r);
            break;
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            r = QRect(QPoint(paneRect.x() - twf->leftCornerWidgetSize.width(), paneRect.y()), twf->leftCornerWidgetSize);
            break;
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            r = QRect(QPoint(paneRect.x() + paneRect.width(), paneRect.y()), twf->leftCornerWidgetSize);
            break;
        default:
            break;
        }

        return r;

    }
    case SE_TabWidgetRightCorner: {
        const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt);
        if(!twf) return QRect();

        QRect paneRect = subElementRect(SE_TabWidgetTabPane, twf, widget);
        switch (twf->shape) {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.y() - twf->rightCornerWidgetSize.height() + gw), twf->rightCornerWidgetSize);
            r = visualRect(twf->direction, twf->rect, r);
            break;
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.height()), twf->rightCornerWidgetSize);
            r = visualRect(twf->direction, twf->rect, r);
            break;
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            r = QRect(QPoint(paneRect.x() - twf->rightCornerWidgetSize.width(), paneRect.y() + paneRect.height() - twf->rightCornerWidgetSize.height()), twf->rightCornerWidgetSize);
            break;
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            r = QRect(QPoint(paneRect.x() + paneRect.width(), paneRect.y() + paneRect.height() - twf->rightCornerWidgetSize.height()), twf->rightCornerWidgetSize);
            break;
        default:
            break;
        }

        return r;
        }
    case SE_TabBarTearIndicator: {
        const QStyleOptionTab *option = qstyleoption_cast<const QStyleOptionTab *>(opt);
        if(!option) return QRect();

        switch (option->shape) {
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
        default:
            break;
    }
    p->restore();
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

    if (QToolBar *t = qobject_cast<QToolBar*>(obj))
    {
        switch(ev->type()) {
            case QEvent::Show:
            case QEvent::Resize: {
                int x, y, w, h;
                t->rect().getRect(&x, &y, &w, &h);
                QRegion reg(x+4, y, w-8, h);
                reg += QRegion(x, y+4, w, h-8);
                reg += QRegion(x+2, y+1, w-4, h-2);
                reg += QRegion(x+1, y+2, w-2, h-4);
                if(t->mask() != reg)
                    t->setMask(reg);
                return false;
            }
            default:
                return false;
        }
    }

    if (QMenu *m = qobject_cast<QMenu*>(obj))
    {
        switch(ev->type()) {
        case QEvent::Show:
        case QEvent::Resize: {
            int x, y, w, h;
            m->rect().getRect(&x, &y, &w, &h);
            QRegion reg(x+4, y, w-8, h);
            reg += QRegion(x, y+4, w, h-8);
            reg += QRegion(x+2, y+1, w-4, h-2);
            reg += QRegion(x+1, y+2, w-2, h-4);
            if(m->mask() != reg)
                m->setMask(reg);
            return false;
        }
        case QEvent::Paint:
        {
            QPainter p(m);
            QPaintEvent *e = (QPaintEvent*)ev;
            QRect r = m->rect();
            QColor color = m->palette().color(QPalette::Background);
            int splitY = qMin(200, 3*r.height()/4);

            p.setClipRegion(e->region());

            QRect upperRect = QRect(0, 0, r.width(), splitY);
            QPixmap tile = _helper.verticalGradient(color, splitY);
            p.drawTiledPixmap(upperRect, tile);

            QRect lowerRect = QRect(0,splitY, r.width(), r.height() - splitY);
            p.fillRect(lowerRect, _helper.backgroundBottomColor(color));
            return false;
        }
        default:
            return false;
        }
    }

    QWidget *widget = static_cast<QWidget*>(obj);
    if (widget->inherits("QComboBoxPrivateContainer")) {
        switch(ev->type()) {
        case QEvent::Show:
        case QEvent::Resize: 
        {
            int x, y, w, h;
            widget->rect().getRect(&x, &y, &w, &h);
            QRegion reg(x+4, y, w-8, h);
            reg += QRegion(x, y+4, w, h-8);
            reg += QRegion(x+2, y+1, w-4, h-2);
            reg += QRegion(x+1, y+2, w-2, h-4);
            if(widget->mask() != reg)
                widget->setMask(reg);
            return false;
        }
        case QEvent::Paint:
        {
            QPainter p(widget);
            _helper.drawFloatFrame(&p, widget->rect(), widget->palette().color(QPalette::Window));
        }
        default:
            return false;
        }
    }

    if (widget->isWindow() && widget->isVisible()) {
        if (ev->type() == QEvent::Paint)
        {
            QBrush brush = widget->palette().brush(widget->backgroundRole());
            // don't use our background if the app requested something else,
            // e.g. a pixmap
            // TODO - draw our light effects over an arbitrary fill?
            if (brush.style() == Qt::SolidPattern) {
            }

            if(widget->testAttribute(Qt::WA_StyledBackground) && !widget->testAttribute(Qt::WA_NoSystemBackground))
            {
                QPainter p(widget);
                _helper.renderWindowBackground(&p, widget->rect(), widget,widget->window()->palette());
            }
        }
    }

    if (QMdiSubWindow *mw = qobject_cast<QMdiSubWindow*>(obj))
    {
        if (ev->type() == QEvent::Show || ev->type() == QEvent::Resize || ev->type() == QEvent::WindowStateChange)
        {
            int x, y, w, h;
            mw->rect().getRect(&x, &y, &w, &h);
            QRegion reg(x+4, y, w-8, h);
            reg += QRegion(x, y+4, w, h-8);
            reg += QRegion(x+2, y+1, w-4, h-2);
            reg += QRegion(x+1, y+2, w-2, h-4);
            if(mw->mask() != reg)
                mw->setMask(reg);
            return false;
        }
    }

    if (QDockWidget*dw = qobject_cast<QDockWidget*>(obj))
    {
        if (ev->type() == QEvent::Show || ev->type() == QEvent::Resize)
        {
            int x, y, w, h;
            dw->rect().getRect(&x, &y, &w, &h);
            QRegion reg(x+4, y, w-8, h);
            reg += QRegion(x, y+4, w, h-8);
            reg += QRegion(x+2, y+1, w-4, h-2);
            reg += QRegion(x+1, y+2, w-2, h-4);
            if(dw->mask() != reg)
                dw->setMask(reg);
            return false;
        }
        if (ev->type() == QEvent::Paint)
        {
            QPainter p(dw);
            const QColor color = dw->palette().color(QPalette::Window);

            if(dw->isWindow())
            {
                _helper.drawFloatFrame(&p, dw->rect(), color);
                return false;
            }

            int w = dw->rect().width();
            int h = dw->rect().height();
            QRect rect(0,0,w,h);

            TileSet *tileSet = _helper.dockFrame(color, w);
            tileSet->render(rect, &p);

            return false;
        }
    }

    if (QToolBox *tb = qobject_cast<QToolBox*>(obj))
    {
        if (ev->type() == QEvent::Paint)
        {
            QRect r = tb->rect();
            StyleOptions opts = NoFill;

            QPainter p(tb);
            p.setClipRegion(((QPaintEvent*)ev)->region());
            renderSlab(&p, r, tb->palette().color(QPalette::Button), opts);
        }
        return false;
    }

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
            QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
            realpm.fill(QColor(0,0,0,0));
            QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
            QPainter painter(&realpm);
            painter.drawPixmap(1,1,pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg = _helper.decoGradient(QRect(3,3,11,11), QColor(0,0,0));
            painter.setPen(QPen(lg,1.4));
            QPointF points[4] = {QPointF(8.5, 6), QPointF(11, 8.5), QPointF(8.5, 11), QPointF(6, 8.5)};
            painter.drawPolygon(points, 4);

            return QIcon(realpm);
        }

        case SP_TitleBarShadeButton:
        {
            QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
            realpm.fill(QColor(0,0,0,0));
            QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
            QPainter painter(&realpm);
            painter.drawPixmap(1,1,pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg = _helper.decoGradient(QRect(3,3,11,11), QColor(0,0,0));
            painter.setPen(QPen(lg,1.4));
            painter.drawLine( QPointF(6.5,6.5), QPointF(8.75,8.75) );
            painter.drawLine( QPointF(8.75,8.75), QPointF(11.0,6.5) );
            painter.drawLine( QPointF(6.5,11.0), QPointF(11.0,11.0) );

            return QIcon(realpm);
        }

        case SP_TitleBarUnshadeButton:
        {
            QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
            realpm.fill(QColor(0,0,0,0));
            QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
            QPainter painter(&realpm);
            painter.drawPixmap(1,1,pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg = _helper.decoGradient(QRect(3,3,11,11), QColor(0,0,0));
            painter.setPen(QPen(lg,1.4));
            painter.drawLine( QPointF(6.5,8.75), QPointF(8.75,6.5) );
            painter.drawLine( QPointF(8.75,6.5), QPointF(11.0,8.75) );
            painter.drawLine( QPointF(6.5,11.0), QPointF(11.0,11.0) );

            return QIcon(realpm);
        }

        case SP_TitleBarCloseButton:
        case SP_DockWidgetCloseButton:
        {
            QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
            realpm.fill(QColor(0,0,0,0));
            QPixmap pm = _helper.windecoButton(buttonColor, false, 15);
            QPainter painter(&realpm);
            painter.drawPixmap(1,1,pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg = _helper.decoGradient(QRect(3,3,11,11), QColor(0,0,0));
            painter.setPen(QPen(lg,1.4));
            painter.drawLine( QPointF(6.5,6.5), QPointF(11.0,11.0) );
            painter.drawLine( QPointF(11.0,6.5), QPointF(6.5,11.0) );

            return QIcon(realpm);
        }
        default:
            return KStyle::standardPixmap(standardIcon, option, widget);
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
