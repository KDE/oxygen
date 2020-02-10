/*************************************************************************************
 * This file was part of KDevPlatform and now of Oxygen                              *
 * Copyright 2016 Zhigalin Alexander <alexander@zhigalin.tk>                         *
 * Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>                             *
 *                                                                                   *
 * This library is free software; you can redistribute it and/or                     *
 * modify it under the terms of the GNU Lesser General Public                        *
 * License as published by the Free Software Foundation; either                      *
 * version 2.1 of the License, or (at your option) version 3, or any                 *
 * later version accepted by the membership of KDE e.V. (or its                      *
 * successor approved by the membership of KDE e.V.), which shall                    *
 * act as a proxy defined in Section 6 of version 3 of the license.                  *
 *                                                                                   *
 * This library is distributed in the hope that it will be useful,                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                 *
 * Lesser General Public License for more details.                                   *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public                  *
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.     *
 *************************************************************************************/

#include "oxygenschemechooser.h"

#include <QStringList>
#include <QActionGroup>
#include <QAction>
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
