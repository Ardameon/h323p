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
# Note: Using system package due to C++17 compatibility issues with older CppUTest versions
if(BUILD_TESTS)
    message(STATUS "Searching for system CppUTest...")
    find_path(CPPUTEST_INCLUDE_DIR CppUTest/TestHarness.h
        PATHS /usr/include /usr/local/include)
    find_library(CPPUTEST_LIBRARY NAMES CppUTest cpputest
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)

    if(CPPUTEST_INCLUDE_DIR AND CPPUTEST_LIBRARY)
        set(CppUTest_FOUND TRUE)
        add_library(CppUTest::CppUTest STATIC IMPORTED)
        set_target_properties(CppUTest::CppUTest PROPERTIES
            IMPORTED_LOCATION ${CPPUTEST_LIBRARY}
            INTERFACE_INCLUDE_DIRECTORIES ${CPPUTEST_INCLUDE_DIR})
        message(STATUS "Found system CppUTest: ${CPPUTEST_LIBRARY}")
    else()
        message(WARNING "System CppUTest not found, tests disabled. Install libcpputest-dev")
        set(BUILD_TESTS OFF)
    endif()
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
