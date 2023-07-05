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
target_link_libraries(kstyle_oxygen_config KF${QT_MAJOR_VERSION}::Completion KF${QT_MAJOR_VERSION}::CoreAddons KF${QT_MAJOR_VERSION}::I18n)
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(kstyle_oxygen_config KF6::KCMUtils)
endif()
target_link_libraries(kstyle_oxygen_config oxygenstyleconfig5)

install(TARGETS kstyle_oxygen_config DESTINATION ${KDE_INSTALL_QTPLUGINDIR})
if (QT_MAJOR_VERSION STREQUAL "5")
    install(FILES oxygenstyleconfig.desktop DESTINATION  ${KDE_INSTALL_KSERVICESDIR})
endif()

########### next target ###############
find_package(KF${QT_MAJOR_VERSION}KCMUtils CONFIG)
set_package_properties(KF${QT_MAJOR_VERSION}KCMUtils PROPERTIES
    DESCRIPTION "KF${QT_MAJOR_VERSION} KCM utils Framework"
    URL "https://projects.kde.org/projects/frameworks/kcmutils"
    TYPE OPTIONAL
    PURPOSE "Required for oxygen-settings5 application to build")

if( KF${QT_MAJOR_VERSION}KCMUtils_FOUND )
    set(oxygen_settings_SOURCES main.cpp)
    add_executable(oxygen-settings5 ${oxygen_settings_SOURCES} )
    target_link_libraries(oxygen-settings5 Qt::Core Qt::Gui Qt::Widgets )
    target_link_libraries(oxygen-settings5 KF${QT_MAJOR_VERSION}::I18n KF${QT_MAJOR_VERSION}::KCMUtils)

    install(TARGETS oxygen-settings5 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

    # icon
    ecm_install_icons(ICONS 256-apps-oxygen-settings.png
        DESTINATION ${KDE_INSTALL_ICONDIR}
        THEME hicolor )
endif()
