cmake_minimum_required(VERSION 3.5)
project(pypebble)

set(CMAKE_CXX_STANDARD 17)

# Find python and Boost - both are required dependencies
find_package(PythonLibs 3.11 REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")

# Add a shared module - modules are intended to be imported at runtime.
# - This is where you add the source files
add_library(pebble_ext MODULE pebblepp/pebble.cpp pebblepp/pypebble.cpp)

# Set up the libraries and header search paths for this target
target_link_libraries(pebble_ext ${CMAKE_SOURCE_DIR}/libpebble.so ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_include_directories(pebble_ext PRIVATE ${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_DIRS})

add_executable(pebtest.out pebblepp/pebble.cpp pebblepp/main.cpp)
target_link_libraries(pebtest.out ${CMAKE_SOURCE_DIR}/libpebble.so ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_include_directories(pebtest.out PRIVATE ${CMAKE_SOURCE_DIR} ${PYTHON_INCLUDE_DIRS})