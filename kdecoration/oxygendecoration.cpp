/*
* Copyright (c) 2014 Martin Gräßlin <mgraesslin@kde.org>
* Copyright (c) 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2015 David Edmundson <davidedmundson@kde.org>
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
#include "oxygenshadowcache.h"
#include "config-oxygen.h"
#include "config/oxygenconfigwidget.h"

#include "oxygenbutton.h"
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
#include <QTimer>

#if OXYGEN_HAVE_X11
#include <QX11Info>
#endif

K_PLUGIN_FACTORY_WITH_JSON(
    OxygenDecoFactory,
    "oxygen.json",
    registerPlugin<Oxygen::Decoration>();
    registerPlugin<Oxygen::Button>(QStringLiteral("button"));
    registerPlugin<Oxygen::ConfigWidget>(QStringLiteral("kcmodule"));
)

namespace Oxygen
{


    //________________________________________________________________
    using DecorationShadowPointer = QSharedPointer<KDecoration2::DecorationShadow>;
    using ShadowMap = QHash<int,DecorationShadowPointer>;

    static int g_sDecoCount = 0;
    static ShadowMap g_sShadows;

    Decoration::Decoration(QObject *parent, const QVariantList &args)
        : KDecoration2::Decoration(parent, args)
        , m_animation( new QPropertyAnimation( this ) )
    {
        g_sDecoCount++;
    }

    //________________________________________________________________
    Decoration::~Decoration()
    {
        g_sDecoCount--;
        if (g_sDecoCount == 0) g_sShadows.clear();

        deleteSizeGrip();

    }


    //________________________________________________________________
    void Decoration::setOpacity( qreal value )
    {
        if( m_opacity == value ) return;
        m_opacity = value;
        updateShadow();
        update();

        if( m_sizeGrip ) m_sizeGrip->update();
    }

    //_________________________________________________________
    QColor Decoration::titlebarTextColor(const QPalette &palette) const
    {
        if( hideTitleBar() ) return titlebarTextColor( palette, false );
        if( m_animation->state() == QPropertyAnimation::Running )
        {

            return KColorUtils::mix(
                titlebarTextColor( palette, false ),
                titlebarTextColor( palette, true ),
                m_opacity );

        } else {

            return titlebarTextColor( palette, client().data()->isActive() );

        }

    }

    //_________________________________________________________
    QColor Decoration::titlebarTextColor(const QPalette &palette, bool active ) const
    { return palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::WindowText ); }

    //_________________________________________________________
    QColor Decoration::titlebarContrastColor(const QPalette& palette) const
    { return titlebarContrastColor( palette.color(QPalette::Background) ); }

    //_________________________________________________________
    QColor Decoration::titlebarContrastColor(const QColor& color) const
    { return SettingsProvider::self()->helper()->calcLightColor( color ); }

    //________________________________________________________________
    void Decoration::init()
    {

        // active state change animation
        m_animation->setStartValue( 0 );
        m_animation->setEndValue( 1.0 );
        m_animation->setTargetObject( this );
        m_animation->setPropertyName( "opacity" );
        m_animation->setEasingCurve( QEasingCurve::InOutQuad );

        reconfigure();
        updateTitleBar();
        auto s = settings();
        connect(s.data(), &KDecoration2::DecorationSettings::borderSizeChanged, this, &Decoration::recalculateBorders);

        // a change in font might cause the borders to change
        connect(s.data(), &KDecoration2::DecorationSettings::fontChanged, this, &Decoration::recalculateBorders);
        connect(s.data(), &KDecoration2::DecorationSettings::spacingChanged, this, &Decoration::recalculateBorders);

        // buttons
        connect(s.data(), &KDecoration2::DecorationSettings::spacingChanged, this, &Decoration::updateButtonsGeometryDelayed);
        connect(s.data(), &KDecoration2::DecorationSettings::decorationButtonsLeftChanged, this, &Decoration::updateButtonsGeometryDelayed);
        connect(s.data(), &KDecoration2::DecorationSettings::decorationButtonsRightChanged, this, &Decoration::updateButtonsGeometryDelayed);

        // full reconfiguration
        connect(s.data(), &KDecoration2::DecorationSettings::reconfigured, this, &Decoration::reconfigure);
        connect(s.data(), &KDecoration2::DecorationSettings::reconfigured, SettingsProvider::self(), &SettingsProvider::reconfigure, Qt::UniqueConnection );

        connect(client().data(), &KDecoration2::DecoratedClient::adjacentScreenEdgesChanged, this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedHorizontallyChanged, this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedVerticallyChanged, this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::captionChanged, this,
            [this]()
            {
                // update the caption area
                update(titleBar());
            }
        );

        connect(client().data(), &KDecoration2::DecoratedClient::activeChanged, this, &Decoration::updateAnimationState);
        connect(client().data(), &KDecoration2::DecoratedClient::activeChanged, this, &Decoration::updateShadow);

        //decoration has an overloaded update function, force the compiler to choose the right one
        connect(client().data(), &KDecoration2::DecoratedClient::paletteChanged,   this,  static_cast<void (Decoration::*)()>(&Decoration::update));
        connect(client().data(), &KDecoration2::DecoratedClient::widthChanged,     this, &Decoration::updateTitleBar);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateTitleBar);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::setOpaque);

        connect(client().data(), &KDecoration2::DecoratedClient::widthChanged,     this, &Decoration::updateButtonsGeometry);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateButtonsGeometry);
        connect(client().data(), &KDecoration2::DecoratedClient::shadedChanged,    this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::shadedChanged,    this, &Decoration::updateButtonsGeometry);

        createButtons();
        updateShadow();

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
    void Decoration::updateAnimationState()
    {
        if( m_internalSettings->animationsEnabled() )
        {

            m_animation->setDirection( client().data()->isActive() ? QPropertyAnimation::Forward : QPropertyAnimation::Backward );
            if( m_animation->state() != QPropertyAnimation::Running ) m_animation->start();

        } else {

            update();

        }
    }

    //________________________________________________________________
    void Decoration::updateSizeGripVisibility()
    {
        auto c = client().data();
        if( m_sizeGrip )
        { m_sizeGrip->setVisible( c->isResizeable() && !isMaximized() && !c->isShaded() ); }
    }

    //________________________________________________________________
    int Decoration::borderSize(bool bottom) const
    {
        const int baseSize = settings()->smallSpacing();
        if( m_internalSettings && (m_internalSettings->mask() & BorderSize ) )
        {
            switch (m_internalSettings->borderSize()) {
                case InternalSettings::BorderNone: return 0;
                case InternalSettings::BorderNoSides: return bottom ? qMax(4, baseSize) : 0;
                default:
                case InternalSettings::BorderTiny: return bottom ? qMax(4, baseSize) : baseSize;
                case InternalSettings::BorderNormal: return baseSize*2;
                case InternalSettings::BorderLarge: return baseSize*3;
                case InternalSettings::BorderVeryLarge: return baseSize*4;
                case InternalSettings::BorderHuge: return baseSize*5;
                case InternalSettings::BorderVeryHuge: return baseSize*6;
                case InternalSettings::BorderOversized: return baseSize*10;
            }

        } else {

            switch (settings()->borderSize()) {
                case KDecoration2::BorderSize::None: return 0;
                case KDecoration2::BorderSize::NoSides: return bottom ? qMax(4, baseSize) : 0;
                default:
                case KDecoration2::BorderSize::Tiny: return bottom ? qMax(4, baseSize) : baseSize;
                case KDecoration2::BorderSize::Normal: return baseSize*2;
                case KDecoration2::BorderSize::Large: return baseSize*3;
                case KDecoration2::BorderSize::VeryLarge: return baseSize*4;
                case KDecoration2::BorderSize::Huge: return baseSize*5;
                case KDecoration2::BorderSize::VeryHuge: return baseSize*6;
                case KDecoration2::BorderSize::Oversized: return baseSize*10;

            }

        }
    }

    //________________________________________________________________
    void Decoration::reconfigure()
    {

        m_internalSettings = SettingsProvider::self()->internalSettings( this );

        // animation
        m_animation->setDuration( m_internalSettings->shadowAnimationsDuration() );

        // borders
        recalculateBorders();

        // clear shadows
        g_sShadows.clear();

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

        // left, right and bottom borders
        auto testFlag = [&]( Qt::Edge edge ) { return edges.testFlag(edge) && !m_internalSettings->drawBorderOnMaximizedWindows(); };
        const int left   = isMaximizedHorizontally() || testFlag(Qt::LeftEdge) ? 0 : borderSize();
        const int right  = isMaximizedHorizontally() || testFlag(Qt::RightEdge) ? 0 : borderSize();
        const int bottom = isMaximizedVertically() || c->isShaded() || testFlag(Qt::BottomEdge) ? 0 : borderSize(true);

        int top = 0;
        if( hideTitleBar() ) top = bottom;
        else {

            QFontMetrics fm(s->font());
            top += qMax(fm.boundingRect(c->caption()).height(), buttonHeight() );

            // padding below
            const int baseSize = settings()->smallSpacing();
            top += baseSize*Metrics::TitleBar_BottomMargin;

            // padding above
            top += baseSize*TitleBar_TopMargin;

        }

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
        updateButtonsGeometry();
    }

    //________________________________________________________________
    void Decoration::updateButtonsGeometryDelayed()
    { QTimer::singleShot( 0, this, &Decoration::updateButtonsGeometry ); }

    //________________________________________________________________
    void Decoration::updateButtonsGeometry()
    {
        auto s = settings();

        // adjust button position
        const int bHeight = captionHeight() + (isMaximized() ? s->smallSpacing()*Metrics::TitleBar_TopMargin:0);
        const int bWidth = buttonHeight();
        const int verticalOffset = (isMaximized() ? s->smallSpacing()*Metrics::TitleBar_TopMargin:0) + (captionHeight()-buttonHeight())/2;
        foreach( const QPointer<KDecoration2::DecorationButton>& button, m_leftButtons->buttons() + m_rightButtons->buttons() )
        {
            button.data()->setGeometry( QRectF( QPoint( 0, 0 ), QSizeF( bWidth, bHeight ) ) );
            static_cast<Button*>( button.data() )->setOffset( QPointF( 0, verticalOffset ) );
            static_cast<Button*>( button.data() )->setIconSize( QSize( bWidth, bWidth ) );
        }

        // left buttons
        if( !m_leftButtons->buttons().isEmpty() )
        {

            // spacing
            m_leftButtons->setSpacing(s->smallSpacing()*Metrics::TitleBar_ButtonSpacing);

            // padding
            const int vPadding = isMaximized() ? 0 : s->smallSpacing()*Metrics::TitleBar_TopMargin;
            const int hPadding = s->smallSpacing()*Metrics::TitleBar_SideMargin;
            if( isMaximizedHorizontally() )
            {
                // add offsets on the side buttons, to preserve padding, but satisfy Fitts law
                auto button = static_cast<Button*>( m_leftButtons->buttons().front().data() );
                button->setGeometry( QRectF( QPoint( 0, 0 ), QSizeF( bWidth + hPadding, bHeight ) ) );
                button->setFlag( Button::FlagFirstInList );
                button->setHorizontalOffset( hPadding );

                m_leftButtons->setPos(QPointF(0, vPadding));

            } else m_leftButtons->setPos(QPointF(hPadding + borderLeft(), vPadding));

        }

        // right buttons
        if( !m_rightButtons->buttons().isEmpty() )
        {

            // spacing
            m_rightButtons->setSpacing(s->smallSpacing()*Metrics::TitleBar_ButtonSpacing);

            // padding
            const int vPadding = isMaximized() ? 0 : s->smallSpacing()*Metrics::TitleBar_TopMargin;
            const int hPadding = s->smallSpacing()*Metrics::TitleBar_SideMargin;
            if( isMaximizedHorizontally() )
            {

                auto button = static_cast<Button*>( m_rightButtons->buttons().back().data() );
                button->setGeometry( QRectF( QPoint( 0, 0 ), QSizeF( bWidth + hPadding, bHeight ) ) );
                button->setFlag( Button::FlagLastInList );

                m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width(), vPadding));

            } else m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width() - hPadding - borderRight(), vPadding));

        }

        update();

    }

    //________________________________________________________________
    void Decoration::paint(QPainter *painter, const QRect &repaintRegion)
    {
        const auto c = client().data();
        const auto palette = c->palette();

        const auto rect = c->isShaded() ? QRect( QPoint(0, 0), QSize(size().width(), borderTop()) ) : this->rect();
        renderWindowBorder(painter, rect, palette);
        if( !isMaximized() ) renderCorners( painter, rect, palette);

        if( !hideTitleBar() )
        {
            m_leftButtons->paint(painter, repaintRegion);
            m_rightButtons->paint(painter, repaintRegion);

            renderTitleText( painter, palette );
        }

    }

    //________________________________________________________________
    int Decoration::buttonHeight() const
    {
        const int baseSize = settings()->gridUnit() + 2; //oxygen icons were always slightly larger
        switch( m_internalSettings->buttonSize() )
        {
            case InternalSettings::ButtonSmall: return baseSize*1.5;
            default:
            case InternalSettings::ButtonDefault: return baseSize*2;
            case InternalSettings::ButtonLarge: return baseSize*2.5;
            case InternalSettings::ButtonVeryLarge: return baseSize*3.5;
        }

    }

    //________________________________________________________________
    int Decoration::captionHeight() const
    { return hideTitleBar() ? borderTop() : borderTop() - settings()->smallSpacing()*(Metrics::TitleBar_BottomMargin + Metrics::TitleBar_TopMargin ); }

    //________________________________________________________________
    QPair<QRect,Qt::Alignment> Decoration::captionRect() const
    {
        if( hideTitleBar() ) return qMakePair( QRect(), Qt::AlignCenter );
        else {

            const int leftOffset = m_leftButtons->buttons().isEmpty() ?
                Metrics::TitleBar_SideMargin*settings()->smallSpacing():
                m_leftButtons->geometry().x() + m_leftButtons->geometry().width() + Metrics::TitleBar_SideMargin*settings()->smallSpacing();

            const int rightOffset = m_rightButtons->buttons().isEmpty() ?
                Metrics::TitleBar_SideMargin*settings()->smallSpacing() :
                size().width() - m_rightButtons->geometry().x() + Metrics::TitleBar_SideMargin*settings()->smallSpacing();

            const int yOffset = settings()->smallSpacing()*Metrics::TitleBar_TopMargin;
            const QRect maxRect( leftOffset, yOffset, size().width() - leftOffset - rightOffset, captionHeight() );

            switch( m_internalSettings->titleAlignment() )
            {
                case InternalSettings::AlignLeft:
                return qMakePair( maxRect, Qt::AlignVCenter|Qt::AlignLeft );

                case InternalSettings::AlignRight:
                return qMakePair( maxRect, Qt::AlignVCenter|Qt::AlignRight );

                case InternalSettings::AlignCenter:
                return qMakePair( maxRect, Qt::AlignCenter );

                default:
                case InternalSettings::AlignCenterFullWidth:
                {

                    // full caption rect
                    const QRect fullRect = QRect( 0, yOffset, size().width(), captionHeight() );
                    QRect boundingRect( settings()->fontMetrics().boundingRect( client().data()->caption()).toRect() );

                    // text bounding rect
                    boundingRect.setTop( yOffset );
                    boundingRect.setHeight( captionHeight() );
                    boundingRect.moveLeft( ( size().width() - boundingRect.width() )/2 );

                    if( boundingRect.left() < leftOffset ) return qMakePair( maxRect, Qt::AlignVCenter|Qt::AlignLeft );
                    else if( boundingRect.right() > size().width() - rightOffset ) return qMakePair( maxRect, Qt::AlignVCenter|Qt::AlignRight );
                    else return qMakePair(fullRect, Qt::AlignCenter);

                }

            }

        }

    }

    //________________________________________________________________
    void Decoration::updateShadow()
    {

        // do nothing if palettes are disabled
        if( !(
            SettingsProvider::self()->shadowCache()->isEnabled( QPalette::Active ) ||
            SettingsProvider::self()->shadowCache()->isEnabled( QPalette::Inactive ) ) )
        { return; }

        // see if shadow should be animated
        const bool animated(
            m_animation->state() == QPropertyAnimation::Running &&
            SettingsProvider::self()->shadowCache()->isEnabled( QPalette::Active ) &&
            SettingsProvider::self()->shadowCache()->isEnabled( QPalette::Inactive )
            );

        // generate key
        ShadowCache::Key key;
        key.active = SettingsProvider::self()->shadowCache()->isEnabled( QPalette::Active ) && client().data()->isActive();
        key.isShade = client().data()->isShaded();
        key.hasBorder = !hasNoBorders();

        if( animated )
        {

            static const int maxIndex = 255;
            key.index = m_opacity * maxIndex;

        }

        const int hash( key.hash() );

        // find key in map
        auto iter = g_sShadows.find( hash );
        if( iter == g_sShadows.end() )
        {

            auto decorationShadow = DecorationShadowPointer::create();
            QPixmap shadowPixmap = animated ?
                SettingsProvider::self()->shadowCache()->animatedPixmap( key, m_opacity ):
                SettingsProvider::self()->shadowCache()->pixmap( key );

            const int shadowSize( shadowPixmap.width()/2 );
            const int overlap = 4;
            decorationShadow->setPadding( QMargins( shadowSize-overlap, shadowSize-overlap, shadowSize-overlap, shadowSize-overlap ) );
            decorationShadow->setInnerShadowRect( QRect( shadowSize, shadowSize, 1, 1 ) );
            decorationShadow->setShadow( shadowPixmap.toImage() );

            iter = g_sShadows.insert( hash, decorationShadow );

        }

        setShadow( iter.value() );

    }

    //_________________________________________________________
    void Decoration::renderCorners( QPainter* painter, const QRect& frame, const QPalette& palette ) const
    {

        const QColor color(palette.color(QPalette::Background));

        QLinearGradient lg = QLinearGradient(0, -0.5, 0, qreal( frame.height() )+0.5);
        lg.setColorAt(0.0, SettingsProvider::self()->helper()->calcLightColor( SettingsProvider::self()->helper()->backgroundTopColor(color) ));
        lg.setColorAt(0.51, SettingsProvider::self()->helper()->backgroundBottomColor(color) );
        lg.setColorAt(1.0, SettingsProvider::self()->helper()->backgroundBottomColor(color) );

        painter->setPen( QPen( lg, 1 ) );
        painter->setBrush( Qt::NoBrush );
        painter->drawRoundedRect( QRectF( frame ).adjusted( 0.5, 0.5, -0.5, -0.5 ), 3.5,  3.5 );
    }

    //_________________________________________________________
    void Decoration::renderWindowBackground( QPainter* painter, const QRect& clipRect, const QPalette& palette ) const
    {

        const auto c = client().data();
        auto innerClientRect = c->isShaded() ?
            QRect(QPoint(0, 0), QSize(size().width(), borderTop())):
            rect();

        // size of window minus the outlines for the rounded corners
        if( settings()->isAlphaChannelSupported() && !isMaximized() )
        { innerClientRect.adjust(1,1,-1,-1); }

        if( SettingsProvider::self()->helper()->hasBackgroundGradient( c->windowId() ) )
        {

            SettingsProvider::self()->helper()->renderWindowBackground(painter, clipRect, innerClientRect, palette.color(QPalette::Window), borderTop()-24 );

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

        // title height
        renderWindowBackground(painter, clipRect, palette );

        // restore painter
        if( clipRect.isValid() )
        { painter->restore(); }

    }

    //_________________________________________________________
    void Decoration::renderTitleText( QPainter* painter, const QPalette& palette ) const
    {

        // setup font
        painter->setFont( settings()->font() );

        // caption rect
        const auto cR = captionRect();

        // copy caption
        const auto c = client().data();
        const QString caption = painter->fontMetrics().elidedText(c->caption(), Qt::ElideMiddle, cR.first.width());

        const auto contrast( titlebarContrastColor( palette ) );
        if( contrast.isValid() )
        {
            painter->setPen( contrast );
            painter->translate( 0, 1 );
            painter->drawText( cR.first, cR.second | Qt::TextSingleLine, caption );
            painter->translate( 0, -1 );
        }

        const auto color( titlebarTextColor( palette ) );
        painter->setPen( color );
        painter->drawText( cR.first, cR.second | Qt::TextSingleLine, caption );

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

        if( c->windowId() != 0 )
        {
            m_sizeGrip = new SizeGrip( this );
            connect( client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateSizeGripVisibility );
            connect( client().data(), &KDecoration2::DecoratedClient::shadedChanged, this, &Decoration::updateSizeGripVisibility );
            connect( client().data(), &KDecoration2::DecoratedClient::resizeableChanged, this, &Decoration::updateSizeGripVisibility );
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
