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
option(USE_H323PLUS "Use H323Plus library" OFF)  # Disabled for Stage 1

# Submodule options
option(BUILD_H323PLUS_FROM_SUBMODULE "Build H323Plus from git submodule" OFF)
option(BUILD_CPPUTEST_FROM_SUBMODULE "Build CppUTest from git submodule" OFF)
option(BUILD_CLI11_FROM_SUBMODULE "Build CLI11 from git submodule" OFF)
option(BUILD_PUGIXML_FROM_SUBMODULE "Build pugixml from git submodule" OFF)

# Module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ============================================================================
# Dependencies from submodules
# ============================================================================

# H323Plus + PTLib from submodule
if(BUILD_H323PLUS_FROM_SUBMODULE)
    message(STATUS "Building H323Plus from submodule...")
    
    # Build PTLib first (required by H323Plus)
    set(PTLIB_DIR "${CMAKE_SOURCE_DIR}/deps/ptlib")
    set(PWLIB_DIR "${PTLIB_DIR}")  # Legacy name
    
    # PTLib include directories
    set(PTLIB_INCLUDE_DIRS 
        "${PTLIB_DIR}/include"
        "${PTLIB_DIR}/src/ptlib/unix"
    )
    
    # PTLib library
    set(PTLIB_LIBRARIES "${PTLIB_DIR}/lib/libptlib.a")
    set(PTLIB_FOUND TRUE)
    
    # H323Plus include directories
    set(H323PLUS_INCLUDE_DIRS 
        "${H323PLUS_DIR}/include"
        "${PTLIB_INCLUDE_DIRS}"
    )
    
    # H323Plus library
    set(H323PLUS_LIBRARIES "${H323PLUS_DIR}/lib/libh323_linux_x86_64.a")
    set(H323PLUS_FOUND TRUE)
    
    set(USE_H323PLUS ON)
    message(STATUS "H323Plus submodule configured")
    message(STATUS "  PTLib: ${PTLIB_DIR}")
    message(STATUS "  H323Plus: ${H323PLUS_DIR}")
endif()

# CppUTest from submodule
if(BUILD_CPPUTEST_FROM_SUBMODULE)
    message(STATUS "Building CppUTest from submodule...")
    add_subdirectory(deps/cpputest)
    set(CppUTest_FOUND TRUE)
endif()

# CLI11 from submodule
if(BUILD_CLI11_FROM_SUBMODULE)
    message(STATUS "Building CLI11 from submodule...")
    add_subdirectory(deps/cli11)
endif()

# pugixml from submodule
if(BUILD_PUGIXML_FROM_SUBMODULE)
    message(STATUS "Building pugixml from submodule...")
    add_subdirectory(deps/pugixml)
endif()

# ============================================================================
# Dependencies (system or submodule)
# ============================================================================

# H323Plus (optional for Stage 1)
if(USE_H323PLUS AND NOT BUILD_H323PLUS_FROM_SUBMODULE)
    find_package(H323Plus REQUIRED)
    find_package(PTLib REQUIRED)
endif()

# Required dependencies
find_package(OpenSSL REQUIRED)

# Optional dependencies (if not using submodules)
if(NOT BUILD_CLI11_FROM_SUBMODULE)
    find_package(CLI11 CONFIG)
endif()

find_package(spdlog CONFIG)

if(NOT BUILD_PUGIXML_FROM_SUBMODULE)
    find_package(pugixml CONFIG)
endif()

# CppUTest (for tests) - workaround for Ubuntu package bug
# Ubuntu's libcpputest-dev has broken cmake config, so we find it manually
if(BUILD_TESTS AND NOT BUILD_CPPUTEST_FROM_SUBMODULE)
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
        message(STATUS "Found CppUTest: ${CPPUTEST_LIBRARY}")
    else()
        message(WARNING "CppUTest not found, tests disabled")
        set(BUILD_TESTS OFF)
    endif()
endif()

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
)

# Link libraries
target_link_libraries(h323p PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Optional linking
if(USE_H323PLUS AND H323PLUS_FOUND)
    target_include_directories(h323p PRIVATE
        ${H323PLUS_INCLUDE_DIRS}
        ${PTLIB_INCLUDE_DIRS}
    )
    target_link_libraries(h323p PRIVATE
        ${H323PLUS_LIBRARIES}
        ${PTLIB_LIBRARIES}
    )
endif()

if(CLI11_FOUND)
    target_link_libraries(h323p PRIVATE CLI11::CLI11)
else()
    message(STATUS "CLI11 not found, using internal parser")
endif()

if(spdlog_FOUND)
    target_link_libraries(h323p PRIVATE spdlog::spdlog)
endif()

if(pugixml_FOUND)
    target_link_libraries(h323p PRIVATE pugixml::pugixml)
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
else()
    message(STATUS "CppUTest not found, tests disabled")
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
