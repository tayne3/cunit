# Project Build Options and Variables

if(PROJECT_IS_TOP_LEVEL)
  get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(NOT isMultiConfig
     AND NOT CMAKE_BUILD_TYPE
     AND NOT CMAKE_CONFIGURATION_TYPES
  )
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()
endif()

option(CUNIT_BUILD_SHARED "build shared library" OFF)
mark_as_advanced(CUNIT_BUILD_SHARED)

if(PROJECT_IS_TOP_LEVEL)
    option(CUNIT_BUILD_EXAMPLE "build example program" OFF)
endif()
