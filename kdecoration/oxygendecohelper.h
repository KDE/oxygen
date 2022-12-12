#ifndef oxygendecohelper_h
#define oxygendecohelper_h

/*
 * SPDX-FileCopyrightText: 2013 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2008 Long Huynh Huu <long.upcase@googlemail.com>
 * SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * SPDX-FileCopyrightText: 2007 Casper Boemann <cbr@boemann.dk>
 * SPDX-FileCopyrightText: 2015 David Edmundson <davidedmundson@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-only
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

    //* reset all caches
    void invalidateCaches() override;

    //* windeco buttons
    QPixmap windecoButton(const QColor &color, const QColor &glow, bool sunken, int size = 21);

private:
    //* windeco buttons
    Cache<QPixmap> _windecoButtonCache;
};
}

#endif // __OXYGEN_STYLE_HELPER_H
