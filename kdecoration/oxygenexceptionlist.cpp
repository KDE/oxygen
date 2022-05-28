//////////////////////////////////////////////////////////////////////////////
// oxygenexceptionlist.cpp
// window decoration exceptions
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenexceptionlist.h"


namespace Oxygen
{

    //______________________________________________________________
    void ExceptionList::readConfig( KSharedConfig::Ptr config )
    {

        _exceptions.clear();

        QString groupName;
        for( int index = 0; config->hasGroup( groupName = exceptionGroupName( index ) ); ++index )
        {

            // create exception
            InternalSettings exception;

            // reset group
            readConfig( &exception, config.data(), groupName );

            // create new configuration
            InternalSettingsPtr configuration( new InternalSettings() );
            configuration.data()->load();

            // apply changes from exception
            configuration->setEnabled( exception.enabled() );
            configuration->setExceptionType( exception.exceptionType() );
            configuration->setExceptionPattern( exception.exceptionPattern() );
            configuration->setMask( exception.mask() );

            // propagate all features found in mask to the output configuration
            if( exception.mask() & BorderSize ) configuration->setBorderSize( exception.borderSize() );
            configuration->setHideTitleBar( exception.hideTitleBar() );

            // append to exceptions
            _exceptions.append( configuration );

        }

    }

    //______________________________________________________________
    void ExceptionList::writeConfig( KSharedConfig::Ptr config )
    {

        // remove all existing exceptions
        QString groupName;
        for( int index = 0; config->hasGroup( groupName = exceptionGroupName( index ) ); ++index )
        { config->deleteGroup( groupName ); }

        // rewrite current exceptions
        int index = 0;
        for ( const InternalSettingsPtr &exception : std::as_const(_exceptions) )
        {

            writeConfig( exception.data(), config.data(), exceptionGroupName( index ) );
            ++index;

        }

    }

    //_______________________________________________________________________
    QString ExceptionList::exceptionGroupName( int index )
    { return QStringLiteral( "Windeco Exception %1" ).arg( index ); }

    //______________________________________________________________
    void ExceptionList::writeConfig( KCoreConfigSkeleton* skeleton, KConfig* config, const QString& groupName )
    {

        // list of items to be written
        static const QStringList keys = {
            QStringLiteral("Enabled"),
            QStringLiteral("ExceptionPattern"),
            QStringLiteral("ExceptionType"),
            QStringLiteral("HideTitleBar"),
            QStringLiteral("Mask"),
            QStringLiteral("BorderSize")
        };

        // write all items
        for ( const auto &key : keys )
        {
            KConfigSkeletonItem* item( skeleton->findItem( key ) );
            if( !item ) continue;

            if( !groupName.isEmpty() ) item->setGroup( groupName );
            KConfigGroup configGroup( config, item->group() );
            configGroup.writeEntry( item->key(), item->property() );

        }

    }

    //______________________________________________________________
    void ExceptionList::readConfig( KCoreConfigSkeleton* skeleton, KConfig* config, const QString& groupName )
    {

        const auto skelItems = skeleton->items();
        for ( KConfigSkeletonItem *item : skelItems )
        {
            if( !groupName.isEmpty() ) item->setGroup( groupName );
            item->readConfig( config );
        }

    }

}
