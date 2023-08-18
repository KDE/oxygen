add_definitions(-DTRANSLATION_DOMAIN=\"liboxygenstyleconfig\")

################# configuration #################
configure_file(config-liboxygen.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-liboxygen.h)

################# liboxygenstyle #################
set(oxygenstyle_LIB_SRCS
    oxygenanimation.cpp
    oxygenhelper.cpp
    oxygenitemmodel.cpp
    oxygenshadowcache.cpp
    oxygentileset.cpp)

kconfig_add_kcfg_files(oxygenstyle_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
)

add_library(oxygenstyle${QT_MAJOR_VERSION} ${oxygenstyle_LIB_SRCS})

generate_export_header(oxygenstyle${QT_MAJOR_VERSION} BASE_NAME oxygen EXPORT_FILE_NAME oxygen_export.h)

# NB: although we install no headers, we still need to have a link
#     interface, since other Oxygen targets link to this library
target_link_libraries(oxygenstyle${QT_MAJOR_VERSION}
    PUBLIC
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Gui
        Qt${QT_MAJOR_VERSION}::Widgets
        KF${QT_MAJOR_VERSION}::ConfigCore
        KF${QT_MAJOR_VERSION}::ConfigWidgets
        KF${QT_MAJOR_VERSION}::WindowSystem
    PRIVATE
        KF${QT_MAJOR_VERSION}::GuiAddons
)

target_include_directories(oxygenstyle${QT_MAJOR_VERSION} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

if(OXYGEN_HAVE_X11)
    target_link_libraries(oxygenstyle${QT_MAJOR_VERSION} PRIVATE XCB::XCB)
    if (QT_MAJOR_VERSION EQUAL "5")
        target_link_libraries(oxygenstyle5 PRIVATE Qt5::X11Extras)
    else()
        target_link_libraries(oxygenstyle6 PRIVATE Qt6::GuiPrivate)
    endif()
endif()

set_target_properties(oxygenstyle${QT_MAJOR_VERSION} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
if(WIN32)
    # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
    # when using MSVC if _USE_MATH_DEFINES is defined (this is needed for angle.cpp)
    target_compile_definitions(oxygenstyle${QT_MAJOR_VERSION} PRIVATE _USE_MATH_DEFINES _GNU_SOURCE)
endif()
install(TARGETS oxygenstyle${QT_MAJOR_VERSION} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

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

add_library(oxygenstyleconfig${QT_MAJOR_VERSION} SHARED
    ${oxygenstyleconfig_LIB_SRCS}
    ${oxygenstyleconfig_LIB_FORMS_HEADERS})

generate_export_header(oxygenstyleconfig${QT_MAJOR_VERSION} BASE_NAME oxygen_config EXPORT_FILE_NAME oxygen_config_export.h)

# NB: although we install no headers, we still need to have a link
#     interface, since other Oxygen targets link to this library
target_link_libraries(oxygenstyleconfig${QT_MAJOR_VERSION}
    PUBLIC
        oxygenstyle${QT_MAJOR_VERSION}
        Qt${QT_MAJOR_VERSION}::Core
        Qt${QT_MAJOR_VERSION}::Gui
        Qt${QT_MAJOR_VERSION}::Widgets
    PRIVATE
        KF${QT_MAJOR_VERSION}::I18n
)

set_target_properties(oxygenstyleconfig${QT_MAJOR_VERSION} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
install(TARGETS oxygenstyleconfig${QT_MAJOR_VERSION} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)
# Note: no headers installed
