macro(adjust in_size in_xhot in_yhot)
   #math(EXPR out_size "( ${in_size} * ${resolution} ) / 90")
   # The Oxygen cursors have different default sizes at 90 dpi:
   # Some have 24x24 px, others have 32x32 px. This causes some
   # side-effects when you want to choose your cursor size.
   # Instead of the real in_size, we use 24 as convenience value.
   # Xcursorlib interpretats this as a _nominal_ size; it is legal
   # that the nominal size is different from the real size of the
   # png image.
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
