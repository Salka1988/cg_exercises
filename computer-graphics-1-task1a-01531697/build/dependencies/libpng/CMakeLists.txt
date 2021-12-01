cmake_minimum_required(VERSION 3.2)

project(libpng)

set(SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB libpng_SOURCES "${SOURCE_DIR}/*.c")

add_library(libpng STATIC ${libpng_SOURCES})

if (MSVC)
	target_compile_definitions(libpng PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif ()

set_property(TARGET libpng PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(libpng PUBLIC ${INCLUDE_DIR})
target_link_libraries(libpng zlib)
