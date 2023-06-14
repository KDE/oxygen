set(oxygendecoration_config_SRCS
    oxygenanimationconfigwidget.cpp
    oxygenconfigwidget.cpp
    oxygendetectwidget.cpp
    oxygenexceptiondialog.cpp
    oxygenexceptionlistwidget.cpp
    oxygenexceptionmodel.cpp
    ../oxygenexceptionlist.cpp
)

ki18n_wrap_ui(oxygendecoration_config_SRCS
   ui/oxygenconfigurationui.ui
   ui/oxygendetectwidget.ui
   ui/oxygenexceptiondialog.ui
   ui/oxygenexceptionlistwidget.ui
)

kcoreaddons_add_plugin(kcm_oxygendecoration SOURCES kcm_oxygendecoration.cpp ${oxygendecoration_config_SRCS} INSTALL_NAMESPACE "${KDECORATION_KCM_PLUGIN_DIR}")
kconfig_add_kcfg_files(kcm_oxygendecoration ../oxygensettings.kcfgc)
target_include_directories(kcm_oxygendecoration PRIVATE ${CMAKE_SOURCE_DIR}/kdecoration ${CMAKE_BINARY_DIR}/kdecoration/)
target_link_libraries(kcm_oxygendecoration
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::DBus
    PRIVATE
        KF6::ConfigCore
        KF6::CoreAddons
        KF6::ConfigWidgets
        KF6::GuiAddons
        KF6::I18n
        KF6::IconThemes
        KF6::KCMUtils
        KF6::WindowSystem
        oxygenstyle5
        oxygenstyleconfig5
)

if(OXYGEN_HAVE_X11)
  target_link_libraries(kcm_oxygendecoration PUBLIC Qt::GuiPrivate XCB::XCB)
endif()

kcmutils_generate_desktop_file(kcm_oxygendecoration)
