cmake_minimum_required(VERSION 3.5)
project(rinoo)

option(DEBUGINFO "Add debuginfo to all builds" ON)
option(BUILD_TEST "build tests" ON)
option(BUILD_TEST_IPV6 "build tests for IPv6" ON)
option(RUN_TEST_VALGRIND "run tests with valgrind" ON)
set(RINOO_JUMP "fcontext" CACHE STRING "Library for switch context (fcontext, boost)")

include(rinoo.cmake)
set_version(2 0 1)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB cmakeFiles cmake/*.cmake)
foreach(cmakeFile ${cmakeFiles})
        message("INCLUDE ${cmakeFile}")
        INCLUDE(${cmakeFile})
endforeach(cmakeFile)

## Dependencies ##
check_dependency_func(epoll_create)
check_dependency_func(epoll_ctl)

check_dependency_sym(res_init "resolv.h")
## !Dependencies ##

include_directories(include)

## Library ##
set (RINOO_REQ_LIBS)
list(APPEND RINOO_REQ_LIBS "ssl")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND RINOO_REQ_LIBS Threads::Threads)
set(test_files "")
set(bench_files "")
list_source_files(src_files "src/*.c")

if (RINOO_JUMP STREQUAL "fcontext")
  list(APPEND src_files "src/scheduler/fcontext/fcontext.S")
  add_definitions(-DRINOO_JUMP_FCONTEXT)
elseif (RINOO_JUMP STREQUAL "boost")
  add_subdirectory(contrib/fcontext)
  list(APPEND RINOO_REQ_LIBS fcontext)
  include_directories(contrib/fcontext/include)
  add_definitions(-DRINOO_JUMP_BOOST)
else ()
  message(FATAL_ERROR "unknown context switch option: ${RINOO_JUMP}")
endif ()

add_library(${CMAKE_PROJECT_NAME} SHARED ${src_files})
target_link_libraries(${CMAKE_PROJECT_NAME} ${RINOO_REQ_LIBS})
add_library(${CMAKE_PROJECT_NAME}_static STATIC ${src_files})
set_target_properties(${CMAKE_PROJECT_NAME}_static  PROPERTIES
    OUTPUT_NAME ${CMAKE_PROJECT_NAME}
    COMPILE_FLAGS "-fPIC"
)
## !Library ##

## Packaging ##
generate_debian_package(
  "lib${CMAKE_PROJECT_NAME}"
  "reginald.l@gmail.com"
  "Reginald LIPS"
  "RiNOO is a library for asynchronous socket management"
)
install(TARGETS ${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME}_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/rinoo" DESTINATION include)
## !Packaging ##
