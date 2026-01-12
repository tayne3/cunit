cmake_minimum_required(VERSION 3.14)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/PreventInSourceBuilds.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake)
git_version_info(VERSION CUNIT_VERSION DEFAULT_VERSION 0.2.5)
project(cunit VERSION ${CUNIT_VERSION})

include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectIsTopLevel.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/OptionVariables.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectConfig.cmake)

set(CUNIT_LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  set(CUNIT_LIB_TYPE SHARED)
endif()
message(STATUS "cunit v${CUNIT_VERSION} ${CUNIT_LIB_TYPE} library")

add_library(cunit ${CUNIT_LIB_TYPE})
add_library(cunit::cunit ALIAS cunit)
target_link_libraries(cunit PRIVATE cunit_compile_dependency)
target_include_directories(cunit PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_sources(cunit PRIVATE
  src/compare.c
  src/init.c
  src/suite.c
)

if(CUNIT_BUILD_EXAMPLE)
  enable_testing()
  add_subdirectory(example)
endif()
