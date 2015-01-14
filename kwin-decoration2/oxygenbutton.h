#ifndef oxygenbutton_h
#define oxygenbutton_h

//////////////////////////////////////////////////////////////////////////////
// Button.h
// -------------------
//
// Copyright (c) 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
// Copyright (c) 2006, 2007 Casper Boemann <cbr@boemann.dk>
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

#include "oxygenanimation.h"
#include "oxygen.h"
#include "oxygendecohelper.h"
#include "oxygendecoration.h"


#include <KDecoration2/DecorationButton>

namespace Oxygen
{
    class Client;

    enum ButtonStatus {
    };

    Q_DECLARE_FLAGS(ButtonState, ButtonStatus)

        class Button : public KDecoration2::DecorationButton
    {

        Q_OBJECT

        //! declare animation progress property
        Q_PROPERTY( qreal glowIntensity READ glowIntensity WRITE setGlowIntensity )

        public:
        Button (QObject *parent, const QVariantList &args);
        static Button *create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent);

        //! destructor
        ~Button();

        //! destructor
        QSize sizeHint() const;

        //! set force inactive
        /*! returns true if value was actually changed */
        void setForceInactive( const bool& value )
        { _forceInactive = value; }

        //! configuration reset
        virtual void reset( unsigned long );

        //!@name glow animation
        //@{
        void setGlowIntensity( qreal value )
        {
            if( _glowIntensity == value ) return;
            _glowIntensity = value;
            update();
        }

        qreal glowIntensity( void ) const
        { return _glowIntensity; }

        //@}

        //! render buttn to provided painter
        virtual void paint(QPainter *painter, const QRect &repaintRegion) Q_DECL_OVERRIDE;

        protected:

        //! draw icon
        void drawIcon( QPainter* );

        //! color
        QColor buttonDetailColor( const QPalette& ) const;

        //! color
        QColor buttonDetailColor( const QPalette& palette, bool active ) const;

        //! true if animation is in progress
        bool isAnimated( void ) const
        { return _glowAnimation->isRunning(); }

        //! true if button is active
        bool isActive( void ) const;

        //! true if buttons hover are animated
        bool buttonAnimationsEnabled( void ) const;


        int buttonHeight( void ) const
        { return qobject_cast<Decoration*>(decoration().data())->buttonHeight(); }

        //!@name button properties
        //@{

        //! true if button if of menu type
        bool isMenuButton( void ) const
        { return type() == KDecoration2::DecorationButtonType::Menu || type() == KDecoration2::DecorationButtonType::ApplicationMenu; }

        //! true if button is of toggle type
        bool isToggleButton( void ) const
        { return type() == KDecoration2::DecorationButtonType::OnAllDesktops || type() == KDecoration2::DecorationButtonType::KeepAbove || type() == KDecoration2::DecorationButtonType::KeepBelow; }

        //! true if button if of close type
        bool isCloseButton( void ) const
        { return type() == KDecoration2::DecorationButtonType::Close; }

        //! true if button has decoration
        bool hasDecoration( void ) const
        { return !isMenuButton() && type() != KDecoration2::DecorationButtonType::Close; }

        //@}

        private:
           explicit Button(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent);

        //! backing store pixmap (when compositing is not active)
        QPixmap _pixmap;

        InternalSettingsPtr m_internalSettings;

        //! true if button should be forced inactive
        bool _forceInactive;

        //! glow animation
        Animation* _glowAnimation;

        //! glow intensity
        qreal _glowIntensity;


    };

} //namespace Oxygen

#endif
