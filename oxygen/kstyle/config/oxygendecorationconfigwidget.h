#ifndef oxygendecorationconfigwidget_h
#define oxygendecorationconfigwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygendecorationconfigwidget.h
// decoration configuration widget
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

#include "oxygenconfigwidget.h"
#include "oxygenshadowconfigdata.h"
#include "ui_oxygendecorationconfigwidget.h"
#include "ui_oxygenshadowconfigbox.h"

namespace Oxygen
{

    class DecorationConfigWidget: public ConfigWidget
    {

        Q_OBJECT

        public:

        //! constructor
        explicit DecorationConfigWidget( QWidget* = 0 );

        public slots:

        //! read current configuration
        virtual void load( void );

        //! save current configuration
        virtual void save( void );

        //! defaults
        virtual void defaults( void );

        protected slots:

        //! check whether configuration is changed and emit appropriate signal if yes
        virtual void updateChanged();

        //! shadow mode changed
        virtual void shadowModeChanged( int );

        protected:

        //!@name shadow configuration handling
        //@{
        virtual void setupShadowConf( Ui_ShadowConfigBox&, QWidget* );
        virtual void loadShadowConf( const Ui_ShadowConfigBox&, ShadowConfigData& );
        virtual void saveShadowConf( const Ui_ShadowConfigBox&, ShadowConfigData& );
        virtual bool shadowConfChanged( const Ui_ShadowConfigBox&, const ShadowConfigData& ) const;

        //!@name mapping between strings and integer
        //@{

        QString frameBorder( int ) const;
        QString sizeGripMode( int ) const;
        QString blendColor( int ) const;
        QString shadowMode( int ) const;
        QString shadowCacheMode( int ) const;

        //@}

        private:

        //! ui
        Ui_DecorationConfigWidget ui;

        ShadowConfigData inactiveShadowConf_;
        ShadowConfigData activeShadowConf_;

        Ui_ShadowConfigBox inactiveShadowUi;
        Ui_ShadowConfigBox activeShadowUi;

    };

}

#endif
