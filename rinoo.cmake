include(CMakeForceCompiler)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckSymbolExists)

enable_language(ASM)
set(CMAKE_ASM_CREATE_SHARED_LIBRARY ${CMAKE_C_CREATE_SHARED_LIBRARY})

function(append_flag FLAGS_VAR FLAG_VAR CHECK_VAR)
  string(FIND FLAGS_VAR "${CHECK_VAR}" res)
  if(res EQUAL -1)
    set(${FLAGS_VAR} "${${FLAGS_VAR}} ${FLAG_VAR}" PARENT_SCOPE)
  endif()
endfunction()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wextra")

string(TOLOWER "${CMAKE_BUILD_TYPE}" MODE)
if (MODE STREQUAL "debug")
  string(TOUPPER "${CMAKE_PROJECT_NAME}" PNAME)
  append_flag(CMAKE_C_FLAGS "-g" "-g")
  append_flag(CMAKE_C_FLAGS "-O0" "-O")
  add_definitions("-DDEBUG")
  add_definitions("-D${PNAME}_DEBUG")
  message("Build mode: debug")
elseif (MODE STREQUAL "release")
  if (DEBUGINFO)
    append_flag(CMAKE_C_FLAGS "-g" "-g")
  endif()
  append_flag(CMAKE_C_FLAGS "-O3" "-O")
  message("Build mode: production")
elseif (DEBUGINFO)
  append_flag(CMAKE_C_FLAGS "-g" "-g")
endif ()

## Version ##

macro(set_version major minor patch)
  set(VERSION_MAJOR ${major})
  set(VERSION_MINOR ${minor})
  set(VERSION_PATCH ${patch})
  set(VERSION "${major}.${minor}.${patch}")
  add_definitions("-DVERSION_MAJOR=\"${major}\"")
  add_definitions("-DVERSION_MINOR=\"${minor}\"")
  add_definitions("-DVERSION_PATCH=\"${patch}\"")
  add_definitions("-DVERSION=\"${VERSION}\"")
  set(CPACK_PACKAGE_VERSION_MAJOR ${major})
  set(CPACK_PACKAGE_VERSION_MINOR ${minor})
  set(CPACK_PACKAGE_VERSION_PATCH ${patch})
  set(CPACK_DEBIAN_PACKAGE_VERSION ${version})
endmacro(set_version)

## !Version ##

## Dependencies ##

macro(check_dependency_func function)
  check_function_exists(${function} has_${function})
  if (NOT has_${function})
    message(FATAL_ERROR "Function ${function} not found!")
  endif (NOT has_${function})
endmacro(check_dependency_func)

macro(check_dependency_lib library function)
  check_library_exists(${library} ${function} "" has_${library}_${function})
  if (NOT has_${library}_${function})
    message(FATAL_ERROR "Library ${library} not found!")
  endif (NOT has_${library}_${function})
endmacro(check_dependency_lib)

macro(check_dependency_sym symbol include)
    check_symbol_exists(${symbol} ${include} has_${symbol}_${include})
  if (NOT has_${symbol}_${include})
    message(FATAL_ERROR "Symbol ${symbol} not found!")
  endif (NOT has_${symbol}_${include})
endmacro(check_dependency_sym)

## !Dependencies ##

## Source files ##

macro(list_source_files var path)
  set(test_files "")
  file(GLOB_RECURSE tmp_src_files "${path}")
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

  ## Build unit tests ##
  if (BUILD_TEST)
    enable_testing()
    list(SORT test_files)
    add_test("clean_traces" ${CMAKE_COMMAND} "-E" "remove_directory" "${CMAKE_HOME_DIRECTORY}/traces")
    foreach (loop_var ${test_files})
      if (NOT BUILD_TEST_IPV6)
        string(FIND "${loop_var}" "_ipv6" res)
        if(NOT res EQUAL -1)
          continue()
        endif ()
      endif ()
      string(REGEX REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/(.*/)([^/]*).c$" "\\1utest_\\2" bin_var "${loop_var}")
      string(REGEX REPLACE ".*/" "" test_name "${bin_var}")
      message("Test found: ${bin_var}")
      get_filename_component(parent_dir ${bin_var} DIRECTORY)
      file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${parent_dir})
      add_executable(${test_name} ${loop_var})
      set_target_properties(${test_name} PROPERTIES OUTPUT_NAME "${bin_var}")
      target_link_libraries("${test_name}" ${CMAKE_PROJECT_NAME} crypto ssl)
      add_test("${test_name}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${bin_var}")
      if (RUN_TEST_VALGRIND)
        add_test("${test_name}_valgrind" "${CMAKE_HOME_DIRECTORY}/valgrind" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${bin_var}")
      endif ()
    endforeach (loop_var)
  endif ()

  file(GLOB_RECURSE emacs_files "*~" "*\#*\#")
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${emacs_files}")
  set(${var} ${src_files})
endmacro(list_source_files)

## !Source files ##

## Packaging ##

macro(generate_debian_package pkgname contact maintainer descr)
  execute_process(COMMAND "dpkg" "--print-architecture" OUTPUT_VARIABLE DPKGARCH)
  string(REPLACE "\n" "" DPKGARCH "${DPKGARCH}")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_SYSTEM_NAME ${DPKGARCH})
  set(CPACK_PACKAGE_NAME ${pkgname})
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${VERSION}_${CPACK_SYSTEM_NAME}")
  set(CPACK_PACKAGE_CONTACT ${contact})
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "/usr/lib/")
  set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${maintainer})
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${descr})
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-dev_${VERSION}_${CPACK_SYSTEM_NAME}")
  set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;\\\\.#.*;#.*#;.*~;CMakeFiles;_CPack_Packages;.*\\\\.cmake;install_manifest.txt;CMakeCache.txt;${CMAKE_PACKAGE_NAME}.*\\\\.(tar\\\\.gz|tar\\\\.bz2|deb);Makefile;")
  set(CPACK_SOURCE_GENERATOR "DEB")
  include(CPack)
endmacro(generate_debian_package)
## !Packaging ##
