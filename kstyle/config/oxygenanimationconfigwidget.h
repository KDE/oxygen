#ifndef oxygenanimationconfigwidget_h
#define oxygenanimationconfigwidget_h

//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.h
// animation configuration item
// -------------------
//
// Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenbaseanimationconfigwidget.h"

namespace Oxygen
{
    class GenericAnimationConfigItem;
    class FollowMouseAnimationConfigItem;

    class AnimationConfigWidget: public BaseAnimationConfigWidget
    {

        Q_OBJECT

        public:

        //* constructor
        explicit AnimationConfigWidget( QWidget* = 0 );

        public Q_SLOTS:

        //* read current configuration
        void load( void ) override;

        //* save current configuration
        void save( void ) override;

        protected Q_SLOTS:

        //* check whether configuration is changed and emit appropriate signal if yes
        void updateChanged() override;

        private:

        GenericAnimationConfigItem* _genericAnimations = nullptr;
        GenericAnimationConfigItem* _progressBarAnimations = nullptr;
        GenericAnimationConfigItem* _progressBarBusyAnimations = nullptr;
        GenericAnimationConfigItem* _stackedWidgetAnimations = nullptr;
        GenericAnimationConfigItem* _labelAnimations = nullptr;
        GenericAnimationConfigItem* _lineEditAnimations = nullptr;
        GenericAnimationConfigItem* _comboBoxAnimations = nullptr;
        FollowMouseAnimationConfigItem* _toolBarAnimations = nullptr;
        FollowMouseAnimationConfigItem* _menuBarAnimations = nullptr;
        FollowMouseAnimationConfigItem* _menuAnimations = nullptr;

    };

}

#endif
