project(oxygen)
set(PROJECT_VERSION "5.0.95")
set(PROJECT_VERSION_MAJOR 5)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

include(WriteBasicConfigVersionFile)
include(FeatureSummary)

if(USE_KDE4)

  ############ Language and toolchain features
  ############ copied from ECM
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND NOT WIN32)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()

  add_definitions( -DUSE_KDE4=1 )

else()

  add_definitions( -DUSE_KDE4=0 )

endif()


################# X11 #################
find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
    URL "http://www.x.org"
    TYPE OPTIONAL
    PURPOSE "Required for building the X11 based workspace"
)

add_definitions( -DHAVE_X11=${X11_FOUND} )

################# Qt/KDE #################
if(USE_KDE4)

  find_package(KDE4 REQUIRED)
  include(KDE4Defaults)
  include(MacroLibrary)

  add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
  include_directories(${KDE4_INCLUDES})

  find_package(KDE4Workspace 4.9.80)
  set_package_properties(
    KDE4Workspace PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Required to oxygen window decoration for kwin"
  )

  if(X11_FOUND)
    ### XCB
    find_package(PkgConfig REQUIRED)
    pkg_check_modules( XCB REQUIRED xcb x11-xcb)
  endif()

  add_subdirectory(liboxygen)
  add_subdirectory(kstyle)
  if( KDE4Workspace_FOUND )
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

  if(X11_FOUND)
    ### XCB
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

endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
