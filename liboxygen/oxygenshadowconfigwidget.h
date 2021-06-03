#ifndef ShadowConfigWidget_h
#define ShadowConfigWidget_h
//////////////////////////////////////////////////////////////////////////////
// ShadowConfigWidget.h
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include <KConfig>
#include <QCheckBox>
#include <QGroupBox>

#include "oxygen_config_export.h"

// forward declaration
class Ui_ShadowConfiguraionUI;

namespace Oxygen
{

    //* shadow configuration widget
    class OXYGEN_CONFIG_EXPORT ShadowConfigWidget: public QGroupBox
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ShadowConfigWidget( QWidget* );

        //* destructor
        ~ShadowConfigWidget( void ) override;

        //* group
        void setGroup( QPalette::ColorGroup group )
        { _group = group; }

        //* read defaults
        void setDefaults( void )
        { load(true ); }

        //* read config
        void load( void )
        { load( false ); }

        //* write config
        void save( void ) const;

        //* true if modified
        bool isChanged( void ) const
        { return _changed; }

        Q_SIGNALS:

        //* emmitted when configuration is changed
        void changed( bool );

        private Q_SLOTS:

        //* update changed state
        void updateChanged();

        private:

        //* read config
        void load( bool );

        //* set changed state
        void setChanged( bool value )
        {
            _changed = value;
            emit changed( value );
        }

        //* ui
        Ui_ShadowConfiguraionUI* ui = nullptr;

        //* color group
        QPalette::ColorGroup _group = QPalette::Inactive;

        //* changed state
        bool _changed = false;

    };

}

#endif
