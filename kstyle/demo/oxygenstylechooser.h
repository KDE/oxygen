/* This file is part of the KDE project
    SPDX-FileCopyrightText: 2016 René J.V. Bertin <rjvbertin@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef QWIDGETSTYLESELECTOR_H


#include <QWidget>
#include <QMenu>
#include <QPushButton>

#include <KActionMenu>

class WidgetStyleChooser : public QPushButton
{
    Q_OBJECT
public:
    explicit WidgetStyleChooser( QWidget *parent = 0 );

    KActionMenu *createStyleSelectionMenu( const QString &text, const QString &selectedStyleName=QString() );

    QString currentStyle() const;

private Q_SLOTS:
    void activateStyle( const QString &styleName );

private:
    QString m_widgetStyle;
};

#define QWIDGETSTYLESELECTOR_H
#endif
