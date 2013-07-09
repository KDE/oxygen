project(kstyle-oxygen)

# add_subdirectory( config )
add_subdirectory( demo )

################# X11 #################
find_package(X11)
if(X11_FOUND)
  find_package(Qt5X11Extras REQUIRED NO_MODULE)
  include_directories( ${X11_X11_INCLUDE_PATH} )
  set (HAVE_X11 1)
else(X11_FOUND)
   set (HAVE_X11 0)
endif(X11_FOUND)

add_definitions( -DHAVE_X11=${HAVE_X11} )

################# includes #################
include_directories (animations debug transitions /home/hpereira/kf5/src/kde-workspace/libs/oxygen)

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

qt5_use_modules( oxygen
    Core
    Gui
    Widgets
    DBus
    Svg )

target_link_libraries(oxygen
    ${KConfig_LIBRARIES}
    ${kguiaddons_LIBRARIES}
    ${kwidgets_LIBRARIES}
    ${KIconThemes_LIBRARIES}
    ${KWindowSystem_LIBRARIES}
)

target_link_libraries(oxygen oxygenstyle)

if(X11_FOUND)
  target_link_libraries(oxygen ${X11_LIBRARIES})
  qt5_use_modules(oxygen X11Extras)
endif(X11_FOUND)

install(TARGETS oxygen DESTINATION ${QT_PLUGIN_INSTALL_DIR}/styles/ )

########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )
