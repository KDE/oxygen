/*
* Copyright 2014  Martin Gräßlin <mgraesslin@kde.org>
* Copyright 2014  Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "oxygensettingsprovider.h"
#include "config-oxygen.h"
#include "config/oxygenconfig.h"

#include "oxygenbutton.h"
#include "oxygentitleanimationdata.h"
#include "oxygensizegrip.h"

#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationButtonGroup>
#include <KDecoration2/DecorationSettings>
#include <KDecoration2/DecorationShadow>

#include <KConfigGroup>
#include <KColorUtils>
#include <KSharedConfig>
#include <KPluginFactory>

#include <QPainter>
#include <QTextStream>

#if OXYGEN_HAVE_X11
#include <QX11Info>
#endif

K_PLUGIN_FACTORY_WITH_JSON(
    OxygenDecoFactory,
    "oxygen.json",
    registerPlugin<Oxygen::Decoration>();
    registerPlugin<Oxygen::Button>(QStringLiteral("button"));
    registerPlugin<Oxygen::ConfigurationModule>(QStringLiteral("kcmodule"));
)

namespace Oxygen
{


    //________________________________________________________________
    static int g_sDecoCount = 0;
    static QSharedPointer<KDecoration2::DecorationShadow> g_sShadow;

    Decoration::Decoration(QObject *parent, const QVariantList &args)
        : KDecoration2::Decoration(parent, args)
        , _titleAnimationData(new TitleAnimationData(this) )
    {
        g_sDecoCount++;
    }

    //________________________________________________________________
    Decoration::~Decoration()
    {
        g_sDecoCount--;
        if (g_sDecoCount == 0) {
            // last deco destroyed, clean up shadow
            g_sShadow.clear();
        }
    }

    //________________________________________________________________
    void Decoration::init()
    {
        reconfigure();
        updateTitleBar();
        auto s = settings();
        connect(s.data(), &KDecoration2::DecorationSettings::borderSizeChanged, this, &Decoration::recalculateBorders);

        // a change in font might cause the borders to change
        connect(s.data(), &KDecoration2::DecorationSettings::fontChanged, this, &Decoration::recalculateBorders);
        connect(s.data(), &KDecoration2::DecorationSettings::spacingChanged, this, &Decoration::recalculateBorders);

        // full reconfiguration
        connect(s.data(), &KDecoration2::DecorationSettings::reconfigured, this, &Decoration::reconfigure);
        connect(s.data(), &KDecoration2::DecorationSettings::reconfigured, SettingsProvider::self(), &SettingsProvider::reconfigure, Qt::UniqueConnection );

        connect(client().data(), &KDecoration2::DecoratedClient::adjacentScreenEdgesChanged, this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedHorizontallyChanged, this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedVerticallyChanged, this, &Decoration::recalculateBorders);

        auto updateCaption = [this]()
        {
            if (m_internalSettings->animationsEnabled())
            { _titleAnimationData->setDirty( true ); }
            update();
        };

        connect(client().data(), &KDecoration2::DecoratedClient::activeChanged, this, updateCaption );
        connect(client().data(), &KDecoration2::DecoratedClient::captionChanged, this, updateCaption );

        //decoration has an overloaded update function, force the compiler to choose the right one
        connect(client().data(), &KDecoration2::DecoratedClient::paletteChanged,   this,  static_cast<void (Decoration::*)()>(&Decoration::update));
        connect(client().data(), &KDecoration2::DecoratedClient::widthChanged,     this, &Decoration::updateTitleBar);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateTitleBar);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::setOpaque);

        connect(client().data(), &KDecoration2::DecoratedClient::widthChanged,     this, &Decoration::updateButtonPositions);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateButtonPositions);
        connect(client().data(), &KDecoration2::DecoratedClient::shadedChanged,    this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::shadedChanged,    this, &Decoration::updateButtonPositions);

        connect(_titleAnimationData, &TitleAnimationData::pixmapsChanged, this,  static_cast<void (Decoration::*)()>(&Decoration::update));

        createButtons();
        createShadow();

        _titleAnimationData->initialize();
    }

    //________________________________________________________________
    void Decoration::updateTitleBar()
    {
        auto s = settings();
        const bool maximized = isMaximized();
        const int width =  maximized ? client().data()->width() : client().data()->width() - 2*s->largeSpacing()*Metrics::TitleBar_SideMargin;
        const int height = maximized ? borderTop() : borderTop() - s->smallSpacing()*Metrics::TitleBar_TopMargin;
        const int x = maximized ? 0 : s->largeSpacing()*Metrics::TitleBar_SideMargin;
        const int y = maximized ? 0 : s->smallSpacing()*Metrics::TitleBar_TopMargin;
        setTitleBar(QRect(x, y, width, height));
    }

    //________________________________________________________________
    int Decoration::borderSize(bool bottom) const
    {
        const int baseSize = settings()->smallSpacing();

        auto maxSize = [] (int size) {
            const int minSize = 4;
            return qMax(size, minSize);
        };

        if( m_internalSettings && (m_internalSettings->mask() & BorderSize ) )
        {
            switch (m_internalSettings->borderSize()) {
                case InternalSettings::BorderNone: return 0;
                case InternalSettings::BorderNoSides: return bottom ? maxSize(baseSize) : 0;
                default:
                case InternalSettings::BorderTiny: return maxSize(baseSize);
                case InternalSettings::BorderNormal: return maxSize(baseSize*2);
                case InternalSettings::BorderLarge: return maxSize(baseSize * 3);
                case InternalSettings::BorderVeryLarge: return maxSize(baseSize * 4);
                case InternalSettings::BorderHuge: return maxSize(baseSize * 5);
                case InternalSettings::BorderVeryHuge: return maxSize(baseSize * 6);
                case InternalSettings::BorderOversized: return maxSize(baseSize * 10);
            }

        } else {

            switch (settings()->borderSize()) {
                case KDecoration2::BorderSize::None: return 0;
                case KDecoration2::BorderSize::NoSides: return bottom ? maxSize(baseSize ) : 0;
                default:
                case KDecoration2::BorderSize::Tiny: return maxSize(baseSize);
                case KDecoration2::BorderSize::Normal: return maxSize(baseSize*2);
                case KDecoration2::BorderSize::Large: return maxSize(baseSize * 3);
                case KDecoration2::BorderSize::VeryLarge: return maxSize(baseSize * 4);
                case KDecoration2::BorderSize::Huge: return maxSize(baseSize * 5);
                case KDecoration2::BorderSize::VeryHuge: return maxSize(baseSize * 6);
                case KDecoration2::BorderSize::Oversized: return maxSize(baseSize * 10);

            }

        }
    }

    //________________________________________________________________
    void Decoration::reconfigure()
    {

        m_internalSettings = SettingsProvider::self()->internalSettings( this );

        // borders
        recalculateBorders();

        // size grip
        if( hasNoBorders() && m_internalSettings->drawSizeGrip() ) createSizeGrip();
        else deleteSizeGrip();

    }

    //________________________________________________________________
    void Decoration::recalculateBorders()
    {
        auto s = settings();
        const auto c = client().data();
        const Qt::Edges edges = c->adjacentScreenEdges();
        int left   = isMaximizedHorizontally() || edges.testFlag(Qt::LeftEdge) ? 0 : borderSize();
        int right  = isMaximizedHorizontally() || edges.testFlag(Qt::RightEdge) ? 0 : borderSize();

        QFontMetrics fm(s->font());
        int top = qMax(fm.boundingRect(c->caption()).height(), buttonHeight() );

        // padding below
        // extra pixel is used for the active window outline
        const int baseSize = settings()->smallSpacing();
        top += baseSize*Metrics::TitleBar_TopMargin + 1;

        // padding above
        if (!isMaximized()) top += baseSize*TitleBar_BottomMargin;

        int bottom = isMaximizedVertically() || c->isShaded() || edges.testFlag(Qt::BottomEdge) ? 0 : borderSize(true);
        setBorders(QMargins(left, top, right, bottom));

        // extended sizes
        const int extSize = s->largeSpacing();
        int extSides = 0;
        int extBottom = 0;
        if( hasNoBorders() )
        {
            extSides = extSize;
            extBottom = extSize;

        } else if( hasNoSideBorders() ) {

            extSides = extSize;

        }

        setResizeOnlyBorders(QMargins(extSides, 0, extSides, extBottom));
    }

    //________________________________________________________________
    void Decoration::createButtons()
    {
        m_leftButtons = new KDecoration2::DecorationButtonGroup(KDecoration2::DecorationButtonGroup::Position::Left, this, &Button::create);
        m_rightButtons = new KDecoration2::DecorationButtonGroup(KDecoration2::DecorationButtonGroup::Position::Right, this, &Button::create);
        updateButtonPositions();
    }

    //________________________________________________________________
    void Decoration::updateButtonPositions()
    {
        auto s = settings();
        const int vPadding = (isMaximized() ? 0 : s->smallSpacing()*Metrics::TitleBar_TopMargin) + (captionHeight()-buttonHeight())/2;
        const int hPadding = isMaximized() ? 0 : s->smallSpacing()*Metrics::TitleBar_SideMargin;

        m_rightButtons->setSpacing(s->smallSpacing()*Metrics::TitleBar_ButtonSpacing);
        m_leftButtons->setSpacing(s->smallSpacing()*Metrics::TitleBar_ButtonSpacing);
        m_leftButtons->setPos(QPointF(hPadding, vPadding));
        m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width() - hPadding, vPadding));
    }

    //________________________________________________________________
    void Decoration::paint(QPainter *painter, const QRect &repaintRegion)
    {
        const QPalette palette = client().data()->palette();

        renderWindowBorder(painter, rect(), palette);
        renderCorners(painter, rect(), palette);

        m_leftButtons->paint(painter, repaintRegion);
        m_rightButtons->paint(painter, repaintRegion);

        renderTitleText(
                painter, captionRect(),
                titlebarTextColor( palette ),
                titlebarContrastColor( palette ) );

    }

    //________________________________________________________________
    int Decoration::buttonHeight() const
    {
        const int baseSize = settings()->gridUnit() + 2; //oxygen icons were always slightly larger
        switch( m_internalSettings->buttonSize() )
        {
            case Oxygen::InternalSettings::ButtonSmall: return baseSize*1.5;
            default:
            case Oxygen::InternalSettings::ButtonDefault: return baseSize*2;
            case Oxygen::InternalSettings::ButtonLarge: return baseSize*2.5;
            case Oxygen::InternalSettings::ButtonVeryLarge: return baseSize*3.5;
        }

    }

    //________________________________________________________________
    int Decoration::captionHeight() const
    {
        return isMaximized() ?
            borderTop() - settings()->smallSpacing()*Metrics::TitleBar_BottomMargin - 1:
            borderTop() - settings()->smallSpacing()*(Metrics::TitleBar_BottomMargin + Metrics::TitleBar_TopMargin ) - 1;
    }

    //________________________________________________________________
    QRect Decoration::captionRect() const
    {
        const int leftOffset = m_leftButtons->geometry().x() + m_leftButtons->geometry().width() + Metrics::TitleBar_SideMargin*settings()->smallSpacing();
        const int rightOffset = size().width() - m_rightButtons->geometry().x() + Metrics::TitleBar_SideMargin*settings()->smallSpacing();
        const int yOffset = isMaximized() ? 0 : settings()->smallSpacing()*Metrics::TitleBar_TopMargin;

        QRect boundingRect( settings()->fontMetrics().boundingRect( client().data()->caption()).toRect() );
        boundingRect.setTop( yOffset );
        boundingRect.setHeight( captionHeight() );

        /* need to increase the bounding rect because it is sometime (font dependent)
        too small, resulting in text being elided */
        boundingRect.setWidth( boundingRect.width()+12 );

        switch( m_internalSettings->titleAlignment() )
        {
            case Oxygen::InternalSettings::AlignLeft:
            boundingRect.moveLeft( leftOffset );
            break;

            case Oxygen::InternalSettings::AlignRight:
            boundingRect.moveRight( size().width() - rightOffset - 1 );
            break;

            case Oxygen::InternalSettings::AlignCenter:
            boundingRect.moveLeft( leftOffset + (size().width() - leftOffset - rightOffset - boundingRect.width() )/2 );
            break;

            default:
            case Oxygen::InternalSettings::AlignCenterFullWidth:
            boundingRect.moveLeft( ( size().width() - boundingRect.width() )/2 );
            break;

        }

        // make sure there is no overlap with buttons
        if( boundingRect.left() < leftOffset )
        {

            boundingRect.moveLeft( leftOffset );
            boundingRect.setRight( qMin( boundingRect.right(), size().width() - rightOffset - 1 ) );

        } else if( boundingRect.right() >  size().width() - rightOffset - 1 ) {

            boundingRect.moveRight( size().width() - rightOffset - 1 );
            boundingRect.setLeft( qMax( boundingRect.left(), leftOffset ) );
        }

        return boundingRect;
    }

    //________________________________________________________________
    void Decoration::createShadow()
    {
        if (g_sShadow) {
            setShadow(g_sShadow);
            return;
        }

        // setup shadow
        auto decorationShadow = QSharedPointer<KDecoration2::DecorationShadow>::create();
        decorationShadow->setPadding( QMargins(
            Metrics::Shadow_Size-Metrics::Shadow_Offset,
            Metrics::Shadow_Size-Metrics::Shadow_Offset,
            Metrics::Shadow_Size,
            Metrics::Shadow_Size ) );

        decorationShadow->setInnerShadowRect(QRect(
            Metrics::Shadow_Size-Metrics::Shadow_Offset+Metrics::Shadow_Overlap,
            Metrics::Shadow_Size-Metrics::Shadow_Offset+Metrics::Shadow_Overlap,
            Metrics::Shadow_Offset - 2*Metrics::Shadow_Overlap,
            Metrics::Shadow_Offset - 2*Metrics::Shadow_Overlap ) );

        // create image
        QImage image(2*Metrics::Shadow_Size, 2*Metrics::Shadow_Size, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);

        QPainter p(&image);
        p.setCompositionMode(QPainter::CompositionMode_Source);

        // create gradient
        auto gradientStopColor = [](QColor color, qreal alpha) {
            color.setAlphaF(alpha);
            return color;
        };

        const QColor shadowColor( client().data()->palette().color( QPalette::Shadow ) );

        QRadialGradient radialGradient( Metrics::Shadow_Size, Metrics::Shadow_Size, Metrics::Shadow_Size);
        radialGradient.setColorAt(0.0,  gradientStopColor(shadowColor, 0.35));
        radialGradient.setColorAt(0.25, gradientStopColor(shadowColor, 0.25));
        radialGradient.setColorAt(0.5,  gradientStopColor(shadowColor, 0.13));
        radialGradient.setColorAt(0.75, gradientStopColor(shadowColor, 0.04));
        radialGradient.setColorAt(1.0,  gradientStopColor(shadowColor, 0.0));

        // fill
        p.fillRect( image.rect(), radialGradient);

        // assign to shadow
        decorationShadow->setShadow(image);

        g_sShadow = decorationShadow;
        setShadow(decorationShadow);
    }


    //_________________________________________________________
    QColor Decoration::titlebarTextColor(const QPalette &palette) const
    {
        if( glowIsAnimated() ) return KColorUtils::mix(
            titlebarTextColor( palette, false, true ),
            titlebarTextColor( palette, true, true ),
            glowIntensity() );
        else return titlebarTextColor( palette, client().data()->isActive(), true );
    }

    //_________________________________________________________
    QColor Decoration::titlebarTextColor(const QPalette &palette, bool windowActive, bool itemActive ) const
    {
        if( itemActive )
        {
            return windowActive ?
                palette.color(QPalette::Active, QPalette::WindowText):
                DecoHelper::self()->inactiveTitleBarTextColor( palette );
        } else if( internalSettings()->drawTitleOutline() ) {
            return palette.color(QPalette::Foreground);
        } else {
            return DecoHelper::self()->inactiveTitleBarTextColor( palette );
        }
    }

    //_________________________________________________________
    QColor Decoration::titlebarContrastColor(const QPalette& palette) const
    {
        return titlebarContrastColor( palette.color(QPalette::Background) );
    }

    //_________________________________________________________
    QColor Decoration::titlebarContrastColor(const QColor& color) const
    {
        return DecoHelper::self()->calcLightColor( color );

    }

    //_________________________________________________________
    void Decoration::renderCorners( QPainter* painter, const QRect& frame, const QPalette& palette ) const
    {

        const QColor color(palette.color(QPalette::Background));

        QLinearGradient lg = QLinearGradient(0, -0.5, 0, qreal( frame.height() )+0.5);
        lg.setColorAt(0.0, DecoHelper::self()->calcLightColor( DecoHelper::self()->backgroundTopColor(color) ));
        lg.setColorAt(0.51, DecoHelper::self()->backgroundBottomColor(color) );
        lg.setColorAt(1.0, DecoHelper::self()->backgroundBottomColor(color) );

        painter->setPen( QPen( lg, 1 ) );
        painter->setBrush( Qt::NoBrush );
        painter->drawRoundedRect( QRectF( frame ).adjusted( 0.5, 0.5, -0.5, -0.5 ), 3.5,  3.5 );
    }

    //_________________________________________________________
    void Decoration::renderWindowBackground( QPainter* painter, const QRect& clipRect, const QPalette& palette ) const
    {

        QRect innerClientRect = rect();
        if (settings()->isAlphaChannelSupported())
        {
            // size of window minus the outlines for the rounded corners
            innerClientRect.adjust(1,1,-1,-1);
        }

        //without compositing without a mask we get black boxes in the corner, just paint a big rectangle over everything
        if( DecoHelper::self()->hasBackgroundGradient( client().data()->windowId() ) )
        {

            DecoHelper::self()->renderWindowBackground(painter, clipRect, innerClientRect, palette.color(QPalette::Window), 0, 20 );

        } else {

            painter->fillRect( innerClientRect, palette.color( QPalette::Window ) );

        }

    }

    //_________________________________________________________
    void Decoration::renderWindowBorder( QPainter* painter, const QRect& clipRect, const QPalette& palette ) const
    {
        // save painter
        if( clipRect.isValid() )
        {
            painter->save();
            painter->setClipRegion(clipRect,Qt::IntersectClip);
        }

        QRect r = rect();

        // base color
        QColor color( palette.color( QPalette::Window ) );

        // title height
        const int titleHeight(7);

        // horizontal line
        {
            const int shadowSize = 7;
            const int height = shadowSize-3;

            const QPoint topLeft( r.topLeft()+QPoint(0,titleHeight-height));
            QRect rect( topLeft, QSize( r.width(), height ) );

            // adjustements to cope with shadow size and outline border.
            rect.adjust( -shadowSize, 0, shadowSize-1, 0 );
            if( internalSettings()->drawTitleOutline() && ( client().data()->isActive() || glowIsAnimated() ) && !isMaximized() )
            {
                if( internalSettings()->borderSize() == InternalSettings::BorderTiny ) rect.adjust( 1, 0, -1, 0 );
                else if( internalSettings()->borderSize() > InternalSettings::BorderTiny ) rect.adjust( Metrics::TitleBar_OutlineMargin-1, 0, -Metrics::TitleBar_OutlineMargin+1, 0 );
            }

            if( rect.isValid() ) {
                DecoHelper::self()->slab( color, 0, shadowSize )->render( rect, painter, TileSet::Top );
            }
        }

        if( internalSettings()->drawTitleOutline() && ( client().data()->isActive() || glowIsAnimated() ) )
        {

            // save old hints and turn off anti-aliasing
            const QPainter::RenderHints hints( painter->renderHints() );
            painter->setRenderHint( QPainter::Antialiasing, false );

            // save mask and frame to where
            // grey window background is to be rendered
            QRegion mask;
            QRect frame;

            // left and right
            const int topOffset = titleHeight;
            const int height = r.height();

            if( SettingsProvider::self()->internalSettings(this)->borderSize() >= 2 )
            {

                const QColor shadow( DecoHelper::self()->calcLightColor( color ) );
                painter->setPen( shadow );

                // left
                int width = borderSize();
                QRect rect( r.topLeft() + QPoint( - width, topOffset ), QSize( width, height ) );
                if( width > 0 ) { mask += rect; frame |= rect; }

                painter->drawLine( rect.topLeft()-QPoint(1,0), rect.bottomLeft()-QPoint(1, 0) );

                // right
                width = borderSize();
                rect = QRect(r.topRight() + QPoint( 0, topOffset ), QSize( width, height ));
                if( width > 0 ) { mask += rect; frame |= rect; }

                painter->drawLine( rect.topRight()+QPoint(1,0), rect.bottomRight()+QPoint(1, 0) );
            }

            // restore old hints
            painter->setRenderHints( hints );

            // paint
            if( !mask.isEmpty() ) {
                painter->setClipRegion( mask, Qt::IntersectClip);
            }
        }
        renderWindowBackground(painter, clipRect, palette );

        // restore painter
        if( clipRect.isValid() )
        { painter->restore(); }

    }

    //_________________________________________________________
    void Decoration::renderTitleOutline(  QPainter* painter, const QRect& rect, const QPalette& palette ) const
    {

        // center (for active windows only)
        {
            painter->save();
            QRect adjustedRect( rect.adjusted( 1, 1, -1, 1 ) );

            // prepare painter mask
            QRegion mask( adjustedRect.adjusted( 1, 0, -1, 0 ) );
            mask += adjustedRect.adjusted( 0, 1, 0, 0 );
            painter->setClipRegion( mask, Qt::IntersectClip );

            // draw window background
            renderWindowBackground(painter, adjustedRect, palette );
            painter->restore();
        }

        // shadow
        const int shadowSize( 7 );
        const int offset( -3 );
        const int voffset( 5-shadowSize );
        const QRect adjustedRect( rect.adjusted(offset, voffset, -offset, shadowSize) );
        QColor color = palette.color(QPalette::Background);

        // render slab
        DecoHelper::self()->slab( color, 0, shadowSize )->render( adjustedRect, painter, TileSet::Tiles(TileSet::Top|TileSet::Left|TileSet::Right) );

    }

    //_________________________________________________________
    void Decoration::renderTitleText( QPainter* painter, const QRect& rect, const QColor& color, const QColor& contrast ) const
    {
        const QString caption = client().data()->caption();

        if( !_titleAnimationData->isValid() )
        {
            // contrast pixmap
            _titleAnimationData->reset(
                rect,
                renderTitleText( rect, caption, color ),
                renderTitleText( rect, caption, contrast ) );
        }

        if( _titleAnimationData->isDirty() )
        {
            // clear dirty flags
            _titleAnimationData->setDirty( false );

            // finish current animation if running
            if( _titleAnimationData->isAnimated() )
            { _titleAnimationData->finishAnimation(); }

            if( !_titleAnimationData->isLocked() )
            {

                // set pixmaps
                _titleAnimationData->setPixmaps(
                    rect,
                    renderTitleText( rect, caption, color ),
                    renderTitleText( rect, caption, contrast ) );

                _titleAnimationData->startAnimation();
                renderTitleText( painter, rect, color, contrast );

            } else if( !caption.isEmpty() ) {

                renderTitleText( painter, rect, caption, color, contrast );

            }

            // lock animations (this must be done whether or not
            // animation was actually started, in order to extend locking
            // every time title get changed too rapidly
            _titleAnimationData->lockAnimations();

        } else if( _titleAnimationData->isAnimated() ) {

            if( isMaximized() ) painter->translate( 0, 2 );
            if( !_titleAnimationData->contrastPixmap().isNull() )
            {
                painter->translate( 0, 1 );
                painter->drawPixmap( rect.topLeft(), _titleAnimationData->contrastPixmap() );
                painter->translate( 0, -1 );
            }

            painter->drawPixmap( rect.topLeft(), _titleAnimationData->pixmap() );

            if( isMaximized() ) painter->translate( 0, -2 );

        } else if( !client().data()->caption().isEmpty() ) {

            renderTitleText( painter, rect, client().data()->caption(), color, contrast );

        }
    }

    //_______________________________________________________________________
    void Decoration::renderTitleText( QPainter* painter, const QRect& rect, const QString& caption, const QColor& color, const QColor& contrast, bool elide ) const
    {
        const QString local( elide ? QFontMetrics( painter->font() ).elidedText( caption, Qt::ElideRight, rect.width() ):caption );

        // setup font
        painter->setFont( settings()->font() );

        // translate title down in case of maximized window
        if( isMaximized() ) painter->translate( 0, 2 );

        if( contrast.isValid() )
        {
            painter->setPen( contrast );
            painter->translate( 0, 1 );
            painter->drawText( rect, Qt::AlignCenter, local ); //align center as we've already put the caption rect in the right place
            painter->translate( 0, -1 );
        }

        painter->setPen( color );
        painter->drawText( rect, Qt::AlignCenter, local );

        // translate back
        if( isMaximized() ) painter->translate( 0, -2 );

    }

    //_______________________________________________________________________
    QPixmap Decoration::renderTitleText( const QRect& rect, const QString& caption, const QColor& color, bool elide ) const
    {

        if( !rect.isValid() ) return QPixmap();

        QPixmap out( rect.size() );
        out.fill( Qt::transparent );
        if( caption.isEmpty() || !color.isValid() ) return out;

        QPainter painter( &out );
        painter.setFont( settings()->font() );
        const QString local( elide ? QFontMetrics( painter.font() ).elidedText( caption, Qt::ElideRight, rect.width() ):caption );

        painter.setPen( color );
        painter.drawText( out.rect(), Qt::AlignCenter, local );
        painter.end();
        return out;
    }

    //_________________________________________________________________
    void Decoration::createSizeGrip( void )
    {

        // do nothing if size grip already exist
        if( m_sizeGrip ) return;

        #if OXYGEN_HAVE_X11
        if( !QX11Info::isPlatformX11() ) return;

        // access client
        KDecoration2::DecoratedClient *c( client().data() );
        if( !c ) return;

        if( ( c->isResizeable() && c->windowId() != 0 ) )
        {
            m_sizeGrip = new SizeGrip( this );
            m_sizeGrip->setVisible( !( isMaximized() || c->isShaded() ) );
        }
        #endif

    }

    //_________________________________________________________________
    void Decoration::deleteSizeGrip( void )
    {
        if( m_sizeGrip )
        {
            m_sizeGrip->deleteLater();
            m_sizeGrip = nullptr;
        }
    }

} // namespace


#include "oxygendecoration.moc"
