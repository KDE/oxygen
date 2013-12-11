include_directories( ${KDEBASE_WORKSPACE_SOURCE_DIR}/libs/oxygen)

########### next target ###############
set(kstyle_oxygen_config_PART_SRCS
    oxygenanimationconfigwidget.cpp
    oxygenfollowmouseanimationconfigitem.cpp
    oxygenstyleconfig.cpp
)

kconfig_add_kcfg_files(kstyle_oxygen_config_PART_SRCS ../oxygenstyleconfigdata.kcfgc)

set( kstyle_oxygen_config_PART_FORMS
    ui/oxygenfollowmouseanimationconfigbox.ui
    ui/oxygenstyleconfig.ui
)

qt5_wrap_ui(kstyle_oxygen_config_PART_FORMS_HEADERS ${kstyle_oxygen_config_PART_FORMS})

add_library(kstyle_oxygen_config MODULE
    ${kstyle_oxygen_config_PART_SRCS}
    ${kstyle_oxygen_config_PART_FORMS_HEADERS}
)

target_link_libraries(kstyle_oxygen_config Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
target_link_libraries(kstyle_oxygen_config KF5::Completion)
target_link_libraries(kstyle_oxygen_config oxygenstyleconfig)

install(TARGETS kstyle_oxygen_config DESTINATION ${QT_PLUGIN_INSTALL_DIR}/kf5 )

########### next target ###############
set(oxygen_settings_SOURCES
    oxygenconfigdialog.cpp
    main.cpp)

set(oxygen_settings_FORMS ui/oxygenconfigdialog.ui)
qt5_wrap_ui(oxygen_settings_FORMS_HEADERS ${oxygen_settings_FORMS})

add_executable(oxygen-settings
    ${oxygen_settings_SOURCES}
    ${oxygen_settings_FORMS_HEADERS}
)

target_link_libraries(oxygen-settings Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(oxygen-settings KF5::I18n KF5::WidgetsAddons KF5::ConfigCore KF5::ConfigGui KF5::Service)

install (TARGETS oxygen-settings ${INSTALL_TARGETS_DEFAULT_ARGS} )
