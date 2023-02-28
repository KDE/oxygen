cmake_minimum_required(VERSION 3.16)
project(oxygen)
set(PROJECT_VERSION "5.27.2")
set(PROJECT_VERSION_MAJOR 5)

include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(FeatureSummary)

################# Qt/KDE #################
set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.102.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets DBus Quick)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    Config
    CoreAddons
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

if(NOT APPLE)
    set(OXYGEN_HAVE_X11 ${XCB_XCB_FOUND})
    if (XCB_XCB_FOUND AND QT_MAJOR_VERSION EQUAL "5")
        find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
    endif()
endif()


add_subdirectory(liboxygen)
add_subdirectory(kstyle)
add_subdirectory(color-schemes)
add_subdirectory(cursors)
add_subdirectory(kdecoration)

install(DIRECTORY lookandfeel/ DESTINATION ${KDE_INSTALL_DATADIR}/plasma/look-and-feel/org.kde.oxygen
        PATTERN "Messages.sh" EXCLUDE PATTERN "CMakeLists.txt" EXCLUDE)

find_package(KF5I18n CONFIG REQUIRED)
ki18n_install(po)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
