#ifndef ShadowConfigWidget_h
#define ShadowConfigWidget_h
//////////////////////////////////////////////////////////////////////////////
// ShadowConfigWidget.h
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
#include <QCheckBox>
#include <QGroupBox>

#include "oxygen_config_export.h"

// forward declaration
class Ui_ShadowConfiguraionUI;

namespace Oxygen
{

    //* shadow configuration widget
    class OXYGEN_CONFIG_EXPORT ShadowConfigWidget: public QGroupBox
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ShadowConfigWidget( QWidget* );

        //* destructor
        ~ShadowConfigWidget( void ) override;

        //* group
        void setGroup( QPalette::ColorGroup group )
        { _group = group; }

        //* read defaults
        void setDefaults( void )
        { load(true ); }

        //* read config
        void load( void )
        { load( false ); }

        //* write config
        void save( void ) const;

        //* true if modified
        bool isChanged( void ) const
        { return _changed; }

        Q_SIGNALS:

        //* emmitted when configuration is changed
        void changed( bool );

        private Q_SLOTS:

        //* update changed state
        void updateChanged();

        private:

        //* read config
        void load( bool );

        //* set changed state
        void setChanged( bool value )
        {
            _changed = value;
            emit changed( value );
        }

        //* ui
        Ui_ShadowConfiguraionUI* ui = nullptr;

        //* color group
        QPalette::ColorGroup _group = QPalette::Inactive;

        //* changed state
        bool _changed = false;

    };

}

#endif
