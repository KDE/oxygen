#ifndef oxygen_animationdata_h
#define oxygen_animationdata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationdata.h
// base class data container needed for widget animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
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
