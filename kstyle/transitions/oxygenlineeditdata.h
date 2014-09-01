#ifndef oxygenlineedit_datah
#define oxygenlineedit_datah

//////////////////////////////////////////////////////////////////////////////
// oxygenlineeditdata.h
// data container for QLineEdit transition
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
#include <QLineEdit>

namespace Oxygen
{

    //! generic data
    class LineEditData: public TransitionData
    {

        Q_OBJECT

        public:

        //! constructor
        LineEditData( QObject*, QLineEdit*, int );

        //! destructor
        virtual ~LineEditData( void )
        {}

        //! event filter
        bool eventFilter( QObject*, QEvent* );

        //! returns true if animations are locked
        bool isLocked( void ) const
        { return _animationLockTimer.isActive(); }

        //! start lock animation timer
        void lockAnimations( void )
        { _animationLockTimer.start( _lockTime, this ); }

        //! start lock animation timer
        void unlockAnimations( void )
        { _animationLockTimer.stop(); }

        protected Q_SLOTS:

        //! text edited
        virtual void textEdited( void );

        //! selection changed
        virtual void selectionChanged( void );

        //! text changed
        virtual void textChanged( void );

        //! initialize animation
        virtual bool initializeAnimation( void );

        //! animate
        virtual bool animate( void );

        //! called when target is destroyed
        virtual void targetDestroyed( void );

        protected:

        //! timer event
        virtual void timerEvent( QTimerEvent* );

        //! target rect
        /*! return rect corresponding to the area to be updated when animating */
        QRect targetRect( void ) const
        {
            if( !_target ) return QRect();
            QRect out( _target.data()->rect() );
            if( _hasClearButton && _clearButtonRect.isValid() )
            { out.setRight( _clearButtonRect.left() ); }

            return out;
        }

        //! check if target has clear button
        void checkClearButton( void );

        private:

        //! lock time (milliseconds
        static const int _lockTime;

        //! timer used to disable animations when triggered too early
        QBasicTimer _animationLockTimer;

        //! needed to start animations out of parent paintEvent
        QBasicTimer _timer;

        //! target
        WeakPointer<QLineEdit> _target;

        //! true if target has clean button
        bool _hasClearButton;

        //! clear button rect
        QRect _clearButtonRect;

        //! true if text was manually edited
        /*! needed to trigger animation only on programatically enabled text */
        bool _edited;

        //! old text
        QString _text;

        //! widget rect
        /*! needed to properly handle QLabel geometry changes */
        QRect _widgetRect;

    };

}

#endif
