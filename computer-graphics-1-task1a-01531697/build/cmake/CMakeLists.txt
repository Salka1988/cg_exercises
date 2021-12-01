cmake_minimum_required(VERSION 3.10)

project(CG1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX D)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:lib>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:lib>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:bin>")

set(CMAKE_CONFIGURATION_TYPES "Release;Debug")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (FULL_VERSION)
	add_definitions(-DFULL_VERSION)
endif ()

option(BUILD_TASK1A "build task1a" ON)
option(BUILD_TASK1B "build task1b" OFF)


function(add_subdirectory_if_exists dir)
	if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
		add_subdirectory(${dir})
	endif (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
endfunction ()

add_subdirectory(framework framework)

function(add_task name)
	source_group("source" FILES ${ARGN})
	add_executable(${name} ${ARGN})
	if (MSVC)
		target_compile_options(${name} PRIVATE /MP /Gm- /permissive-)
		target_compile_definitions(${name} PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	endif ()
	target_link_libraries(${name} framework)
endfunction(add_task)

if (BUILD_TASK1A)
	add_subdirectory_if_exists(task1a task1a)
endif ()

if (BUILD_TASK1B)
	add_subdirectory_if_exists(task1b task1b)
endif ()

set_directory_properties(${CMAKE_CURRENT_LIST_DIR} VS_STARTUP_PROJECT task1a)
