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
    animations/oxygenbusyindicatordata.cpp
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
    oxygenstyleplugin.cpp
    oxygenwindowmanager.cpp
)

kconfig_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)
add_library(oxygen${QT_MAJOR_VERSION} MODULE ${oxygen_PART_SRCS})

target_link_libraries(oxygen${QT_MAJOR_VERSION} Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Gui Qt${QT_MAJOR_VERSION}::Widgets Qt${QT_MAJOR_VERSION}::DBus Qt${QT_MAJOR_VERSION}::Quick)
target_link_libraries(oxygen${QT_MAJOR_VERSION} KF${QT_MAJOR_VERSION}::GuiAddons KF${QT_MAJOR_VERSION}::Style KF${QT_MAJOR_VERSION}::WindowSystem)
target_link_libraries(oxygen${QT_MAJOR_VERSION} oxygenstyle${QT_MAJOR_VERSION})

if(OXYGEN_HAVE_X11)
    target_link_libraries(oxygen${QT_MAJOR_VERSION} XCB::XCB)
    if (QT_MAJOR_VERSION EQUAL "5")
        target_link_libraries(oxygen5 Qt5::X11Extras)
    else()
        target_link_libraries(oxygen6 Qt6::GuiPrivate)
    endif()
endif()

install(TARGETS oxygen${QT_MAJOR_VERSION} DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/styles/)

if(WIN32)
  # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
  # when using MSVC if _USE_MATH_DEFINES is defined (this is needed for angle.cpp)
  target_compile_definitions(oxygen${QT_MAJOR_VERSION} PRIVATE _USE_MATH_DEFINES _GNU_SOURCE)
endif()

########### install files ###############
install(FILES oxygen.themerc  DESTINATION  ${KDE_INSTALL_DATADIR}/kstyle/themes)

########### subdirectories ###############
if(QT_MAJOR_VERSION EQUAL 6)
    add_subdirectory(config)
endif()
add_subdirectory(demo)
