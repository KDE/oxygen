// krazy:excludeall=qclasses

//////////////////////////////////////////////////////////////////////////////
// oxygentransitions.cpp
// container for all transition engines
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygentransitions.h"
#include "oxygenpropertynames.h"
#include "oxygenstyleconfigdata.h"

namespace Oxygen
{

//________________________________________________________--
Transitions::Transitions(QObject *parent)
    : QObject(parent)
{
    registerEngine(_comboBoxEngine = new ComboBoxEngine(this));
    registerEngine(_labelEngine = new LabelEngine(this));
    registerEngine(_lineEditEngine = new LineEditEngine(this));
    registerEngine(_stackedWidgetEngine = new StackedWidgetEngine(this));
}

//________________________________________________________--
void Transitions::setupEngines(void)
{
    // animation steps
    TransitionWidget::setSteps(StyleConfigData::animationSteps());

    // default enability, duration and maxFrame
    bool animationsEnabled(StyleConfigData::animationsEnabled());

    // enability
    comboBoxEngine().setEnabled(animationsEnabled && StyleConfigData::comboBoxTransitionsEnabled());
    labelEngine().setEnabled(animationsEnabled && StyleConfigData::labelTransitionsEnabled());
    lineEditEngine().setEnabled(animationsEnabled && StyleConfigData::lineEditTransitionsEnabled());
    stackedWidgetEngine().setEnabled(animationsEnabled && StyleConfigData::stackedWidgetTransitionsEnabled());

    // durations
    comboBoxEngine().setDuration(StyleConfigData::comboBoxTransitionsDuration());
    labelEngine().setDuration(StyleConfigData::labelTransitionsDuration());
    lineEditEngine().setDuration(StyleConfigData::lineEditTransitionsDuration());
    stackedWidgetEngine().setDuration(StyleConfigData::stackedWidgetTransitionsDuration());
}

//____________________________________________________________
void Transitions::registerWidget(QWidget *widget) const
{
    if (!widget)
        return;

    // check against noAnimations propery
    QVariant propertyValue(widget->property(PropertyNames::noAnimations));
    if (propertyValue.isValid() && propertyValue.toBool())
        return;

    if (QLabel *label = qobject_cast<QLabel *>(widget)) {
        // do not animate labels from tooltips
        if (widget->window() && widget->window()->windowFlags().testFlag(Qt::ToolTip))
            return;
        else if (widget->window() && widget->window()->inherits("KWin::GeometryTip"))
            return;
        else
            labelEngine().registerWidget(label);

    } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(widget)) {
        comboBoxEngine().registerWidget(comboBox);

    } else if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget)) {
        lineEditEngine().registerWidget(lineEdit);

    } else if (QStackedWidget *stack = qobject_cast<QStackedWidget *>(widget)) {
        stackedWidgetEngine().registerWidget(stack);
    }
}

//____________________________________________________________
void Transitions::unregisterWidget(QWidget *widget) const
{
    if (!widget)
        return;

    // the following allows some optimization of widget unregistration
    // it assumes that a widget can be registered atmost in one of the
    // engines stored in the list.
    for (const BaseEngine::Pointer &engine : std::as_const(_engines)) {
        if (engine && engine.data()->unregisterWidget(widget))
            break;
    }
}
}
