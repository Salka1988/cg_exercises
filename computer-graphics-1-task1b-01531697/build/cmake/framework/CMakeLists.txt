cmake_minimum_required(VERSION 3.2)

project(framework)

set(DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies")

add_subdirectory(${DEPENDENCIES_DIR}/zlib zlib)
add_subdirectory(${DEPENDENCIES_DIR}/libpng libpng)
add_subdirectory(${DEPENDENCIES_DIR}/rapidjson rapidjson)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../source/framework")

source_group("source" REGULAR_EXPRESSION ".*\.h$|.*\.cpp$")
source_group("source\\math" REGULAR_EXPRESSION ".*/math/.*")
source_group("source\\utils" REGULAR_EXPRESSION ".*/utils/.*")

add_library(framework
	"${SOURCE_DIR}/math/math.h"
	"${SOURCE_DIR}/math/vector.h"
	"${SOURCE_DIR}/math/matrix.h"
	"${SOURCE_DIR}/utils/argparse.h"
	"${SOURCE_DIR}/utils/argparse.cpp"
	"${SOURCE_DIR}/utils/memory.h"
	"${SOURCE_DIR}/utils/color.h"
	"${SOURCE_DIR}/utils/io.h"
	"${SOURCE_DIR}/utils/thread_pool.h"
	"${SOURCE_DIR}/color.h"
	"${SOURCE_DIR}/image.h"
	"${SOURCE_DIR}/png.h"
	"${SOURCE_DIR}/png.cpp"
	"${SOURCE_DIR}/pfm.h"
	"${SOURCE_DIR}/pfm.cpp"
	"${SOURCE_DIR}/rgba8.h"
	"${SOURCE_DIR}/config.h"
	"${SOURCE_DIR}/config.cpp"
	"${SOURCE_DIR}/obj_stream.h"
	"${SOURCE_DIR}/obj_reader.h"
	"${SOURCE_DIR}/obj.h"
	"${SOURCE_DIR}/obj.cpp"
	"${SOURCE_DIR}/bounding_box.h"
	"${SOURCE_DIR}/bvh.h"
	"${SOURCE_DIR}/main.cpp"
)

if (MSVC)
	target_compile_options(framework PRIVATE /WX /MP /Gm- /permissive-)
	target_compile_definitions(framework PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif ()

set_target_properties(framework PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(framework PUBLIC "${SOURCE_DIR}/../")
if (NOT WIN32)
	target_link_libraries(framework stdc++fs)
endif ()
target_link_libraries(framework libpng rapidjson)


option(BUILD_MATH_TEST "build math test" OFF)

if (BUILD_MATH_TEST)
	add_executable(math_test "${SOURCE_DIR}/math/test.cpp")
	target_link_libraries(math_test framework)
	set_property(TARGET math_test PROPERTY FOLDER framework)
endif ()

set_property(TARGET zlib PROPERTY FOLDER framework)
set_property(TARGET libpng PROPERTY FOLDER framework)
set_property(TARGET framework PROPERTY FOLDER framework)

set_directory_properties(${CMAKE_CURRENT_SOURCE_DIR} VS_STARTUP_PROJECT framework)
