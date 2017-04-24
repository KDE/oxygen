/* This file is part of the KDE project
 * Copyright (C) 2016 René J.V. Bertin <rjvbertin@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "oxygenstylechooser.h"

#ifdef Q_OS_WIN
#include <QSysInfo>
#endif
#include <QString>
#include <QAction>
#include <QActionGroup>
#include <QIcon>
#include <QStyle>
#include <QStyleFactory>
#include <QApplication>
#include <QDebug>

#include <KSharedConfig>
#include <KConfigGroup>
#include <KLocalizedString>

WidgetStyleChooser::WidgetStyleChooser( QWidget *parent )
    : QPushButton( parent )
    , m_widgetStyle( QString() )
{
}

KActionMenu *WidgetStyleChooser::createStyleSelectionMenu( const QString &text, const QString &selectedStyleName )
{
    QIcon icon = QIcon::fromTheme( QStringLiteral( "preferences-desktop-theme" ) );
    KActionMenu *stylesAction= new KActionMenu( icon, text, this );
    setText( text );
    if ( !icon.isNull() )
    {
        setIcon( icon );
    }
    stylesAction->setToolTip( i18n( "Select the application widget style" ) );
    stylesAction->setStatusTip( stylesAction->toolTip() );
    QActionGroup *stylesGroup = new QActionGroup( stylesAction );

    QStringList availableStyles = QStyleFactory::keys();
    QString desktopStyle = QApplication::style()->objectName();

    m_widgetStyle = selectedStyleName;
    bool setStyle = false;
    if ( m_widgetStyle.isEmpty() )
    {
        m_widgetStyle = desktopStyle;
    }
    else if ( selectedStyleName.compare( desktopStyle, Qt::CaseInsensitive) )
    {
        setStyle = true;
    }

    foreach( const QString &style, availableStyles )
    {
        QAction *a = new QAction( style, stylesGroup );
        a->setCheckable( true );
        a->setData( style );
        if ( m_widgetStyle.compare(style, Qt::CaseInsensitive ) == 0 )
        {
            a->setChecked( true );
            if (setStyle)
            {
                // selectedStyleName was not empty and the
                // the style exists: activate it.
                activateStyle( style );
            }
        }
        stylesAction->addAction( a );
    }
    connect( stylesGroup, &QActionGroup::triggered, this,
        [&]( QAction *a ) { activateStyle(a->data().toString()); } );

    setMenu( stylesAction->menu() );

    return stylesAction;
}

QString WidgetStyleChooser::currentStyle() const
{
    return m_widgetStyle;
}

void WidgetStyleChooser::activateStyle( const QString &styleName )
{
    m_widgetStyle = styleName;
    QApplication::setStyle( QStyleFactory::create( m_widgetStyle ) );
}
