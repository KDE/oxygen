/*
 * Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
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

#ifndef __HELPER_H
#define __HELPER_H

#include <QtGui/QColor>

// alphaBlendColors Copyright 2003 Sandro Giessl <ceebx@users.sourceforge.net>
// DEPRECATED
QColor alphaBlendColors(const QColor &backgroundColor, const QColor &foregroundColor, const int alpha);

class OxygenHelper
{
public:
    static QColor backgroundRadialColor(const QColor &color);
    static QColor backgroundTopColor(const QColor &color);
    static QColor backgroundBottomColor(const QColor &color);
};

#endif // __HELPER_H
