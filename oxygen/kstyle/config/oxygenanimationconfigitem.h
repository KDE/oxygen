#ifndef oxygenanimationconfigitem_h
#define oxygenanimationconfigitem_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigitem.h
// animation configuration item
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "ui_oxygenanimationconfigitem.h"

#include <QtGui/QWidget>
#include <cassert>

namespace Oxygen
{

    class AnimationConfigItem: public QWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit AnimationConfigItem( QWidget* parent, const QString& title = QString(), const QString& description = QString() );

        //! title
        virtual void setTitle( const QString& value )
        { ui.enableCheckBox->setText( value ); }

        //! title
        virtual QString title( void ) const
        { return ui.enableCheckBox->text(); }

        //! description
        virtual void setDescription( const QString& value )
        {
            _description = value;
            ui.descriptionButton->setEnabled( !_description.isEmpty() );
        }

        //! description
        virtual const QString& description( void ) const
        { return _description; }

        //! enability
        virtual void setEnabled( const bool& value )
        { ui.enableCheckBox->setChecked( value ); }

        //! enability
        virtual bool enabled( void ) const
        { return ui.enableCheckBox->isChecked(); }

        //! config widget
        virtual QWidget* configurationWidget( void ) const = 0;

        //! initialize config widget
        virtual void initializeConfigurationWidget( QWidget* ) = 0;

        //! configuration button
        KPushButton* configurationButton( void ) const
        { return ui.configurationButton; }

        signals:

        //! emmited when changed
        void changed( void );

        protected slots:

        //! about info
        virtual void about( void );

        protected:

        //! set configuration widget
        virtual void setConfigurationWidget( QWidget* widget );

        private:

        //! description
        QString _description;

        //! ui
        Ui_AnimationConfigItem ui;

    };

}

#endif
