//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigitem.h
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <QAbstractScrollArea>
#include <QApplication>
#include <QIcon>

#include <KCMultiDialog>
#include <KLocalizedString>

//__________________________________________
int main(int argc, char *argv[])
{
    KLocalizedString::setApplicationDomain("oxygen_style_config");

    QApplication app( argc, argv );
    app.setApplicationName( i18n( "Oxygen Settings" ) );
    app.setWindowIcon( QIcon::fromTheme( QStringLiteral( "oxygen-settings" ) ) );

    KCMultiDialog dialog;
    dialog.setWindowTitle( i18n( "Oxygen Settings" ) );
    dialog.addModule( KPluginMetaData(QStringLiteral( "oxygenstyleconfig" )));
    dialog.addModule( KPluginMetaData(QStringLiteral( "oxygendecorationconfig" )));
    dialog.show();

    foreach( auto child, dialog.findChildren<QAbstractScrollArea*>() )
    {
        child->adjustSize();
        child->viewport()->adjustSize();
    }

    return app.exec();
}
