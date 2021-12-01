cmake_minimum_required(VERSION 3.2)

project(zlib)

set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB zlib_SOURCES "${SOURCE_DIR}/*.c")

add_library(zlib STATIC ${zlib_SOURCES})

if (MSVC)
	target_compile_definitions(zlib PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif ()

set_property(TARGET zlib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(zlib PUBLIC Z_SOLO)
target_include_directories(zlib PUBLIC ${INCLUDE_DIR})
