################# X11 #################
add_definitions( -DHAVE_X11=${X11_FOUND} )
add_definitions(-DTRANSLATION_DOMAIN=\"liboxygenstyleconfig\")

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

add_library(oxygenstyle5 ${oxygenstyle_LIB_SRCS})
generate_export_header(oxygenstyle5 EXPORT_MACRO_NAME OXYGEN_EXPORT EXPORT_FILE_NAME oxygen_export.h)

# NB: although we install no headers, we still need to have a link
#     interface, since other Oxygen targets link to this library
target_link_libraries(oxygenstyle5
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        KF5::ConfigCore
        KF5::ConfigWidgets
    PRIVATE
        KF5::GuiAddons
)
target_include_directories(oxygenstyle5 PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
if( X11_FOUND )
    target_link_libraries(oxygenstyle5 PRIVATE ${XCB_LIBRARIES} Qt5::X11Extras)
endif()

set_target_properties(oxygenstyle5 PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR} )
if(WIN32)
    # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
    # when using MSVC if _USE_MATH_DEFINES is defined (this is needed for angle.cpp)
    target_compile_definitions(oxygenstyle5 PRIVATE _USE_MATH_DEFINES _GNU_SOURCE)
endif()
install(TARGETS oxygenstyle5 ${INSTALL_TARGETS_DEFAULT_ARGS} )

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

ki18n_wrap_ui(oxygenstyleconfig_LIB_FORMS_HEADERS ${oxygenstyleconfig_LIB_FORMS})

kconfig_add_kcfg_files(oxygenstyleconfig_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
)

add_library(oxygenstyleconfig5 SHARED
    ${oxygenstyleconfig_LIB_SRCS}
    ${oxygenstyleconfig_LIB_FORMS_HEADERS} )
    
generate_export_header(oxygenstyleconfig5 EXPORT_MACRO_NAME OXYGEN_CONFIG_EXPORT EXPORT_FILE_NAME oxygen_config_export.h)    


# NB: although we install no headers, we still need to have a link
#     interface, since other Oxygen targets link to this library
target_link_libraries(oxygenstyleconfig5
    PUBLIC
        oxygenstyle5
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
    PRIVATE
        KF5::I18n
)

set_target_properties(oxygenstyleconfig5 PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
install(TARGETS oxygenstyleconfig5 ${INSTALL_TARGETS_DEFAULT_ARGS} )

# Note: no headers installed
