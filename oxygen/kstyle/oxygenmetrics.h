#ifndef oxygenmetrics_h
#define oxygenmetrics_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

namespace Oxygen
{

    //! metrics
    /*! these are copied from the old KStyle WidgetProperties */
    enum Metrics
    {
        GlowWidth = 1,

        // checkbox. Do not change, unless
        // changing the actual cached pixmap size
        CheckBox_Size = 21,
        CheckBox_BoxTextSpace = 4,

        // combobox
        ComboBox_FrameWidth = 3,
        ComboBox_ButtonWidth = 19,
        ComboBox_ButtonMargin = 2,
        ComboBox_ButtonMargin_Left = 0,
        ComboBox_ButtonMargin_Right = 4,
        ComboBox_ButtonMargin_Top = 2,
        ComboBox_ButtonMargin_Bottom = 1,

        ComboBox_ContentsMargin = 0,
        ComboBox_ContentsMargin_Left = 2,
        ComboBox_ContentsMargin_Right = 0,
        ComboBox_ContentsMargin_Top = 0,
        ComboBox_ContentsMargin_Bottom = 0,

        // dockwidgets
        DockWidget_FrameWidth = 0,
        DockWidget_SeparatorExtend = 3,
        DockWidget_TitleMargin = 3,

        // generic frames
        Frame_FrameWidth = 3,

        // group boxes
        GroupBox_FrameWidth = 3,

        // header
        Header_TextToIconSpace = 3,
        Header_ContentsMargin = 3,

        // line edit
        LineEdit_FrameWidth = 3,

        // menu item
        MenuItem_AccelSpace = 16,
        MenuItem_ArrowWidth = 11,
        MenuItem_ArrowSpace = 3,
        MenuItem_CheckWidth = 16,
        MenuItem_CheckSpace = 3,
        MenuItem_IconWidth = 12,
        MenuItem_IconSpace = 3,
        MenuItem_Margin = 2,
        MenuItem_MinHeight = 20,

        // menu bar item
        MenuBarItem_Margin = 3,
        MenuBarItem_Margin_Left = 5,
        MenuBarItem_Margin_Right = 5,

        // pushbuttons
        PushButton_ContentsMargin = 5,
        PushButton_ContentsMargin_Left = 8,
        PushButton_ContentsMargin_Top = -1,
        PushButton_ContentsMargin_Right = 8,
        PushButton_ContentsMargin_Bottom = 0,
        PushButton_MenuIndicatorSize = 8,
        PushButton_TextToIconSpace = 6,

        // progress bar
        ProgressBar_BusyIndicatorSize = 10,
        ProgressBar_GrooveMargin = 0,

        // scrollbar
        ScrollBar_MinimumSliderHeight = 21,

        // slider groove height
        Slider_GrooveWidth = 7,

        // spin boxes
        SpinBox_FrameWidth = 3,
        SpinBox_ButtonWidth = 19,
        SpinBox_ButtonMargin = 0,
        SpinBox_ButtonMargin_Left = 2,
        SpinBox_ButtonMargin_Right = 6,
        SpinBox_ButtonMargin_Top = 4,
        SpinBox_ButtonMargin_Bottom = 2,

        // splitter
        Splitter_Width = 3,

        // tabs
        TabBar_BaseOverlap = 7,
        TabBar_BaseHeight = 2,
        TabBar_ScrollButtonWidth = 18,
        TabBar_TabContentsMargin = 4,
        TabBar_TabContentsMargin_Left = 5,
        TabBar_TabContentsMargin_Right = 5,
        TabBar_TabContentsMargin_Top = 2,
        TabBar_TabContentsMargin_Bottom = 4,
        TabBar_TabOverlap =0,

        TabWidget_ContentsMargin = 4,

        // toolbuttons
        ToolButton_ContentsMargin = 4,
        ToolButton_InlineMenuIndicatorSize = 8,
        ToolButton_InlineMenuIndicatorXOff = -11,
        ToolButton_InlineMenuIndicatorYOff = -10,
        ToolButton_MenuIndicatorSize = 11,

        Tree_MaxExpanderSize = 9

    };

}

#endif
