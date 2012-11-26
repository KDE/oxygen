//////////////////////////////////////////////////////////////////////////////
// oxygenclient.cpp
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright (c) 2006, 2007 Casper Boemann <cbr@boemann.dk>
// Copyright (c) 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
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

#include "oxygenclient.h"
#include "oxygenclient.moc"

#include "oxygenbutton.h"
#include "oxygensizegrip.h"

#include <cassert>
#include <cmath>

#include <KLocale>
#include <KColorUtils>
#include <KDebug>
#include <KStyle>

#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QX11Info>
#include <QtCore/QObjectList>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace Oxygen
{

    //___________________________________________
    Client::Client(KDecorationBridge *b, Factory *f):
        KCommonDecorationUnstable(b, f),
        _factory( f ),
        _sizeGrip( 0 ),
        _glowAnimation( new Animation( 200, this ) ),
        _titleAnimationData( new TitleAnimationData( this ) ),
        _glowIntensity(0),
        _initialized( false ),
        _forceActive( false ),
        _mouseButton( Qt::NoButton ),
        _itemData( this ),
        _sourceItem( -1 ),
        _shadowAtom( 0 )
    {}

    //___________________________________________
    Client::~Client()
    {

        // delete sizegrip if any
        if( hasSizeGrip() ) deleteSizeGrip();

    }

    //___________________________________________
    QString Client::visibleName() const
    { return i18n("Oxygen"); }

    //___________________________________________
    void Client::init()
    {

        KCommonDecoration::init();

        widget()->setAttribute(Qt::WA_NoSystemBackground );
        widget()->setAutoFillBackground( false );
        widget()->setAcceptDrops( true );

        // setup glow animation
        _glowAnimation->setStartValue( glowBias() );
        _glowAnimation->setEndValue( 1.0 );
        _glowAnimation->setTargetObject( this );
        _glowAnimation->setPropertyName( "glowIntensity" );
        _glowAnimation->setEasingCurve( QEasingCurve::InOutQuad );
        connect( _glowAnimation, SIGNAL(finished()), this, SLOT(clearForceActive()) );


        // title animation data
        _titleAnimationData->initialize();
        connect( _titleAnimationData, SIGNAL(pixmapsChanged()), SLOT(updateTitleRect()) );

        // lists
        connect( _itemData.animation().data(), SIGNAL(finished()), this, SLOT(clearTargetItem()) );

        // in case of preview, one wants to make the label used
        // for the central widget transparent. This allows one to have
        // the correct background (with gradient) rendered
        // Remark: this is minor (and safe) a hack.
        // This should be moved upstream (into kwin/lib/kdecoration)
        if( isPreview() )
        {

            QList<QLabel*> children( widget()->findChildren<QLabel*>() );
            foreach( QLabel* widget, children )
            { widget->setAutoFillBackground( false ); }

            // also change shadow configuration size to something that fits in the preview list
            shadowCache().setShadowSize( QPalette::Active, 15 );
            shadowCache().setShadowSize( QPalette::Inactive, 15 );

        }

        setAlphaEnabled(!isMaximized());

        _initialized = true;

        // first reset is needed to store Oxygen configuration
        reset(0);

    }

    //___________________________________________
    void Client::reset( unsigned long changed )
    {
        KCommonDecorationUnstable::reset( changed );

        // update window mask when compositing is changed
        if( !_initialized ) return;
        if( changed & SettingCompositing )
        {
            updateWindowShape();
            widget()->update();
        }

        _configuration = _factory->configuration( *this );

        // glow animations
        _glowAnimation->setDuration( configuration().shadowAnimationsDuration() );

        // title transitions
        _titleAnimationData->setDuration( configuration().titleAnimationsDuration() );

        // tabs
        _itemData.setAnimationsEnabled( animationsEnabled() && configuration().tabAnimationsEnabled() );
        _itemData.animation().data()->setDuration( configuration().tabAnimationsDuration() );

        // reset title transitions
        _titleAnimationData->reset();

        // should also update animations for buttons
        resetButtons();

        // also reset tab buttons
        for( int index = 0; index < _itemData.count(); index++ )
        {
            ClientGroupItemData& item( _itemData[index] );
            if( item._closeButton ) { item._closeButton.data()->reset(0); }
        }

        // reset tab geometry
        _itemData.setDirty( true );

        // handle size grip
        if( configuration().drawSizeGrip() )
        {

            if( !hasSizeGrip() ) createSizeGrip();

        } else if( hasSizeGrip() ) deleteSizeGrip();

        // needs to remove shadow property on window since shadows are handled by the decoration
        removeShadowHint();

    }

    //___________________________________________
    bool Client::decorationBehaviour(DecorationBehaviour behaviour) const
    {
        switch (behaviour)
        {

            case DB_MenuClose:
            return configuration().closeFromMenuButton();

            case DB_WindowMask:
            return false;

            default:
            return KCommonDecoration::decorationBehaviour(behaviour);
        }
    }

    //_________________________________________________________
    KCommonDecorationButton *Client::createButton(::ButtonType type)
    {

        switch (type) {

            case MenuButton:
            return new Button(*this, i18n("Window Actions Menu"), ButtonMenu);

            case AppMenuButton:
            return new Button(*this, i18n("Application Menu"), ButtonApplicationMenu);

            case HelpButton:
            return new Button(*this, i18n("Help"), ButtonHelp);

            case MinButton:
            return new Button(*this, i18n("Minimize"), ButtonMin);

            case MaxButton:
            return new Button(*this, i18n("Maximize"), ButtonMax);

            case CloseButton:
            return new Button(*this, i18n("Close"), ButtonClose);

            case AboveButton:
            return new Button(*this, i18n("Keep Above Others"), ButtonAbove);

            case BelowButton:
            return new Button(*this, i18n("Keep Below Others"), ButtonBelow);

            case OnAllDesktopsButton:
            return new Button(*this, i18n("On All Desktops"), ButtonSticky);

            case ShadeButton:
            return new Button(*this, i18n("Shade Button"), ButtonShade);

            default: break;

        }

        return NULL;

    }

    //_________________________________________________________
    QRegion Client::calcMask( void ) const
    {

        if( isMaximized() ) { return widget()->rect(); }
        const QRect frame( widget()->rect().adjusted(
            layoutMetric( LM_OuterPaddingLeft ), layoutMetric( LM_OuterPaddingTop ),
            -layoutMetric( LM_OuterPaddingRight ), -layoutMetric( LM_OuterPaddingBottom ) ) );

        QRegion mask;
        if( configuration().frameBorder() == Configuration::BorderNone && !isShade() )
        {

            if( hideTitleBar() ) mask = QRegion();
            else if( compositingActive() ) mask = QRegion();
            else mask = helper().roundedMask( frame, 1, 1, 1, 0 );

        } else {

            if( compositingActive() ) mask = QRegion();
            else mask = helper().roundedMask( frame );

        }

        return mask;

    }

    //___________________________________________
    int Client::layoutMetric(LayoutMetric lm, bool respectWindowState, const KCommonDecorationButton *btn) const
    {

        const bool maximized( isMaximized() );
        const bool shaded( isShade() );
        const bool narrowSpacing( configuration().useNarrowButtonSpacing() );
        const int frameBorder( configuration().frameBorder() );
        const int buttonSize( hideTitleBar() ? 0 : configuration().buttonSize() );

        switch (lm)
        {
            case LM_BorderLeft:
            case LM_BorderRight:
            {
                int border( frameBorder );
                if( respectWindowState && maximized )
                {

                    border = 0;

                } else if( frameBorder < Configuration::BorderTiny ) {

                    border = 0;

                } else if( !compositingActive() && frameBorder == Configuration::BorderTiny ) {

                    border = qMax( frameBorder, 3 );

                }

                return border;
            }

            case LM_BorderBottom:
            {
                int border( frameBorder );
                if( (respectWindowState && maximized) || shaded )
                {

                    border = 0;

                } else if( frameBorder >= Configuration::BorderNoSide ) {

                    // for tiny border, the convention is to have a larger bottom area in order to
                    // make resizing easier
                    border = qMax(frameBorder, 4);

                } else if( frameBorder < Configuration::BorderTiny ) {

                    border = 0;

                } else if( !compositingActive() && frameBorder == Configuration::BorderTiny ) {

                    border = qMax( frameBorder, 3 );

                }

                return border;
            }

            case LM_TitleEdgeTop:
            {
                int border = 0;
                if( frameBorder == Configuration::BorderNone && hideTitleBar() )
                {

                    border = 0;

                } else if( !( respectWindowState && maximized )) {

                    border = TFRAMESIZE;

                }

                return border;

            }

            case LM_TitleEdgeBottom:
            {
                return 0;
            }

            case LM_TitleEdgeLeft:
            case LM_TitleEdgeRight:
            {
                int border = 0;
                if( !(respectWindowState && maximized) )
                { border = 4; }

                return border;

            }

            case LM_TitleBorderLeft:
            case LM_TitleBorderRight:
            {
                int border = 5;

                // if title outline is to be drawn, one adds the space needed to
                // separate title and tab border. namely the same value
                if( configuration().drawTitleOutline() ) border += border;

                return border;
            }

            case LM_ButtonWidth:
            case LM_ButtonHeight:
            case LM_TitleHeight:
            {
                return buttonSize;
            }

            case LM_ButtonSpacing:
            return narrowSpacing ? 1:3;

            case LM_ButtonMarginTop:
            return 0;

            // outer margin for shadow/glow
            case LM_OuterPaddingLeft:
            case LM_OuterPaddingRight:
            case LM_OuterPaddingTop:
            case LM_OuterPaddingBottom:
            if( maximized ) return 0;
            else return shadowCache().shadowSize();

            default:
            return KCommonDecoration::layoutMetric(lm, respectWindowState, btn);
        }

    }

    //_________________________________________________________
    QRect Client::defaultTitleRect( bool active ) const
    {

        QRect titleRect( this->titleRect().adjusted( 0, -layoutMetric( LM_TitleEdgeTop ), 0, 0 ) );

        // when drawing title outline, shrink the rect so that it matches the actual caption size
        if( active && configuration().drawTitleOutline() && isActive() )
        {


            if( configuration().centerTitleOnFullWidth() )
            {
                titleRect.setLeft( widget()->rect().left() + layoutMetric( LM_OuterPaddingLeft ) );
                titleRect.setRight( widget()->rect().right() - layoutMetric( LM_OuterPaddingRight ) );
            }

            const QRect textRect( titleBoundingRect( options()->font( true, false),  titleRect, caption() ) );
            titleRect.setLeft( textRect.left() - layoutMetric( LM_TitleBorderLeft ) );
            titleRect.setRight( textRect.right() + layoutMetric( LM_TitleBorderRight ) );

        } else {

            // buttons are properly accounted for in titleBoundingRect method
            titleRect.setLeft( widget()->rect().left() + layoutMetric( LM_OuterPaddingLeft ) );
            titleRect.setRight( widget()->rect().right() - layoutMetric( LM_OuterPaddingRight ) );

        }

        return titleRect;

    }

    //_________________________________________________________
    QRect Client::titleBoundingRect( const QFont& font, QRect rect, const QString& caption ) const
    {

        // get title bounding rect
        QRect boundingRect( QFontMetrics( font ).boundingRect( rect, configuration().titleAlignment() | Qt::AlignVCenter, caption ) );

        // adjust to make sure bounding rect
        // 1/ has same vertical alignment as original titleRect
        // 2/ does not exceeds available horizontal space
        boundingRect.setTop( rect.top() );
        boundingRect.setBottom( rect.bottom() );

        // check bounding rect against input rect
        boundRectTo( boundingRect, rect );

        if( configuration().centerTitleOnFullWidth() )
        {

            /*
            check bounding rect against max available space, for buttons
            this is not needed if centerTitleOnFullWidth flag is set to false,
            because it was already done before calling titleBoundingRect
            */
            boundRectTo( boundingRect, titleRect() );

        }

        return boundingRect;

    }

    //_________________________________________________________
    void Client::boundRectTo( QRect& rect, const QRect& bound ) const
    {

        if( bound.left() > rect.left() )
        {
            rect.moveLeft( bound.left() );
            if( bound.right() < rect.right() )
            { rect.setRight( bound.right() ); }

        } else if( bound.right() < rect.right() ) {

            rect.moveRight( bound.right() );
            if( bound.left() > rect.left() )
            { rect.setLeft( bound.left() ); }

        }

        return;
    }

    //_________________________________________________________
    void Client::clearTargetItem( void )
    {

        if( _itemData.animationType() == AnimationLeave )
        { _itemData.setDirty( true ); }

    }

    //_________________________________________________________
    void Client::updateItemBoundingRects( bool alsoUpdate )
    {

        // make sure items are not animated
        _itemData.animate( AnimationNone );

        // maximum available space
        const QRect titleRect( this->titleRect() );

        // get tabs
        const int items( tabCount() );

        // make sure item data have the correct number of items
        while( _itemData.count() < items ) _itemData.push_back( ClientGroupItemData() );
        while( _itemData.count() > items )
        {
            if( _itemData.back()._closeButton ) delete _itemData.back()._closeButton.data();
            _itemData.pop_back();
        }

        assert( !_itemData.isEmpty() );

        // create buttons
        if( _itemData.count() == 1 )
        {

            // remove button
            if( _itemData.front()._closeButton )
            { delete _itemData.front()._closeButton.data(); }

            // set active rect
            _itemData.front()._activeRect = titleRect.adjusted( 0, -layoutMetric( LM_TitleEdgeTop ), 0, 0 );

        } else {

            int left( titleRect.left() );
            const int width( titleRect.width()/items );
            for( int index = 0; index < _itemData.count(); index++ )
            {

                ClientGroupItemData& item(_itemData[index]);

                // make sure button exists
                if( !item._closeButton )
                {
                    item._closeButton = ClientGroupItemData::ButtonPointer( new Button( *this, "Close this tab", ButtonItemClose ) );
                    item._closeButton.data()->show();
                    item._closeButton.data()->installEventFilter( this );
                }

                // set active rect
                QRect local(  QPoint( left, titleRect.top() ), QSize( width, titleRect.height() ) );
                local.adjust( 0, -layoutMetric( LM_TitleEdgeTop ), 0, 0 );
                item._activeRect = local;
                left += width;

            }

        }

        if( _itemData.count() == 1 )
        {

            _itemData.front().reset( defaultTitleRect() );

        } else {

            for( int index = 0; index < _itemData.count(); index++ )
            { _itemData[index].reset( _itemData[index]._activeRect ); }

        }

        // button activity
        _itemData.updateButtonActivity( currentTabId() );

        // reset buttons location
        _itemData.updateButtons( alsoUpdate );
        _itemData.setDirty( false );

        return;

    }

    //_________________________________________________________
    QColor Client::titlebarTextColor(const QPalette &palette) const
    {
        if( glowIsAnimated() ) return KColorUtils::mix(
            titlebarTextColor( palette, false ),
            titlebarTextColor( palette, true ),
            glowIntensity() );
        else return titlebarTextColor( palette, isActive() );
    }

    //_________________________________________________________
    void Client::renderWindowBackground( QPainter* painter, const QRect& rect, const QWidget* widget, const QPalette& palette ) const
    {

        // window background
        if(
            configuration().blendColor() == Configuration::NoBlending ||
            ( configuration().blendColor() == Configuration::BlendFromStyle &&
            !helper().hasBackgroundGradient( windowId() )
            ) )
        {

            painter->fillRect( rect, palette.color( QPalette::Window ) );

        } else {

            int offset = layoutMetric( LM_OuterPaddingTop );

            // radial gradient positionning
            const int height = hideTitleBar() ? 0:configuration().buttonSize();
            if( isMaximized() ) offset -= 3;

            const QWidget* window( isPreview() ? this->widget() : widget->window() );
            helper().renderWindowBackground(painter, rect, widget, window, palette, offset, height );

        }

        // background pixmap
        if( isPreview() || helper().hasBackgroundPixmap( windowId() ) )
        {
            int offset = layoutMetric( LM_OuterPaddingTop );

            // radial gradient positionning
            const int height = hideTitleBar() ? 0:configuration().buttonSize();
            if( isMaximized() ) offset -= 3;

            // background pixmap
            QPoint backgroundPixmapOffset( layoutMetric( LM_OuterPaddingLeft ) + layoutMetric( LM_BorderLeft ), 0 );
            helper().setBackgroundPixmapOffset( backgroundPixmapOffset );

            const QWidget* window( isPreview() ? this->widget() : widget->window() );
            helper().renderBackgroundPixmap(painter, rect, widget, window, offset, height );

        }

    }

    //_________________________________________________________
    void Client::renderWindowBorder( QPainter* painter, const QRect& clipRect, const QWidget* widget, const QPalette& palette ) const
    {

        // get coordinates relative to the client area
        // this is annoying. One could use mapTo if this was taking const QWidget* and not
        // const QWidget* as argument.
        const QWidget* window = (isPreview()) ? this->widget() : widget->window();
        const QWidget* w = widget;
        QPoint position( 0, 0 );
        while (  w != window && !w->isWindow() && w != w->parentWidget() )
        {
            position += w->geometry().topLeft();
            w = w->parentWidget();
        }

        // save painter
        if( clipRect.isValid() )
        {
            painter->save();
            painter->setClipRegion(clipRect,Qt::IntersectClip);
        }

        QRect r = (isPreview()) ? this->widget()->rect():window->rect();
        r.adjust( layoutMetric( LM_OuterPaddingLeft ), layoutMetric( LM_OuterPaddingTop ), -layoutMetric( LM_OuterPaddingRight ), -layoutMetric( LM_OuterPaddingBottom ) );
        r.adjust(0,0, 1, 1);

        // base color
        QColor color( palette.window().color() );

        // add alpha channel
        if( _itemData.count() == 1 && glowIsAnimated() )
        { color = helper().alphaColor( color, glowIntensity() ); }

        // title height
        const int titleHeight( layoutMetric( LM_TitleEdgeTop ) + layoutMetric( LM_TitleEdgeBottom ) + layoutMetric( LM_TitleHeight ) );

        // make titlebar background darker for tabbed, non-outline window
        if( ( tabCount() >= 2 || _itemData.isAnimated() ) && !(configuration().drawTitleOutline() && isActive() ) )
        {

            const QPoint topLeft( r.topLeft()-position );
            const QRect rect( topLeft, QSize( r.width(), titleHeight ) );

            QLinearGradient lg( rect.topLeft(), rect.bottomLeft() );
            lg.setColorAt( 0, helper().alphaColor( Qt::black, 0.05 ) );
            lg.setColorAt( 1, helper().alphaColor( Qt::black, 0.10 ) );
            painter->setBrush( lg );
            painter->setPen( Qt::NoPen );
            painter->drawRect( rect );

        }

        // horizontal line
        {
            const int shadowSize = 7;
            const int height = shadowSize-3;

            const QPoint topLeft( r.topLeft()+QPoint(0,titleHeight-height)-position );
            QRect rect( topLeft, QSize( r.width(), height ) );

            // adjustements to cope with shadow size and outline border.
            rect.adjust( -shadowSize, 0, shadowSize-1, 0 );
            if( configuration().drawTitleOutline() && ( isActive() || glowIsAnimated() ) && !isMaximized() )
            {
                if( configuration().frameBorder() == Configuration::BorderTiny ) rect.adjust( 1, 0, -1, 0 );
                else if( configuration().frameBorder() > Configuration::BorderTiny ) rect.adjust( HFRAMESIZE-1, 0, -HFRAMESIZE+1, 0 );
            }

            if( rect.isValid() )
            { helper().slab( color, 0, shadowSize )->render( rect, painter, TileSet::Top ); }

        }

        if( configuration().drawTitleOutline() && ( isActive() || glowIsAnimated() ) )
        {

            // save old hints and turn off anti-aliasing
            const QPainter::RenderHints hints( painter->renderHints() );
            painter->setRenderHint( QPainter::Antialiasing, false );

            // save mask and frame to where
            // grey window background is to be rendered
            QRegion mask;
            QRect frame;

            // bottom line
            const int leftOffset = qMin( layoutMetric( LM_BorderLeft ), int(HFRAMESIZE) );
            const int rightOffset = qMin( layoutMetric( LM_BorderRight ), int(HFRAMESIZE) );
            if( configuration().frameBorder() > Configuration::BorderNone )
            {

                const int height = qMax( 0, layoutMetric( LM_BorderBottom ) - HFRAMESIZE );
                const int width = r.width() - leftOffset - rightOffset - 1;

                const QRect rect( r.bottomLeft()-position + QPoint( leftOffset, -layoutMetric( LM_BorderBottom ) ), QSize( width, height ) );
                if( height > 0 ) { mask += rect; frame |= rect; }

                const QColor shadow( helper().calcDarkColor( color ) );
                painter->setPen( shadow );
                painter->drawLine( rect.bottomLeft()+QPoint(-1,1), rect.bottomRight()+QPoint(1,1) );

            }

            // left and right
            const int topOffset = titleHeight;
            const int bottomOffset = qMin( layoutMetric( LM_BorderBottom ), int(HFRAMESIZE) );
            const int height = r.height() - topOffset - bottomOffset - 1;

            if( configuration().frameBorder() >= Configuration::BorderTiny )
            {

                const QColor shadow( helper().calcLightColor( color ) );
                painter->setPen( shadow );

                // left
                int width = qMax( 0, layoutMetric( LM_BorderLeft ) - HFRAMESIZE );
                QRect rect( r.topLeft()-position + QPoint( layoutMetric( LM_BorderLeft ) - width, topOffset ), QSize( width, height ) );
                if( width > 0 ) { mask += rect; frame |= rect; }

                painter->drawLine( rect.topLeft()-QPoint(1,0), rect.bottomLeft()-QPoint(1, 0) );

                // right
                width = qMax( 0, layoutMetric( LM_BorderRight ) - HFRAMESIZE );
                rect = QRect(r.topRight()-position + QPoint( -layoutMetric( LM_BorderRight ), topOffset ), QSize( width, height ));
                if( width > 0 ) { mask += rect; frame |= rect; }

                painter->drawLine( rect.topRight()+QPoint(1,0), rect.bottomRight()+QPoint(1, 0) );
            }

            // restore old hints
            painter->setRenderHints( hints );

            // in preview mode also adds center square
            if( isPreview() )
            {
                const QRect rect( r.topLeft()-position + QPoint( layoutMetric( LM_BorderLeft ), topOffset ), QSize(r.width()-layoutMetric( LM_BorderLeft )-layoutMetric( LM_BorderRight ),height) );
                mask += rect; frame |= rect;
            }

            // paint
            if( !mask.isEmpty() )
            {
                painter->setClipRegion( mask, Qt::IntersectClip);
                renderWindowBackground(painter, frame, widget, palette );
            }

        }

        // restore painter
        if( clipRect.isValid() )
        { painter->restore(); }

    }

    //_________________________________________________________
    void Client::renderSeparator( QPainter* painter, const QRect& clipRect, const QWidget* widget, const QColor& color ) const
    {

        const QWidget* window = (isPreview()) ? this->widget() : widget->window();

        // get coordinates relative to the client area
        // this is annoying. One could use mapTo if this was taking const QWidget* and not
        // const QWidget* as argument.
        QPoint position( 0, 0 );
        {
            const QWidget* w = widget;
            while (  w != window && !w->isWindow() && w != w->parentWidget() ) {
                position += w->geometry().topLeft();
                w = w->parentWidget();
            }
        }

        // setup painter
        if (clipRect.isValid())
        {
            painter->save();
            painter->setClipRegion(clipRect,Qt::IntersectClip);
        }

        QRect r = (isPreview()) ? this->widget()->rect():window->rect();
        r.adjust( layoutMetric( LM_OuterPaddingLeft ), layoutMetric( LM_OuterPaddingTop ), -layoutMetric( LM_OuterPaddingRight ), -layoutMetric( LM_OuterPaddingBottom ) );

        // dimensions
        const int titleHeight = layoutMetric(LM_TitleHeight);
        const int titleTop = layoutMetric(LM_TitleEdgeTop) + r.top();

        // set color
        QColor local( color );
        if( glowIsAnimated() && configuration().separatorMode() != Configuration::SeparatorAlways )
        { local = helper().alphaColor( color, glowIntensity() ); }

        // render
        helper().drawSeparator( painter, QRect(r.top(), titleTop+titleHeight-1.5, r.width(), 2).translated( -position ), local, Qt::Horizontal);

        if (clipRect.isValid()) { painter->restore(); }

    }

    //_________________________________________________________
    void Client::renderTitleOutline(  QPainter* painter, const QRect& rect, const QPalette& palette ) const
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
            renderWindowBackground(painter, adjustedRect, widget(), palette );
            painter->restore();
        }

        // shadow
        const int shadowSize( 7 );
        const int offset( -3 );
        const int voffset( 5-shadowSize );
        const QRect adjustedRect( rect.adjusted(offset, voffset, -offset, shadowSize) );
        QColor color( palette.color( widget()->backgroundRole() ) );

        // add alpha channel
        if( _itemData.count() == 1 && glowIsAnimated() )
        { color = helper().alphaColor( color, glowIntensity() ); }

        // render slab
        helper().slab( color, 0, shadowSize )->render( adjustedRect, painter, TileSet::Tiles(TileSet::Top|TileSet::Left|TileSet::Right) );

    }

    //_________________________________________________________
    void Client::renderTitleText( QPainter* painter, const QRect& rect, const QColor& color, const QColor& contrast ) const
    {

        if( !_titleAnimationData->isValid() )
        {
            // contrast pixmap
            _titleAnimationData->reset(
                rect,
                renderTitleText( rect, caption(), color ),
                renderTitleText( rect, caption(), contrast ) );
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
                    renderTitleText( rect, caption(), color ),
                    renderTitleText( rect, caption(), contrast ) );

                _titleAnimationData->startAnimation();
                renderTitleText( painter, rect, color, contrast );

            } else if( !caption().isEmpty() ) {

                renderTitleText( painter, rect, caption(), color, contrast );

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

        } else if( !caption().isEmpty() ) {
            renderTitleText( painter, rect, caption(), color, contrast );

        }

    }

    //_______________________________________________________________________
    void Client::renderTitleText( QPainter* painter, const QRect& rect, const QString& caption, const QColor& color, const QColor& contrast, bool elide ) const
    {

        const Qt::Alignment alignment( configuration().titleAlignment() | Qt::AlignVCenter );
        const QString local( elide ? QFontMetrics( painter->font() ).elidedText( caption, Qt::ElideRight, rect.width() ):caption );

        // translate title down in case of maximized window
        if( isMaximized() ) painter->translate( 0, 2 );

        if( contrast.isValid() )
        {
            painter->setPen( contrast );
            painter->translate( 0, 1 );
            painter->drawText( rect, alignment, local );
            painter->translate( 0, -1 );
        }

        painter->setPen( color );
        painter->drawText( rect, alignment, local );

        // translate back
        if( isMaximized() ) painter->translate( 0, -2 );

    }

    //_______________________________________________________________________
    QPixmap Client::renderTitleText( const QRect& rect, const QString& caption, const QColor& color, bool elide ) const
    {

        if( !rect.isValid() ) return QPixmap();

        QPixmap out( rect.size() );
        out.fill( Qt::transparent );
        if( caption.isEmpty() || !color.isValid() ) return out;

        QPainter painter( &out );
        painter.setFont( options()->font(isActive(), false) );
        const Qt::Alignment alignment( configuration().titleAlignment() | Qt::AlignVCenter );
        const QString local( elide ? QFontMetrics( painter.font() ).elidedText( caption, Qt::ElideRight, rect.width() ):caption );

        painter.setPen( color );
        painter.drawText( out.rect(), alignment, local );
        painter.end();
        return out;

    }

    //_______________________________________________________________________
    void Client::renderItem( QPainter* painter, int index, const QPalette& palette )
    {

        const ClientGroupItemData& item( _itemData[index] );

        // see if tag is active
        const int itemCount( _itemData.count() );

        // check item bounding rect
        if( !item._boundingRect.isValid() ) return;

        // create rect in which text is to be drawn
        QRect textRect( item._boundingRect.adjusted( 0, layoutMetric( LM_TitleEdgeTop )-1, 0, -1 ) );

        // add extra space needed for title outline
        if( itemCount > 1 || _itemData.isAnimated() )
        { textRect.adjust( layoutMetric( LM_TitleBorderLeft ), 0, -layoutMetric(LM_TitleBorderRight), 0 ); }

        // add extra space for the button
        if( itemCount > 1 && item._closeButton && item._closeButton.data()->isVisible() )
        { textRect.adjust( 0, 0, - configuration().buttonSize() - layoutMetric(LM_TitleEdgeRight), 0 ); }

        // check if current item is active
        const bool active( tabId(index) == currentTabId() );

        // get current item caption and update text rect
        const QString caption( itemCount == 1 ? this->caption() : this->caption(index) );

        if( !configuration().centerTitleOnFullWidth() )
        { boundRectTo( textRect, titleRect() ); }

        // adjust textRect
        textRect = titleBoundingRect( painter->font(), textRect, caption );

        // title outline
        if( itemCount == 1 )
        {

            // no title outline if the window caption is empty
            if( !caption.trimmed().isEmpty() )
            {
                if( _itemData.isAnimated() ) {

                    renderTitleOutline( painter, item._boundingRect, palette );

                } else if( (isActive()||glowIsAnimated()) && configuration().drawTitleOutline() ) {

                    // adjusts boundingRect accordingly
                    QRect boundingRect( item._boundingRect );
                    boundingRect.setLeft( textRect.left() - layoutMetric( LM_TitleBorderLeft ) );
                    boundingRect.setRight( textRect.right() + layoutMetric( LM_TitleBorderRight ) );

                    // render bounding rect around it with extra margins
                    renderTitleOutline( painter, boundingRect, palette );

                }

            }

        } else if( active ) {

            // in multiple tabs render title outline in all cases
            renderTitleOutline( painter, item._boundingRect, palette );

        }

        // render text
        if( active || itemCount == 1 )
        {

            // for active tab, current caption is "merged" with old caption, if any
            renderTitleText(
                painter, textRect,
                titlebarTextColor( palette ),
                titlebarContrastColor( palette ) );


        } else {

            QColor background( backgroundPalette( widget(), palette ).color( widget()->window()->backgroundRole() ) );

            // add extra shade (as used in renderWindowBorder
            if( !( isActive() && configuration().drawTitleOutline() ) )
            { background = KColorUtils::mix( background, Qt::black, 0.10 ); }

            // otherwise current caption is rendered directly
            renderTitleText(
                painter, textRect, caption,
                titlebarTextColor( backgroundPalette( widget(), palette ), false ),
                titlebarContrastColor( background ) );

        }

        // render separators between inactive tabs
        if( !( active || itemCount == 1 ) && item._closeButton && item._closeButton.data()->isVisible() )
        {

            // separators
            // draw Left separator
            const QColor color( backgroundPalette( widget(), palette ).window().color() );
            const bool isFirstItem(  index == 0 || (index == 1 && !_itemData[0]._boundingRect.isValid() ) );
            if( !active && ( ( isFirstItem && buttonsLeftWidth() > 0 ) || _itemData.isTarget( index ) ) )
            {

                const QRect local( item._boundingRect.topLeft()+QPoint(0,2), QSize( 2, item._boundingRect.height()-3 ) );
                helper().drawSeparator( painter, local, color, Qt::Vertical);

            }

            // draw right separator
            if(
                ( index == itemCount-1 && buttonsRightWidth() > 0 ) ||
                ( index+1 < itemCount && ( _itemData.isTarget( index+1 ) ||
                !( tabId(index+1) == currentTabId() && _itemData[index+1]._boundingRect.isValid() ) ) ) )
            {

                const QRect local( item._boundingRect.topRight()+QPoint(0,2), QSize( 2, item._boundingRect.height()-3 ) );
                helper().drawSeparator( painter, local, color, Qt::Vertical);

            }

        }

    }

    //_______________________________________________________________________
    void Client::renderTargetRect( QPainter* p, const QPalette& palette )
    {
        if( _itemData.targetRect().isNull() || _itemData.isAnimationRunning() ) return;

        const QColor color = palette.color(QPalette::Highlight);
        p->setPen(KColorUtils::mix(color, palette.color(QPalette::Active, QPalette::WindowText)));
        p->setBrush( helper().alphaColor( color, 0.5 ) );
        p->drawRect( QRectF(_itemData.targetRect()).adjusted( 4.5, 2.5, -4.5, -2.5 ) );

    }

    //_______________________________________________________________________
    void Client::renderCorners( QPainter* painter, const QRect& frame, const QPalette& palette ) const
    {

        const QColor color( backgroundColor( widget(), palette ) );

        QLinearGradient lg = QLinearGradient(0, -0.5, 0, qreal( frame.height() )+0.5);
        lg.setColorAt(0.0, helper().calcLightColor( helper().backgroundTopColor(color) ));
        lg.setColorAt(0.51, helper().backgroundBottomColor(color) );
        lg.setColorAt(1.0, helper().backgroundBottomColor(color) );

        painter->setPen( QPen( lg, 1 ) );
        painter->setBrush( Qt::NoBrush );
        painter->drawRoundedRect( QRectF( frame ).adjusted( 0.5, 0.5, -0.5, -0.5 ), 3.5,  3.5 );

    }

    //_______________________________________________________________________
    void Client::renderFloatFrame( QPainter* painter, const QRect& frame, const QPalette& palette ) const
    {

        // shadow and resize handles
        if( !isMaximized() )
        {

            if( configuration().frameBorder() >= Configuration::BorderTiny )
            {

                helper().drawFloatFrame(
                    painter, frame, backgroundColor( widget(), palette ),
                    !compositingActive(), isActive() && configuration().useOxygenShadows(),
                    KDecoration::options()->color(ColorTitleBar)
                    );

            } else {

                // for small borders, use a frame that matches the titlebar only
                const QRect local( frame.topLeft(), QSize( frame.width(), layoutMetric(LM_TitleHeight) + layoutMetric(LM_TitleEdgeTop) ) );
                helper().drawFloatFrame(
                    painter, local, backgroundColor( widget(), palette ),
                    false, isActive() && configuration().useOxygenShadows(),
                    KDecoration::options()->color(ColorTitleBar)
                    );
            }

        } else if( isShade() ) {

            // for shaded maximized windows adjust frame and draw the bottom part of it
            helper().drawFloatFrame(
                painter, frame, backgroundColor( widget(), palette ),
                !( compositingActive() || configuration().frameBorder() == Configuration::BorderNone ), isActive(),
                KDecoration::options()->color(ColorTitleBar),
                TileSet::Bottom
                );

        }

    }

    //____________________________________________________________________________
    void Client::renderDots( QPainter* painter, const QRect& frame, const QColor& color ) const
    {

        if( configuration().frameBorder() >= Configuration::BorderTiny )
        {

            // dimensions
            int x,y,w,h;
            frame.getRect(&x, &y, &w, &h);

            if( isResizable() && !isShade() && !isMaximized() )
            {

                // Draw right side 3-dots resize handles
                const int cenY = (h / 2 + y) ;
                const int posX = (w + x - 3);

                helper().renderDot( painter, QPoint(posX, cenY - 3), color);
                helper().renderDot( painter, QPoint(posX, cenY), color);
                helper().renderDot( painter, QPoint(posX, cenY + 3), color);

            }

            // Draw bottom-right cornet 3-dots resize handles
            if( isResizable() && !isShade() && !configuration().drawSizeGrip() )
            {

                painter->save();
                painter->translate(x + w-9, y + h-9);
                helper().renderDot( painter, QPoint(2, 6), color);
                helper().renderDot( painter, QPoint(5, 5), color);
                helper().renderDot( painter, QPoint(6, 2), color);
                painter->restore();
            }

        }

    }

    //_________________________________________________________
    void Client::activeChange( void )
    {

        KCommonDecorationUnstable::activeChange();
        _itemData.setDirty( true );

        // reset animation
        if( shadowAnimationsEnabled() )
        {
            _glowAnimation->setDirection( isActive() ? Animation::Forward : Animation::Backward );
            if(!glowIsAnimated()) { _glowAnimation->start(); }
        }

        // update size grip so that it gets the right color
        // also make sure it is remaped to from z stack,
        // unless hidden
        if( hasSizeGrip() && !(isShade() || isMaximized() ))
        {
            sizeGrip().activeChange();
            sizeGrip().update();
        }

    }

    //_________________________________________________________
    void Client::maximizeChange( void  )
    {
        if( hasSizeGrip() ) sizeGrip().setVisible( !( isShade() || isMaximized() ) );
        setAlphaEnabled(!isMaximized());
        KCommonDecorationUnstable::maximizeChange();
    }

    //_________________________________________________________
    void Client::shadeChange( void  )
    {
        if( hasSizeGrip() ) sizeGrip().setVisible( !( isShade() || isMaximized() ) );
        KCommonDecorationUnstable::shadeChange();
    }

    //_________________________________________________________
    void Client::captionChange( void  )
    {

        KCommonDecorationUnstable::captionChange();
        _itemData.setDirty( true );
        if( titleAnimationsEnabled() )
        { _titleAnimationData->setDirty( true ); }

    }

    //_________________________________________________________
    QPalette Client::backgroundPalette( const QWidget* widget, QPalette palette ) const
    {

        if( configuration().drawTitleOutline() )
        {
            if( glowIsAnimated() && !isForcedActive() )
            {

                const QColor inactiveColor( backgroundColor( widget, palette, false ) );
                const QColor activeColor( backgroundColor( widget, palette, true ) );
                const QColor mixed( KColorUtils::mix( inactiveColor, activeColor, glowIntensity() ) );
                palette.setColor( QPalette::Window, mixed );
                palette.setColor( QPalette::Button, mixed );

            } else if( isActive() || isForcedActive() ) {

                const QColor color =  options()->color( KDecorationDefines::ColorTitleBar, true );
                palette.setColor( QPalette::Window, color );
                palette.setColor( QPalette::Button, color );

            }

        }

        return palette;

    }

    //_________________________________________________________
    QColor Client::backgroundColor( const QWidget*, QPalette palette, bool active ) const
    {

        return ( configuration().drawTitleOutline() && active ) ?
            options()->color( KDecorationDefines::ColorTitleBar, true ):
            palette.color( QPalette::Window );

    }

    //_________________________________________________________
    QString Client::defaultButtonsLeft() const
    { return KCommonDecoration::defaultButtonsLeft(); }

    //_________________________________________________________
    QString Client::defaultButtonsRight() const
    { return "HIAX"; }

    //________________________________________________________________
    void Client::updateWindowShape()
    {

        if(isMaximized()) clearMask();
        else setMask( calcMask() );

    }

    //______________________________________________________________________________
    bool Client::eventFilter( QObject* object, QEvent* event )
    {

        // all dedicated event filtering is here to handle multiple tabs.

        bool state = false;
        switch( event->type() )
        {

            case QEvent::Show:
            if( widget() == object )
            { _itemData.setDirty( true ); }
            break;

            case QEvent::MouseButtonPress:
            if( widget() == object )
            { state = mousePressEvent( static_cast< QMouseEvent* >( event ) ); }
            break;

            case QEvent::MouseButtonRelease:
            if( widget() == object ) state = mouseReleaseEvent( static_cast< QMouseEvent* >( event ) );
            else if( Button *btn = qobject_cast< Button* >( object ) )
            {
                QMouseEvent* mouseEvent( static_cast< QMouseEvent* >( event ) );
                if( mouseEvent->button() == Qt::LeftButton && btn->rect().contains( mouseEvent->pos() ) )
                { state = closeItem( btn ); }
            }

            break;

            case QEvent::MouseMove:
            state = mouseMoveEvent( static_cast< QMouseEvent* >( event ) );
            break;

            case QEvent::DragEnter:
            if(  widget() == object )
            { state = dragEnterEvent( static_cast< QDragEnterEvent* >( event ) ); }
            break;

            case QEvent::DragMove:
            if( widget() == object )
            { state = dragMoveEvent( static_cast< QDragMoveEvent* >( event ) ); }
            break;

            case QEvent::DragLeave:
            if( widget() == object )
            { state = dragLeaveEvent( static_cast< QDragLeaveEvent* >( event ) ); }
            break;

            case QEvent::Drop:
            if( widget() == object )
            { state = dropEvent( static_cast< QDropEvent* >( event ) ); }
            break;

            default: break;

        }
        return state || KCommonDecorationUnstable::eventFilter( object, event );

    }

    //_________________________________________________________
    void Client::resizeEvent( QResizeEvent* event )
    {

        // prepare item data updates
        _itemData.setDirty( true );

        // mark title animation as dirty
        if( event->oldSize().width() != event->size().width() )
        { _titleAnimationData->setDirty( true ); }

        // resize backing store pixmap
        if( !compositingActive() )
        { _pixmap = QPixmap( event->size() ); }

        // base class implementation
        KCommonDecorationUnstable::resizeEvent( event );
    }

    //_________________________________________________________
    void Client::paintBackground( QPainter& painter ) const
    {
        if( !compositingActive() )
        { painter.drawPixmap( QPoint(), _pixmap ); }
    }

    //_________________________________________________________
    QRegion Client::region( KDecorationDefines::Region r )
    {

        // return empty region for anything but extended borders, when enabled
        if( !( r == KDecorationDefines::ExtendedBorderRegion && configuration().useExtendedWindowBorder() ) )
        { return QRegion(); }

        // return empty region for maximized windows
        if( isMaximized() ) return QRegion();

        // return 3 pixels extended borders for sides that have no visible borders
        // also add the invisible pixels at the masked rounded corners, in non compositing mode
        if( configuration().frameBorder() <= Configuration::BorderNoSide || !compositingActive() )
        {

            QRect rect = widget()->rect().adjusted(
                layoutMetric( LM_OuterPaddingLeft ),
                layoutMetric( LM_OuterPaddingTop ),
                - layoutMetric( LM_OuterPaddingRight ),
                - layoutMetric( LM_OuterPaddingBottom ) );

            rect.translate( -layoutMetric( LM_OuterPaddingLeft ), -layoutMetric( LM_OuterPaddingTop ) );

            // mask
            QRegion mask( calcMask() );
            if( mask.isEmpty() ) mask = rect;
            else mask.translate( -layoutMetric( LM_OuterPaddingLeft ), -layoutMetric( LM_OuterPaddingTop ) );

            // only return non-empty region on the sides for which there is no border
            if( configuration().frameBorder() == Configuration::BorderNone ) return QRegion( rect.adjusted( -3, 0, 3, 3 ) ) - mask;
            else if( configuration().frameBorder() == Configuration::BorderNoSide ) return QRegion( rect.adjusted( -3, 0, 3, 0 ) ) - mask;
            else if( !compositingActive() ) return QRegion( rect ) - mask;

        }

        // fall back
        return QRegion();

    }


    //_________________________________________________________
    void Client::paintEvent( QPaintEvent* event )
    {

        // factory
        if(!( _initialized && _factory->initialized() ) ) return;

        if( compositingActive() )
        {

            QPainter painter(widget());
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setClipRegion( event->region() );
            paint( painter );

            // update buttons
            QList<Button*> buttons( widget()->findChildren<Button*>() );
            foreach( Button* button, buttons )
            {
                if( ( button->isVisible() || isPreview() ) && event->rect().intersects( button->geometry() ) )
                {

                    painter.save();
                    painter.setViewport( button->geometry() );
                    painter.setWindow( button->rect() );
                    button->paint( painter );
                    painter.restore();

                }
            }


        } else {

            {
                // update backing store pixmap
                QPainter painter( &_pixmap );
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setClipRegion( event->region() );
                paint( painter );
            }

            QPainter painter( widget() );
            painter.setClipRegion( event->region() );
            painter.drawPixmap( QPoint(), _pixmap );

            // update buttons
            QList<Button*> buttons( widget()->findChildren<Button*>() );
            foreach( Button* button, buttons )
            {
                if( event->rect().intersects( button->geometry() ) )
                { button->update(); }
            }

        }



    }

    //_________________________________________________________
    void Client::paint( QPainter& painter )
    {

        // palette
        QPalette palette = widget()->palette();
        palette.setCurrentColorGroup( (isActive() ) ? QPalette::Active : QPalette::Inactive );

        // define frame
        QRect frame = widget()->rect();

        // base color
        QColor color = palette.window().color();

        // draw shadows
        if( compositingActive() && shadowCache().shadowSize() > 0 && !isMaximized() )
        {

            TileSet *tileSet( 0 );
            const ShadowCache::Key key( this->key() );
            if( configuration().useOxygenShadows() && glowIsAnimated() && !isForcedActive() )
            {

                tileSet = shadowCache().tileSet( key, glowIntensity() );

            } else {

                tileSet = shadowCache().tileSet( key );

            }

            tileSet->render( frame, &painter, TileSet::Ring);

        }

        // adjust frame
        frame.adjust(
            layoutMetric(LM_OuterPaddingLeft),
            layoutMetric(LM_OuterPaddingTop),
            -layoutMetric(LM_OuterPaddingRight),
            -layoutMetric(LM_OuterPaddingBottom) );

        //  adjust mask
        if( compositingActive() || isPreview() )
        {

            if( isMaximized() ) {

                painter.setClipRect( frame, Qt::IntersectClip );

            } else {

                // multipliers
                const int left = 1;
                const int right = 1;
                const int top = 1;
                int bottom = 1;

                // disable bottom corners when border frame is too small and window is not shaded
                if( configuration().frameBorder() == Configuration::BorderNone && !isShade() ) bottom = 0;
                QRegion mask( helper().roundedMask( frame, left, right, top, bottom ) );

                renderCorners( &painter, frame, palette );
                painter.setClipRegion( mask, Qt::IntersectClip );

            }

        }

        // make sure ItemData and tabList are synchronized
        /*
        this needs to be done before calling RenderWindowBorder
        since some painting in there depend on the clientGroups state
        */
        if(  _itemData.isDirty() || _itemData.count() != tabCount() )
        { updateItemBoundingRects( false ); }

        // window background
        renderWindowBackground( &painter, frame, widget(), backgroundPalette( widget(), palette ) );

        // window border (for title outline)
        if( hasTitleOutline() ) renderWindowBorder( &painter, frame, widget(), palette );

        // clipping
        if( compositingActive() )
        {
            painter.setClipping(false);
            frame.adjust(-1,-1, 1, 1);
        }

        // float frame
        renderFloatFrame( &painter, frame, palette );

        // resize handles
        renderDots( &painter, frame, backgroundColor( widget(), palette ) );

        if( !hideTitleBar() )
        {

            // title bounding rect
            painter.setFont( options()->font(isActive(), false) );

            // draw ClientGroupItems
            const int itemCount( _itemData.count() );
            for( int i = 0; i < itemCount; i++ ) renderItem( &painter, i, palette );

            // draw target rect
            renderTargetRect( &painter, widget()->palette() );

            // separator
            if( itemCount == 1 && !_itemData.isAnimated() && drawSeparator() )
            { renderSeparator(&painter, frame, widget(), color ); }

        }

    }

    //_____________________________________________________________
    bool Client::mousePressEvent( QMouseEvent* event )
    {

        const QPoint point = event->pos();
        if( tabIndexAt( point ) < 0 ) return false;
        _dragPoint = point;

        _mouseButton = event->button();
        bool accepted( false );
        if( buttonToWindowOperation( _mouseButton ) == TabDragOp )
        {

            accepted = true;

        } else if( buttonToWindowOperation( _mouseButton ) == OperationsOp ) {

            QPoint point = event->pos();
            const int clickedIndex( tabIndexAt( point ) );
            _mouseButton = Qt::NoButton;
            if ( tabIndexAt( point ) > -1)
            { showWindowMenu( widget()->mapToGlobal( event->pos() ), tabId(clickedIndex) ); }

            accepted = true; // displayClientMenu can possibly destroy the deco...

        }
        return accepted;
    }

    //_____________________________________________________________
    bool Client::mouseReleaseEvent( QMouseEvent* event )
    {

        bool accepted( false );
        if( _mouseButton == event->button() && buttonToWindowOperation( _mouseButton ) != OperationsOp )
        {

            const QPoint point = event->pos();

            const long visibleItem = currentTabId();
            const int clickedIndex( tabIndexAt( point ) );
            if( clickedIndex >= 0 && visibleItem != tabId(clickedIndex) )
            {
                setCurrentTab( tabId(clickedIndex) );
                setForceActive( true );
                accepted = true;
            }

        }

        _mouseButton = Qt::NoButton;
        return accepted;

    }

    //_____________________________________________________________
    bool Client::mouseMoveEvent( QMouseEvent* event )
    {

        // check button and distance to drag point
        if( hideTitleBar() || _mouseButton == Qt::NoButton  || ( event->pos() - _dragPoint ).manhattanLength() <= QApplication::startDragDistance() )
        { return false; }

        bool accepted( false );
        if( buttonToWindowOperation( _mouseButton ) == TabDragOp )
        {

            const QPoint point = event->pos();
            const int clickedIndex( tabIndexAt( point ) );
            if( clickedIndex < 0 ) return false;

            _titleAnimationData->reset();

            QDrag *drag = new QDrag( widget() );
            QMimeData *groupData = new QMimeData();
            groupData->setData( tabDragMimeType(), QString().setNum( tabId(clickedIndex) ).toAscii() );
            drag->setMimeData( groupData );
            _sourceItem = tabIndexAt( _dragPoint );

            // get tab geometry
            QRect geometry( _itemData[clickedIndex]._boundingRect );

            // remove space used for buttons
            if( _itemData.count() > 1  )
            {

                geometry.adjust( 0, 0,  - configuration().buttonSize() - layoutMetric(LM_TitleEdgeRight), 0 );

            } else if( !( isActive() && configuration().drawTitleOutline() ) ) {

                geometry.adjust(
                    buttonsLeftWidth() + layoutMetric( LM_TitleEdgeLeft ) , 0,
                    -( buttonsRightWidth() + layoutMetric( LM_TitleEdgeRight )), 0 );

            }

            // adjust geometry to include shadow size
            const int shadowSize( shadowCache().shadowSize() );
            const bool drawShadow(
                compositingActive() &&
                KStyle::customStyleHint( "SH_ArgbDndWindow", widget() ) &&
                shadowSize > 0 );

            if( drawShadow )
            { geometry.adjust( -shadowSize, -shadowSize, shadowSize, shadowSize ); }

            // compute pixmap and assign
            drag->setPixmap( itemDragPixmap( clickedIndex, geometry, drawShadow ) );

            // note: the pixmap is moved just above the pointer on purpose
            // because overlapping pixmap and pointer slows down the pixmap a lot.
            QPoint hotSpot( QPoint( event->pos().x() - geometry.left(), -1 ) );
            if( drawShadow ) hotSpot += QPoint( 0, shadowSize );

            // make sure the horizontal hotspot position is not too far away (more than 1px)
            // from the pixmap
            if( hotSpot.x() < -1 ) hotSpot.setX(-1);
            if( hotSpot.x() > geometry.width() ) hotSpot.setX( geometry.width() );

            drag->setHotSpot( hotSpot );

            _dragStartTimer.start( 50, this );
            drag->exec( Qt::MoveAction );

            // detach tab from window
            if( drag->target() == 0 && _itemData.count() > 1 )
            {
                _itemData.setDirty( true );
                untab( tabId(_sourceItem),
                    widget()->frameGeometry().adjusted(
                    layoutMetric( LM_OuterPaddingLeft ),
                    layoutMetric( LM_OuterPaddingTop ),
                    -layoutMetric( LM_OuterPaddingRight ),
                    -layoutMetric( LM_OuterPaddingBottom )
                    ).translated( QCursor::pos() - event->pos() +
                    QPoint( layoutMetric( LM_OuterPaddingLeft ), layoutMetric( LM_OuterPaddingTop )))
                    );
            }

            // reset button
            _mouseButton = Qt::NoButton;
            accepted = true;

        }

        return accepted;

    }

    //_____________________________________________________________
    bool Client::dragEnterEvent( QDragEnterEvent* event )
    {

        // check if drag enter is allowed
        if( !event->mimeData()->hasFormat( tabDragMimeType() ) || hideTitleBar() ) return false;

        // accept event
        event->acceptProposedAction();

        // animate
        if( event->source() != widget() )
        {

            _itemData.animate( AnimationEnter, tabIndexAt( event->pos(), true ) );

        } else if( _itemData.count() > 1 )  {

            _itemData.animate( AnimationEnter|AnimationSameTarget, tabIndexAt( event->pos(), true ) );

        }

        return true;

    }

    //_____________________________________________________________
    bool Client::dragLeaveEvent( QDragLeaveEvent* )
    {

        if( _itemData.animationType() & AnimationSameTarget )
        {

            if( _dragStartTimer.isActive() ) _dragStartTimer.stop();
            _itemData.animate( AnimationLeave|AnimationSameTarget, _sourceItem );

        } else if( _itemData.isAnimated() ) {

            _itemData.animate( AnimationLeave );

        }

        return true;

    }

    //_____________________________________________________________
    bool Client::dragMoveEvent( QDragMoveEvent* event )
    {

        // check format
        if( !event->mimeData()->hasFormat( tabDragMimeType() ) ) return false;

        // animate
        if( event->source() != widget() )
        {

            _itemData.animate( AnimationMove, tabIndexAt( event->pos(), true ) );

        } else if( _itemData.count() > 1 )  {

            if( _dragStartTimer.isActive() ) _dragStartTimer.stop();
            _itemData.animate( AnimationMove|AnimationSameTarget, tabIndexAt( event->pos(), true ) );

        }

        return false;

    }

    //_____________________________________________________________
    bool Client::dropEvent( QDropEvent* event )
    {

        const QPoint point = event->pos();
        _itemData.animate( AnimationNone );

        const QMimeData *groupData = event->mimeData();
        if( !groupData->hasFormat( tabDragMimeType() ) ) return false;

        _itemData.setDirty( true );

        if( widget() != event->source() ) setForceActive( true );

        const long source = QString( groupData->data( tabDragMimeType() ) ).toLong();
        const int clickedIndex( tabIndexAt( point, true ) );
        if( clickedIndex < 0 ) tab_A_behind_B( source, tabId(_itemData.count()-1) );
        else tab_A_before_B( source, tabId(clickedIndex) );

        // update title
        if( widget() == event->source() ) updateTitleRect();

        _titleAnimationData->reset();
        return true;

    }

    //_____________________________________________________________
    void Client::timerEvent( QTimerEvent* event )
    {

        if( event->timerId() != _dragStartTimer.timerId() )
        { return KCommonDecorationUnstable::timerEvent( event ); }

        _dragStartTimer.stop();

        // do nothing if there is only one tab
        if( _itemData.count() > 1 )
        {
            _itemData.animate( AnimationMove|AnimationSameTarget, _sourceItem );
            _itemData.animate( AnimationLeave|AnimationSameTarget, _sourceItem );
        }

    }

    //_____________________________________________________________
    bool Client::closeItem( const Button* button )
    {

        for( int i=0; i <  _itemData.count(); i++ )
        {
            if( button == _itemData[i]._closeButton.data() )
            {
                _itemData.setDirty( true );
                closeTab( tabId(i) );
                return true;
            }
        }
        return false;

    }

    //________________________________________________________________
    QPixmap Client::itemDragPixmap( int index, QRect geometry, bool drawShadow )
    {
        const bool itemValid( index >= 0 && index < tabCount() );

        QPixmap pixmap( geometry.size() );
        pixmap.fill( Qt::transparent );
        QPainter painter( &pixmap );
        painter.setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing);

        painter.translate( -geometry.topLeft() );

        // draw shadows
        if( drawShadow )
        {

            // shadow
            const int shadowSize( shadowCache().shadowSize() );
            TileSet *tileSet( shadowCache().tileSet( ShadowCache::Key() ) );
            tileSet->render( geometry, &painter, TileSet::Ring);
            geometry.adjust( shadowSize, shadowSize, -shadowSize, -shadowSize );

            renderCorners( &painter, geometry, widget()->palette() );

        }

        // mask
        painter.setClipRegion( helper().roundedMask( geometry ), Qt::IntersectClip );

        // render window background
        renderWindowBackground( &painter, geometry, widget(), widget()->palette() );

        // darken background if item is inactive
        const bool itemActive = (tabCount() <= 1) || !( itemValid && tabId(index) != currentTabId() );
        if( !itemActive )
        {

            QLinearGradient lg( geometry.topLeft(), geometry.bottomLeft() );
            lg.setColorAt( 0, helper().alphaColor( Qt::black, 0.05 ) );
            lg.setColorAt( 1, helper().alphaColor( Qt::black, 0.10 ) );
            painter.setBrush( lg );
            painter.setPen( Qt::NoPen );
            painter.drawRect( geometry );

        }

        // render title text
        painter.setFont( options()->font(isActive(), false) );
        QRect textRect( geometry.adjusted( 0, layoutMetric( LM_TitleEdgeTop )-1, 0, -1 ) );

        if( itemValid )
        { textRect.adjust( layoutMetric( LM_TitleBorderLeft ), 0, -layoutMetric(LM_TitleBorderRight), 0 ); }

        const QString caption( itemValid ? this->caption(index) : this->caption() );

        renderTitleText(
            &painter, textRect, caption,
            titlebarTextColor( widget()->palette(), isActive() && itemActive ),
            titlebarContrastColor( widget()->palette() ) );

        // adjust geometry for floatFrame when compositing is on.
        if( drawShadow )
        { geometry.adjust(-1, -1, 1, 1 ); }

        // floating frame
        helper().drawFloatFrame(
            &painter, geometry, widget()->palette().window().color(),
            !drawShadow, false,
            KDecoration::options()->color(ColorTitleBar)
            );

        painter.end();
        return pixmap;

    }

    //_________________________________________________________________
    void Client::createSizeGrip( void )
    {

        assert( !hasSizeGrip() );
        if( ( isResizable() && windowId() != 0 ) || isPreview() )
        {
            _sizeGrip = new SizeGrip( this );
            sizeGrip().setVisible( !( isMaximized() || isShade() ) );
        }

    }

    //_________________________________________________________________
    void Client::deleteSizeGrip( void )
    {
        assert( hasSizeGrip() );
        _sizeGrip->deleteLater();
        _sizeGrip = 0;
    }

    //_________________________________________________________________
    void Client::removeShadowHint( void )
    {

        // do nothing if no window id
        if( !windowId() ) return;

        // create atom
        if( !_shadowAtom )
        { _shadowAtom = XInternAtom( QX11Info::display(), "_KDE_NET_WM_SHADOW", False); }

        XDeleteProperty(QX11Info::display(), windowId(), _shadowAtom);
    }

}
