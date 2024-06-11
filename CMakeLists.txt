cmake_minimum_required(VERSION 3.5)
project(pypebble)

set(CMAKE_CXX_STANDARD 17)

# Find python and pybind11 - both are required dependencies
find_package(Python 3.7 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

add_subdirectory(pebble_bindings)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

# pebblepp library
file(GLOB pebblepp_srcs ${CMAKE_CURRENT_SOURCE_DIR}/pebblepp/*.cpp)
add_library(pebblepp STATIC ${pebblepp_srcs})
target_link_libraries(pebblepp binding_lib)
target_include_directories(pebblepp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# pebtest.out executable
# TODO: Refactor this into a real test using gtest
add_executable(pebtest.out
        ${CMAKE_CURRENT_SOURCE_DIR}/pebblepp/tests/pebtest.cpp)
target_link_libraries(pebtest.out pebblepp)
target_include_directories(pebtest.out PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# pypebble shared module - modules are intended to be imported at runtime.
file(GLOB pypebble_srcs ${CMAKE_CURRENT_SOURCE_DIR}/pypebble/*.cpp)
add_library(pypebble MODULE ${pypebble_srcs})
target_link_libraries(pypebble
        pebblepp
        pybind11::module
        ${Python_LIBRARIES})
target_include_directories(pypebble PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Python_INCLUDE_DIRS})