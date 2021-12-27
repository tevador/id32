# Copyright (c) 2021 tevador <tevador@gmail.com>
# See LICENSE for licensing information

cmake_minimum_required(VERSION 3.5)

project(id32)

set(id32_sources
src/id32.c)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message(STATUS "Setting default build type: ${CMAKE_BUILD_TYPE}")
endif()

add_library(id32 SHARED ${id32_sources})
set_property(TARGET id32 PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET id32 PROPERTY PUBLIC_HEADER include/id32.h)
include_directories(id32
  include/)
target_compile_definitions(id32 PRIVATE ID32_SHARED)
set_target_properties(id32 PROPERTIES VERSION 1.0.0
                                      SOVERSION 1
                                      C_STANDARD 11
                                      C_STANDARD_REQUIRED ON)

add_library(id32_static STATIC ${id32_sources})
set_property(TARGET id32_static PROPERTY POSITION_INDEPENDENT_CODE ON)
include_directories(id32_static
  include/)
target_compile_definitions(id32_static PRIVATE ID32_STATIC)
set_target_properties(id32_static PROPERTIES OUTPUT_NAME id32
                                             C_STANDARD 11
                                             C_STANDARD_REQUIRED ON)

add_executable(id32-tests
  tests/tests.c)
include_directories(id32-tests
  include/)
target_compile_definitions(id32-tests PRIVATE ID32_STATIC)
target_link_libraries(id32-tests
  PRIVATE id32_static)
