#ifndef oxygenshadowconfiguration_h
#define oxygenshadowconfiguration_h

//////////////////////////////////////////////////////////////////////////////
// oxygenshadowconfiguration.h
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

#include "oxygen_export.h"

#include <KConfigGroup>
#include <QtGui/QPalette>

namespace OxygenConfig
{

    static const QString SHADOW_SIZE = "Size";
    static const QString SHADOW_VOFFSET= "VerticalOffset";
    static const QString SHADOW_INNER_COLOR = "InnerColor";
    static const QString SHADOW_OUTER_COLOR = "OuterColor";
    static const QString SHADOW_USE_OUTER_COLOR = "UseOuterColor";

    static const QString ANIMATIONS_DURATION = "AnimationsDuration";
    static const QString USE_DROP_SHADOWS = "UseDropShadows";
    static const QString USE_OXYGEN_SHADOWS = "UseOxygenShadows";

}

namespace Oxygen
{

    class OXYGEN_EXPORT ShadowConfiguration
    {

        public:

        //! button size enumeration
        //! default constructor
        ShadowConfiguration( QPalette::ColorGroup );

        //! constructor from KConfig
        ShadowConfiguration( QPalette::ColorGroup, const KConfigGroup& );

        //! destructor
        virtual ~ShadowConfiguration( void )
        {}

        //! write to kconfig group
        virtual void write( KConfigGroup& ) const;

        //! color group
        QPalette::ColorGroup colorGroup( void ) const
        { return _colorGroup; }

        //! enability
        void setEnabled( bool value )
        { _enabled = value; }

        //! enability
        bool isEnabled( void ) const
        { return _enabled; }

        //! shadow size
        qreal shadowSize( void ) const
        { return _shadowSize; }

        //! shadow size
        void setShadowSize( qreal value )
        { _shadowSize = value; }

        //! vertical offset
        qreal verticalOffset( void ) const
        { return _verticalOffset; }

        //! vertical offset
        void setVerticalOffset( qreal value )
        { _verticalOffset = value; }

        //! inner color
        QColor innerColor( void ) const
        { return _innerColor; }

        //! inner color
        void setInnerColor( QColor );

        //! outer color
        QColor outerColor( void ) const
        { return _outerColor; }

        //! outer color
        void setOuterColor( QColor );

        //! use outer color
        bool useOuterColor( void ) const
        { return _useOuterColor; }

        //! use outer color
        void setUseOuterColor( bool value )
        { _useOuterColor = value; }

        //! equal to operator
        bool operator == (const ShadowConfiguration& other ) const
        {
            return
                _colorGroup == other._colorGroup &&
                _enabled == other._enabled &&
                _shadowSize == other._shadowSize &&
                _verticalOffset == other._verticalOffset &&
                _innerColor == other._innerColor &&
                ( _useOuterColor == false || _outerColor == other._outerColor ) &&
                _useOuterColor == other._useOuterColor;
        }

        private:

        //! color group
        QPalette::ColorGroup _colorGroup;

        //! enability
        bool _enabled;

        //! shadow size
        qreal _shadowSize;

        //! vertical offset
        qreal _verticalOffset;

        //! inner color
        QColor _innerColor;

        //! outer color
        QColor _outerColor;

        //! use outer color
        bool _useOuterColor;

    };

}

#endif
