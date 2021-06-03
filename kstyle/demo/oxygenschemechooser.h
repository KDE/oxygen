/*
    This file was part of KDevPlatform and now of Oxygen
    SPDX-FileCopyrightText: 2016 Zhigalin Alexander <alexander@zhigalin.tk>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef COLORSCHEMECHOOSER_H
#define COLORSCHEMECHOOSER_H

#include <QObject>
#include <QAction>
#include <QPushButton>
#include <QString>

namespace Oxygen
{
/**
 * Provides a menu that will offer to change the color scheme
 *
 * Furthermore, it will save the selection in the user configuration.
 */
class ColorSchemeChooser : public QPushButton
{
    Q_OBJECT
    public:
        explicit ColorSchemeChooser( QWidget* parent );
};

}  // namespace Oxygen

#endif // COLORSCHEMECHOOSER_H
