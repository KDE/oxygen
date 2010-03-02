
project(kstyle-oxygen)

add_subdirectory( config )

include_directories (animations transitions ${KDEBASE_WORKSPACE_SOURCE_DIR}/libs/oxygen)

########### next target ###############

set(oxygen_PART_SRCS
    helper.cpp
    animations/oxygenanimations.cpp
    animations/oxygenanimationdata.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygendockseparatordata.cpp
    animations/oxygendockseparatorengine.cpp
    animations/oxygenenabledata.cpp
    animations/oxygengenericdata.cpp
    animations/oxygenhoverdata.cpp
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
    animations/oxygentabbardata.cpp
    animations/oxygentabbarengine.cpp
    animations/oxygenwidgetstatedata.cpp
    animations/oxygenwidgetstateengine.cpp
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
    oxygen.cpp
)

kde4_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} oxygenstyle)

install(TARGETS oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )




