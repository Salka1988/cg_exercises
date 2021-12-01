cmake_minimum_required(VERSION 2.8)

project(imgui)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/imgui/")

file(GLOB imgui_HEADERS "${SOURCE_DIR}/*.h")
file(GLOB imgui_SOURCES "${SOURCE_DIR}/*.cpp")

add_library(imgui STATIC ${imgui_HEADERS} ${imgui_SOURCES})

set(IMGUI_LIBRARY imgui PARENT_SCOPE)
set(IMGUI_INCLUDE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
