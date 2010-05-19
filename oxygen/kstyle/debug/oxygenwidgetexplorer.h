#ifndef oxygenwidgetexplorer_h
#define oxygenwidgetexplorer_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwidgetexplorer.h
// print widget's and parent's information on mouse click
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

namespace Oxygen
{

    //! print widget's and parent's information on mouse click
    class WidgetExplorer: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        WidgetExplorer( QObject* parent );

        //! enable
        bool enabled( void ) const;

        //! enable
        void setEnabled( bool );

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        protected:

        //! event type
        QString eventType( const QEvent::Type& ) const;

        //! print widget information
        QString widgetInformation( const QWidget* ) const;

        private:

        //! enable state
        bool _enabled;

    };

}

#endif
