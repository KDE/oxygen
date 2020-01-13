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
    ui/oxygenframedemowidget.ui
    ui/oxygeninputdemowidget.ui
    ui/oxygenlistdemowidget.ui
    ui/oxygenmdidemowidget.ui
    ui/oxygensliderdemowidget.ui
    ui/oxygentabdemowidget.ui
)

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
