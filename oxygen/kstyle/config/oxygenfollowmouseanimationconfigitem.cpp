//////////////////////////////////////////////////////////////////////////////
// oxygenfollowmouseanimationconfigitem.cpp
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

#include "oxygenfollowmouseanimationconfigitem.h"
#include "oxygenfollowmouseanimationconfigitem.moc"
#include "ui_oxygenfollowmouseanimationconfigbox.h"

namespace Oxygen
{

    //_______________________________________________
    FollowMouseAnimationConfigBox::FollowMouseAnimationConfigBox(QWidget* parent):
        QFrame( parent ),
        ui( new Ui_FollowMouseAnimationConfigBox() )
    {
        ui->setupUi( this );
        ui->followMouseDurationSpinBox->setEnabled( false );
        connect( ui->typeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int)) );
    }

    //_______________________________________________
    FollowMouseAnimationConfigBox::~FollowMouseAnimationConfigBox( void )
    { delete ui; }

    //_______________________________________________
    KComboBox* FollowMouseAnimationConfigBox::typeComboBox( void ) const
    { return ui->typeComboBox; }

    //_______________________________________________
    QSpinBox* FollowMouseAnimationConfigBox::durationSpinBox( void ) const
    { return ui->durationSpinBox; }

    //_______________________________________________
    QLabel* FollowMouseAnimationConfigBox::durationLabel( void ) const
    { return ui->durationLabel; }

    //_______________________________________________
    QSpinBox* FollowMouseAnimationConfigBox::followMouseDurationSpinBox( void ) const
    { return ui->followMouseDurationSpinBox; }

    //_______________________________________________
    void FollowMouseAnimationConfigBox::typeChanged( int value )
    {
        ui->followMouseDurationLabel->setEnabled( value == 1 );
        ui->followMouseDurationSpinBox->setEnabled( value == 1 );
    }

    //_______________________________________________
    void FollowMouseAnimationConfigItem::initializeConfigurationWidget( QWidget* parent )
    {
        assert( !_configurationWidget );
        _configurationWidget = new FollowMouseAnimationConfigBox( parent );
        setConfigurationWidget( _configurationWidget.data() );

        connect( _configurationWidget.data()->typeComboBox(), SIGNAL(currentIndexChanged(int)), SIGNAL(changed()) );
        connect( _configurationWidget.data()->durationSpinBox(), SIGNAL(valueChanged(int)), SIGNAL(changed()) );
        connect( _configurationWidget.data()->followMouseDurationSpinBox(), SIGNAL(valueChanged(int)), SIGNAL(changed()) );

    }

}
