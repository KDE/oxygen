#ifndef oxygendatamap_h
#define oxygendatamap_h

//////////////////////////////////////////////////////////////////////////////
// oxygendatamap.h
// stores event filters and maps widgets to timelines for animations
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

#include <QtCore/QMap>
#include <QtCore/QWeakPointer>

namespace Oxygen
{
    //! data map
    /*! it maps templatized data object to associated object */
    template< typename T > class DataMap: public QMap< const QObject*, QWeakPointer<T> >
    {

        public:

        typedef const QObject* Key;
        typedef QWeakPointer<T> Value;

        //! constructor
        DataMap( void ):
            QMap<Key, Value>(),
            enabled_( true ),
            lastKey_( NULL )
        {}

        //! destructor
        virtual ~DataMap( void )
        {}

        //! find value
        Value find( Key key )
        {
            if( !( enabled() && key ) ) return Value();
            if( key == lastKey_ ) return lastValue_;
            else {
                Value out;
                typename QMap<Key, Value>::iterator iter( QMap<Key, Value>::find( key ) );
                if( iter != QMap<Key, Value>::end() ) out = iter.value();
                lastKey_ = key;
                lastValue_ = out;
                return out;
            }
        }

        //! unregister widget
        bool unregisterWidget( Key key )
        {

            // check key
            if( !key ) return false;

            // clear last value if needed
            if( key == lastKey_ && lastValue_ )
            {

                lastValue_.data()->deleteLater();
                lastKey_ = NULL;
                lastValue_.clear();

            }

            // find key in map
            typename QMap<Key, Value>::iterator iter( QMap<Key, Value>::find( key ) );
            if( iter == QMap<Key, Value>::end() ) return false;

            // delete value from map if found
            if( iter.value() ) iter.value().data()->deleteLater();
            QMap<Key, Value>::erase( iter );

            return true;

        }

        //! maxFrame
        void setEnabled( bool enabled )
        {
            enabled_ = enabled;
            foreach( const Value& value, *this )
            { if( value ) value.data()->setEnabled( enabled ); }
        }

        //! enability
        bool enabled( void ) const
        { return enabled_; }

        //! duration
        void setDuration( int duration ) const
        {
            foreach( const Value& value, *this )
            { if( value ) value.data()->setDuration( duration ); }
        }

        private:

        //! enability
        bool enabled_;

        //! last key
        Key lastKey_;

        //! last value
        Value lastValue_;

    };

}

#endif
