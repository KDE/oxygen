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

#ifndef OXYGEN_EXPORT_H
#define OXYGEN_EXPORT_H

#ifdef OXYGENSTYLE5_STATIC_DEFINE
#  define OXYGEN_EXPORT
#  define OXYGENSTYLE5_NO_EXPORT
#else
#  ifndef OXYGEN_EXPORT
#    ifdef oxygenstyle5_EXPORTS
        /* We are building this library */
#      define OXYGEN_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define OXYGEN_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef OXYGENSTYLE5_NO_EXPORT
#    define OXYGENSTYLE5_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef OXYGENSTYLE5_DEPRECATED
#  define OXYGENSTYLE5_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef OXYGENSTYLE5_DEPRECATED_EXPORT
#  define OXYGENSTYLE5_DEPRECATED_EXPORT OXYGEN_EXPORT OXYGENSTYLE5_DEPRECATED
#endif

#ifndef OXYGENSTYLE5_DEPRECATED_NO_EXPORT
#  define OXYGENSTYLE5_DEPRECATED_NO_EXPORT OXYGENSTYLE5_NO_EXPORT OXYGENSTYLE5_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define OXYGENSTYLE5_NO_DEPRECATED
#endif

#endif
