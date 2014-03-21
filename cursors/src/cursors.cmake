# This macro produces a "themed" version of the SVG, using the spcified colors.
# Than, it renders this themed SVG to PNG files.
# You can specify one or more dpi values when you call this macro.
macro(add_cursor cursor color theme dpi)
    # Produce a "themed" SVG
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/oxy-${theme}/svg/${cursor}.svg
                       DEPENDS ${MAKE_SVG} ${CMAKE_CURRENT_SOURCE_DIR}/colors.in ${SVGDIR}/${cursor}.svg
                       COMMAND ${CMAKE_COMMAND} -Dconfig=${CMAKE_CURRENT_SOURCE_DIR}/colors.in
                                                -Dinput=${SVGDIR}/${cursor}.svg
                                                -Doutput=${CMAKE_BINARY_DIR}/oxy-${theme}/svg/${cursor}.svg
                                                -P ${MAKE_SVG}
                      )
    # Prepare a list of resolutions in dpi
    set(resolutions ${ARGV})
    list(REMOVE_AT resolutions 0)
    list(REMOVE_AT resolutions 0)
    list(REMOVE_AT resolutions 0)
    # Render the SVG
    foreach(resolution ${resolutions})
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/oxy-${theme}/png/${resolution}/${cursor}.png
                           DEPENDS ${CMAKE_BINARY_DIR}/oxy-${theme}/svg/${cursor}.svg
                           COMMAND ${INKSCAPE} --without-gui --export-dpi=${resolution}
                                               --export-png=${CMAKE_BINARY_DIR}/oxy-${theme}/png/${resolution}/${cursor}.png
                                               ${CMAKE_BINARY_DIR}/oxy-${theme}/svg/${cursor}.svg
                          )
    endforeach(resolution)
endmacro(add_cursor)

macro(add_x_cursor theme cursor dpi)
    # Prepare a list of resolutions in dpi
    set(resolutions ${ARGV})
    list(REMOVE_AT resolutions 0)
    list(REMOVE_AT resolutions 0)
    # Prepare a list of the png files that are necessary
    set(inputs)
    foreach(png ${${cursor}_inputs})
        foreach(resolution ${resolutions})
            list(APPEND inputs ${CMAKE_BINARY_DIR}/oxy-${theme}/png/${resolution}/${png})
        endforeach(resolution)
    endforeach(png)
    # Make a coma-separated list (normal lists can't be passed as argument)
    string(REPLACE ";" "," resolutions_coma_separated "${resolutions}")
    # Adopt the x cursor config file to the requested resolutions
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/oxy-${theme}/config/${cursor}.in
                       DEPENDS ${MAKE_CONFIG} ${CONFIGDIR}/${cursor}.in
                       COMMAND ${CMAKE_COMMAND} -Dconfig=${CONFIGDIR}/${cursor}.in
                                                -Doutput=${CMAKE_BINARY_DIR}/oxy-${theme}/config/${cursor}.in
                                                -Dresolutions=${resolutions_coma_separated}
                                                -P ${MAKE_CONFIG}
                      )
    # Use the adopted x cursor config file and the png files to produce the cursor file
    if(NOT WIN32)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors/${cursor}
                       DEPENDS ${inputs} ${CMAKE_BINARY_DIR}/oxy-${theme}/config/${cursor}.in
                       COMMAND ${XCURSORGEN} -p ${CMAKE_BINARY_DIR}/oxy-${theme}/png
                                             ${CMAKE_BINARY_DIR}/oxy-${theme}/config/${cursor}.in
                                             ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors/${cursor}
                      )
    else(NOT WIN32)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors/${cursor}
                       DEPENDS ${inputs} ${CMAKE_BINARY_DIR}/oxy-${theme}/config/${cursor}.in
                       COMMAND "${CMAKE_SOURCE_DIR}/wincursor.py" ${CMAKE_BINARY_DIR}/oxy-${theme}/png
                                             ${CMAKE_BINARY_DIR}/oxy-${theme}/config/${cursor}.in
                                             ${CMAKE_BINARY_DIR}/wincursors/oxy-${theme}
                                             ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors/${cursor}
                      )
    endif(NOT WIN32)
endmacro(add_x_cursor)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)

# Macro that adds a theme. You can specify more than one dpi value (overloaded macro).
macro(add_theme color theme dpi)
    # Prepare a list of resolutions in dpi
    set(resolutions ${ARGV})
    list(REMOVE_AT resolutions 0)
    list(REMOVE_AT resolutions 0)
    # Make missing directories
    foreach (resolution ${resolutions})
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/oxy-${theme}/png/${resolution})
    endforeach(resolution)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/oxy-${theme}/svg)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/oxy-${theme}/config)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors)
    if(WIN32)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/wincursors/oxy-${theme})
    endif(WIN32)
    set(${theme}_cursors)
    # render SVG to PNG files
    foreach(svg ${SVGS})
        string(REGEX REPLACE ".*/" "" cursor ${svg})           # use relative paths
        string(REGEX REPLACE "[.]svg" "" cursor ${cursor})     # remove ".svg" from the path
        add_cursor(${cursor} ${color} ${theme} ${resolutions}) # Render a "themed" version of the SVG to PNG files.
    endforeach(svg)
    # produce cursor files from the png files
    foreach(cursor ${CURSORS})
        add_x_cursor(${theme} ${cursor} ${resolutions})
        list(APPEND ${theme}_cursors ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors/${cursor})
    endforeach(cursor)
    # add the symbolic links
    foreach(symlink ${SYMLINKS_PATHS})
        file(COPY ${symlink} DESTINATION ${CMAKE_BINARY_DIR}/oxy-${theme}/cursors)
    endforeach(symlink)
    # packaging
    add_custom_target(theme-${theme} ALL DEPENDS ${${theme}_cursors})
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/oxy-${theme}/index.theme
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/index.theme
                       COMMAND ${CMAKE_COMMAND} -E copy
                                                   ${CMAKE_CURRENT_SOURCE_DIR}/index.theme
                                                   ${CMAKE_BINARY_DIR}/oxy-${theme}/index.theme
                      )
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/packages/oxy-${theme}.tar.bz2
                       DEPENDS ${${theme}_cursors} ${CMAKE_BINARY_DIR}/oxy-${theme}/index.theme
                       COMMAND ${TAR} cjf ${CMAKE_BINARY_DIR}/packages/oxy-${theme}.tar.bz2
                                      oxy-${theme}/cursors
                                      oxy-${theme}/index.theme
                       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                      )
    add_custom_target(package-${theme} ALL DEPENDS ${CMAKE_BINARY_DIR}/packages/oxy-${theme}.tar.bz2)
endmacro(add_theme)
