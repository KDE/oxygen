################# X11 #################
find_package(X11)
if(X11_FOUND)
  find_package(Qt5X11Extras REQUIRED NO_MODULE)
  include_directories( ${X11_X11_INCLUDE_PATH} )
  set (HAVE_X11 1)
else(X11_FOUND)
   set (HAVE_X11 0)
endif(X11_FOUND)

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
# set(oxygenstyleconfig_LIB_SRCS
#     oxygenanimationconfigitem.cpp
#     oxygenbaseanimationconfigwidget.cpp
#     oxygengenericanimationconfigitem.cpp
#     oxygenshadowconfigwidget.cpp
# )
#
# kde4_add_ui_files(oxygenstyleconfig_LIB_SRCS
#     ui/oxygenanimationconfigitem.ui
#     ui/oxygenanimationconfigwidget.ui
#     ui/oxygengenericanimationconfigbox.ui
#     ui/oxygenshadowconfigurationui.ui
# )
#
# kconfig_add_kcfg_files(oxygenstyleconfig_LIB_SRCS
#   oxygenactiveshadowconfiguration.kcfgc
#   oxygeninactiveshadowconfiguration.kcfgc
# )
#
# add_library(oxygenstyleconfig SHARED ${oxygenstyleconfig_LIB_SRCS})
#
# target_link_libraries(oxygenstyleconfig oxygenstyle)
# target_link_libraries(oxygenstyleconfig ${KDE4_KDEUI_LIBS})
#
# if ( X11_FOUND )
#    target_link_libraries(oxygenstyleconfig ${X11_LIBRARIES})
# endif( X11_FOUND )
#
# set_target_properties(oxygenstyleconfig PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} DEFINE_SYMBOL MAKE_OXYGENSTYLE_LIB)
# install(TARGETS oxygenstyleconfig ${INSTALL_TARGETS_DEFAULT_ARGS} )

# Note: no headers installed
