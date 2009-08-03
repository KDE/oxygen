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

    /* Stop 1+2: Set up the UI */
    setupUi(this);

    /* Stop 3: Set up the configuration struct and your widget */
    _toolBarDrawItemSeparator->setChecked( OxygenStyleConfigData::toolBarDrawItemSeparator() );
    _checkDrawX->setChecked( OxygenStyleConfigData::checkBoxStyle() == OxygenStyleConfigData::CS_X );
    _viewDrawTriangularExpander->setChecked( OxygenStyleConfigData::viewDrawTriangularExpander() );
    _viewDrawTreeBranchLines->setChecked(OxygenStyleConfigData::viewDrawTreeBranchLines() );
    _scrollBarWidth->setValue( qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH,
                                OxygenStyleConfigData::scrollBarWidth())) );
    _scrollBarColored->setChecked( OxygenStyleConfigData::scrollBarColored() );
    _menuHighlightDark->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_DARK );
    _menuHighlightStrong->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG
                                   || OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );
    _menuHighlightSubtle->setChecked( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );
    _menuHighlightSubtle->setEnabled( OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_STRONG
                                   || OxygenStyleConfigData::menuHighlightMode() == OxygenStyleConfigData::MM_SUBTLE );


    /* Stop 4: Emit a signal on changes */
    connect( _toolBarDrawItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _checkDrawX, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _viewDrawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _viewDrawTreeBranchLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _scrollBarColored, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _scrollBarWidth, SIGNAL( valueChanged(int) ), SLOT( updateChanged() ) );
    connect( _menuHighlightDark, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _menuHighlightStrong, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _menuHighlightSubtle, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
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
    OxygenStyleConfigData::setViewDrawTreeBranchLines( _viewDrawTreeBranchLines->isChecked() );
    OxygenStyleConfigData::setScrollBarColored( _scrollBarColored->isChecked() );
    OxygenStyleConfigData::setScrollBarWidth( _scrollBarWidth->value() );
    OxygenStyleConfigData::setMenuHighlightMode( menuMode() );

    OxygenStyleConfigData::self()->writeConfig();
}

void OxygenStyleConfig::defaults()
{
    /* Stop 6: Set defaults */
    _toolBarDrawItemSeparator->setChecked(true);
    _checkDrawX->setChecked(false);
    _viewDrawTriangularExpander->setChecked(false);
    _viewDrawTreeBranchLines->setChecked(true);
    _scrollBarColored->setChecked(false);
    _scrollBarWidth->setValue(SCROLLBAR_DEFAULT_WIDTH);
    _menuHighlightSubtle->setChecked(false);
    _menuHighlightDark->setChecked(true);
    //updateChanged would be done by setChecked already
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

void OxygenStyleConfig::updateChanged()
{
    /* Stop 7: Check if some value changed */
    if (
        (_toolBarDrawItemSeparator->isChecked() == OxygenStyleConfigData::toolBarDrawItemSeparator())
        && (_viewDrawTriangularExpander->isChecked() == OxygenStyleConfigData::viewDrawTriangularExpander())
        && (_viewDrawTreeBranchLines->isChecked() == OxygenStyleConfigData::viewDrawTreeBranchLines())
        && (_scrollBarColored->isChecked() == OxygenStyleConfigData::scrollBarColored())
        && (_scrollBarWidth->value() == OxygenStyleConfigData::scrollBarWidth())
        && ((_checkDrawX->isChecked() ? OxygenStyleConfigData::CS_X : OxygenStyleConfigData::CS_CHECK) == OxygenStyleConfigData::checkBoxStyle())
        && (menuMode() == OxygenStyleConfigData::menuHighlightMode())
        )
        emit changed(false);
    else
        emit changed(true);
}

#include "oxygenconf.moc"
