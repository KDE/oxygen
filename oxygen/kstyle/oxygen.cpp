/* Oxygen widget style for KDE 4
   Copyright (C) 2007 Casper Boemann <cbr@boemann.dk>
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
#include <QStyleOptionDockWidget>
#include <QPaintEvent>
#include <QToolBox>
#include <QAbstractScrollArea>
#include <QAbstractItemView>

#include <QtDBus/QtDBus>

#include <KGlobal>
#include <KGlobalSettings>
#include <KConfigGroup>
#include <KColorUtils>
#include <kdebug.h>

#include <math.h>

#include "helper.h"
#include "tileset.h"

K_EXPORT_STYLE("Oxygen", OxygenStyle)

K_GLOBAL_STATIC_WITH_ARGS(OxygenStyleHelper, globalHelper, ("oxygen"))

OxygenStyle::OxygenStyle() :
    KStyle(),
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
    setWidgetLayoutProp(WT_DockWidget, DockWidget::FrameWidth, 0);
    setWidgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, 2);

    setWidgetLayoutProp(WT_Menu, Menu::FrameWidth, 5);
    setWidgetLayoutProp(WT_Menu, Menu::Margin,     4);

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

    setWidgetLayoutProp(WT_TabBar, TabBar::TabOverlap, 0);
    setWidgetLayoutProp(WT_TabBar, TabBar::BaseOverlap, 7);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Left, 8);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Right, 8);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Top, 2);
    setWidgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Bot, 2);
    setWidgetLayoutProp(WT_TabBar, TabBar::ScrollButtonWidth, 18);

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
    setWidgetLayoutProp(WT_ToolBar, ToolBar::ItemMargin, 2);

    setWidgetLayoutProp(WT_ToolButton, ToolButton::ContentsMargin, 4);
    setWidgetLayoutProp(WT_ToolButton, ToolButton::FocusMargin,    0);

    setWidgetLayoutProp(WT_GroupBox, GroupBox::FrameWidth, 5);

    setWidgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin, 5);

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
    _animateProgressBar = cfg.readEntry("AnimateProgressBar", false);
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
            //iter.value() = (iter.value() + 1) % 32;
            // dont' update right now      iter.key()->update();
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
                    bool enabled = option->state & State_Enabled;
                    QPixmap pm = option->icon.pixmap(pixelMetric(QStyle::PM_SmallIconSize, option, widget),
                                                 enabled ? QIcon::Normal : QIcon::Disabled);
                    int cm = widgetLayoutProp(WT_ToolBoxTab, ToolBoxTab::Margin, opt, widget);
                    QRect cr = r.adjusted(cm,2,-50-cm,0);

                    if(!pm.isNull() && cr.width() >= pm.width())
                    {
                        QRect pr(cr.x(), cr.y()+(cr.height()-pm.height())/2, pm.width(), pm.height());
                        pr = visualRect(option->direction, cr, pr);
                        p->drawPixmap(pr.x(), pr.y(), pm);
                        cr.adjust(pm.width()+4, 0, reverseLayout ? -(pm.width()+4) : 0, 0);
                    }

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
                    path.moveTo(r.x()+r.width()-52, r.y());
                    int y = r.height()*15/100;
                    path.cubicTo(QPointF(r.x()+r.width()-50+8, r.y()), QPointF(r.x()+r.width()-50+10, r.y()+y), QPointF(r.x()+r.width()-50+10, r.y()+y));
                    path.lineTo(r.x()+r.width()-18-9, r.y()+r.height()-y);
                    path.cubicTo(QPointF(r.x()+r.width()-18-9, r.y()+r.height()-y), QPointF(r.x()+r.width()-19-6, r.y()+r.height()-1-0.3), QPointF(r.x()+r.width()-19, r.y()+r.height()-1-0.3));

                    p->setRenderHint(QPainter::Antialiasing, true);
                    p->translate(0,1);
                    p->setPen(light);
                    p->drawPath(path);
                    p->translate(0,-1);
                    p->setPen(dark);
                    p->drawPath(path);

                    p->setRenderHint(QPainter::Antialiasing, false);
                    p->drawLine(r.x(), r.y(), r.x()+r.width()-50+1, r.y());
                    p->drawLine(r.x()+r.width()-20, r.y()+r.height()-2, r.x()+r.width(), r.y()+r.height()-2);
                    p->setPen(light);
                    p->drawLine(r.x(), r.y()+1, r.x()+r.width()-50, r.y()+1);
                    p->drawLine(r.x()+r.width()-20, r.y()+r.height()-1, r.x()+r.width(), r.y()+r.height()-1);

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
                    //p->setClipRect(rect.adjusted(-32,0,32,0));
                    /* HACK - make progress bars with a few percent progress look less broken. */
                    p->setClipRect(rect.adjusted(1,0,0,0));
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
                    QColor color = pal.color(QPalette::Window);
                    QColor light = _helper.calcLightColor(color);
                    QColor dark = _helper.calcDarkColor(color);
                    dark.setAlpha(120);
                        p->setRenderHint(QPainter::Antialiasing);

                    QLinearGradient lg(r.x(),0,r.right(),0);
                    lg.setColorAt(0.5, dark);
                    dark.setAlpha(0);
                    lg.setColorAt(0.0, dark);
                    lg.setColorAt(1.0, dark);
                    p->setPen(QPen(lg,1));

                    p->drawLine(QPointF(r.x(), r.y()+0.5),
                                            QPointF(r.right(), r.y()+0.5));

                    lg = QLinearGradient(r.x(), 0, r.right(),0);
                    lg.setColorAt(0.5, light);
                    light.setAlpha(0);
                    lg.setColorAt(0.0, light);
                    lg.setColorAt(1.0, light);
                    p->setPen(QPen(lg,1));

                    p->drawLine(QPointF(r.x(), r.y()+1.5),
                                        QPointF(r.right(), r.y()+1.5));
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

                        QRect maskr(rr.width()-40, 0, 40,rr.height());
                        QLinearGradient gradient(maskr.left(), 0, maskr.right()-4, 0);
                        gradient.setColorAt(0.0, QColor(0,0,0,255));
                        gradient.setColorAt(1.0, QColor(0,0,0,0));
                        pp.setBrush(gradient);
                        pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                        pp.drawRect(maskr);

                        p->drawPixmap(r, pm);
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
                case Generic::Text:
                {
                    const QStyleOptionDockWidget* dwOpt = ::qstyleoption_cast<const QStyleOptionDockWidget*>(opt);
                    if (!dwOpt) return;
                    const QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(opt);
                    bool verticalTitleBar = v2 ? v2->verticalTitleBar : false;

                    QRect btnr = subElementRect(dwOpt->floatable ? SE_DockWidgetFloatButton : SE_DockWidgetCloseButton, opt, widget);
                    int fw = widgetLayoutProp(WT_DockWidget, DockWidget::TitleMargin, opt, widget);
                    QRect r = dwOpt->rect.adjusted(fw, fw, -fw, -fw);
                    if (verticalTitleBar)
                        r.setY(btnr.y()+btnr.height());
                    else if(reverseLayout)
                    {
                        r.setLeft(btnr.x()+btnr.width());
                        r.adjust(0,0,-4,0);
                    }
                    else
                    {
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
                        drawItemText(p, r, (reverseLayout ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter
                        | Qt::TextShowMnemonic, dwOpt->palette, dwOpt->state & State_Enabled, title,
                        QPalette::WindowText);
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
                        rect = visualRect(option->direction, r, rect);
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
                        break;
                    default:
                        return;
                    }

                    QRect gr;
                    if(!vertical && !reverseLayout)
                        gr = QRect(0, 0, r.width(), 0);
                    else if(!vertical && reverseLayout)
                        if(!option->cornerWidgets & QStyleOptionTab::LeftCornerWidget)
                            gr = QRect(r.x()-4, 0, r.x()-4+r.width(), 0);
                        else
                            gr = QRect(r.x(), 0, r.x()+r.width(), 0);
                    else
                        gr = QRect(0, 0, 0, r.height());

/*
                    QLinearGradient grad(gr.x(), gr.y(), gr.width(), gr.height());
                    grad.setColorAt(0, Qt::transparent);
                    grad.setColorAt(0.2, Qt::transparent);
                    grad.setColorAt(1, Qt::black);
                    p->setCompositionMode((reverseLayout && !vertical) ? QPainter::CompositionMode_DestinationOut : QPainter::CompositionMode_DestinationIn);
                    p->fillRect(br, QBrush(grad));

                    p->setCompositionMode(slabCompMode);
                    renderSlab(p, rect, opt->palette.color(QPalette::Window), NoFill, flag);
*/                    return;
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
                                fr.adjust(-7,0,7,-1);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Top);
                            }
                            if (tabOpt->tabBarRect.right() < r.right())
                            {
                                QRect fr = r;
                                fr.setLeft(tabOpt->tabBarRect.right());
                                fr.adjust(-7,0,7,-1);
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
                                fr.adjust(-7,0,7,-1);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                            }
                            if (tabOpt->tabBarRect.right() < r.right())
                            {
                                QRect fr = r;
                                fr.setLeft(tabOpt->tabBarRect.right());
                                fr.adjust(-6,0,7,-1);
                                renderSlab(p, fr, pal.color(QPalette::Window), NoFill, TileSet::Bottom);
                            }
                            return;
                        }
                        default:
                            break;
                    }
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

                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode
                                if (h+lh >0)
                                    renderSlab(p, QRect(r.x(),  h + lh - 7, 7, r.height() - h - lh+7),
                                               pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Left);
                                else
                                    renderSlab(p, QRect(r.x(), r.y(), r.width(), 7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Top | TileSet::Right);

                                if (lh > 0)
                                    renderSlab(p, QRect(r.x(), r.y() , 7, lh+7),
                                               pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Left);
                            }
                            else
                            {
                                if (lh > 0)
                                    renderSlab(p, QRect(r.x(), r.y(), 7, lh+7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Top);

                                if (h+lh >0)
                                    renderSlab(p, QRect(r.x(), r.y()+h+lh-7, 7, r.height() - h - lh+7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Bottom);
                                else
                                    renderSlab(p, QRect(r.x(), r.y(), 7, r.height()), pal.color(QPalette::Window), NoFill,
                                               TileSet::Top | TileSet::Left | TileSet::Bottom);
                            }

                            return;

                        case QTabBar::RoundedEast:
                        case QTabBar::TriangularEast:

                            renderSlab(p, r, pal.color(QPalette::Window), NoFill,
                                       TileSet::Top | TileSet::Left | TileSet::Bottom);
                            if(reverseLayout)
                            {
                                // Left and right widgets are placed right and left when in reverse mode
                                if (h+lh >0)
                                    renderSlab(p, QRect(r.x()+r.width()-7,  h + lh - 7, 7, r.height() - h - lh+7),
                                               pal.color(QPalette::Window), NoFill, TileSet::Bottom | TileSet::Right);
                                else
                                    renderSlab(p, QRect(r.x()+r.width()-7, r.y(), r.width(), 7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Left | TileSet::Top | TileSet::Right);

                                if (lh > 0)
                                    renderSlab(p, QRect(r.x()+r.width()-7, r.y() , 7, lh+7),
                                               pal.color(QPalette::Window), NoFill, TileSet::Top | TileSet::Right);
                            }
                            else
                            {
                                if (lh > 0)
                                    renderSlab(p, QRect(r.width()-7, r.y(), 7, lh+7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Top | TileSet::Right);

                                if (h+lh >0)
                                    renderSlab(p, QRect(r.width()-7, r.y()+h+lh-7, 7, r.height() - h - lh+7), pal.color(QPalette::Window), NoFill,
                                               TileSet::Bottom | TileSet::Right);
                                else
                                    renderSlab(p, QRect(r.x(), r.y(), 7, r.height()), pal.color(QPalette::Window), NoFill,
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

                     p->fillRect(fr.adjusted(3,3,-3,-3), inputColor);

                     p->restore();
                    // TODO use widget background role?
                    renderHole(p, pal.color(QPalette::Window), fr, hasFocus, mouseOver);
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

                        p->fillRect(fr.adjusted(3,3,-3,-3), inputColor);

                        p->restore();

                        if (hasFocus && enabled)
                        {
                            renderHole(p, pal.color(QPalette::Window), fr, true, mouseOver);
                        }
                        else
                        {
                            renderHole(p, pal.color(QPalette::Window), fr, false, mouseOver);
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
                    const QColor inputColor =  pal.color(QPalette::Window);
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
                    const QColor inputColor =
                                enabled?pal.color(QPalette::Base):pal.color(QPalette::Background);

                    if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
                    {
                        const int lineWidth(panel->lineWidth);

                        if (lineWidth > 0)
                        {
                            p->save();
                            p->setRenderHint(QPainter::Antialiasing);
                            p->setPen(Qt::NoPen);
                            p->setBrush(inputColor);

                            p->fillRect(r.adjusted(5,5,-5,-5), inputColor);
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
                    const QToolButton* t=dynamic_cast<const QToolButton*>(widget);
                    const QStyleOptionToolButton* tbOpt = qstyleoption_cast<const QStyleOptionToolButton*>(opt);
                    if (t && t->autoRaise()==false && t->icon().isNull())
                    {
                        StyleOptions opts = 0;
                        if ((flags & State_On) || (flags & State_Sunken))
                            opts |= Sunken;
                        if (flags & State_HasFocus)
                            opts |= Focus;
                        if (enabled && (flags & State_MouseOver))
                            opts |= Hover;
                        renderSlab(p, r, pal.color(QPalette::Button), opts);
                        return;
                    }

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

        case WT_Limit: //max value for the enum, only here to silence the compiler
        case WT_Generic: // handled below since the primitives arevalid for all WT_ types
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
                // TODO use widget background role? - probably not
                //renderHole(p, pal.color(widget->backgroundRole()), r, focusHighlight);
                renderHole(p, pal.color(QPalette::Window), r, focusHighlight);
            } else
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
            break;
        case Qt::Popup: // we currently don't want gradients on menus etc
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
        widget->setContentsMargins(2,1,2,2);
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
    else if (qobject_cast<QMenu*>(widget))
    {
        widget->installEventFilter(this);
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
        QRect Rc = southAlignment ? r.adjusted(0,6,0,0) : r.adjusted(0,0,0,-7);

        // the area where the fairing should appear
        const QRect Rb(r.x(), southAlignment?r.top():Rc.bottom()+1, r.width(), r.height()-Rc.height() );

        // FIXME - maybe going to redo tabs
        if (selected) {
            int x,y,w,h;
            r.getRect(&x, &y, &w, &h);
            // parts of the adjacent tabs
            if(!isSingle && ((!reverseLayout && !isFirst) || (reverseLayout && !isLast))) {
                p->setPen(darkColor);
                if(southAlignment) {
                    p->fillRect(r.x(), r.y()+5, 2, r.height()-10, midColor);
                    p->drawLine(QPointF(x, y+h-6), QPointF(x+2, y+h-6));
                }
                else {
                    p->fillRect(r.x(), r.y()+5, 2, r.height()-8, midColor);
                    p->drawLine(QPointF(x, y+5), QPointF(x+2, y+5));
                }
            }
            if(!isSingle && ((!reverseLayout && !isLast) || (reverseLayout && !isFirst))) {
                p->setPen(darkColor);
                if(southAlignment) {
                    p->fillRect(r.x()+r.width()-2, r.y()+5, 1, r.height()-10, midColor);
                    p->drawLine(QPointF(x+w-3, y+h-6), QPointF(x+w-1, y+h-6));
                }
                else {
                    p->fillRect(r.x()+r.width()-2, r.y()+5, 1, r.height()-8, midColor);
                    p->drawLine(QPointF(x+w-3, y+5), QPointF(x+w-1, y+5));
                }
            }

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
            p->setPen(darkColor);

            if (!southAlignment) {
                r.adjusted(0,4,0,0).getRect(&x, &y, &w, &h);
                if(isLeftMost) {
                    p->drawArc(QRectF(x+2.5, y+0.5, 9.5, 9.5),90*16, 90*16);
                    if(isFrameAligned)
                        p->drawLine(QPointF(x+2.5, y+6.3), QPointF(x+2.5, y+h-2));
                    else
                        p->drawLine(QPointF(x+2.5, y+6.3), QPointF(x+2.5, y+h-6));
                    // topline
                    p->drawLine(QPointF(x+8.8, y+0.5), QPointF(x+w-1, y+0.5));
                    if(!isLeftOfSelected)
                        p->drawLine(QPointF(x+w-0.5, y+1.5), QPointF(x+w-0.5, y+h-6.3));
                    p->fillRect(x+2, y+1, w-2, h-5, midColor);
                } else  if(isRightMost) {
                    p->drawArc(QRectF(x+w-9.5-2.5, y+0.5, 9.5, 9.5), 0, 90*16);
                    if(isFrameAligned)
                        p->drawLine(QPointF(x+w-2.5, y+6.3), QPointF(x+w-2.5, y+h+0.5));
                    else
                        p->drawLine(QPointF(x+w-2.5, y+6.3), QPointF(x+w-2.5, y+h-6.3));
                    // topline
                    p->drawLine(QPointF(x, y+0.5), QPointF(x+w-8.8, y+0.5));
                    p->fillRect(x-1, y+1, w-1, h-5, midColor);
                } else {
                    // topline
                    p->drawLine(QPointF(x, y+0.5), QPointF(x+w-1, y+0.5));
                    if(!isLeftOfSelected)
                        p->drawLine(QPointF(x+w-0.5, y+1.5), QPointF(x+w-0.5, y+h-6.3));
                    p->fillRect(x-1, y+1, w-1+2, h-5, midColor);
                }
            }
            else { // southAlignment
                r.adjusted(0,0,0,-6).getRect(&x, &y, &w, &h);
                if(isLeftMost) {
                    p->drawArc(QRectF(x+2.5, y+h+0.2-9.5, 9.5, 9.5),180*16, 90*16);
                    if(isFrameAligned)
                        p->drawLine(QPointF(x+2.5, y+1.5), QPointF(x+2.5, y+h+3-9.5));
                    else
                        p->drawLine(QPointF(x+2.5, y+2+1.5), QPointF(x+2.5, y+h+3-9.5));
                    // bottomline
                    p->drawLine(QPointF(x+8.8, y+h), QPointF(x+w-1, y+h));
                    if(!isLeftOfSelected)
                        p->drawLine(QPointF(x+w-0.5, y+2+1.5), QPointF(x+w-0.5, y+h-1));
                    p->fillRect(x+2, y+5, w-2, h-4, midColor);
                } else  if(isRightMost) {
                    p->drawArc(QRectF(x+w-9.5-2.5, y+h+0.2-9.5, 9.5, 9.5), 270*16, 90*16);
                    if(isFrameAligned) // in reverseLayout mode
                        p->drawLine(QPointF(x+w-2.5, y+1.5), QPointF(x+w-2.5, y+h-6.3));
                    else
                        p->drawLine(QPointF(x+w-2.5, y+2+1.5), QPointF(x+w-2.5, y+h-6.3));
                    // bottomline
                    p->drawLine(QPointF(x, y+h), QPointF(x+w-8.8, y+h));
                    p->fillRect(x-1, y+5, w-1, h-4, midColor);
                } else {
                    // bottomline
                    p->drawLine(QPointF(x, y+h), QPointF(x+w-1, y+h));
                    if(!isLeftOfSelected)
                        p->drawLine(QPointF(x+w-0.5, y+2+1.5), QPointF(x+w-0.5, y+h-1));
                    p->fillRect(x-1, y+5, w-1+2, h-4, midColor);
                }


            }


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
                Ractual.adjust(-7,0,0,0);

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
                Ractual.adjust(0,0,7,0);

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
        QRect Rc = eastAlignment ? r.adjusted(7,0,0,0) : r.adjusted(0,0,-7,0);
        // the area where the fairing should appear
        const QRect Rb(eastAlignment ? r.x() : Rc.width(), r.top(), r.width()-Rc.width(), r.height() );

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
            p->setPen(darkColor);

            if (westAlignment) {
                Rc.adjusted(5,0,2,0).getRect(&x, &y, &w, &h);

                if(isLeftMost) { // at top
                        p->drawArc(QRectF(x+0.5, y+1.5, 9.5, 9.5),90*16, 90*16);
                    if(isFrameAligned)
                        p->drawLine(QPointF(x-3+9.5, y+1.5), QPointF(x+w+1.5, y+1.5));
                    else
                        p->drawLine(QPointF(x-3+9.5, y+1.5), QPointF(x+w-1, y+1.5));
                    // leftline
                    p->drawLine(QPointF(x, y-3+9.5), QPointF(x, y+h-1));
                    // separator
                    if((!reverseLayout && !isLeftOfSelected) || (reverseLayout && !isRightOfSelected))
                        p->drawLine(QPointF(x+1.5, y+h-1), QPointF(x+w-0.5, y+h-1));
                    p->fillRect(x, y+2, w, h-2, midColor);
                } else  if(isRightMost) { // at bottom
                    p->drawArc(QRectF(x+0.5, y+h-0.5-9.5, 9.5, 9.5), 180*16, 90*16);
                    if(isFrameAligned)
                        p->drawLine(QPointF(x+9.5, y+h-1), QPointF(x+w, y+h-1));
                    else
                        p->drawLine(QPointF(x-4+9.5, y+h-1), QPointF(x+w-1, y+h-1));
                    // leftline
                    p->drawLine(QPointF(x, y), QPointF(x, y+h+3-9.5));
                    p->fillRect(x, y, w, h, midColor);
                } else {
                    // leftline
                    p->drawLine(QPointF(x, y), QPointF(x, y+h-1));
                    if((!reverseLayout && !isLeftOfSelected) || (reverseLayout && !isRightOfSelected))
                        p->drawLine(QPointF(x+1.5, y+h-1), QPointF(x+w-0.5, y+h-1));
                    p->fillRect(x, y, w, h, midColor);
                }
            }
            else { // eastAlignment

                Rc.adjusted(-2,0,-5,0).getRect(&x, &y, &w, &h);

                if(isLeftMost) { // at top
                    p->drawArc(QRectF(x+w-0.5-9.5, y+1.5, 9.5, 9.5),0*16, 90*16);
                    if(isFrameAligned)
                        p->drawLine(QPointF(x-1.5, y+1.5), QPointF(x+w+3-9.5, y+1.5));
                    else
                        p->drawLine(QPointF(x, y+1.5), QPointF(x+w+3-9.5, y+1.5));
                    // rightline
                    p->drawLine(QPointF(x+w-1, y-3+9.5), QPointF(x+w-1, y+h-1));
                    // separator
                    if((!reverseLayout && !isLeftOfSelected) || (reverseLayout && !isRightOfSelected))
                        p->drawLine(QPointF(x+0.5, y+h-1), QPointF(x+w-1.5, y+h-1));
                    p->fillRect(x, y+2, w, h-2, midColor);
                } else  if(isRightMost) { // at bottom
                    p->drawArc(QRectF(x+w-9.5-0.5, y+h-0.5-9.5, 9.5, 9.5), 270*16, 90*16);
                    if(isFrameAligned) // in reverseLayout mode
                        p->drawLine(QPointF(x-2.5, y+h-1), QPointF(x+w+3-9.5, y+h-1));
                    else
                        p->drawLine(QPointF(x+0.5, y+h-1), QPointF(x+w+4-9.5, y+h-1));
                    // rightline
                    p->drawLine(QPointF(x+w-1, y), QPointF(x+w-1, y+h+3-9.5));
                    p->fillRect(x, y, w, h, midColor);
                } else {
                    // rightline
                    p->drawLine(QPointF(x+w-1, y), QPointF(x+w-1, y+h-1));
                    if((!reverseLayout && !isLeftOfSelected) || (reverseLayout && !isRightOfSelected))
                        p->drawLine(QPointF(x+0.5, y+h-1), QPointF(x+w-1.5, y+h-1));
                    p->fillRect(x, y, w, h, midColor);
                }

            }

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
                Ractual.adjust(0,-7,0,0);

            if(isRightMost) { // at bottom
                if(isFrameAligned && !reverseLayout)
                    posFlag |= TileSet::Top;
                Ractual.adjust(0,0,0,7);
            }
            else
                Ractual.adjust(0,0,0,7);

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
        case SH_Menu_SubMenuPopupDelay:
            return 96; // Motif-like delay...

        case SH_ScrollView_FrameOnlyAroundContents:
            return true;

        case SH_ItemView_ShowDecorationSelected:
            return true;

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
                return 5;
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
        case CT_TabBarTab: {
            const QStyleOptionTab* opt = qstyleoption_cast<const QStyleOptionTab*>(option);
            if(!opt) return contentsSize;

            int left = widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Left, option, widget);
            int right = widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Right, option, widget);
            int top =  widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Top, option, widget);
            int bottom = widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + Bot, option, widget);
            top += 2*widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + MainMargin, opt, widget);
            left += 2*widgetLayoutProp(WT_TabBar, TabBar::TabContentsMargin + MainMargin, opt, widget);

            switch(opt->shape) {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                return QSize(contentsSize.width() + left + right + 2, contentsSize.height() + top + bottom);
            default:
                return QSize(contentsSize.width() + top + bottom,  contentsSize.height() + left + right + 2);
            }
        }
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

					r.adjust(fw,fw + qMax(th, cr.height()), -fw, -fw);

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
                            return visualRect(option->direction, option->rect, gcr);
                    }

					// left align labels in flat group boxes, center align labels in framed group boxes
					if (isFlat)
						r = QRect(0,r.y(),tw,r.height());
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
            r = QRect(QPoint(paneRect.x(), paneRect.y() - twf->leftCornerWidgetSize.height()), twf->leftCornerWidgetSize);
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
            r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(), paneRect.y() - twf->rightCornerWidgetSize.height()), twf->rightCornerWidgetSize);
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

    if (static_cast<QWidget*>(obj)->isWindow()) {
        if (ev->type() == QEvent::Paint)
        {
            QWidget *widget = static_cast<QWidget*>(obj);
            QBrush brush = widget->palette().brush(widget->backgroundRole());
            // don't use our background if the app requested something else,
            // e.g. a pixmap
            // TODO - draw our light effects over an arbitrary fill?
            if (brush.style() == Qt::SolidPattern &&
                     !widget->testAttribute(Qt::WA_NoSystemBackground)) {
                QPainter p(widget);
                QPaintEvent *e = (QPaintEvent*)ev;
                p.setClipRegion(e->region());
                renderWindowBackground(&p, e->rect(), widget);
            }
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
            if(dw->isWindow())
            {
                _helper.drawFloatFrame(&p, dw->rect(), dw->palette().color(QPalette::Window));
                return false;
            }

            int x,y,w,h;

            dw->rect().getRect(&x, &y, &w, &h);

            h--;
            p.setPen(QColor(0,0,0, 30));
            p.drawLine(QPointF(6.3, 0.5), QPointF(w-6.3, 0.5));
            p.drawArc(QRectF(0.5, 0.5, 9.5, 9.5),90*16, 90*16);
            p.drawArc(QRectF(w-9.5-0.5, 0.5, 9.5, 9.5), 0, 90*16);
            p.drawLine(QPointF(0.5, 6.3), QPointF(0.5, h-6.3));
            p.drawLine(QPointF(w-0.5, 6.3), QPointF(w-0.5, h-6.3));
            p.drawArc(QRectF(0.5, h-9.5-0.5, 9.5, 9.5),180*16, 90*16);
            p.drawArc(QRectF(w-9.5-0.5, h-9.5-0.5, 9.5, 9.5), 270*16, 90*16);
            p.drawLine(QPointF(6.3, h-0.5), QPointF(w-6.3, h-0.5));
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
            return false;
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
            QPixmap pm = _helper.windecoButton(buttonColor, 5);
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

        case SP_TitleBarCloseButton:
        case SP_DockWidgetCloseButton:
        {
            QPixmap realpm(pixelMetric(QStyle::PM_SmallIconSize,0,0), pixelMetric(QStyle::PM_SmallIconSize,0,0));
            realpm.fill(QColor(0,0,0,0));
            QPixmap pm = _helper.windecoButton(buttonColor,5);
            QPainter painter(&realpm);
            painter.drawPixmap(1,1,pm);
            painter.setRenderHints(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            QLinearGradient lg = _helper.decoGradient(QRect(3,3,11,11), QColor(0,0,0));
            painter.setPen(QPen(lg,1.4));
            painter.drawLine(6.5,6.5,11,11);
            painter.drawLine(11,6.5,6.5,11);

            return QIcon(realpm);
        }
        default:
            return KStyle::standardPixmap(standardIcon, option, widget);
    }
}

void OxygenStyle::renderWindowBackground(QPainter *p, const QRect &clipRect, const QWidget *widget) const
{
    QRect r = widget->rect();
    QColor color = widget->palette().color(widget->backgroundRole());
    int splitY = qMin(300, 3*r.height()/4);

    QRect upperRect = QRect(0, 0, r.width(), splitY);
    QPixmap tile = _helper.verticalGradient(color, splitY);
    p->drawTiledPixmap(upperRect, tile);

    QRect lowerRect = QRect(0,splitY, r.width(), r.height() - splitY);
    p->fillRect(lowerRect, _helper.backgroundBottomColor(color));

    int radialW = qMin(600, r.width());
    int frameH = 32; // on first paint the frame may not have been done yet, so just fixate it
    QRect radialRect = QRect((r.width() - radialW) / 2, 0, radialW, 64-frameH);
    if (clipRect.intersects(radialRect))
    {
        tile = _helper.radialGradient(color, radialW);
        p->drawPixmap(radialRect, tile, QRect(0, frameH, radialW, 64-frameH));
    }
}

// kate: indent-width 4; replace-tabs on; tab-width 4; space-indent on;
