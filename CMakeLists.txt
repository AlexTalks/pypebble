cmake_minimum_required(VERSION 3.5)
project(pypebble)

set(CMAKE_CXX_STANDARD 17)

# Find python and pybind11 - both are required dependencies
find_package(Python 3.7 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

add_subdirectory(pebble_bindings)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

# pypebble shared module - modules are intended to be imported at runtime.
add_library(pypebble MODULE
        pebblepp/common.cpp
        pebblepp/iterator.cpp
        pebblepp/options.cpp
        pebblepp/pebble.cpp
        pebblepp/pypebble.cpp)
add_dependencies(pypebble pebble_bindings)
target_link_libraries(pypebble
        binding_lib
        pybind11::module
        ${Python_LIBRARIES})
target_include_directories(pypebble PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Python_INCLUDE_DIRS})

# pebtest.out
add_executable(pebtest.out
        pebblepp/common.cpp
        pebblepp/iterator.cpp
        pebblepp/options.cpp
        pebblepp/pebble.cpp
        pebblepp/main.cpp)
add_dependencies(pebtest.out binding_lib)
target_link_libraries(pebtest.out binding_lib)
target_include_directories(pebtest.out PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Copy shared objects as necessary for debugging/IDEs.
#add_custom_command(
#        OUTPUT ${CMAKE_SOURCE_DIR}/libpebble.so ${CMAKE_SOURCE_DIR}/pypebble.so
#        COMMAND cp ${CMAKE_BINARY_DIR}/libpebble.so ${CMAKE_SOURCE_DIR}/libpebble.so
#        COMMAND cp ${CMAKE_BINARY_DIR}/pypebble.so ${CMAKE_SOURCE_DIR}/pypebble.so
#        DEPENDS gen_libpebble pypebble
#        COMMENT "Copying shared objects."
#)
#add_custom_target(copy_shared_objs DEPENDS libpebble.so pypebble.so)