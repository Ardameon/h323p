# Tests CMakeLists.txt

# Check if CppUTest is available
if(NOT CppUTest_FOUND)
    message(WARNING "Tests require CppUTest but it was not found")
    return()
endif()

# Create library with source files for testing
add_library(h323p_test_lib STATIC
    ${CMAKE_SOURCE_DIR}/src/utils/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/timer.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/logging.cpp
    ${CMAKE_SOURCE_DIR}/src/cli/cli_parser.cpp
)

target_include_directories(h323p_test_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(h323p_test_lib PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
)

# Test executable
add_executable(h323p_tests
    test_main.cpp
    test_utils.cpp
    test_timer.cpp
    test_logging.cpp
    test_cli_parser.cpp
    test_integration.cpp
)

# Include directories
target_include_directories(h323p_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CPPUTEST_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(h323p_tests
    CppUTest::CppUTest
    h323p_test_lib
)

# Register tests with CTest
add_test(NAME UtilsTest COMMAND h323p_tests -g UtilsTest)
add_test(NAME TimerTest COMMAND h323p_tests -g TimerTest)
add_test(NAME LoggingTest COMMAND h323p_tests -g LoggingTest)
add_test(NAME CliParserTest COMMAND h323p_tests -g CliParserTest)
add_test(NAME IntegrationTest COMMAND h323p_tests -g IntegrationTest)

# Set test properties
set_tests_properties(UtilsTest TimerTest LoggingTest CliParserTest IntegrationTest
    PROPERTIES
    TIMEOUT 30
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
