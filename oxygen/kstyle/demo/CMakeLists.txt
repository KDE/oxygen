include_directories( ${KDE4_KDEUI_INCLUDES} )


########### next target ###############
set( oxygen_demo_SOURCES
    oxygenbuttondemowidget.cpp
    oxygendemodialog.cpp
    oxygeninputdemowidget.cpp
    oxygentabdemowidget.cpp
    main.cpp)

kde4_add_ui_files(oxygen_demo_SOURCES
    ui/oxygenbuttondemowidget.ui
    ui/oxygeninputdemowidget.ui
    ui/oxygenlistdemowidget.ui
    ui/oxygentabdemowidget.ui
)

kde4_add_executable( oxygen-demo ${oxygen_demo_SOURCES} )

target_link_libraries( oxygen-demo
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY})

########### install files ###############
install (TARGETS oxygen-demo DESTINATION ${BIN_INSTALL_DIR} )
