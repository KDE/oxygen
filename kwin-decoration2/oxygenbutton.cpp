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
#include "oxygenbutton.h"

#include <KDecoration2/DecoratedClient>
#include <KColorUtils>

#include <QPainter>

namespace Oxygen
{

    //__________________________________________________________________
    Button::Button(KDecoration2::DecorationButtonType type, Decoration* decoration, QObject* parent)
        : DecorationButton(type, decoration, parent)
        , m_animation( new QPropertyAnimation( this ) )
    {

        // setup animation
        m_animation->setStartValue( 0 );
        m_animation->setEndValue( 1.0 );
        m_animation->setTargetObject( this );
        m_animation->setPropertyName( "opacity" );
        m_animation->setEasingCurve( QEasingCurve::InOutQuad );

        // setup geometry
        const int height = decoration->buttonHeight();
        setGeometry(QRect(0, 0, height, height));
        connect(decoration, &Decoration::bordersChanged, this, [this, decoration]
        {
            const int height = decoration->buttonHeight();
            if (height == geometry().height()) return;
            setGeometry(QRectF(geometry().topLeft(), QSizeF(height, height)));
        });

        // connect hover state changed
        connect( this, &KDecoration2::DecorationButton::hoveredChanged, this, &Button::updateAnimationState );

    }

    //__________________________________________________________________
    Button::Button(QObject *parent, const QVariantList &args)
        : DecorationButton(args.at(0).value<KDecoration2::DecorationButtonType>(), args.at(1).value<Decoration*>(), parent)
        , m_standalone(true)
        , m_animation( new QPropertyAnimation( this ) )
    {}

    //__________________________________________________________________
    Button *Button::create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent)
    {
        if (auto d = qobject_cast<Decoration*>(decoration))
        {
            Button *b = new Button(type, d, parent);
            if (type == KDecoration2::DecorationButtonType::Menu)
            {
                QObject::connect(d->client().data(), &KDecoration2::DecoratedClient::iconChanged, b, [b]() { b->update(); });
            }
            return b;
        }
        return nullptr;
    }

    //__________________________________________________________________
    Button::~Button() = default;

    //__________________________________________________________________
    void Button::paint(QPainter *painter, const QRect &repaintRegion)
    {
        Q_UNUSED(repaintRegion)

        if (!decoration()) return;

        if (type() == KDecoration2::DecorationButtonType::Menu)
        {
            const QPixmap pixmap = decoration()->client().data()->icon().pixmap(size().toSize());
            painter->drawPixmap(geometry().center() - QPoint(pixmap.width()/2, pixmap.height()/2), pixmap);

        } else {

            drawIcon( painter );

        }

    }

    //__________________________________________________________________
    void Button::drawIcon( QPainter *painter ) const
    {

        painter->save();
        painter->setRenderHints( QPainter::Antialiasing );

        /*
        scale painter so that its window matches QRect( -1, -1, 20, 20 )
        this makes all further rendering and scaling simpler
        all further rendering is preformed inside QRect( 0, 0, 18, 18 )
        */
        painter->translate( geometry().topLeft() );
        painter->scale( geometry().width()/20, geometry().height()/20 );
        painter->translate( 1, 1 );

        // render background
        const QColor backgroundColor( this->backgroundColor() );
        if( backgroundColor.isValid() )
        {
            painter->setPen( Qt::NoPen );
            painter->setBrush( backgroundColor );
            painter->drawEllipse( QRectF( 0, 0, 18, 18 ) );
        }

        // render mark
        const QColor foregroundColor( this->foregroundColor() );
        if( foregroundColor.isValid() )
        {

            // setup painter
            QPen pen( foregroundColor );
            pen.setCapStyle( Qt::RoundCap );
            pen.setJoinStyle( Qt::MiterJoin );
            const qreal penWidth( 1 );
            pen.setWidth( penWidth*2 );

            painter->setPen( pen );
            painter->setBrush( Qt::NoBrush );

            switch( type() )
            {

                case KDecoration2::DecorationButtonType::Close:
                {
                    painter->drawLine( QPointF( 5 + penWidth, 5 + penWidth ), QPointF( 13 - penWidth, 13 - penWidth ) );
                    painter->drawLine( 13 - penWidth, 5 + penWidth, 5 + penWidth, 13 - penWidth );
                    break;
                }

                case KDecoration2::DecorationButtonType::Maximize:
                {
                    painter->drawLine(QPointF( 7.5, 11.5 ), QPointF( 10.5, 8.5) );
                    painter->drawLine(QPointF( 10.5, 8.5 ), QPointF( 13.5, 11.5) );
                }

                case KDecoration2::DecorationButtonType::Minimize:
                {

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 3.5 + penWidth, 6.5 + penWidth )
                        << QPointF( 9, 12.5 - penWidth )
                        << QPointF( 14.5 - penWidth, 6.5 + penWidth ) );
                    break;

                }

                case KDecoration2::DecorationButtonType::OnAllDesktops:
                {
                    painter->setPen( Qt::NoPen );
                    painter->setBrush( foregroundColor );

                    if( isChecked())
                    {

                        // outer ring
                        painter->drawEllipse( QRectF( 3, 3, 12, 12 ) );

                        // center dot
                        QColor backgroundColor( this->backgroundColor() );
                        auto d = qobject_cast<Decoration*>( decoration() );
                        if( !backgroundColor.isValid() && d ) backgroundColor = d->titleBarColor();

                        if( backgroundColor.isValid() )
                        {
                            painter->setBrush( backgroundColor );
                            painter->drawEllipse( QRectF( 8, 8, 2, 2 ) );
                        }

                    } else {

                        painter->drawRoundedRect( QRectF( 6, 2, 6, 9 ), 1.5, 1.5 );
                        painter->drawRect( QRectF( 4, 10, 10, 2 ) );
                        painter->drawRoundRect( QRectF( 8, 12, 2, 4 ) );

                    }
                    break;
                }

                case KDecoration2::DecorationButtonType::Shade:
                {

                    if (isChecked())
                    {

                        painter->drawLine( 3 + penWidth, 5.5 + penWidth, 15 - penWidth, 5.5+penWidth );
                        painter->drawPolyline( QPolygonF()
                            << QPointF( 3.5 + penWidth, 8.5 + penWidth )
                            << QPointF( 9, 14.5 - penWidth )
                            << QPointF( 14.5 - penWidth, 8.5 + penWidth ) );

                    } else {

                        painter->drawLine( 3 + penWidth, 5.5 + penWidth, 15 - penWidth, 5.5+penWidth );
                        painter->drawPolyline( QPolygonF()
                            << QPointF( 3.5 + penWidth, 14.5 - penWidth )
                            << QPointF( 9, 8.5 + penWidth )
                            << QPointF( 14.5 - penWidth, 14.5 - penWidth ) );
                    }

                    break;

                }

                case KDecoration2::DecorationButtonType::KeepBelow:
                {

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 3.5 + penWidth, 4.5 + penWidth )
                        << QPointF( 9, 10.5 - penWidth )
                        << QPointF( 14.5 - penWidth, 4.5 + penWidth ) );

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 3.5 + penWidth, 8.5 + penWidth )
                        << QPointF( 9, 14.5 - penWidth )
                        << QPointF( 14.5 - penWidth, 8.5 + penWidth ) );
                    break;

                }

                case KDecoration2::DecorationButtonType::KeepAbove:
                {
                    painter->drawPolyline( QPolygonF()
                        << QPointF( 3.5 + penWidth, 9.5 - penWidth )
                        << QPointF( 9, 3.5 + penWidth )
                        << QPointF( 14.5 - penWidth, 9.5 - penWidth ) );

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 3.5 + penWidth, 13.5 - penWidth )
                        << QPointF( 9, 7.5 + penWidth )
                        << QPointF( 14.5 - penWidth, 13.5 - penWidth ) );
                    break;
                }

                case KDecoration2::DecorationButtonType::ApplicationMenu:
                {
                    painter->drawLine( QPointF( 3.5, 5 ), QPointF( 14.5, 5 ) );
                    painter->drawLine( QPointF( 3.5, 9 ), QPointF( 14.5, 9 ) );
                    painter->drawLine( QPointF( 3.5, 13 ), QPointF( 14.5, 13 ) );
                    break;
                }

                case KDecoration2::DecorationButtonType::ContextHelp:
                {
                    QPainterPath path;
                    path.moveTo( 5, 6 );
                    path.arcTo( QRectF( 5, 3.5, 8, 5 ), 180, -180 );
                    path.cubicTo( QPointF(12.5, 9.5), QPointF( 9, 7.5 ), QPointF( 9, 11.5 ) );
                    painter->drawPath( path );

                    painter->drawPoint( 9, 15 );

                    break;
                }

                default: break;

            }

        }

        painter->restore();




    }

    //__________________________________________________________________
    QColor Button::foregroundColor( void ) const
    {

        auto d = qobject_cast<Decoration*>( decoration() );
        if( !d ) return QColor();
        if( type() == KDecoration2::DecorationButtonType::Close ) {

            return d->titleBarColor();

        } else if( ( type() == KDecoration2::DecorationButtonType::KeepBelow || type() == KDecoration2::DecorationButtonType::KeepAbove ) && isChecked() ) {

            return d->titleBarColor();

        } else if( m_animation->state() == QPropertyAnimation::Running ) {

            return KColorUtils::mix( d->fontColor(), d->titleBarColor(), m_opacity );

        } else if( isHovered() ) {

            return d->titleBarColor();

        } else {

            return d->fontColor();

        }

    }

    //__________________________________________________________________
    QColor Button::backgroundColor( void ) const
    {
        auto d = qobject_cast<Decoration*>( decoration() );
        if( !d ) return QColor();
        if( isPressed() )
        {

            if( type() == KDecoration2::DecorationButtonType::Close ) return d->colorSettings().closeButtonColor();
            else return KColorUtils::mix( d->titleBarColor(), d->fontColor(), 0.3 );

        } else if( ( type() == KDecoration2::DecorationButtonType::KeepBelow || type() == KDecoration2::DecorationButtonType::KeepAbove ) && isChecked() ) {

            return d->fontColor();

        } else if( m_animation->state() == QPropertyAnimation::Running ) {

            if( type() == KDecoration2::DecorationButtonType::Close )
            {

                return KColorUtils::mix( d->fontColor(), d->colorSettings().closeButtonColor().lighter(), m_opacity );

            } else {

                QColor color( d->fontColor() );
                color.setAlpha( color.alpha()*m_opacity );
                return color;

            }

        } else if( isHovered() ) {

            if( type() == KDecoration2::DecorationButtonType::Close ) return d->colorSettings().closeButtonColor().lighter();
            else return d->fontColor();

        } else if( type() == KDecoration2::DecorationButtonType::Close ) {

            return d->fontColor();

        } else {

            return QColor();

        }

    }

    //__________________________________________________________________
    void Button::updateAnimationState( bool hovered )
    {

        auto d = qobject_cast<Decoration*>(decoration());
        if( !(d && d->internalSettings()->animationsEnabled() ) ) return;

        m_animation->setDirection( hovered ? QPropertyAnimation::Forward : QPropertyAnimation::Backward );
        if( m_animation->state() != QPropertyAnimation::Running ) m_animation->start();

   }

} // namespace
