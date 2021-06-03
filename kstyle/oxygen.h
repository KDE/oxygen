#ifndef oxygen_h
#define oxygen_h
/*
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QFlags>
#include <QPointer>
#include <QScopedPointer>
#include <QWeakPointer>

namespace Oxygen
{

    //*@name convenience typedef
    //@{

    //* scoped pointer convenience typedef
    template <typename T> using WeakPointer = QPointer<T>;

    //* scoped pointer convenience typedef
    template <typename T> using ScopedPointer = QScopedPointer<T, QScopedPointerPodDeleter>;

    //@}

    //* metrics
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
        LineEdit_FrameWidth = 6,

        // menu items
        MenuItem_MarginWidth = 3,
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
        CheckBox_Size = 23,
        CheckBox_FocusMarginWidth = 2,
        CheckBox_ItemSpacing = 6,

        // menubar items
        MenuBarItem_MarginWidth = 10,
        MenuBarItem_MarginHeight = 6,

        // scrollbars
        ScrollBar_MinSliderHeight = 21,

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
        Slider_GrooveThickness = 7,
        Slider_ControlThickness = 21,

        // tabbar
        TabBar_TabMarginHeight = 6,
        TabBar_TabMarginWidth = 12,
        TabBar_TabMinWidth = 80,
        TabBar_TabMinHeight = 28,
        TabBar_TabItemSpacing = 8,
        TabBar_TabOverlap = 1,
        TabBar_TabOffset = 4,
        TabBar_BaseOverlap = 7,

        // tab widget
        TabWidget_MarginWidth = 4,

        // toolbox
        ToolBox_TabMinWidth = 80,
        ToolBox_TabItemSpacing = 4,

        // tooltips
        ToolTip_FrameWidth = 3,

        // list headers
        Header_MarginWidth = 6,
        Header_ItemSpacing = 4,
        Header_ArrowSize = 10,

        // tree view
        ItemView_ArrowSize = 10,
        ItemView_ItemMarginWidth = 3,

        // splitter
        Splitter_SplitterWidth = 3,

    };

    //* animation mode
    enum AnimationMode
    {
        AnimationNone = 0,
        AnimationHover = 0x1,
        AnimationFocus = 0x2,
        AnimationEnable = 0x4,
        AnimationPressed = 0x8
    };

    Q_DECLARE_FLAGS(AnimationModes, AnimationMode)

    //* arrow orientation
    enum ArrowOrientation
    {
        ArrowNone,
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight
    };


    //* get polygon corresponding to generic arrow
    enum ArrowSize
    {
        ArrowNormal,
        ArrowSmall,
        ArrowTiny
    };

    //* internal option flags to pass arguments around
    enum StyleOption
    {
        Sunken = 0x1,
        Focus = 0x2,
        Hover = 0x4,
        Disabled = 0x8,
        NoFill = 0x10,
        HoleOutline = 0x20,
        HoleContrast = 0x80
    };

    Q_DECLARE_FLAGS(StyleOptions, StyleOption)

    //* shadow area
    enum ShadowArea
    {
        ShadowAreaTop,
        ShadowAreaBottom,
        ShadowAreaLeft,
        ShadowAreaRight
    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS( Oxygen::AnimationModes );
Q_DECLARE_OPERATORS_FOR_FLAGS( Oxygen::StyleOptions );

#endif
