//////////////////////////////////////////////////////////////////////////////
// oxygenshadowcache.cpp
// handles caching of TileSet objects to draw shadows
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenshadowcache.h"

#include <cassert>
#include <cmath>
#include <KColorUtils>
#include <KConfigGroup>
#include <QtGui/QPainter>
#include <QtCore/QTextStream>

namespace Oxygen
{

    //_______________________________________________________
    qreal sqr( qreal x )
    { return x*x; }

    //_______________________________________________________
    ShadowCache::ShadowCache( Helper& helper ):
        _helper( helper ),
        _activeShadowConfiguration( ShadowConfiguration( QPalette::Active ) ),
        _inactiveShadowConfiguration( ShadowConfiguration( QPalette::Inactive ) )
    {

        setEnabled( true );
        setMaxIndex( 256 );

    }

    //_______________________________________________________
    bool ShadowCache::readConfig( const KConfig& config )
    {

        bool changed( false );

        // initialize shadowCacheMode
        const KConfigGroup group( config.group("Windeco") );
        const QString shadowCacheMode( group.readEntry( OxygenConfig::SHADOW_CACHE_MODE, "Variable" ) );

        if( shadowCacheMode == "Disabled" )
        {

            if( _enabled )
            {
                setEnabled( false );
                changed = true;
            }

        } else if( shadowCacheMode == "Maximum" ) {

            if( !_enabled )
            {
                setEnabled( true );
                changed = true;
            }

            if( _maxIndex != 256 )
            {
                setMaxIndex( 256 );
                changed = true;
            }

        } else {

            if( !_enabled )
            {
                setEnabled( true );
                changed = true;
            }

            // get animation duration
            const int duration( group.readEntry( OxygenConfig::ANIMATIONS_DURATION, 150 ) );
            const int maxIndex( qMin( 256, int( (120*duration)/1000 ) ) );
            if( _maxIndex != maxIndex )
            {
                setMaxIndex( maxIndex );
                changed = true;
            }

        }

        // active shadows
        ShadowConfiguration activeShadowConfiguration( QPalette::Active, config.group( "ActiveShadow" ) );
        activeShadowConfiguration.setEnabled( group.readEntry( OxygenConfig::USE_OXYGEN_SHADOWS, true ) );
        if( shadowConfigurationChanged( activeShadowConfiguration ) )
        {
            setShadowConfiguration( activeShadowConfiguration );
            changed = true;
        }

        // inactive shadows
        ShadowConfiguration inactiveShadowConfiguration( QPalette::Inactive, config.group( "InactiveShadow" ) );
        inactiveShadowConfiguration.setEnabled( group.readEntry( OxygenConfig::USE_DROP_SHADOWS, true ) );
        if( shadowConfigurationChanged( inactiveShadowConfiguration ) )
        {
            setShadowConfiguration( inactiveShadowConfiguration );
            changed = true;
        }

        if( changed ) invalidateCaches();
        return changed;

    }

    //_______________________________________________________
    bool ShadowCache::shadowConfigurationChanged( const ShadowConfiguration& other ) const
    {
        const ShadowConfiguration& local = (other.colorGroup() == QPalette::Active ) ? _activeShadowConfiguration:_inactiveShadowConfiguration;
        return !(local == other);
    }

    //_______________________________________________________
    void ShadowCache::setShadowConfiguration( const ShadowConfiguration& other )
    {
        ShadowConfiguration& local = (other.colorGroup() == QPalette::Active ) ? _activeShadowConfiguration:_inactiveShadowConfiguration;
        local = other;
    }

    //_______________________________________________________
    TileSet* ShadowCache::tileSet( const Key& key )
    {

        // check if tileSet already in cache
        int hash( key.hash() );
        if( _enabled && _shadowCache.contains(hash) ) return _shadowCache.object(hash);

        // create tileSet otherwise
        qreal size( shadowSize() + overlap );
        TileSet* tileSet = new TileSet( pixmap( key, key.active ), size, size, size, size, size, size, 1, 1);
        _shadowCache.insert( hash, tileSet );

        return tileSet;

    }

    //_______________________________________________________
    TileSet* ShadowCache::tileSet( Key key, qreal opacity )
    {

        int index( opacity*_maxIndex );
        assert( index <= _maxIndex );

        // construct key
        key.index = index;

        // check if tileSet already in cache
        int hash( key.hash() );
        if( _enabled && _animatedShadowCache.contains(hash) ) return _animatedShadowCache.object(hash);

        // create shadow and tileset otherwise
        qreal size( shadowSize() + overlap );

        QPixmap shadow( size*2, size*2 );
        shadow.fill( Qt::transparent );
        QPainter p( &shadow );
        p.setRenderHint( QPainter::Antialiasing );

        QPixmap inactiveShadow( pixmap( key, false ) );
        {
            QPainter pp( &inactiveShadow );
            pp.setRenderHint( QPainter::Antialiasing );
            pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            pp.fillRect( inactiveShadow.rect(), QColor( 0, 0, 0, 255*(1.0-opacity ) ) );
        }

        QPixmap activeShadow( pixmap( key, true ) );
        {
            QPainter pp( &activeShadow );
            pp.setRenderHint( QPainter::Antialiasing );
            pp.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            pp.fillRect( activeShadow.rect(), QColor( 0, 0, 0, 255*( opacity ) ) );
        }

        p.drawPixmap( QPointF(0,0), inactiveShadow );
        p.drawPixmap( QPointF(0,0), activeShadow );
        p.end();

        TileSet* tileSet = new TileSet(shadow, size, size, 1, 1);
        _animatedShadowCache.insert( hash, tileSet );
        return tileSet;

    }

    //_______________________________________________________
    QPixmap ShadowCache::pixmap( const Key& key, bool active ) const
    {

        // local reference to relevant shadow configuration
        const ShadowConfiguration& shadowConfiguration(
            active ? _activeShadowConfiguration:_inactiveShadowConfiguration );

        static const qreal fixedSize = 25.5;
        qreal size( shadowSize() );
        qreal shadowSize( shadowConfiguration.isEnabled() ? shadowConfiguration.shadowSize():0 );

        if( !shadowSize ) return QPixmap();

        // add overlap
        size += overlap;
        shadowSize += overlap;

        QPixmap shadow = QPixmap( size*2, size*2 );
        shadow.fill( Qt::transparent );

        QPainter p( &shadow );
        p.setRenderHint( QPainter::Antialiasing );
        p.setPen( Qt::NoPen );

        // some gradients rendering are different at bottom corners if client has no border
        bool hasBorder( key.hasBorder || key.isShade );

        if( active )
        {

            {

                // inner (sharp) gradient
                const qreal gradientSize = qMin( shadowSize, (shadowSize+fixedSize)/2 );
                const qreal hoffset( 0 );
                const qreal voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                QRadialGradient rg = QRadialGradient( size+12.0*hoffset, size+12.0*voffset, gradientSize );
                rg.setColorAt(1, Qt::transparent );

                // gaussian shadow is used
                int nPoints( (10*gradientSize)/fixedSize );
                Gaussian f( 0.85, 0.17 );
                QColor c = shadowConfiguration.innerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    rg.setColorAt( x, c );

                }

                p.setBrush( rg );
                renderGradient( p, shadow.rect(), rg, hasBorder );

            }

            {

                // outer (spread) gradient
                const qreal gradientSize = shadowSize;
                const qreal hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                const qreal voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                QRadialGradient rg = QRadialGradient( size+12.0*hoffset, size+12.0*voffset, gradientSize );
                rg.setColorAt(1, Qt::transparent );

                // gaussian shadow is used
                int nPoints( (10*gradientSize)/fixedSize );
                Gaussian f( 0.46, 0.34 );
                QColor c = shadowConfiguration.outerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    rg.setColorAt( x, c );

                }

                p.setBrush( rg );
                p.drawRect( shadow.rect() );

            }

        } else {

            {
                // inner (sharp gradient)
                const qreal gradientSize = qMin( shadowSize, fixedSize );
                const qreal hoffset( 0 );
                const qreal voffset( 0.2 );

                QRadialGradient rg = QRadialGradient( size+hoffset, size+voffset, gradientSize );
                rg.setColorAt(1, Qt::transparent );

                // parabolic shadow is used
                int nPoints( (10*gradientSize)/fixedSize );
                Parabolic f( 1.0, 0.22 );
                QColor c = shadowConfiguration.outerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    rg.setColorAt( x, c );

                }


                p.setBrush( rg );
                renderGradient( p, shadow.rect(), rg, hasBorder );

            }

            {

                // mid gradient
                const qreal gradientSize = qMin( shadowSize, (shadowSize+2*fixedSize)/3 );
                const qreal hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                const qreal voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                // gaussian shadow is used
                QRadialGradient rg = QRadialGradient( size+8.0*hoffset, size+8.0*voffset, gradientSize );
                rg.setColorAt(1, Qt::transparent );

                int nPoints( (10*gradientSize)/fixedSize );
                Gaussian f( 0.54, 0.21);
                QColor c = shadowConfiguration.outerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    rg.setColorAt( x, c );

                }

                p.setBrush( rg );
                p.drawRect( shadow.rect() );

            }

            {

                // outer (spread) gradient
                const qreal gradientSize = shadowSize;
                const qreal hoffset = shadowConfiguration.horizontalOffset()*gradientSize/fixedSize;
                const qreal voffset = shadowConfiguration.verticalOffset()*gradientSize/fixedSize;

                // gaussian shadow is used
                QRadialGradient rg = QRadialGradient( size+20.0*hoffset, size+20.0*voffset, gradientSize );
                rg.setColorAt(1, Qt::transparent );

                int nPoints( (20*gradientSize)/fixedSize );
                Gaussian f( 0.155, 0.445);
                QColor c = shadowConfiguration.outerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    rg.setColorAt( x, c );
                }

                p.setBrush( rg );
                p.drawRect( shadow.rect() );

            }

        }

        // mask
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.setBrush( Qt::black );
        p.drawEllipse( QRectF( size-3, size-3, 6, 6 ) );

        p.end();
        return shadow;

    }

    //_______________________________________________________
    void ShadowCache::renderGradient( QPainter& p, const QRectF& rect, const QRadialGradient& rg, bool hasBorder ) const
    {

        if( hasBorder )
        {
            p.setBrush( rg );
            p.drawRect( rect );
            return;
        }

        const qreal size( rect.width()/2.0 );
        const qreal hoffset( rg.center().x() - size );
        const qreal voffset( rg.center().y() - size );
        const qreal radius( rg.radius() );

        // load gradient stops
        QGradientStops stops( rg.stops() );

        // draw ellipse for the upper rect
        {
            QRectF rect( hoffset, voffset, 2*size-hoffset, size );
            p.setBrush( rg );
            p.drawRect( rect );
        }

        // draw square gradients for the lower rect
        {
            // vertical lines
            const QRectF rect( hoffset, size+voffset, 2*size-hoffset, 4 );
            QLinearGradient lg( hoffset, 0.0, 2*size+hoffset, 0.0 );
            for( int i = 0; i<stops.size(); i++ )
            {
                const QColor c( stops[i].second );
                const qreal xx( stops[i].first*radius );
                lg.setColorAt( (size-xx)/(2.0*size), c );
                lg.setColorAt( (size+xx)/(2.0*size), c );
            }

            p.setBrush( lg );
            p.drawRect( rect );

        }

        {
            // horizontal line
            const QRectF rect( size-4+hoffset, size+voffset, 8, size );
            QLinearGradient lg = QLinearGradient( 0, voffset, 0, 2*size+voffset );
            for( int i = 0; i<stops.size(); i++ )
            {
                const QColor c( stops[i].second );
                const qreal xx( stops[i].first*radius );
                lg.setColorAt( (size+xx)/(2.0*size), c );
            }

            p.setBrush( lg );
            p.drawRect( rect );
        }

        {

            // bottom-left corner
            const QRectF rect( hoffset, size+voffset+4, size-4, size );
            QRadialGradient rg = QRadialGradient( size+hoffset-4, size+voffset+4, radius );
            for( int i = 0; i<stops.size(); i++ )
            {
                QColor c( stops[i].second );
                qreal xx( stops[i].first -4.0/radius );
                if( xx<0 )
                {
                    if( i < stops.size()-1 )
                    {
                        const qreal x1( stops[i+1].first -4.0/radius );
                        c = KColorUtils::mix( c, stops[i+1].second, -xx/(x1-xx) );
                    }
                    xx = 0;
                }

                rg.setColorAt( xx, c );
            }

            p.setBrush( rg );
            p.drawRect( rect );

        }

        {
            // bottom-right corner
            const QRectF rect( size+hoffset+4, size+voffset+4, size-4, size );
            QRadialGradient rg = QRadialGradient( size+hoffset+4, size+voffset+4, radius );
            for( int i = 0; i<stops.size(); i++ )
            {
                QColor c( stops[i].second );
                qreal xx( stops[i].first -4.0/radius );
                if( xx<0 )
                {
                    if( i < stops.size()-1 )
                    {
                        const qreal x1( stops[i+1].first -4.0/radius );
                        c = KColorUtils::mix( c, stops[i+1].second, -xx/(x1-xx) );
                    }
                    xx = 0;
                }

                rg.setColorAt( xx, c );
            }

            p.setBrush( rg );
            p.drawRect( rect );

        }

    }

}
