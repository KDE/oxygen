/*
 * Copyright 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenutil.h"

namespace Oxygen
{

    //______________________________________________________________
    void Util::writeConfig( KCoreConfigSkeleton* skeleton, KConfig* config, const QString& groupName )
    {

        // write all items
        foreach( KConfigSkeletonItem* item, skeleton->items() )
        {
            if( !groupName.isEmpty() ) item->setGroup( groupName );
            item->writeConfig( config );
        }

    }

    //______________________________________________________________
    void Util::readConfig( KCoreConfigSkeleton* skeleton, KConfig* config, const QString& groupName )
    {

        foreach( KConfigSkeletonItem* item, skeleton->items() )
        {
            if( !groupName.isEmpty() ) item->setGroup( groupName );
            item->readConfig( config );
        }

    }

}
