//////////////////////////////////////////////////////////////////////////////
// oxygenanimationconfigwidget.cpp
// animation configuration widget
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

#include "oxygenanimationconfigwidget.h"
#include "oxygenanimationconfigitem.h"
#include "oxygengenericanimationconfigitem.h"

#include <QButtonGroup>
#include <QHoverEvent>
#include <QTextStream>
#include <KLocalizedString>

namespace Oxygen
{

    //_______________________________________________
    AnimationConfigWidget::AnimationConfigWidget( QWidget* parent ):
        BaseAnimationConfigWidget( parent )
    {

        QGridLayout* layout( qobject_cast<QGridLayout*>( BaseAnimationConfigWidget::layout() ) );

        setupItem( layout, m_buttonAnimations = new GenericAnimationConfigItem( this,
            i18n("Button mouseover transition"),
            i18n("Configure window buttons' mouseover highlight animation" ) ) );

        setupItem( layout, m_shadowAnimations = new GenericAnimationConfigItem( this,
            i18n("Window active state change transitions" ),
            i18n("Configure fading between window shadow and glow when window's active state is changed" ) ) );

        // add spacers to the first column, previous row to finalize layout
        layout->addItem( new QSpacerItem( 25, 0 ), _row-1, 0, 1, 1 );

        // add vertical spacer
        layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ), _row, 1, 1, 1 );
        ++_row;

        connect( animationsEnabled(), SIGNAL(toggled(bool)), SLOT(updateChanged()) );
        foreach( AnimationConfigItem* item, findChildren<AnimationConfigItem*>() )
        {
            item->QWidget::setEnabled( false );
            connect( animationsEnabled(), SIGNAL(toggled(bool)), item, SLOT(setEnabled(bool)) );
        }

    }

    //_______________________________________________
    void AnimationConfigWidget::load( void )
    {

        // check configuration
        if( !m_internalSettings ) return;

        animationsEnabled()->setChecked( m_internalSettings.data()->animationsEnabled() );

        m_buttonAnimations->setEnabled( m_internalSettings.data()->buttonAnimationsEnabled() );
        m_buttonAnimations->setDuration( m_internalSettings.data()->buttonAnimationsDuration() );

        m_shadowAnimations->setEnabled( m_internalSettings.data()->shadowAnimationsEnabled() );
        m_shadowAnimations->setDuration( m_internalSettings.data()->shadowAnimationsDuration() );

        setChanged( false );

    }

    //_______________________________________________
    void AnimationConfigWidget::save( void )
    {

        // check configuration
        if( !m_internalSettings ) return;

        // save modifications
        m_internalSettings.data()->setAnimationsEnabled( animationsEnabled()->isChecked() );

        m_internalSettings.data()->setButtonAnimationsEnabled( m_buttonAnimations->enabled() );
        m_internalSettings.data()->setButtonAnimationsDuration( m_buttonAnimations->duration() );

        m_internalSettings.data()->setShadowAnimationsEnabled( m_shadowAnimations->enabled() );
        m_internalSettings.data()->setShadowAnimationsDuration( m_shadowAnimations->duration() );

        setChanged( false );

    }

    //_______________________________________________
    void AnimationConfigWidget::updateChanged( void )
    {

        // check configuration
        if( !m_internalSettings ) return;

        // track modifications
        bool modified( false );
        if( animationsEnabled()->isChecked() != m_internalSettings.data()->animationsEnabled() ) modified = true;
        else if( m_buttonAnimations->enabled() != m_internalSettings.data()->buttonAnimationsEnabled() ) modified = true;
        else if( m_buttonAnimations->duration() != m_internalSettings.data()->buttonAnimationsDuration() ) modified = true;

        else if( m_shadowAnimations->enabled() != m_internalSettings.data()->shadowAnimationsEnabled() ) modified = true;
        else if( m_shadowAnimations->duration() != m_internalSettings.data()->shadowAnimationsDuration() ) modified = true;

        setChanged( modified );

    }

}
