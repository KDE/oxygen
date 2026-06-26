macro(adjust in_size in_xhot in_yhot)
   # Keep the historical Oxygen cursor scale model (24 px nominal at 90 DPI).
   math(EXPR out_size "( 24 * ${resolution} ) / 90")
   math(EXPR out_xhot "( ${in_xhot} * ${resolution} ) / 90")
   math(EXPR out_yhot "( ${in_yhot} * ${resolution} ) / 90")
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
