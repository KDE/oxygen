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

kconfig_add_kcfg_files(kwin_oxygen_config_PART_SRCS ../oxygenconfiguration.kcfgc)

set( kwin_oxygen_config_PART_FORMS
   ui/oxygenconfigurationui.ui
   ui/oxygendetectwidget.ui
   ui/oxygenexceptiondialog.ui
   ui/oxygenexceptionlistwidget.ui
)

ki18n_wrap_ui(kwin_oxygen_config_PART_FORMS_HEADERS ${kwin_oxygen_config_PART_FORMS})
add_library(kwin_oxygen_config MODULE
    ${kwin_oxygen_config_PART_SRCS}
    ${kwin_oxygen_config_PART_FORMS_HEADERS})

target_link_libraries(kwin_oxygen_config Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
target_link_libraries(kwin_oxygen_config KF5::I18n KF5::WindowSystem KF5::Completion)
target_link_libraries(kwin_oxygen_config kdecorations oxygenstyle5  oxygenstyleconfig5)

if(X11_FOUND)
  target_link_libraries(kwin_oxygen_config XCB::XCB)
  target_link_libraries(kwin_oxygen_config Qt5::X11Extras)
endif()

install(TARGETS kwin_oxygen_config DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/kdecorations/config)
