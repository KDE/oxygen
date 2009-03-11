/*
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
    drawToolBarItemSeparator = new QCheckBox(i18n("Draw toolbar item separators"), this);
    drawTriangularExpander = new QCheckBox(i18n("Triangular tree expander"), this);
    drawTreeBranchLines = new QCheckBox(i18n("Draw tree branch lines"), this);
    scrollBarWidth = new KIntNumInput(this);
    scrollBarWidth->setRange(SCROLLBAR_MINIMUM_WIDTH, SCROLLBAR_MAXIMUM_WIDTH, 1);
    scrollBarWidth->setSliderEnabled(true);
    scrollBarWidth->setLabel(i18n("Scrollbar width"), Qt::AlignLeft | Qt::AlignVCenter);
    colorfulScrollBar = new QCheckBox(i18n("Colorful hovered scrollbars"));

    /* Stop 2: Add your widget somewhere */
    layout->addWidget(drawToolBarItemSeparator);
    layout->addWidget(drawTriangularExpander);
    layout->addWidget(drawTreeBranchLines);
    layout->addWidget(colorfulScrollBar);
    layout->addWidget(scrollBarWidth);
    layout->addStretch(1);

    /* Stop 3: Set up the configuration struct and your widget */
    orig.ToolBar.drawItemSeparator = OxygenStyleConfigData::drawToolBarItemSeparator();
    drawToolBarItemSeparator->setChecked(orig.ToolBar.drawItemSeparator);
    orig.View.drawTriangularExpander = OxygenStyleConfigData::drawTriangularExpander();
    drawTriangularExpander->setChecked(orig.View.drawTriangularExpander);
    orig.View.drawTreeBranchLines = OxygenStyleConfigData::drawTreeBranchLines();
    drawTreeBranchLines->setChecked(orig.View.drawTreeBranchLines);
    orig.ScrollBar.width = OxygenStyleConfigData::scrollBarWidth();
    scrollBarWidth->setValue(qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH, orig.ScrollBar.width)));
    orig.ScrollBar.colored = OxygenStyleConfigData::colorfulScrollBar();
    colorfulScrollBar->setChecked(orig.ScrollBar.colored);


    /* Stop 4: Emit a signal on changes */
    connect(drawToolBarItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawTreeBranchLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(colorfulScrollBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(scrollBarWidth, SIGNAL( valueChanged(int) ), SLOT( updateChanged() ) );
}

OxygenStyleConfig::~OxygenStyleConfig()
{
}


void OxygenStyleConfig::save()
{
    /* Stop 5: Save the configuration */
    OxygenStyleConfigData::setDrawToolBarItemSeparator(drawToolBarItemSeparator->isChecked());
    OxygenStyleConfigData::setDrawTriangularExpander(drawTriangularExpander->isChecked());
    OxygenStyleConfigData::setDrawTreeBranchLines(drawTreeBranchLines->isChecked());
    OxygenStyleConfigData::setColorfulScrollBar(colorfulScrollBar->isChecked());
    OxygenStyleConfigData::setScrollBarWidth(scrollBarWidth->value());

    OxygenStyleConfigData::self()->writeConfig();
}

void OxygenStyleConfig::defaults()
{
    /* Stop 6: Set defaults */
    drawToolBarItemSeparator->setChecked(true);
    drawTriangularExpander->setChecked(false);
    drawTreeBranchLines->setChecked(true);
    colorfulScrollBar->setChecked(false);
    scrollBarWidth->setValue(SCROLLBAR_DEFAULT_WIDTH);
    //updateChanged would be done by setChecked already
}

void OxygenStyleConfig::updateChanged()
{
    /* Stop 7: Check if some value changed */
    if (
        (drawToolBarItemSeparator->isChecked() == orig.ToolBar.drawItemSeparator)
        && (drawTriangularExpander->isChecked() == orig.View.drawTriangularExpander)
        && (drawTreeBranchLines->isChecked() == orig.View.drawTreeBranchLines)
        && (colorfulScrollBar->isChecked() == orig.ScrollBar.colored)
        && (scrollBarWidth->value() == orig.ScrollBar.width)
        )
        emit changed(false);
    else
        emit changed(true);
}

#include "oxygenconf.moc"
