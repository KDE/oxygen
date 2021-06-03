#ifndef oxygenprogressbarengine_h
#define oxygenprogressbarengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenprogressbarengine.h
// handle progress bar animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygenprogressbardata.h"
#include "oxygendatamap.h"

#include <QWidget>

namespace Oxygen
{

    //* handles progress bar animations
    class ProgressBarEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ProgressBarEngine( QObject* object ):
            BaseEngine( object )
        {}

        //* register progressbar
        bool registerWidget( QWidget* );

        //* true if widget is animated
        bool isAnimated( const QObject* );

        //* animation opacity
        int value( const QObject* object )
        { return isAnimated( object ) ? data( object ).data()->value():0 ; }

        //* enable state
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

        //* returns data associated to widget
        DataMap<ProgressBarData>::Value data( const QObject* );

        //* map widgets to progressbar data
        DataMap<ProgressBarData> _data;

    };

}

#endif
