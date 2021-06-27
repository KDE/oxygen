#ifndef oxygentabbarengine_h
#define oxygentabbarengine_h
/*
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "oxygen.h"
#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygentabbardata.h"

namespace Oxygen
{

    //* stores tabbar hovered action and timeLine
    class TabBarEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit TabBarEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register tabbar
        bool registerWidget( QWidget* );

        //* true if widget hover state is changed
        bool updateState( const QObject*, const QPoint&, AnimationMode, bool );

        //* true if widget is animated
        bool isAnimated( const QObject* object, const QPoint& point, AnimationMode );

        //* animation opacity
        qreal opacity( const QObject* object, const QPoint& point, AnimationMode mode )
        { return isAnimated( object, point, mode ) ? data( object, mode ).data()->opacity( point ) : AnimationData::OpacityInvalid; }

        //* enable state
        void setEnabled( bool value ) override
        {
            BaseEngine::setEnabled( value );
            _hoverData.setEnabled( value );
            _focusData.setEnabled( value );
        }

        //* duration
        void setDuration( int value ) override
        {
            BaseEngine::setDuration( value );
            _hoverData.setDuration( value );
            _focusData.setDuration( value );
        }

        public Q_SLOTS:

        //* remove widget from map
        bool unregisterWidget( QObject* object ) override
        {
            if( !object ) return false;
            bool found = false;
            if( _hoverData.unregisterWidget( object ) ) found = true;
            if( _focusData.unregisterWidget( object ) ) found = true;
            return found;
        }

        private:

        //* returns data associated to widget
        DataMap<TabBarData>::Value data( const QObject*, AnimationMode );

        //* data map
        DataMap<TabBarData> _hoverData;
        DataMap<TabBarData> _focusData;

    };

}

#endif
