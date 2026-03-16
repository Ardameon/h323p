# Stage 1 Expectations — Infrastructure

**Stage:** 1 of 7  
**Title:** Basic Infrastructure  
**Duration:** 2-3 weeks  
**Status:** 📋 Ready for implementation

---

## 📋 Overview

This document describes the expected outcomes from Stage 1 (Infrastructure) implementation. Upon completion, the project should have a working foundation for all subsequent stages.

---

## ✅ Expected Results

### 1. Project Structure Created

**Expected:**
```
h323p/
├── CMakeLists.txt              ✅ Main build configuration
├── README.md                   ✅ Project overview
├── LICENSE                     ✅ MPL-2.0 license
├── .gitignore                  ✅ Git ignore rules
├── .github/
│   └── workflows/
│       └── build.yml           ✅ CI/CD pipeline
├── cmake/
│   ├── FindH323Plus.cmake      ✅ H323Plus finder module
│   └── FindPTLib.cmake         ✅ PTLib finder module
├── src/
│   ├── main.cpp                ✅ Application entry point
│   ├── cli/
│   │   ├── cli_parser.hpp      ✅ CLI parser headers
│   │   ├── cli_parser.cpp      ✅ CLI parser implementation
│   │   └── commands.hpp        ✅ Command definitions
│   ├── core/
│   │   ├── call_manager.hpp    ✅ Call Manager stub
│   │   ├── call_manager.cpp
│   │   ├── call.hpp            ✅ Call class stub
│   │   ├── call.cpp
│   │   ├── endpoint.hpp        ✅ Endpoint stub
│   │   └── endpoint.cpp
│   ├── h323/
│   │   └── (empty for Stage 2)
│   ├── media/
│   │   └── (empty for Stage 2)
│   ├── scenario/
│   │   └── (empty for Stage 4)
│   ├── load/
│   │   └── (empty for Stage 5)
│   ├── fuzz/
│   │   └── (empty for Stage 6)
│   ├── stats/
│   │   └── (empty for Stage 5)
│   └── utils/
│       ├── logging.hpp         ✅ Logging system
│       ├── logging.cpp
│       ├── timer.hpp           ✅ Timer utilities
│       ├── timer.cpp
│       ├── utils.hpp           ✅ String/file utilities
│       └── utils.cpp
├── tests/
│   ├── CMakeLists.txt          ✅ Test build configuration
│   ├── test_cli_parser.cpp     ✅ CLI parser tests
│   ├── test_logging.cpp        ✅ Logging tests
│   ├── test_timer.cpp          ✅ Timer tests
│   └── test_utils.cpp          ✅ Utilities tests
└── scenarios/
    └── (empty for Stage 4)
```

**Acceptance Criteria:**
- [ ] All directories created
- [ ] All placeholder files created
- [ ] File structure matches design

---

### 2. CMake Build System Working

**Expected:**
```bash
$ mkdir build && cd build
$ cmake .. -DH323PLUS_DIR=/path/to/h323plus
-- Configuring done
-- Generating done
-- Build files have been written to: /path/to/build

$ make -j4
[ 10%] Building CXX object CMakeFiles/h323p.dir/src/main.cpp.o
[ 20%] Building CXX object CMakeFiles/h323p.dir/src/cli/cli_parser.cpp.o
...
[100%] Linking CXX executable h323p
[100%] Built target h323p

$ ./h323p --version
h323p version 0.1.0
H.323 Test Utility
Based on H323Plus + PTLib
```

**Acceptance Criteria:**
- [ ] CMake configuration completes without errors
- [ ] Project builds successfully with `make`
- [ ] Executable `h323p` is created
- [ ] `--version` command works
- [ ] `--help` command works

---

### 3. CLI Parser Functional

**Expected:**
```bash
# Help output
$ ./h323p --help

H.323 Test Utility (h323p) v0.1.0

Usage:
  h323p <command> [options]

Commands:
  call       Initiate a call
  listen     Listen mode for incoming calls
  load       Load testing
  fuzz       Fuzzing testing
  register   Register with Gatekeeper
  info       System information

Examples:
  h323p call 192.168.1.100
  h323p load 192.168.1.100 -c 100 -r 10
  h323p listen -m 50

# Unknown command
$ ./h323p unknown
Error: Unknown command 'unknown'
Use './h323p --help' for more information.
```

**Acceptance Criteria:**
- [ ] `--help` displays usage information
- [ ] `--version` displays version
- [ ] Unknown commands show error message
- [ ] All 6 commands recognized (call, listen, load, fuzz, register, info)
- [ ] Global options parsed correctly (--log-file, --log-level, --quiet)

---

### 4. Logging System Operational

**Expected:**
```bash
# Console logging (default)
$ ./h323p call 192.168.1.100
2026-03-16 10:00:00.123 [INFO] Starting h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Command: call
2026-03-16 10:00:00.125 [INFO] Destination: 192.168.1.100

# File logging
$ ./h323p call 192.168.1.100 --log-file h323p.log --log-level debug
$ cat h323p.log
2026-03-16 10:00:00.123 [DEBUG] Initializing logger
2026-03-16 10:00:00.124 [INFO] Starting h323p v0.1.0
2026-03-16 10:00:00.125 [DEBUG] CLI parsed successfully

# Quiet mode
$ ./h323p call 192.168.1.100 --quiet
# (no output)

# Log levels
$ ./h323p call 192.168.1.100 --log-level debug
2026-03-16 10:00:00.123 [DEBUG] Debug message
2026-03-16 10:00:00.124 [INFO] Info message
2026-03-16 10:00:00.125 [WARN] Warning message
2026-03-16 10:00:00.126 [ERROR] Error message
```

**Acceptance Criteria:**
- [ ] Log messages include timestamp, level, and message
- [ ] Console output works (stdout/stderr)
- [ ] File output works (--log-file)
- [ ] Log levels work (debug, info, warn, error)
- [ ] Quiet mode suppresses output (--quiet)
- [ ] Async logging implemented (non-blocking)

---

### 5. Timer Utilities Working

**Expected:**
```cpp
// ElapsedTimer test
ElapsedTimer timer;
std::this_thread::sleep_for(std::chrono::milliseconds(100));
auto elapsed = timer.elapsed();
assert(elapsed.count() >= 90);  // With tolerance

// CountdownTimer test
CountdownTimer timer;
bool callbackCalled = false;
timer.start(std::chrono::milliseconds(100), [&callbackCalled]() {
    callbackCalled = true;
});
std::this_thread::sleep_for(std::chrono::milliseconds(150));
assert(callbackCalled == true);
```

**Acceptance Criteria:**
- [ ] `ElapsedTimer` measures elapsed time
- [ ] `CountdownTimer` triggers callback after duration
- [ ] Timer stop/reset functions work
- [ ] Thread-safe timer operations

---

### 6. Utility Functions Implemented

**Expected:**
```cpp
// String utilities
utils::trim("  hello  ") == "hello"
utils::split("a,b,c", ',') == ["a", "b", "c"]
utils::join(["a", "b", "c"], ",") == "a,b,c"
utils::toLower("HELLO") == "hello"
utils::toUpper("hello") == "HELLO"
utils::startsWith("hello world", "hello") == true
utils::endsWith("hello world", "world") == true

// Address parsing
utils::parseAddress("192.168.1.1:1720") == Address{"192.168.1.1", 1720, "tcp"}
utils::parseAddress("invalid") == nullopt

// File utilities
utils::fileExists("/etc/passwd") == true
utils::fileExists("/nonexistent") == false
utils::writeFile("/tmp/test.txt", "content") == true
utils::readFile("/tmp/test.txt") == "content"
```

**Acceptance Criteria:**
- [ ] All string utilities implemented
- [ ] Address parsing works (host:port format)
- [ ] File utilities work (exists, read, write)
- [ ] All utility functions tested

---

### 7. Unit Tests Passing

**Expected:**
```bash
$ ctest --output-on-failure
Test project /path/to/build
    Start 1: UtilsTest.Trim
1/4 Test #1: UtilsTest.Trim ...................   Passed    0.00 sec
    Start 2: TimerTest.ElapsedTimer
2/4 Test #2: TimerTest.ElapsedTimer ...........   Passed    0.15 sec
    Start 3: LoggingTest.LogLevelConversion
3/4 Test #3: LoggingTest.LogLevelConversion ...   Passed    0.00 sec
    Start 4: CliParserTest.HelpOutput
4/4 Test #4: CliParserTest.HelpOutput .........   Passed    0.00 sec

100% tests passed!
```

**Acceptance Criteria:**
- [ ] All unit tests written (CppUTest)
- [ ] All tests pass (100%)
- [ ] Test coverage > 80% for Stage 1 code
- [ ] Tests run with `ctest`

---

### 8. Integration Tests Working

**Expected:**
```bash
# Basic integration test
$ ./h323p_tests -g BasicIntegrationTest
Tests running in group BasicIntegrationTest:
....
OK (4 tests, 0 failures)

# CLI integration
$ ./h323p info --version | grep "h323p version"
h323p version 0.1.0
```

**Acceptance Criteria:**
- [ ] Integration tests created
- [ ] CLI commands tested end-to-end
- [ ] All integration tests pass

---

### 9. CI/CD Pipeline Configured

**Expected:**
```yaml
# .github/workflows/build.yml
name: Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install dependencies
        run: sudo apt-get install -y cmake libssl-dev libcpputest-dev
      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build
      - name: Test
        run: ctest --test-dir build --output-on-failure
```

**Acceptance Criteria:**
- [ ] GitHub Actions workflow created
- [ ] Build passes on GitHub Actions
- [ ] Tests run on every push
- [ ] Build badge in README.md

---

### 10. Graceful Shutdown Implemented

**Expected:**
```bash
# Start call (will hang without media)
$ ./h323p call 192.168.1.100
2026-03-16 10:00:00.123 [INFO] Starting h323p v0.1.0
2026-03-16 10:00:00.124 [INFO] Command: call
^C2026-03-16 10:00:05.456 [INFO] Shutdown requested...
2026-03-16 10:00:05.457 [INFO] Cleaning up resources
2026-03-16 10:00:05.458 [INFO] Goodbye!
```

**Acceptance Criteria:**
- [ ] SIGINT (Ctrl+C) handled gracefully
- [ ] SIGTERM handled gracefully
- [ ] Resources cleaned up on exit
- [ ] No memory leaks (valgrind clean)

---

## 📊 Summary Checklist

### Code Deliverables

- [ ] `CMakeLists.txt` — Build configuration
- [ ] `src/main.cpp` — Entry point with graceful shutdown
- [ ] `src/cli/cli_parser.*` — CLI parser implementation
- [ ] `src/utils/logging.*` — Async logging system
- [ ] `src/utils/timer.*` — Timer utilities
- [ ] `src/utils/utils.*` — String/file utilities
- [ ] `src/core/*` — Stub files for Stage 2
- [ ] `tests/*` — Unit and integration tests

### Documentation Deliverables

- [ ] `README.md` — Project overview
- [ ] `INSTALL.md` — Installation instructions
- [ ] Code comments — All public APIs documented
- [ ] `CHANGELOG.md` — Initial entry created

### Quality Metrics

- [ ] Build passes without warnings (`-Wall -Wextra -Wpedantic`)
- [ ] All tests pass (100%)
- [ ] Code coverage > 80%
- [ ] No memory leaks (valgrind)
- [ ] CI/CD pipeline green

---

## 🚀 Next Steps (Stage 2)

Upon successful completion of Stage 1, proceed to Stage 2:

**Stage 2: H.323 Stack (H.225)**
- Implement H323Plus wrapper
- Implement H.225 RAS (Gatekeeper registration)
- Implement H.225 Q.931 (call signaling)
- Implement Call Manager
- Implement `call` and `listen` commands

---

*Document Version: 1.0*  
*Created: 2026-03-16*  
*Repository: https://github.com/Ardameon/h323p*
