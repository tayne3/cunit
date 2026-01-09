cmake_minimum_required(VERSION 3.14)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/PreventInSourceBuilds.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GitVersion.cmake)
git_version_info(VERSION CUNIT_VERSION DEFAULT_VERSION 0.2.4)
project(cunit VERSION ${CUNIT_VERSION})

include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectIsTopLevel.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/OptionVariables.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectConfig.cmake)

if(CUNIT_BUILD_SHARED)
  add_library(cunit_shared SHARED)
  set(CUNIT cunit_shared)
  message(STATUS "cunit v${CUNIT_VERSION} shared library")
else()
  add_library(cunit_static STATIC)
  set(CUNIT cunit_static)
  message(STATUS "cunit v${CUNIT_VERSION} static library")
endif()
add_library(cunit::cunit ALIAS ${CUNIT})

target_sources(${CUNIT} PRIVATE
  src/compare.c
  src/init.c
  src/suite.c
)
target_link_libraries(${CUNIT} PRIVATE 
  cunit_compile_dependency
)
target_include_directories(${CUNIT} PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(CUNIT_BUILD_EXAMPLE)
  enable_testing()
  add_subdirectory(example)
endif()
