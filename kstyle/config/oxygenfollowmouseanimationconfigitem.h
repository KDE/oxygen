#ifndef oxygenfollowmouseanimationconfigitem_h
#define oxygenfollowmouseanimationconfigitem_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigitem.h
// animation configuration item
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "../oxygen.h"

#include <KComboBox>

#include <QFrame>
#include <QLabel>
#include <QSpinBox>

class Ui_FollowMouseAnimationConfigBox;

namespace Oxygen
{

    class FollowMouseAnimationConfigBox: public QFrame
    {
        Q_OBJECT

        public:

        //* constructor
        explicit FollowMouseAnimationConfigBox(QWidget*);

        //* destructor
        ~FollowMouseAnimationConfigBox( void ) override;

        //* type ComboBox
        KComboBox* typeComboBox( void ) const;

        //* duration spin box
        QSpinBox* durationSpinBox( void ) const;

        //* duration spin box
        QLabel* durationLabel( void ) const;

        //* follow mouse duration spinbox
        QSpinBox* followMouseDurationSpinBox( void ) const;

        private Q_SLOTS:

        //* type changed
        void typeChanged( int );

        private:

        Ui_FollowMouseAnimationConfigBox* ui;

    };

    //* generic animation config item
    class FollowMouseAnimationConfigItem: public AnimationConfigItem
    {

        Q_OBJECT

        public:

        //* constructor
        explicit FollowMouseAnimationConfigItem( QWidget* parent, const QString& title = QString(), const QString& description = QString() ):
            AnimationConfigItem( parent, title, description )
        {}

        //* initialize configuration widget
        void initializeConfigurationWidget( QWidget* ) override;

        //* configuration widget
        QWidget* configurationWidget( void ) const override
        { return _configurationWidget.data(); }

        //* type
        int type( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->typeComboBox()->currentIndex():0; }

        //* duration
        int duration( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->durationSpinBox()->value():0; }

        //* duration
        int followMouseDuration( void ) const
        { return (_configurationWidget) ? _configurationWidget.data()->followMouseDurationSpinBox()->value():0; }

        //* hide duration spinbox
        void hideDurationSpinBox( void )
        {
            if( _configurationWidget )
            {
                _configurationWidget.data()->durationLabel()->hide();
                _configurationWidget.data()->durationSpinBox()->hide();
            }
        }

        public Q_SLOTS:

        //* type
        void setType( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->typeComboBox()->setCurrentIndex( value ); }
        }

        //* duration
        void setDuration( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->durationSpinBox()->setValue( value ); }
        }

        //* follow mouse duration
        void setFollowMouseDuration( int value )
        {
            if( _configurationWidget )
            { _configurationWidget.data()->followMouseDurationSpinBox()->setValue( value ); }
        }

        private:

        //* configuration widget
        WeakPointer<FollowMouseAnimationConfigBox> _configurationWidget;

    };

}

#endif
