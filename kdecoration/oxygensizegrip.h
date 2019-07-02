#ifndef oxygensizegrip_h
#define oxygensizegrip_h

/*************************************************************************
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>    *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *************************************************************************/

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
