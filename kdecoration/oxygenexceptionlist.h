#ifndef oxygenexceptionlist_h
#define oxygenexceptionlist_h

//////////////////////////////////////////////////////////////////////////////
// oxygenexceptionlist.h
// window decoration exceptions
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygensettings.h"
#include "oxygen.h"

#include <KSharedConfig>

namespace Oxygen
{

    //* oxygen exceptions list
    class ExceptionList
    {

        public:

        //* constructor from list
        explicit ExceptionList( const InternalSettingsList& exceptions = InternalSettingsList() ):
            _exceptions( exceptions )
        {}

        //* exceptions
        const InternalSettingsList& get( void ) const
        { return _exceptions; }

        //* read from KConfig
        void readConfig( KSharedConfig::Ptr );

        //* write to kconfig
        void writeConfig( KSharedConfig::Ptr );

        private:

        //* generate exception group name for given exception index
        static QString exceptionGroupName( int index );

        //* read configuration
        static void readConfig( KCoreConfigSkeleton*, KConfig*, const QString& );

        //* write configuration
        static void writeConfig( KCoreConfigSkeleton*, KConfig*, const QString& );

        //* exceptions
        InternalSettingsList _exceptions;

    };

}

#endif
