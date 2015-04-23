#ifndef oxygensettingsprovider_h
#define oxygensettingsprovider_h
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

#include "oxygendecoration.h"
#include "oxygen.h"

#include <KSharedConfig>
#include <QObject>

namespace Oxygen
{

    class DecoHelper;
    class ShadowCache;

    class SettingsProvider: public QObject
    {

        Q_OBJECT

        public:

        //* destructor
        ~SettingsProvider();

        //* singleton
        static SettingsProvider *self();

        //* helper
        DecoHelper* helper( void ) const
        { return m_decoHelper; }

        //* shadow cache
        ShadowCache* shadowCache( void ) const
        { return m_shadowCache; }

        //* internal settings for given decoration
        InternalSettingsPtr internalSettings(const Decoration *) const;

        public Q_SLOTS:

        //* reconfigure
        void reconfigure( void );

        private:

        //* contructor
        SettingsProvider( void );

        //* default configuration
        InternalSettingsPtr m_defaultSettings;

        //* exceptions
        InternalSettingsList m_exceptions;

        //* config object
        KSharedConfigPtr m_config;

        //* decoration helper
        DecoHelper* m_decoHelper = nullptr;

        //* shadow cache
        ShadowCache* m_shadowCache = nullptr;

        //* singleton
        static SettingsProvider *s_self;

    };

}

#endif
