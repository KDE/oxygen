#ifndef oxygenexceptiondialog_h
#define oxygenexceptiondialog_h
//////////////////////////////////////////////////////////////////////////////
// oxygenexceptiondialog.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "ui_oxygenexceptiondialog.h"
#include "oxygen.h"

#include <QCheckBox>
#include <QMap>

namespace Oxygen
{

    class DetectDialog;

    //* oxygen exceptions list
    class ExceptionDialog: public QDialog
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ExceptionDialog( QWidget* parent );

        //* set exception
        void setException( InternalSettingsPtr );

        //* save exception
        void save( void );

        //* true if changed
        bool isChanged( void ) const
        { return m_changed; }

        Q_SIGNALS:

        //* emmited when changed
        void changed( bool );

        private Q_SLOTS:

        //* check whether configuration is changed and emit appropriate signal if yes
        void updateChanged();

        //* select window properties from grabbed pointers
        void selectWindowProperties( void );

        //* read properties of selected window
        void readWindowProperties( bool );

        private:

        //* set changed state
        void setChanged( bool value )
        {
            m_changed = value;
            emit changed( value );
        }

        //* map mask and checkbox
        using CheckBoxMap=QMap< ExceptionMask, QCheckBox*>;

        Ui::OxygenExceptionDialog m_ui;

        //* map mask and checkbox
        CheckBoxMap m_checkboxes;

        //* internal exception
        InternalSettingsPtr m_exception;

        //* detection dialog
        DetectDialog* m_detectDialog = nullptr;

        //* changed state
        bool m_changed = false;

    };

}

#endif
