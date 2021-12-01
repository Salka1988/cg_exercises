cmake_minimum_required(VERSION 2.8)

project(zlib)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/zlib")

file(GLOB zlib_SOURCES "${SOURCE_DIR}/*.c")

add_library(zlib STATIC ${zlib_SOURCES})

set(ZLIB_LIBRARY zlib PARENT_SCOPE)
set(ZLIB_INCLUDE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
