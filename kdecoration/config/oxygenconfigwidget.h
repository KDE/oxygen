#ifndef oxygenconfigwidget_h
#define oxygenconfigwidget_h
//////////////////////////////////////////////////////////////////////////////
// oxygenconfigurationui.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "ui_oxygenconfigurationui.h"
#include "oxygenexceptionlistwidget.h"
#include "oxygensettings.h"
#include "oxygenshadowconfigwidget.h"
#include "oxygen.h"

#include <KCModule>
#include <KSharedConfig>

#include <QWidget>
#include <QSharedPointer>

namespace Oxygen
{

    //_____________________________________________
    class ConfigWidget: public KCModule
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ConfigWidget( QWidget*, const QVariantList& );

        //* default
        void defaults() override;

        //* load configuration
        void load( void ) override;

        //* save configuration
        void save( void ) override;

        //* true if changed
        bool isChanged( void ) const
        { return m_changed; }

        //* exceptions
        ExceptionListWidget* exceptionListWidget( void ) const
        { return m_ui.exceptions; }

        private Q_SLOTS:

        //* update changed state
        void updateChanged();

        private:

        //* set changed state
        void setChanged( bool value )
        {
            m_changed = value;
            emit changed( value );
        }

        //* ui
        Ui_OxygenConfigurationUI m_ui;

        //* kconfiguration object
        KSharedConfig::Ptr m_configuration;

        //* internal exception
        InternalSettingsPtr m_internalSettings;

        //* changed state
        bool m_changed = false;

    };

}

#endif
