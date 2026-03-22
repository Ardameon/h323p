cmake_minimum_required(VERSION 3.16)
project(h323p
    VERSION 0.1.0
    DESCRIPTION "H.323 Test Utility"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(USE_H323PLUS "Use H323Plus library" OFF)

# ============================================================================
# Check submodules
# ============================================================================

set(SUBMODULES_OK TRUE)

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/deps/ptlib/configure")
    message(FATAL_ERROR "PTLib submodule not found. Run: git submodule update --init --recursive")
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/deps/h323plus/configure")
    message(FATAL_ERROR "H323Plus submodule not found. Run: git submodule update --init --recursive")
endif()

# Note: CppUTest uses system package due to C++17 compatibility issues

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/deps/cli11/CMakeLists.txt")
    message(FATAL_ERROR "CLI11 submodule not found. Run: git submodule update --init --recursive")
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/deps/pugixml/CMakeLists.txt")
    message(FATAL_ERROR "pugixml submodule not found. Run: git submodule update --init --recursive")
endif()

# ============================================================================
# Dependencies from submodules
# ============================================================================

# PTLib
message(STATUS "Building PTLib from submodule...")
set(PTLIB_DIR "${CMAKE_SOURCE_DIR}/deps/ptlib")
set(PWLIB_DIR "${PTLIB_DIR}")

# CLI11 - header only library
message(STATUS "Building CLI11 from submodule...")
add_subdirectory(deps/cli11)

# pugixml
message(STATUS "Building pugixml from submodule...")
add_subdirectory(deps/pugixml)

# CppUTest (for tests)
# Build from submodule with C++11 using ExternalProject for isolation
if(BUILD_TESTS)
    message(STATUS "Building CppUTest from submodule...")
    
    include(ExternalProject)
    
    # Paths for CppUTest build (note: libraries are in src/ subdirectory)
    set(CPPUTEST_BUILD_DIR "${CMAKE_BINARY_DIR}/_deps/cpputest_build")
    set(CPPUTEST_LIB_DIR "${CPPUTEST_BUILD_DIR}/src/CppUTest")
    set(CPPUTEST_EXT_LIB_DIR "${CPPUTEST_BUILD_DIR}/src/CppUTestExt")
    
    # Build CppUTest separately with C++11 to avoid polluting global C++17 flags
    ExternalProject_Add(CppUTest_build
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/cpputest"
        BINARY_DIR "${CPPUTEST_BUILD_DIR}"
        CMAKE_ARGS
            -DCMAKE_C_COMPILER=/usr/bin/gcc
            -DCMAKE_CXX_COMPILER=/usr/bin/g++
            -DCMAKE_CXX_STANDARD=11
            -DCMAKE_CXX_STANDARD_REQUIRED=ON
            -DCMAKE_CXX_EXTENSIONS=OFF
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCPPUTEST_BUILD_STATIC_LIBS=ON
            -DCPPUTEST_BUILD_32_BIT=OFF
            -DCPPUTEST_BUILD_TESTING=OFF
            -DCPPUTEST_EXTENSIONS=OFF
            -DCPPUTEST_MEM_LEAK_DETECTION_DISABLED=OFF
            -DPROJECT_IS_TOP_LEVEL=OFF
        BUILD_COMMAND $(MAKE)
        INSTALL_COMMAND ""  # No install step - use build directory directly
        STEP_TARGETS build
    )
    
    # Create imported target for CppUTest (will exist after build)
    add_library(CppUTest::CppUTest STATIC IMPORTED GLOBAL)
    set_target_properties(CppUTest::CppUTest PROPERTIES
        IMPORTED_LOCATION "${CPPUTEST_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CppUTest${CMAKE_STATIC_LIBRARY_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/deps/cpputest/include;${CPPUTEST_BUILD_DIR}"
    )
    add_dependencies(CppUTest::CppUTest CppUTest_build)
    
    # Create imported target for CppUTestExt
    add_library(CppUTest::CppUTestExt STATIC IMPORTED GLOBAL)
    set_target_properties(CppUTest::CppUTestExt PROPERTIES
        IMPORTED_LOCATION "${CPPUTEST_EXT_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}CppUTestExt${CMAKE_STATIC_LIBRARY_SUFFIX}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/deps/cpputest/include;${CPPUTEST_BUILD_DIR}"
    )
    add_dependencies(CppUTest::CppUTestExt CppUTest_build)
    
    set(CppUTest_FOUND TRUE)
endif()

# OpenSSL (system library - required for SSL/TLS support)
find_package(OpenSSL REQUIRED)

# ============================================================================
# Source files
# ============================================================================

set(H323P_SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
    src/core/call_manager.cpp
    src/core/call.cpp
    src/core/endpoint.cpp
    src/utils/logging.cpp
    src/utils/timer.cpp
    src/utils/utils.cpp
    # Stage 2 H.323 module
    src/h323/h323_endpoint.cpp
    src/h323/h323_event_handler.cpp
    src/h323/h323_errors.cpp
    src/h323/q931.cpp
    # src/h323/q931_logger.cpp  # Temporarily disabled - H323Plus dependency
    src/h323/ras.cpp
    src/h323/gatekeeper_manager.cpp
)

set(H323P_HEADERS
    src/cli/cli_parser.hpp
    src/cli/commands.hpp
    src/core/call_manager.hpp
    src/core/call.hpp
    src/core/endpoint.hpp
    src/utils/logging.hpp
    src/utils/timer.hpp
    src/utils/utils.hpp
    # Stage 2 H.323 module
    src/h323/h323_endpoint.hpp
    src/h323/h323_event_handler.hpp
    src/h323/h323_errors.hpp
    src/h323/q931.hpp
    src/h323/q931_logger.hpp
    src/h323/ras.hpp
    src/h323/gatekeeper_manager.hpp
    src/h323/call_states.hpp
)

# ============================================================================
# Main executable
# ============================================================================

add_executable(h323p ${H323P_SOURCES} ${H323P_HEADERS})

# Include directories
target_include_directories(h323p PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${PTLIB_DIR}/include
    ${PTLIB_DIR}/src/ptlib/unix
)

# Link libraries
target_link_libraries(h323p PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    CLI11::CLI11
    pugixml-static
)

# H323Plus (optional for Stage 2+)
if(USE_H323PLUS)
    message(STATUS "H323Plus support enabled (Stage 2+)")
    # Note: H323Plus requires manual build via ./configure
    # See INSTALL.md for instructions
endif()

# Compiler definitions
target_compile_definitions(h323p PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
)

# Compiler flags
target_compile_options(h323p PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3>
)

# ============================================================================
# Tests
# ============================================================================

if(BUILD_TESTS AND CppUTest_FOUND)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# Installation
# ============================================================================

install(TARGETS h323p DESTINATION bin)
install(DIRECTORY scenarios/ DESTINATION share/h323p/scenarios)

# ============================================================================
# Documentation (optional)
# ============================================================================

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
        doxygen_add_docs(docs ${CMAKE_SOURCE_DIR}/src)
    endif()
endif()

# ============================================================================
# Summary
# ============================================================================

message(STATUS "")
message(STATUS "h323p ${PROJECT_VERSION} configuration summary:")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build tests:       ${BUILD_TESTS}")
message(STATUS "  H323Plus support:  ${USE_H323PLUS}")
message(STATUS "  Submodules:")
message(STATUS "    PTLib:           ${PTLIB_DIR}")
message(STATUS "    H323Plus:        ${CMAKE_SOURCE_DIR}/deps/h323plus")
message(STATUS "    CppUTest:        ${CMAKE_SOURCE_DIR}/deps/cpputest")
message(STATUS "    CLI11:           ${CMAKE_SOURCE_DIR}/deps/cli11")
message(STATUS "    pugixml:         ${CMAKE_SOURCE_DIR}/deps/pugixml")
message(STATUS "")
