//////////////////////////////////////////////////////////////////////////////
// oxygencomboboxdata.cpp
// data container for QComboBox transition
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

#include "oxygencomboboxdata.h"
#include "oxygencomboboxdata.moc"

namespace Oxygen
{

    //______________________________________________________
    ComboBoxData::ComboBoxData( QComboBox* parent, int duration ):
        TransitionData( parent, duration ),
        target_( parent ),
        lineEdit_( parent->lineEdit() )
    {
        connect( target_.data(), SIGNAL( currentIndexChanged( int ) ), SLOT( indexChanged() ) );
        connect( target_.data(), SIGNAL( editTextChanged( const QString& ) ), SLOT( textChanged() ) );

        if( lineEdit_ )
        { connect( lineEdit_.data(), SIGNAL( selectionChanged() ), SLOT( selectionChanged() ) ); }
    }

    //___________________________________________________________________
    void ComboBoxData::textChanged( void )
    {

        timer_.start( 50, this );

        // check if QLineEdit associated to combobox has changed
        // connect selectionChanged signal
        if( target_.data()->lineEdit() && target_.data()->lineEdit() != lineEdit_ )
        {
            lineEdit_ = target_.data()->lineEdit();
            connect( lineEdit_.data(), SIGNAL( selectionChanged() ), SLOT( selectionChanged() ) );
        }

    }

    //___________________________________________________________________
    void ComboBoxData::indexChanged( void )
    {
        if( initializeAnimation() )
        { animate(); }
    }

    //___________________________________________________________________
    void ComboBoxData::timerEvent( QTimerEvent* event )
    {

        if( event->timerId() == timer_.timerId() )
        {
            timer_.stop();
            if( target_ )
            { transition().data()->setEndPixmap( transition().data()->grab( target_.data(), targetRect() ) ); }
        } else return TransitionData::timerEvent( event );

    }

    //___________________________________________________________________
    bool ComboBoxData::initializeAnimation( void )
    {
        if( !( enabled() && target_ && target_.data()->isVisible() ) ) return false;
        transition().data()->setOpacity(0);
        transition().data()->setGeometry( targetRect() );
        transition().data()->setStartPixmap( transition().data()->endPixmap() );
        transition().data()->show();
        transition().data()->raise();
        return true;
    }

    //___________________________________________________________________
    bool ComboBoxData::animate( void )
    {

        if( !enabled() ) return false;

        // check enability
        transition().data()->setEndPixmap( transition().data()->grab( target_.data(), targetRect() ) );
        transition().data()->animate();
        return true;

    }

}
