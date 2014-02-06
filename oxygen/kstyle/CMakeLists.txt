project(kstyle-oxygen)

################# X11 #################
add_definitions( -DHAVE_X11=${X11_FOUND} )

if(X11_FOUND)
    find_package(XCB REQUIRED)
endif()

################# includes #################
include_directories(
    animations
    debug
    transitions
)

########### next target ###############
set(oxygen_PART_SRCS
    animations/oxygenanimations.cpp
    animations/oxygenanimationdata.cpp
    animations/oxygenbaseengine.cpp
    animations/oxygenbusyindicatorengine.cpp
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
    oxygenblurhelper.cpp
    oxygenframeshadow.cpp
    oxygenmdiwindowshadow.cpp
    oxygenmnemonics.cpp
    oxygenpropertynames.cpp
    oxygenshadowhelper.cpp
    oxygensplitterproxy.cpp
    oxygenstyle.cpp
    oxygenstylehelper.cpp
    oxygenwindowmanager.cpp
)

kconfig_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

add_library(oxygen MODULE ${oxygen_PART_SRCS})

target_link_libraries(oxygen Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
target_link_libraries(oxygen KF5::Style KF5::WindowSystem)
target_link_libraries(oxygen oxygenstyle)

if(X11_FOUND)
  target_link_libraries(oxygen ${XCB_XCB_LIBRARIES} )
  target_link_libraries(oxygen Qt5::X11Extras)
endif()

install(TARGETS oxygen DESTINATION ${QT_PLUGIN_INSTALL_DIR}/styles/ )

########### install files ###############
install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )

########### subdirectories ###############
add_subdirectory( config )
add_subdirectory( demo )
