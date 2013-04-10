#ifndef oxygen_config_h
#define oxygen_config_h

//////////////////////////////////////////////////////////////////////////////
// config.h
// -------------------
//
// Copyright (c) 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
// Copyright (C) 2008 Lubos Lunak <l.lunak@kde.org>
//
// Based on the Quartz configuration module,
//     Copyright (c) 2001 Karol Szwed <gallium@kde.org>
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
#include "oxygenconfiguration.h"
#include "../oxygendecorationdefines.h"

#include <KSharedConfig>
#include <QtGui/QPalette>

namespace Oxygen {

    class Configuration;

    // oxygen configuration object
    class Config: public QObject
    {

        Q_OBJECT

        public:

        //! constructor
        Config( KConfig* conf, QWidget* parent );

        //! destructor
        ~Config();

        signals:

        //! emmited whenever configuration is changed
        void changed();

        //! emmited whenever configuration is changed
        void changed( bool );

        public slots:

        //! load configuration
        /*! although kconfiggroup argument is not used. It is required by KWin API */
        void load( const KConfigGroup& )
        { load(); }

        //! save configuration
        /*! although kconfiggroup argument is not used. It is required by KWin API */
        void save( KConfigGroup& )
        { save(); }

        //! load configuration
        void load( void );

        //! save configuration
        void save( void );

        //! restore defaults
        void defaults( void );

        //! toggle expert mode
        void toggleExpertMode( bool );

        private slots:

        //! update change state
        void updateChanged( void );

        private:

        //! load configuration
        void loadConfiguration( ConfigurationPtr );

        //! user interface
        ConfigWidget* _configWidget;

        //! kconfiguration object
        KSharedConfig::Ptr _configuration;

    };

} //namespace Oxygen

#endif
