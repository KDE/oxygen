#ifndef oxygenlineeditengine_h
#define oxygenlineeditengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenlineeditengine.h
// stores event filters and maps widgets to animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenlineeditdata.h"

namespace Oxygen
{

    //* used for simple widgets
    class LineEditEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit LineEditEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register widget
        bool registerWidget( QLineEdit* );

        //* duration
        void setEnabled( bool value ) override
        {
            BaseEngine::setEnabled( value );
            _data.setEnabled( value );
        }

        //* duration
        void setDuration( int value ) override
        {
            BaseEngine::setDuration( value );
            _data.setDuration( value );
        }

        public Q_SLOTS:

        //* remove widget from map
        bool unregisterWidget( QObject* object ) override
        { return _data.unregisterWidget( object ); }

        private:

        //* maps
        DataMap<LineEditData> _data;

    };

}

#endif
