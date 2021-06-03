#ifndef oxygenmdiwindowengine_h
#define oxygenmdiwindowengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenmdiwindowengine.h
// stores event filters and maps widgets to animations
// -------------------
//
// SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenmdiwindowdata.h"

namespace Oxygen
{

    //* handle mdiwindow arrows hover effect
    class MdiWindowEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit MdiWindowEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register widget
        bool registerWidget( QWidget* );

        //* state
        bool updateState( const QObject* object, int primitive, bool value )
        {
            if( DataMap<MdiWindowData>::Value data = _data.find( object ) )
            {
                return data.data()->updateState( primitive, value );
            } else return false;
        }

        //* true if widget is animated
        bool isAnimated( const QObject* object, int primitive )
        {
            if( DataMap<MdiWindowData>::Value data = _data.find( object ) )
            {
                return data.data()->isAnimated( primitive );
            } else return false;

        }

        //* animation opacity
        qreal opacity( const QObject* object, int primitive )
        {
            if( DataMap<MdiWindowData>::Value data = _data.find( object ) )
            {
                return data.data()->opacity( primitive );
            } else return AnimationData::OpacityInvalid;
        }

        //* enability
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

        //* data map
        DataMap<MdiWindowData> _data;

    };

}

#endif
