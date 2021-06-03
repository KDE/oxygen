//////////////////////////////////////////////////////////////////////////////
// main.cpp
// oxygen-demo main
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygendemodialog.h"
#include "../oxygen.h"
#include "config-liboxygen.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIcon>

#include <KLocalizedString>

namespace Oxygen
{

    int run(int argc, char *argv[])
    {
        QApplication app( argc, argv );

        QCommandLineParser commandLine;
        QCommandLineOption enableHighDpi( "highdpi", "Enable High DPI pixmaps" );
        commandLine.addOption( enableHighDpi );
        commandLine.process( app );

        app.setAttribute( Qt::AA_UseHighDpiPixmaps, commandLine.isSet( enableHighDpi ) );

        app.setApplicationName( i18n( "Oxygen Demo" ) );
        app.setWindowIcon( QIcon::fromTheme( QStringLiteral( "oxygen" ) ) );
        DemoDialog dialog;
        dialog.show();
        bool result = app.exec();
        return result;
    }

}

//__________________________________________
int main(int argc, char *argv[])
{
    KLocalizedString::setApplicationDomain("oxygen_style_demo");

    return Oxygen::run( argc, argv );

}
