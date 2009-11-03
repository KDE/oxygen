#ifndef oxygenanimation_h
#define oxygenanimation_h
//////////////////////////////////////////////////////////////////////////////
// oxygenanimation.h
// stores event filters and maps widgets to animations for animations
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

#include <QtCore/QPointer>
#include <QtCore/QTimeLine>
#include <QtCore/QVariant>

namespace Oxygen
{

    class Animation: public QTimeLine
    {

        Q_OBJECT

        public:

        //! TimeLine shared pointer
        typedef QPointer<Animation> Pointer;

        //! constructor
        Animation( int duration, QObject* parent ):
            QTimeLine( duration, parent )
        {
            setFrameRange( 0, 512 );
            connect( this, SIGNAL( frameChanged( int ) ), SLOT( updateProperty( int ) ) );
        }

        //! destructor
        virtual ~Animation( void )
        {}

        //! true if running
        bool isRunning( void ) const
        { return state() == Animation::Running; }

        //! restart
        void restart( void )
        {
            if( isRunning() ) stop();
            start();
        }

        //! start
        void setStartValue( qreal value )
        { start_ = value; }

        //! end
        void setEndValue( qreal value )
        { end_ = value; }

        //! target
        void setTargetObject( QObject* object )
        { target_ = object; }

        //! property
        void setPropertyName( const QByteArray& array )
        { property_ = array; }


        signals:

        void valueChanged( const QVariant& );


        private slots:

        //! update property
        void updateProperty( int );

        private:

        qreal start_;
        qreal end_;
        QPointer<QObject> target_;
        QByteArray property_;

    };

}

#endif
