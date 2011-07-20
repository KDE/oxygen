include_directories( ${KDE4_KDEUI_INCLUDES} ${KDEBASE_WORKSPACE_SOURCE_DIR}/libs/oxygen)

########### next target ###############
set(kstyle_oxygen_config_PART_SRCS
    oxygenanimationconfigwidget.cpp
    oxygenfollowmouseanimationconfigitem.cpp
    oxygenstyleconfig.cpp
)

kde4_add_kcfg_files(kstyle_oxygen_config_PART_SRCS ../oxygenstyleconfigdata.kcfgc)

kde4_add_ui_files(kstyle_oxygen_config_PART_SRCS
    ui/oxygenfollowmouseanimationconfigbox.ui
    ui/oxygenstyleconfig.ui
)

kde4_add_plugin(kstyle_oxygen_config ${kstyle_oxygen_config_PART_SRCS})

target_link_libraries(
    kstyle_oxygen_config
    ${KDE4_KDEUI_LIBS}
    oxygenstyleconfig )

install(TARGETS kstyle_oxygen_config  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### next target ###############
set( oxygen_settings_SOURCES
    oxygenconfigdialog.cpp
    main.cpp)

kde4_add_executable( oxygen-settings ${oxygen_settings_SOURCES} )

target_link_libraries( oxygen-settings
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY})

########### install files ###############
install (TARGETS oxygen-settings ${INSTALL_TARGETS_DEFAULT_ARGS} )





