cmake_minimum_required(VERSION 2.8)

project(framework)

find_package(OpenGL REQUIRED)

set(CMAKE_DEBUG_POSTFIX "D")

set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")

if (WIN32)
	add_subdirectory(${DEPENDENCIES_DIR}/Win32_core_tools/build/cmake Win32_core_tools)
endif()
add_subdirectory(${DEPENDENCIES_DIR}/GL_platform_tools/build/cmake GL_platform_tools)
add_subdirectory(${DEPENDENCIES_DIR}/GL_core_tools/build/cmake GL_core_tools)

add_definitions(-DZ_SOLO)
add_subdirectory(imgui imgui)
add_subdirectory(zlib zlib)
add_subdirectory(libpng libpng)
add_subdirectory(rapidjson rapidjson)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../source/framework")

set(Framework_INCLUDE_DIRS_internal
	${Win32_core_tools_INCLUDE_DIRS}
	${GL_platform_tools_INCLUDE_DIRS}
	${GL_core_tools_INCLUDE_DIRS}
	${IMGUI_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${LPNG_INCLUDE_DIR}
	${RAPIDJSON_INCLUDE_DIR}
	"${SOURCE_DIR}/"
	"${SOURCE_DIR}/../"
)

include_directories(${Framework_INCLUDE_DIRS_internal})

add_subdirectory_if_exists(utils)

file(GLOB FRAMEWORK_SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h" "${SOURCE_DIR}/math/*.cpp" "${SOURCE_DIR}/math/*.h" "${SOURCE_DIR}/utils/*.cpp" "${SOURCE_DIR}/utils/*.h")

add_library(framework ${FRAMEWORK_PLATFORM_SOURCES} ${FRAMEWORK_SOURCES})

if (WIN32)
	set(Framework_INCLUDE_DIRS ${Framework_INCLUDE_DIRS_internal} PARENT_SCOPE)
	set(Framework_LIBRARIES framework ${LPNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_gl_LIBRARY} Win32_core_tools ${GL_platform_tools_LIBRARIES} ${IMGUI_LIBRARY} GL_core_tools PARENT_SCOPE)
else ()
	target_link_libraries(framework stdc++fs)
	set(Framework_INCLUDE_DIRS ${Framework_INCLUDE_DIRS_internal} PARENT_SCOPE)
	set(Framework_LIBRARIES framework ${LPNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENGL_gl_LIBRARY} ${X11_LIBRARIES} ${CMAKE_DL_LIBS} ${GL_platform_tools_LIBRARIES}  ${IMGUI_LIBRARY} GL_core_tools pthread PARENT_SCOPE)
endif ()

set_property(TARGET zlib PROPERTY FOLDER framework)
set_property(TARGET lpng PROPERTY FOLDER framework)
set_property(TARGET imgui PROPERTY FOLDER framework)
set_property(TARGET GL_platform_tools PROPERTY FOLDER framework)
set_property(TARGET GL_core_tools PROPERTY FOLDER framework)
if (WIN32)
	set_property(TARGET Win32_core_tools PROPERTY FOLDER framework)
	set_property(TARGET glcore PROPERTY FOLDER framework)
endif()
set_property(TARGET framework PROPERTY FOLDER framework)
