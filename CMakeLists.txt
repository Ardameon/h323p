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

# Module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ============================================================================
# Dependencies
# ============================================================================

# H323Plus (optional for Stage 1)
if(USE_H323PLUS)
    find_package(H323Plus REQUIRED)
    find_package(PTLib REQUIRED)
endif()

# Required dependencies
find_package(OpenSSL REQUIRED)

# Optional dependencies
find_package(CLI11 CONFIG)
find_package(spdlog CONFIG)
find_package(pugixml CONFIG)
find_package(CppUTest CONFIG)

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
