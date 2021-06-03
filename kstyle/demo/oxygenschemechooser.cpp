/*
    This file was part of KDevPlatform and now of Oxygen
    SPDX-FileCopyrightText: 2016 Zhigalin Alexander <alexander@zhigalin.tk>
    SPDX-FileCopyrightText: 2017 René J.V. Bertin <rjvbertin@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "oxygenschemechooser.h"

#include <QStringList>
#include <QActionGroup>
#include <QMenu>
#include <QModelIndex>

#include <KActionMenu>
#include <KColorSchemeManager>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>

namespace Oxygen
{

ColorSchemeChooser::ColorSchemeChooser( QWidget* parent )
    : QPushButton( parent )
{
    auto manager = new KColorSchemeManager( parent );

    auto selectionMenu = manager->createSchemeSelectionMenu(this);

    setMenu( selectionMenu->menu() );

    setIcon( menu()->icon() );
    setText( menu()->title() );
}

}  // namespace Oxygen
