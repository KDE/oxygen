#ifndef oxygenstyleconfig_h
#define oxygenstyleconfig_h
/*
Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
Copyright (C) 2003 Sandro Giessl <ceebx@users.sourceforge.net>

based on the Keramik configuration dialog:
Copyright (c) 2003 Maksim Orlovich <maksim.orlovich@kdemail.net>

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include "ui_oxygenstyleconfig.h"

namespace Oxygen
{
    class AnimationConfigWidget;

    class StyleConfig: public QWidget, Ui::OxygenStyleConfig
    {

        Q_OBJECT

        public:

        //! constructor
        explicit StyleConfig(QWidget*);

        //! destructor
        virtual ~StyleConfig( void )
        {}

        //! event filter
        virtual bool eventFilter( QObject*, QEvent* );

        Q_SIGNALS:

        //! emmited whenever one option is changed.
        void changed(bool);

        public Q_SLOTS:

        //! load setup from config data
        void load( void );

        //! save current state
        void save( void );

        //! restore all default values
        void defaults( void );

        //Everything below this is internal.

        //! reset to saved configuration
        void reset( void );

        //! toggle expert mode
        virtual void toggleExpertMode( bool );

        protected Q_SLOTS:

        //! update minimum size
        void updateMinimumSize( void );

        //! update layout
        /*! needed in expert mode to accommodate with animations config widget size changes */
        void updateLayout( void );

        //! update modified state when option is checked/unchecked
        void updateChanged( void );

        //! toggle expert mode
        virtual void toggleExpertModeInternal( void )
        { toggleExpertModeInternal( !_expertMode ); }

        //! toggle expert mode
        virtual void toggleExpertModeInternal( bool );

        protected:

        //! event processing
        virtual bool event( QEvent* );

        // menu mode from ui
        int menuMode( void ) const;

        // expander size from ui
        int triangularExpanderSize( void ) const;

        private:

        bool _expertMode;

        //! animation config (expert mode only)
        AnimationConfigWidget* _animationConfigWidget;

    };

}
#endif
