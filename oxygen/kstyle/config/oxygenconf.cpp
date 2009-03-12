/*
Copyright (C) 2009 Long Huynh Huu <long.upcase@googlemail.com>
Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>

based on the Keramik configuration dialog:
Copyright (c) 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

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

#ifndef QT3_SUPPORT
#define QT3_SUPPORT
#endif
#include <QtGui/QCheckBox>
#include <knuminput.h>
#include <QtGui/QLayout>
#include <khbox.h>
#include <QtGui/QColor>
#include <KGlobal>
#include <KLocale>
#include <KColorButton>
#include <KComponentData>
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
    //Should have no margins here, the dialog provides them
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    KGlobal::locale()->insertCatalog("kstyle_config");

    /* Stop 1: Add a widget */
    _toolBarDrawItemSeparator = new QCheckBox(i18n("Draw toolbar item separators"), this);
    _viewDrawTriangularExpander = new QCheckBox(i18n("Triangular tree expander"), this);
    _viewDrawTreeBranchLines = new QCheckBox(i18n("Draw tree branch lines"), this);
    _scrollBarWidth = new KIntNumInput(this);
    _scrollBarWidth->setRange(SCROLLBAR_MINIMUM_WIDTH, SCROLLBAR_MAXIMUM_WIDTH, 1);
    _scrollBarWidth->setSliderEnabled(true);
    _scrollBarWidth->setLabel(i18n("Scrollbar width"), Qt::AlignLeft | Qt::AlignVCenter);
    _scrollBarColored = new QCheckBox(i18n("Colorful hovered scrollbars"));

    /* Stop 2: Add your widget somewhere */
    layout->addWidget( _toolBarDrawItemSeparator );
    layout->addWidget( _viewDrawTriangularExpander );
    layout->addWidget( _viewDrawTreeBranchLines );
    layout->addWidget( _scrollBarColored );
    layout->addWidget( _scrollBarWidth );

    layout->addStretch(1);

    /* Stop 3: Set up the configuration struct and your widget */
    _toolBarDrawItemSeparator->setChecked( OxygenStyleConfigData::toolBarDrawItemSeparator() );
    _viewDrawTriangularExpander->setChecked( OxygenStyleConfigData::viewDrawTriangularExpander() );
    _viewDrawTreeBranchLines->setChecked(OxygenStyleConfigData::viewDrawTreeBranchLines() );
    _scrollBarWidth->setValue( qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH,
                                OxygenStyleConfigData::scrollBarWidth())) );
    _scrollBarColored->setChecked( OxygenStyleConfigData::scrollBarColored() );


    /* Stop 4: Emit a signal on changes */
    connect( _toolBarDrawItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _viewDrawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _viewDrawTreeBranchLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _scrollBarColored, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect( _scrollBarWidth, SIGNAL( valueChanged(int) ), SLOT( updateChanged() ) );
}

OxygenStyleConfig::~OxygenStyleConfig()
{
}


void OxygenStyleConfig::save()
{
    /* Stop 5: Save the configuration */
    OxygenStyleConfigData::setToolBarDrawItemSeparator( _toolBarDrawItemSeparator->isChecked() );
    OxygenStyleConfigData::setViewDrawTriangularExpander( _viewDrawTriangularExpander->isChecked() );
    OxygenStyleConfigData::setViewDrawTreeBranchLines( _viewDrawTreeBranchLines->isChecked() );
    OxygenStyleConfigData::setScrollBarColored( _scrollBarColored->isChecked() );
    OxygenStyleConfigData::setScrollBarWidth( _scrollBarWidth->value() );

    OxygenStyleConfigData::self()->writeConfig();
}

void OxygenStyleConfig::defaults()
{
    /* Stop 6: Set defaults */
    _toolBarDrawItemSeparator->setChecked(true);
    _viewDrawTriangularExpander->setChecked(false);
    _viewDrawTreeBranchLines->setChecked(true);
    _scrollBarColored->setChecked(false);
    _scrollBarWidth->setValue(SCROLLBAR_DEFAULT_WIDTH);
    //updateChanged would be done by setChecked already
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
        )
        emit changed(false);
    else
        emit changed(true);
}

#include "oxygenconf.moc"
