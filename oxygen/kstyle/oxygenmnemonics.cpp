//////////////////////////////////////////////////////////////////////////////
// oxygenmnemonics.cpp
// enable/disable mnemonics display
// -------------------
//
// Copyright (C) 2011 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenmnemonics.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QWidget>

namespace Oxygen
{

    //____________________________________________________
    void Mnemonics::setMode( int mode )
    {

        switch( mode )
        {
            case StyleConfigData::MN_NEVER:
            qApp->removeEventFilter( this );
            setEnabled( false );
            break;

            default:
            case StyleConfigData::MN_ALWAYS:
            qApp->removeEventFilter( this );
            setEnabled( true );
            break;

            case StyleConfigData::MN_AUTO:
            qApp->removeEventFilter( this );
            qApp->installEventFilter( this );
            setEnabled( false );
            break;

        }

        return;

    }

    //____________________________________________________
    bool Mnemonics::eventFilter( QObject*, QEvent* event )
    {

        switch( event->type() )
        {

            case QEvent::KeyPress:
            if( static_cast<QKeyEvent*>(event)->key() == Qt::Key_Alt )
            { setEnabled( true ); }
            break;

            case QEvent::KeyRelease:
            if( static_cast<QKeyEvent*>(event)->key() == Qt::Key_Alt )
            { setEnabled( false ); }
            break;

            default: break;

        }

        return false;

    }

    //____________________________________________________
    void Mnemonics::setEnabled( bool value )
    {
        if( _enabled == value ) return;

        _enabled = value;

        // update all top level widgets
        foreach( QWidget* widget, qApp->topLevelWidgets() )
        { widget->update(); }

    }

}
