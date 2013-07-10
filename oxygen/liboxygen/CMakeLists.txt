cmake_minimum_required(VERSION 2.8.10.1)
include(FeatureSummary)

################# set KDE specific information #################
find_package(ECM 0.0.8 REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

################# Qt5 #################
find_package(Qt5Core REQUIRED NO_MODULE)
find_package(Qt5Gui REQUIRED NO_MODULE)
find_package(Qt5Widgets REQUIRED NO_MODULE)
find_package(Qt5DBus REQUIRED NO_MODULE)
find_package(Qt5Svg REQUIRED NO_MODULE)

################# KF5 #################
find_package(kdeqt5staging REQUIRED)
find_package(KF5 MODULE REQUIRED COMPONENTS
    CMake
    Compiler
    InstallDirs
    KCoreAddons
    KConfig
    kguiaddons
    KWidgetsAddons
    KWindowSystem
    KCodecs KAuth
    ki18n
    kservice
    kwidgets
    ItemViews
    KIconThemes

    # needed for KDEUI
    KNotifications
    KCompletion
    KArchive
    kjs
    ItemModels
    XmlGui
    KJobWidgets
    KConfigWidgets
    Solid
    KWallet
)

# FIXME: remove when kdeui is fully split up
find_package(KDELibs4 REQUIRED NO_MODULE)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${KF5_INCLUDE_DIRS}
    ${KF5_INCLUDE_DIRS}/KDE
)

################# X11 #################
find_package(X11)
if(X11_FOUND)

    find_package(Qt5X11Extras REQUIRED NO_MODULE)
    include_directories( ${X11_X11_INCLUDE_PATH} )
    set (HAVE_X11 1)

else()

    set (HAVE_X11 0)

endif()

add_definitions( -DHAVE_X11=${HAVE_X11} )

################# liboxygenstyle #################
set(oxygenstyle_LIB_SRCS
    oxygenanimation.cpp
    oxygenhelper.cpp
    oxygenitemmodel.cpp
    oxygenshadowcache.cpp
    oxygentileset.cpp
    oxygenutil.cpp
)

kconfig_add_kcfg_files(oxygenstyle_LIB_SRCS
  oxygenactiveshadowconfiguration.kcfgc
  oxygeninactiveshadowconfiguration.kcfgc
)

add_library(oxygenstyle SHARED ${oxygenstyle_LIB_SRCS})

qt5_use_modules( oxygenstyle Core Gui Widgets )
target_link_libraries(oxygenstyle ${KConfig_LIBRARIES} ${kguiaddons_LIBRARIES} ${kwidgets_LIBRARIES})

if( X11_FOUND )
  target_link_libraries(oxygenstyle ${X11_LIBRARIES})
  qt5_use_modules( oxygenstyle X11Extras )
endif( X11_FOUND )

set_target_properties(oxygenstyle PROPERTIES VERSION 5.0.0 SOVERSION 5 )
install(TARGETS oxygenstyle ${INSTALL_TARGETS_DEFAULT_ARGS} )

################# liboxygenstyleconfig #################
set(oxygenstyleconfig_LIB_SRCS
    oxygenanimationconfigitem.cpp
    oxygenbaseanimationconfigwidget.cpp
    oxygengenericanimationconfigitem.cpp
    oxygenshadowconfigwidget.cpp
)

set(oxygenstyleconfig_LIB_FORMS
    ui/oxygenanimationconfigitem.ui
    ui/oxygenanimationconfigwidget.ui
    ui/oxygengenericanimationconfigbox.ui
    ui/oxygenshadowconfigurationui.ui
)

qt5_wrap_ui(oxygenstyleconfig_LIB_FORMS_HEADERS ${oxygenstyleconfig_LIB_FORMS})

kconfig_add_kcfg_files(oxygenstyleconfig_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
)

add_library(oxygenstyleconfig SHARED
    ${oxygenstyleconfig_LIB_SRCS}
    ${oxygenstyleconfig_LIB_FORMS_HEADERS} )

qt5_use_modules( oxygenstyleconfig
    Core
    Gui
    Widgets
    # needed for KDEUI
    PrintSupport
    Xml
    Network
    Concurrent
    X11Extras )

target_link_libraries(oxygenstyleconfig oxygenstyle)
target_link_libraries(oxygenstyleconfig
  ${KConfig_LIBRARIES}
  ${kguiaddons_LIBRARIES}
  ${kwidgets_LIBRARIES}
  ${ki18n_LIBRARIES}
  ${KDE4_KDEUI_LIBS}
)

if ( X11_FOUND )
  target_link_libraries(oxygenstyleconfig ${X11_LIBRARIES})
  qt5_use_modules( oxygenstyle X11Extras )
endif( X11_FOUND )

set_target_properties(oxygenstyleconfig PROPERTIES VERSION 5.0.0 SOVERSION 5 DEFINE_SYMBOL MAKE_OXYGENSTYLE_LIB )
install(TARGETS oxygenstyleconfig ${INSTALL_TARGETS_DEFAULT_ARGS} )

# Note: no headers installed
