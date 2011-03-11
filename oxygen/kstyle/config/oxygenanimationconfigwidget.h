#ifndef oxygenanimationconfigwidget_h
#define oxygenanimationconfigwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.h
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

#include <QtGui/QWidget>
#include <QtGui/QLayout>

#include "ui_oxygenanimationconfigwidget.h"

namespace Oxygen
{
    class AnimationConfigItem;
    class GenericAnimationConfigItem;
    class FollowMouseAnimationConfigItem;

    class AnimationConfigWidget: public QWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit AnimationConfigWidget( QWidget* = 0 );

        //! true if changed
        virtual bool isChanged( void ) const
        { return _changed; }

        signals:

        //! emmited when layout is changed
        void layoutChanged( void );

        //! emmited when changed
        void changed( bool );

        public slots:

        //! read current configuration
        virtual void load( void );

        //! save current configuration
        virtual void save( void );

        protected slots:

        //! update visible ites
        virtual void updateItems( bool );

        //! check whether configuration is changed and emit appropriate signal if yes
        virtual void updateChanged();

        protected:

        virtual void setupItem( QGridLayout*, AnimationConfigItem* );

        //! set changed state
        virtual void setChanged( bool value )
        {
            _changed = value;
            emit changed( value );
        }

        private:

        //! changed state
        bool _changed;

        Ui_AnimationConfigWidget ui;

        GenericAnimationConfigItem* _genericAnimations;
        GenericAnimationConfigItem* _progressBarAnimations;
        GenericAnimationConfigItem* _progressBarBusyAnimations;
        GenericAnimationConfigItem* _stackedWidgetAnimations;
        GenericAnimationConfigItem* _labelAnimations;
        GenericAnimationConfigItem* _lineEditAnimations;
        GenericAnimationConfigItem* _comboBoxAnimations;
        FollowMouseAnimationConfigItem* _toolBarAnimations;
        FollowMouseAnimationConfigItem* _menuBarAnimations;
        FollowMouseAnimationConfigItem* _menuAnimations;

        //! grid running row. Used for layout
        int row_;

    };

}

#endif
