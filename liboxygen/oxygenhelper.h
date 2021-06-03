#ifndef oxygen_helper_h
#define oxygen_helper_h

/*
 * SPDX-FileCopyrightText: 2016 Michael Pyne <mpyne@kde.org>
 * SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * SPDX-FileCopyrightText: 2007 Casper Boemann <cbr@boemann.dk>
 * SPDX-FileCopyrightText: 2007 Fredrik H ?glund <fredrik@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
 */

#include "oxygentileset.h"
#include "config-liboxygen.h"

#include "liboxygen.h"

#include <KColorScheme>
#include <KSharedConfig>

#include <QBitmap>
#include <QCache>
#include <QColor>
#include <QPixmap>
#include <QQueue>
#include <QWidget>
#include <QPainterPath>
#include <QScopedPointer>

#if OXYGEN_HAVE_X11
#include <xcb/xcb.h>
#endif

namespace Oxygen
{

    template<typename T> class BaseCache: public QCache<quint64, T>
    {

        public:

        //* constructor
        BaseCache( int maxCost ):
            QCache<quint64, T>( maxCost ),
            _enabled( true )
        {}

        //* constructor
        explicit BaseCache( void ):
            _enabled( true )
            {}

        //* destructor
        ~BaseCache( void )
        {}

        //* enable
        void setEnabled( bool value )
        { _enabled = value; }

        //* enable state
        bool enabled( void ) const
        { return _enabled; }

        //* access
        T* object( const quint64& key )
        { return _enabled ? QCache<quint64, T>::object( key ) : 0; }

        //* max cost
        void setMaxCost( int cost )
        {
            if( cost <= 0 ) {

                QCache<quint64, T>::clear();
                QCache<quint64, T>::setMaxCost( 1 );
                setEnabled( false );

            } else {

                setEnabled( true );
                QCache<quint64, T>::setMaxCost( cost );

            }
        }

        private:

        //* enable flag
        bool _enabled;

    };

    /**
     * Holds up to a limited number of items keyed by quint64. If items must be
     * removed to fall within limit, removes those added the earliest.
     */
    template<typename T>
    class FIFOCache
    {
        using CachePair = QPair<quint64, T>;

        public:

        //* constructor
        FIFOCache(size_t _limit = 256) : m_limit(_limit)
        {}

        //* insert
        void insert(quint64 key, T value)
        {
            if (static_cast<size_t>(m_queue.size()) > m_limit)
            { (void) m_queue.dequeue(); }
            m_queue.enqueue( CachePair( key, value) );
        }

        //* find
        T find(quint64 key) const
        {
            for(const auto &item : m_queue)
            { if (item.first == key) return item.second; }
            return T();
        }

        //* for_each
        template<typename F>
        void for_each(F f)
        {
            for(auto &item : m_queue)
            { f(item.second); }
        }

        //* maxCost
        void setMaxCost( size_t max )
        {
            m_limit = max;
            while (static_cast<size_t>(m_queue.size()) > m_limit)
            { (void) m_queue.dequeue(); }
        }

        //* maxCost
        size_t maxCost() const
        { return m_limit; }

        //* clear
        void clear()
        { m_queue.clear(); }

        private:

        //* queue
        QQueue<CachePair> m_queue;

        //* max size
        size_t m_limit;
    };

    template<typename T> class Cache
    {

        public:

        //* constructor
        Cache()
        {}

        //* destructor
        ~Cache()
        {}

        using Value = QSharedPointer<BaseCache<T>>;

        //* return cache matching a given key
        Value get( const QColor& color )
        {
            const quint64 key = ( color.isValid() ? color.rgba():0 );

            Value retValue = data_.find( key );
            if ( !retValue )
            {
                retValue = Value( new BaseCache<T>( data_.maxCost() ) );
                data_.insert( key, retValue );
            }

            return retValue;
        }

        //* clear
        void clear( void )
        { data_.clear(); }

        //* max cache size
        void setMaxCacheSize( int value )
        {
            data_.setMaxCost( value );
            data_.for_each( [value] (Value item) { item->setMaxCost( value );} );
        }

        private:

        //* data
        FIFOCache<Value> data_;

    };

    //* oxygen style helper class.
    /** contains utility functions used at multiple places in both oxygen style and oxygen window decoration */
    class OXYGEN_EXPORT Helper
    {
        public:

        //* constructor
        explicit Helper( KSharedConfig::Ptr config );

        //* destructor
        virtual ~Helper()
        {}

        //* load configuration
        virtual void loadConfig();

        //* pointer to shared config
        KSharedConfig::Ptr config() const;

        //* reset all caches
        virtual void invalidateCaches();

        //* update maximum cache size
        virtual void setMaxCacheSize( int );

        //*@name window background gradients
        //@{
        /**
        \par y_shift: shift the background gradient upwards, to fit with the windec
        \par gradientHeight: the height of the generated gradient.
        for different heights, the gradient is translated so that it is always at the same position from the bottom
        */
        virtual void renderWindowBackground( QPainter* p, const QRect& clipRect, const QWidget* widget, const QPalette&  pal, int y_shift=-23 )
        { renderWindowBackground( p, clipRect, widget, pal.color( widget->window()->backgroundRole() ), y_shift ); }

        /**
        y_shift: shift the background gradient upwards, to fit with the windec
        gradientHeight: the height of the generated gradient.
        for different heights, the gradient is translated so that it is always at the same position from the bottom
        */
        virtual void renderWindowBackground( QPainter* p, const QRect& clipRect, const QWidget* widget, const QWidget* window, const QPalette&  pal, int y_shift=-23 )
        { renderWindowBackground( p, clipRect, widget, window, pal.color( window->backgroundRole() ), y_shift ); }

        //* render window background using a given color as a reference
        virtual void renderWindowBackground( QPainter* p, const QRect& clipRect, const QWidget* widget, const QColor& color, int y_shift=-23 )
        { renderWindowBackground( p, clipRect, widget, widget->window(), color, y_shift ); }

        //* render window background using a given color as a reference
        virtual void renderWindowBackground( QPainter* p, const QRect& clipRect, const QWidget* widget, const QWidget* window, const QColor& color, int y_shift=-23 );

        virtual void renderWindowBackground( QPainter* p, const QRect& clipRect, const QRect& windowRect, const QColor& color, int y_shift );

        //@}

        //* dots
        void renderDot( QPainter*, const QPoint&, const QColor& );

        //* returns true for too 'dark' colors
        bool lowThreshold( const QColor& color );

        //* returns true for too 'light' colors
        bool highThreshold( const QColor& color );

        //* add alpha channel multiplier to color
        static QColor alphaColor( QColor color, qreal alpha );

        //* calculated light color from argument
        virtual QColor calcLightColor( const QColor& color );

        //* calculated dark color from argument
        virtual QColor calcDarkColor( const QColor& color );

        //* calculated shadow color from argument
        virtual QColor calcShadowColor( const QColor& color );

        //* returns menu background color matching position in a given top level widget
        virtual QColor backgroundColor( const QColor& color, const QWidget* w, const QPoint& point )
        {
            if( !( w && w->window() ) || checkAutoFillBackground( w ) ) return color;
            else return backgroundColor( color, w->window()->height(), w->mapTo( w->window(), point ).y() );
        }

        //* returns menu background color matching position in a top level widget of given height
        virtual QColor backgroundColor( const QColor& color, int height, int y )
        { return backgroundColor( color, qMin( qreal( 1.0 ), qreal( y )/qMin( 300, 3*height/4 ) ) ); }

        //* color used for background radial gradient
        virtual QColor backgroundRadialColor( const QColor& color );

        //* color used at the top of window background
        virtual QColor backgroundTopColor( const QColor& color );

        //* color used at the bottom of window background
        virtual QColor backgroundBottomColor( const QColor& color );

        //* vertical gradient for window background
        virtual QPixmap verticalGradient( const QColor& color, int height, int offset = 0 );

        //* radial gradient for window background
        virtual QPixmap radialGradient( const QColor& color, int width, int height = 20 );

        //* merge background and front color for check marks, arrows, etc. using _contrast
        virtual QColor decoColor( const QColor& background, const QColor& color );

        //* returns a region matching given rect, with rounded corners, based on the multipliers
        /** setting any of the multipliers to zero will result in no corners shown on the corresponding side */
        virtual QRegion roundedMask( const QRect&, int left = 1, int right = 1, int top = 1, int bottom = 1 ) const;

        //* returns a region matching given rect, with rounded corners
        virtual QBitmap roundedMask( const QSize&, Corners corners = AllCorners, qreal radius = 4 ) const;

        //* return rounded path in a given rect, with only selected corners rounded, and for a given radius
        QPainterPath roundedPath( const QRect&, Corners = AllCorners, qreal = 4 ) const;

        //* draw frame that mimics some sort of shadows around a panel
        /** it is used for menus, detached dock panels and toolbar, as well as window decoration when compositing is disabled */
        virtual void drawFloatFrame(
            QPainter* p, const QRect r, const QColor& color,
            bool drawUglyShadow=true, bool isActive=false,
            const QColor& frameColor=QColor(),
            TileSet::Tiles tiles = TileSet::Ring
            );

        //* draw dividing line
        virtual void drawSeparator( QPainter*, const QRect&, const QColor&, Qt::Orientation );

        //* focus color
        QColor focusColor( const QPalette& palette ) const
        { return _viewFocusBrush.brush( palette ).color(); }

        //* hover color
        QColor hoverColor( const QPalette& palette ) const
        { return _viewHoverBrush.brush( palette ).color(); }

        //* negative text color
        QColor negativeTextColor( const QPalette& palette ) const
        { return _viewNegativeTextBrush.brush( palette ).color(); }

        //* focus color
        QColor focusColor( QPalette::ColorGroup group ) const
        { return _viewFocusBrush.brush( group ).color(); }

        //* hover color
        QColor hoverColor( QPalette::ColorGroup group ) const
        { return _viewHoverBrush.brush( group ).color(); }

        //* negative text color
        QColor negativeTextColor( QPalette::ColorGroup group ) const
        { return _viewNegativeTextBrush.brush( group ).color(); }


        /**
        returns first widget in parent chain that sets autoFillBackground to true,
        or nullptr if none
        */
        const QWidget* checkAutoFillBackground( const QWidget* ) const;

        //*@name background gradient XProperty
        //@{

        //* set background gradient hint to widget
        virtual void setHasBackgroundGradient( WId, bool ) const;

        //* true if background gradient hint is set
        virtual bool hasBackgroundGradient( WId ) const;

        //@}

        //@name high dpi utility functions
        //@{

        //* return dpi-aware pixmap of given size
        virtual QPixmap highDpiPixmap( const QSize& size ) const
        { return highDpiPixmap( size.width(), size.height() ); }

        //* return dpi-aware pixmap of given size
        virtual QPixmap highDpiPixmap( int width ) const
        { return highDpiPixmap( width, width ); }

        //* return dpi-aware pixmap of given size
        virtual QPixmap highDpiPixmap( int width, int height ) const;

        //* return device pixel ratio for a given pixmap
        virtual qreal devicePixelRatio( const QPixmap& ) const;

        //@}

        //*@name compositing utilities
        //@{

        //* true if style was compiled for and is running on X11
        static bool isX11( void );

        //* true if running on platform Wayland
        static bool isWayland( void );

        //@}

        #if OXYGEN_HAVE_X11

        //* xcb connection
        static xcb_connection_t* connection( void );

        //* create xcb atom
        xcb_atom_t createAtom( const QString& ) const;

        #endif

        //* scoped pointer convenience typedef
        template <typename T> using ScopedPointer = QScopedPointer<T, QScopedPointerPodDeleter>;

        protected:

        //* return color key for a given color, properly accounting for invalid colors
        quint64 colorKey( const QColor& color ) const
        { return color.isValid() ? color.rgba():0; }

        //* generic outer shadow (to be stored in tilesets)
        virtual void drawShadow( QPainter&, const QColor&, int size );

        //* generic outer glow (to be stored in tilesets)
        virtual void drawOuterGlow( QPainter&, const QColor&, int size );

        //* return background adjusted color matching relative vertical position in window
        QColor backgroundColor( const QColor&, qreal ratio );

        //*@name global configuration parameters
        //@{

        static const qreal _glowBias;
        static const qreal _slabThickness;
        static const qreal _shadowGain;
        qreal _contrast;

        //@}

        //* shortcut to color caches
        /** it is made protected because it is also used in the style helper */
        using ColorCache = BaseCache<QColor>;

        //* shortcut to pixmap cache
        using PixmapCache = BaseCache<QPixmap>;

        private:

        //* initialize
        void init( void );

        //* configuration
        KSharedConfig::Ptr _config;
        qreal _bgcontrast;

        //*@name brushes
        //@{
        KStatefulBrush _viewFocusBrush;
        KStatefulBrush _viewHoverBrush;
        KStatefulBrush _viewNegativeTextBrush;
        //@}

        //*@name color caches
        //@{
        ColorCache _decoColorCache;
        ColorCache _lightColorCache;
        ColorCache _darkColorCache;
        ColorCache _shadowColorCache;
        ColorCache _backgroundTopColorCache;
        ColorCache _backgroundBottomColorCache;
        ColorCache _backgroundRadialColorCache;
        ColorCache _backgroundColorCache;
        //@}

        PixmapCache _backgroundCache;
        PixmapCache _dotCache;

        //* high threshold colors
        using ColorMap = QMap<quint32, bool>;
        ColorMap _highThreshold;
        ColorMap _lowThreshold;

        #if OXYGEN_HAVE_X11

        //* set value for given hint
        void setHasHint( xcb_window_t, xcb_atom_t, bool ) const;

        //* value for given hint
        bool hasHint( xcb_window_t, xcb_atom_t ) const;

        //* background gradient hint atom
        xcb_atom_t _backgroundGradientAtom;

        #endif

        bool _isX11;
    };

}

#endif
