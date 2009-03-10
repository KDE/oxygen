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
    config = KComponentData("oxygen").config();
    //Should have no margins here, the dialog provides them
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    KGlobal::locale()->insertCatalog("kstyle_config");

    //animateProgressBar = new QCheckBox(i18n("Animate progress bars"), this);
    drawToolBarItemSeparator = new QCheckBox(i18n("Draw toolbar item separators"), this);
    drawTriangularExpander = new QCheckBox(i18n("Triangular tree expander"), this);
    drawTreeBranchLines = new QCheckBox(i18n("Draw tree branch lines"), this);
    scrollBarWidth = new KIntNumInput(this);

    //layout->addWidget(animateProgressBar);
    layout->addWidget(drawToolBarItemSeparator);
    layout->addWidget(drawTriangularExpander);
    layout->addWidget(drawTreeBranchLines);
    layout->addWidget(scrollBarWidth);
    layout->addStretch(1);

    KConfigGroup cfg(config, "Style");
    //origAnimProgressBar = cfg.readEntry("AnimateProgressBar", false);
    //animateProgressBar->setChecked(origAnimProgressBar);
    origDrawToolBarItemSeparator = cfg.readEntry("DrawToolBarItemSeparator", true);
    drawToolBarItemSeparator->setChecked(origDrawToolBarItemSeparator);
    origDrawTriangularExpander = cfg.readEntry("DrawTriangularExpander", false);
    drawTriangularExpander->setChecked(origDrawTriangularExpander);
    origDrawTreeBranchLines = cfg.readEntry("DrawTreeBranchLines", true);
    drawTreeBranchLines->setChecked(origDrawTreeBranchLines);
    origScrollBarWidth = cfg.readEntry("ScrollBarWidth", SCROLLBAR_DEFAULT_WIDTH);
    int value = qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH, origScrollBarWidth));
    scrollBarWidth->setValue(qMin(SCROLLBAR_MAXIMUM_WIDTH, qMax(SCROLLBAR_MINIMUM_WIDTH, origScrollBarWidth)));
    scrollBarWidth->setRange(SCROLLBAR_MINIMUM_WIDTH, SCROLLBAR_MAXIMUM_WIDTH, 1);
    scrollBarWidth->setSliderEnabled(true);
    scrollBarWidth->setLabel(i18n("Scrollbar width"), Qt::AlignLeft | Qt::AlignVCenter);

    //connect(animateProgressBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawToolBarItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawTreeBranchLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(scrollBarWidth, SIGNAL( valueChanged(int) ), SLOT( updateChanged() ) );
}

OxygenStyleConfig::~OxygenStyleConfig()
{
}


void OxygenStyleConfig::save()
{
    KConfigGroup cfg(config, "Style");
    //s.writeEntry("AnimateProgressBar", animateProgressBar->isChecked() ? true : false);
    cfg.writeEntry("DrawToolBarItemSeparator", drawToolBarItemSeparator->isChecked() ? true : false);
    cfg.writeEntry("DrawTriangularExpander", drawTriangularExpander->isChecked() ? true : false);
    cfg.writeEntry("DrawTreeBranchLines", drawTreeBranchLines->isChecked() ? true : false);
    cfg.writeEntry("ScrollBarWidth", scrollBarWidth->value());
}

void OxygenStyleConfig::defaults()
{
//    animateProgressBar->setChecked(true);
    drawToolBarItemSeparator->setChecked(true);
    drawTriangularExpander->setChecked(false);
    drawTreeBranchLines->setChecked(true);
    scrollBarWidth->setValue(SCROLLBAR_DEFAULT_WIDTH);
    //updateChanged would be done by setChecked already
}

void OxygenStyleConfig::updateChanged()
{
    if (
        //(animateProgressBar->isChecked() == origAnimProgressBar)
        (drawToolBarItemSeparator->isChecked() == origDrawToolBarItemSeparator)
        && (drawTriangularExpander->isChecked() == origDrawTriangularExpander)
        && (drawTreeBranchLines->isChecked() == origDrawTreeBranchLines)
        && (scrollBarWidth->value() == origScrollBarWidth)
        )
        emit changed(false);
    else
        emit changed(true);
}

#include "oxygenconf.moc"
