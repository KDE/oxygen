/*
    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "oxygendecoration.h"

#include "config-oxygen.h"
#include "oxygensettingsprovider.h"
#include "oxygenshadowcache.h"

#include "oxygenbutton.h"

#include <KDecoration3/DecorationButtonGroup>
#include <KDecoration3/DecorationShadow>

#include <KColorUtils>
#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QPainter>
#include <QTextStream>
#include <QTimer>

#if OXYGEN_HAVE_X11
#include <private/qtx11extras_p.h>
#endif

K_PLUGIN_FACTORY_WITH_JSON(OxygenDecoFactory, "oxygen.json", registerPlugin<Oxygen::Decoration>(); registerPlugin<Oxygen::Button>();)

namespace Oxygen
{
using KDecoration3::ColorGroup;
using KDecoration3::ColorRole;

//________________________________________________________________
using ShadowMap = QHash<int, std::shared_ptr<KDecoration3::DecorationShadow>>;

static int g_sDecoCount = 0;
static ShadowMap g_sShadows;

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
    , m_animation(new QPropertyAnimation(this))
{
    g_sDecoCount++;
}

//________________________________________________________________
Decoration::~Decoration()
{
    g_sDecoCount--;
    if (g_sDecoCount == 0)
        g_sShadows.clear();
}

//________________________________________________________________
void Decoration::setOpacity(qreal value)
{
    if (m_opacity == value)
        return;
    m_opacity = value;
    updateShadow();
    update();
}

//_________________________________________________________
QColor Decoration::titleBarColor(const QPalette &palette) const
{
    if (m_animation->state() == QPropertyAnimation::Running) {
        return KColorUtils::mix(titleBarColor(palette, false), titleBarColor(palette, true), m_opacity);

    } else {
        return titleBarColor(palette, window()->isActive());
    }
}

//_________________________________________________________
QColor Decoration::titleBarColor(const QPalette &palette, bool active) const
{
    if (m_internalSettings->useWindowColors()) {
        return palette.color(active ? QPalette::Active : QPalette::Inactive, QPalette::Window);

    } else {
        return window()->color(active ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::TitleBar);
    }
}

//_________________________________________________________
QColor Decoration::fontColor(const QPalette &palette) const
{
    if (hideTitleBar())
        return fontColor(palette, false);
    if (m_animation->state() == QPropertyAnimation::Running) {
        return KColorUtils::mix(fontColor(palette, false), fontColor(palette, true), m_opacity);

    } else {
        return fontColor(palette, window()->isActive());
    }
}

//_________________________________________________________
QColor Decoration::fontColor(const QPalette &palette, bool active) const
{
    if (m_internalSettings->useWindowColors()) {
        return palette.color(active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText);

    } else {
        return window()->color(active ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::Foreground);
    }
}

//_________________________________________________________
QColor Decoration::contrastColor(const QPalette &palette) const
{
    if (m_internalSettings->useWindowColors())
        return contrastColor(palette.color(QPalette::Window));
    else {
        const auto cl = window();
        return contrastColor(cl->color(cl->isActive() ? ColorGroup::Active : ColorGroup::Inactive, ColorRole::TitleBar));
    }
}

//_________________________________________________________
QColor Decoration::contrastColor(const QColor &color) const
{
    return SettingsProvider::self()->helper()->calcLightColor(color);
}

//________________________________________________________________
bool Decoration::init()
{
    // active state change animation
    m_animation->setStartValue(0);
    m_animation->setEndValue(1.0);
    m_animation->setTargetObject(this);
    m_animation->setPropertyName("opacity");
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);

    reconfigure();
    updateTitleBar();
    auto s = settings();
    connect(s.get(), &KDecoration3::DecorationSettings::borderSizeChanged, this, &Decoration::recalculateBorders);

    // a change in font might cause the borders to change
    connect(s.get(), &KDecoration3::DecorationSettings::fontChanged, this, &Decoration::recalculateBorders);
    connect(s.get(), &KDecoration3::DecorationSettings::spacingChanged, this, &Decoration::recalculateBorders);

    // buttons
    connect(s.get(), &KDecoration3::DecorationSettings::spacingChanged, this, &Decoration::updateButtonsGeometryDelayed);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsLeftChanged, this, &Decoration::updateButtonsGeometryDelayed);
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsRightChanged, this, &Decoration::updateButtonsGeometryDelayed);

    // full reconfiguration
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, this, &Decoration::reconfigure);
    connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, SettingsProvider::self(), &SettingsProvider::reconfigure, Qt::UniqueConnection);

    const auto *cl = window();
    connect(cl, &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, &Decoration::recalculateBorders);
    connect(cl, &KDecoration3::DecoratedWindow::maximizedHorizontallyChanged, this, &Decoration::recalculateBorders);
    connect(cl, &KDecoration3::DecoratedWindow::maximizedVerticallyChanged, this, &Decoration::recalculateBorders);
    connect(cl, &KDecoration3::DecoratedWindow::captionChanged, this, [this]() {
        // update the caption area
        update(titleBar());
    });

    connect(cl, &KDecoration3::DecoratedWindow::activeChanged, this, &Decoration::updateAnimationState);
    connect(cl, &KDecoration3::DecoratedWindow::activeChanged, this, &Decoration::updateShadow);

    // decoration has an overloaded update function, force the compiler to choose the right one
    connect(cl, &KDecoration3::DecoratedWindow::paletteChanged, this, static_cast<void (Decoration::*)()>(&Decoration::update));
    connect(this, &KDecoration3::Decoration::bordersChanged, this, &Decoration::updateTitleBar);
    connect(cl, &KDecoration3::DecoratedWindow::widthChanged, this, &Decoration::updateTitleBar);
    connect(cl, &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::updateTitleBar);
    connect(cl, &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::setOpaque);

    connect(cl, &KDecoration3::DecoratedWindow::widthChanged, this, &Decoration::updateButtonsGeometry);
    connect(cl, &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::updateButtonsGeometry);
    connect(cl, &KDecoration3::DecoratedWindow::shadedChanged, this, &Decoration::recalculateBorders);
    connect(cl, &KDecoration3::DecoratedWindow::shadedChanged, this, &Decoration::updateButtonsGeometry);

    createButtons();
    updateShadow();
    return true;
}

//________________________________________________________________
void Decoration::updateTitleBar()
{
    auto s = settings();
    const bool maximized = isMaximized();
    const auto clientPtr = window();
    const int width = maximized ? clientPtr->width() : clientPtr->width() - 2 * s->largeSpacing() * Metrics::TitleBar_SideMargin;
    const int height = maximized ? borderTop() : borderTop() - s->smallSpacing() * Metrics::TitleBar_TopMargin;
    const int x = maximized ? 0 : s->largeSpacing() * Metrics::TitleBar_SideMargin;
    const int y = maximized ? 0 : s->smallSpacing() * Metrics::TitleBar_TopMargin;
    setTitleBar(QRect(x, y, width, height));
}

//________________________________________________________________
void Decoration::updateAnimationState()
{
    if (m_internalSettings->animationsEnabled()) {
        m_animation->setDirection(window()->isActive() ? QPropertyAnimation::Forward : QPropertyAnimation::Backward);
        if (m_animation->state() != QPropertyAnimation::Running)
            m_animation->start();

    } else {
        update();
    }
}

//________________________________________________________________
int Decoration::borderSize(bool bottom) const
{
    const int baseSize = settings()->smallSpacing();
    if (m_internalSettings && (m_internalSettings->mask() & BorderSize)) {
        switch (m_internalSettings->borderSize()) {
        case InternalSettings::BorderNone:
            return 0;
        case InternalSettings::BorderNoSides:
            return bottom ? qMax(4, baseSize) : 0;
        default:
        case InternalSettings::BorderTiny:
            return bottom ? qMax(4, baseSize) : baseSize;
        case InternalSettings::BorderNormal:
            return baseSize * 2;
        case InternalSettings::BorderLarge:
            return baseSize * 3;
        case InternalSettings::BorderVeryLarge:
            return baseSize * 4;
        case InternalSettings::BorderHuge:
            return baseSize * 5;
        case InternalSettings::BorderVeryHuge:
            return baseSize * 6;
        case InternalSettings::BorderOversized:
            return baseSize * 10;
        }

    } else {
        switch (settings()->borderSize()) {
        case KDecoration3::BorderSize::None:
            return 0;
        case KDecoration3::BorderSize::NoSides:
            return bottom ? qMax(4, baseSize) : 0;
        default:
        case KDecoration3::BorderSize::Tiny:
            return bottom ? qMax(4, baseSize) : baseSize;
        case KDecoration3::BorderSize::Normal:
            return baseSize * 2;
        case KDecoration3::BorderSize::Large:
            return baseSize * 3;
        case KDecoration3::BorderSize::VeryLarge:
            return baseSize * 4;
        case KDecoration3::BorderSize::Huge:
            return baseSize * 5;
        case KDecoration3::BorderSize::VeryHuge:
            return baseSize * 6;
        case KDecoration3::BorderSize::Oversized:
            return baseSize * 10;
        }
    }
}

//________________________________________________________________
void Decoration::reconfigure()
{
    m_internalSettings = SettingsProvider::self()->internalSettings(this);

    // animation
    m_animation->setDuration(m_internalSettings->shadowAnimationsDuration());

    // borders
    recalculateBorders();

    // clear shadows
    g_sShadows.clear();
}

//________________________________________________________________
void Decoration::recalculateBorders()
{
    auto s = settings();
    const auto c = window();
    const auto edges = c->adjacentScreenEdges();

    // left, right and bottom borders
    auto testFlag = [&](Qt::Edge edge) {
        return edges.testFlag(edge) && !m_internalSettings->drawBorderOnMaximizedWindows();
    };
    const int left = isMaximizedHorizontally() || testFlag(Qt::LeftEdge) ? 0 : borderSize();
    const int right = isMaximizedHorizontally() || testFlag(Qt::RightEdge) ? 0 : borderSize();
    const int bottom = isMaximizedVertically() || c->isShaded() || testFlag(Qt::BottomEdge) ? 0 : borderSize(true);

    int top = 0;
    if (hideTitleBar())
        top = bottom;
    else {
        QFontMetrics fm(s->font());
        top += qMax(fm.height(), buttonHeight());

        // padding below
        const int baseSize = s->smallSpacing();
        top += baseSize * Metrics::TitleBar_BottomMargin;

        // padding above
        top += baseSize * TitleBar_TopMargin;
    }

    setBorders(QMargins(left, top, right, bottom));

    // extended sizes
    const int extSize = s->largeSpacing();
    int extSides = 0;
    int extBottom = 0;
    if (hasNoBorders()) {
        if (!isMaximizedHorizontally())
            extSides = extSize;
        if (!isMaximizedVertically())
            extBottom = extSize;

    } else if (hasNoSideBorders() && !isMaximizedHorizontally()) {
        extSides = extSize;
    }

    setResizeOnlyBorders(QMargins(extSides, 0, extSides, extBottom));
}

//________________________________________________________________
void Decoration::createButtons()
{
    m_leftButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Left, this, &Button::create);
    m_rightButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Right, this, &Button::create);
    updateButtonsGeometry();
}

//________________________________________________________________
void Decoration::updateButtonsGeometryDelayed()
{
    QTimer::singleShot(0, this, &Decoration::updateButtonsGeometry);
}

//________________________________________________________________
void Decoration::updateButtonsGeometry()
{
    auto s = settings();

    // adjust button position
    const int bHeight = captionHeight() + (isMaximized() ? s->smallSpacing() * Metrics::TitleBar_TopMargin : 0);
    const int bWidth = buttonHeight();
    const int verticalOffset = (isMaximized() ? s->smallSpacing() * Metrics::TitleBar_TopMargin : 0) + (captionHeight() - buttonHeight()) / 2;

    const QVector<KDecoration3::DecorationButton *> leftButtons = m_leftButtons->buttons();
    const QVector<KDecoration3::DecorationButton *> rightButtons = m_rightButtons->buttons();

    const auto allButtons = leftButtons + rightButtons;
    for (const auto &button : allButtons) {
        button->setGeometry(QRectF(QPoint(0, 0), QSizeF(bWidth, bHeight)));
        static_cast<Button *>(button)->setOffset(QPointF(0, verticalOffset));
        static_cast<Button *>(button)->setIconSize(QSize(bWidth, bWidth));
    }

    // left buttons
    if (!leftButtons.isEmpty()) {
        // spacing
        m_leftButtons->setSpacing(s->smallSpacing() * Metrics::TitleBar_ButtonSpacing);

        // padding
        const int vPadding = isMaximized() ? 0 : s->smallSpacing() * Metrics::TitleBar_TopMargin;
        const int hPadding = s->smallSpacing() * Metrics::TitleBar_SideMargin;
        if (isMaximizedHorizontally()) {
            // add offsets on the side buttons, to preserve padding, but satisfy Fitts law
            auto button = static_cast<Button *>(leftButtons.front());
            button->setGeometry(QRectF(QPoint(0, 0), QSizeF(bWidth + hPadding, bHeight)));
            button->setFlag(Button::FlagFirstInList);
            button->setHorizontalOffset(hPadding);

            m_leftButtons->setPos(QPointF(0, vPadding));

        } else
            m_leftButtons->setPos(QPointF(hPadding + borderLeft(), vPadding));
    }

    // right buttons
    if (!rightButtons.isEmpty()) {
        // spacing
        m_rightButtons->setSpacing(s->smallSpacing() * Metrics::TitleBar_ButtonSpacing);

        // padding
        const int vPadding = isMaximized() ? 0 : s->smallSpacing() * Metrics::TitleBar_TopMargin;
        const int hPadding = s->smallSpacing() * Metrics::TitleBar_SideMargin;
        if (isMaximizedHorizontally()) {
            auto button = static_cast<Button *>(rightButtons.back());
            button->setGeometry(QRectF(QPoint(0, 0), QSizeF(bWidth + hPadding, bHeight)));
            button->setFlag(Button::FlagLastInList);

            m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width(), vPadding));

        } else
            m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width() - hPadding - borderRight(), vPadding));
    }

    update();
}

//________________________________________________________________
void Decoration::paint(QPainter *painter, const QRectF &repaintRegion)
{
    const auto c = window();
    const auto palette = c->palette();

    const auto rect = c->isShaded() ? QRect(QPoint(0, 0), QSize(size().width(), borderTop())) : this->rect();
    renderWindowBorder(painter, rect, palette);
    if (!isMaximized())
        renderCorners(painter, rect, palette);

    if (!hideTitleBar()) {
        m_leftButtons->paint(painter, repaintRegion);
        m_rightButtons->paint(painter, repaintRegion);

        renderTitleText(painter, palette);
    }
}

//________________________________________________________________
int Decoration::buttonHeight() const
{
    const int baseSize = settings()->gridUnit() + 2; // oxygen icons were always slightly larger
    switch (m_internalSettings->buttonSize()) {
    case InternalSettings::ButtonSmall:
        return baseSize * 1.5;
    default:
    case InternalSettings::ButtonDefault:
        return baseSize * 2;
    case InternalSettings::ButtonLarge:
        return baseSize * 2.5;
    case InternalSettings::ButtonVeryLarge:
        return baseSize * 3.5;
    }
}

//________________________________________________________________
int Decoration::captionHeight() const
{
    return hideTitleBar() ? borderTop() : borderTop() - settings()->smallSpacing() * (Metrics::TitleBar_BottomMargin + Metrics::TitleBar_TopMargin);
}

//________________________________________________________________
QPair<QRect, Qt::Alignment> Decoration::captionRect() const
{
    if (hideTitleBar())
        return qMakePair(QRect(), Qt::AlignCenter);
    else {
        const int leftOffset = m_leftButtons->buttons().isEmpty()
            ? Metrics::TitleBar_SideMargin * settings()->smallSpacing()
            : m_leftButtons->geometry().x() + m_leftButtons->geometry().width() + Metrics::TitleBar_SideMargin * settings()->smallSpacing();

        const int rightOffset = m_rightButtons->buttons().isEmpty()
            ? Metrics::TitleBar_SideMargin * settings()->smallSpacing()
            : size().width() - m_rightButtons->geometry().x() + Metrics::TitleBar_SideMargin * settings()->smallSpacing();

        const int yOffset = settings()->smallSpacing() * Metrics::TitleBar_TopMargin;
        const QRect maxRect(leftOffset, yOffset, size().width() - leftOffset - rightOffset, captionHeight());

        switch (m_internalSettings->titleAlignment()) {
        case InternalSettings::AlignLeft:
            return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignLeft);

        case InternalSettings::AlignRight:
            return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignRight);

        case InternalSettings::AlignCenter:
            return qMakePair(maxRect, Qt::AlignCenter);

        default:
        case InternalSettings::AlignCenterFullWidth: {
            // full caption rect
            const QRect fullRect = QRect(0, yOffset, size().width(), captionHeight());
            QRect boundingRect(settings()->fontMetrics().boundingRect(window()->caption()).toRect());

            // text bounding rect
            boundingRect.setTop(yOffset);
            boundingRect.setHeight(captionHeight());
            boundingRect.moveLeft((size().width() - boundingRect.width()) / 2);

            if (boundingRect.left() < leftOffset)
                return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignLeft);
            else if (boundingRect.right() > size().width() - rightOffset)
                return qMakePair(maxRect, Qt::AlignVCenter | Qt::AlignRight);
            else
                return qMakePair(fullRect, Qt::AlignCenter);
        }
        }
    }
}

//________________________________________________________________
void Decoration::updateShadow()
{
    // do nothing if palettes are disabled
    if (!(SettingsProvider::self()->shadowCache()->isEnabled(QPalette::Active) || SettingsProvider::self()->shadowCache()->isEnabled(QPalette::Inactive))) {
        return;
    }

    // see if shadow should be animated
    const bool animated(m_animation->state() == QPropertyAnimation::Running && SettingsProvider::self()->shadowCache()->isEnabled(QPalette::Active)
                        && SettingsProvider::self()->shadowCache()->isEnabled(QPalette::Inactive));

    // generate key
    ShadowCache::Key key;
    const auto clientPtr = window();
    key.active = SettingsProvider::self()->shadowCache()->isEnabled(QPalette::Active) && clientPtr->isActive();
    key.isShade = clientPtr->isShaded();
    key.hasBorder = !hasNoBorders();

    if (animated) {
        static const int maxIndex = 255;
        key.index = m_opacity * maxIndex;
    }

    const int hash(key.hash());

    // find key in map
    auto iter = g_sShadows.find(hash);
    if (iter == g_sShadows.end()) {
        auto decorationShadow = std::make_shared<KDecoration3::DecorationShadow>();
        QPixmap shadowPixmap =
            animated ? SettingsProvider::self()->shadowCache()->animatedPixmap(key, m_opacity) : SettingsProvider::self()->shadowCache()->pixmap(key);

        const int shadowSize(shadowPixmap.width() / 2);
        const int overlap = 4;
        decorationShadow->setPadding(QMargins(shadowSize - overlap, shadowSize - overlap, shadowSize - overlap, shadowSize - overlap));
        decorationShadow->setInnerShadowRect(QRect(shadowSize, shadowSize, 1, 1));
        decorationShadow->setShadow(shadowPixmap.toImage());

        iter = g_sShadows.insert(hash, decorationShadow);
    }

    setShadow(iter.value());
}

//_________________________________________________________
void Decoration::renderCorners(QPainter *painter, const QRectF &frame, const QPalette &palette) const
{
    const QColor color(titleBarColor(palette));

    QLinearGradient lg = QLinearGradient(0, -0.5, 0, frame.height() + 0.5);
    lg.setColorAt(0.0, SettingsProvider::self()->helper()->calcLightColor(SettingsProvider::self()->helper()->backgroundTopColor(color)));
    lg.setColorAt(0.51, SettingsProvider::self()->helper()->backgroundBottomColor(color));
    lg.setColorAt(1.0, SettingsProvider::self()->helper()->backgroundBottomColor(color));

    painter->setPen(QPen(lg, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(frame.adjusted(0.5, 0.5, -0.5, -0.5), 3.5, 3.5);
}

//_________________________________________________________
void Decoration::renderWindowBackground(QPainter *painter, const QRectF &clipRect, const QPalette &palette) const
{
    const auto c = window();
    auto innerClientRect = c->isShaded() ? QRect(QPoint(0, 0), QSize(size().width(), borderTop())) : rect();

    // size of window minus the outlines for the rounded corners
    if (settings()->isAlphaChannelSupported() && !isMaximized()) {
        innerClientRect.adjust(1, 1, -1, -1);
    }

    SettingsProvider::self()->helper()->renderWindowBackground(painter, clipRect, innerClientRect, titleBarColor(palette), borderTop() - 24);
}

//_________________________________________________________
void Decoration::renderWindowBorder(QPainter *painter, const QRectF &clipRect, const QPalette &palette) const
{
    // save painter
    if (clipRect.isValid()) {
        painter->save();
        painter->setClipRect(clipRect, Qt::IntersectClip);
    }

    // title height
    renderWindowBackground(painter, clipRect, palette);

    // restore painter
    if (clipRect.isValid()) {
        painter->restore();
    }
}

//_________________________________________________________
void Decoration::renderTitleText(QPainter *painter, const QPalette &palette) const
{
    // setup font
    painter->setFont(settings()->font());

    // caption rect
    const auto cR = captionRect();

    // copy caption
    const QString caption = painter->fontMetrics().elidedText(window()->caption(), Qt::ElideMiddle, cR.first.width());

    const auto contrast(contrastColor(palette));
    if (contrast.isValid()) {
        painter->setPen(contrast);
        painter->translate(0, 1);
        painter->drawText(cR.first, cR.second | Qt::TextSingleLine, caption);
        painter->translate(0, -1);
    }

    const auto color(fontColor(palette));
    painter->setPen(color);
    painter->drawText(cR.first, cR.second | Qt::TextSingleLine, caption);
}

} // namespace

#include "oxygendecoration.moc"
