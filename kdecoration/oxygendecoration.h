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

#include <KDecoration2/DecoratedClient>
#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationSettings>

#include <QPalette>
#include <QPropertyAnimation>
#include <QVariant>

namespace KDecoration2
{
class DecorationButton;
class DecorationButtonGroup;
}

namespace Oxygen
{
class SizeGrip;

class Decoration : public KDecoration2::Decoration
{
    Q_OBJECT

    //* declare active state opacity
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    //* constructor
    explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());

    //* destructor
    ~Decoration() override;

    //* paint
    void paint(QPainter *painter, const QRect &repaintRegion) override;

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
    void init() override;

private Q_SLOTS:
    void reconfigure();
    void recalculateBorders();
    void updateButtonsGeometry();
    void updateButtonsGeometryDelayed();
    void updateTitleBar();
    void updateAnimationState();
    void updateSizeGripVisibility();
    void updateShadow();

private:
    //* return the rect in which caption will be drawn
    QPair<QRect, Qt::Alignment> captionRect(void) const;

    void createButtons();

    //* window background
    void renderWindowBackground(QPainter *, const QRect &, const QPalette &) const;

    //* window border
    void renderWindowBorder(QPainter *, const QRect &, const QPalette &) const;

    //* title text
    void renderTitleText(QPainter *, const QPalette &) const;

    //* corners
    void renderCorners(QPainter *, const QRect &, const QPalette &) const;

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

    //*@name size grip
    //@{
    void createSizeGrip(void);
    void deleteSizeGrip(void);
    SizeGrip *sizeGrip(void) const
    {
        return m_sizeGrip;
    }
    //@}

    InternalSettingsPtr m_internalSettings;

    QList<KDecoration2::DecorationButton *> m_buttons;
    KDecoration2::DecorationButtonGroup *m_leftButtons = nullptr;
    KDecoration2::DecorationButtonGroup *m_rightButtons = nullptr;

    //* size grip widget
    SizeGrip *m_sizeGrip = nullptr;

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
        return settings()->borderSize() == KDecoration2::BorderSize::None;
}

bool Decoration::hasNoSideBorders(void) const
{
    if (m_internalSettings && m_internalSettings->mask() & BorderSize)
        return m_internalSettings->borderSize() == InternalSettings::BorderNoSides;
    else
        return settings()->borderSize() == KDecoration2::BorderSize::NoSides;
}

bool Decoration::isMaximized(void) const
{
    return client().toStrongRef()->isMaximized() && !m_internalSettings->drawBorderOnMaximizedWindows();
}
bool Decoration::isMaximizedHorizontally(void) const
{
    return client().toStrongRef()->isMaximizedHorizontally() && !m_internalSettings->drawBorderOnMaximizedWindows();
}
bool Decoration::isMaximizedVertically(void) const
{
    return client().toStrongRef()->isMaximizedVertically() && !m_internalSettings->drawBorderOnMaximizedWindows();
}
bool Decoration::hideTitleBar(void) const
{
    return m_internalSettings->hideTitleBar() && !client().toStrongRef()->isShaded();
}
}

#endif
