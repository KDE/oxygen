add_definitions(-DTRANSLATION_DOMAIN=\"liboxygenstyleconfig\")

################# configuration #################
configure_file(config-liboxygen.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-liboxygen.h)

################# liboxygenstyle #################
set(oxygenstyle_LIB_SRCS
    oxygenanimation.cpp
    oxygenhelper.cpp
    oxygenitemmodel.cpp
    oxygenshadowcache.cpp
    oxygentileset.cpp
    oxygenutil.cpp)

if(OXYGEN_USE_KDE4)

  kde4_add_kcfg_files(oxygenstyle_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
 )

  kde4_add_library(oxygenstyle SHARED ${oxygenstyle_LIB_SRCS})

  target_link_libraries(oxygenstyle ${KDE4_KDEUI_LIBS})

  if(OXYGEN_HAVE_X11)
    target_link_libraries(oxygenstyle ${X11_XCB_LIBRARIES})
    target_link_libraries(oxygenstyle ${XCB_LIBRARIES})
  endif()

  set_target_properties(oxygenstyle PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

 if(WIN32)
      # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
      # when using MSVC if _USE_MATH_DEFINES is defined (this is needed for angle.cpp)
      target_compile_definitions(oxygenstyle PRIVATE _USE_MATH_DEFINES _GNU_SOURCE)
  endif()
  install(TARGETS oxygenstyle ${INSTALL_TARGETS_DEFAULT_ARGS})

else()

  kconfig_add_kcfg_files(oxygenstyle_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
 )

  add_library(oxygenstyle5 ${oxygenstyle_LIB_SRCS})

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

  if(OXYGEN_HAVE_X11)
      target_link_libraries(oxygenstyle5 PRIVATE XCB::XCB Qt5::X11Extras)
  endif()

  set_target_properties(oxygenstyle5 PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
  if(WIN32)
      # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
      # when using MSVC if _USE_MATH_DEFINES is defined (this is needed for angle.cpp)
      target_compile_definitions(oxygenstyle5 PRIVATE _USE_MATH_DEFINES _GNU_SOURCE)
  endif()
  install(TARGETS oxygenstyle5 ${INSTALL_TARGETS_DEFAULT_ARGS})

endif()

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

if(OXYGEN_USE_KDE4)

  kde4_add_ui_files(oxygenstyleconfig_LIB_FORMS_HEADERS ${oxygenstyleconfig_LIB_FORMS})
  kde4_add_kcfg_files(oxygenstyleconfig_LIB_SRCS
      oxygenactiveshadowconfiguration.kcfgc
      oxygeninactiveshadowconfiguration.kcfgc
 )

  kde4_add_library(oxygenstyleconfig SHARED
      ${oxygenstyleconfig_LIB_SRCS}
      ${oxygenstyleconfig_LIB_FORMS_HEADERS})

  target_link_libraries(oxygenstyleconfig ${KDE4_KDEUI_LIBS})

  set_target_properties(oxygenstyleconfig PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
  install(TARGETS oxygenstyleconfig ${INSTALL_TARGETS_DEFAULT_ARGS})

  # NB: although we install no headers, we still need to have a link
  #     interface, since other Oxygen targets link to this library
  target_link_libraries(oxygenstyleconfig oxygenstyle)

else()

  ki18n_wrap_ui(oxygenstyleconfig_LIB_FORMS_HEADERS ${oxygenstyleconfig_LIB_FORMS})

  kconfig_add_kcfg_files(oxygenstyleconfig_LIB_SRCS
      oxygenactiveshadowconfiguration.kcfgc
      oxygeninactiveshadowconfiguration.kcfgc
 )

  add_library(oxygenstyleconfig5 SHARED
      ${oxygenstyleconfig_LIB_SRCS}
      ${oxygenstyleconfig_LIB_FORMS_HEADERS})

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
  install(TARGETS oxygenstyleconfig5 ${INSTALL_TARGETS_DEFAULT_ARGS})
  # Note: no headers installed

endif()
