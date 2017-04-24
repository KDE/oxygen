add_definitions(-DTRANSLATION_DOMAIN=\"oxygen_style_demo\")

include_directories(${CMAKE_BINARY_DIR}/liboxygen)

########### next target ###############
set(oxygen_demo_SOURCES
    oxygenbenchmarkwidget.cpp
    oxygenbuttondemowidget.cpp
    oxygendemodialog.cpp
    oxygendemowidget.cpp
    oxygenframedemowidget.cpp
    oxygeninputdemowidget.cpp
    oxygenlistdemowidget.cpp
    oxygenmdidemowidget.cpp
    oxygensimulator.cpp
    oxygensliderdemowidget.cpp
    oxygentabdemowidget.cpp
    oxygentabwidget.cpp
    main.cpp)

set(oxygen_demo_FORMS
    ui/oxygenbenchmarkwidget.ui
    ui/oxygenbuttondemowidget.ui
    ui/oxygendemodialog.ui
    ui/oxygenframedemowidget.ui
    ui/oxygeninputdemowidget.ui
    ui/oxygenlistdemowidget.ui
    ui/oxygenmdidemowidget.ui
    ui/oxygensliderdemowidget.ui
    ui/oxygentabdemowidget.ui
)

if(OXYGEN_USE_KDE4)

  kde4_add_ui_files(oxygen_demo_FORMS_HEADERS ${oxygen_demo_FORMS})

  kde4_add_executable(oxygen-demo
      ${oxygen_demo_SOURCES}
      ${oxygen_demo_FORMS_HEADERS}
 )

  target_link_libraries(oxygen-demo ${KDE4_KDEUI_LIBS})

  ########### install files ###############
  install(TARGETS oxygen-demo ${INSTALL_TARGETS_DEFAULT_ARGS})

else()

  find_package(KF5ConfigWidgets CONFIG)
  if(KF5ConfigWidgets_FOUND)
    set(oxygen_demo_SOURCES ${oxygen_demo_SOURCES} oxygenschemechooser.cpp oxygenstylechooser.cpp)
    add_definitions(-DHAVE_SCHEME_CHOOSER)
  endif()

  ki18n_wrap_ui(oxygen_demo_FORMS_HEADERS ${oxygen_demo_FORMS})

  add_executable(oxygen-demo5
      ${oxygen_demo_SOURCES}
      ${oxygen_demo_FORMS_HEADERS}
 )

  target_link_libraries(oxygen-demo5 Qt5::Core Qt5::Gui Qt5::Widgets)
  target_link_libraries(oxygen-demo5 KF5::Completion KF5::ConfigGui KF5::WidgetsAddons KF5::I18n)
  if(KF5ConfigWidgets_FOUND)
    target_link_libraries(oxygen-demo5 KF5::ConfigWidgets)
  endif()

  ########### install files ###############
  install(TARGETS oxygen-demo5 ${INSTALL_TARGETS_DEFAULT_ARGS})

endif()
