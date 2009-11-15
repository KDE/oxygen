#ifndef oxygenscrollbardata_h
#define oxygenscrollbardata_h

//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbardata.h
// data container for QScrollBar animations
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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
        Q_PROPERTY( qreal addLineOpacity READ addLineOpacity WRITE setAddLineOpacity )
        Q_PROPERTY( qreal subLineOpacity READ subLineOpacity WRITE setSubLineOpacity )

        public:

        //! constructor
        ScrollBarData( QObject* parent, QWidget* target, int );

        //! destructor
        virtual ~ScrollBarData( void )
        {}

        //! return default animation
        virtual const Animation::Pointer& animation() const
        { return SliderData::animation(); }

        //! return animation for a given subcontrol
        virtual const Animation::Pointer& animation( QStyle::SubControl ) const;

        //! return default opacity
        virtual qreal opacity( void ) const
        { return SliderData::opacity(); }

        //! return default opacity for a given subcontrol
        virtual qreal opacity( QStyle::SubControl ) const;

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
            addLineAnimation().data()->setDuration( duration );
            subLineAnimation().data()->setDuration( duration );
        }

        //! addLine opacity
        virtual void setAddLineOpacity( qreal value )
        { addLineOpacity_ = value; }

        //! addLine opacity
        virtual qreal addLineOpacity( void ) const
        { return addLineOpacity_; }

        //! subLine opacity
        virtual void setSubLineOpacity( qreal value )
        { subLineOpacity_ = value; }

        //! subLine opacity
        virtual qreal subLineOpacity( void ) const
        { return subLineOpacity_; }

        protected slots:

        //! clear addLineRect
        void clearAddLineRect( void )
        {
            if( addLineAnimation().data()->direction() == Animation::Backward )
            { addLineRect_ = QRect(); }
        }

        //! clear subLineRect
        void clearSubLineRect( void )
        {
            if( subLineAnimation().data()->direction() == Animation::Backward )
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

        virtual const Animation::Pointer& addLineAnimation( void ) const
        { return addLineAnimation_; }

        virtual const Animation::Pointer& subLineAnimation( void ) const
        { return subLineAnimation_; }

        private:

        //! true when one arrow is overed
        bool addLineArrowHovered_;
        bool subLineArrowHovered_;

        //! timelines
        Animation::Pointer addLineAnimation_;
        Animation::Pointer subLineAnimation_;

        //! opacities
        qreal addLineOpacity_;
        qreal subLineOpacity_;

        //! subControlRect
        QRect addLineRect_;
        QRect subLineRect_;

    };

}

#endif
