/*
 * Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * Copyright (c) 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
 * Copyright (c) 2006, 2007 Casper Boemann <cbr@boemann.dk>
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

            return new Button(type, d, parent);

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

        reset(0);

        // setup connections
        if( isMenuButton() )
        { connect(decoration->client().data(), SIGNAL(iconChanged(QIcon)), this, SLOT(update())); }

        connect( this, &KDecoration2::DecorationButton::hoveredChanged, this, &Button::updateAnimationState );

    }

    //_______________________________________________
    Button::Button(QObject *parent, const QVariantList &args)
        : KDecoration2::DecorationButton(args.at(0).value<KDecoration2::DecorationButtonType>(), args.at(1).value<Decoration*>(), parent)
        , m_flag(FlagStandalone)
        , m_animation( new QPropertyAnimation( this ) )
    {}

    //_______________________________________________
    QColor Button::buttonDetailColor(const QPalette &palette) const
    {
        auto d( qobject_cast<Decoration*>( decoration().data() ) );
        if( d->isAnimated() )
        {

            return KColorUtils::mix(
                buttonDetailColor( palette, false ),
                buttonDetailColor( palette, true ),
                d->opacity() );

        } else {

            return buttonDetailColor( palette, isActive());

        }

    }

    //___________________________________________________
    QColor Button::buttonDetailColor( const QPalette& palette, bool active ) const
    { return palette.color( active ? QPalette::Active : QPalette::Disabled, QPalette::ButtonText ); }

    //___________________________________________________
    bool Button::isActive( void ) const
    { return decoration().data()->client().data()->isActive(); }

    //___________________________________________________
    bool Button::buttonAnimationsEnabled( void ) const
    { return static_cast<Decoration*>(decoration().data())->internalSettings()->buttonAnimationsEnabled(); }

    //___________________________________________________
    void Button::reset( unsigned long )
    { m_animation->setDuration( static_cast<Decoration*>(decoration().data())->internalSettings()->buttonAnimationsDuration() ); }

    //___________________________________________________
    void Button::paint( QPainter* painter, const QRect& repaintRegion)
    {
        Q_UNUSED(repaintRegion)

        if (!decoration()) return;

        painter->save();

        // translate from offset
        if( m_flag == FlagFirstInList ) painter->translate( m_offset );
        else painter->translate( 0, m_offset.y() );

        // menu buttons
        if( isMenuButton() )
        {

            const QSizeF iconSize( size().width()-m_offset.x(), size().height()-m_offset.y() );
            const QRectF iconRect( geometry().topLeft(), iconSize );
            const QPixmap pixmap = decoration()->client().data()->icon().pixmap( iconSize.toSize());
            painter->drawPixmap(iconRect.center() - QPoint(pixmap.width()/2, pixmap.height()/2), pixmap);
            return;

        }

        // palette
        QPalette palette( decoration().data()->client().data()->palette() );
        palette.setCurrentColorGroup( isActive() ? QPalette::Active : QPalette::Inactive);

        // base button color
        QColor base = palette.button().color();

        // text color
        QColor color = buttonDetailColor( palette );

        // decide decoration color
        QColor glow;
        if( isAnimated() || isHovered() )
        {
            glow = isCloseButton() ?
                SettingsProvider::self()->helper()->negativeTextColor(palette):
                SettingsProvider::self()->helper()->hoverColor(palette);

            if( isAnimated() )
            {
                color = KColorUtils::mix( color, glow, m_opacity );
                glow = SettingsProvider::self()->helper()->alphaColor( glow, m_opacity );

            } else if( isHovered() ) color = glow;

        }

        // draw button shape
        const bool sunken = isPressed() || ( isToggleButton() && isChecked() );
        const QSizeF iconSize( size().width()-m_offset.x(), size().height()-m_offset.y() );
        const QRectF iconRect( geometry().topLeft(), iconSize );
        const QPixmap pixmap = decoration()->client().data()->icon().pixmap( iconSize.toSize());
        painter->drawPixmap(iconRect.center() - QPoint(pixmap.width()/2, pixmap.height()/2), SettingsProvider::self()->helper()->windecoButton( base, glow, sunken, iconSize.height()) );

        // Icon
        painter->setRenderHints(QPainter::Antialiasing);
        painter->translate( geometry().topLeft() );

        qreal width( 1.2 );

        // contrast
        painter->setBrush(Qt::NoBrush);
        painter->translate(0, 0.5);
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
        const qreal width( geometry().width() - m_offset.x() );
        painter->scale( width/21, width/21 );

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
            painter->drawLine(QPointF( 7.5, 9.5), QPointF(10.5,12.5));
            painter->drawLine(QPointF(10.5,12.5), QPointF(13.5, 9.5));
            break;

            case KDecoration2::DecorationButtonType::Maximize:
            if(decoration()->client().data()->isMaximized())
            {
                painter->translate(1.5, 1.0 );
                QPoint points[4] = {QPoint(9, 6), QPoint(12, 9), QPoint(9, 12), QPoint(6, 9)};
                painter->drawPolygon(points, 4);
                painter->translate(-1.5, -1.0 );
            }
            else
            {
                painter->drawLine(QPointF( 7.5, 11.5 ), QPointF( 10.5, 8.5) );
                painter->drawLine(QPointF( 10.5, 8.5 ), QPointF( 13.5, 11.5) );
            }
            break;

            case KDecoration2::DecorationButtonType::Close:
            painter->drawLine(QPointF( 7.5,7.5), QPointF(13.5,13.5));
            painter->drawLine(QPointF(13.5,7.5), QPointF( 7.5,13.5));
            break;

            case KDecoration2::DecorationButtonType::KeepAbove:
            painter->drawLine(QPointF( 7.5,14), QPointF(10.5,11));
            painter->drawLine(QPointF(10.5,11), QPointF(13.5,14));
            painter->drawLine(QPointF( 7.5,10), QPointF(10.5, 7));
            painter->drawLine(QPointF(10.5, 7), QPointF(13.5,10));
            break;

            case KDecoration2::DecorationButtonType::KeepBelow:
            painter->drawLine(QPointF( 7.5,11), QPointF(10.5,14));
            painter->drawLine(QPointF(10.5,14), QPointF(13.5,11));
            painter->drawLine(QPointF( 7.5, 7), QPointF(10.5,10));
            painter->drawLine(QPointF(10.5,10), QPointF(13.5, 7));
            break;

            case KDecoration2::DecorationButtonType::Shade:
            if (!isChecked())
            {

                // shade button
                painter->drawLine(QPointF( 7.5, 7.5), QPointF(10.5,10.5));
                painter->drawLine(QPointF(10.5,10.5), QPointF(13.5, 7.5));
                painter->drawLine(QPointF( 7.5,13.0), QPointF(13.5,13.0));

            } else {

                // unshade button
                painter->drawLine(QPointF( 7.5,10.5), QPointF(10.5, 7.5));
                painter->drawLine(QPointF(10.5, 7.5), QPointF(13.5,10.5));
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
