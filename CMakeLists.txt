cmake_minimum_required(VERSION 3.10)
project(cligen_example C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_definitions(-D_DEFAULT_SOURCE)

include(ExternalProject)

if(CMAKE_CROSSCOMPILING)
    set(CLIGEN_DEPENDENCIES_DEFAULT ON)
else()
    set(CLIGEN_DEPENDENCIES_DEFAULT OFF)
endif()

option(CLIGEN_DEPENDENCIES "Build cligen from source" ${CLIGEN_DEPENDENCIES_DEFAULT})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FindModules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/BuildModules")

if(CLIGEN_DEPENDENCIES)
    include(BuildCligen)
else()
    find_package(Cligen REQUIRED)
    include_directories(${CLIGEN_INCLUDE_DIRS})
endif()

# add_executable(simple_cli_cligen
#     simple_cli_cligen.c
# )

file(COPY ${CMAKE_SOURCE_DIR}/network.cli DESTINATION ${CMAKE_BINARY_DIR})

add_executable(simple_cli_cligen_embedded
    simple_cli_cligen_embedded.c
    network_spec.h
)

if(CLIGEN_DEPENDENCIES)
    # add_dependencies(simple_cli_cligen cligen_target)
    add_dependencies(simple_cli_cligen_embedded cligen_target)
endif()

# target_link_libraries(simple_cli_cligen ${CLIGEN_LIBRARIES})
target_link_libraries(simple_cli_cligen_embedded ${CLIGEN_LIBRARIES})

target_compile_options(simple_cli_cligen_embedded PRIVATE -Wno-error=incompatible-pointer-types -Wno-error=cast-align)