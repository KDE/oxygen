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
#include "oxygenactiveshadowconfiguration.h"
#include "oxygeninactiveshadowconfiguration.h"

#include <cmath>
#include <KColorUtils>
#include <QPainter>
#include <QTextStream>

namespace Oxygen
{

    //_______________________________________________________
    ShadowCache::ShadowCache( Helper& helper ):
        _helper( helper ),
        _activeShadowSize( 40 ),
        _inactiveShadowSize( 40 )
    {

        setEnabled( true );
        setMaxIndex( 256 );

    }

    //_______________________________________________________
    void ShadowCache::readConfig( void )
    {

        if( !_enabled ) setEnabled( true );

        // shadows
        #if OXYGEN_USE_KDE4
        ActiveShadowConfiguration::self()->readConfig();
        InactiveShadowConfiguration::self()->readConfig();
        #else
        ActiveShadowConfiguration::self()->load();
        InactiveShadowConfiguration::self()->load();
        #endif

        // copy sizes to local
        _activeShadowSize = ActiveShadowConfiguration::shadowSize();
        _inactiveShadowSize = InactiveShadowConfiguration::shadowSize();

        // invalidate caches
        invalidateCaches();

        // for now, always return true (meaning that config has changed)
        return;

    }

    //_______________________________________________________
    void ShadowCache::setAnimationsDuration( int value )
    {
        setMaxIndex( qMin( 256, int( (120*value)/1000 ) ) );
        invalidateCaches();
    }

    //_______________________________________________________
    bool ShadowCache::isEnabled( QPalette::ColorGroup group ) const
    {
        if( group == QPalette::Active ) return ActiveShadowConfiguration::enabled();
        else if( group == QPalette::Inactive ) return InactiveShadowConfiguration::enabled();
        else return false;
    }

    //_______________________________________________________
    void ShadowCache::setShadowSize( QPalette::ColorGroup group, int size )
    {

        if( group == QPalette::Active && _activeShadowSize != size )
        {
            _activeShadowSize = size;
            invalidateCaches();

        } else if( group == QPalette::Inactive && _inactiveShadowSize != size ) {

            _inactiveShadowSize = size;
            invalidateCaches();

        }

    }

    //_______________________________________________________
    int ShadowCache::shadowSize( void ) const
    {
        int activeSize( ActiveShadowConfiguration::enabled() ? _activeShadowSize:0 );
        int inactiveSize( InactiveShadowConfiguration::enabled() ? _inactiveShadowSize:0 );

        // even if shadows are disabled,
        return qMax( activeSize, inactiveSize );
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
        Q_ASSERT( index <= _maxIndex );

        // construct key
        key.index = index;

        // check if tileSet already in cache
        int hash( key.hash() );
        if( _enabled && _animatedShadowCache.contains(hash) ) return _animatedShadowCache.object(hash);

        // create shadow and tileset otherwise
        qreal size( shadowSize() + overlap );

        QPixmap shadow( _helper.highDpiPixmap( size*2 ) );
        // QPixmap shadow( size*2, size*2 );
        shadow.fill( Qt::transparent );
        QPainter painter( &shadow );
        painter.setRenderHint( QPainter::Antialiasing );

        QPixmap inactiveShadow( pixmap( key, false ) );
        if( !inactiveShadow.isNull() )
        {
            QPainter local( &inactiveShadow );
            local.setRenderHint( QPainter::Antialiasing );
            local.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            local.fillRect( inactiveShadow.rect(), QColor( 0, 0, 0, 255*(1.0-opacity ) ) );
        }

        QPixmap activeShadow( pixmap( key, true ) );
        if( !activeShadow.isNull() )
        {
            QPainter local( &activeShadow );
            local.setRenderHint( QPainter::Antialiasing );
            local.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            local.fillRect( activeShadow.rect(), QColor( 0, 0, 0, 255*( opacity ) ) );
        }

        painter.drawPixmap( QPointF(0,0), inactiveShadow );
        painter.drawPixmap( QPointF(0,0), activeShadow );
        painter.end();

        TileSet* tileSet = new TileSet( shadow, size, size, 1, 1);
        _animatedShadowCache.insert( hash, tileSet );
        return tileSet;

    }

    //_______________________________________________________
    QPixmap ShadowCache::pixmap( const Key& key, bool active ) const
    {

        static const qreal fixedSize = 25.5;
        qreal size( shadowSize() );
        qreal shadowSize( 0 );

        if( active && ActiveShadowConfiguration::enabled() ) shadowSize = _activeShadowSize;
        else if( !active && InactiveShadowConfiguration::enabled() ) shadowSize = _inactiveShadowSize;

        if( !shadowSize ) return QPixmap();

        // add overlap
        size += overlap;
        shadowSize += overlap;

        QPixmap shadow( _helper.highDpiPixmap( size*2 ) );
        // QPixmap shadow( size*2, size*2 );
        shadow.fill( Qt::transparent );

        QPainter painter( &shadow );
        painter.setRenderHint( QPainter::Antialiasing );
        painter.setPen( Qt::NoPen );

        // some gradients rendering are different at bottom corners if client has no border
        bool hasBorder( key.hasBorder || key.isShade );

        if( active )
        {

            {

                // inner (sharp) gradient
                const qreal gradientSize = qMin( shadowSize, (shadowSize+fixedSize)/2 );
                const qreal voffset = qMin( 12.0*(gradientSize*ActiveShadowConfiguration::verticalOffset())/fixedSize, 4.0 );

                QRadialGradient radialGradient = QRadialGradient( size, size + voffset, gradientSize );
                radialGradient.setColorAt(1, Qt::transparent );

                // gaussian shadow is used
                int nPoints( (10*gradientSize)/fixedSize );
                Gaussian f( 0.85, 0.17 );
                QColor c = ActiveShadowConfiguration::innerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    radialGradient.setColorAt( x, c );

                }

                painter.setBrush( radialGradient );
                renderGradient( painter, shadow.rect(), radialGradient, hasBorder );

            }

            {

                // outer (spread) gradient
                const qreal gradientSize = shadowSize;
                const qreal voffset = qMin( 12.0*(gradientSize*ActiveShadowConfiguration::verticalOffset())/fixedSize, 4.0 );

                QRadialGradient radialGradient = QRadialGradient( size, size+voffset, gradientSize );
                radialGradient.setColorAt(1, Qt::transparent );

                // gaussian shadow is used
                int nPoints( (10*gradientSize)/fixedSize );
                Gaussian f( 0.46, 0.34 );
                QColor c = ActiveShadowConfiguration::useOuterColor() ? ActiveShadowConfiguration::outerColor():ActiveShadowConfiguration::innerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    radialGradient.setColorAt( x, c );

                }

                painter.setBrush( radialGradient );
                painter.drawRect( shadow.rect() );

            }

        } else {

            {
                // inner (sharp gradient)
                const qreal gradientSize = qMin( shadowSize, fixedSize );
                const qreal voffset( 0.2 );

                QRadialGradient radialGradient = QRadialGradient( size, size+voffset, gradientSize );
                radialGradient.setColorAt(1, Qt::transparent );

                // parabolic shadow is used
                int nPoints( (10*gradientSize)/fixedSize );
                Parabolic f( 1.0, 0.22 );
                QColor c = InactiveShadowConfiguration::useOuterColor() ? InactiveShadowConfiguration::outerColor():InactiveShadowConfiguration::innerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    radialGradient.setColorAt( x, c );

                }


                painter.setBrush( radialGradient );
                renderGradient( painter, shadow.rect(), radialGradient, hasBorder );

            }

            {

                // mid gradient
                const qreal gradientSize = qMin( shadowSize, (shadowSize+2*fixedSize)/3 );
                const qreal voffset = qMin( 8.0*(gradientSize*InactiveShadowConfiguration::verticalOffset())/fixedSize, 4.0 );

                // gaussian shadow is used
                QRadialGradient radialGradient = QRadialGradient( size, size+voffset, gradientSize );
                radialGradient.setColorAt(1, Qt::transparent );

                int nPoints( (10*gradientSize)/fixedSize );
                Gaussian f( 0.54, 0.21);
                QColor c = InactiveShadowConfiguration::useOuterColor() ? InactiveShadowConfiguration::outerColor():InactiveShadowConfiguration::innerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    radialGradient.setColorAt( x, c );

                }

                painter.setBrush( radialGradient );
                painter.drawRect( shadow.rect() );

            }

            {

                // outer (spread) gradient
                const qreal gradientSize = shadowSize;
                const qreal voffset = qMin( 20.0*(gradientSize*InactiveShadowConfiguration::verticalOffset())/fixedSize, 4.0 );

                // gaussian shadow is used
                QRadialGradient radialGradient = QRadialGradient( size, size+voffset, gradientSize );
                radialGradient.setColorAt(1, Qt::transparent );

                int nPoints( (20*gradientSize)/fixedSize );
                Gaussian f( 0.155, 0.445);
                QColor c = InactiveShadowConfiguration::useOuterColor() ? InactiveShadowConfiguration::outerColor():InactiveShadowConfiguration::innerColor();
                for( int i = 0; i < nPoints; i++ )
                {
                    qreal x = qreal(i)/nPoints;
                    c.setAlphaF( f(x) );
                    radialGradient.setColorAt( x, c );
                }

                painter.setBrush( radialGradient );
                painter.drawRect( shadow.rect() );

            }

        }

        // mask
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.setBrush( Qt::black );
        painter.drawEllipse( QRectF( size-3, size-3, 6, 6 ) );

        painter.end();
        return shadow;

    }

    //_______________________________________________________
    void ShadowCache::renderGradient( QPainter& painter, const QRectF& rect, const QRadialGradient& radialGradient, bool hasBorder ) const
    {

        if( hasBorder )
        {
            painter.setBrush( radialGradient );
            painter.drawRect( rect );
            return;
        }

        const qreal size( rect.width()/2.0 );
        const qreal hoffset( radialGradient.center().x() - size );
        const qreal voffset( radialGradient.center().y() - size );
        const qreal radius( radialGradient.radius() );

        // load gradient stops
        QGradientStops stops( radialGradient.stops() );

        // draw ellipse for the upper rect
        {
            QRectF rect( hoffset, voffset, 2*size-hoffset, size );
            painter.setBrush( radialGradient );
            painter.drawRect( rect );
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

            painter.setBrush( lg );
            painter.drawRect( rect );

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

            painter.setBrush( lg );
            painter.drawRect( rect );
        }

        {

            // bottom-left corner
            const QRectF rect( hoffset, size+voffset+4, size-4, size );
            QRadialGradient radialGradient = QRadialGradient( size+hoffset-4, size+voffset+4, radius );
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

                radialGradient.setColorAt( xx, c );
            }

            painter.setBrush( radialGradient );
            painter.drawRect( rect );

        }

        {
            // bottom-right corner
            const QRectF rect( size+hoffset+4, size+voffset+4, size-4, size );
            QRadialGradient radialGradient = QRadialGradient( size+hoffset+4, size+voffset+4, radius );
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

                radialGradient.setColorAt( xx, c );
            }

            painter.setBrush( radialGradient );
            painter.drawRect( rect );

        }

    }

}
