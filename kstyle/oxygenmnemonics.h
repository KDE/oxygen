#ifndef oxygenmnemonics_h
#define oxygenmnemonics_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmnemonics.h
// enable/disable mnemonics display
// -------------------
//
// SPDX-FileCopyrightText: 2011 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: LGPL-2.0-only
//////////////////////////////////////////////////////////////////////////////

#include <QApplication>
#include <QEvent>
#include <QObject>

#include "oxygenstyleconfigdata.h"

namespace Oxygen
{

class Mnemonics : public QObject
{
public:
    //* constructor
    explicit Mnemonics(QObject *parent)
        : QObject(parent)
    {
    }

    //* set mode
    void setMode(int);

    //* event filter
    bool eventFilter(QObject *, QEvent *) override;

    //* true if mnemonics are enabled
    const bool &enabled(void) const
    {
        return _enabled;
    }

    //* alignment flag
    int textFlags(void) const
    {
        return _enabled ? Qt::TextShowMnemonic : Qt::TextHideMnemonic;
    }

private:
    //* set enable state
    void setEnabled(bool);

    //* enable state
    bool _enabled = true;
};
}

#endif
