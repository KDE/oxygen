add_definitions(-DTRANSLATION_DOMAIN=\"oxygen_style_demo\")


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

find_package(KF${QT_MAJOR_VERSION}ConfigWidgets CONFIG)
if(KF${QT_MAJOR_VERSION}ConfigWidgets_FOUND)
    set(oxygen_demo_SOURCES ${oxygen_demo_SOURCES} oxygenschemechooser.cpp oxygenstylechooser.cpp)
    add_definitions(-DHAVE_SCHEME_CHOOSER)
endif()

ki18n_wrap_ui(oxygen_demo_FORMS_HEADERS ${oxygen_demo_FORMS})

add_executable(oxygen-demo${QT_MAJOR_VERSION}
    ${oxygen_demo_SOURCES}
    ${oxygen_demo_FORMS_HEADERS}
)

target_include_directories(oxygen-demo${QT_MAJOR_VERSION} PRIVATE ${CMAKE_BINARY_DIR}/liboxygen${QT_MAJOR_VERSION})

target_link_libraries(oxygen-demo${QT_MAJOR_VERSION} Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Gui Qt${QT_MAJOR_VERSION}::Widgets)
target_link_libraries(oxygen-demo${QT_MAJOR_VERSION} KF${QT_MAJOR_VERSION}::Completion KF${QT_MAJOR_VERSION}::ConfigGui KF${QT_MAJOR_VERSION}::WidgetsAddons KF${QT_MAJOR_VERSION}::I18n)

if(KF${QT_MAJOR_VERSION}ConfigWidgets_FOUND)
    target_link_libraries(oxygen-demo${QT_MAJOR_VERSION} KF${QT_MAJOR_VERSION}::ConfigWidgets)
endif()

########### install files ###############
install(TARGETS oxygen-demo${QT_MAJOR_VERSION} ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
