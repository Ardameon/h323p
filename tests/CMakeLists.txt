# Tests CMakeLists.txt

# Test executable
add_executable(h323p_tests
    test_utils.cpp
    test_timer.cpp
    test_logging.cpp
    test_cli_parser.cpp
    test_integration.cpp
)

# Include directories
target_include_directories(h323p_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(h323p_tests
    CppUTest::CppUTest
    h323p
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
