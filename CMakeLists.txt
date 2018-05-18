project(oxygen)
set(PROJECT_VERSION "5.12.90")
set(PROJECT_VERSION_MAJOR 5)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

include(GenerateExportHeader)
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

  ### XCB
  find_package(PkgConfig)
  pkg_check_modules(XCB xcb x11-xcb)
  add_feature_info("x11-xcb" XCB_FOUND "Required to pass style properties to native Windows on X11 Platform")
  set_feature_info("x11-xcb" "Required to pass style properties to native Windows on X11 Platform" "http://xcb.freedesktop.org")
  set(OXYGEN_HAVE_X11 ${XCB_FOUND})
  set(OXYGEN_HAVE_KWAYLAND FALSE)

  add_subdirectory(liboxygen)
  add_subdirectory(kstyle)

else()

  find_package(ECM 0.0.9 REQUIRED NO_MODULE)
  set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

  find_package(Qt5 REQUIRED CONFIG COMPONENTS Widgets DBus Quick)
  find_package(KF5 REQUIRED COMPONENTS
      I18n
      Config
      GuiAddons
      WidgetsAddons
      Service
      Completion
      FrameworkIntegration
      WindowSystem)

  find_package(XCB COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES
      DESCRIPTION "X protocol C-language Binding"
      URL "http://xcb.freedesktop.org"
      TYPE OPTIONAL
      PURPOSE "Required to pass style properties to native Windows on X11 Platform"
  )

  find_package(KF5Wayland CONFIG)
  set(OXYGEN_HAVE_KWAYLAND ${KF5Wayland_FOUND})

  if(NOT APPLE)

    set(OXYGEN_HAVE_X11 ${XCB_XCB_FOUND})
    if (XCB_XCB_FOUND)
      find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
    endif()

  endif()

  include(ECMInstallIcons)
  include(KDEInstallDirs)
  include(KDECMakeSettings)
  include(KDECompilerSettings NO_POLICY_SCOPE)

  add_subdirectory(liboxygen)
  add_subdirectory(kstyle)
  add_subdirectory(cursors)

  add_subdirectory(sounds)
  add_subdirectory(kdecoration)

  install(DIRECTORY lookandfeel/ DESTINATION ${KDE_INSTALL_DATADIR}/plasma/look-and-feel/org.kde.oxygen
          PATTERN "Messages.sh" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE)

  # po/ dir added by packaging scripts
  if(EXISTS ${CMAKE_SOURCE_DIR}/po AND IS_DIRECTORY ${CMAKE_SOURCE_DIR}/po)
    find_package(KF5I18n CONFIG REQUIRED)
    ki18n_install(po)
  endif()

endif()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
