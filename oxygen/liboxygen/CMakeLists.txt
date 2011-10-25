set(oxygenstyle_LIB_SRCS
    oxygenanimation.cpp
    oxygenhelper.cpp
    oxygenitemmodel.cpp
    oxygenshadowcache.cpp
    oxygenshadowconfiguration.cpp
    oxygentileset.cpp)

kde4_add_library(oxygenstyle SHARED ${oxygenstyle_LIB_SRCS})

target_link_libraries(oxygenstyle ${KDE4_KDEUI_LIBS})

if ( X11_FOUND )
   target_link_libraries(oxygenstyle ${X11_LIBRARIES})
endif( X11_FOUND )

set_target_properties(oxygenstyle PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS oxygenstyle ${INSTALL_TARGETS_DEFAULT_ARGS} )

# next target
set(oxygenstyleconfig_LIB_SRCS
    oxygenanimationconfigitem.cpp
    oxygenbaseanimationconfigwidget.cpp
    oxygengenericanimationconfigitem.cpp)

kde4_add_ui_files(oxygenstyleconfig_LIB_SRCS
    ui/oxygenanimationconfigitem.ui
    ui/oxygenanimationconfigwidget.ui
    ui/oxygengenericanimationconfigbox.ui
)

kde4_add_library(oxygenstyleconfig SHARED ${oxygenstyleconfig_LIB_SRCS})

target_link_libraries(oxygenstyleconfig ${KDE4_KDEUI_LIBS})

if ( X11_FOUND )
   target_link_libraries(oxygenstyleconfig ${X11_LIBRARIES})
endif( X11_FOUND )

set_target_properties(oxygenstyleconfig PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} DEFINE_SYMBOL MAKE_OXYGENSTYLE_LIB)
install(TARGETS oxygenstyleconfig ${INSTALL_TARGETS_DEFAULT_ARGS} )

# Note: no headers installed
