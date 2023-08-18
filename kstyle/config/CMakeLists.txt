### configuration plugin
add_definitions(-DTRANSLATION_DOMAIN=\"oxygen_style_config\")
set(kstyle_oxygen_config_PART_SRCS
    oxygenanimationconfigwidget.cpp
    oxygenfollowmouseanimationconfigitem.cpp
    oxygenstyleconfig.cpp
)

set(kstyle_oxygen_config_PART_FORMS
    ui/oxygenfollowmouseanimationconfigbox.ui
    ui/oxygenstyleconfig.ui
)

set(kstyle_oxygen_config_PART_SRCS ${kstyle_oxygen_config_PART_SRCS} oxygenstyleconfigmodule.cpp)

kconfig_add_kcfg_files(kstyle_oxygen_config_PART_SRCS ../oxygenstyleconfigdata.kcfgc)

set(kstyle_oxygen_config_PART_FORMS
    ui/oxygenfollowmouseanimationconfigbox.ui
    ui/oxygenstyleconfig.ui
)

ki18n_wrap_ui(kstyle_oxygen_config_PART_FORMS_HEADERS ${kstyle_oxygen_config_PART_FORMS})

add_library(kstyle_oxygen_config MODULE
    ${kstyle_oxygen_config_PART_SRCS}
    ${kstyle_oxygen_config_PART_FORMS_HEADERS}
)

target_link_libraries(kstyle_oxygen_config Qt::Core Qt::Gui Qt::Widgets Qt::DBus)
target_link_libraries(kstyle_oxygen_config KF6::Completion KF6::CoreAddons KF6::I18n)
target_link_libraries(kstyle_oxygen_config KF6::KCMUtils)
target_link_libraries(kstyle_oxygen_config oxygenstyleconfig6)


install(TARGETS kstyle_oxygen_config DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/kstyle_config/)

########### next target ###############
find_package(KF6KCMUtils CONFIG)
set_package_properties(KF6KCMUtils PROPERTIES
    DESCRIPTION "KF6 KCM utils Framework"
    URL "https://projects.kde.org/projects/frameworks/kcmutils"
    TYPE OPTIONAL
    PURPOSE "Required for oxygen-settings application to build")

if(KF6KCMUtils_FOUND)
    set(oxygen_settings_SOURCES main.cpp)
    add_executable(oxygen-settings6 ${oxygen_settings_SOURCES} )
    target_link_libraries(oxygen-settings6 Qt::Core Qt::Gui Qt::Widgets )
    target_link_libraries(oxygen-settings6 KF6::I18n KF6::KCMUtils)


    install(TARGETS oxygen-settings6 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

    # icon
    ecm_install_icons(ICONS 256-apps-oxygen-settings.png
        DESTINATION ${KDE_INSTALL_ICONDIR}
        THEME hicolor )
endif()
