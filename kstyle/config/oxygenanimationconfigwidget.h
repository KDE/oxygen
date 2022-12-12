#ifndef oxygenanimationconfigwidget_h
#define oxygenanimationconfigwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.h
// animation configuration item
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseanimationconfigwidget.h"

namespace Oxygen
{
class GenericAnimationConfigItem;
class FollowMouseAnimationConfigItem;

class AnimationConfigWidget : public BaseAnimationConfigWidget
{
    Q_OBJECT

public:
    //* constructor
    explicit AnimationConfigWidget(QWidget * = nullptr);

public Q_SLOTS:

    //* read current configuration
    void load(void) override;

    //* save current configuration
    void save(void) override;

protected Q_SLOTS:

    //* check whether configuration is changed and emit appropriate signal if yes
    void updateChanged() override;

private:
    GenericAnimationConfigItem *_genericAnimations = nullptr;
    GenericAnimationConfigItem *_progressBarAnimations = nullptr;
    GenericAnimationConfigItem *_progressBarBusyAnimations = nullptr;
    GenericAnimationConfigItem *_stackedWidgetAnimations = nullptr;
    GenericAnimationConfigItem *_labelAnimations = nullptr;
    GenericAnimationConfigItem *_lineEditAnimations = nullptr;
    GenericAnimationConfigItem *_comboBoxAnimations = nullptr;
    FollowMouseAnimationConfigItem *_toolBarAnimations = nullptr;
    FollowMouseAnimationConfigItem *_menuBarAnimations = nullptr;
    FollowMouseAnimationConfigItem *_menuAnimations = nullptr;
};
}

#endif
