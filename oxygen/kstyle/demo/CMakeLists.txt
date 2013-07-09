include_directories( ${KDE4_KDEUI_INCLUDES} )


########### next target ###############
set( oxygen_demo_SOURCES
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

set( oxygen_demo_FORMS
    ui/oxygenbenchmarkwidget.ui
    ui/oxygenbuttondemowidget.ui
    ui/oxygenframedemowidget.ui
    ui/oxygeninputdemowidget.ui
    ui/oxygenlistdemowidget.ui
    ui/oxygenmdidemowidget.ui
    ui/oxygensliderdemowidget.ui
    ui/oxygentabdemowidget.ui
)

qt5_wrap_ui(oxygen_demo_FORMS_HEADERS ${oxygen_demo_FORMS})

kde4_add_executable( oxygen-demo
    ${oxygen_demo_SOURCES}
    ${oxygen_demo_FORMS_HEADERS}
 )

qt5_use_modules( oxygen-demo
    Core
    Gui
    Widgets
    # needed for KDEUI
    PrintSupport
    Xml
    Network
    Concurrent
    X11Extras
)

target_link_libraries(oxygen-demo
    ${kguiaddons_LIBRARIES}
    ${kwidgets_LIBRARIES}
    ${KWidgetsAddons_LIBRARIES}
    ${ki18n_LIBRARIES}
    ${KDE4_KDEUI_LIBS}
)

########### install files ###############
install (TARGETS oxygen-demo ${INSTALL_TARGETS_DEFAULT_ARGS} )
