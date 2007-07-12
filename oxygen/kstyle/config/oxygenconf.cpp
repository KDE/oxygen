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

#include "plastikconf.h"

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
		return new PlastikStyleConfig(parent);
	}
}

PlastikStyleConfig::PlastikStyleConfig(QWidget* parent): QWidget(parent)
{
	//Should have no margins here, the dialog provides them
	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

	KGlobal::locale()->insertCatalog("kstyle_config");

// 	scrollBarLines = new QCheckBox(i18n("Scrollbar handle lines"), this);
	animateProgressBar = new QCheckBox(i18n("Animate progress bars"), this);
	drawToolBarSeparator = new QCheckBox(i18n("Draw toolbar separator"), this);
	drawToolBarItemSeparator = new QCheckBox(i18n("Draw toolbar item separators"), this);
// 	drawFocusRect = new QCheckBox(i18n("Draw focus rectangles"), this);
	drawTriangularExpander = new QCheckBox(i18n("Triangular tree expander"), this);
	inputFocusHighlight = new QCheckBox(i18n("Highlight focused text input fields"), this);

    customFocusHighlightColor = new QCheckBox(i18n("Custom text input highlight color:"), this);
    KHBox *hbox1 = new KHBox(this);
    hbox1->layout()->addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Minimum) );
    focusHighlightColor = new KColorButton(hbox1);

    customOverHighlightColor = new QCheckBox(i18n("Custom mouseover highlight color:"), this);
    KHBox *hbox2 = new KHBox(this);
    hbox2->layout()->addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Minimum) );
    overHighlightColor = new KColorButton(hbox2);

    customCheckMarkColor = new QCheckBox(i18n("Custom checkmark color:"), this);
    KHBox *hbox3 = new KHBox(this);
    hbox3->layout()->addItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Minimum) );
    checkMarkColor = new KColorButton(hbox3);

// 	layout->addWidget(scrollBarLines);
	layout->addWidget(animateProgressBar);
	layout->addWidget(drawToolBarSeparator);
	layout->addWidget(drawToolBarItemSeparator);
// 	layout->addWidget(drawFocusRect);
	layout->addWidget(drawTriangularExpander);
	layout->addWidget(inputFocusHighlight);
    layout->addWidget(customFocusHighlightColor);
	layout->addWidget(hbox1);
    layout->addWidget(customOverHighlightColor);
    layout->addWidget(hbox2);
	layout->addWidget(customCheckMarkColor);
	layout->addWidget(hbox3);
	layout->addStretch(1);

	QSettings s;
// 	origScrollBarLines = s.value("/plastikstyle/Settings/scrollBarLines", false).toBool();
// 	scrollBarLines->setChecked(origScrollBarLines);
	origAnimProgressBar = s.value("/plastikstyle/Settings/animateProgressBar", true).toBool();
	animateProgressBar->setChecked(origAnimProgressBar);
	origDrawToolBarSeparator = s.value("/plastikstyle/Settings/drawToolBarSeparator", true).toBool();
	drawToolBarSeparator->setChecked(origDrawToolBarSeparator);
	origDrawToolBarItemSeparator = s.value("/plastikstyle/Settings/drawToolBarItemSeparator", true).toBool();
	drawToolBarItemSeparator->setChecked(origDrawToolBarItemSeparator);
// 	origDrawFocusRect = s.value("/plastikstyle/Settings/drawFocusRect", true).toBool();
// 	drawFocusRect->setChecked(origDrawFocusRect);
	origDrawTriangularExpander = s.value("/plastikstyle/Settings/drawTriangularExpander", false).toBool();
	drawTriangularExpander->setChecked(origDrawTriangularExpander);
	origInputFocusHighlight = s.value("/plastikstyle/Settings/inputFocusHighlight", true).toBool();
	inputFocusHighlight->setChecked(origInputFocusHighlight);
	origCustomOverHighlightColor = s.value("/plastikstyle/Settings/customOverHighlightColor", false).toBool();
	customOverHighlightColor->setChecked(origCustomOverHighlightColor);
	origOverHighlightColor = s.value("/plastikstyle/Settings/overHighlightColor", "black").toString();
	overHighlightColor->setColor(origOverHighlightColor);
	origCustomFocusHighlightColor = s.value("/plastikstyle/Settings/customFocusHighlightColor", false).toBool();
	customFocusHighlightColor->setChecked(origCustomFocusHighlightColor);
	origFocusHighlightColor = s.value("/plastikstyle/Settings/focusHighlightColor", "black").toString();
	focusHighlightColor->setColor(origFocusHighlightColor);
	origCustomCheckMarkColor = s.value("/plastikstyle/Settings/customCheckMarkColor", false).toBool();
	customCheckMarkColor->setChecked(origCustomCheckMarkColor);
	origCheckMarkColor = s.value("/plastikstyle/Settings/checkMarkColor", "black").toString();
	checkMarkColor->setColor(origCheckMarkColor);

// 	connect(scrollBarLines, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(animateProgressBar, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(drawToolBarSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(drawToolBarItemSeparator, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
// 	connect(drawFocusRect, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(drawTriangularExpander, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(inputFocusHighlight, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(customOverHighlightColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(overHighlightColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );
	connect(customFocusHighlightColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(focusHighlightColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );
	connect(customCheckMarkColor, SIGNAL( toggled(bool) ), SLOT( updateChanged() ) );
	connect(checkMarkColor, SIGNAL( changed(const QColor&) ), SLOT( updateChanged() ) );
	if ( customOverHighlightColor->isChecked() )
	 overHighlightColor->setEnabled(true);
	else
	 overHighlightColor->setEnabled(false);
	if ( customFocusHighlightColor->isChecked() )
	 focusHighlightColor->setEnabled(true);
	else
	 focusHighlightColor->setEnabled(false);
	if ( customCheckMarkColor->isChecked() )
	 checkMarkColor->setEnabled(true);
	else
	 checkMarkColor->setEnabled(false);
}

PlastikStyleConfig::~PlastikStyleConfig()
{
}


void PlastikStyleConfig::save()
{
	QSettings s;
// 	s.writeEntry("/plastikstyle/Settings/scrollBarLines", scrollBarLines->isChecked());
	s.setValue("/plastikstyle/Settings/animateProgressBar", animateProgressBar->isChecked());
	s.setValue("/plastikstyle/Settings/drawToolBarSeparator", drawToolBarSeparator->isChecked());
	s.setValue("/plastikstyle/Settings/drawToolBarItemSeparator", drawToolBarItemSeparator->isChecked());
// 	s.writeEntry("/plastikstyle/Settings/drawFocusRect", drawFocusRect->isChecked());
	s.setValue("/plastikstyle/Settings/drawTriangularExpander", drawTriangularExpander->isChecked());
	s.setValue("/plastikstyle/Settings/inputFocusHighlight", inputFocusHighlight->isChecked());
	s.setValue("/plastikstyle/Settings/customOverHighlightColor", customOverHighlightColor->isChecked());
	s.setValue("/plastikstyle/Settings/overHighlightColor", QColor(overHighlightColor->color()).name());
	s.setValue("/plastikstyle/Settings/customFocusHighlightColor", customFocusHighlightColor->isChecked());
	s.setValue("/plastikstyle/Settings/focusHighlightColor", QColor(focusHighlightColor->color()).name());
	s.setValue("/plastikstyle/Settings/customCheckMarkColor", customCheckMarkColor->isChecked());
	s.setValue("/plastikstyle/Settings/checkMarkColor", QColor(checkMarkColor->color()).name());
}

void PlastikStyleConfig::defaults()
{
// 	scrollBarLines->setChecked(false);
	animateProgressBar->setChecked(true);
	drawToolBarSeparator->setChecked(true);
	drawToolBarItemSeparator->setChecked(true);
// 	drawFocusRect->setChecked(true);
	drawTriangularExpander->setChecked(false);
	inputFocusHighlight->setChecked(true);
	customOverHighlightColor->setChecked(false);
	overHighlightColor->setColor("black");
	customFocusHighlightColor->setChecked(false);
	focusHighlightColor->setColor("black");
	customCheckMarkColor->setChecked(false);
	checkMarkColor->setColor("black");
	//updateChanged would be done by setChecked already
}

void PlastikStyleConfig::updateChanged()
{
	if ( customOverHighlightColor->isChecked() )
	 overHighlightColor->setEnabled(true);
	else
	 overHighlightColor->setEnabled(false);
	if ( customFocusHighlightColor->isChecked() )
	 focusHighlightColor->setEnabled(true);
	else
	 focusHighlightColor->setEnabled(false);
	if ( customCheckMarkColor->isChecked() )
	 checkMarkColor->setEnabled(true);
	else
	 checkMarkColor->setEnabled(false);

	if (/*(scrollBarLines->isChecked() == origScrollBarLines) &&*/
	     (animateProgressBar->isChecked() == origAnimProgressBar) &&
	      (drawToolBarSeparator->isChecked() == origDrawToolBarSeparator) &&
	       (drawToolBarItemSeparator->isChecked() == origDrawToolBarItemSeparator) &&
// 	        (drawFocusRect->isChecked() == origDrawFocusRect) &&
		  (drawTriangularExpander->isChecked() == origDrawTriangularExpander) &&
		   (inputFocusHighlight->isChecked() == origInputFocusHighlight) &&
		    (customOverHighlightColor->isChecked() == origCustomOverHighlightColor) &&
		     (overHighlightColor->color() == origOverHighlightColor) &&
		      (customFocusHighlightColor->isChecked() == origCustomFocusHighlightColor) &&
		       (focusHighlightColor->color() == origFocusHighlightColor) &&
			(customCheckMarkColor->isChecked() == origCustomCheckMarkColor) &&
			 (checkMarkColor->color() == origCheckMarkColor)
		    )
		emit changed(false);
	else
		emit changed(true);
}

#include "plastikconf.moc"
