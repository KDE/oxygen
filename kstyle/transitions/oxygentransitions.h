#ifndef oxygentransitions_h
#define oxygentransitions_h

//////////////////////////////////////////////////////////////////////////////
// oxygentransitions.h
// container for all transition engines
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygencomboboxengine.h"
#include "oxygenlabelengine.h"
#include "oxygenlineeditengine.h"
#include "oxygenstackedwidgetengine.h"

namespace Oxygen
{

//* stores engines
class Transitions : public QObject
{
    Q_OBJECT

public:
    //* constructor
    explicit Transitions(QObject *);

    //* register animations corresponding to given widget, depending on its type.
    void registerWidget(QWidget *widget) const;

    /** unregister all animations associated to a widget */
    void unregisterWidget(QWidget *widget) const;

    //* qlabel engine
    ComboBoxEngine &comboBoxEngine(void) const
    {
        return *_comboBoxEngine;
    }

    //* qlabel engine
    LabelEngine &labelEngine(void) const
    {
        return *_labelEngine;
    }

    //* qlineedit engine
    LineEditEngine &lineEditEngine(void) const
    {
        return *_lineEditEngine;
    }

    //* stacked widget engine
    StackedWidgetEngine &stackedWidgetEngine(void) const
    {
        return *_stackedWidgetEngine;
    }

public Q_SLOTS:

    //* setup engines
    void setupEngines(void);

private:
    //* register new engine
    void registerEngine(BaseEngine *engine)
    {
        _engines.push_back(engine);
    }

    //* qcombobox engine
    ComboBoxEngine *_comboBoxEngine;

    //* qlabel engine
    LabelEngine *_labelEngine;

    //* qlineedit engine
    LineEditEngine *_lineEditEngine;

    //* stacked widget engine
    StackedWidgetEngine *_stackedWidgetEngine;

    //* keep list of existing engines
    QList<BaseEngine::Pointer> _engines;
};
}

#endif
