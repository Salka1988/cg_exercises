cmake_minimum_required(VERSION 2.8)

project(rapidjson)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/")

#add_library(rapidjson INTERFACE)

set(RAPIDJSON_LIBRARY rapidjson PARENT_SCOPE)
set(RAPIDJSON_INCLUDE_DIR "${INCLUDE_DIR}" PARENT_SCOPE)
