/*
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "oxygenstyleplugin.h"
#include "oxygenstyle.h"

#include <QApplication>

namespace Oxygen
{

    //_________________________________________________
    QStyle* StylePlugin::create( const QString &key )
    {
        if( key.toLower() == QStringLiteral( "oxygen" ) )
        {
            return new Style;
        }
        return nullptr;
    }

    //_________________________________________________
    StylePlugin::~StylePlugin()
    {
    }

    //_________________________________________________
    QStringList StylePlugin::keys() const
    { return QStringList( QStringLiteral( "Oxygen" ) ); }

}
