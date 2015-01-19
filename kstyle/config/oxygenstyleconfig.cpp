/*
Copyright 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
Copyright 2009 Matthew Woehlke <mw.triad@users.sourceforge.net>
Copyright 2009 Long Huynh Huu <long.upcase@googlemail.com>
Copyright 2003 Sandro Giessl <ceebx@users.sourceforge.net>

originally based on the Keramik configuration dialog:
Copyright 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "oxygenstyleconfig.h"
#include "oxygenanimationconfigwidget.h"
#include "oxygenstyleconfigdata.h"
#include "../oxygen.h"
#include "config-liboxygen.h"

#include <QTextStream>
#include <QDBusMessage>
#include <QDBusConnection>

#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>
#if !OXYGEN_USE_KDE4
#include <Kdelibs4Migration>
#endif

#define SCROLLBAR_DEFAULT_WIDTH 15
#define SCROLLBAR_MINIMUM_WIDTH 10
#define SCROLLBAR_MAXIMUM_WIDTH 30

extern "C"
{
    Q_DECL_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
    { return new Oxygen::StyleConfig(parent); }
}

namespace Oxygen
{

    //__________________________________________________________________
    StyleConfig::StyleConfig(QWidget* parent):
        QWidget(parent),
        _expertMode( false ),
        _animationConfigWidget(0)
    {
        setupUi(this);

        // connections
        connect( _expertModeButton, SIGNAL(pressed()), SLOT(toggleExpertModeInternal()) );

        _expertModeButton->setIcon( QIcon::fromTheme( QStringLiteral( "configure" ) ) );

        // toggle expert mode
        toggleExpertModeInternal( false );

        // load setup from configData
        load();

        connect( _useBackgroundGradient, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _toolBarDrawItemSeparator, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _splitterProxyEnabled, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _mnemonicsMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _animationsEnabled, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _viewDrawFocusIndicator, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _viewTriangularExpanderSize, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _viewDrawTreeBranchLines, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _scrollBarWidth, SIGNAL(valueChanged(int)), SLOT(updateChanged()) );
        connect( _scrollBarAddLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _scrollBarSubLineButtons, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );
        connect( _menuHighlightDark, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _menuHighlightStrong, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _menuHighlightSubtle, SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        connect( _windowDragMode, SIGNAL(currentIndexChanged(int)), SLOT(updateChanged()) );

    }

    //__________________________________________________________________
    void StyleConfig::save( void )
    {
        StyleConfigData::setUseBackgroundGradient( _useBackgroundGradient->isChecked() );
        StyleConfigData::setToolBarDrawItemSeparator( _toolBarDrawItemSeparator->isChecked() );
        StyleConfigData::setSplitterProxyEnabled( _splitterProxyEnabled->isChecked() );
        StyleConfigData::setMnemonicsMode( _mnemonicsMode->currentIndex() );
        StyleConfigData::setViewDrawFocusIndicator( _viewDrawFocusIndicator->isChecked() );
        StyleConfigData::setViewTriangularExpanderSize( triangularExpanderSize() );
        StyleConfigData::setViewDrawTreeBranchLines( _viewDrawTreeBranchLines->isChecked() );
        StyleConfigData::setScrollBarWidth( _scrollBarWidth->value() );
        StyleConfigData::setScrollBarAddLineButtons( _scrollBarAddLineButtons->currentIndex() );
        StyleConfigData::setScrollBarSubLineButtons( _scrollBarSubLineButtons->currentIndex() );
        StyleConfigData::setMenuHighlightMode( menuMode() );
        StyleConfigData::setWindowDragMode( _windowDragMode->currentIndex()  );

        if( _expertMode )
        {

            _animationConfigWidget->save();

        } else {

            StyleConfigData::setAnimationsEnabled( _animationsEnabled->isChecked() );

        }

        #if OXYGEN_USE_KDE4
        StyleConfigData::self()->writeConfig();
        #else
        StyleConfigData::self()->save();

        //update the KDE4 config to match
        Kdelibs4Migration migration;
        const QString kde4ConfigDirPath = migration.saveLocation("config");

        QScopedPointer<KConfig> kde4Config(new KConfig);
        StyleConfigData::self()->config()->copyTo(kde4ConfigDirPath+"/oxygenrc", kde4Config.data());
        kde4Config->sync();

        #endif

        // emit dbus signal
        QDBusMessage message( QDBusMessage::createSignal( QStringLiteral( "/OxygenStyle" ),  QStringLiteral( "org.kde.Oxygen.Style" ), QStringLiteral( "reparseConfiguration" ) ) );
        QDBusConnection::sessionBus().send(message);

    }

    //__________________________________________________________________
    void StyleConfig::defaults( void )
    {
        StyleConfigData::self()->setDefaults();
        load();
    }

    //__________________________________________________________________
    void StyleConfig::reset( void )
    {
        // reparse configuration
        #if OXYGEN_USE_KDE4
        StyleConfigData::self()->readConfig();
        #else
        StyleConfigData::self()->load();
        #endif

        load();
    }

    //__________________________________________________________________
    void StyleConfig::toggleExpertMode( bool value )
    {
        _expertModeContainer->hide();
        toggleExpertModeInternal( value );
    }

    //__________________________________________________________________
    void StyleConfig::toggleExpertModeInternal( bool value )
    {

        // store value
        _expertMode = value;

        // update button text
        _expertModeButton->setText( _expertMode ? i18n( "Hide Advanced Configuration Options" ):i18n( "Show Advanced Configuration Options" ) );

        // update widget visibility based on expert mode
        if( _expertMode )
        {

            // create animationConfigWidget if needed
            if( !_animationConfigWidget )
            {
                _animationConfigWidget = new AnimationConfigWidget();
                _animationConfigWidget->installEventFilter( this );
                connect( _animationConfigWidget, SIGNAL(changed(bool)), SLOT(updateChanged()) );
                connect( _animationConfigWidget, SIGNAL(layoutChanged()), SLOT(updateLayout()) );
                _animationConfigWidget->load();
            }

            // add animationConfigWidget to tabbar if needed
            if( tabWidget->indexOf( _animationConfigWidget ) < 0 )
            { tabWidget->insertTab( 1, _animationConfigWidget, i18n( "Animations" ) ); }

        } else if( _animationConfigWidget ) {

            if( int index = tabWidget->indexOf( _animationConfigWidget ) >= 0 )
            { tabWidget->removeTab( index ); }

        }

        _mnemonicsLabel->setVisible( _expertMode );
        _mnemonicsMode->setVisible( _expertMode );
        _animationsEnabled->setVisible( !_expertMode );
        _viewsExpertWidget->setVisible( _expertMode );

        updateMinimumSize();

    }

    //__________________________________________________________________
    bool StyleConfig::eventFilter( QObject* object, QEvent* event )
    {

        switch( event->type() )
        {

            case QEvent::ShowToParent:
            object->event( event );
            updateLayout();
            return true;

            default:
            return false;
        }
    }

    //__________________________________________________________________
    bool StyleConfig::event( QEvent* event )
    {
        const bool result( QWidget::event( event ) );
        switch( event->type() )
        {
            case QEvent::Show:
            case QEvent::ShowToParent:
            updateMinimumSize();
            break;

            default: break;
        }

        return result;

    }

    //__________________________________________________________________
    void StyleConfig::updateMinimumSize( void )
    { setMinimumSize( minimumSizeHint() ); }

    //__________________________________________________________________
    void StyleConfig::updateLayout( void )
    {
        if( _animationConfigWidget )
        {
            const int delta = _animationConfigWidget->minimumSizeHint().height() - _animationConfigWidget->size().height();
            window()->setMinimumSize( QSize( window()->minimumSizeHint().width(), window()->size().height() + delta ) );
        }
    }

    //__________________________________________________________________
    void StyleConfig::updateChanged()
    {

        bool modified( false );

        // check if any value was modified
        if ( _useBackgroundGradient->isChecked() != StyleConfigData::useBackgroundGradient() ) modified = true;
        if ( _toolBarDrawItemSeparator->isChecked() != StyleConfigData::toolBarDrawItemSeparator() ) modified = true;
        else if( _mnemonicsMode->currentIndex() != StyleConfigData::mnemonicsMode() ) modified = true;
        else if( _viewDrawTreeBranchLines->isChecked() != StyleConfigData::viewDrawTreeBranchLines() ) modified = true;
        else if( _scrollBarWidth->value() != StyleConfigData::scrollBarWidth() ) modified = true;
        else if( _scrollBarAddLineButtons->currentIndex() != StyleConfigData::scrollBarAddLineButtons() ) modified = true;
        else if( _scrollBarSubLineButtons->currentIndex() != StyleConfigData::scrollBarSubLineButtons() ) modified = true;
        else if( _splitterProxyEnabled->isChecked() != StyleConfigData::splitterProxyEnabled() ) modified = true;
        else if( menuMode() != StyleConfigData::menuHighlightMode() ) modified = true;
        else if( _animationsEnabled->isChecked() != StyleConfigData::animationsEnabled() ) modified = true;
        else if( _viewDrawFocusIndicator->isChecked() != StyleConfigData::viewDrawFocusIndicator() ) modified = true;
        else if( triangularExpanderSize() != StyleConfigData::viewTriangularExpanderSize() ) modified = true;
        else if( _animationConfigWidget && _animationConfigWidget->isChanged() ) modified = true;
        else if( _windowDragMode->currentIndex() != StyleConfigData::windowDragMode() ) modified = true;

        emit changed(modified);

    }

    //__________________________________________________________________
    void StyleConfig::load( void )
    {

        _useBackgroundGradient->setChecked( StyleConfigData::useBackgroundGradient() );
        _toolBarDrawItemSeparator->setChecked( StyleConfigData::toolBarDrawItemSeparator() );
        _mnemonicsMode->setCurrentIndex( StyleConfigData::mnemonicsMode() );
        _splitterProxyEnabled->setChecked( StyleConfigData::splitterProxyEnabled() );
        _viewDrawTreeBranchLines->setChecked(StyleConfigData::viewDrawTreeBranchLines() );

        _scrollBarWidth->setValue(
            qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH,
            StyleConfigData::scrollBarWidth())) );

        _scrollBarAddLineButtons->setCurrentIndex( StyleConfigData::scrollBarAddLineButtons() );
        _scrollBarSubLineButtons->setCurrentIndex( StyleConfigData::scrollBarSubLineButtons() );

        // menu highlight
        _menuHighlightDark->setChecked( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_DARK );
        _menuHighlightStrong->setChecked( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_STRONG );
        _menuHighlightSubtle->setChecked( StyleConfigData::menuHighlightMode() == StyleConfigData::MM_SUBTLE );

        _animationsEnabled->setChecked( StyleConfigData::animationsEnabled() );
        _windowDragMode->setCurrentIndex( StyleConfigData::windowDragMode() );

        _viewDrawFocusIndicator->setChecked( StyleConfigData::viewDrawFocusIndicator() );
        switch( StyleConfigData::viewTriangularExpanderSize() )
        {
            case StyleConfigData::TE_TINY: _viewTriangularExpanderSize->setCurrentIndex(0); break;
            case StyleConfigData::TE_SMALL: default: _viewTriangularExpanderSize->setCurrentIndex(1); break;
            case StyleConfigData::TE_NORMAL: _viewTriangularExpanderSize->setCurrentIndex(2); break;
        }

        // animation config widget
        if( _animationConfigWidget ) _animationConfigWidget->load();

    }

    //____________________________________________________________
    int StyleConfig::menuMode( void ) const
    {
        if (_menuHighlightDark->isChecked()) return StyleConfigData::MM_DARK;
        else if (_menuHighlightSubtle->isChecked()) return StyleConfigData::MM_SUBTLE;
        else return StyleConfigData::MM_STRONG;
    }

    //____________________________________________________________
    int StyleConfig::triangularExpanderSize( void ) const
    {
        switch( _viewTriangularExpanderSize->currentIndex() )
        {
            case 0: return StyleConfigData::TE_TINY;
            case 1: default: return StyleConfigData::TE_SMALL;
            case 2: return StyleConfigData::TE_NORMAL;
        }

    }

}
