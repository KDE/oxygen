/*
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

#include "oxygenconf.h"
#include "oxygenstyleconfigdata.h"

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
        return new OxygenStyleConfig(parent);
    }
}

OxygenStyleConfig::OxygenStyleConfig(QWidget* parent): QWidget(parent)
{
    KGlobal::locale()->insertCatalog("kstyle_config");

    if( KDialog* dialog = qobject_cast<KDialog*>( parent ) )
    { dialog->showButtonSeparator( false ); }


    /* Stop 1+2: Set up the UI */
    setupUi(this);

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


    // animations
    // menubar animation type
    switch( OxygenStyleConfigData::menuBarAnimationType() )
    {
        case OxygenStyleConfigData::MB_NONE:
        {
            _menuBarAnimationType->setCurrentIndex(0);
            break;
        }

        case OxygenStyleConfigData::MB_FOLLOW_MOUSE:
        {
            _menuBarAnimationType->setCurrentIndex(2);
            break;
        }

        default:
        case OxygenStyleConfigData::MB_FADE:
        {
            _menuBarAnimationType->setCurrentIndex(1);
            break;
        }

    }

    _menuBarAnimationType->setEnabled( false );

    // menu animation type
    switch( OxygenStyleConfigData::menuAnimationType() )
    {
        case OxygenStyleConfigData::ME_NONE:
        {
            _menuAnimationType->setCurrentIndex(0);
            break;
        }

        case OxygenStyleConfigData::ME_FOLLOW_MOUSE:
        {
            _menuAnimationType->setCurrentIndex(2);
            break;
        }

        default:
        case OxygenStyleConfigData::ME_FADE:
        {
            _menuAnimationType->setCurrentIndex(1);
            break;
        }

    }
    _menuAnimationType->setEnabled( false );

    _stackedWidgetTransitionsEnabled->setChecked( OxygenStyleConfigData::stackedWidgetTransitionsEnabled() );
    _stackedWidgetTransitionsEnabled->setEnabled( false );


    connect( _animationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _stackedWidgetTransitionsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _menuBarAnimationType, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _menuAnimationType, SLOT( setEnabled( bool) ) );
    _animationsEnabled->setChecked( OxygenStyleConfigData::animationsEnabled() );

    /* Stop 4: Emit a signal on changes */
    connect( _toolBarDrawItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _checkDrawX, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _viewDrawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
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
    connect( _menuBarAnimationType, SIGNAL( currentIndexChanged( int )), SLOT( updateChanged() ) );
    connect( _menuAnimationType, SIGNAL( currentIndexChanged( int )), SLOT( updateChanged() ) );

    connect( _stackedWidgetTransitionsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );

 }

OxygenStyleConfig::~OxygenStyleConfig()
{}

void OxygenStyleConfig::save()
{
    /* Stop 5: Save the configuration */
    OxygenStyleConfigData::setToolBarDrawItemSeparator( _toolBarDrawItemSeparator->isChecked() );
    OxygenStyleConfigData::setCheckBoxStyle( ( _checkDrawX->isChecked() ? OxygenStyleConfigData::CS_X : OxygenStyleConfigData::CS_CHECK ) );
    OxygenStyleConfigData::setViewDrawTriangularExpander( _viewDrawTriangularExpander->isChecked() );
    OxygenStyleConfigData::setViewDrawFocusIndicator( _viewDrawFocusIndicator->isChecked() );
    OxygenStyleConfigData::setViewDrawTreeBranchLines( _viewDrawTreeBranchLines->isChecked() );
    OxygenStyleConfigData::setScrollBarColored( _scrollBarColored->isChecked() );
    OxygenStyleConfigData::setScrollBarBevel( _scrollBarBevel->isChecked() );
    OxygenStyleConfigData::setScrollBarWidth( _scrollBarWidth->value() );
    OxygenStyleConfigData::setMenuHighlightMode( menuMode() );
    OxygenStyleConfigData::setTabStyle( tabStyle() );
    OxygenStyleConfigData::setMenuBarAnimationType( menuBarAnimationType() );
    OxygenStyleConfigData::setMenuAnimationType( menuAnimationType() );

    OxygenStyleConfigData::setAnimationsEnabled( _animationsEnabled->isChecked() );
    OxygenStyleConfigData::setStackedWidgetTransitionsEnabled( _stackedWidgetTransitionsEnabled->isChecked() );

    OxygenStyleConfigData::self()->writeConfig();
}

void OxygenStyleConfig::defaults()
{
    /* Stop 6: Set defaults */
    _toolBarDrawItemSeparator->setChecked(true);
    _checkDrawX->setChecked(false);
    _viewDrawTriangularExpander->setChecked(false);
    _viewDrawFocusIndicator->setChecked(true);
    _viewDrawTreeBranchLines->setChecked(true);
    _scrollBarColored->setChecked(false);
    _scrollBarBevel->setChecked(true);
    _scrollBarWidth->setValue(SCROLLBAR_DEFAULT_WIDTH);

    _menuHighlightDark->setChecked(true);
    _tabStyleSingle->setChecked(true);
    _menuBarAnimationType->setCurrentIndex(1);
    _menuAnimationType->setCurrentIndex(1);

    _animationsEnabled->setChecked( true );
    _stackedWidgetTransitionsEnabled->setChecked( false );

}

void OxygenStyleConfig::updateChanged()
{
    /* Stop 7: Check if some value changed */
    if (
        (_toolBarDrawItemSeparator->isChecked() == OxygenStyleConfigData::toolBarDrawItemSeparator())
        && (_viewDrawTriangularExpander->isChecked() == OxygenStyleConfigData::viewDrawTriangularExpander())
        && (_viewDrawFocusIndicator->isChecked() == OxygenStyleConfigData::viewDrawFocusIndicator())
        && (_viewDrawTreeBranchLines->isChecked() == OxygenStyleConfigData::viewDrawTreeBranchLines())
        && (_scrollBarColored->isChecked() == OxygenStyleConfigData::scrollBarColored())
        && (_scrollBarBevel->isChecked() == OxygenStyleConfigData::scrollBarBevel())
        && (_scrollBarWidth->value() == OxygenStyleConfigData::scrollBarWidth())
        && ((_checkDrawX->isChecked() ? OxygenStyleConfigData::CS_X : OxygenStyleConfigData::CS_CHECK) == OxygenStyleConfigData::checkBoxStyle())
        && (menuMode() == OxygenStyleConfigData::menuHighlightMode())
        && (tabStyle() == OxygenStyleConfigData::tabStyle())
        && (menuBarAnimationType() == OxygenStyleConfigData::menuBarAnimationType())
        && (menuAnimationType() == OxygenStyleConfigData::menuAnimationType())
        && (_animationsEnabled->isChecked() == OxygenStyleConfigData::animationsEnabled() )
        && (_stackedWidgetTransitionsEnabled->isChecked() == OxygenStyleConfigData::stackedWidgetTransitionsEnabled() )
        )
        emit changed(false);
    else
        emit changed(true);
}

//____________________________________________________________
int OxygenStyleConfig::menuMode( void ) const
{
    if (_menuHighlightDark->isChecked()) return OxygenStyleConfigData::MM_DARK;
    else if (_menuHighlightSubtle->isChecked()) return OxygenStyleConfigData::MM_SUBTLE;
    else return OxygenStyleConfigData::MM_STRONG;
}

//____________________________________________________________
int OxygenStyleConfig::tabStyle( void ) const
{
    if( _tabStylePlain->isChecked() ) return OxygenStyleConfigData::TS_PLAIN;
    else return OxygenStyleConfigData::TS_SINGLE;
}

//____________________________________________________________
int OxygenStyleConfig::menuBarAnimationType( void ) const
{
    switch (_menuBarAnimationType->currentIndex())
    {
        case 0: return OxygenStyleConfigData::MB_NONE;
        case 2: return OxygenStyleConfigData::MB_FOLLOW_MOUSE;
        case 1:
        default:
        return OxygenStyleConfigData::MB_FADE;
    }
}

//____________________________________________________________
int OxygenStyleConfig::menuAnimationType( void ) const
{
    switch (_menuAnimationType->currentIndex())
    {
        case 0: return OxygenStyleConfigData::ME_NONE;
        case 2: return OxygenStyleConfigData::ME_FOLLOW_MOUSE;
        case 1:
        default:
        return OxygenStyleConfigData::ME_FADE;
    }
}
#include "oxygenconf.moc"
