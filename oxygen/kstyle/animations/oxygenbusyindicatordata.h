#ifndef oxygenbusyindicatordata_h
#define oxygenbusyindicatordata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenbusyindicatordata.h
// data container for progressbar busy indicator
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

#include <QObject>

namespace Oxygen
{

    class BusyIndicatorData: public QObject
    {

        public:

        //! constructor
        BusyIndicatorData( QObject* parent ):
            QObject( parent ),
            _animated( false ),
            _value( 0 )
        {}

        //! destructor
        virtual ~BusyIndicatorData( void )
        {}

        //!@name accessors
        //@{

        //! animated
        bool isAnimated( void ) const
        { return _animated; }

        //! value
        int value( void ) const
        { return _value; }

        //@}

        //!@name modifiers
        //@{

        //! enabled
        void setEnabled( bool )
        {}

        //! enabled
        void setDuration( int )
        {}

        //! animated
        void setAnimated( bool value )
        { _animated = value; }

        //! value
        void setValue( bool value )
        { _value = value; }

        //! increment
        void increment( void )
        { _value++; }

        //@}

        private:

        //! animated
        bool _animated;

        //! value
        int _value;

    };

}

#endif
