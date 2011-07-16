//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.cpp
// animation configuration widget
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

#include "oxygenbaseanimationconfigwidget.h"
#include "oxygenbaseanimationconfigwidget.moc"
#include "oxygenanimationconfigitem.h"

#include "ui_oxygenanimationconfigwidget.h"

#include <QtGui/QButtonGroup>
#include <QtGui/QHoverEvent>
#include <QtCore/QTextStream>
#include <KLocale>

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
        foreach( AnimationConfigItem* item, findChildren<AnimationConfigItem*>() )
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

        connect( item->configurationButton(), SIGNAL( toggled( bool ) ), SLOT( updateItems( bool ) ) );

        item->initializeConfigurationWidget( this );
        layout->addWidget( item->configurationWidget(), _row, 1, 1, 1 );
        ++_row;

        item->configurationWidget()->setVisible( false );
        connect( item->configurationButton(), SIGNAL( toggled( bool ) ), SIGNAL( layoutChanged( void ) ) );
        connect( item, SIGNAL( changed( void ) ), SLOT( updateChanged( void ) ) );
    }

}
