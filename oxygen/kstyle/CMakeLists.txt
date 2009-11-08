
project(kstyle-oxygen)

add_subdirectory( config )

include_directories (animations transitions)


########### next target ###############

set(oxygen_PART_SRCS
    lib/tileset.cpp
    lib/helper.cpp
    helper.cpp
    animations/oxygenanimations.cpp
    animations/oxygenanimation.cpp
    animations/oxygengenericdata.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygenenabledata.cpp
    animations/oxygenfocusdata.cpp
    animations/oxygengenericengine.cpp
    animations/oxygenhoverdata.cpp
    animations/oxygenmenubardata.cpp
    animations/oxygenmenubarengine.cpp
    animations/oxygenmenudata.cpp
    animations/oxygenmenuengine.cpp
    animations/oxygenscrollbardata.cpp
    animations/oxygenscrollbarengine.cpp
    animations/oxygensliderdata.cpp
    animations/oxygensliderengine.cpp
    animations/oxygentabbardata.cpp
    animations/oxygentabbarengine.cpp
    animations/oxygenwidgetdata.cpp
    transitions/oxygencomboboxdata.cpp
    transitions/oxygencomboboxengine.cpp
    transitions/oxygenlabeldata.cpp
    transitions/oxygenlabelengine.cpp
    transitions/oxygenstackedwidgetdata.cpp
    transitions/oxygenstackedwidgetengine.cpp
    transitions/oxygentransitiondata.cpp
    transitions/oxygentransitions.cpp
    transitions/oxygentransitionwidget.cpp
    oxygen.cpp
)

kde4_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )




