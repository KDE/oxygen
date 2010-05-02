/*
Copyright 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
#include "oxygenstyleconfig.moc"
#include "oxygenanimationconfigwidget.h"
#include "oxygenstyleconfigdata.h"

#include <QtCore/QTextStream>

#include <KGlobal>
#include <KLocale>
#include <KSharedConfig>
#include <KConfigGroup>
#include <kdemacros.h>
#include <KDialog>

#define SCROLLBAR_DEFAULT_WIDTH 15
#define SCROLLBAR_MINIMUM_WIDTH 10
#define SCROLLBAR_MAXIMUM_WIDTH 30

extern "C"
{
    KDE_EXPORT QWidget* allocate_kstyle_config(QWidget* parent)
    {
        KGlobal::locale()->insertCatalog("kstyle_config");
        return new Oxygen::StyleConfig(parent);
    }
}

namespace Oxygen
{

    //__________________________________________________________________
    StyleConfig::StyleConfig(QWidget* parent):
        QWidget(parent),
        _expertMode( false ),
        _animationConfigWidget(0)
    {
        KGlobal::locale()->insertCatalog("kstyle_config");

        if( KDialog* dialog = qobject_cast<KDialog*>( parent ) )
        { dialog->showButtonSeparator( false ); }

        /* Stop 1+2: Set up the UI */
        setupUi(this);

        // connections
        connect( _animationsEnabled, SIGNAL( toggled(bool) ), _stackedWidgetTransitionsEnabled, SLOT( setEnabled( bool) ) );
        connect( _windowDragMode, SIGNAL( currentIndexChanged( int ) ), SLOT( windowDragModeChanged( int ) ) );
        connect( _viewDrawTriangularExpander, SIGNAL( toggled( bool ) ), _viewTriangularExpanderSize, SLOT( setEnabled( bool ) ) );

        // toggle expert mode
        toggleExpertMode( false );

        // load setup from configData
        load();

        /* Stop 4: Emit a signal on changes */
        connect( tabWidget, SIGNAL( currentChanged( int ) ), SLOT( currentTabChanged( int ) ) );
        connect( _animationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _toolBarDrawItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _checkDrawX, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _viewDrawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _viewTriangularExpanderSize, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( _viewDrawFocusIndicator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _viewDrawTreeBranchLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _scrollBarColored, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _scrollBarBevel, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _scrollBarWidth, SIGNAL( valueChanged(int) ), SLOT( updateChanged() ) );
        connect( _menuHighlightDark, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _menuHighlightStrong, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _menuHighlightSubtle, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
        connect( _tabStylePlain, SIGNAL( toggled(bool)), SLOT( updateChanged() ) );
        connect( _tabStyleSingle, SIGNAL( toggled(bool)), SLOT( updateChanged() ) );
        connect( _windowDragMode, SIGNAL( currentIndexChanged( int ) ), SLOT( updateChanged() ) );
        connect( _useWMMoveResize, SIGNAL( toggled( bool ) ), SLOT( updateChanged() ) );
        connect( _stackedWidgetTransitionsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );

    }

    //__________________________________________________________________
    void StyleConfig::save( void )
    {
        /* Stop 5: Save the configuration */
        OxygenStyleConfigData::setToolBarDrawItemSeparator( _toolBarDrawItemSeparator->isChecked() );
        OxygenStyleConfigData::setCheckBoxStyle( ( _checkDrawX->isChecked() ? OxygenStyleConfigData::CS_X : OxygenStyleConfigData::CS_CHECK ) );
        OxygenStyleConfigData::setViewDrawTriangularExpander( _viewDrawTriangularExpander->isChecked() );
        OxygenStyleConfigData::setViewTriangularExpanderSize( triangularExpanderSize() );
        OxygenStyleConfigData::setViewDrawFocusIndicator( _viewDrawFocusIndicator->isChecked() );
        OxygenStyleConfigData::setViewDrawTreeBranchLines( _viewDrawTreeBranchLines->isChecked() );
        OxygenStyleConfigData::setScrollBarColored( _scrollBarColored->isChecked() );
        OxygenStyleConfigData::setScrollBarBevel( _scrollBarBevel->isChecked() );
        OxygenStyleConfigData::setScrollBarWidth( _scrollBarWidth->value() );
        OxygenStyleConfigData::setMenuHighlightMode( menuMode() );
        OxygenStyleConfigData::setTabStyle( tabStyle() );
        OxygenStyleConfigData::setViewTriangularExpanderSize( triangularExpanderSize() );

        if( _expertMode )
        {

            _animationConfigWidget->save();

        } else {
            OxygenStyleConfigData::setAnimationsEnabled( _animationsEnabled->isChecked() );
            OxygenStyleConfigData::setStackedWidgetTransitionsEnabled( _stackedWidgetTransitionsEnabled->isChecked() );
        }

        OxygenStyleConfigData::setUseWMMoveResize( _useWMMoveResize->isChecked() );
        if( _windowDragMode->currentIndex() == 0 )
        {

            OxygenStyleConfigData::setWindowDragEnabled( false  );

        } else {

            OxygenStyleConfigData::setWindowDragEnabled( true  );
            OxygenStyleConfigData::setWindowDragMode( windowDragMode()  );

        }

        OxygenStyleConfigData::self()->writeConfig();
    }

    //__________________________________________________________________
    void StyleConfig::defaults( void )
    {
        OxygenStyleConfigData::self()->setDefaults();
        load();
    }

    //__________________________________________________________________
    void StyleConfig::reset( void )
    {
        OxygenStyleConfigData::self()->readConfig();
        load();
    }

    //__________________________________________________________________
    void StyleConfig::toggleExpertMode( bool value )
    {

        _expertMode = value;

        // update widget visibility based on expert mode
        if( _expertMode )
        {

            // create animationConfigWidget if needed
            if( !_animationConfigWidget )
            {
                _animationConfigWidget = new AnimationConfigWidget();
                connect( _animationConfigWidget, SIGNAL( changed( bool ) ), SLOT( updateChanged() ) );
                connect( _animationConfigWidget, SIGNAL( layoutChanged( void ) ), SLOT( updateLayout() ) );
                _animationConfigWidget->load();
            }

            // add animationConfigWidget to tabbar if needed
            if( tabWidget->indexOf( _animationConfigWidget ) < 0 )
            { tabWidget->insertTab( 1, _animationConfigWidget, i18n( "Animations" ) ); }

        } else if( _animationConfigWidget ) {

            if( int index =tabWidget->indexOf( _animationConfigWidget ) >= 0 )
            { tabWidget->removeTab( index ); }

        }

        _animationsEnabled->setVisible( !_expertMode );
        _stackedTransitionWidget->setVisible( !_expertMode );
        _generalExpertWidget->setVisible( _expertMode );
        _viewsExpertWidget->setVisible( _expertMode );

    }

    //__________________________________________________________________
    void StyleConfig::updateLayout( void )
    {

        if( !_animationConfigWidget ) return;
        int delta = _animationConfigWidget->minimumSizeHint().height() - _animationConfigWidget->size().height();
        window()->setMinimumSize( QSize( window()->minimumSizeHint().width(), window()->size().height() + delta ) );

    }

    //__________________________________________________________________
    void StyleConfig::updateChanged()
    {

        bool modified( false );

        // check if any value was modified
        if ( _toolBarDrawItemSeparator->isChecked() != OxygenStyleConfigData::toolBarDrawItemSeparator() ) modified = true;
        else if( _viewDrawTriangularExpander->isChecked() != OxygenStyleConfigData::viewDrawTriangularExpander() ) modified = true;
        else if( _viewDrawFocusIndicator->isChecked() != OxygenStyleConfigData::viewDrawFocusIndicator() ) modified = true;
        else if( _viewDrawTreeBranchLines->isChecked() != OxygenStyleConfigData::viewDrawTreeBranchLines() ) modified = true;
        else if( _scrollBarColored->isChecked() != OxygenStyleConfigData::scrollBarColored() ) modified = true;
        else if( _scrollBarBevel->isChecked() != OxygenStyleConfigData::scrollBarBevel() ) modified = true;
        else if( _scrollBarWidth->value() != OxygenStyleConfigData::scrollBarWidth() ) modified = true;
        else if( (_checkDrawX->isChecked() ? OxygenStyleConfigData::CS_X : OxygenStyleConfigData::CS_CHECK) != OxygenStyleConfigData::checkBoxStyle() ) modified = true;
        else if( menuMode() != OxygenStyleConfigData::menuHighlightMode() ) modified = true;
        else if( tabStyle() != OxygenStyleConfigData::tabStyle() ) modified = true;
        else if( _animationsEnabled->isChecked() != OxygenStyleConfigData::animationsEnabled() ) modified = true;
        else if( _stackedWidgetTransitionsEnabled->isChecked() != OxygenStyleConfigData::stackedWidgetTransitionsEnabled() ) modified = true;
        else if( _useWMMoveResize->isChecked() != OxygenStyleConfigData::useWMMoveResize() ) modified = true;
        else if( triangularExpanderSize() != OxygenStyleConfigData::viewTriangularExpanderSize() ) modified = true;
        else if( _animationConfigWidget && _animationConfigWidget->isChanged() ) modified = true;

        if( !modified )
        {
            switch( _windowDragMode->currentIndex() )
            {
                case 0:
                {
                    if( OxygenStyleConfigData::windowDragEnabled() ) modified = true;
                    break;
                }

                case 1:
                case 2:
                default:
                {
                    if( !OxygenStyleConfigData::windowDragEnabled() || windowDragMode() != OxygenStyleConfigData::windowDragMode() )
                    { modified = true; }
                    break;
                }

            }
        }

        emit changed(modified);

    }

    //__________________________________________________________________
    void StyleConfig::currentTabChanged( int index )
    {
        if( _animationConfigWidget && index == tabWidget->indexOf( _animationConfigWidget ) )
        { updateLayout(); }

    }

    //__________________________________________________________________
    void StyleConfig::load( void )
    {

        /* Stop 3: Set up the configuration struct and your widget */
        _toolBarDrawItemSeparator->setChecked( OxygenStyleConfigData::toolBarDrawItemSeparator() );
        _checkDrawX->setChecked( OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_X );
        _viewDrawTriangularExpander->setChecked( OxygenStyleConfigData::viewDrawTriangularExpander() );
        _viewDrawFocusIndicator->setChecked( OxygenStyleConfigData::viewDrawFocusIndicator() );
        _viewDrawTreeBranchLines->setChecked(OxygenStyleConfigData::viewDrawTreeBranchLines() );

        _scrollBarWidth->setValue(
            qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH,
            OxygenStyleConfigData::scrollBarWidth())) );
        _scrollBarColored->setChecked( OxygenStyleConfigData::scrollBarColored() );
        _scrollBarBevel->setChecked( OxygenStyleConfigData::scrollBarBevel() );

        // menu highlight
        _menuHighlightDark->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_DARK );
        _menuHighlightStrong->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG );
        _menuHighlightSubtle->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );

        // tab style
        _tabStyleSingle->setChecked( OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_SINGLE );
        _tabStylePlain->setChecked( OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_PLAIN );

        _stackedWidgetTransitionsEnabled->setChecked( OxygenStyleConfigData::stackedWidgetTransitionsEnabled() );
        _stackedWidgetTransitionsEnabled->setEnabled( false );

        _animationsEnabled->setChecked( OxygenStyleConfigData::animationsEnabled() );

        if( !OxygenStyleConfigData::windowDragEnabled() ) _windowDragMode->setCurrentIndex(0);
        else if( OxygenStyleConfigData::windowDragMode() == OxygenStyleConfigData::WD_MINIMAL ) _windowDragMode->setCurrentIndex(1);
        else _windowDragMode->setCurrentIndex(2);

        switch( OxygenStyleConfigData::viewTriangularExpanderSize() )
        {
            case OxygenStyleConfigData::TE_TINY: _viewTriangularExpanderSize->setCurrentIndex(0); break;
            case OxygenStyleConfigData::TE_SMALL: default: _viewTriangularExpanderSize->setCurrentIndex(1); break;
            case OxygenStyleConfigData::TE_NORMAL: _viewTriangularExpanderSize->setCurrentIndex(2); break;
        }

        _useWMMoveResize->setChecked( OxygenStyleConfigData::useWMMoveResize() );

        // animation config widget
        if( _animationConfigWidget ) _animationConfigWidget->load();

    }

    //__________________________________________________________________
    void StyleConfig::windowDragModeChanged( int value )
    { _useWMMoveResize->setEnabled( value != 0 ); }

    //____________________________________________________________
    int StyleConfig::menuMode( void ) const
    {
        if (_menuHighlightDark->isChecked()) return OxygenStyleConfigData::MM_DARK;
        else if (_menuHighlightSubtle->isChecked()) return OxygenStyleConfigData::MM_SUBTLE;
        else return OxygenStyleConfigData::MM_STRONG;
    }

    //____________________________________________________________
    int StyleConfig::tabStyle( void ) const
    {
        if( _tabStylePlain->isChecked() ) return OxygenStyleConfigData::TS_PLAIN;
        else return OxygenStyleConfigData::TS_SINGLE;
    }

    //____________________________________________________________
    int StyleConfig::windowDragMode( void ) const
    {
        switch( _windowDragMode->currentIndex() )
        {
            case 1: return OxygenStyleConfigData::WD_MINIMAL;
            case 2: default: return OxygenStyleConfigData::WD_FULL;
        }
    }

    //____________________________________________________________
    int StyleConfig::triangularExpanderSize( void ) const
    {
        switch( _viewTriangularExpanderSize->currentIndex() )
        {
            case 0: return OxygenStyleConfigData::TE_TINY;
            case 1: default: return OxygenStyleConfigData::TE_SMALL;
            case 2: return OxygenStyleConfigData::TE_NORMAL;
        }

    }

}
