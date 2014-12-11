project(oxygen)
set(PROJECT_VERSION "5.1.2")
set(PROJECT_VERSION_MAJOR 5)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

include(WriteBasicConfigVersionFile)
include(FeatureSummary)

set(OXYGEN_USE_KDE4 ${USE_KDE4})
if(OXYGEN_USE_KDE4)

  ############ Language and toolchain features
  ############ copied from ECM
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND NOT WIN32)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()

endif()

################# Qt/KDE #################
if(OXYGEN_USE_KDE4)

  find_package(KDE4 REQUIRED)
  include(KDE4Defaults)
  include(MacroLibrary)

  add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
  include_directories(${KDE4_INCLUDES})

  ### KDE workspace (needed for window decoration)
  find_package(KDE4Workspace 4.9.80)
  set_package_properties(
    KDE4Workspace PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Required to oxygen window decoration for kwin"
 )

  ### XCB
  find_package(PkgConfig)
  pkg_check_modules(XCB xcb x11-xcb)
  set(OXYGEN_HAVE_X11 ${XCB_FOUND})

  add_subdirectory(liboxygen)
  add_subdirectory(kstyle)
  if(KDE4Workspace_FOUND)
     add_subdirectory(kwin-decoration)
  endif()

else()

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
  set_package_properties(
    KDecorations PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Required to oxygen window decoration for kwin"
  )

  find_package(XCB COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES
      DESCRIPTION "X protocol C-language Binding"
      URL "http://xcb.freedesktop.org"
      TYPE OPTIONAL
      PURPOSE "Required to pass style properties to native Windows on X11 Platform"
  )

  if(NOT APPLE)

    set(OXYGEN_HAVE_X11 ${XCB_XCB_FOUND})
    if (XCB_XCB_FOUND)
      find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
    endif()

  endif()

  include(KDEInstallDirs)
  include(KDECMakeSettings)
  include(KDECompilerSettings)
  include(GenerateExportHeader)

  add_subdirectory(liboxygen)
  add_subdirectory(kstyle)
  add_subdirectory(cursors)
  if(KDecorations_FOUND)
    add_subdirectory(kwin-decoration)
  endif()

  add_subdirectory(sounds)

  install(DIRECTORY lookandfeel/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/plasma/look-and-feel/org.kde.oxygen
          PATTERN "Messages.sh" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE)

  # po/ dir added by packaging scripts
  if(EXISTS ${CMAKE_SOURCE_DIR}/po AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/po)
    find_package(KF5I18n CONFIG REQUIRED)
    ki18n_install(po)
  endif()

endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
