/*
    SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
    SPDX-FileCopyrightText: 2006, 2007 Casper Boemann <cbr@boemann.dk>
    SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "oxygenbutton.h"
#include "oxygensettingsprovider.h"

#include <KDecoration2/DecoratedClient>
#include <KColorUtils>
#include <KColorScheme>

#include <QPainter>

namespace Oxygen
{

    //____________________________________________________________________________________
    Button* Button::create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration* decoration, QObject* parent)
    {
        if (auto d = qobject_cast<Decoration*>(decoration))
        {

            const auto clientPtr = d->client().toStrongRef();
            Button *b = new Button(type, d, parent);
            switch( type )
            {

                case KDecoration2::DecorationButtonType::Close:
                b->setVisible( clientPtr->isCloseable() );
                QObject::connect(clientPtr.data(), &KDecoration2::DecoratedClient::closeableChanged, b, &Oxygen::Button::setVisible );
                break;

                case KDecoration2::DecorationButtonType::Maximize:
                b->setVisible( clientPtr->isMaximizeable() );
                QObject::connect(clientPtr.data(), &KDecoration2::DecoratedClient::maximizeableChanged, b, &Oxygen::Button::setVisible );
                break;

                case KDecoration2::DecorationButtonType::Minimize:
                b->setVisible( clientPtr->isMinimizeable() );
                QObject::connect(clientPtr.data(), &KDecoration2::DecoratedClient::minimizeableChanged, b, &Oxygen::Button::setVisible );
                break;

                case KDecoration2::DecorationButtonType::ContextHelp:
                b->setVisible( clientPtr->providesContextHelp() );
                QObject::connect(clientPtr.data(), &KDecoration2::DecoratedClient::providesContextHelpChanged, b, &Oxygen::Button::setVisible );
                break;

                case KDecoration2::DecorationButtonType::Shade:
                b->setVisible( clientPtr->isShadeable() );
                QObject::connect(clientPtr.data(), &KDecoration2::DecoratedClient::shadeableChanged, b, &Oxygen::Button::setVisible );
                break;

                case KDecoration2::DecorationButtonType::Menu:
                QObject::connect(clientPtr.data(), &KDecoration2::DecoratedClient::iconChanged, b, [b]() { b->update(); });
                break;

                default: break;

            }

            return b;

        } else return nullptr;
    }

    //____________________________________________________________________________________
    Button::Button(KDecoration2::DecorationButtonType type, Decoration* decoration, QObject* parent):
        KDecoration2::DecorationButton(type, decoration, parent)
        , m_animation( new QPropertyAnimation( this ) )
        , m_opacity(0)
    {

        // setup animation
        m_animation->setStartValue( 0 );
        m_animation->setEndValue( 1.0 );
        m_animation->setTargetObject( this );
        m_animation->setPropertyName( "opacity" );
        m_animation->setEasingCurve( QEasingCurve::InOutQuad );

        // setup default geometry
        const int height = decoration->buttonHeight();
        setGeometry(QRect(0, 0, height, height));
        setIconSize(QSize( height, height ));

        reconfigure();

        // setup connections
        if( isMenuButton() )
        { connect(decoration->client().toStrongRef().data(), SIGNAL(iconChanged(QIcon)), this, SLOT(update())); }

        connect(decoration->settings().data(), &KDecoration2::DecorationSettings::reconfigured, this, &Button::reconfigure);
        connect( this, &KDecoration2::DecorationButton::hoveredChanged, this, &Button::updateAnimationState );

    }

    //_______________________________________________
    Button::Button(QObject *parent, const QVariantList &args)
        : Button(args.at(0).value<KDecoration2::DecorationButtonType>(), args.at(1).value<Decoration*>(), parent)
    {
        m_flag = FlagStandalone;
        //! icon size must return to !valid because it was altered from the default constructor,
        //! in Standalone mode the button is not using the decoration metrics but its geometry
        m_iconSize = QSize(-1, -1);
    }

    //_______________________________________________
    QColor Button::foregroundColor(const QPalette &palette) const
    {
        auto d( qobject_cast<Decoration*>( decoration().data() ) );
        if( d->isAnimated() )
        {

            return KColorUtils::mix(
                foregroundColor( palette, false ),
                foregroundColor( palette, true ),
                d->opacity() );

        } else {

            return foregroundColor( palette, isActive());

        }

    }

    //___________________________________________________
    QColor Button::foregroundColor( const QPalette& palette, bool active ) const
    {
        auto d( qobject_cast<Decoration*>( decoration().data() ) );
        if( d->internalSettings()->useWindowColors() )
        {

            return palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::ButtonText );

        } else {

            return d->fontColor( palette, active );

        }

    }

    //_______________________________________________
    QColor Button::backgroundColor(const QPalette &palette) const
    {
        auto d( qobject_cast<Decoration*>( decoration().data() ) );
        if( d->isAnimated() )
        {

            return KColorUtils::mix(
                backgroundColor( palette, false ),
                backgroundColor( palette, true ),
                d->opacity() );

        } else {

            return backgroundColor( palette, isActive());

        }

    }

    //___________________________________________________
    QColor Button::backgroundColor( const QPalette& palette, bool active ) const
    {
        auto d( qobject_cast<Decoration*>( decoration().data() ) );
        if( d->internalSettings()->useWindowColors() )
        {

            return palette.color( active ? QPalette::Active : QPalette::Inactive, QPalette::Button );

        } else {

            return d->titleBarColor( palette, active );

        }

    }

    //___________________________________________________
    bool Button::isActive( void ) const
    { return decoration().data()->client().toStrongRef()->isActive(); }

    //___________________________________________________
    void Button::reconfigure( void )
    {

        // animation
        auto d = qobject_cast<Decoration*>(decoration());
        if( d ) m_animation->setDuration( d->internalSettings()->buttonAnimationsDuration() );

    }

    //___________________________________________________
    void Button::paint( QPainter* painter, const QRect& repaintRegion)
    {
        Q_UNUSED(repaintRegion)

        if (!decoration()) return;

        painter->save();

        // translate from offset
        if( m_flag == FlagFirstInList ) painter->translate( m_offset );
        else painter->translate( 0, m_offset.y() );

        if( !m_iconSize.isValid() || isStandAlone() ) m_iconSize = geometry().size().toSize();

        const auto clientPtr = decoration()->client().toStrongRef();
        // menu buttons
        if( isMenuButton() )
        {

            const QRectF iconRect( geometry().topLeft(), m_iconSize  );
            clientPtr->icon().paint(painter, iconRect.toRect());
            painter->restore();
            return;

        }

        // palette
        QPalette palette( clientPtr->palette() );
        palette.setCurrentColorGroup( isActive() ? QPalette::Active : QPalette::Inactive);

        // base button color
        QColor base = backgroundColor( palette );

        // text color
        QColor color = foregroundColor( palette );

        // decide decoration color
        QColor glow;
        if( isAnimated() || isHovered() || ( isToggleButton() && isChecked() ) )
        {

            QColor toggleColor = SettingsProvider::self()->helper()->focusColor(palette);
            QColor toggledHoverGlow = foregroundColor( palette, false );
            QColor toggledHoverColor = KColorUtils::mix( toggledHoverColor, color, 0.6 );

            if( isCloseButton() ) glow = SettingsProvider::self()->helper()->negativeTextColor(palette);    // Button is close button
            else if( isHovered() && ( isToggleButton() && isChecked() ) ) glow = toggledHoverGlow;          // Button is checked and hovered
            else if( isToggleButton() && isChecked() ) glow = toggleColor;                                  // Button is checked but not hovered
            else glow = SettingsProvider::self()->helper()->hoverColor(palette);                            // Button is hovered but not checked

            if( isAnimated() )
            {
                if( isToggleButton() && isChecked() ) {
                    color = KColorUtils::mix( toggleColor, toggledHoverColor, m_opacity );
                    glow = KColorUtils::mix( toggleColor, toggledHoverGlow, m_opacity );
                } else {
                    color = KColorUtils::mix( color, glow, m_opacity );
                    glow = SettingsProvider::self()->helper()->alphaColor( glow, m_opacity );
                }

            } else if ( ! isHovered() != !( isToggleButton() && isChecked() ) ) color = glow;               // If button is eigther hovered or checked, use glow color as text color
            else if ( ( isToggleButton() && isChecked() ) ) color = toggledHoverColor;                      // If button is checked and hovered, use different color

        }

        // draw button shape
        const bool sunken = isPressed() || ( isToggleButton() && isChecked() );
        const QRectF iconRect( geometry().topLeft(), m_iconSize  );
        painter->drawPixmap(iconRect.topLeft(), SettingsProvider::self()->helper()->windecoButton( base, glow, sunken, m_iconSize.width() ) );

        // Icon
        painter->setRenderHints(QPainter::Antialiasing);
        painter->translate( geometry().topLeft() );

        qreal width( 1.2 );

        // contrast
        painter->setBrush(Qt::NoBrush);
        painter->translate(0, 1.5);
        painter->setPen(QPen( SettingsProvider::self()->helper()->calcLightColor( base ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        drawIcon(painter);

        // main
        painter->translate(0,-1.5);
        painter->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        drawIcon(painter);

        painter->restore();
    }

    //___________________________________________________
    void Button::drawIcon( QPainter* painter )
    {
        painter->save();

        //keep all co-ordinates between 0 and 21
        const qreal width( m_iconSize.width() );
        painter->scale( width/21, width/21 );

        // make sure pen width is always larger than 1.1 in "real" coordinates
        QPen pen( painter->pen() );
        pen.setWidthF( qMax( 1.1*21/width, pen.widthF() ) );
        painter->setPen( pen );

        switch(type())
        {

            case KDecoration2::DecorationButtonType::OnAllDesktops:
            painter->drawPoint(QPointF(10.5,10.5));
            break;

            case KDecoration2::DecorationButtonType::ContextHelp:
            painter->translate(1.5, 1.5);
            painter->drawArc(7,5,4,4,135*16, -180*16);
            painter->drawArc(9,8,4,4,135*16,45*16);
            painter->drawPoint(9,12);
            painter->translate(-1.5, -1.5);
            break;

            case KDecoration2::DecorationButtonType::ApplicationMenu:
            painter->drawLine(QPointF(7.5, 7.5), QPointF(13.5, 7.5));
            painter->drawLine(QPointF(7.5, 10.5), QPointF(13.5, 10.5));
            painter->drawLine(QPointF(7.5, 13.5), QPointF(13.5, 13.5));
            break;

            case KDecoration2::DecorationButtonType::Minimize:
            {
                painter->drawPolyline( QPolygonF()
                    << QPointF(7.5, 9.5)
                    << QPointF(10.5, 12.5)
                    << QPointF(13.5, 9.5) );
                break;
            }

            case KDecoration2::DecorationButtonType::Maximize:
            if(decoration()->client().toStrongRef()->isMaximized())
            {

                painter->drawPolygon( QPolygonF()
                    << QPointF(7.5, 10.5)
                    << QPointF(10.5, 7.5)
                    << QPointF(13.5, 10.5)
                    << QPointF(10.5, 13.5) );

            } else {

                painter->drawPolyline( QPolygonF()
                    << QPointF(7.5, 11.5)
                    << QPointF(10.5, 8.5)
                    << QPointF(13.5, 11.5) );

            }
            break;

            case KDecoration2::DecorationButtonType::Close:
            painter->drawLine(QPointF( 7.5,7.5), QPointF(13.5,13.5));
            painter->drawLine(QPointF(13.5,7.5), QPointF( 7.5,13.5));
            break;

            case KDecoration2::DecorationButtonType::KeepAbove:
            {
                painter->drawPolyline( QPolygonF()
                    << QPointF( 7.5, 14)
                    << QPointF(10.5, 11)
                    << QPointF(13.5, 14) );

                painter->drawPolyline( QPolygonF()
                    << QPointF(7.5, 10)
                    << QPointF(10.5, 7)
                    << QPointF(13.5, 10) );
                break;

            }

            case KDecoration2::DecorationButtonType::KeepBelow:
            {
                painter->drawPolyline( QPolygonF()
                    << QPointF( 7.5, 11)
                    << QPointF(10.5, 14)
                    << QPointF(13.5, 11) );

                painter->drawPolyline( QPolygonF()
                    << QPointF(7.5, 7)
                    << QPointF(10.5, 10)
                    << QPointF(13.5, 7) );

                break;
            }

            case KDecoration2::DecorationButtonType::Shade:
            if (!isChecked())
            {

                // shade button
                painter->drawPolyline( QPolygonF()
                        << QPointF(7.5, 7.5)
                        << QPointF(10.5, 10.5)
                        << QPointF(13.5, 7.5) );

                painter->drawLine(QPointF( 7.5,13.0), QPointF(13.5,13.0));

            } else {

                painter->drawPolyline( QPolygonF()
                    << QPointF(7.5, 10.5)
                    << QPointF(10.5, 7.5)
                    << QPointF(13.5, 10.5) );

                painter->drawLine(QPointF( 7.5,13.0), QPointF(13.5,13.0));

            }
            break;

            default:
            break;
        }
        painter->restore();
        return;
    }

    //__________________________________________________________________
    void Button::updateAnimationState( bool hovered )
    {

        auto d = qobject_cast<Decoration*>(decoration());
        if( !(d && d->internalSettings()->animationsEnabled() ) ) return;

        m_animation->setDirection( hovered ? QPropertyAnimation::Forward : QPropertyAnimation::Backward );
        if( m_animation->state() != QPropertyAnimation::Running ) m_animation->start();

    }

}
