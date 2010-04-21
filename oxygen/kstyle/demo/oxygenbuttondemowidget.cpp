//////////////////////////////////////////////////////////////////////////////
// oxygenbuttondemowidget.cpp
// oxygen tabwidget demo dialog
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

#include "oxygenbuttondemowidget.h"
#include "oxygenbuttondemowidget.moc"

#include <QtGui/QMenu>
#include <KIcon>

namespace Oxygen
{

    //_____________________________________________________________
    ButtonDemoWidget::ButtonDemoWidget( QWidget* parent ):
        QWidget( parent )
    {

        ui.setupUi( this );

        ui.pushButton_4->setIcon( KIcon("oxygen") );
        ui.pushButton_5->setIcon( KIcon("oxygen") );
        ui.pushButton_6->setIcon( KIcon("oxygen") );

        installMenu( ui.pushButton_7 );
        installMenu( ui.pushButton_8 );

        ui.toolButton->setIcon( KIcon("oxygen") );
        ui.toolButton_2->setIcon( KIcon("oxygen") );
        ui.toolButton_3->setIcon( KIcon("oxygen") );
        ui.toolButton_4->setIcon( KIcon("document-new") );
        ui.toolButton_5->setIcon( KIcon("document-open") );
        ui.toolButton_6->setIcon( KIcon("document-save") );

        ui.toolButton_7->setIcon( KIcon("oxygen") );
        ui.toolButton_8->setIcon( KIcon("oxygen") );
        ui.toolButton_9->setIcon( KIcon("oxygen") );
        ui.toolButton_10->setIcon( KIcon("oxygen") );

        installMenu( ui.toolButton_7 );
        installMenu( ui.toolButton_8 );
        installMenu( ui.toolButton_9 );
        installMenu( ui.toolButton_10 );

        ui.checkBox_3->setCheckState( Qt::PartiallyChecked );

        buttons_
            << ui.toolButton
            << ui.toolButton_2
            << ui.toolButton_3
            << ui.toolButton_4
            << ui.toolButton_5
            << ui.toolButton_6
            << ui.toolButton_7
            << ui.toolButton_8
            << ui.toolButton_9
            << ui.toolButton_10
            ;

        connect( ui.textPosition, SIGNAL( currentIndexChanged( int ) ), SLOT( textPosition( int ) ) );
        connect( ui.iconSize, SIGNAL( currentIndexChanged( int ) ), SLOT( iconSize( int ) ) );
        ui.iconSize->setCurrentIndex( 2 );

    }

    //_____________________________________________________________
    void ButtonDemoWidget::textPosition( int index)
    {
        foreach( QToolButton* button, buttons_ )
        {
            switch( index )
            {
                default:
                case 0: button->setToolButtonStyle( Qt::ToolButtonIconOnly ); break;
                case 1: button->setToolButtonStyle( Qt::ToolButtonTextOnly ); break;
                case 2: button->setToolButtonStyle( Qt::ToolButtonTextBesideIcon ); break;
                case 3: button->setToolButtonStyle( Qt::ToolButtonTextUnderIcon ); break;
            }
        }
    }

    //_____________________________________________________________
    void ButtonDemoWidget::iconSize( int index)
    {
        static QList<int> sizes( QList<int>() << 16 << 22 << 32 << 48 );
        foreach( QToolButton* button, buttons_ )
        { button->setIconSize( QSize( sizes[index], sizes[index] ) ); }

    }


    //_____________________________________________________________
    void ButtonDemoWidget::installMenu( QToolButton* button )
    {
        QMenu* menu = new QMenu();
        menu->addAction( KIcon( "document-new" ), i18n( "New" ) );
        menu->addAction( KIcon( "document-open" ), i18n( "Open" ) );
        menu->addAction( KIcon( "document-save" ), i18n( "Save" ) );
        button->setMenu( menu );
    }

    //_____________________________________________________________
    void ButtonDemoWidget::installMenu( QPushButton* button )
    {
        QMenu* menu = new QMenu();
        menu->addAction( KIcon( "document-new" ), i18n( "New" ) );
        menu->addAction( KIcon( "document-open" ), i18n( "Open" ) );
        menu->addAction( KIcon( "document-save" ), i18n( "Save" ) );
        button->setMenu( menu );
    }


}
