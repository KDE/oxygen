#ifndef oxygentransitions_h
#define oxygentransitions_h

//////////////////////////////////////////////////////////////////////////////
// oxygentransitions.h
// container for all transition engines
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygencomboboxengine.h"
#include "oxygenlabelengine.h"
#include "oxygenlineeditengine.h"
#include "oxygenstackedwidgetengine.h"

namespace Oxygen
{

    //! stores engines
    class Transitions: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        explicit Transitions( QObject* );

        //! destructor
        virtual ~Transitions( void )
        {}

        //! register animations corresponding to given widget, depending on its type.
        void registerWidget( QWidget* widget ) const;

        /*! unregister all animations associated to a widget */
        void unregisterWidget( QWidget* widget ) const;

        //! qlabel engine
        ComboBoxEngine& comboBoxEngine( void ) const
        { return *_comboBoxEngine; }

        //! qlabel engine
        LabelEngine& labelEngine( void ) const
        { return *_labelEngine; }

        //! qlineedit engine
        LineEditEngine& lineEditEngine( void ) const
        { return *_lineEditEngine; }

        //! stacked widget engine
        StackedWidgetEngine& stackedWidgetEngine( void ) const
        { return *_stackedWidgetEngine; }

        public slots:

        //! setup engines
        void setupEngines( void );

        private:

        //! register new engine
        void registerEngine( BaseEngine* engine )
        { _engines.push_back( engine ); }

        //! qcombobox engine
        ComboBoxEngine* _comboBoxEngine;

        //! qlabel engine
        LabelEngine* _labelEngine;

        //! qlineedit engine
        LineEditEngine* _lineEditEngine;

        //! stacked widget engine
        StackedWidgetEngine* _stackedWidgetEngine;

        //! keep list of existing engines
        QList< BaseEngine::Pointer > _engines;

    };

}

#endif
