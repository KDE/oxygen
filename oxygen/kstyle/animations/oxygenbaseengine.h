#ifndef oxygenbaseengine_h
#define oxygenbaseengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbaseengine.h
// base engine
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

#include "oxygentimeline.h"
#include <QtCore/QObject>

namespace Oxygen
{

    class BaseEngine: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        BaseEngine( QObject* parent ):
        QObject( parent ),
        enabled_( true ),
        maxFrame_( 500 ),
        duration_( 200 )
        {}

        //! destructor
        virtual ~BaseEngine( void )
        {}

        //! enability
        virtual void setEnabled( bool value )
        { enabled_ = value; }

        //! enability
        virtual bool enabled( void ) const
        { return enabled_; }

        //! max frame
        virtual void setMaxFrame( int value )
        { maxFrame_ = value; }

        //! max frame
        virtual int maxFrame( void ) const
        { return maxFrame_; }

        //! duration
        virtual void setDuration( int value )
        { duration_ = value; }

        //! duration
        virtual int duration( void ) const
        { return duration_; }

        protected:

        //! return timeLine if running, null pointer otherwise
        virtual TimeLine::Pointer _timeLine( const TimeLine::Pointer& timeLine ) const
        { return timeLine && timeLine->isRunning() ? timeLine : TimeLine::Pointer(); }

        private:

        bool enabled_;
        int maxFrame_;
        int duration_;

    };

}

#endif
