#ifndef oxygenmetrics_h
#define oxygenmetrics_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
        // frames
        Frame_FrameWidth = 2,
        Frame_FrameRadius = 3,

        // layout
        Layout_TopLevelMarginWidth = 10,
        Layout_ChildMarginWidth = 6,
        Layout_DefaultSpacing = 6,

        // line editors
        LineEdit_FrameWidth = 8,

        // menu items
        MenuItem_MarginWidth = 4,
        MenuItem_ItemSpacing = 4,
        MenuItem_AcceleratorSpace = 16,
        MenuButton_IndicatorWidth = 20,

        // combobox
        ComboBox_FrameWidth = 6,

        // spinbox
        SpinBox_FrameWidth = LineEdit_FrameWidth,
        SpinBox_ArrowButtonWidth = 20,

        // groupbox title margin
        GroupBox_TitleMarginWidth = 4,

        // buttons
        Button_MinWidth = 80,
        Button_MarginWidth = 6,
        Button_ItemSpacing = 4,

        // tool buttons
        ToolButton_MarginWidth = 6,
        ToolButton_ItemSpacing = 4,
        ToolButton_InlineIndicatorWidth = 12,

        // checkboxes and radio buttons
        CheckBox_Size = 21,
        CheckBox_FocusMarginWidth = 2,
        CheckBox_ItemSpacing = 8,

        // menubar items
        MenuBarItem_MarginWidth = 10,
        MenuBarItem_MarginHeight = 8,

        // scrollbars
        ScrollBar_Extend = 20,
        ScrollBar_SliderWidth = 10,
        ScrollBar_MinSliderHeight = 10,
        ScrollBar_NoButtonHeight = (ScrollBar_Extend-ScrollBar_SliderWidth)/2,
        ScrollBar_SingleButtonHeight = ScrollBar_Extend,
        ScrollBar_DoubleButtonHeight = 2*ScrollBar_Extend,

        // toolbars
        ToolBar_FrameWidth = 2,
        ToolBar_HandleExtent = 10,
        ToolBar_HandleWidth = 6,
        ToolBar_SeparatorWidth = 8,
        ToolBar_ExtensionWidth = 20,
        ToolBar_ItemSpacing = 0,

        // progressbars
        ProgressBar_BusyIndicatorSize = 14,
        ProgressBar_Thickness = 6,
        ProgressBar_ItemSpacing = 4,

        // mdi title bar
        TitleBar_MarginWidth = 4,

        // sliders
        Slider_TickLength = 8,
        Slider_TickMarginWidth = 2,
        Slider_GrooveThickness = 6,
        Slider_ControlThickness = 20,

        // tabbar
        TabBar_TabMarginHeight = 4,
        TabBar_TabMarginWidth = 8,
        TabBar_TabMinWidth = 80,
        TabBar_TabMinHeight = 28,
        TabBar_TabOverlap = 1,
        TabBar_BaseOverlap = 2,

        // tab widget
        TabWidget_MarginWidth = 4,

        // toolbox
        ToolBox_TabMinWidth = 80,
        ToolBox_TabItemSpacing = 4,

        // tooltips
        ToolTip_FrameWidth = 3,

        // list headers
        Header_MarginWidth = 8,
        Header_ItemSpacing = 4,
        Header_ArrowSize = 10,

        // tree view
        ItemView_ArrowSize = 10,
        ItemView_ItemMarginWidth = 4,

        // splitter
        Splitter_SplitterWidth = 3,

    };

}

#endif
