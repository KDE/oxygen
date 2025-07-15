cmake_minimum_required(VERSION 3.25)
project(oxygen)
set(PROJECT_VERSION "6.4.3")
set(PROJECT_VERSION_MAJOR 6)

include(GenerateExportHeader)
include(WriteBasicConfigVersionFile)
include(FeatureSummary)

################# Qt/KDE #################
set(PROJECT_DEP_VERSION "6.4.3")
set(QT5_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.102.0")
set(QT_MIN_VERSION "6.8.0")
set(KF6_MIN_VERSION "6.14.0")

set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

set(QT_NO_CREATE_VERSIONLESS_TARGETS ON)
set(QT_NO_CREATE_VERSIONLESS_FUNCTIONS ON)

option(BUILD_QT5 "Build Qt5 style" ON)
option(BUILD_QT6 "Build with Qt6" ON)

# For KDE CI only. As the current infrastructure doesn't allow us to set CMAKE options per build variant
if($ENV{CI_JOB_NAME_SLUG} MATCHES "qt5")
    set(BUILD_QT5 ON)
    set(BUILD_QT6 OFF)
elseif($ENV{CI_JOB_NAME_SLUG} MATCHES "qt6")
    set(BUILD_QT5 OFF)
    set(BUILD_QT6 ON)
endif()

include(ECMInstallIcons)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
    DESCRIPTION "X protocol C-language Binding"
    URL "http://xcb.freedesktop.org"
    TYPE OPTIONAL
    PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

if(NOT APPLE)
    set(OXYGEN_HAVE_X11 ${XCB_XCB_FOUND})
endif()

if(BUILD_QT5)
block(SCOPE_FOR VARIABLES)
    set(QT_MAJOR_VERSION 5)
    unset(QUERY_EXECUTABLE CACHE)

    include(KDEInstallDirs5)
    include (KDECMakeSettings)

    find_package(Qt5 ${QT5_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets DBus Quick)
    find_package(KF5 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    Config
    CoreAddons
    GuiAddons
    WidgetsAddons
    Service
    Completion
    FrameworkIntegration
    WindowSystem)

    if(OXYGEN_HAVE_X11)
        find_package(Qt5X11Extras ${QT5_MIN_VERSION} REQUIRED CONFIG)
    endif()


    add_subdirectory(liboxygen liboxygen5)
    add_subdirectory(kstyle kstyle5)
endblock()
endif()

if(BUILD_QT6)
block(SCOPE_FOR VARIABLES)
    set(QT_MAJOR_VERSION 6)
    unset(QUERY_EXECUTABLE CACHE)

    include(KDEInstallDirs6)
    include (KDECMakeSettings)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS Widgets DBus Quick)

    if (Qt6Gui_VERSION VERSION_GREATER_EQUAL "6.10.0")
        find_package(Qt6GuiPrivate ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)
    endif()

    find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    Config
    CoreAddons
    GuiAddons
    KCMUtils
    WidgetsAddons
    Service
    Completion
    FrameworkIntegration
    WindowSystem)
    find_package(Plasma ${PROJECT_DEP_VERSION} REQUIRED)

    # https://bugreports.qt.io/browse/QTBUG-114706
    add_library(Qt::Core ALIAS Qt6::Core)
    add_library(Qt::Gui ALIAS Qt6::Gui)
    add_library(Qt::OpenGL ALIAS Qt6::OpenGL)
    add_library(Qt::Network ALIAS Qt6::Network)
    if(Qt6_VERSION_MINOR GREATER 6)
        add_library(Qt::PlatformModuleInternal ALIAS Qt6::PlatformModuleInternal)
    endif()

    add_subdirectory(liboxygen liboxygen6)
    add_subdirectory(kstyle kstyle6)

    add_subdirectory(color-schemes)
    add_subdirectory(cursors)
    add_subdirectory(desktoptheme)
    add_subdirectory(kdecoration)

    plasma_install_package(lookandfeel org.kde.oxygen look-and-feel lookandfeel)

    ki18n_install(po)
endblock()
endif()

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
