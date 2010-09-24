cmake_minimum_required(VERSION 2.6)
project(rinoohttpd)
set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

list(APPEND common_flags "-W")
list(APPEND common_flags "-Wall")
list(APPEND common_flags "-g3")
list(APPEND common_flags "-DRINOO_DEBUG")
list(APPEND common_flags "-DVERSION_MAJOR=${VERSION_MAJOR}")
list(APPEND common_flags "-DVERSION_MINOR=${VERSION_MINOR}")
list(APPEND common_flags "-DVERSION_PATCH=${VERSION_PATCH}")
list(APPEND common_flags "-DVERSION=${VERSION}")

## Ragel - compile Ragel files ##
find_program(cmd_ragel ragel)
if (NOT EXISTS ${cmd_ragel})
  message(FATAL_ERROR "Ragel is not installed!")
endif (NOT EXISTS ${cmd_ragel})
file(GLOB_RECURSE ragel_files "*.rl")
foreach (loop_var ${ragel_files})
  message("Ragel file found: ${loop_var}")
  execute_process(COMMAND ${cmd_ragel} "${loop_var}" OUTPUT_VARIABLE ragel_out ERROR_VARIABLE ragel_err)
  if (NOT ${ragel_err} STREQUAL "")
    message(FATAL_ERROR "${ragel_err}")
  endif (NOT ${ragel_err} STREQUAL "")
endforeach(loop_var)
## !Ragel ##

add_definitions(${common_flags})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

## HTTPD ##
file(GLOB_RECURSE src_files "src/*.c")
add_executable(${CMAKE_PROJECT_NAME} ${src_files})
target_link_libraries(${CMAKE_PROJECT_NAME} rinoo)
## !Library ##
