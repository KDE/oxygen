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

    _menuHighlightDark->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_DARK );

    _menuHighlightStrong->setChecked(
        OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG
        || OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );

    _menuHighlightSubtle->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );

    _menuHighlightSubtle->setEnabled(
        OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG
        || OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );

    _tabStyle->setCurrentIndex(
        OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_SINGLE ? 0 :
        OxygenStyleConfigData::tabStyle() == OxygenStyleConfigData::TS_PLAIN ? 1 :
        0);

    _genericAnimationsEnabled->setEnabled( false );
    _toolBarAnimationsEnabled->setEnabled( false );
    _menuBarAnimationsEnabled->setEnabled( false );
    _menuAnimationsEnabled->setEnabled( false );
    _progressBarAnimationsEnabled->setEnabled( false );
    _stackedWidgetTransitionsEnabled->setEnabled( false );
    _labelTransitionsEnabled->setEnabled( false );

    connect( _animationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _genericAnimationsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _toolBarAnimationsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _menuBarAnimationsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _menuAnimationsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _progressBarAnimationsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _stackedWidgetTransitionsEnabled, SLOT( setEnabled( bool) ) );
    connect( _animationsEnabled, SIGNAL( toggled(bool) ), _labelTransitionsEnabled, SLOT( setEnabled( bool) ) );

    _animationsEnabled->setChecked( OxygenStyleConfigData::animationsEnabled() );
    _genericAnimationsEnabled->setChecked( OxygenStyleConfigData::genericAnimationsEnabled() );
    _toolBarAnimationsEnabled->setChecked( OxygenStyleConfigData::toolBarAnimationsEnabled() );
    _menuBarAnimationsEnabled->setChecked( OxygenStyleConfigData::menuBarAnimationsEnabled() );
    _menuAnimationsEnabled->setChecked( OxygenStyleConfigData::menuAnimationsEnabled() );
    _progressBarAnimationsEnabled->setChecked( OxygenStyleConfigData::progressBarAnimationsEnabled() );
    _stackedWidgetTransitionsEnabled->setChecked( OxygenStyleConfigData::stackedWidgetTransitionsEnabled() );
    _labelTransitionsEnabled->setChecked( OxygenStyleConfigData::labelTransitionsEnabled() );

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
    connect( _tabStyle, SIGNAL( currentIndexChanged( int )), SLOT( updateChanged() ) );

    connect( _genericAnimationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _toolBarAnimationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _menuBarAnimationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _menuAnimationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _progressBarAnimationsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _stackedWidgetTransitionsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _labelTransitionsEnabled, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );

 }

OxygenStyleConfig::~OxygenStyleConfig()
{
}

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

    OxygenStyleConfigData::setAnimationsEnabled( _animationsEnabled->isChecked() );
    OxygenStyleConfigData::setGenericAnimationsEnabled( _genericAnimationsEnabled->isChecked() );
    OxygenStyleConfigData::setToolBarAnimationsEnabled( _toolBarAnimationsEnabled->isChecked() );
    OxygenStyleConfigData::setMenuBarAnimationsEnabled( _menuBarAnimationsEnabled->isChecked() );
    OxygenStyleConfigData::setMenuAnimationsEnabled( _menuAnimationsEnabled->isChecked() );
    OxygenStyleConfigData::setProgressBarAnimationsEnabled( _progressBarAnimationsEnabled->isChecked() );
    OxygenStyleConfigData::setStackedWidgetTransitionsEnabled( _stackedWidgetTransitionsEnabled->isChecked() );
    OxygenStyleConfigData::setLabelTransitionsEnabled( _labelTransitionsEnabled->isChecked() );

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
    _menuHighlightSubtle->setChecked(false);
    _menuHighlightDark->setChecked(true);
    _tabStyle->setCurrentIndex(0);


     _animationsEnabled->setChecked( true );
     _genericAnimationsEnabled->setChecked( true );
     _toolBarAnimationsEnabled->setChecked( true );
     _menuBarAnimationsEnabled->setChecked( false );
     _menuAnimationsEnabled->setChecked( false );
     _progressBarAnimationsEnabled->setChecked( false );
     _stackedWidgetTransitionsEnabled->setChecked( false );
     _labelTransitionsEnabled->setChecked( false );

    //updateChanged would be done by setChecked already

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
        && (_animationsEnabled->isChecked() == OxygenStyleConfigData::animationsEnabled() )
        && (_genericAnimationsEnabled->isChecked() == OxygenStyleConfigData::genericAnimationsEnabled() )
        && (_toolBarAnimationsEnabled->isChecked() == OxygenStyleConfigData::toolBarAnimationsEnabled() )
        && (_menuBarAnimationsEnabled->isChecked() == OxygenStyleConfigData::menuBarAnimationsEnabled() )
        && (_menuAnimationsEnabled->isChecked() == OxygenStyleConfigData::menuAnimationsEnabled() )
        && (_progressBarAnimationsEnabled->isChecked() == OxygenStyleConfigData::progressBarAnimationsEnabled() )
        && (_stackedWidgetTransitionsEnabled->isChecked() == OxygenStyleConfigData::stackedWidgetTransitionsEnabled() )
        && (_labelTransitionsEnabled->isChecked() == OxygenStyleConfigData::labelTransitionsEnabled() )
        )
        emit changed(false);
    else
        emit changed(true);
}

int OxygenStyleConfig::menuMode() const
{
    if (_menuHighlightDark->isChecked())
        return OxygenStyleConfigData::MM_DARK;
    else if (_menuHighlightSubtle->isChecked())
        return OxygenStyleConfigData::MM_SUBTLE;
    else
        return OxygenStyleConfigData::MM_STRONG;
}

int OxygenStyleConfig::tabStyle() const
{
    switch (_tabStyle->currentIndex()) {
        case 0: return OxygenStyleConfigData::TS_SINGLE;
        case 1:
        default:
                return OxygenStyleConfigData::TS_PLAIN;
    }
}

#include "oxygenconf.moc"
