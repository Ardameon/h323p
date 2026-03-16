# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Planned
- Stage 2: H.323 Stack (H.225 RAS/Q.931)
- Stage 3: H.245 and Media (RTP, codecs)
- Stage 4: XML Scenarios
- Stage 5: Load Testing
- Stage 6: Fuzzing and Extensions
- Stage 7: Documentation and Release

## [0.1.0] - 2026-03-16

### Added
- **Project Structure**
  - Initial project directory structure
  - CMake build system configuration
  - CMake modules for H323Plus and PTLib detection

- **CLI Parser**
  - Command parsing for: call, listen, load, fuzz, register, info
  - Global options: --log-file, --log-level, --quiet, --verbose, --help
  - Command-specific options parsing

- **Logging System**
  - Async logging with background thread
  - Log levels: DEBUG, INFO, WARN, ERROR
  - Console and file output
  - Singleton Logger class
  - LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR macros

- **Timer Utilities**
  - ElapsedTimer for measuring elapsed time
  - CountdownTimer with callback support
  - Thread-safe operations

- **String and File Utilities**
  - trim, split, join functions
  - toLower, toUpper functions
  - startsWith, endsWith functions
  - Address parsing (host:port/protocol)
  - File operations: exists, read, write

- **Core Stubs (Stage 2 preparation)**
  - CallManager class (singleton)
  - Call class with states
  - Endpoint class (H323Plus wrapper preparation)

- **Testing**
  - CppUTest integration
  - Unit tests for utils, timer, logging, CLI parser
  - Integration tests for CLI commands
  - CTest configuration

- **Graceful Shutdown**
  - Signal handling for SIGINT and SIGTERM
  - Clean resource cleanup on exit
  - Atomic shutdown flag

- **Documentation**
  - README.md with quick start guide
  - INSTALL.md with build instructions
  - CHANGELOG.md
  - Existing stage documentation (STAGE1-7.md)
  - Architecture document (ARCH_PROJECT.md)

- **CI/CD**
  - GitHub Actions workflow for builds
  - Automated testing on push

### Technical Details
- C++17 standard
- CMake 3.16+ build system
- MPL-2.0 license
- Async logging implementation (Recommendation #6)
- Thread-safe timers with mutex protection
- Graceful shutdown implementation (Recommendation #10)

### Known Limitations
- Stage 1 is infrastructure only
- H.323 functionality requires Stage 2 implementation
- H323Plus integration is optional and disabled by default

---

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.1.0 | 2026-03-16 | Initial release (Stage 1 infrastructure) |

---

*Last updated: 2026-03-16*
