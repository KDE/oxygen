#ifndef oxygenframeshadow_h
#define oxygenframeshadow_h

//////////////////////////////////////////////////////////////////////////////
// oxygenframeshadow.h
// handle sunken frames' shadows
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include <QtCore/QEvent>
#include <QtCore/QObject>
#include <QtCore/QSet>

#include <QtGui/QWidget>
#include <QtGui/QPaintEvent>
#include <KColorScheme>

#include "oxygenstylehelper.h"
#include "tileset.h"

namespace Oxygen
{

    //! shadow area
    enum ShadowArea {
        Unknown,
        Left,
        Top,
        Right,
        Bottom
    };

    //! shadow manager
    class FrameShadowFactory: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        FrameShadowFactory( QObject* parent ):
        QObject( parent )
        {}

        //! destructor
        virtual ~FrameShadowFactory( void )
        {}

        //! register widget
        bool registerWidget( QWidget*, StyleHelper& );

        //! unregister
        void unregisterWidget( QWidget* );

        //! true if widget is registered
        bool isRegistered( const QWidget* widget ) const
        { return _registeredWidgets.contains( widget ); }

        //! event filter
        virtual bool eventFilter( QObject*, QEvent*);

        //! update state
        void updateState( const QWidget* widget, bool focus, bool hover, qreal opacity, AnimationMode ) const;

        protected:

        //! install shadows on given widget
        virtual void installShadows( QWidget*, StyleHelper& );

        //! remove shadows from widget
        virtual void removeShadows( QWidget* );

        //! update shadows geometry
        virtual void updateShadowsGeometry( QObject* ) const;

        //! install shadow on given side
        virtual void installShadow( QWidget*, StyleHelper&, ShadowArea ) const;

        protected slots:

        //! triggered by object destruction
        void widgetDestroyed( QObject* );

        private:

        //! set of registered widgets
        QSet<const QObject*> _registeredWidgets;

    };

    //! frame shadow
    /*! this allows the shadow to be painted over the widgets viewport */
    class FrameShadow : public QWidget
    {
        Q_OBJECT

        public:

        //! constructor
        explicit FrameShadow( ShadowArea area, StyleHelper& helper ):
            QWidget(0),
            _helper( helper ),
            _viewFocusBrush( KColorScheme::View, KColorScheme::FocusColor, helper.config() ),
            _viewHoverBrush( KColorScheme::View, KColorScheme::HoverColor, helper.config() ),
            _area( area ),
            _focus( false ),
            _hover( false ),
            _opacity( -1 ),
            _mode( AnimationNone )
        { init(); }


        //! destructor
        virtual ~FrameShadow()
        {}

        //! shadow area
        void setShadowArea(ShadowArea area)
        { _area = area; }

        //! shadow area
        ShadowArea shadowArea() const
        { return _area; }

        //! update geometry
        virtual void updateGeometry( void );

        //! update state
        void updateState( bool focus, bool hover, qreal opacity, AnimationMode mode );

        protected:

        //! event handler
        virtual bool event(QEvent *e);

        //! painting
        virtual void paintEvent(QPaintEvent *);

        //! initialization
        virtual void init();

        //! return viewport associated to parent widget
        virtual QWidget* viewport( void ) const;

        private:

        enum
        {
            SHADOW_SIZE_TOP = 5,
            SHADOW_SIZE_BOTTOM = 5,
            SHADOW_SIZE_LEFT = 5,
            SHADOW_SIZE_RIGHT = 5
        };

        //! helper
        StyleHelper& _helper;

        KStatefulBrush _viewFocusBrush;
        KStatefulBrush _viewHoverBrush;

        //! shadow area
        ShadowArea _area;

        //!@name widget state
        //@{
        bool _focus;
        bool _hover;
        qreal _opacity;
        AnimationMode _mode;

    };

}

#endif
