#ifndef oxygenlabelengine_h
#define oxygenlabelengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenlabelengine.h
// stores event filters and maps widgets to animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenlabeldata.h"

namespace Oxygen
{

    //* used for simple widgets
    class LabelEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit LabelEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register widget
        bool registerWidget( QLabel* );

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
        DataMap<LabelData> _data;

    };

}

#endif
