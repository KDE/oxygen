#ifndef oxygenbaseanimationconfigwidget_h
#define oxygenbaseanimationconfigwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbaseanimationconfigwidget.h
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

#include "oxygen_export.h"

#include <QtGui/QWidget>
#include <QtGui/QLayout>
#include <QtGui/QCheckBox>

class Ui_AnimationConfigWidget;

namespace Oxygen
{
    class AnimationConfigItem;

    class OXYGEN_EXPORT BaseAnimationConfigWidget: public QWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit BaseAnimationConfigWidget( QWidget* = 0 );

        //! destructor
        virtual ~BaseAnimationConfigWidget( void );

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
        virtual void load( void ) = 0;

        //! save current configuration
        virtual void save( void ) = 0;

        protected slots:

        //! update visible ites
        virtual void updateItems( bool );

        //! check whether configuration is changed and emit appropriate signal if yes
        virtual void updateChanged() = 0;

        protected:

        //! get global animations enabled checkbox
        QCheckBox* animationsEnabled( void ) const;

        //! add item to ui
        virtual void setupItem( QGridLayout*, AnimationConfigItem* );

        //! set changed state
        virtual void setChanged( bool value )
        {
            _changed = value;
            emit changed( value );
        }

        //! user interface
        Ui_AnimationConfigWidget* ui;

        //! row index
        int _row;

        private:

        //! changed state
        bool _changed;

    };

}


#endif
