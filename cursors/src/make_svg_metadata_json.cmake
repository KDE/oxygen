macro(adjust _size hotspot_x hotspot_y png_filename)
   set(size "24")
   # the .in files were written expecting .png files
   string(REPLACE ".png" ".svg" filename "${png_filename}")
   
   list(APPEND out_contents "    {")
   list(APPEND out_contents "        \"filename\": \"${filename}\",")
   # duration if present
   if(ARGN)
      list(APPEND out_contents "        \"delay\": ${ARGN},")
   endif()
   list(APPEND out_contents "        \"hotspot_x\": ${hotspot_x},")
   list(APPEND out_contents "        \"hotspot_y\": ${hotspot_y},")
   list(APPEND out_contents "        \"nominal_size\": ${size}")
   list(APPEND out_contents "    },")
endmacro(adjust)

# load config file
file(READ "${config}" in_contents)
string(REPLACE "\n" ";" in_contents "${in_contents}")

set(out_contents)
# generate metadata.json
list(APPEND out_contents "[")
foreach(in_line ${in_contents})
   string(REGEX REPLACE "[ \t]+" ";" in_line "${in_line}")
   adjust(${in_line})
endforeach(in_line)
list(APPEND out_contents "]")
# fix list ending in comma now, and add trailing newline
string(REPLACE "},;]" "};];" out_contents "${out_contents}")
# save metadata.json
string(REPLACE ";" "\n" out_contents "${out_contents}")
file(WRITE "${output}" "${out_contents}")
