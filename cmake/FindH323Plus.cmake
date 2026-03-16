# FindH323Plus.cmake
# Find H323Plus library module

find_path(H323PLUS_INCLUDE_DIR
    NAMES h323.h
    PATHS
        ${H323PLUS_DIR}/include
        $ENV{OPENH323DIR}/include
        /usr/local/include
        /usr/include
)

find_library(H323PLUS_LIBRARY
    NAMES h323 H323
    PATHS
        ${H323PLUS_DIR}/lib
        $ENV{OPENH323DIR}/lib
        /usr/local/lib
        /usr/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(H323Plus
    REQUIRED_VARS H323PLUS_LIBRARY H323PLUS_INCLUDE_DIR
)

if(H323PLUS_FOUND)
    set(H323PLUS_INCLUDE_DIRS ${H323PLUS_INCLUDE_DIR})
    set(H323PLUS_LIBRARIES ${H323PLUS_LIBRARY})
endif()
