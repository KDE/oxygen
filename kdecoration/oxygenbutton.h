#ifndef oxygenbutton_h
#define oxygenbutton_h

/*
* Copyright 2006, 2007 Riccardo Iaconelli <riccardo@kde.org>
* Copyright 2006, 2007 Casper Boemann <cbr@boemann.dk>
* Copyright 2009 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright 2015 David Edmundson <davidedmundson@kde.org>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "oxygenanimation.h"
#include "oxygen.h"
#include "oxygendecohelper.h"
#include "oxygendecoration.h"


#include <KDecoration2/DecorationButton>

namespace Oxygen
{

    class Button : public KDecoration2::DecorationButton
    {

        Q_OBJECT

        //* declare active state opacity
        Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

        public:

        //* constructor
        explicit Button(QObject *parent, const QVariantList &args);

        //* destructor
        virtual ~Button() = default;

        //* button creation
        static Button *create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent);

        //* render
        virtual void paint(QPainter *painter, const QRect &repaintRegion) Q_DECL_OVERRIDE;

        //* flag
        enum Flag
        {
            FlagNone,
            FlagStandalone,
            FlagFirstInList,
            FlagLastInList
        };

        //* flag
        void setFlag( Flag value )
        { m_flag = value; }

        //* standalone buttons
        bool isStandAlone() const { return m_flag == FlagStandalone; }

        //* offset
        void setOffset( const QPointF& value )
        { m_offset = value; }

        //* horizontal offset, for rendering
        void setHorizontalOffset( qreal value )
        { m_offset.setX( value ); }

        //* vertical offset, for rendering
        void setVerticalOffset( qreal value )
        { m_offset.setY( value ); }

        //* set icon size
        void setIconSize( const QSize& value )
        { m_iconSize = value; }

        //*@name active state change animation
        //@{
        void setOpacity( qreal value )
        {
            if( m_opacity == value ) return;
            m_opacity = value;
            update();
        }

        qreal opacity( void ) const
        { return m_opacity; }

        //@}

        protected:

        //* draw icon
        void drawIcon( QPainter* );

        //* color
        QColor buttonDetailColor( const QPalette& ) const;

        //* color
        QColor buttonDetailColor( const QPalette& palette, bool active ) const;

        //* true if animation is in progress
        bool isAnimated( void ) const
        { return m_animation->state() == QPropertyAnimation::Running; }

        //* true if button is active
        bool isActive( void ) const;

        //*@name button properties
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
        { return !isMenuButton();}

        //@}

        private Q_SLOTS:

        //* apply configuration changes
        void reconfigure();

        //* animation state
        void updateAnimationState(bool);

        private:

        //* private constructor
        explicit Button(KDecoration2::DecorationButtonType type, Decoration *decoration, QObject *parent);

        Flag m_flag = FlagNone;

        //* glow animation
        QPropertyAnimation* m_animation;

        //* vertical offset (for rendering)
        QPointF m_offset;

        //* icon size
        QSize m_iconSize;

        //* glow intensity
        qreal m_opacity;


    };

} //namespace

#endif
