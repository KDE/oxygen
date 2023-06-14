// SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>
//
// SPDX-License-Identifier: MIT

#include "oxygenconfigwidget.h"
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(Oxygen::ConfigWidget, "kcm_oxygendecoration.json")

#include "kcm_oxygendecoration.moc"
