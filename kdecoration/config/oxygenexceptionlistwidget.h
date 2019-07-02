#ifndef oxygenexceptionlistwidget_h
#define oxygenexceptionlistwidget_h
//////////////////////////////////////////////////////////////////////////////
// oxygenexceptionlistwidget.h
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "ui_oxygenexceptionlistwidget.h"
#include "oxygenexceptionmodel.h"

//* QDialog used to commit selected files
namespace Oxygen
{

    class ExceptionListWidget: public QWidget
    {

        //* Qt meta object
        Q_OBJECT

        public:

        //* constructor
        explicit ExceptionListWidget( QWidget* = nullptr );

        //* set exceptions
        void setExceptions( const InternalSettingsList& );

        //* get exceptions
        InternalSettingsList exceptions( void );

        //* true if changed
        bool isChanged( void ) const
        { return m_changed; }

        Q_SIGNALS:

        //* emitted when changed
        void changed( bool );

        private Q_SLOTS:

        //* update button states
        void updateButtons( void );

        //* add
        void add( void );

        //* edit
        void edit( void );

        //* remove
        void remove( void );

        //* toggle
        void toggle( const QModelIndex& );

        //* move up
        void up( void );

        //* move down
        void down( void );

        private:

        //* resize columns
        void resizeColumns( void ) const;

        //* check exception
        bool checkException( InternalSettingsPtr );

        //* set changed state
        virtual void setChanged( bool value )
        {
            m_changed = value;
            emit changed( value );
        }

        //* model
        const ExceptionModel& model() const
        { return m_model; }

        //* model
        ExceptionModel& model()
        { return m_model; }

        //* model
        ExceptionModel m_model;

        //* ui
        Ui_OxygenExceptionListWidget m_ui;

        //* changed state
        bool m_changed = false;

    };

}

#endif
