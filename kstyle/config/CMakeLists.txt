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

if(OXYGEN_USE_KDE4)

  kde4_add_kcfg_files(kstyle_oxygen_config_PART_SRCS ../oxygenstyleconfigdata.kcfgc)

  kde4_add_ui_files(kstyle_oxygen_config_PART_FORMS_HEADERS ${kstyle_oxygen_config_PART_FORMS})

  kde4_add_plugin(kstyle_oxygen_config
      ${kstyle_oxygen_config_PART_SRCS}
      ${kstyle_oxygen_config_PART_FORMS_HEADERS}
 )

  target_link_libraries(kstyle_oxygen_config ${KDE4_KDEUI_LIBS})
  target_link_libraries(kstyle_oxygen_config oxygenstyleconfig)
  install(TARGETS kstyle_oxygen_config DESTINATION ${PLUGIN_INSTALL_DIR}/)

else()

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

  target_link_libraries(kstyle_oxygen_config Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
  target_link_libraries(kstyle_oxygen_config KF5::Completion KF5::I18n)
  target_link_libraries(kstyle_oxygen_config oxygenstyleconfig5)

  install(TARGETS kstyle_oxygen_config DESTINATION ${QT_PLUGIN_INSTALL_DIR})
  install(FILES oxygenstyleconfig.desktop DESTINATION  ${SERVICES_INSTALL_DIR})

endif()

########### next target ###############
if(NOT OXYGEN_USE_KDE4)

  find_package(KF5KCMUtils CONFIG)
  set_package_properties(KF5KCMUtils PROPERTIES
      DESCRIPTION "KF5 KCM utils Framework"
      URL "https://projects.kde.org/projects/frameworks/kcmutils"
      TYPE OPTIONAL
      PURPOSE "Required for oxygen-settings5 application to build")

  if( KF5KCMUtils_FOUND )

      set(oxygen_settings_SOURCES main.cpp)
      add_executable(oxygen-settings5 ${oxygen_settings_SOURCES} )
      target_link_libraries(oxygen-settings5 Qt5::Core Qt5::Gui Qt5::Widgets )
      target_link_libraries(oxygen-settings5 KF5::I18n KF5::KCMUtils)

      install(TARGETS oxygen-settings5 ${INSTALL_TARGETS_DEFAULT_ARGS})

      # icon
      ecm_install_icons(ICONS 256-apps-oxygen-settings.png
          DESTINATION ${ICON_INSTALL_DIR}
          THEME hicolor )
    endif()
endif()
