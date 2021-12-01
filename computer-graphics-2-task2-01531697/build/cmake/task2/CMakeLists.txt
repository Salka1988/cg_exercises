cmake_minimum_required(VERSION 2.8)

project(task2)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../source/task2")

include_directories(${Framework_INCLUDE_DIRS})

file(GLOB cpp_SOURCES "${SOURCE_DIR}/*.h" "${SOURCE_DIR}/*.cpp")

add_executable(${PROJECT_NAME} ${cpp_SOURCES})
target_link_libraries(${PROJECT_NAME} ${Framework_LIBRARIES})
