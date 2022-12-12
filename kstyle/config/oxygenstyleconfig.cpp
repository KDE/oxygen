/*
SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
SPDX-FileCopyrightText: 2009 Matthew Woehlke <mw.triad@users.sourceforge.net>
SPDX-FileCopyrightText: 2009 Long Huynh Huu <long.upcase@googlemail.com>
SPDX-FileCopyrightText: 2003 Sandro Giessl <ceebx@users.sourceforge.net>

originally based on the Keramik configuration dialog:
SPDX-FileCopyrightText: 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

SPDX-License-Identifier: MIT

*/

#include "oxygenstyleconfig.h"
#include "../oxygen.h"
#include "config-liboxygen.h"
#include "oxygenanimationconfigwidget.h"
#include "oxygenstyleconfigdata.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QTextStream>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#define SCROLLBAR_DEFAULT_WIDTH 15
#define SCROLLBAR_MINIMUM_WIDTH 10
#define SCROLLBAR_MAXIMUM_WIDTH 30

extern "C" {
Q_DECL_EXPORT QWidget *allocate_kstyle_config(QWidget *parent)
{
    return new Oxygen::StyleConfig(parent);
}
}

namespace Oxygen
{

//__________________________________________________________________
StyleConfig::StyleConfig(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    // load setup from configData
    load();

    connect(_useBackgroundGradient, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_toolBarDrawItemSeparator, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_splitterProxyEnabled, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_mnemonicsMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_viewDrawFocusIndicator, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_viewTriangularExpanderSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_viewDrawTreeBranchLines, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_scrollBarWidth, SIGNAL(valueChanged(int)), SLOT(updateChanged()));
    connect(_scrollBarAddLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_scrollBarSubLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));
    connect(_menuHighlightDark, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_menuHighlightStrong, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_menuHighlightSubtle, SIGNAL(toggled(bool)), SLOT(updateChanged()));
    connect(_windowDragMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()));

    connect(_animationConfigWidget, SIGNAL(changed(bool)), SLOT(updateChanged()));
    connect(_animationConfigWidget, SIGNAL(layoutChanged()), SLOT(updateLayout()));
}

//__________________________________________________________________
void StyleConfig::save(void)
{
    StyleConfigData::setUseBackgroundGradient(_useBackgroundGradient->isChecked());
    StyleConfigData::setToolBarDrawItemSeparator(_toolBarDrawItemSeparator->isChecked());
    StyleConfigData::setSplitterProxyEnabled(_splitterProxyEnabled->isChecked());
    StyleConfigData::setMnemonicsMode(_mnemonicsMode->currentIndex());
    StyleConfigData::setViewDrawFocusIndicator(_viewDrawFocusIndicator->isChecked());
    StyleConfigData::setViewTriangularExpanderSize(triangularExpanderSize());
    StyleConfigData::setViewDrawTreeBranchLines(_viewDrawTreeBranchLines->isChecked());
    StyleConfigData::setScrollBarWidth(_scrollBarWidth->value());
    StyleConfigData::setScrollBarAddLineButtons(_scrollBarAddLineButtons->currentIndex());
    StyleConfigData::setScrollBarSubLineButtons(_scrollBarSubLineButtons->currentIndex());
    StyleConfigData::setMenuHighlightMode(menuMode());
    StyleConfigData::setWindowDragMode(_windowDragMode->currentIndex());
    StyleConfigData::setWindowDragEnabled(_windowDragMode->currentIndex() != StyleConfigData::WD_NONE);

    _animationConfigWidget->save();

    StyleConfigData::self()->save();

    // emit dbus signal
    QDBusMessage message(
        QDBusMessage::createSignal(QStringLiteral("/OxygenStyle"), QStringLiteral("org.kde.Oxygen.Style"), QStringLiteral("reparseConfiguration")));
    QDBusConnection::sessionBus().send(message);
}

//__________________________________________________________________
void StyleConfig::defaults(void)
{
    StyleConfigData::self()->setDefaults();
    load();
}

//__________________________________________________________________
void StyleConfig::reset(void)
{
    // reparse configuration
    StyleConfigData::self()->load();

    load();
}

//__________________________________________________________________
bool StyleConfig::event(QEvent *event)
{
    const bool result(QWidget::event(event));
    switch (event->type()) {
    case QEvent::Show:
    case QEvent::ShowToParent:

        // explicitly update minimum size from hint
        // this is needed to automatically resize the window to fit animations tab
        setMinimumSize(minimumSizeHint());
        break;

    default:
        break;
    }

    return result;
}

//__________________________________________________________________
void StyleConfig::updateLayout(void)
{
    const int delta = _animationConfigWidget->minimumSizeHint().height() - _animationConfigWidget->size().height();
    window()->setMinimumSize(QSize(window()->minimumSizeHint().width(), window()->size().height() + delta));
}

//__________________________________________________________________
void StyleConfig::updateChanged()
{
    bool modified(false);

    // check if any value was modified
    if (_useBackgroundGradient->isChecked() != StyleConfigData::useBackgroundGradient())
        modified = true;
    if (_toolBarDrawItemSeparator->isChecked() != StyleConfigData::toolBarDrawItemSeparator())
        modified = true;
    else if (_mnemonicsMode->currentIndex() != StyleConfigData::mnemonicsMode())
        modified = true;
    else if (_viewDrawTreeBranchLines->isChecked() != StyleConfigData::viewDrawTreeBranchLines())
        modified = true;
    else if (_scrollBarWidth->value() != StyleConfigData::scrollBarWidth())
        modified = true;
    else if (_scrollBarAddLineButtons->currentIndex() != StyleConfigData::scrollBarAddLineButtons())
        modified = true;
    else if (_scrollBarSubLineButtons->currentIndex() != StyleConfigData::scrollBarSubLineButtons())
        modified = true;
    else if (_splitterProxyEnabled->isChecked() != StyleConfigData::splitterProxyEnabled())
        modified = true;
    else if (menuMode() != StyleConfigData::menuHighlightMode())
        modified = true;
    else if (_viewDrawFocusIndicator->isChecked() != StyleConfigData::viewDrawFocusIndicator())
        modified = true;
    else if (triangularExpanderSize() != StyleConfigData::viewTriangularExpanderSize())
        modified = true;
    else if (_animationConfigWidget && _animationConfigWidget->isChanged())
        modified = true;
    else if (_windowDragMode->currentIndex() != StyleConfigData::windowDragMode())
        modified = true;

    emit changed(modified);
}

//__________________________________________________________________
void StyleConfig::load(void)
{
    _useBackgroundGradient->setChecked(StyleConfigData::useBackgroundGradient());
    _toolBarDrawItemSeparator->setChecked(StyleConfigData::toolBarDrawItemSeparator());
    _mnemonicsMode->setCurrentIndex(StyleConfigData::mnemonicsMode());
    _splitterProxyEnabled->setChecked(StyleConfigData::splitterProxyEnabled());
    _viewDrawTreeBranchLines->setChecked(StyleConfigData::viewDrawTreeBranchLines());

    _scrollBarWidth->setValue(qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH, StyleConfigData::scrollBarWidth())));

    _scrollBarAddLineButtons->setCurrentIndex(StyleConfigData::scrollBarAddLineButtons());
    _scrollBarSubLineButtons->setCurrentIndex(StyleConfigData::scrollBarSubLineButtons());

    // menu highlight
    _menuHighlightDark->setChecked(StyleConfigData::menuHighlightMode() == StyleConfigData::MM_DARK);
    _menuHighlightStrong->setChecked(StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG);
    _menuHighlightSubtle->setChecked(StyleConfigData::menuHighlightMode() == StyleConfigData::MM_SUBTLE);

    _windowDragMode->setCurrentIndex(StyleConfigData::windowDragMode());

    _viewDrawFocusIndicator->setChecked(StyleConfigData::viewDrawFocusIndicator());
    switch (StyleConfigData::viewTriangularExpanderSize()) {
    case StyleConfigData::TE_TINY:
        _viewTriangularExpanderSize->setCurrentIndex(0);
        break;
    case StyleConfigData::TE_SMALL:
    default:
        _viewTriangularExpanderSize->setCurrentIndex(1);
        break;
    case StyleConfigData::TE_NORMAL:
        _viewTriangularExpanderSize->setCurrentIndex(2);
        break;
    }

    // animation config widget
    if (_animationConfigWidget)
        _animationConfigWidget->load();
}

//____________________________________________________________
int StyleConfig::menuMode(void) const
{
    if (_menuHighlightDark->isChecked())
        return StyleConfigData::MM_DARK;
    else if (_menuHighlightSubtle->isChecked())
        return StyleConfigData::MM_SUBTLE;
    else
        return StyleConfigData::MM_STRONG;
}

//____________________________________________________________
int StyleConfig::triangularExpanderSize(void) const
{
    switch (_viewTriangularExpanderSize->currentIndex()) {
    case 0:
        return StyleConfigData::TE_TINY;
    case 1:
    default:
        return StyleConfigData::TE_SMALL;
    case 2:
        return StyleConfigData::TE_NORMAL;
    }
}
}
