cmake_minimum_required(VERSION 3.5)
project(rinoo)

option(DEBUGINFO "Add debuginfo to all builds" ON)
option(BUILD_TEST "build tests" ON)
option(BUILD_TEST_IPV6 "build tests for IPv6" ON)
option(RUN_TEST_VALGRIND "run tests with valgrind" ON)

include(rinoo.cmake)
set_version(2 0 1)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## Dependencies ##
check_dependency_func(epoll_create)
check_dependency_func(epoll_ctl)

check_dependency_sym(res_init "resolv.h")
## !Dependencies ##

include_directories(include)

## Library ##
list_source_files(src_files "src/*.[cS]")
add_library(${CMAKE_PROJECT_NAME} SHARED ${src_files})
target_link_libraries(${CMAKE_PROJECT_NAME} ssl pthread)
add_library(${CMAKE_PROJECT_NAME}_static STATIC ${src_files})
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
