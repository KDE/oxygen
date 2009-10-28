#ifndef oxygenwidgetdata_h
#define oxygenwidgetdata_h

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

#include "oxygentimeline.h"

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtGui/QWidget>

namespace Oxygen
{

    //! base class
    class WidgetData: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        WidgetData( QObject* parent, QWidget* target ):
        QObject( parent ),
        target_( target ),
        enabled_( true )
        { target->installEventFilter( this ); }

        //! destructor
        virtual ~WidgetData( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* ) = 0;

        //! duration
        virtual void setDuration( int ) = 0;

        //! max frame
        virtual void setMaxFrame( int ) = 0;

        //! enability
        virtual bool enabled( void ) const
        { return enabled_; }

        //! enability
        virtual void setEnabled( bool value )
        { enabled_ = value; }

        protected slots:

        /*! allows to trigger widget update in specified QRect only */
        virtual void setDirty( void )
        { if( target_ ) target_->update(); }

        protected:

        //! target
        const QPointer<QWidget>& target( void ) const
        { return target_; }

        private:

        //! guarded target
        QPointer<QWidget> target_;

        //! enability
        bool enabled_;

    };

}

#endif
