#ifndef ShadowConfigurationUi_h
#define ShadowConfigurationUi_h
//////////////////////////////////////////////////////////////////////////////
// ShadowConfigurationUi.h
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

#include <KConfig>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>

// forward declaration
class Ui_ShadowConfiguraionUI;

namespace Oxygen
{

    //! shadow configuration widget
    class ShadowConfigurationUi: public QGroupBox
    {

        Q_OBJECT

        public:

        //! constructor
        explicit ShadowConfigurationUi( QWidget* );

        //! destructor
        virtual ~ShadowConfigurationUi( void );

        //! group
        void setGroup( QPalette::ColorGroup group )
        { _group = group; }

        //! read defaults
        void readDefaults( KConfig* config )
        { readConfig( config, true ); }

        //! read config
        void readConfig( KConfig* config )
        { readConfig( config, false ); }

        //! write config
        void writeConfig( KConfig* ) const;

        //! true if modified
        bool isModified( void ) const;

        signals:

        //! emmitted when configuration is changed
        void changed( void );

        protected slots:

        //! update enable state of outer color chooser
        void enableOuterColor( void );

        protected:

        //! read config
        void readConfig( KConfig*, bool );

        private:

        //! ui
        Ui_ShadowConfiguraionUI* ui;

        //! color group
        QPalette::ColorGroup _group;

    };

}

#endif
