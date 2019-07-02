#ifndef oxygenframeshadow_h
#define oxygenframeshadow_h

//////////////////////////////////////////////////////////////////////////////
// oxygenframeshadow.h
// handle frames' shadows and rounded corners
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
//
// Largely inspired from skulpture widget style
// Copyright (c) 2007-2009 Christoph Feck <christoph@maxiom.de>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "oxygenaddeventfilter.h"
#include "oxygenstylehelper.h"

#include <QEvent>
#include <QObject>
#include <QSet>

#include <QWidget>
#include <QPaintEvent>
#include <KColorScheme>

namespace Oxygen
{

    //* shadow manager
    class FrameShadowFactory: public QObject
    {

        Q_OBJECT

        public:

        //* constructor
        explicit FrameShadowFactory( QObject* parent ):
        QObject( parent )
        {}

        //* register widget
        bool registerWidget( QWidget*, StyleHelper& );

        //* unregister
        void unregisterWidget( QWidget* );

        //* true if widget is registered
        bool isRegistered( const QWidget* widget ) const
        { return _registeredWidgets.contains( widget ); }

        //* event filter
        bool eventFilter( QObject*, QEvent*) override;

        //* set contrast
        void setHasContrast( const QWidget* widget, bool ) const;

        //* update state
        void updateState( const QWidget*, bool focus, bool hover, qreal opacity, AnimationMode ) const;

        //* update shadows geometry
        void updateShadowsGeometry( const QObject*, QRect ) const;

        protected:

        //* install shadows on given widget
        void installShadows( QWidget*, StyleHelper&, bool flat = false );

        //* update shadows geometry
        void updateShadowsGeometry( QObject* ) const;

        //* remove shadows from widget
        void removeShadows( QWidget* );

        //* raise shadows
        void raiseShadows( QObject* ) const;

        //* update shadows
        void update( QObject* ) const;

        //* install shadow on given side
        void installShadow( QWidget*, StyleHelper&, ShadowArea, bool flat = false ) const;

        protected Q_SLOTS:

        //* triggered by object destruction
        void widgetDestroyed( QObject* );

        private:

        //* needed to block ChildAdded events when creating shadows
        AddEventFilter _addEventFilter;

        //* set of registered widgets
        QSet<const QObject*> _registeredWidgets;

    };

    //* frame shadow
    /** this allows the shadow to be painted over the widgets viewport */
    class FrameShadowBase: public QWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit FrameShadowBase( ShadowArea area ):
            _area( area )
        {}

        //* shadow area
        void setShadowArea(ShadowArea area)
        { _area = area; }

        //* shadow area
        const ShadowArea& shadowArea() const
        { return _area; }

        //* set contrast
        void setHasContrast( bool value )
        {
            if( _contrast == value ) return;
            _contrast = value;
        }

        //* true if contrast pixel is enabled
        bool hasContrast( void ) const
        { return _contrast; }

        //* update geometry
        virtual void updateGeometry( void ) = 0;

        //* update geometry
        virtual void updateGeometry( QRect ) = 0;

        //* update state
        virtual void updateState( bool, bool, qreal, AnimationMode )
        {}

        protected:

        //* initialization
        virtual void init();

        //* return viewport associated to parent widget
        virtual QWidget* viewport( void ) const;

        //* parent margins
        /** offsets between update rect and parent widget rect. It is set via updateGeometry */
        const QMargins& margins( void ) const
        { return _margins; }

        //* margins
        /** offsets between update rect and parent widget rect. It is set via updateGeometry */
        void setMargins( const QMargins& margins )
        { _margins = margins; }

        private:

        //* shadow area
        ShadowArea _area;

        //* margins
        /** offsets between update rect and parent widget rect. It is set via updateGeometry */
        QMargins _margins;

        //* contrast pixel
        bool _contrast = false;

    };

    //* frame shadow
    /** this allows the shadow to be painted over the widgets viewport */
    class SunkenFrameShadow : public FrameShadowBase
    {
        Q_OBJECT

        public:

        //* constructor
        SunkenFrameShadow( ShadowArea area, StyleHelper& helper ):
            FrameShadowBase( area ),
            _helper( helper )
        { init(); }


        //* update geometry
        /** nothing is done. Rect must be passed explicitly */
        void updateGeometry( void ) override
        {}

        //* update geometry
        void updateGeometry( QRect ) override;

        //* update state
        void updateState( bool focus, bool hover, qreal opacity, AnimationMode ) override;

        protected:

        //* painting
        void paintEvent(QPaintEvent *) override;

        private:

        //* helper
        StyleHelper& _helper;

        //*@name widget state
        //@{
        bool _hasFocus = false;
        bool _mouseOver = false;
        qreal _opacity = -1;
        AnimationMode _mode = AnimationNone;

    };


    //* frame shadow
    /** this allows the shadow to be painted over the widgets viewport */
    class FlatFrameShadow : public FrameShadowBase
    {
        Q_OBJECT

        public:

        //* constructor
        FlatFrameShadow( ShadowArea area, StyleHelper& helper ):
            FrameShadowBase( area ),
            _helper( helper )
        { init(); }

        //* update geometry
        void updateGeometry( void ) override;

        //* update geometry
        void updateGeometry( QRect ) override;

        protected:

        //* painting
        void paintEvent(QPaintEvent *) override;

        private:

        //* helper
        StyleHelper& _helper;

    };
}

#endif
