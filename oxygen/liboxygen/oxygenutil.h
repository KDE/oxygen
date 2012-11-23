#ifndef oxygenutil_h
#define oxygenutil_h

/*
 * Copyright 2012 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "oxygen_export.h"

#include <kconfigskeleton.h>
#include <KConfig>

namespace Oxygen
{

    class OXYGEN_EXPORT Util
    {

        public:

        //! write configuration
        static void writeConfig( KCoreConfigSkeleton*, KConfig*, const QString& = QString() );

        //! read configuration
        static void readConfig( KCoreConfigSkeleton*, KConfig*, const QString& = QString() );

    };

}

#endif
