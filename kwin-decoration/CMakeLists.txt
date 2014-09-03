########### add version number into compilation defines
add_definitions(-DHAVE_X11=${X11_FOUND})
add_definitions(-DTRANSLATION_DOMAIN=\"oxygen_kwin_deco\")

if(USE_KDE4)
  include_directories ( ../liboxygen)
endif()

add_subdirectory(config)

########### next target ###############
set(kwin_oxygen_SRCS
     oxygenbutton.cpp
     oxygenclient.cpp
     oxygenclientgroupitemdata.cpp
     oxygendecohelper.cpp
     oxygenexceptionlist.cpp
     oxygenfactory.cpp
     oxygensizegrip.cpp
     oxygentitleanimationdata.cpp
  )

if(USE_KDE4)

  kde4_add_kcfg_files(kwin_oxygen_SRCS oxygenconfiguration.kcfgc )
  kde4_add_plugin( kwin3_oxygen ${kwin_oxygen_SRCS} )
  target_link_libraries(
      kwin3_oxygen
      ${KDE4_KDEUI_LIBS}
      ${KDE4WORKSPACE_KDECORATIONS_LIBS}
      oxygenstyle
  )

  if(X11_FOUND)
    target_link_libraries(kwin3_oxygen ${X11_X11_LIB} )
    target_link_libraries(kwin3_oxygen ${X11_XCB_LIBRARIES} )
    target_link_libraries(kwin3_oxygen ${XCB_LIBRARIES} )
  endif()

  install(TARGETS kwin3_oxygen DESTINATION ${PLUGIN_INSTALL_DIR} )
  install( FILES oxygenclient.desktop  DESTINATION  ${DATA_INSTALL_DIR}/kwin/ )

else()

  kconfig_add_kcfg_files(kwin_oxygen_SRCS oxygenconfiguration.kcfgc )

  add_library(kwin3_oxygen MODULE ${kwin_oxygen_SRCS})
  target_link_libraries(kwin3_oxygen Qt5::Widgets )
  target_link_libraries(kwin3_oxygen KF5::GuiAddons KF5::I18n KF5::WindowSystem KF5::Style KF5::Service)

  target_link_libraries(kwin3_oxygen kdecorations)

  target_link_libraries(kwin3_oxygen oxygenstyle5)

  kservice_desktop_to_json(kwin3_oxygen oxygenclient.desktop)

  if(X11_FOUND)
    target_link_libraries(kwin3_oxygen XCB::XCB)
  endif()

  install(TARGETS kwin3_oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/kdecorations)

endif()
