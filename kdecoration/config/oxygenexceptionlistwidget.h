#ifndef oxygenexceptionlistwidget_h
#define oxygenexceptionlistwidget_h
//////////////////////////////////////////////////////////////////////////////
// oxygenexceptionlistwidget.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenexceptionmodel.h"
#include "ui_oxygenexceptionlistwidget.h"

//* QDialog used to commit selected files
namespace Oxygen
{

class ExceptionListWidget : public QWidget
{
    //* Qt meta object
    Q_OBJECT

public:
    //* constructor
    explicit ExceptionListWidget(QWidget * = nullptr);

    //* set exceptions
    void setExceptions(const InternalSettingsList &);

    //* get exceptions
    InternalSettingsList exceptions(void);

    //* true if changed
    bool isChanged(void) const
    {
        return m_changed;
    }

Q_SIGNALS:

    //* emitted when changed
    void changed(bool);

private Q_SLOTS:

    //* update button states
    void updateButtons(void);

    //* add
    void add(void);

    //* edit
    void edit(void);

    //* remove
    void remove(void);

    //* toggle
    void toggle(const QModelIndex &);

    //* move up
    void up(void);

    //* move down
    void down(void);

private:
    //* resize columns
    void resizeColumns(void) const;

    //* check exception
    bool checkException(InternalSettingsPtr);

    //* set changed state
    virtual void setChanged(bool value)
    {
        m_changed = value;
        emit changed(value);
    }

    //* model
    const ExceptionModel &model() const
    {
        return m_model;
    }

    //* model
    ExceptionModel &model()
    {
        return m_model;
    }

    //* model
    ExceptionModel m_model;

    //* ui
    Ui_OxygenExceptionListWidget m_ui;

    //* changed state
    bool m_changed = false;
};
}

#endif
