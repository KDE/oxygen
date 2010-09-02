#ifndef oxygen_style_helper_h
#define oxygen_style_helper_h

/*
 * Copyright 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright 2007 Casper Boemann <cbr@boemann.dk>
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

#include "oxygenhelper.h"
#include "oxygenanimationmodes.h"

#include <KWindowSystem>

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#endif

//! helper class
/*! contains utility functions used at multiple places in oxygen style */
namespace Oxygen
{
    class StyleHelper : public Helper
    {
        public:

        //! constructor
        explicit StyleHelper(const QByteArray &componentName);

        //! destructor
        virtual ~StyleHelper() {}

        //! clear cache
        virtual void invalidateCaches();

        //! update maximum cache size
        virtual void setMaxCacheSize( int );

        // render menu background
        void renderMenuBackground( QPainter* p, const QRect& clipRect, const QWidget* widget, const QPalette& pal)
        { renderMenuBackground( p, clipRect, widget, pal.color( widget->window()->backgroundRole() ) ); }

        // render menu background
        void renderMenuBackground( QPainter*, const QRect&, const QWidget*, const QColor& );

        //! returns menu background color matching position in a given menu widget
        virtual const QColor& menuBackgroundColor(const QColor &color, const QWidget* w, const QPoint& point )
        {
            if( !( w && w->window() ) || checkAutoFillBackground( w ) ) return color;
            else return menuBackgroundColor( color, w->window()->height(), w->mapTo( w->window(), point ).y() );
        }

        //! returns menu background color matching position in a menu widget of given height
        virtual const QColor& menuBackgroundColor(const QColor &color, int height, int y)
        { return backgroundColor( color, qMin(qreal(1.0), qreal(y)/qMin(200, 3*height/4) ) ); }

        //! color
        inline const QColor& calcMidColor(const QColor &color);

        //! merge active and inactive palettes based on ratio, for smooth enable state change transition
        QPalette mergePalettes( const QPalette&, qreal ratio ) const;

        //! overloaded window decoration buttons for MDI windows
        virtual QPixmap windecoButton(const QColor &color, bool pressed, int size = 21);

        //! round corners (used for Menus, combobox drop-down, detached toolbars and dockwidgets
        TileSet *roundCorner(const QColor&, int size = 5);

        //! groupbox background
        TileSet *slope(const QColor&, qreal shade, int size = 7);

        //!@name slabs
        //@{

        void fillSlab(QPainter&, const QRect&, int size = 7) const;

        // progressbar
        QPixmap progressBarIndicator( const QPalette&, const QRect& );

        QPixmap dialSlab(const QColor&, qreal shade, int size = 7);
        QPixmap dialSlabFocused(const QColor&, const QColor&, qreal shade, int size = 7);
        QPixmap roundSlab(const QColor&, qreal shade, int size = 7);
        QPixmap roundSlabFocused(const QColor&, const QColor &glowColor, qreal shade, int size = 7);

        TileSet *slabFocused(const QColor&, const QColor &glowColor, qreal shade, int size = 7);
        TileSet *slabSunken(const QColor&, qreal shade, int size = 7);
        TileSet *slabInverted(const QColor&, qreal shade, int size = 7);

        //@}

        //!@name holes
        //@{

        void fillHole(QPainter&, const QRect&, int size = 7) const;

        //! generic hole
        void renderHole(QPainter *p, const QColor& color, const QRect &r,
            bool focus=false, bool hover=false,
            TileSet::Tiles posFlags = TileSet::Ring, bool outline = false)
        { renderHole( p, color, r, focus, hover, -1, Oxygen::AnimationNone, posFlags, outline ); }

        //! generic hole (with animated glow)
        void renderHole(QPainter *p, const QColor&, const QRect &r,
            bool focus, bool hover,
            qreal opacity, Oxygen::AnimationMode animationMode,
            TileSet::Tiles posFlags = TileSet::Ring, bool outline = false);

        TileSet *hole(const QColor&, qreal shade, int size = 7, bool outline = false);
        TileSet *holeFlat(const QColor&, qreal shade, int size = 7);
        TileSet *holeFocused(const QColor&, const QColor &glowColor, qreal shade, int size = 7, bool outline = false);

        //! scrollbar hole
        TileSet *scrollHole(const QColor&, Qt::Orientation orientation, bool smallShadow = false );

        //@}

        //! scrollbar groove
        TileSet *groove(const QColor&, qreal shade, int size = 7);

        //! focus rect for flat toolbuttons
        TileSet *slitFocused(const QColor&);

        //! dock frame
        TileSet *dockFrame(const QColor&, int size);

        //! selection
        TileSet *selection( const QColor&, int height, bool custom );

        // these two methods must be public because they are used directly by OxygenStyle to draw dials
        void drawInverseShadow(QPainter&, const QColor&, int pad, int size, qreal fuzz);
        void drawInverseGlow(QPainter&, const QColor&, int pad, int size, int rsize) const;

        //!@name utility functions

        //! returns true if compositing is active
        bool compositingActive( void ) const
        { return KWindowSystem::compositingActive(); }

        //! returns true if a given widget supports alpha channel
        inline bool hasAlphaChannel( const QWidget* ) const;

        //@}

        protected:

        void drawHole(QPainter&, const QColor&, qreal shade, int r = 7);
        void drawRoundSlab( QPainter&, const QColor&, qreal );

        Cache<QPixmap> m_dialSlabCache;
        Cache<QPixmap> m_roundSlabCache;
        Cache<TileSet> m_holeFocusedCache;

        //! mid color cache
        ColorCache m_midColorCache;

        //! progressbar cache
        PixmapCache m_progressBarCache;

        typedef BaseCache<TileSet> TileSetCache;
        TileSetCache m_cornerCache;
        TileSetCache m_slabSunkenCache;
        TileSetCache m_slabInvertedCache;
        TileSetCache m_holeCache;
        TileSetCache m_holeFlatCache;
        TileSetCache m_slopeCache;
        TileSetCache m_grooveCache;
        TileSetCache m_slitCache;
        TileSetCache m_dockFrameCache;
        TileSetCache m_scrollHoleCache;
        TileSetCache m_selectionCache;

    };

    //____________________________________________________________________
    const QColor& StyleHelper::calcMidColor( const QColor& color )
    {
        const quint64 key( color.rgba() );
        QColor* out( m_midColorCache.object( key ) );
        if( !out )
        {
            out = new QColor( KColorScheme::shade(color, KColorScheme::MidShade, _contrast - 1.0) );
            m_midColorCache.insert( key, out );
        }

        return *out;
    }

    //____________________________________________________________________
    bool StyleHelper::hasAlphaChannel( const QWidget* widget ) const
    {
        #ifdef Q_WS_X11
        if( compositingActive() )
        {

            if( widget ) return widget->x11Info().depth() == 32;
            else return QX11Info().appDepth() == 32;

        } else return false;

        #else
        return compositingActive();
        #endif

    }

}
#endif
