# FindPTLib.cmake
# Find PTLib library module

find_path(PTLIB_INCLUDE_DIR
    NAMES ptlib.h
    PATHS
        ${PTLIB_DIR}/include
        $ENV{PTLIBDIR}/include
        ${H323PLUS_DIR}/include
        $ENV{OPENH323DIR}/include
        /usr/local/include
        /usr/include
)

find_library(PTLIB_LIBRARY
    NAMES ptlib PTLib
    PATHS
        ${PTLIB_DIR}/lib
        $ENV{PTLIBDIR}/lib
        ${H323PLUS_DIR}/lib
        $ENV{OPENH323DIR}/lib
        /usr/local/lib
        /usr/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PTLib
    REQUIRED_VARS PTLIB_LIBRARY PTLIB_INCLUDE_DIR
)

if(PTLIB_FOUND)
    set(PTLIB_INCLUDE_DIRS ${PTLIB_INCLUDE_DIR})
    set(PTLIB_LIBRARIES ${PTLIB_LIBRARY})
endif()
