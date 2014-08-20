project(oxygen)
set(PROJECT_VERSION "5.0.95")
set(PROJECT_VERSION_MAJOR 5)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

include(WriteBasicConfigVersionFile)
include(FeatureSummary)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 REQUIRED CONFIG COMPONENTS Widgets)
find_package(KF5 REQUIRED COMPONENTS
    I18n
    Config
    GuiAddons
    WidgetsAddons
    Service
    Completion
    FrameworkIntegration
    WindowSystem)

find_package(KDecorations CONFIG)
set_package_properties(KDecorations PROPERTIES
                       TYPE OPTIONAL
                       PURPOSE "Required to oxygen window decoration for kwin"
                      )

find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                        URL "http://www.x.org"
                        TYPE OPTIONAL
                        PURPOSE "Required for building the X11 based workspace"
                    )
if(X11_FOUND)
  find_package(XCB REQUIRED COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES TYPE REQUIRED)

  find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
else()
    set(X11_FOUND 0)
endif()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)

add_subdirectory(liboxygen)
add_subdirectory(kstyle)
add_subdirectory(cursors)
if( KDecorations_FOUND )
  add_subdirectory(kwin-decoration)
endif()

add_subdirectory(sounds)

install(DIRECTORY lookandfeel/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/plasma/look-and-feel/org.kde.oxygen)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
