macro(adjust in_size in_xhot in_yhot)
   # Inkscape exports SVG pixel units using a 96 DPI baseline.
   # Keep a consistent nominal-size ladder across cursors based on a 32px baseline,
   # so desktop UIs expose 16/24/32/48/64/72 at 48/72/96/144/192/216 DPI.
   math(EXPR out_size "( 32 * ${resolution} ) / 96")
   math(EXPR out_xhot "( ${in_xhot} * ${resolution} ) / 96")
   math(EXPR out_yhot "( ${in_yhot} * ${resolution} ) / 96")
   set(out_line "${out_size} ${out_xhot} ${out_yhot} ${resolution}/${ARGN}")
   string(REPLACE ";" " " out_line "${out_line}")
   list(APPEND out_contents "${out_line}")
endmacro(adjust)

# load config file
file(READ "${config}" in_contents)
set(out_contents)
string(REPLACE "\n" ";" in_contents "${in_contents}")
# convert the coma separated list in a standard list
string(REPLACE "," ";" resolutions ${resolutions})
# adjust the config file
foreach(resolution ${resolutions})
   foreach(in_line ${in_contents})
      string(REGEX REPLACE "[ \t]+" ";" in_line "${in_line}")
      adjust(${in_line})
   endforeach(in_line)
endforeach(resolution)
# save the adjusted config file
string(REPLACE ";" "\n" out_contents "${out_contents}")
file(WRITE "${output}" "${out_contents}\n")
