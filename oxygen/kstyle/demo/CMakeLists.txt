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
    main.cpp)

kde4_add_ui_files(oxygen_demo_SOURCES
    ui/oxygenbenchmarkwidget.ui
    ui/oxygenbuttondemowidget.ui
    ui/oxygenframedemowidget.ui
    ui/oxygeninputdemowidget.ui
    ui/oxygenlistdemowidget.ui
    ui/oxygenmdidemowidget.ui
    ui/oxygensliderdemowidget.ui
    ui/oxygentabdemowidget.ui
)

kde4_add_executable( oxygen-demo ${oxygen_demo_SOURCES} )

target_link_libraries( oxygen-demo
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTGUI_LIBRARY})

########### install files ###############
install (TARGETS oxygen-demo ${INSTALL_TARGETS_DEFAULT_ARGS} )
