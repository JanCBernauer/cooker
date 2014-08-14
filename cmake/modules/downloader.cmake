function(downloadfile_script script_filename remote local)
  file(WRITE ${script_filename}
"
IF (EXISTS \"${local}\")
message(STATUS \"Skipping download of ${local}\")
else()
message(STATUS \"downloading...
     src='${remote}'
     dst='${local}'
     timeout='${timeout_msg}'\")

file(DOWNLOAD
  \"${remote}\"
  \"${local}\"
  SHOW_PROGRESS
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR \"error: downloading '${remote}' failed
  status_code: \${status_code}
  status_string: \${status_string}
  log: \${log}
\")
endif()

message(STATUS \"downloading... done\")
endif()
"
)

endfunction(downloadfile_script)

function (download name remote local)
downloadfile_script("${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/download-${name}.cmake" ${remote} ${local})
add_custom_target(${name} ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/download-${name}.cmake)
endfunction(download)
