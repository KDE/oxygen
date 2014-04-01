project(oxygen)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

include(WriteBasicConfigVersionFile)
include(FeatureSummary)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 REQUIRED CONFIG COMPONENTS Widgets)
find_package(KF5 REQUIRED CONFIG COMPONENTS
    I18n
    Config
    GuiAddons
    WidgetsAddons
    Service
    Completion
    FrameworkIntegration
    WindowSystem
    KDE4Support)

find_package(KDecorations REQUIRED CONFIG)

find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                        URL "http://www.x.org"
                        TYPE OPTIONAL
                        PURPOSE "Required for building the X11 based workspace"
                    )
if(X11_FOUND)
  find_package(XCB REQUIRED COMPONENTS XCB XFIXES DAMAGE COMPOSITE SHAPE SYNC RENDER RANDR KEYSYMS IMAGE SHM XTEST)
  set_package_properties(XCB PROPERTIES TYPE REQUIRED)
  find_package(XCB COMPONENTS ICCCM)
  add_feature_info("XCB-ICCCM" XCB_ICCCM_FOUND "Required for building test applications for KWin")

  find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
endif()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)

add_subdirectory(liboxygen)
add_subdirectory(kstyle)
add_subdirectory(cursors)
add_subdirectory(kwin-decoration)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
