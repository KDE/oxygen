
project(kstyle-oxygen)

find_package (KDE4 REQUIRED)
add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)
include (KDE4Defaults)
include (MacroLibrary)
include_directories (${KDE4_INCLUDES} ${CMAKE_BINARY_DIR})
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

add_subdirectory( config )



########### next target ###############

set(oxygen_PART_SRCS
    lib/tileset.cpp
    lib/helper.cpp helper.cpp
    oxygen.cpp)

kde4_add_kcfg_files(oxygen_PART_SRCS oxygenstyleconfigdata.kcfgc)

kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )




