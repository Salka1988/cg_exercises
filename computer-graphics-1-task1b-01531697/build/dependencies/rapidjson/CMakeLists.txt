cmake_minimum_required(VERSION 3.2)

project(rapidjson)

set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")

add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${INCLUDE_DIR})
