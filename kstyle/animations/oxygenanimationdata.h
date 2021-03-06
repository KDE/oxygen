#ifndef oxygen_animationdata_h
#define oxygen_animationdata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationdata.h
// base class data container needed for widget animations
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

#include "oxygen.h"
#include "oxygenanimation.h"

#include <QEvent>
#include <QObject>
#include <QWidget>
#include <cmath>

namespace Oxygen
{

    //* base class
    class AnimationData: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        AnimationData( QObject* parent, QWidget* target ):
            QObject( parent ),
            _target( target )
        {}

        //*@name accessors
        //@{

        //* enable state
        virtual bool enabled( void ) const
        { return _enabled; }

        //* target
        const WeakPointer<QWidget>& target( void ) const
        { return _target; }


        //@}

        //*@name modifiers
        //@{

        //* duration
        virtual void setDuration( int ) = 0;

        //* steps
        static void setSteps( int value )
        { _steps = value; }

        //* enability
        virtual void setEnabled( bool value )
        { _enabled = value; }

        //@}

        //* invalid opacity
        static const qreal OpacityInvalid;

        protected:

        //* setup animation
        virtual void setupAnimation( const Animation::Pointer& animation, const QByteArray& property );

        //* apply step
        virtual qreal digitize( const qreal& value ) const
        {
            if( _steps > 0 ) return std::floor( value*_steps )/_steps;
            else return value;
        }

        //* trigger target update
        virtual void setDirty( void ) const
        { if( _target ) _target.data()->update(); }

        private:

        //* guarded target
        WeakPointer<QWidget> _target;

        //* enability
        bool _enabled = true;

        //* steps
        static int _steps;

    };

}

#endif
