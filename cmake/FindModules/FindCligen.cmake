find_path(CLIGEN_INCLUDE_DIR cligen/cligen.h)

if(CLIGEN_LINK_STATIC)
    find_library(CLIGEN_LIBRARY NAMES cligen_a.a libcligen_a.a cligen libcligen)
else()
    find_library(CLIGEN_LIBRARY NAMES cligen libcligen)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Cligen DEFAULT_MSG CLIGEN_LIBRARY CLIGEN_INCLUDE_DIR)

set(CLIGEN_INCLUDE_DIRS ${CLIGEN_INCLUDE_DIR})
set(CLIGEN_LIBRARIES ${CLIGEN_LIBRARY})

include_directories(${CLIGEN_INCLUDE_DIRS})

mark_as_advanced(CLIGEN_INCLUDE_DIR CLIGEN_LIBRARY)