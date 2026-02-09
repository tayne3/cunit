cmake_minimum_required(VERSION 3.14)

# In-source build guard
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
    FATAL_ERROR
    "In-source builds are not supported. "
    "You may need to delete 'CMakeCache.txt' and 'CMakeFiles/' before rebuilding this project."
  )
endif()

project(cunit VERSION 0.2.7)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# This variable is set by project() in CMake 3.21+
if(NOT DEFINED PROJECT_IS_TOP_LEVEL)
  string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" PROJECT_IS_TOP_LEVEL)
endif()

if(PROJECT_IS_TOP_LEVEL)
  option(CUNIT_BUILD_EXAMPLE "build example program" OFF)

  get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(NOT isMultiConfig
     AND NOT CMAKE_BUILD_TYPE
     AND NOT CMAKE_CONFIGURATION_TYPES
  )
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()

  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  
  include(GNUInstallDirs)
  set(CMAKE_SKIP_BUILD_RPATH OFF)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)
  if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/${CMAKE_INSTALL_LIBDIR}")
  else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/${CMAKE_INSTALL_LIBDIR}")
  endif()
endif()

add_library(cunit_options INTERFACE)
target_compile_features(cunit_options INTERFACE c_std_99)
if(UNIX)
  target_link_libraries(cunit_options INTERFACE pthread)
endif()
if(WIN32)
  target_compile_definitions(cunit_options INTERFACE
    _WIN32_WINNT=0x0600
    WIN32_LEAN_AND_MEAN
  )
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(cunit_options INTERFACE 
    "$<$<COMPILE_LANGUAGE:C>:/utf-8>"
  )
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(cunit_options INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:/utf-8>"
    "$<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>"
  )
endif()

if(NOT (CMAKE_C_COMPILER_ID STREQUAL "MSVC" OR ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")))
  target_compile_options(cunit_options INTERFACE
    "$<$<COMPILE_LANGUAGE:C>:-Wall>"
    "$<$<COMPILE_LANGUAGE:C>:-Wextra>"
  )
  check_c_compiler_flag("-Werror=return-type" HAVE_C_WERROR_RETURN_TYPE)
  if(HAVE_C_WERROR_RETURN_TYPE)
    target_compile_options(cunit_options INTERFACE
      "$<$<COMPILE_LANGUAGE:C>:-Werror=return-type>"
    )
  endif()
endif()
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")))
  target_compile_options(cunit_options INTERFACE
    "$<$<COMPILE_LANGUAGE:CXX>:-Wall>"
    "$<$<COMPILE_LANGUAGE:CXX>:-Wextra>"
  )
  check_cxx_compiler_flag("-Werror=return-type" HAVE_CXX_WERROR_RETURN_TYPE)
  if(HAVE_CXX_WERROR_RETURN_TYPE)
    target_compile_options(cunit_options INTERFACE
      "$<$<COMPILE_LANGUAGE:CXX>:-Werror=return-type>"
    )
  endif()
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS 6.0)
  target_compile_options(cunit_options INTERFACE 
    "$<$<COMPILE_LANGUAGE:C>:-Wno-missing-field-initializers>"
  )
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
  target_compile_options(cunit_options INTERFACE 
    "$<$<COMPILE_LANGUAGE:CXX>:-Wno-missing-field-initializers>"
  )
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(cunit_options INTERFACE 
    "$<$<COMPILE_LANGUAGE:C>:/GS>"
  )
  if(MSVC_VERSION GREATER_EQUAL 1920)
    target_compile_options(cunit_options INTERFACE 
      "$<$<COMPILE_LANGUAGE:C>:/guard:cf>"
    )
  endif()
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(cunit_options INTERFACE 
    "$<$<COMPILE_LANGUAGE:CXX>:/GS>"
  )
  if(MSVC_VERSION GREATER_EQUAL 1920)
    target_compile_options(cunit_options INTERFACE 
      "$<$<COMPILE_LANGUAGE:CXX>:/guard:cf>"
    )
  endif()
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" OR ("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_C_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC"))
  string(REPLACE "/" "\\\\" CUNIT_ROOT_PATH ${CMAKE_SOURCE_DIR})
  string(REPLACE "/" "\\\\" CUNIT_BUILD_PATH ${CMAKE_BINARY_DIR})
else()
  set(CUNIT_ROOT_PATH ${CMAKE_SOURCE_DIR})
  set(CUNIT_BUILD_PATH ${CMAKE_BINARY_DIR})
endif()

if(BUILD_SHARED_LIBS)
  set(CUNIT_LIB_TYPE SHARED)
else()
  set(CUNIT_LIB_TYPE STATIC)
endif()
message(STATUS "cunit v${PROJECT_VERSION} ${CUNIT_LIB_TYPE} library")
add_library(cunit ${CUNIT_LIB_TYPE}
  src/compare.c
  src/init.c
  src/suite.c
)
add_library(cunit::cunit ALIAS cunit)
target_link_libraries(cunit PRIVATE cunit_options)
target_compile_definitions(cunit PRIVATE
  CUNIT_ROOT_PATH="${CUNIT_ROOT_PATH}"
  CUNIT_BUILD_PATH="${CUNIT_BUILD_PATH}"
)
target_include_directories(cunit PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(CUNIT_BUILD_EXAMPLE)
  enable_testing()
  add_subdirectory(example)
endif()
