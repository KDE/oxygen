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
        { return ui.durationSpinBox_; }

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
            ui.followMouseDurationSpinBox_->setEnabled( false );
            connect( ui.typeComboBox_, SIGNAL( currentIndexChanged( int ) ), SLOT( typeChanged( int ) ) );
        }

        //! type ComboBox
        KComboBox* typeComboBox( void ) const
        { return ui.typeComboBox_; }

        //! duration spin box
        QSpinBox* durationSpinBox( void ) const
        { return ui.durationSpinBox_; }

        //! duration spin box
        QLabel* durationLabel( void ) const
        { return ui.durationLabel_; }

        //! follow mouse duration spinbox
        QSpinBox* followMouseDurationSpinBox( void ) const
        { return ui.followMouseDurationSpinBox_; }

        protected slots:

        //! type changed
        void typeChanged( int value )
        {
            ui.followMouseDurationLabel_->setEnabled( value == 1 );
            ui.followMouseDurationSpinBox_->setEnabled( value == 1 );
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
        { ui.enableCheckBox_->setText( value ); }

        //! title
        virtual QString title( void ) const
        { return ui.enableCheckBox_->text(); }

        //! description
        virtual void setDescription( const QString& value )
        {
            description_ = value;
            ui.descriptionButton_->setEnabled( !description_.isEmpty() );
        }

        //! description
        virtual const QString& description( void ) const
        { return description_; }

        //! enability
        virtual void setEnabled( const bool& value )
        { ui.enableCheckBox_->setChecked( value ); }

        //! enability
        virtual bool enabled( void ) const
        { return ui.enableCheckBox_->isChecked(); }

        //! config widget
        virtual QWidget* configurationWidget( void ) const = 0;

        //! initialize config widget
        virtual void initializeConfigurationWidget( QWidget* ) = 0;

        //! configuration button
        KPushButton* configurationButton( void ) const
        { return ui.configurationButton_; }

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
        QString description_;

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
            assert( configurationWidget_ );
            return configurationWidget_.data();
        }

        //! duration
        virtual int duration( void ) const
        { return (configurationWidget_) ? configurationWidget_.data()->durationSpinBox()->value():0; }

        public slots:

        //! duration
        virtual void setDuration( int value )
        {
            if( configurationWidget_ )
            { configurationWidget_.data()->durationSpinBox()->setValue( value ); }
        }

        private:

        //! configuration widget
        QWeakPointer<GenericAnimationConfigBox> configurationWidget_;

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
            assert( configurationWidget_ );
            return configurationWidget_.data();
        }

        //! type
        virtual int type( void ) const
        { return (configurationWidget_) ? configurationWidget_.data()->typeComboBox()->currentIndex():0; }

        //! duration
        virtual int duration( void ) const
        { return (configurationWidget_) ? configurationWidget_.data()->durationSpinBox()->value():0; }

        //! duration
        virtual int followMouseDuration( void ) const
        { return (configurationWidget_) ? configurationWidget_.data()->followMouseDurationSpinBox()->value():0; }

        //! hide duration spinbox
        virtual void hideDurationSpinBox( void )
        {
            if( configurationWidget_ )
            {
                configurationWidget_.data()->durationLabel()->hide();
                configurationWidget_.data()->durationSpinBox()->hide();
            }
        }

        public slots:

        //! type
        virtual void setType( int value )
        {
            if( configurationWidget_ )
            { configurationWidget_.data()->typeComboBox()->setCurrentIndex( value ); }
        }

        //! duration
        virtual void setDuration( int value )
        {
            if( configurationWidget_ )
            { configurationWidget_.data()->durationSpinBox()->setValue( value ); }
        }

        //! follow mouse duration
        virtual void setFollowMouseDuration( int value )
        {
            if( configurationWidget_ )
            { configurationWidget_.data()->followMouseDurationSpinBox()->setValue( value ); }
        }

        private:

       //! configuration widget
        QWeakPointer<FollowMouseAnimationConfigBox> configurationWidget_;

    };

}

#endif
