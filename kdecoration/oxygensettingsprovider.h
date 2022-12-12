#ifndef oxygensettingsprovider_h
#define oxygensettingsprovider_h
/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "oxygen.h"
#include "oxygendecoration.h"

#include <KSharedConfig>
#include <QObject>

namespace Oxygen
{

class DecoHelper;
class ShadowCache;

class SettingsProvider : public QObject
{
    Q_OBJECT

public:
    //* destructor
    ~SettingsProvider();

    //* singleton
    static SettingsProvider *self();

    //* helper
    DecoHelper *helper(void) const
    {
        return m_decoHelper;
    }

    //* shadow cache
    ShadowCache *shadowCache(void) const
    {
        return m_shadowCache;
    }

    //* internal settings for given decoration
    InternalSettingsPtr internalSettings(const Decoration *) const;

public Q_SLOTS:

    //* reconfigure
    void reconfigure(void);

private:
    //* contructor
    SettingsProvider(void);

    //* default configuration
    InternalSettingsPtr m_defaultSettings;

    //* exceptions
    InternalSettingsList m_exceptions;

    //* config object
    KSharedConfigPtr m_config;

    //* decoration helper
    DecoHelper *m_decoHelper = nullptr;

    //* shadow cache
    ShadowCache *m_shadowCache = nullptr;

    //* singleton
    static SettingsProvider *s_self;
};
}

#endif
