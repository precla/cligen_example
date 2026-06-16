cmake_minimum_required(VERSION 3.10)
project(cligen_example C)

# set(CMAKE_C_STANDARD 17)
# set(CMAKE_C_STANDARD_REQUIRED ON)

# add_definitions(-D_DEFAULT_SOURCE)

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

add_executable(scli scli.c)

if(CLIGEN_DEPENDENCIES)
    add_dependencies(scli cligen_target)
endif()

target_link_libraries(scli ${CLIGEN_LIBRARIES})
