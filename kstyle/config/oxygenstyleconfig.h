#ifndef oxygenstyleconfig_h
#define oxygenstyleconfig_h
/*
SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
SPDX-FileCopyrightText: 2003 Sandro Giessl <ceebx@users.sourceforge.net>

based on the Keramik configuration dialog:
SPDX-FileCopyrightText: 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

SPDX-License-Identifier: MIT

*/

#include "ui_oxygenstyleconfig.h"

namespace Oxygen
{

    class StyleConfig: public QWidget, Ui::OxygenStyleConfig
    {

        Q_OBJECT

        public:

        //* constructor
        explicit StyleConfig(QWidget*);

        Q_SIGNALS:

        //* emmited whenever one option is changed.
        void changed(bool);

        public Q_SLOTS:

        //* load setup from config data
        void load( void );

        //* save current state
        void save( void );

        //* restore all default values
        void defaults( void );

        //Everything below this is internal.

        //* reset to saved configuration
        void reset( void );

        //* update modified state when option is checked/unchecked
        void updateChanged( void );

        protected:

        //* event processing
        bool event( QEvent* ) override;

        // menu mode from ui
        int menuMode( void ) const;

        // expander size from ui
        int triangularExpanderSize( void ) const;

        private Q_SLOTS:

        //* update layout
        /** needed in expert mode to accommodate with animations config widget size changes */
        void updateLayout( void );

    };

}
#endif
