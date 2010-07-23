set(oxygenstyle_LIB_SRCS oxygenanimation.cpp
                         oxygenhelper.cpp
                         oxygenitemmodel.cpp
                         oxygentileset.cpp)

kde4_add_library(oxygenstyle SHARED ${oxygenstyle_LIB_SRCS})

target_link_libraries(oxygenstyle ${KDE4_KDEUI_LIBS})
set_target_properties(oxygenstyle PROPERTIES VERSION ${GENERIC_LIB_VERSION} SOVERSION ${GENERIC_LIB_SOVERSION} )
install(TARGETS oxygenstyle ${INSTALL_TARGETS_DEFAULT_ARGS} )

# Note: no headers installed
