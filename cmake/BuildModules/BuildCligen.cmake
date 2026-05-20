include_guard(GLOBAL)

set(CONFIGURE_HOST_FLAG "")
if(CMAKE_CROSSCOMPILING)
    string(REGEX MATCH "-target ([^ ]*)" MATCH_TARGET "${CMAKE_C_FLAGS_INIT}")
    if(CMAKE_MATCH_1)
        set(CONFIGURE_HOST_FLAG "--host=${CMAKE_MATCH_1}")
    endif()
endif()

ExternalProject_Add(
    cligen_external
    GIT_REPOSITORY     https://github.com/clicon/cligen.git
    GIT_TAG            7.7.0
    GIT_SHALLOW        1
    CONFIGURE_COMMAND  env CC=${CMAKE_C_COMPILER} CFLAGS=${CMAKE_C_FLAGS_INIT} LDFLAGS=${CMAKE_C_FLAGS_INIT} AR=${CMAKE_AR} RANLIB=${CMAKE_RANLIB} <SOURCE_DIR>/configure --prefix=${CMAKE_BINARY_DIR} ${CONFIGURE_HOST_FLAG}
    BUILD_COMMAND      make -j LINKAGE=static
    INSTALL_COMMAND    make install LINKAGE=static
)

set(CLIGEN_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/include)
set(CLIGEN_LIBRARY_PATH ${CMAKE_BINARY_DIR}/lib/libcligen.a)

add_custom_target(cligen_target
    BYPRODUCTS ${CLIGEN_LIBRARY_PATH} ${CLIGEN_INCLUDE_DIRS}
    COMMAND "" 
    DEPENDS cligen_external
)

set(CLIGEN_LIBRARIES ${CLIGEN_LIBRARY_PATH})
include_directories(${CLIGEN_INCLUDE_DIRS})
