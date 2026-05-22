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
        Qt6::Core
        Qt6::Gui
        Qt6::DBus
    PRIVATE
        KF6::ConfigCore
        KF6::CoreAddons
        KF6::ConfigWidgets
        KF6::GuiAddons
        KF6::I18n
        KF6::IconThemes
        KF6::KCMUtils
        KF6::WindowSystem
        oxygenstyle6
        oxygenstyleconfig6
)

if(OXYGEN_HAVE_X11)
  target_link_libraries(kcm_oxygendecoration PUBLIC Qt6::GuiPrivate XCB::XCB)
endif()

kcmutils_generate_desktop_file(kcm_oxygendecoration)

# kcmshell6 < 6.27.0 doesn't know the --smoke-test param, so skip this test for
# now.
# TODO: Drop this when KF6_MIN_VERSION >= 6.27.0
if(KF6KCMUtils_VERSION VERSION_GREATER_EQUAL "6.27.0")
    add_test(NAME kcm_oxygendecoration_smoketest COMMAND kcmshell6 -style Oxygen --smoke-test ${KDECORATION_KCM_PLUGIN_DIR}/kcm_oxygendecoration)
    set_tests_properties(kcm_oxygendecoration_smoketest PROPERTIES
        ENVIRONMENT_MODIFICATION QT_PLUGIN_PATH=path_list_prepend:${CMAKE_BINARY_DIR}/bin
    )
endif()
