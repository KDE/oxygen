#ifndef OXYGEN_DECORATION_H
#define OXYGEN_DECORATION_H

/*
 * Copyright 2014  Martin Gräßlin <mgraesslin@kde.org>
 * Copyright 2014  Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygen.h"
#include "oxygensettings.h"

#include <KDecoration2/Decoration>
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationSettings>

#include <QPalette>
#include <QPropertyAnimation>
#include <QVariant>

namespace KDecoration2
{
    class DecorationButton;
    class DecorationButtonGroup;
}

namespace Oxygen
{
    class SizeGrip;
    class TitleAnimationData;




    class Decoration : public KDecoration2::Decoration
    {
        Q_OBJECT

        public:

        //* constructor
        explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());

        //* destructor
        virtual ~Decoration();

        //* paint
        void paint(QPainter *painter, const QRect &repaintRegion) override;

        //* internal settings
        InternalSettingsPtr internalSettings() const
        { return m_internalSettings; }

        //* caption height
        int captionHeight() const;

        int layoutMetric() const {
            return 2;
        }

        //* button height
        int buttonHeight() const;

        //@}

        public Q_SLOTS:
        void init() override;

        private Q_SLOTS:
        void reconfigure();
        void recalculateBorders();
        void updateButtonPositions();
        void updateTitleBar();

        private:
        QRect captionRect() const;

        void createButtons();

        void createShadow();

        bool glowIsAnimated() const {return false;}
        qreal glowIntensity() const {return 1.0;}

        //* text color
        QColor titlebarTextColor(const QPalette&) const;

        //* text color
        QColor titlebarTextColor(const QPalette& palette, bool windowActive, bool itemActive ) const;

        //* text color
        QColor titlebarContrastColor(const QPalette& palette ) const;

        //* text color
        QColor titlebarContrastColor(const QColor& color ) const;

        //* window background
        void renderWindowBackground( QPainter*, const QRect&, const QPalette& ) const;

        //* window border
        // this draws a "blue" border around active window
        void renderWindowBorder( QPainter*, const QRect&, const QPalette& ) const;

        //* title outline
        void renderTitleOutline( QPainter*, const QRect&, const QPalette& ) const;

        //* title text
        /** second color, if valid, is for contrast pixel */
        void renderTitleText( QPainter*, const QRect&, const QColor&, const QColor& = QColor() ) const;

        //* title text
        /** second color, if valid, is for contrast pixel */
        void renderTitleText( QPainter*, const QRect&, const QString&, const QColor&, const QColor& = QColor(), bool elide = true ) const;

        //* title text
        QPixmap renderTitleText( const QRect&, const QString&, const QColor&, bool elide = true ) const;

        //* corners
        void renderCorners( QPainter*, const QRect &frame, const QPalette &) const;

        bool hideTitleBar() const
        {return false;}

        //*@name border size
        //@{
        int borderSize(bool bottom = false) const;
        inline bool hasNoBorders( void ) const;
        inline bool hasNoSideBorders( void ) const;
        //@}

        //*@name maximization modes
        //@{
        inline bool isMaximized( void ) const;
        inline bool isMaximizedHorizontally( void ) const;
        inline bool isMaximizedVertically( void ) const;
        //@}

        //*@name size grip
        //@{
        void createSizeGrip( void );
        void deleteSizeGrip( void );
        SizeGrip* sizeGrip( void ) const
        { return m_sizeGrip; }
        //@}

        InternalSettingsPtr m_internalSettings;

        QList<KDecoration2::DecorationButton*> m_buttons;
        KDecoration2::DecorationButtonGroup *m_leftButtons = nullptr;
        KDecoration2::DecorationButtonGroup *m_rightButtons = nullptr;

        //* size grip widget
        SizeGrip *m_sizeGrip = nullptr;

        TitleAnimationData *_titleAnimationData;
    };

    bool Decoration::hasNoBorders( void ) const
    {
        if( m_internalSettings && m_internalSettings->mask() & BorderSize ) return m_internalSettings->borderSize() == InternalSettings::BorderNone;
        else return settings()->borderSize() == KDecoration2::BorderSize::None;
    }

    bool Decoration::hasNoSideBorders( void ) const
    {
        if( m_internalSettings && m_internalSettings->mask() & BorderSize ) return m_internalSettings->borderSize() == InternalSettings::BorderNoSides;
        else return settings()->borderSize() == KDecoration2::BorderSize::NoSides;
    }

    bool Decoration::isMaximized( void ) const { return client().data()->isMaximized() && !m_internalSettings->drawBorderOnMaximizedWindows(); }
    bool Decoration::isMaximizedHorizontally( void ) const { return client().data()->isMaximizedHorizontally() && !m_internalSettings->drawBorderOnMaximizedWindows(); }
    bool Decoration::isMaximizedVertically( void ) const { return client().data()->isMaximizedVertically() && !m_internalSettings->drawBorderOnMaximizedWindows(); }

}

#endif
