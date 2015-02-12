#ifndef oxygen_h
#define oxygen_h

/*
 * Copyright 2014  Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * Copyright 2015  David Edmundson <davidedmundson@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "oxygensettings.h"

#include <QSharedPointer>
#include <QList>

namespace Oxygen
{
    //* convenience typedefs
    using InternalSettingsPtr = QSharedPointer<InternalSettings>;
    using InternalSettingsList = QList<InternalSettingsPtr>;
    using InternalSettingsListIterator = QListIterator<InternalSettingsPtr>;

    //* metrics
    enum Metrics
    {

        //* corner radius (pixels)
        Frame_FrameRadius = 3,

        //* titlebar metrics, in units of small spacing
        TitleBar_TopMargin = 2,
        TitleBar_BottomMargin = 2,
        TitleBar_SideMargin = 2,
        TitleBar_ButtonSpacing = 2,

        // shadow dimensions (pixels)
        Shadow_Size = 16,
        Shadow_Offset = 6,
        Shadow_Overlap = 2,

        /** the extra edge
        needed to outline active window title
        */
        TitleBar_OutlineMargin = 4

    };

    //* exception
    enum ExceptionMask
    {
        None = 0,
        BorderSize = 1<<4
    };
}

#endif
