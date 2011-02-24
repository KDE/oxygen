#ifndef oxygenlabel_datah
#define oxygenlabel_datah

//////////////////////////////////////////////////////////////////////////////
// oxygenlabeldata.h
// data container for QLabel transition
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include <QtCore/QString>
#include <QtCore/QBasicTimer>
#include <QtGui/QLabel>

namespace Oxygen
{

    //! generic data
    class LabelData: public TransitionData
    {

        Q_OBJECT

        public:

        //! constructor
        LabelData( QObject*, QLabel*, int );

        //! destructor
        virtual ~LabelData( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! returns true if animations are locked
        bool isLocked( void ) const
        { return _animationLockTimer.isActive(); }

        //! start lock animation timer
        void lockAnimations( void )
        { _animationLockTimer.start( _lockTime, this ); }

        //! start lock animation timer
        void unlockAnimations( void )
        { _animationLockTimer.stop(); }

        protected slots:

        //! initialize animation
        virtual bool initializeAnimation( void );

        //! animate
        virtual bool animate( void );

        //! called when target is destroyed
        virtual void targetDestroyed( void );

        protected:

        //! true if transparent
        bool transparent( void ) const
        { return transition() && transition().data()->testFlag( TransitionWidget::Transparent ); }

        //! timer event
        virtual void timerEvent( QTimerEvent* );

        private:

        //! lock time (milliseconds
        static const int _lockTime;

        //! timer used to disable animations when triggered too early
        QBasicTimer _animationLockTimer;

        //! needed to start animations out of parent paintEvent
        QBasicTimer _timer;

        //! target
        QWeakPointer<QLabel> _target;

        //! old text
        QString _text;

        //! widget rect
        /*! needed to properly handle QLabel geometry changes */
        QRect _widgetRect;

    };

}

#endif
