cmake_minimum_required(VERSION 2.8)

project(lpng)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/libpng")

file(GLOB lpng_SOURCES "${SOURCE_DIR}/*.c")

include_directories(${ZLIB_INCLUDE_DIR})
add_library(lpng STATIC ${lpng_SOURCES})

set(LPNG_LIBRARY lpng PARENT_SCOPE)
set(LPNG_INCLUDE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
