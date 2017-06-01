#ifndef oxygendecohelper_h
#define oxygendecohelper_h

/*
 * Copyright 2013 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright 2007 Casper Boemann <cbr@boemann.dk>
 * Copyright 2015  David Edmundson <davidedmundson@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "oxygenhelper.h"
#include <ksharedconfig.h>

//* helper class
/*! contains utility functions used at multiple places in oxygen style */
namespace Oxygen
{

    class DecoHelper : public Helper
    {

        public:

        //* constructor
        explicit DecoHelper();

        //* destructor
        virtual ~DecoHelper()
        {}

        //* reset all caches
        void invalidateCaches() Q_DECL_OVERRIDE;

        //* windeco buttons
        virtual QPixmap windecoButton(const QColor &color, const QColor& glow, bool sunken, int size = 21);

        private:

        //* windeco buttons
        Cache<QPixmap> _windecoButtonCache;

    };

}

#endif // __OXYGEN_STYLE_HELPER_H
