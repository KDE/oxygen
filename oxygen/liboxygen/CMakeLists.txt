################# X11 #################
add_definitions( -DHAVE_X11=${X11_FOUND} )

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
