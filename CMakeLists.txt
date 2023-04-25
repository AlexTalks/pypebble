cmake_minimum_required(VERSION 3.5)
project(pypebble)

set(CMAKE_CXX_STANDARD 17)

# Find python and Boost - both are required dependencies
find_package(Python 3.7 REQUIRED COMPONENTS Interpreter Development)
find_package(Boost COMPONENTS python REQUIRED)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

# Generate the CGo bindings
file(GLOB GO_BINDING_SRCS ${CMAKE_SOURCE_DIR}/pkg/bindings/*.go)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/libpebble.so ${CMAKE_BINARY_DIR}/libpebble.h
    COMMAND CGO_CPPFLAGS="-I${CMAKE_SOURCE_DIR}" go build -buildmode=c-shared -o libpebble.so ${CMAKE_SOURCE_DIR}/pkg/bindings
    # Also copy generated header to source dir so it can be under version control.
    COMMAND cp ${CMAKE_BINARY_DIR}/libpebble.h ${CMAKE_SOURCE_DIR}/libpebble.h
    DEPENDS ${CMAKE_SOURCE_DIR}/libpebble-common.h ${GO_BINDING_SRCS}
    COMMENT "Generating CGo bindings."
)
add_custom_target(gen_libpebble DEPENDS libpebble.h)

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
add_library(pypebble MODULE
        pebblepp/common.cpp
        pebblepp/iterator.cpp
        pebblepp/options.cpp
        pebblepp/pebble.cpp
        pebblepp/pypebble.cpp)
add_dependencies(pypebble gen_libpebble)

# Set up the libraries and header search paths for this target
target_link_libraries(pypebble
        ${CMAKE_BINARY_DIR}/libpebble.so
        ${Boost_LIBRARIES}
        ${Python_LIBRARIES})
target_include_directories(pypebble PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${Python_INCLUDE_DIRS})

add_executable(pebtest.out
        pebblepp/common.cpp
        pebblepp/iterator.cpp
        pebblepp/options.cpp
        pebblepp/pebble.cpp
        pebblepp/main.cpp)
add_dependencies(pebtest.out gen_libpebble)
target_link_libraries(pebtest.out ${CMAKE_BINARY_DIR}/libpebble.so)
target_include_directories(pebtest.out PRIVATE ${CMAKE_SOURCE_DIR})

# Copy shared objects as necessary for debugging/IDEs.
add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/libpebble.so ${CMAKE_SOURCE_DIR}/pypebble.so
        COMMAND cp ${CMAKE_BINARY_DIR}/libpebble.so ${CMAKE_SOURCE_DIR}/libpebble.so
        COMMAND cp ${CMAKE_BINARY_DIR}/pypebble.so ${CMAKE_SOURCE_DIR}/pypebble.so
        DEPENDS gen_libpebble pypebble
        COMMENT "Copying shared objects."
)
add_custom_target(copy_shared_objs DEPENDS libpebble.so pypebble.so)