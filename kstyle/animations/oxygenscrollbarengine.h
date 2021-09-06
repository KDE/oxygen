#ifndef oxygenscrollbarengine_h
#define oxygenscrollbarengine_h

//////////////////////////////////////////////////////////////////////////////
// oxygenscrollbarengine.h
// stores event filters and maps widgets to timelines for animations
// -------------------
//
// SPDX-FileCopyrightText: 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// SPDX-License-Identifier: MIT
//////////////////////////////////////////////////////////////////////////////

#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygenscrollbardata.h"

namespace Oxygen
{

    //* stores scrollbar hovered action and timeLine
    class ScrollBarEngine: public BaseEngine
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ScrollBarEngine( QObject* parent ):
            BaseEngine( parent )
        {}

        //* register scrollbar
        bool registerWidget( QWidget* );

        //* true if widget is animated
        bool isAnimated( const QObject* object, QStyle::SubControl control );

        //* animation opacity
        qreal opacity( const QObject* object, QStyle::SubControl control )
        { return isAnimated( object, control ) ? _data.find( object ).data()->opacity( control ):AnimationData::OpacityInvalid; }

        //* return true if given subcontrol is hovered
        bool isHovered( const QObject* object, QStyle::SubControl control )
        {
            if( DataMap<ScrollBarData>::Value data = _data.find( object ) ) return data.data()->isHovered( control );
            else return false;
        }

        //* control rect associated to object
        QRect subControlRect( const QObject* object, QStyle::SubControl control )
        {
            if( DataMap<ScrollBarData>::Value data = _data.find( object ) ) return data.data()->subControlRect( control );
            else return QRect();
        }

        //* control rect
        void setSubControlRect( const QObject* object, QStyle::SubControl control, const QRect& rect )
        {
            if( DataMap<ScrollBarData>::Value data = _data.find( object ) )
            { data.data()->setSubControlRect( control, rect ); }
        }

        //* control rect
        void updateState( const QObject* object, bool state )
        {
            if( DataMap<ScrollBarData>::Value data = _data.find( object ) )
            { data.data()->updateState( state ); }
        }

        //* mouse position
        QPoint position( const QObject* object )
        {
            if( DataMap<ScrollBarData>::Value data = _data.find( object ) ) return data.data()->position();
            else return QPoint( -1, -1 );
        }

        //* enable state
        void setEnabled( bool value ) override
        {
            BaseEngine::setEnabled( value );
            /*
            do not disable the map directly, because the contained OxygenScrollbarData
            are also used in non animated mode to store scrollbar arrows rect. However
            do disable all contains DATA object, in order to prevent actual animations
            */
            for ( const DataMap<ScrollBarData>::Value &data : std::as_const(_data) )
            { if( data ) data.data()->setEnabled( value ); }

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
        DataMap<ScrollBarData> _data;

    };

}

#endif
