cmake_minimum_required(VERSION 3.2)

project(task1a)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../source/task1a")

add_task(task1a
	"${SOURCE_DIR}/task_builder.h"
	"${SOURCE_DIR}/task_builder.cpp"
	"${SOURCE_DIR}/RayTracer.h"
	"${SOURCE_DIR}/RayTracer.cpp"
	"${SOURCE_DIR}/Scene.h"
	"${SOURCE_DIR}/Scene.cpp"
	"${SOURCE_DIR}/Task.h"
	"${SOURCE_DIR}/Task.cpp"
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/task1a.h"
	"${SOURCE_DIR}/task1a.cpp"
)

set_directory_properties(${CMAKE_CURRENT_SOURCE_DIR} VS_STARTUP_PROJECT task1a)
