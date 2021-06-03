//////////////////////////////////////////////////////////////////////////////
// oxygengenericanimationconfigitem.cpp
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygengenericanimationconfigitem.h"

#include "ui_oxygengenericanimationconfigbox.h"

#include <KLocalizedString>

namespace Oxygen
{

    //_______________________________________________
    GenericAnimationConfigBox::GenericAnimationConfigBox(QWidget* parent):
        QFrame( parent ),
        ui( new Ui_GenericAnimationConfigBox )
    { ui->setupUi( this ); }

    //_______________________________________________
    GenericAnimationConfigBox::~GenericAnimationConfigBox()
    { delete ui; }

    //_______________________________________________
    QSpinBox* GenericAnimationConfigBox::durationSpinBox( void ) const
    { return ui->durationSpinBox; }

    //_______________________________________________
    void GenericAnimationConfigItem::initializeConfigurationWidget( QWidget* parent )
    {
        Q_ASSERT( !_configurationWidget );
        _configurationWidget = new GenericAnimationConfigBox( parent );
        setConfigurationWidget( _configurationWidget.data() );

        connect( _configurationWidget.data()->durationSpinBox(), SIGNAL(valueChanged(int)), SIGNAL(changed()) );

    }

}
