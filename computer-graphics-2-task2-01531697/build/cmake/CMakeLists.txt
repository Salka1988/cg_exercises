make_minimum_required(VERSION 3.2)

project(CG2)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX D)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:lib>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:lib>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<1:bin>")

set(CMAKE_CONFIGURATION_TYPES "Release;Debug")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


if (MSVC)
	set(CMAKE_CXX_FLAGS "/MP /Gm- ${CMAKE_CXX_FLAGS}")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif ()

if (WIN32)
	add_definitions(-DGLCORE_STATIC)
endif ()

if (FULL_VERSION)
	add_definitions(-DFULL_VERSION)
endif ()

function(add_subdirectory_if_exists dir)
	if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
		add_subdirectory(${dir})
	endif (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
endfunction ()

add_subdirectory(framework)
add_subdirectory_if_exists(task2)
