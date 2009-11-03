#ifndef oxygentabbardata_h
#define oxygentabbardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetdata.h
// stores event filters and maps widgets to Animations for animations
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

#include "oxygenwidgetdata.h"

namespace Oxygen
{

    //! tabbars
    class TabBarData: public WidgetData
    {

        Q_OBJECT

        //! declare opacity property
        Q_PROPERTY( qreal currentOpacity READ currentOpacity WRITE setCurrentOpacity )
        Q_PROPERTY( qreal previousOpacity READ previousOpacity WRITE setPreviousOpacity )

        public:

        //! constructor
        TabBarData( QWidget* parent, int duration );

        //! destructor
        virtual ~TabBarData( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! duration
        void setDuration( int duration )
        {
            currentIndexAnimation().data()->setDuration( duration );
            previousIndexAnimation().data()->setDuration( duration );
        }

        //!@name current index handling
        //@{

        //! current opacity
        virtual qreal currentOpacity( void ) const
        { return currentOpacity_; }

        //! current opacity
        virtual void setCurrentOpacity( qreal value )
        { currentOpacity_ = value; }

        //! current index
        virtual int currentIndex( void ) const
        { return currentIndex_; }

        //! current index
        virtual void setCurrentIndex( int index )
        { currentIndex_ = index; }

        //! current index animation
        virtual const Animation::Pointer& currentIndexAnimation( void ) const
        { return currentIndexAnimation_; }

        //@}

        //!@name previous index handling
        //@{

        //! previous opacity
        virtual qreal previousOpacity( void ) const
        { return previousOpacity_; }

        //! previous opacity
        virtual void setPreviousOpacity( qreal value )
        { previousOpacity_ = value; }

        //! previous index
        virtual int previousIndex( void ) const
        { return previousIndex_; }

        //! previous index
        virtual void setPreviousIndex( int index )
        { previousIndex_ = index; }

        //! previous index Animation
        virtual const Animation::Pointer& previousIndexAnimation( void ) const
        { return previousIndexAnimation_; }

        //@}

        //! return Animation associated to action at given position, if any
        virtual Animation::Pointer animation( const QPoint& position ) const;

        //! return opacity associated to action at given position, if any
        virtual qreal opacity( const QPoint& position ) const;

        protected:

        //! menubar enterEvent
        virtual void enterEvent( const QObject* object );

        //! menubar enterEvent
        virtual void leaveEvent( const QObject* object );

        //! menubar mouseMoveEvent
        virtual void mouseMoveEvent( const QObject* object, const QPoint& );

        private:

        //! current index
        int currentIndex_;

        //! previous index
        int previousIndex_;

        //! Animation
        Animation::Pointer currentIndexAnimation_;

        //! Animation
        Animation::Pointer previousIndexAnimation_;

        //! current opacity
        qreal currentOpacity_;

        //! previous opacity
        qreal previousOpacity_;

    };

}

#endif
