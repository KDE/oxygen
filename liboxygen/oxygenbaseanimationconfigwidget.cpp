//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.cpp
// animation configuration widget
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseanimationconfigwidget.h"
#include "oxygenanimationconfigitem.h"

#include "ui_oxygenanimationconfigwidget.h"

#include <QButtonGroup>
#include <QHoverEvent>
#include <QTextStream>

namespace Oxygen
{

    //_______________________________________________
    BaseAnimationConfigWidget::BaseAnimationConfigWidget( QWidget* parent ):
        QWidget( parent ),
        ui( new Ui_AnimationConfigWidget() ),
        _row(0),
        _changed( false )
    {

        ui->setupUi( this );
        QGridLayout* layout( qobject_cast<QGridLayout*>( BaseAnimationConfigWidget::layout() ) );
        _row = layout->rowCount();

    }

    //_______________________________________________
    BaseAnimationConfigWidget::~BaseAnimationConfigWidget( void )
    { delete ui; }

    //_______________________________________________
    void BaseAnimationConfigWidget::updateItems( bool state )
    {
        if( !state ) return;
        const auto children = findChildren<AnimationConfigItem*>();
        for ( AnimationConfigItem* item : children )
        { if( item->configurationWidget()->isVisible() ) item->configurationButton()->setChecked( false ); }
    }

    //_______________________________________________
    QCheckBox* BaseAnimationConfigWidget::animationsEnabled( void ) const
    { return ui->animationsEnabled; }

    //_______________________________________________
    void BaseAnimationConfigWidget::setupItem( QGridLayout* layout, AnimationConfigItem* item )
    {
        layout->addWidget( item, _row, 0, 1, 2 );
        ++_row;

        connect( item->configurationButton(), SIGNAL(toggled(bool)), SLOT(updateItems(bool)) );

        item->initializeConfigurationWidget( this );
        layout->addWidget( item->configurationWidget(), _row, 1, 1, 1 );
        ++_row;

        item->configurationWidget()->setVisible( false );
        connect( item->configurationButton(), SIGNAL(toggled(bool)), SIGNAL(layoutChanged()) );
        connect( item, SIGNAL(changed()), SLOT(updateChanged()) );
    }

}
