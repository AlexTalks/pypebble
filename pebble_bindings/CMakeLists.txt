cmake_minimum_required(VERSION 3.5)

set(BINDING_LIB libpebble.a)
set(BINDING_HEADER libpebble.h)
set(BINDING_TARGET pebble_bindings)

add_library(cgo-common INTERFACE)
target_include_directories(cgo-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Generate the CGo bindings
file(GLOB GO_BINDING_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/pkg/bindings/*.go)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_LIB} ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_HEADER}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND CGO_CPPFLAGS="-I${CMAKE_CURRENT_SOURCE_DIR}" go build -tags=netgo -buildmode=c-archive -o ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_LIB} ${CMAKE_GO_FLAGS} ./...
        DEPENDS cgo-common ${GO_BINDING_SRCS}
        COMMENT "Generating CGo bindings."
)
add_custom_target(${BINDING_TARGET} DEPENDS ${BINDING_LIB} ${BINDING_HEADER})

add_library(binding_lib STATIC IMPORTED GLOBAL)
add_dependencies(binding_lib ${BINDING_TARGET})
target_link_libraries(binding_lib INTERFACE cgo-common)
if(APPLE)
    find_library(CoreFoundation CoreFoundation)
    find_library(Security Security)
    target_link_libraries(binding_lib INTERFACE ${CoreFoundation} ${Security})
endif()
set_target_properties(binding_lib
  PROPERTIES
  IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_LIB}
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})
