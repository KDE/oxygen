
project(kstyle-oxygen)

add_subdirectory( config )
add_subdirectory( demo )

include_directories (animations debug transitions ${KDEBASE_WORKSPACE_SOURCE_DIR}/libs/oxygen)

########### next target ###############

set(oxygen_PART_SRCS
    animations/oxygenanimations.cpp
    animations/oxygenanimationdata.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygendockseparatordata.cpp
    animations/oxygendockseparatorengine.cpp
    animations/oxygenenabledata.cpp
    animations/oxygengenericdata.cpp
    animations/oxygenheaderviewdata.cpp
    animations/oxygenheaderviewengine.cpp
    animations/oxygenmdiwindowdata.cpp
    animations/oxygenmdiwindowengine.cpp
    animations/oxygenmenubardata.cpp
    animations/oxygenmenubarengine.cpp
    animations/oxygenmenudata.cpp
    animations/oxygenmenuengine.cpp
    animations/oxygenprogressbardata.cpp
    animations/oxygenprogressbarengine.cpp
    animations/oxygenscrollbardata.cpp
    animations/oxygenscrollbarengine.cpp
    animations/oxygensliderdata.cpp
    animations/oxygensliderengine.cpp
    animations/oxygenspinboxdata.cpp
    animations/oxygenspinboxengine.cpp
    animations/oxygensplitterengine.cpp
    animations/oxygentabbardata.cpp
    animations/oxygentabbarengine.cpp
    animations/oxygentoolbardata.cpp
    animations/oxygentoolbarengine.cpp
    animations/oxygentoolboxengine.cpp
    animations/oxygenwidgetstatedata.cpp
    animations/oxygenwidgetstateengine.cpp
    debug/oxygenwidgetexplorer.cpp
    transitions/oxygencomboboxdata.cpp
    transitions/oxygencomboboxengine.cpp
    transitions/oxygenlabeldata.cpp
    transitions/oxygenlabelengine.cpp
    transitions/oxygenlineeditdata.cpp
    transitions/oxygenlineeditengine.cpp
    transitions/oxygenstackedwidgetdata.cpp
    transitions/oxygenstackedwidgetengine.cpp
    transitions/oxygentransitiondata.cpp
    transitions/oxygentransitions.cpp
    transitions/oxygentransitionwidget.cpp
    oxygenframeshadow.cpp
    oxygenmdiwindowshadow.cpp
    oxygenpropertynames.cpp
    oxygenshadowhelper.cpp
    oxygensplitterproxy.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenwindowmanager.cpp
)

kde4_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} oxygenstyle)

if ( X11_FOUND )
   target_link_libraries(oxygen ${X11_LIBRARIES})
endif( X11_FOUND )

install(TARGETS oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )

########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )
