//////////////////////////////////////////////////////////////////////////////
// oxygenshadowconfiguration.cpp
// shadow configuration
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

#include "oxygenshadowconfiguration.h"

#include <cassert>

namespace Oxygen
{

    //_________________________________________________________
    ShadowConfiguration::ShadowConfiguration( QPalette::ColorGroup colorGroup ):
        _colorGroup( colorGroup ),
        _enabled( true )
    {

        // check colorgroup
        assert( colorGroup == QPalette::Active || colorGroup == QPalette::Inactive );

        if( colorGroup == QPalette::Active )
        {

            _shadowSize = 40;
            _verticalOffset = 0.1;
            _useOuterColor = true;

            _innerColor = QColor( "#70EFFF" );
            _outerColor = QColor( "#54A7F0" );

        } else {

            _shadowSize = 40;
            _verticalOffset = 0.2;
            _useOuterColor = false;

            _innerColor = QColor( Qt::black );
            _outerColor = QColor( Qt::black );

        }

    }

    //_________________________________________________________
    ShadowConfiguration::ShadowConfiguration( QPalette::ColorGroup colorGroup, const KConfigGroup& group ):
        _colorGroup( colorGroup ),
        _enabled( true )
    {

        // get default configuration
        ShadowConfiguration defaultConfiguration( ShadowConfiguration::colorGroup() );

        setShadowSize( group.readEntry( OxygenConfig::SHADOW_SIZE, defaultConfiguration.shadowSize() ) );
        setVerticalOffset( group.readEntry( OxygenConfig::SHADOW_VOFFSET, defaultConfiguration.verticalOffset() ) );
        setUseOuterColor( group.readEntry( OxygenConfig::SHADOW_USE_OUTER_COLOR, defaultConfiguration.useOuterColor() ) );

        setInnerColor( group.readEntry( OxygenConfig::SHADOW_INNER_COLOR, defaultConfiguration.innerColor() ) );
        setOuterColor( group.readEntry( OxygenConfig::SHADOW_OUTER_COLOR, defaultConfiguration.outerColor() ) );

    }

    //_________________________________________________________
    void ShadowConfiguration::write( KConfigGroup& group ) const
    {
        ShadowConfiguration defaultConfiguration( _colorGroup );
        if( shadowSize() != defaultConfiguration.shadowSize() ) group.writeEntry( OxygenConfig::SHADOW_SIZE, shadowSize() );
        if( verticalOffset() != defaultConfiguration.verticalOffset() ) group.writeEntry( OxygenConfig::SHADOW_VOFFSET, verticalOffset() );
        if( innerColor() != defaultConfiguration.innerColor() ) group.writeEntry( OxygenConfig::SHADOW_INNER_COLOR, innerColor() );
        if( outerColor() != defaultConfiguration.outerColor() ) group.writeEntry( OxygenConfig::SHADOW_OUTER_COLOR, outerColor() );
        if( useOuterColor() != defaultConfiguration.useOuterColor() ) group.writeEntry( OxygenConfig::SHADOW_USE_OUTER_COLOR, useOuterColor() );
    }

    //_________________________________________________________
    void ShadowConfiguration::setInnerColor( QColor color )
    { _innerColor = color.isValid() ? color : ShadowConfiguration( colorGroup() ).innerColor(); }

    //_________________________________________________________
    void ShadowConfiguration::setOuterColor( QColor color )
    { _outerColor = color.isValid() ? color : ShadowConfiguration( colorGroup() ).outerColor(); }

}
