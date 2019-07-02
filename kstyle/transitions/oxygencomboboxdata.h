#ifndef oxygencombobox_datah
#define oxygencombobox_datah

//////////////////////////////////////////////////////////////////////////////
// oxygencomboboxdata.h
// data container for QComboBox transition
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

#include "oxygentransitiondata.h"

#include <QString>
#include <QBasicTimer>
#include <QTimerEvent>
#include <QComboBox>
#include <QLineEdit>

namespace Oxygen
{

    //* generic data
    class ComboBoxData: public TransitionData
    {

        Q_OBJECT

        public:

        //* constructor
        ComboBoxData( QObject*, QComboBox*, int );

        //* event filter
        bool eventFilter( QObject*, QEvent* ) override;

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        protected Q_SLOTS:

        //* initialize animation
        bool initializeAnimation( void ) override;

        //* animate
        bool animate( void ) override;

        private Q_SLOTS:

        //* triggered when item is activated in combobox
        void  indexChanged( void );

        //* called when target is destroyed
        void targetDestroyed( void );

        private:

        //* target rect
        /** return rect corresponding to the area to be updated when animating */
        QRect targetRect( void ) const
        { return _target ? _target.data()->rect().adjusted( 5, 5, -5, -5 ):QRect(); }

        //* needed to start animations out of parent paintEvent
        QBasicTimer _timer;

        //* target
        WeakPointer<QComboBox> _target;

    };

}

#endif
