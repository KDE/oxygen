#ifndef oxygenconfigwidget_h
#define oxygenconfigwidget_h
//////////////////////////////////////////////////////////////////////////////
// oxygenconfigurationui.h
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

#include "ui_oxygenconfigurationui.h"
#include "oxygenexceptionlistwidget.h"
#include "oxygensettings.h"
#include "oxygen.h"

#include <KCModule>
#include <KSharedConfig>

#include <QWidget>
#include <QSharedPointer>

namespace Oxygen
{

    //_____________________________________________
    class ConfigWidget: public KCModule
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ConfigWidget( QWidget*, const QVariantList& );

        //* destructor
        virtual ~ConfigWidget( void ) = default;

        //* default
        void defaults() override;

        //* load configuration
        void load( void ) override;

        //* save configuration
        void save( void ) override;

        //* true if changed
        virtual bool isChanged( void ) const
        { return m_changed; }

        //* exceptions
        ExceptionListWidget* exceptionListWidget( void ) const
        { return m_ui.exceptions; }

        protected Q_SLOTS:

        //* update changed state
        virtual void updateChanged();

        protected:

        //* set changed state
        virtual void setChanged( bool value )
        {
            m_changed = value;
            emit changed( value );
        }

        private:

        //* ui
        Ui_OxygenConfigurationUI m_ui;

        //* kconfiguration object
        KSharedConfig::Ptr m_configuration;

        //* internal exception
        InternalSettingsPtr m_internalSettings;

        //* changed state
        bool m_changed;

    };

}

#endif
