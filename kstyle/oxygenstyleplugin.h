#ifndef oxygenstyleplugin_h
#define oxygenstyleplugin_h
/*
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QStylePlugin>

namespace Oxygen
{

    class StylePlugin : public QStylePlugin
    {

        Q_OBJECT

        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "oxygen.json" )

        public:

        //* constructor
        explicit StylePlugin(QObject *parent = 0):
            QStylePlugin(parent)
        {}

        //* destructor
        ~StylePlugin();

        //* returns list of valid keys
        QStringList keys() const;

        //* create style
        QStyle* create( const QString& ) override;

    };

}

#endif
