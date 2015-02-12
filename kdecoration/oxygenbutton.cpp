//////////////////////////////////////////////////////////////////////////////
// Button.cpp
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright (c) 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
// Copyright (c) 2006, 2007 Casper Boemann <cbr@boemann.dk>
// Copyright (c) 2015 David Edmundson <davidedmundson@kde.org>
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

#include "oxygenbutton.h"

#include "oxygen.h"
#include "oxygendecoration.h"
#include "oxygensettingsprovider.h"

#include <cmath>

#include <QPainter>
#include <QPen>

#include <KColorUtils>
#include <KColorScheme>

#include <kdecoration2/decorationbutton.h>
#include <kdecoration2/decoration.h>
#include <kdecoration2/decoratedclient.h>

namespace Oxygen
{
    Button* Button::create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration* decoration, QObject* parent)
    {
        return new Button(type, decoration, parent);
    }

    Button::Button(KDecoration2::DecorationButtonType type, KDecoration2::Decoration* decoration, QObject* parent):
        KDecoration2::DecorationButton(type, decoration, parent),
        _glowAnimation( new Animation( 150, this ) ),
        _glowIntensity(0),
        m_internalSettings(qobject_cast<Decoration*>(decoration)->internalSettings())
    {
        //setup geometry
        setGeometry(QRectF(QPointF(0, 0), sizeHint()));

        // setup animation
        _glowAnimation->setStartValue( 0 );
        _glowAnimation->setEndValue( 1.0 );
        _glowAnimation->setTargetObject( this );
        _glowAnimation->setPropertyName( "glowIntensity" );
        _glowAnimation->setEasingCurve( QEasingCurve::InOutQuad );
        // setup connections
        reset(0);

        if( isMenuButton() ) {
            connect(decoration->client().data(), SIGNAL(iconChanged(QIcon)), this, SLOT(update()));
        }


        connect(this, &DecorationButton::hoveredChanged, this, [this](bool hovered){
            if( buttonAnimationsEnabled() && hasDecoration() ) {
                _glowAnimation->setDirection( hovered ? Animation::Forward : Animation::Backward );
                    if( !isAnimated() ) _glowAnimation->start();
            }
        });
    }

    Button::Button(QObject *parent, const QVariantList &args)
        : KDecoration2::DecorationButton(args.at(0).value<KDecoration2::DecorationButtonType>(), args.at(1).value<Decoration*>(), parent),
        _glowAnimation( new Animation( 150, this ) ),
        _glowIntensity(0)
    {
        //weird standalone mode that is going to crash
    }

    //_______________________________________________
    Button::~Button()
    {}

    //_______________________________________________
    QColor Button::buttonDetailColor(const QPalette &palette) const
    {
        if( m_internalSettings->animationsEnabled() ) return KColorUtils::mix(
            buttonDetailColor( palette, false ),
            buttonDetailColor( palette, true ),
            16 ); //FIXME
        else return buttonDetailColor( palette, isActive());
    }

    //___________________________________________________
    QColor Button::buttonDetailColor( const QPalette& palette, bool active ) const
    {
        if( type() == KDecoration2::DecorationButtonType::Close)
        {

            return active ?
                palette.color(QPalette::Active, QPalette::WindowText ):
                DecoHelper::self()->inactiveTitleBarTextColor( palette );

        } else {

            return active ?
                palette.color(QPalette::Active, QPalette::ButtonText ):
                DecoHelper::self()->inactiveButtonTextColor( palette );

        }

    }

    //___________________________________________________
    bool Button::isActive( void ) const
    { return decoration().data()->client().data()->isActive(); }

    //___________________________________________________
    bool Button::buttonAnimationsEnabled( void ) const
    { return m_internalSettings->buttonAnimationsEnabled(); }

    //___________________________________________________
    QSize Button::sizeHint() const
    {
        const int baseSize = decoration()->settings()->gridUnit() + 2; //kde4 oxygen buttons were 18px, make them match
        unsigned int size = 0;
        switch( m_internalSettings->buttonSize() )
        {
            case Oxygen::InternalSettings::ButtonSmall: size = baseSize*1.5; break;
            default:
            case Oxygen::InternalSettings::ButtonDefault: size = baseSize*2; break;
            case Oxygen::InternalSettings::ButtonLarge: size = baseSize*2.5; break;
            case Oxygen::InternalSettings::ButtonVeryLarge: size = baseSize*3.5; break;
        }
        return QSize( size, size );
    }

    //___________________________________________________
    void Button::reset( unsigned long )
    { _glowAnimation->setDuration( m_internalSettings->buttonAnimationsDuration() ); }

    //___________________________________________________
    void Button::paint( QPainter* painter, const QRect &repaintArea )
    {
        painter->save();
        painter->translate(geometry().topLeft());

        QPalette palette( decoration().data()->client().data()->palette() );

        palette.setCurrentColorGroup( isActive() ? QPalette::Active : QPalette::Inactive);

        //translate buttons down if window maximized
        if( decoration().data()->client().data()->isMaximized() ) painter->translate( 0, 1 );

        // base button color
        QColor base = palette.button().color();

        // text color
        QColor color = buttonDetailColor( palette );

        // decide decoration color
        QColor glow;
        if( isAnimated() || (isHovered()) )
        {
            glow = isCloseButton() ?
                DecoHelper::self()->negativeTextColor(palette):
                DecoHelper::self()->hoverColor(palette);

            if( isAnimated() )
            {
                color = KColorUtils::mix( color, glow, glowIntensity() );
                glow = DecoHelper::self()->alphaColor( glow, glowIntensity() );

            } else if( isHovered() ) color = glow;

        }

        if( hasDecoration() )
        {
            // draw button shape
            painter->drawPixmap(0, 0, DecoHelper::self()->windecoButton( base, glow, isPressed(), sizeHint().height()) );
        }

        // Icon
        // for menu button the application icon is used
        if( isMenuButton() )
        {
            const QPixmap pixmap = decoration()->client().data()->icon().pixmap(size().toSize());
            const double offset = 0;//FIXME ? 0.5*(width()-pixmap.width() );
            painter->drawPixmap(offset, offset-1, pixmap );

        } else {

            painter->setRenderHints(QPainter::Antialiasing);
            qreal width( 1.2 );

            // contrast
            painter->setBrush(Qt::NoBrush);
            painter->translate(0, 0.5);
            painter->setPen(QPen( DecoHelper::self()->calcLightColor( base ), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            drawIcon(painter);

            // main
            painter->translate(0,-1.5);
            painter->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            drawIcon(painter);
        }
        painter->restore();
    }

    //___________________________________________________
    void Button::drawIcon( QPainter* painter )
    {
        painter->save();

        //keep all co-ordinates between 0 and 21
        painter->scale(geometry().width()/21.0, geometry().height()/21.0);

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
}
