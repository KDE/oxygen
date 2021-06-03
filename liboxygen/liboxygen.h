#ifndef liboxygen_h
#define liboxygen_h
/*
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QFlags>
#include <QWeakPointer>

namespace Oxygen
{

    //* scoped pointer convenience typedef
    template <typename T> using WeakPointer = QPointer<T>;

    //* corners
    enum Corner
    {
        CornerTopLeft = 0x1,
        CornerTopRight = 0x2,
        CornerBottomLeft = 0x4,
        CornerBottomRight = 0x8,
        CornersTop = CornerTopLeft|CornerTopRight,
        CornersBottom = CornerBottomLeft|CornerBottomRight,
        CornersLeft = CornerTopLeft|CornerBottomLeft,
        CornersRight = CornerTopRight|CornerBottomRight,
        AllCorners = CornerTopLeft|CornerTopRight|CornerBottomLeft|CornerBottomRight
    };

    Q_DECLARE_FLAGS( Corners, Corner );
}

Q_DECLARE_OPERATORS_FOR_FLAGS( Oxygen::Corners );

#endif
