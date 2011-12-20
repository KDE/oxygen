//////////////////////////////////////////////////////////////////////////////
// oxygengenericanimationconfigitem.cpp
// animation configuration item
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygengenericanimationconfigitem.h"
#include "oxygengenericanimationconfigitem.moc"

#include "ui_oxygengenericanimationconfigbox.h"

namespace Oxygen
{

    //_______________________________________________
    GenericAnimationConfigBox::GenericAnimationConfigBox(QWidget* parent):
        QFrame( parent ),
        ui( new Ui_GenericAnimationConfigBox() )
    { ui->setupUi( this ); }

    //_______________________________________________
    GenericAnimationConfigBox::~GenericAnimationConfigBox( void )
    { delete ui; }

    //_______________________________________________
    QSpinBox* GenericAnimationConfigBox::durationSpinBox( void ) const
    { return ui->durationSpinBox; }

    //_______________________________________________
    void GenericAnimationConfigItem::initializeConfigurationWidget( QWidget* parent )
    {
        assert( !_configurationWidget );
        _configurationWidget = new GenericAnimationConfigBox( parent );
        setConfigurationWidget( _configurationWidget.data() );

        connect( _configurationWidget.data()->durationSpinBox(), SIGNAL(valueChanged(int)), SIGNAL(changed()) );

    }

}
