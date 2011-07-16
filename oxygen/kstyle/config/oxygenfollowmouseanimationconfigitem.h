#ifndef oxygenfollowmouseanimationconfigitem_h
#define oxygenfollowmouseanimationconfigitem_h

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

#include "oxygenanimationconfigitem.h"

#include <KComboBox>

#include <QtCore/QWeakPointer>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

class Ui_FollowMouseAnimationConfigBox;

namespace Oxygen
{

    class FollowMouseAnimationConfigBox: public QFrame
    {
        Q_OBJECT

        public:

        //! constructor
        FollowMouseAnimationConfigBox(QWidget*);

        //! destructor
        virtual ~FollowMouseAnimationConfigBox( void );

        //! type ComboBox
        KComboBox* typeComboBox( void ) const;

        //! duration spin box
        QSpinBox* durationSpinBox( void ) const;

        //! duration spin box
        QLabel* durationLabel( void ) const;

        //! follow mouse duration spinbox
        QSpinBox* followMouseDurationSpinBox( void ) const;

        protected slots:

        //! type changed
        void typeChanged( int );

        private:

        Ui_FollowMouseAnimationConfigBox* ui;

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
