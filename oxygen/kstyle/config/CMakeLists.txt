include_directories( ${KDE4_KDEUI_INCLUDES} )

########### next target ###############

set(kstyle_oxygen_config_PART_SRCS oxygenconf.cpp )

kde4_add_kcfg_files(kstyle_oxygen_config_PART_SRCS ../oxygenstyleconfigdata.kcfgc)

kde4_add_ui_files(kstyle_oxygen_config_PART_SRCS ui/oxygenconf.ui)

kde4_add_plugin(kstyle_oxygen_config ${kstyle_oxygen_config_PART_SRCS})

target_link_libraries(kstyle_oxygen_config ${KDE4_KDEUI_LIBS})

install(TARGETS kstyle_oxygen_config  DESTINATION ${PLUGIN_INSTALL_DIR} )

########### next target ###############
set( oxygen_settings_SOURCES
    oxygenanimationconfigitem.cpp
    oxygenanimationconfigwidget.cpp
    oxygenappearanceconfigwidget.cpp
    oxygenconfigwidget.cpp
    oxygenconfigdialog.cpp
    oxygendecorationconfigwidget.cpp
    oxygenshadowconfigdata.cpp
    main.cpp)

kde4_add_ui_files(oxygen_settings_SOURCES
    ui/oxygenanimationconfigitem.ui
    ui/oxygenanimationconfigwidget.ui
    ui/oxygenappearanceconfigwidget.ui
    ui/oxygendecorationconfigwidget.ui
    ui/oxygenfollowmouseanimationconfigbox.ui
    ui/oxygengenericanimationconfigbox.ui
    ui/oxygenshadowconfigbox.ui
)

kde4_add_kcfg_files(oxygen_settings_SOURCES
    ../oxygenstyleconfigdata.kcfgc
)

kde4_add_executable( oxygen-settings ${oxygen_settings_SOURCES} )

target_link_libraries( oxygen-settings
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY})

########### install files ###############
install (TARGETS oxygen-settings DESTINATION ${BIN_INSTALL_DIR} )





