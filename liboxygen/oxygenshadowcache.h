#ifndef oxygen_shadowCacheh
#define oxygen_shadowCacheh

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowcache.h
// handles caching of TileSet objects to draw shadows
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenhelper.h"
#include "oxygen_export.h"

#include <QCache>
#include <QRadialGradient>
#include <cmath>

namespace Oxygen
{

    class OXYGEN_EXPORT ShadowCache
    {
        public:

        //* constructor
        explicit ShadowCache( Helper& );

        //* read configuration
        void readConfig( void );

        //* animations duration
        void setAnimationsDuration( int );

        //* cache size
        void setEnabled( bool enabled )
        {
            _enabled = enabled;
            if( enabled )
            {

                _shadowCache.setMaxCost( 1<<6 );
                _animatedShadowCache.setMaxCost( _maxIndex<<6 );

            } else {

                _shadowCache.setMaxCost( 1 );
                _animatedShadowCache.setMaxCost( 1 );

            }
        }

        //* max animation index
        int maxIndex( void ) const
        { return _maxIndex; }

        //* max animation index
        void setMaxIndex( int value )
        {
            _maxIndex = value;
            if( _enabled )
            {

                _shadowCache.setMaxCost( 1<<6 );
                _animatedShadowCache.setMaxCost( _maxIndex<<6 );

            }

        }

        //* invalidate caches
        void invalidateCaches( void )
        {
            _shadowCache.clear();
            _animatedShadowCache.clear();
        }

        //* true if shadow is enabled for a given group
        bool isEnabled( QPalette::ColorGroup ) const;

        //* set shadow size manually
        void setShadowSize( QPalette::ColorGroup, int );

        //* shadow size
        int shadowSize( void ) const;

        //* Key class to be used into QCache
        /*! class is entirely inline for optimization */
        class Key
        {

            public:

            //* explicit constructor
            explicit Key( void )
            {}

            //* constructor from int
            explicit Key( int hash ):
                index( hash >> 3 ),
                active( ( hash >> 2 )&1 ),
                isShade( ( hash >> 1)&1 ),
                hasBorder( (hash)&1 )
            {}

            //* hash function
            int hash( void ) const
            {
                return
                    ( index << 3 ) |
                    ( active << 2 ) |
                    ( isShade<< 1 ) |
                    ( hasBorder );
            }

            int index = 0;
            bool active = false;
            bool isShade = false;
            bool hasBorder = true;

        };

        //* get shadow matching client
        TileSet tileSet( const Key& );

        //* get shadow matching client and opacity
        TileSet tileSet( Key, qreal );

        //* simple pixmap
        QPixmap pixmap( const Key& key ) const
        { return pixmap( key, key.active ); }

        //* simple pixmap, with opacity
        QPixmap animatedPixmap( const Key&, qreal opacity );

        private:

        Helper& helper( void ) const
        { return _helper; }

        //* simple pixmap
        QPixmap pixmap( const Key&, bool active ) const;

        //* draw gradient into rect
        /*! a separate method is used in order to properly account for corners */
        void renderGradient( QPainter&, const QRectF&, const QRadialGradient&, bool hasBorder = true ) const;

        //* helper
        Helper& _helper;

        //* defines overlap between shadows and body
        enum { overlap = 4 };

        //* caching enable state
        bool _enabled;

        //* shadow size
        int _activeShadowSize;

        //* shadow size
        int _inactiveShadowSize;

        //* max index
        /*! it is used to set caches max cost, and calculate animation opacity */
        int _maxIndex;

        //* cache
        using TileSetCache = QCache<int, TileSet>;

        //* shadow cache
        TileSetCache _shadowCache;

        //* animated shadow cache
        TileSetCache _animatedShadowCache;

    };

}

#endif
