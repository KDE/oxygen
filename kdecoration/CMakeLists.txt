add_definitions(-DTRANSLATION_DOMAIN="oxygen_kdecoration")

find_package(KDecoration2 REQUIRED)
find_package(KF6 REQUIRED COMPONENTS CoreAddons ConfigWidgets WindowSystem)
find_package(Qt${QT_MAJOR_VERSION} CONFIG REQUIRED COMPONENTS DBus)

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
  if (XCB_XCB_FOUND AND QT_MAJOR_VERSION EQUAL "5")
    find_package(Qt6 REQUIRED CONFIG COMPONENTS X11Extras)
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

### build library
add_library(oxygendecoration MODULE
    ${oxygendecoration_SRCS}
)
set_target_properties(oxygendecoration PROPERTIES
    OUTPUT_NAME org.kde.oxygen
)

target_link_libraries(oxygendecoration
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::DBus
    PRIVATE
        oxygenstyle5
        oxygenstyleconfig5
        KDecoration2::KDecoration
        KF6::ConfigCore
        KF6::CoreAddons
        KF6::ConfigWidgets
        KF6::GuiAddons
        KF6::KCMUtils
        KF6::I18n
        KF6::WindowSystem)

if(OXYGEN_HAVE_X11)
  target_link_libraries(oxygendecoration PUBLIC XCB::XCB)
  if (QT_MAJOR_VERSION EQUAL "5")
    target_link_libraries(oxygendecoration PUBLIC Qt::X11Extras)
  else()
    target_link_libraries(oxygendecoration PUBLIC Qt::GuiPrivate)
  endif()
endif()

add_subdirectory(config)

install(TARGETS oxygendecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration2)
