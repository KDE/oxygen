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
        , m_colorSettings(client().data()->palette())
        , m_animation( new QPropertyAnimation( this ) )
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
    void Decoration::setOpacity( qreal value )
    {
        if( m_opacity == value ) return;
        m_opacity = value;
        update();

        if( m_sizeGrip ) m_sizeGrip->update();
    }

    //________________________________________________________________
    QColor Decoration::titleBarColor() const
    {

        if( m_animation->state() == QPropertyAnimation::Running )
        {
            return KColorUtils::mix( m_colorSettings.inactiveTitleBar(), m_colorSettings.activeTitleBar(), m_opacity );
        } else return m_colorSettings.titleBar( client().data()->isActive() );

    }

    //________________________________________________________________
    QColor Decoration::outlineColor() const
    {

        if( m_animation->state() == QPropertyAnimation::Running )
        {
            QColor color( client().data()->palette().color( QPalette::Highlight ) );
            color.setAlpha( color.alpha()*m_opacity );
            return color;
        } else if( client().data()->isActive() ) return client().data()->palette().color( QPalette::Highlight );
        else return QColor();
    }

    //________________________________________________________________
    QColor Decoration::fontColor() const
    {

        if( m_animation->state() == QPropertyAnimation::Running )
        {
            return KColorUtils::mix( m_colorSettings.inactiveFont(), m_colorSettings.activeFont(), m_opacity );
        } else return m_colorSettings.font( client().data()->isActive() );

    }

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
        connect(client().data(), &KDecoration2::DecoratedClient::paletteChanged,   this,
            [this]() {
                m_colorSettings.update(client().data()->palette());
                update();
            }
        );
        connect(client().data(), &KDecoration2::DecoratedClient::widthChanged,     this, &Decoration::updateTitleBar);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateTitleBar);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::setOpaque);

        connect(client().data(), &KDecoration2::DecoratedClient::widthChanged,     this, &Decoration::updateButtonPositions);
        connect(client().data(), &KDecoration2::DecoratedClient::maximizedChanged, this, &Decoration::updateButtonPositions);
        connect(client().data(), &KDecoration2::DecoratedClient::shadedChanged,    this, &Decoration::recalculateBorders);
        connect(client().data(), &KDecoration2::DecoratedClient::shadedChanged,    this, &Decoration::updateButtonPositions);

        createButtons();
        createShadow();
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
        }
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

        // read internal settings
        // m_internalSettings->read();

        // animation
        m_animation->setDuration( m_internalSettings->buttonAnimationsDuration() ); //FIXME wrong animation value

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
        // TODO: optimize based on repaintRegion

        // paint background
        if( !client().data()->isShaded() )
        {
            painter->fillRect(rect(), Qt::transparent);
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(Qt::NoPen);
            painter->setBrush(m_colorSettings.frame(client().data()->isActive()));

            // clip away the top part
            painter->setClipRect(0, borderTop(), size().width(), size().height() - borderTop(), Qt::IntersectClip);
            painter->drawRoundedRect(rect(), Metrics::Frame_FrameRadius, Metrics::Frame_FrameRadius);
            painter->restore();
        }

        paintTitleBar(painter, repaintRegion);
    }

    //________________________________________________________________
    void Decoration::paintTitleBar(QPainter *painter, const QRect &repaintRegion)
    {
        const auto c = client().data();
        const QRect titleRect(QPoint(0, 0), QSize(size().width(), borderTop()));

        // render a linear gradient on title area
        const QColor titleBarColor( this->titleBarColor() );
        QLinearGradient gradient( 0, 0, 0, titleRect.height() );
        gradient.setColorAt(0.0, titleBarColor.lighter(100.0));
        gradient.setColorAt(0.8, titleBarColor);

        painter->save();
        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);

        if (isMaximized())
        {

            painter->drawRect(titleRect);

        } else if( c->isShaded() ) {

            painter->drawRoundedRect(titleRect, Metrics::Frame_FrameRadius, Metrics::Frame_FrameRadius);

        } else {

            // we make the rect a little bit larger to be able to clip away the rounded corners on bottom
            painter->setClipRect(titleRect, Qt::IntersectClip);
            painter->drawRoundedRect(titleRect.adjusted(0, 0, 0, Metrics::Frame_FrameRadius), Metrics::Frame_FrameRadius, Metrics::Frame_FrameRadius);

        }

        auto s = settings();

        const QColor outlineColor( this->outlineColor() );
        if( !c->isShaded() && outlineColor.isValid() )
        {
            // outline
            painter->setRenderHint( QPainter::Antialiasing, false );
            painter->setBrush( Qt::NoBrush );
            painter->setPen( outlineColor );
            painter->drawLine( titleRect.bottomLeft(), titleRect.bottomRight() );
        }

        painter->restore();

        // draw caption
        painter->setFont(s->font());
        const QRect cR = captionRect();
        const QString caption = painter->fontMetrics().elidedText(c->caption(), Qt::ElideMiddle, cR.width());
        painter->setPen(m_colorSettings.font(c->isActive()));
        painter->drawText(cR, Qt::AlignVCenter| Qt::AlignLeft | Qt::TextSingleLine, caption);

        // draw all buttons
        m_leftButtons->paint(painter, repaintRegion);
        m_rightButtons->paint(painter, repaintRegion);
    }

    //________________________________________________________________
    int Decoration::buttonHeight() const
    {
        const int baseSize = settings()->gridUnit();
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
        boundingRect.setWidth( boundingRect.width()+4 );

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
