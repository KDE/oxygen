add_definitions(-DTRANSLATION_DOMAIN="oxygen_kdecoration")

find_package(KDecoration3 REQUIRED)
find_package(KF6 REQUIRED COMPONENTS CoreAddons ConfigWidgets WindowSystem)
find_package(Qt${QT_MAJOR_VERSION} CONFIG REQUIRED COMPONENTS DBus)

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
)

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
        Qt6::Core
        Qt6::Gui
        Qt6::DBus
    PRIVATE
        oxygenstyle6
        oxygenstyleconfig6
        KDecoration3::KDecoration
        KF6::ConfigCore
        KF6::CoreAddons
        KF6::ConfigWidgets
        KF6::GuiAddons
        KF6::KCMUtils
        KF6::I18n
        KF6::WindowSystem)

if(OXYGEN_HAVE_X11)
  target_link_libraries(oxygendecoration PUBLIC XCB::XCB  Qt6::GuiPrivate)
endif()

add_subdirectory(config)

install(TARGETS oxygendecoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration3)
