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

#include <QtCore/QString>
#include <QtCore/QWeakPointer>

#include <cassert>

#include "ui_oxygenanimationconfigitem.h"
#include "ui_oxygenfollowmouseanimationconfigbox.h"
#include "ui_oxygengenericanimationconfigbox.h"

namespace Oxygen
{

    class GenericAnimationConfigBox: public QFrame
    {

        Q_OBJECT

        public:

        //! constructor
        GenericAnimationConfigBox(QWidget* parent):
        QFrame( parent )
        { ui.setupUi( this ); }

        //! duration spin box
        QSpinBox* durationSpinBox( void ) const
        { return ui.durationSpinBox; }

        private:

        Ui_GenericAnimationConfigBox ui;

    };

    class FollowMouseAnimationConfigBox: public QFrame
    {
        Q_OBJECT

        public:

        //! constructor
        FollowMouseAnimationConfigBox(QWidget* parent):
        QFrame( parent )
        {
            ui.setupUi( this );
            ui.followMouseDurationSpinBox->setEnabled( false );
            connect( ui.typeComboBox, SIGNAL( currentIndexChanged( int ) ), SLOT( typeChanged( int ) ) );
        }

        //! type ComboBox
        KComboBox* typeComboBox( void ) const
        { return ui.typeComboBox; }

        //! duration spin box
        QSpinBox* durationSpinBox( void ) const
        { return ui.durationSpinBox; }

        //! duration spin box
        QLabel* durationLabel( void ) const
        { return ui.durationLabel; }

        //! follow mouse duration spinbox
        QSpinBox* followMouseDurationSpinBox( void ) const
        { return ui.followMouseDurationSpinBox; }

        protected slots:

        //! type changed
        void typeChanged( int value )
        {
            ui.followMouseDurationLabel->setEnabled( value == 1 );
            ui.followMouseDurationSpinBox->setEnabled( value == 1 );
        }

        private:

        Ui_FollowMouseAnimationConfigBox ui;

    };

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

    //! generic animation config item
    class GenericAnimationConfigItem: public AnimationConfigItem
    {

        Q_OBJECT

        public:

        //! constructor
        explicit GenericAnimationConfigItem( QWidget* parent, const QString& title = QString(), const QString& description = QString() ):
            AnimationConfigItem( parent, title, description )
        {}

        //! configure
        virtual void initializeConfigurationWidget( QWidget* );

        //! configuration widget
        virtual QWidget* configurationWidget( void ) const
        {
            assert( _configurationWidget );
            return _configurationWidget.data();
        }

        //! duration
        virtual int duration( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->durationSpinBox()->value():0; }

        public slots:

        //! duration
        virtual void setDuration( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->durationSpinBox()->setValue( value ); }
        }

        private:

        //! configuration widget
        QWeakPointer<GenericAnimationConfigBox> _configurationWidget;

    };

    //! generic animation config item
    class FollowMouseAnimationConfigItem: public AnimationConfigItem
    {

        Q_OBJECT

        public:

        //! constructor
        explicit FollowMouseAnimationConfigItem( QWidget* parent, const QString& title = QString(), const QString& description = QString() ):
            AnimationConfigItem( parent, title, description )
        {}

        //! initialize configuration widget
        virtual void initializeConfigurationWidget( QWidget* );

        //! configuration widget
        virtual QWidget* configurationWidget( void ) const
        {
            assert( _configurationWidget );
            return _configurationWidget.data();
        }

        //! type
        virtual int type( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->typeComboBox()->currentIndex():0; }

        //! duration
        virtual int duration( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->durationSpinBox()->value():0; }

        //! duration
        virtual int followMouseDuration( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->followMouseDurationSpinBox()->value():0; }

        //! hide duration spinbox
        virtual void hideDurationSpinBox( void )
        {
            if( _configurationWidget )
            {
                _configurationWidget.data()->durationLabel()->hide();
                _configurationWidget.data()->durationSpinBox()->hide();
            }
        }

        public slots:

        //! type
        virtual void setType( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->typeComboBox()->setCurrentIndex( value ); }
        }

        //! duration
        virtual void setDuration( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->durationSpinBox()->setValue( value ); }
        }

        //! follow mouse duration
        virtual void setFollowMouseDuration( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->followMouseDurationSpinBox()->setValue( value ); }
        }

        private:

        //! configuration widget
        QWeakPointer<FollowMouseAnimationConfigBox> _configurationWidget;

    };

}

#endif
