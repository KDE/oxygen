/*
 * Copyright 2009 Long Huynh Huu <long.upcase@googlemail.com>
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


#ifndef SETTINGS_H
#define SETTINGS_H
enum MenuHighLightMode {
    MM_DARK=0,
    MM_SUBTLE=1,
    MM_STRONG=2};

struct Settings {
    struct CheckBox {
        bool drawCheck;
    } CheckBox;

    struct Menu {
        MenuHighLightMode highLightMode;
    } Menu;

    struct ScrollBar {
        bool colored;
        int width;
    } ScrollBar;

    struct ProgressBar {
        bool animated;
    } ProgressBar;

    struct ToolBar {
        bool drawItemSeparator;
    } ToolBar;

    struct View {
        bool drawTreeBranchLines;
        bool drawTriangularExpander;
    } View;
};

#endif // SETTINGS_H
