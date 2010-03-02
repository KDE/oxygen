set(oxygenstyle_LIB_SRCS oxygenanimation.cpp
                         oxygenhelper.cpp
                         tileset.cpp
   )

if(WIN32)
    set (LIB_MODE)
else(WIN32)
    set (LIB_MODE SHARED)
endif(WIN32)

kde4_add_library(oxygenstyle ${LIB_MODE} ${oxygenstyle_LIB_SRCS})

target_link_libraries(oxygenstyle ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
set_target_properties(oxygenstyle PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS oxygenstyle ${INSTALL_TARGETS_DEFAULT_ARGS} )

# Note: no headers installed
