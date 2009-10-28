#ifndef oxygentabbardata_h
#define oxygentabbardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetdata.h
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

#include "oxygenwidgetdata.h"

namespace Oxygen
{

    //! tabbars
    class TabBarData: public WidgetData
    {

        Q_OBJECT

        public:

        //! constructor
        TabBarData( QObject* parent, QWidget* target, int maxFrame, int duration );

        //! destructor
        virtual ~TabBarData( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        //! duration
        void setDuration( int duration )
        {
            currentIndexTimeLine_->setDuration( duration );
            previousIndexTimeLine_->setDuration( duration );
        }

        //! maxFrame
        void setMaxFrame( int maxFrame )
        {
            currentIndexTimeLine_->setFrameRange( 0, maxFrame );
            previousIndexTimeLine_->setFrameRange( 0, maxFrame );
        }

        //!@name current index handling
        //@{

        //! current index
        virtual int currentIndex( void ) const
        { return currentIndex_; }

        //! current index
        virtual void setCurrentIndex( int index )
        { currentIndex_ = index; }

        //! current index timeLine
        virtual const TimeLine::Pointer& currentIndexTimeLine( void ) const
        { return currentIndexTimeLine_; }

        //@}

        //!@name previous index handling
        //@{

        //! previous index
        virtual int previousIndex( void ) const
        { return previousIndex_; }

        //! previous index
        virtual void setPreviousIndex( int index )
        { previousIndex_ = index; }

        //! previous index timeLine
        virtual const TimeLine::Pointer& previousIndexTimeLine( void ) const
        { return previousIndexTimeLine_; }

        //@}

        //! return timeLine associated to action at given position, if any
        virtual TimeLine::Pointer timeLine( const QObject* widget, const QPoint& position ) const;

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

        //! timeline
        TimeLine::Pointer currentIndexTimeLine_;

        //! timeline
        TimeLine::Pointer previousIndexTimeLine_;

    };

}

#endif
