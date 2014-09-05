set(kwin_oxygen_config_PART_SRCS
    oxygenanimationconfigwidget.cpp
    oxygenconfig.cpp
    oxygenconfigwidget.cpp
    oxygendetectwidget.cpp
    oxygenexceptiondialog.cpp
    oxygenexceptionlistwidget.cpp
    oxygenexceptionmodel.cpp
    ../oxygenexceptionlist.cpp
 )

set(kwin_oxygen_config_PART_FORMS
   ui/oxygenconfigurationui.ui
   ui/oxygendetectwidget.ui
   ui/oxygenexceptiondialog.ui
   ui/oxygenexceptionlistwidget.ui
)

if(OXYGEN_USE_KDE4)
  kde4_add_kcfg_files(kwin_oxygen_config_PART_SRCS ../oxygenconfiguration.kcfgc)
  kde4_add_ui_files(kwin_oxygen_config_PART_FORMS_HEADERS ${kwin_oxygen_config_PART_FORMS})
  kde4_add_plugin(kwin_oxygen_config
      ${kwin_oxygen_config_PART_SRCS}
      ${kwin_oxygen_config_PART_FORMS_HEADERS})

  target_link_libraries(
      kwin_oxygen_config
      ${KDE4_KDEUI_LIBS}
      ${KDE4WORKSPACE_KDECORATIONS_LIBS}
      oxygenstyle oxygenstyleconfig
 )

  if(OXYGEN_HAVE_X11)
    target_link_libraries(kwin_oxygen_config ${X11_XCB_LIBRARIES})
    target_link_libraries(kwin_oxygen_config ${XCB_LIBRARIES})
  endif()

  install(TARGETS kwin_oxygen_config DESTINATION ${PLUGIN_INSTALL_DIR})

else()
  kconfig_add_kcfg_files(kwin_oxygen_config_PART_SRCS ../oxygenconfiguration.kcfgc)

  ki18n_wrap_ui(kwin_oxygen_config_PART_FORMS_HEADERS ${kwin_oxygen_config_PART_FORMS})
  add_library(kwin_oxygen_config MODULE
      ${kwin_oxygen_config_PART_SRCS}
      ${kwin_oxygen_config_PART_FORMS_HEADERS})

  target_link_libraries(kwin_oxygen_config Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
  target_link_libraries(kwin_oxygen_config KF5::I18n KF5::WindowSystem KF5::Completion)
  target_link_libraries(kwin_oxygen_config kdecorations oxygenstyle5  oxygenstyleconfig5)

  if(OXYGEN_HAVE_X11)
    target_link_libraries(kwin_oxygen_config XCB::XCB)
    target_link_libraries(kwin_oxygen_config Qt5::X11Extras)
  endif()

  install(TARGETS kwin_oxygen_config DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/kdecorations/config)
endif()
