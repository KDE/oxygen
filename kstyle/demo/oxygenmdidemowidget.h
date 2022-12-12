#ifndef oxygenmdidemowidget_h
#define oxygenmdidemowidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmdidemowidget.h
// oxygen mdi windows demo widget
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QToolButton>
#include <QWidget>

#include "oxygendemowidget.h"
#include "ui_oxygenmdidemowidget.h"

namespace Oxygen
{
class MdiDemoWidget : public DemoWidget
{
    Q_OBJECT

public:
    //* constructor
    explicit MdiDemoWidget(QWidget * = nullptr);

public Q_SLOTS:

    void setLayoutTiled(void);
    void setLayoutCascade(void);
    void setLayoutTabbed(void);

    void benchmark(void);

private:
    Ui_MdiDemoWidget ui;
};
}

#endif
