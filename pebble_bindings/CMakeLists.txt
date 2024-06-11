set(BINDING_HEADER libpebble.h)
set(BINDING_TARGET pebble_bindings)

if(CGO_DYLD_ENABLED)
    message(STATUS "CGo dynamic linking enabled")
    set(BINDING_LIB libpebble.so)
    set(CGO_BUILDMODE c-shared)
else()
    set(BINDING_LIB libpebble.a)
    set(CGO_BUILDMODE c-archive)
    list(APPEND GO_TAG_LIST "netgo")
endif()

add_library(cgo-common INTERFACE)
target_include_directories(cgo-common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(COCKROACH)
    list(APPEND GO_TAG_LIST "cockroach_support")
endif()

# Generate the CGo bindings
file(GLOB GO_BINDING_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/pkg/bindings/*.go)
list(JOIN GO_TAG_LIST "," GO_TAGS)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_LIB} ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_HEADER}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND CGO_CPPFLAGS="-I${CMAKE_CURRENT_SOURCE_DIR}" go build -tags=${GO_TAGS} -buildmode=${CGO_BUILDMODE} -o ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_LIB} ${CMAKE_GO_FLAGS} ./...
        DEPENDS cgo-common ${GO_BINDING_SRCS}
        COMMENT "Generating CGo bindings."
)
add_custom_target(${BINDING_TARGET} DEPENDS ${BINDING_LIB} ${BINDING_HEADER})

if(CGO_DYLD_ENABLED)
    add_library(binding_lib SHARED IMPORTED GLOBAL)
else()
    add_library(binding_lib STATIC IMPORTED GLOBAL)
endif()
add_dependencies(binding_lib ${BINDING_TARGET})
target_link_libraries(binding_lib INTERFACE cgo-common)
if(APPLE AND NOT CGO_DYLD_ENABLED)
    find_library(CoreFoundation CoreFoundation)
    find_library(Security Security)
    target_link_libraries(binding_lib INTERFACE ${CoreFoundation} ${Security})
endif()
set_target_properties(binding_lib
  PROPERTIES
  IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${BINDING_LIB}
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})
