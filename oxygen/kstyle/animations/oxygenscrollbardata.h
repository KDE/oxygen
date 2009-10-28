#ifndef oxygenscrollbardata_h
#define oxygenscrollbardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbardata.h
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

#include "oxygensliderdata.h"

namespace Oxygen
{

    //! scrollbar data
    class ScrollBarData: public SliderData
    {

        Q_OBJECT

        public:

        //! constructor
        ScrollBarData( QObject*, QWidget*, int, int );

        //! destructor
        virtual ~ScrollBarData( void )
        {}

        virtual const TimeLine::Pointer& timeLine() const
        { return GenericData::timeLine(); }

        //! return timeLine
        virtual const TimeLine::Pointer& timeLine( QStyle::SubControl ) const;

        //! subControlRect
        virtual QRect subControlRect( QStyle::SubControl control ) const
        {
            switch( control )
            {
                case QStyle::SC_ScrollBarAddLine: return addLineRect_;
                case QStyle::SC_ScrollBarSubLine: return subLineRect_;
                default: return QRect();
            }
        }


        //! subcontrol rect
        virtual void setSubControlRect( QStyle::SubControl control, const QRect& rect )
        {
            switch( control )
            {
                case QStyle::SC_ScrollBarAddLine:
                addLineRect_ = rect;
                break;

                case QStyle::SC_ScrollBarSubLine:
                subLineRect_ = rect;
                break;

                default: break;
            }
        }

        //! duration
        virtual void setDuration( int duration )
        {
            SliderData::setDuration( duration );
            addLineTimeLine()->setDuration( duration );
            subLineTimeLine()->setDuration( duration );
        }

        //! maxFrame
        virtual void setMaxFrame( int maxFrame )
        {
            SliderData::setMaxFrame( maxFrame );
            addLineTimeLine()->setFrameRange( 0, maxFrame );
            subLineTimeLine()->setFrameRange( 0, maxFrame );
        }

        protected slots:

        //! clear addLineRect
        void clearAddLineRect( void )
        {
            if( addLineTimeLine()->direction() == QTimeLine::Backward )
            { addLineRect_ = QRect(); }
        }

        //! clear subLineRect
        void clearSubLineRect( void )
        {
            if( subLineTimeLine()->direction() == QTimeLine::Backward )
            { subLineRect_ = QRect(); }
        }

        protected:

        //! hoverMoveEvent
        virtual void hoverMoveEvent( QObject*, QEvent* );

        //! hoverMoveEvent
        virtual void hoverLeaveEvent( QObject*, QEvent* );

        //!@name hover flags
        //@{

        virtual bool addLineArrowHovered( void ) const
        { return addLineArrowHovered_; }

        virtual void setAddLineArrowHovered( bool value )
        { addLineArrowHovered_ = value; }

        virtual bool subLineArrowHovered( void ) const
        { return subLineArrowHovered_; }

        virtual void setSubLineArrowHovered( bool value )
        { subLineArrowHovered_ = value; }

        //@}

        //! update slider
        virtual void updateSlider( QStyle::SubControl );

        //! update add line arrow
        virtual void updateAddLineArrow( QStyle::SubControl );

        //! update sub line arrow
        virtual void updateSubLineArrow( QStyle::SubControl );

        //!@name timelines
        //@{

        virtual const TimeLine::Pointer& addLineTimeLine( void ) const
        { return addLineTimeLine_; }

        virtual const TimeLine::Pointer& subLineTimeLine( void ) const
        { return subLineTimeLine_; }

        private:

        //! true when one arrow is overed
        bool addLineArrowHovered_;
        bool subLineArrowHovered_;

        //! timelines
        TimeLine::Pointer addLineTimeLine_;
        TimeLine::Pointer subLineTimeLine_;

        //! subControlRect
        QRect addLineRect_;
        QRect subLineRect_;

    };

}

#endif
