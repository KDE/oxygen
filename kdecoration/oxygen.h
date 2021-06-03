#ifndef oxygen_h
#define oxygen_h

/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
