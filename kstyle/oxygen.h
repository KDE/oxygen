#ifndef oxygen_h
#define oxygen_h

/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

#include <QFlags>
#include <QPointer>
#include <QScopedPointer>
#include <QWeakPointer>

namespace Oxygen
{

    //*@name convenience typedef
    //@{

    #if QT_VERSION >= 0x050000
    //* scoped pointer convenience typedef
    template <typename T> using WeakPointer = QPointer<T>;
    #else
    //* scoped pointer convenience typedef
    template <typename T> using WeakPointer = QWeakPointer<T>;
    #endif

    //* scoped pointer convenience typedef
    template <typename T> using ScopedPointer = QScopedPointer<T, QScopedPointerPodDeleter>;

    //* disable QStringLiteral for older Qt version
    #if QT_VERSION < 0x050000
    using QStringLiteral = QString;
    #endif

    //@}

}

#endif
