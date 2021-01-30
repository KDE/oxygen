add_definitions(-DTRANSLATION_DOMAIN="oxygen_kdecoration")

find_package(KDecoration2 REQUIRED)
find_package(KF5 REQUIRED COMPONENTS CoreAddons ConfigWidgets WindowSystem)
find_package(Qt5 CONFIG REQUIRED COMPONENTS DBus)

### XCB
find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
  DESCRIPTION "X protocol C-language Binding"
  URL "http://xcb.freedesktop.org"
  TYPE OPTIONAL
  PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

include_directories(${CMAKE_SOURCE_DIR}/liboxygen)
include_directories(${CMAKE_BINARY_DIR}/liboxygen)

if(UNIX AND NOT APPLE)

  set(OXYGEN_HAVE_X11 ${XCB_XCB_FOUND})
  if (XCB_XCB_FOUND)
    find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
  endif()

else()

  set(OXYGEN_HAVE_X11 FALSE)

endif()

################# configuration #################
configure_file(config-oxygen.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-oxygen.h )

################# newt target #################
### plugin classes
set(oxygendecoration_SRCS
    oxygenbutton.cpp
    oxygendecohelper.cpp
    oxygendecoration.cpp
    oxygenexceptionlist.cpp
    oxygensettingsprovider.cpp
    oxygensizegrip.cpp)

kconfig_add_kcfg_files(oxygendecoration_SRCS oxygensettings.kcfgc)

### config classes
### they are kept separately because they might move in a separate library in the future
set(oxygendecoration_config_SRCS
    config/oxygenanimationconfigwidget.cpp
    config/oxygenconfigwidget.cpp
    config/oxygendetectwidget.cpp
    config/oxygenexceptiondialog.cpp
    config/oxygenexceptionlistwidget.cpp
    config/oxygenexceptionmodel.cpp
)

set(oxygendecoration_config_PART_FORMS
   config/ui/oxygenconfigurationui.ui
   config/ui/oxygendetectwidget.ui
   config/ui/oxygenexceptiondialog.ui
   config/ui/oxygenexceptionlistwidget.ui
)

ki18n_wrap_ui(oxygendecoration_config_PART_FORMS_HEADERS ${oxygendecoration_config_PART_FORMS})

### build library
add_library(oxygendecoration MODULE
    ${oxygendecoration_SRCS}
    ${oxygendecoration_config_SRCS}
    ${oxygendecoration_config_PART_FORMS_HEADERS})

target_link_libraries(oxygendecoration
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::DBus
    PRIVATE
        oxygenstyle5
        oxygenstyleconfig5
        KDecoration2::KDecoration
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::ConfigWidgets
        KF5::GuiAddons
        KF5::I18n
        KF5::WindowSystem)

if(OXYGEN_HAVE_X11)
  target_link_libraries(oxygendecoration
    PUBLIC
      Qt::X11Extras
      XCB::XCB)
endif()


install(TARGETS oxygendecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2)
install(FILES config/oxygendecorationconfig.desktop DESTINATION  ${SERVICES_INSTALL_DIR})
