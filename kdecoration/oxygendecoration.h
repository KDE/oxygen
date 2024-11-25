#ifndef OXYGEN_DECORATION_H
#define OXYGEN_DECORATION_H

/*
 * SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "oxygen.h"
#include "oxygensettings.h"

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationSettings>

#include <QPalette>
#include <QPropertyAnimation>
#include <QVariant>

namespace KDecoration3
{
class DecorationButton;
class DecorationButtonGroup;
}

namespace Oxygen
{

class Decoration : public KDecoration3::Decoration
{
    Q_OBJECT

    //* declare active state opacity
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    //* constructor
    explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;

    void paint(QPainter *painter, const QRectF &repaintRegion) override;
    QMarginsF bordersFor(double scale) const override;

    //* internal settings
    InternalSettingsPtr internalSettings() const
    {
        return m_internalSettings;
    }

    //* caption height
    int captionHeight() const;

    //* button height
    int buttonHeight() const;

    //*@name active state change animation
    //@{
    bool isAnimated(void) const
    {
        return m_animation->state() == QPropertyAnimation::Running;
    }

    void setOpacity(qreal);

    qreal opacity(void) const
    {
        return m_opacity;
    }

    //@}

    //*@name colors
    //@{

    QColor titleBarColor(const QPalette &) const;
    QColor titleBarColor(const QPalette &, bool active) const;

    QColor fontColor(const QPalette &) const;
    QColor fontColor(const QPalette &palette, bool active) const;

    QColor contrastColor(const QPalette &palette) const;
    QColor contrastColor(const QColor &color) const;

    //@}

public Q_SLOTS:
    bool init() override;

private Q_SLOTS:
    void reconfigure();
    void recalculateBorders();
    void updateButtonsGeometry();
    void updateButtonsGeometryDelayed();
    void updateTitleBar();
    void updateAnimationState();
    void updateShadow();

private:
    //* return the rect in which caption will be drawn
    QPair<QRect, Qt::Alignment> captionRect(void) const;

    void createButtons();

    //* window background
    void renderWindowBackground(QPainter *, const QRectF &, const QPalette &) const;

    //* window border
    void renderWindowBorder(QPainter *, const QRectF &, const QPalette &) const;

    //* title text
    void renderTitleText(QPainter *, const QPalette &) const;

    //* corners
    void renderCorners(QPainter *, const QRectF &, const QPalette &) const;

    //*@name border size
    //@{
    int borderSize(bool bottom = false) const;
    inline bool hasNoBorders(void) const;
    inline bool hasNoSideBorders(void) const;
    //@}

    //*@name maximization modes
    //@{
    inline bool isMaximized(void) const;
    inline bool isMaximizedHorizontally(void) const;
    inline bool isMaximizedVertically(void) const;
    inline bool hideTitleBar(void) const;
    //@}

    InternalSettingsPtr m_internalSettings;

    QList<KDecoration3::DecorationButton *> m_buttons;
    KDecoration3::DecorationButtonGroup *m_leftButtons = nullptr;
    KDecoration3::DecorationButtonGroup *m_rightButtons = nullptr;

    //* active state change animation
    QPropertyAnimation *m_animation;

    //* active state change opacity
    qreal m_opacity = 0;
};

bool Decoration::hasNoBorders(void) const
{
    if (m_internalSettings && m_internalSettings->mask() & BorderSize)
        return m_internalSettings->borderSize() == InternalSettings::BorderNone;
    else
        return settings()->borderSize() == KDecoration3::BorderSize::None;
}

bool Decoration::hasNoSideBorders(void) const
{
    if (m_internalSettings && m_internalSettings->mask() & BorderSize)
        return m_internalSettings->borderSize() == InternalSettings::BorderNoSides;
    else
        return settings()->borderSize() == KDecoration3::BorderSize::NoSides;
}

bool Decoration::isMaximized(void) const
{
    return window()->isMaximized() && !m_internalSettings->drawBorderOnMaximizedWindows();
}
bool Decoration::isMaximizedHorizontally(void) const
{
    return window()->isMaximizedHorizontally() && !m_internalSettings->drawBorderOnMaximizedWindows();
}
bool Decoration::isMaximizedVertically(void) const
{
    return window()->isMaximizedVertically() && !m_internalSettings->drawBorderOnMaximizedWindows();
}
bool Decoration::hideTitleBar(void) const
{
    return m_internalSettings->hideTitleBar() && !window()->isShaded();
}
}

#endif
