#ifndef oxygentransitionwidget_h
#define oxygentransitionwidget_h
//////////////////////////////////////////////////////////////////////////////
// oxygentransitionwidget.h
// stores event filters and maps widgets to transitions for transitions
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

#include <oxygenanimation.h>
#include <QtGui/QWidget>

namespace Oxygen
{

    class TransitionWidget: public QWidget
    {

        Q_OBJECT

        //! declare opacity property
        Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

        public:

        //! shortcut to painter
        typedef QPointer<TransitionWidget> Pointer;

        //! constructor
        TransitionWidget( QWidget* parent, int duration );

        //! destructor
        virtual ~TransitionWidget( void )
        {}

        //! duration
        void setDuration( int duration )
        {
            if( animation_ )
            { animation_.data()->setDuration( duration ); }
        }

        //! duration
        int duration( void ) const
        { return ( animation_ ) ? animation_.data()->duration() : 0; }

        //!@name opacity
        //@{

        virtual qreal opacity( void ) const
        { return opacity_; }

        virtual void setOpacity( qreal value )
        { opacity_ = value; }

        //@}

        //@name pixmaps handling
        //@{

        //! start
        void resetStartPixmap( void )
        { setStartPixmap( QPixmap() ); }

        //! start
        void setStartPixmap( QPixmap pixmap )
        { startPixmap_ = pixmap; }

        //! start
        const QPixmap& startPixmap( void ) const
        { return startPixmap_; }

        //! end
        void resetEndPixmap( void )
        { setEndPixmap( QPixmap() ); }

        //! end
        void setEndPixmap( QPixmap pixmap )
        { endPixmap_ = pixmap; }

        //! start
        const QPixmap& endPixmap( void ) const
        { return endPixmap_; }

        //!

        //@}

        //! grap pixmap
        QPixmap grab( QWidget* = 0, QRect = QRect() ) const;

        //! animate transition
        virtual void animate( void )
        {
            if( animation_.data()->isRunning() ) animation_.data()->stop();
            animation_.data()->start();
        }

        signals:

        //! emmitted when animation is finished/aborder
        void finished( void );

        protected slots:

        /*! allows to trigger widget update in specified QRect only */
        virtual void setDirty( void )
        { update(); }

        protected:

        //! paint event
        virtual void paintEvent( QPaintEvent* );

        //! grab widget background
        /*!
        Background is not rendered properly using QWidget::render.
        Use home-made grabber instead. This is directly inspired from bespin.
        Copyright (C) 2007 Thomas Luebking <thomas.luebking@web.de>
        */
        virtual void grabBackground( QPixmap&, QWidget*, QRect& ) const;

        //! grab widget
        virtual void grabWidget( QPixmap&, QWidget*, QRect& ) const;

        private:

        //! internal transition animation
        Animation::Pointer animation_;

        //! animation starting pixmap
        QPixmap startPixmap_;

        //! animation starting pixmap
        QPixmap endPixmap_;

        //! current state opacity
        qreal opacity_;

    };

}

#endif
