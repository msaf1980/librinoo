cmake_minimum_required(VERSION 2.6)
project(rinoo)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "5")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

#include(CMakeForceCompiler)
#cmake_force_c_compiler(clang 0)

list(APPEND common_flags "-W")
list(APPEND common_flags "-Wall")
list(APPEND common_flags "-g3")
list(APPEND common_flags "-DRINOO_DEBUG")
list(APPEND common_flags "-DVERSION_MAJOR=${VERSION_MAJOR}")
list(APPEND common_flags "-DVERSION_MINOR=${VERSION_MINOR}")
list(APPEND common_flags "-DVERSION_PATCH=${VERSION_PATCH}")
list(APPEND common_flags "-DVERSION=${VERSION}")

## Dependencies checks ##
include(CheckFunctionExists)
macro(check_poller function macro)
  check_function_exists(${function} TMP_MACRO)
  if (${TMP_MACRO} EQUAL 1)
    list(APPEND common_flags "-D${macro}")
  endif (${TMP_MACRO} EQUAL 1)
endmacro(check_poller)

check_poller(epoll_create HAS_EPOLL)
check_poller(poll HAS_POLL)
check_poller(select HAS_SELECT)
## !Dependencies ##

add_definitions(${common_flags})
include_directories(include)

## Library ##
set(test_files "")
file(GLOB_RECURSE tmp_src_files "src/*.c")
 ## Split source files and test files
foreach (loop_var ${tmp_src_files})
  if (NOT loop_var MATCHES "test/.*\\.c")
    list(APPEND src_files "${loop_var}")
  else (NOT loop_var MATCHES "test/.*\\.c")
    if (NOT loop_var MATCHES "test/utest.*\\.c")
      list(APPEND test_files "${loop_var}")
    endif (NOT loop_var MATCHES "test/utest.*\\.c")
  endif (NOT loop_var MATCHES "test/.*\\.c")
endforeach (loop_var)

add_library(${CMAKE_PROJECT_NAME}_static STATIC ${src_files})
add_library(${CMAKE_PROJECT_NAME} SHARED ${src_files})

target_link_libraries("${CMAKE_PROJECT_NAME}")
## !Library ##


## Build unit tests ##
enable_testing()

list(SORT test_files)
add_test("clean_traces" ${CMAKE_COMMAND} "-E" "remove_directory" "${CMAKE_HOME_DIRECTORY}/traces")
foreach (loop_var ${test_files})
  string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/(.*/)([^/]*).c$" "\\1utest_\\2" bin_var "${loop_var}")
  string(REGEX REPLACE ".*/" "" test_name "${bin_var}")
  message("Test found: ${bin_var}")
  add_executable("${bin_var}" ${loop_var})
  target_link_libraries("${bin_var}" rinoo)
  add_test("${test_name}" "${bin_var}")
  add_test("${test_name}_valgrind" "${CMAKE_HOME_DIRECTORY}/valgrind" "${bin_var}")
endforeach (loop_var)
file(GLOB_RECURSE emacs_files "*~" "*\#*\#")
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${emacs_files}")

## Documentation ##
find_program(doxygen_bin doxygen)
if (NOT EXISTS ${doxygen_bin})
  message(FATAL_ERROR "Doxygen is not installed!")
endif (NOT EXISTS ${doxygen_bin})

add_custom_target(
  doc
  COMMAND ${doxygen_bin} "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
)

## Packaging ##
execute_process(COMMAND "uname" "-m" OUTPUT_VARIABLE MACHINE)
string(REPLACE "\n" "" MACHINE "${MACHINE}")
set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${MACHINE})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "rinoo library")
set(CPACK_PACKAGE_VENDOR "NoX")
set(CPACK_PACKAGE_CONTACT "nox@puffy.fr")
set(CPACK_PACKAGE_NAME lib${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION}")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/lib/")
set(CPACK_GENERATOR "DEB")

set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;\\\\.#.*;#.*#;.*~;CMakeFiles;_CPack_Packages;.*\\\\.cmake;install_manifest.txt;CMakeCache.txt;${CMAKE_PACKAGE_NAME}.*\\\\.(tar\\\\.gz|tar\\\\.bz2|deb);Makefile;")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;DEB")

include(CPack)

install(TARGETS
  ${CMAKE_PROJECT_NAME}
  ${CMAKE_PROJECT_NAME}_static
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/rinoo" DESTINATION include)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/backtrace.gdb" DESTINATION include/rinoo)
## !Packaging ##