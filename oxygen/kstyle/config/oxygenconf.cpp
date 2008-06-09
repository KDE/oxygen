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
#include <QtGui/QLayout>
#include <khbox.h>
#include <QtCore/QSettings>
#include <QtGui/QColor>
#include <kglobal.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kdemacros.h>

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

    //animateProgressBar = new QCheckBox(i18n("Animate progress bars"), this);
    drawToolBarItemSeparator = new QCheckBox(i18n("Draw toolbar item separators"), this);
    drawTriangularExpander = new QCheckBox(i18n("Triangular tree expander"), this);

    //layout->addWidget(animateProgressBar);
    layout->addWidget(drawToolBarItemSeparator);
    layout->addWidget(drawTriangularExpander);
    layout->addStretch(1);

    QSettings s("KDE","Oxygen");
    //origAnimProgressBar = s.value("/oxygenstyle/Settings/animateProgressBar", true).toBool();
    //animateProgressBar->setChecked(origAnimProgressBar);
    origDrawToolBarItemSeparator = s.value("/oxygenstyle/Settings/drawToolBarItemSeparator", true).toBool();
    drawToolBarItemSeparator->setChecked(origDrawToolBarItemSeparator);
    origDrawTriangularExpander = s.value("/oxygenstyle/Settings/drawTriangularExpander", false).toBool();
    drawTriangularExpander->setChecked(origDrawTriangularExpander);

    //connect(animateProgressBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawToolBarItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
    connect(drawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
}

OxygenStyleConfig::~OxygenStyleConfig()
{
}


void OxygenStyleConfig::save()
{
    QSettings s("KDE","Oxygen");
    //s.setValue("/oxygenstyle/Settings/animateProgressBar", animateProgressBar->isChecked());
    s.setValue("/oxygenstyle/Settings/drawToolBarItemSeparator", drawToolBarItemSeparator->isChecked());
    s.setValue("/oxygenstyle/Settings/drawTriangularExpander", drawTriangularExpander->isChecked());
}

void OxygenStyleConfig::defaults()
{
//    animateProgressBar->setChecked(true);
    drawToolBarItemSeparator->setChecked(true);
    drawTriangularExpander->setChecked(false);
    //updateChanged would be done by setChecked already
}

void OxygenStyleConfig::updateChanged()
{
    if (
        //(animateProgressBar->isChecked() == origAnimProgressBar) &&
        (drawToolBarItemSeparator->isChecked() == origDrawToolBarItemSeparator) &&
        (drawTriangularExpander->isChecked() == origDrawTriangularExpander)
        )
        emit changed(false);
    else
        emit changed(true);
}

#include "oxygenconf.moc"
