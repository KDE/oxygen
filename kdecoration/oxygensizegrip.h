#ifndef oxygensizegrip_h
#define oxygensizegrip_h
/*
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "oxygendecoration.h"
#include "config-oxygen.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QWidget>
#include <QPointer>

#if OXYGEN_HAVE_X11
#include <xcb/xcb.h>
#endif

namespace Oxygen
{

    //* implements size grip for all widgets
    class SizeGrip: public QWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit SizeGrip( Decoration* );

        protected:

        //*@name event handlers
        //@{

        //* paint
        void paintEvent( QPaintEvent* ) override;

        //* mouse press
        void mousePressEvent( QMouseEvent* ) override;

        //@}

        private Q_SLOTS:

        //* update background color
        void updateActiveState( void );

        //* update position
        void updatePosition( void );

        //* embed into parent widget
        void embed( void );

        private:

        //* send resize event
        void sendMoveResizeEvent( QPoint );

        //* grip size
        enum {
            Offset = 0,
            GripSize = 14
        };

        //* decoration
        QPointer<Decoration> m_decoration;

        //* move/resize atom
        #if OXYGEN_HAVE_X11
        xcb_atom_t m_moveResizeAtom = 0;
        #endif

    };


}

#endif
