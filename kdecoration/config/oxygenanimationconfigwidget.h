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

#include "oxygen.h"
#include "oxygenbaseanimationconfigwidget.h"

namespace Oxygen
{

// forward declaration
class GenericAnimationConfigItem;

//* container to configure animations individually
class AnimationConfigWidget : public BaseAnimationConfigWidget
{
    Q_OBJECT

public:
    //* constructor
    explicit AnimationConfigWidget(QWidget * = nullptr);

    //* configuration
    void setInternalSettings(InternalSettingsPtr internalSettings)
    {
        m_internalSettings = internalSettings;
    }

public Q_SLOTS:

    //* read current configuration
    void load(void) override;

    //* save current configuration
    void save(void) override;

protected Q_SLOTS:

    //* check whether configuration is changed and emit appropriate signal if yes
    void updateChanged() override;

private:
    //* internal exception
    InternalSettingsPtr m_internalSettings;

    //*@name animations
    //@{
    GenericAnimationConfigItem *m_buttonAnimations = nullptr;
    GenericAnimationConfigItem *m_shadowAnimations = nullptr;
    //@}
};
}

#endif
