//////////////////////////////////////////////////////////////////////////////
// Button.cpp
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright (c) 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
// Copyright (c) 2006, 2007 Casper Boemann <cbr@boemann.dk>
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
#include "oxygenbutton.moc"
#include "oxygenclient.h"

#include <cmath>

#include <QPainter>
#include <QPen>

#include <KColorUtils>
#include <KColorScheme>
#include <kcommondecoration.h>

namespace Oxygen
{
    //_______________________________________________
    Button::Button(
        Client &parent,
        const QString& tip,
        ButtonType type):
        KCommonDecorationButton((::ButtonType)type, &parent),
        _client(parent),
        _helper( parent.helper() ),
        _type(type),
        _status( 0 ),
        _forceInactive( false ),
        _glowAnimation( new Animation( 150, this ) ),
        _glowIntensity(0)
    {
        setAutoFillBackground(false);
        setAttribute(Qt::WA_NoSystemBackground);

        int size( _client.buttonSize() );
        setFixedSize( size, size );

        setCursor(Qt::ArrowCursor);
        setToolTip(tip);

        // setup animation
        _glowAnimation->setStartValue( 0 );
        _glowAnimation->setEndValue( 1.0 );
        _glowAnimation->setTargetObject( this );
        _glowAnimation->setPropertyName( "glowIntensity" );
        _glowAnimation->setEasingCurve( QEasingCurve::InOutQuad );

        // setup connections
        reset(0);


    }

    //_______________________________________________
    Button::~Button()
    {}

    //_______________________________________________
    QColor Button::buttonDetailColor(const QPalette &palette) const
    {
        if( _client.glowIsAnimated() && !_forceInactive && !_client.isForcedActive()) return KColorUtils::mix(
            buttonDetailColor( palette, false ),
            buttonDetailColor( palette, true ),
            _client.glowIntensity() );
        else return buttonDetailColor( palette, isActive() || _client.isForcedActive() );
    }

    //___________________________________________________
    QColor Button::buttonDetailColor( const QPalette& palette, bool active ) const
    {
        if( _type == ButtonItemClose )
        {

            return active ?
                palette.color(QPalette::Active, QPalette::WindowText ):
                _helper.inactiveTitleBarTextColor( palette );

        } else {

            return active ?
                palette.color(QPalette::Active, QPalette::ButtonText ):
                _helper.inactiveButtonTextColor( palette );

        }

    }

    //___________________________________________________
    bool Button::isActive( void ) const
    { return (!_forceInactive) && _client.isActive(); }

    //___________________________________________________
    bool Button::buttonAnimationsEnabled( void ) const
    { return _client.animationsEnabled() && _client.configuration()->buttonAnimationsEnabled(); }

    //___________________________________________________
    QSize Button::sizeHint() const
    {
        unsigned int size( _client.buttonSize() );
        return QSize( size, size );
    }

    //___________________________________________________
    void Button::reset( unsigned long )
    { _glowAnimation->setDuration( _client.configuration()->buttonAnimationsDuration() ); }

    //___________________________________________________
    void Button::paint( QPainter& painter )
    {

        #if OXYGEN_USE_KDE4
        QPalette palette( this->palette() );
        #else
        QPalette palette( _client.palette() );
        #endif

        palette.setCurrentColorGroup( isActive() ? QPalette::Active : QPalette::Inactive);

        if(
            _client.compositingActive() &&
            !( _client.isMaximized() || _type == ButtonItemClose || _type == ButtonItemMenu ) )
        { painter.translate( 0, -1 ); }

        // translate buttons down if window maximized
        if( _client.isMaximized() ) painter.translate( 0, 1 );

        QColor foreground = _client.backgroundPalette( this, palette ).color( QPalette::WindowText );
        QColor background = _client.backgroundPalette( this, palette ).color( QPalette::Window );

        const bool mouseOver( _status&Hovered );

        if( _type == ButtonItemClose || _type == ButtonClose )
        {

            qSwap( foreground, background );
            if( isAnimated() ) background = KColorUtils::mix( background, _helper.negativeTextColor(palette), glowIntensity() );
            else if( mouseOver ) background = _helper.negativeTextColor(palette);

        } else if( isAnimated() ) {

            QColor copy( background );
            background = KColorUtils::mix( background, foreground, glowIntensity() );
            foreground = KColorUtils::mix( foreground, copy, glowIntensity() );

            if( isActive() || _client.isForcedActive() )
            { background = _helper.alphaColor( background, 0.5 ); }

        } else if( mouseOver ) {

            qSwap( foreground, background );

            if( isActive() || _client.isForcedActive() )
            { background = _helper.alphaColor( background, 0.5 ); }

        }

        // Icon
        // for menu button the application icon is used
        if( isMenuButton() )
        {

            int iconScale( 0 );
            switch( _client.buttonSize() )
            {
                case Configuration::ButtonSmall: iconScale = 13; break;

                default:
                case Configuration::ButtonDefault: iconScale = 16; break;
                case Configuration::ButtonLarge: iconScale = 20; break;
                case Configuration::ButtonVeryLarge: iconScale = 24; break;
                case Configuration::ButtonHuge: iconScale = 35; break;
            }

            const QPixmap& pixmap( _client.icon().pixmap( iconScale ) );
            const double offset = 0.5*(width()-pixmap.width() );
            painter.drawPixmap(offset, offset-1, pixmap );

        } else {


            drawIcon( &painter, foreground, background );

        }

    }

    //___________________________________________________
    void Button::mousePressEvent( QMouseEvent *event )
    {

        if( _type == ButtonMax || event->button() == Qt::LeftButton )
        {
            _status |= Pressed;
            parentUpdate();
        }

        KCommonDecorationButton::mousePressEvent( event );
    }

    //___________________________________________________
    void Button::mouseReleaseEvent( QMouseEvent* event )
    {
        if (_type != ButtonApplicationMenu)
        {
            _status &= ~Pressed;
            parentUpdate();
        }

        KCommonDecorationButton::mouseReleaseEvent( event );
    }

    //___________________________________________________
    void Button::enterEvent( QEvent *event )
    {

        KCommonDecorationButton::enterEvent( event );
        _status |= Hovered;

        if( buttonAnimationsEnabled() )
        {

            _glowAnimation->setDirection( Animation::Forward );
            if( !isAnimated() ) _glowAnimation->start();

        } else parentUpdate();

    }

    //___________________________________________________
    void Button::leaveEvent( QEvent *event )
    {

        KCommonDecorationButton::leaveEvent( event );

        if( _status&Hovered && buttonAnimationsEnabled() )
        {
            _glowAnimation->setDirection( Animation::Backward );
            if( !isAnimated() ) _glowAnimation->start();
        }

        _status &= ~Hovered;
        parentUpdate();

    }

    //___________________________________________________
    void Button::resizeEvent( QResizeEvent *event )
    {

        // resize backing store pixmap
        if( !_client.compositingActive() )
        { _pixmap = QPixmap( event->size() ); }

        // base class implementation
        KCommonDecorationButton::resizeEvent( event );

    }

    //___________________________________________________
    void Button::paintEvent(QPaintEvent *event)
    {

        if( _client.hideTitleBar() ) return;

        /*
        do nothing in compositing mode.
        painting is performed by the parent widget
        */
        if( !_client.compositingActive() )
        {

            {

                // create painter
                QPainter painter( &_pixmap );
                painter.setRenderHints(QPainter::Antialiasing);
                painter.setClipRect( this->rect().intersected( event->rect() ) );

                // render parent background
                painter.translate( -geometry().topLeft() );
                _client.paintBackground( painter );

                // render buttons
                painter.translate( geometry().topLeft() );
                paint( painter );

            }

            QPainter painter(this);
            painter.setClipRegion( event->region() );
            painter.drawPixmap( QPoint(), _pixmap );

        }

    }

    //___________________________________________________
    void Button::parentUpdate( void )
    {

        #if OXYGEN_USE_KDE4
        if( ( _client.compositingActive() || _client.isPreview() ) && parentWidget() ) parentWidget()->update( geometry().adjusted( -1, -1, 1, 1 ) );
        #else
        if( _client.compositingActive() || _client.isPreview() ) _client.update( geometry().adjusted( -1, -1, 1, 1 ) );
        #endif
        else if( parentWidget() ) parentWidget()->update( geometry().adjusted( -1, -1, 1, 1 ) );
        else this->update();

    }

    //___________________________________________________
    void Button::drawIcon( QPainter* painter )
    {

        painter->save();
        painter->setWindow( 0, 0, 21, 21 );

        switch(_type)
        {

            case ButtonSticky:
            painter->drawPoint(QPointF(10.5,10.5));
            break;

            case ButtonHelp:
            painter->translate(1.5, 1.5);
            painter->drawArc(7,5,4,4,135*16, -180*16);
            painter->drawArc(9,8,4,4,135*16,45*16);
            painter->drawPoint(9,12);
            painter->translate(-1.5, -1.5);
            break;

            case ButtonApplicationMenu:
            painter->drawLine(QPointF(7.5, 7.5), QPointF(13.5, 7.5));
            painter->drawLine(QPointF(7.5, 10.5), QPointF(13.5, 10.5));
            painter->drawLine(QPointF(7.5, 13.5), QPointF(13.5, 13.5));
            break;

            case ButtonMin:
            painter->drawLine(QPointF( 7.5, 9.5), QPointF(10.5,12.5));
            painter->drawLine(QPointF(10.5,12.5), QPointF(13.5, 9.5));
            break;

            case ButtonMax:
            switch(_client.maximizeMode())
            {
                case Client::MaximizeRestore:
                case Client::MaximizeVertical:
                case Client::MaximizeHorizontal:
                painter->drawLine(QPointF( 7.5, 11.5 ), QPointF( 10.5, 8.5) );
                painter->drawLine(QPointF( 10.5, 8.5 ), QPointF( 13.5, 11.5) );
                break;

                case Client::MaximizeFull:
                {
                    painter->translate(1.5, 1.0 );
                    QPoint points[4] = {QPoint(9, 6), QPoint(12, 9), QPoint(9, 12), QPoint(6, 9)};
                    painter->drawPolygon(points, 4);
                    painter->translate(-1.5, -1.0 );
                    break;
                }
            }
            break;

            case ButtonItemClose:
            case ButtonClose:
            painter->drawLine(QPointF( 7.5,7.5), QPointF(13.5,13.5));
            painter->drawLine(QPointF(13.5,7.5), QPointF( 7.5,13.5));
            break;

            case ButtonAbove:
            painter->drawLine(QPointF( 7.5,14), QPointF(10.5,11));
            painter->drawLine(QPointF(10.5,11), QPointF(13.5,14));
            painter->drawLine(QPointF( 7.5,10), QPointF(10.5, 7));
            painter->drawLine(QPointF(10.5, 7), QPointF(13.5,10));
            break;

            case ButtonBelow:
            painter->drawLine(QPointF( 7.5,11), QPointF(10.5,14));
            painter->drawLine(QPointF(10.5,14), QPointF(13.5,11));
            painter->drawLine(QPointF( 7.5, 7), QPointF(10.5,10));
            painter->drawLine(QPointF(10.5,10), QPointF(13.5, 7));
            break;

            case ButtonShade:
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

    //___________________________________________________
    void Button::drawIcon( QPainter* painter, QColor foreground, QColor background )
    {

        painter->save();
        painter->setWindow( 0, 0, 18, 18 );
        painter->setRenderHints( QPainter::Antialiasing );

        // outside circle
        if( background.isValid() )
        {

            // render circle
            painter->setPen( Qt::NoPen );
            painter->setBrush( background );
            painter->drawEllipse( QRectF( 0, 0, 18, 18 ) );

        }

        if( foreground.isValid() )
        {
            // render mark
            QPen pen;
            pen.setCapStyle( Qt::RoundCap );
            pen.setJoinStyle( Qt::MiterJoin );
            pen.setColor( foreground );

            const qreal penWidth( 1 );
            pen.setWidth( 2*penWidth );

            painter->setBrush( Qt::NoBrush );
            painter->setPen( pen );

            switch(_type)
            {

                case ButtonItemClose:
                case ButtonClose:

                // render
                painter->drawLine( QPointF( 5 + penWidth, 5 + penWidth ), QPointF( 13 - penWidth, 13 - penWidth ) );
                painter->drawLine( 13 - penWidth, 5 + penWidth, 5 + penWidth, 13 - penWidth );
                break;

                case ButtonMax:
                switch(_client.maximizeMode())
                {
                    case Client::MaximizeRestore:
                    case Client::MaximizeVertical:
                    case Client::MaximizeHorizontal:

                    painter->drawPolyline( QPolygonF()
                        << QPointF( 3.5 + penWidth, 11.5 - penWidth )
                        << QPointF( 9, 5.5 + penWidth )
                        << QPointF( 14.5 - penWidth, 11.5 - penWidth ) );

                    break;

                    case Client::MaximizeFull:
                    pen.setJoinStyle( Qt::RoundJoin );
                    painter->setPen( pen );

                    painter->drawPolygon( QPolygonF()
                        << QPointF( 3.5 + penWidth, 9 )
                        << QPointF( 9, 3.5 + penWidth )
                        << QPointF( 14.5 - penWidth, 9 )
                        << QPointF( 9, 14.5 - penWidth ) );

                    break;

                }
                break;

                case ButtonMin:
                painter->drawPolyline( QPolygonF()
                    << QPointF( 3.5 + penWidth, 6.5 + penWidth )
                    << QPointF( 9, 12.5 - penWidth )
                    << QPointF( 14.5 - penWidth, 6.5 + penWidth ) );

                break;

                default: break;

            }

        }

        painter->restore();

    }

    //___________________________________________________
    void Button::slotAppMenuHidden()
    {
        _status = Normal;
        update();
    }

}
