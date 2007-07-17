
project(kstyle-oxygen)

add_subdirectory( config )



########### next target ###############

set(oxygen_PART_SRCS oxygen.cpp helper.cpp tileset.cpp)


kde4_add_plugin(oxygen ${oxygen_PART_SRCS})

target_link_libraries(oxygen  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} kdefx )

install(TARGETS oxygen  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/ )


########### install files ###############

install( FILES oxygen.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes )




