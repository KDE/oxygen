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

#ifndef OXYGEN_CONFIG_EXPORT_H
#define OXYGEN_CONFIG_EXPORT_H

#ifdef OXYGENSTYLECONFIG5_STATIC_DEFINE
#  define OXYGEN_CONFIG_EXPORT
#  define OXYGENSTYLECONFIG5_NO_EXPORT
#else
#  ifndef OXYGEN_CONFIG_EXPORT
#    ifdef oxygenstyleconfig5_EXPORTS
        /* We are building this library */
#      define OXYGEN_CONFIG_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define OXYGEN_CONFIG_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef OXYGENSTYLECONFIG5_NO_EXPORT
#    define OXYGENSTYLECONFIG5_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef OXYGENSTYLECONFIG5_DEPRECATED
#  define OXYGENSTYLECONFIG5_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef OXYGENSTYLECONFIG5_DEPRECATED_EXPORT
#  define OXYGENSTYLECONFIG5_DEPRECATED_EXPORT OXYGEN_CONFIG_EXPORT OXYGENSTYLECONFIG5_DEPRECATED
#endif

#ifndef OXYGENSTYLECONFIG5_DEPRECATED_NO_EXPORT
#  define OXYGENSTYLECONFIG5_DEPRECATED_NO_EXPORT OXYGENSTYLECONFIG5_NO_EXPORT OXYGENSTYLECONFIG5_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define OXYGENSTYLECONFIG5_NO_DEPRECATED
#endif

#endif
