# H.323 Test Utility (h323p)

[![Build Status](https://github.com/Ardameon/h323p/workflows/Build/badge.svg)](https://github.com/Ardameon/h323p/actions)
[![License: MPL](https://img.shields.io/badge/License-MPL-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)
[![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)](https://github.com/Ardameon/h323p/releases)

**h323p** — H.323 call testing utility similar to SIPP for SIP.

## ✨ Features

- 📞 **Load Testing** — 1000+ simultaneous calls
- 🎭 **Fuzzing Testing** — RTP, H.225, H.245
- 📹 **Video Support** — H.261, H.263, H.239 presentations
- 🔒 **TLS Encryption** — Secure signaling
- 📊 **Statistics & Reports** — JSON, CSV, HTML
- 📜 **XML Scenarios** — Flexible test descriptions

## 🚀 Quick Start

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libssl-dev libcpputest-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install -y gcc-c++ cmake git openssl-devel cpputest-devel
```

**macOS:**
```bash
xcode-select --install
brew install cmake openssl cpputest
```

> **Note:** The only system dependencies are OpenSSL and CppUTest. All other dependencies (CLI11, pugixml, PTLib, H323Plus) are built from git submodules.

### Clone with Submodules

```bash
# Clone repository with all submodules
git clone --recurse-submodules https://github.com/Ardameon/h323p.git
cd h323p
```

Or for existing repository:
```bash
git submodule update --init --recursive
```

### Build

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)
```

### First Run

```bash
# Basic call
./h323p call 192.168.1.100

# Load testing
./h323p load 192.168.1.100 -c 100 -r 10

# Listen mode
./h323p listen

# Information
./h323p info --version
```

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [INSTALL.md](INSTALL.md) | Installation instructions |
| [STAGE1.md](STAGE1.md) | Stage 1: Infrastructure |
| [ARCH_PROJECT.md](ARCH_PROJECT.md) | Full architecture document |

## 📋 Commands

| Command | Description |
|---------|-------------|
| `call` | Initiate an H.323 call |
| `listen` | Listen mode for incoming calls |
| `load` | Load testing |
| `fuzz` | Fuzzing testing |
| `register` | Register with Gatekeeper |
| `info` | System information |

## 📊 Project Status

**Phase:** ✅ Stage 1 Completed
**Version:** 0.1.0

### Implementation Progress

| Stage | Description | Status |
|-------|-------------|--------|
| 1 | Infrastructure (CMake, CLI, logging, tests) | ✅ **Completed** |
| 2 | H.323 Stack (H.225 RAS/Q.931) | ⏳ Pending |
| 3 | H.245 and Media (RTP, codecs) | ⏳ Pending |
| 4 | XML Scenarios | ⏳ Pending |
| 5 | Load Testing | ⏳ Pending |
| 6 | Fuzzing and Extensions | ⏳ Pending |
| 7 | Documentation and Release | ⏳ Pending |

### Stage 1 Features

- ✅ CMake build system with git submodules
- ✅ CLI parser with 6 commands (call, listen, load, fuzz, register, info)
- ✅ Async logging system with background thread
- ✅ Timer utilities (ElapsedTimer, CountdownTimer)
- ✅ String and file utilities
- ✅ Core stubs for Stage 2 (CallManager, Call, Endpoint)
- ✅ Unit tests with CppUTest
- ✅ Integration tests
- ✅ Graceful Shutdown handler (SIGINT/SIGTERM)
- ✅ CI/CD workflow (GitHub Actions)

## 🔧 Dependencies

All dependencies are included as git submodules (except OpenSSL and CppUTest):

| Dependency | Purpose | Source |
|------------|---------|--------|
| **OpenSSL** | SSL/TLS support | System package |
| **CppUTest** | Unit testing | System package (*) |
| **CLI11** | CLI parsing | `deps/cli11/` |
| **pugixml** | XML parsing | `deps/pugixml/` |
| **PTLib** | H.323Plus dependency | `deps/ptlib/` |
| **H323Plus** | H.323 stack (Stage 2+) | `deps/h323plus/` |

(*) CppUTest uses system package due to C++17 compatibility issues.

See [INSTALL.md](INSTALL.md) for detailed build instructions.

## 🧪 Testing

```bash
# Build with tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Run specific test group
./tests/h323p_tests -g UtilsTest
```

## 📝 License

Distributed under the [MPL-2.0 License](LICENSE).

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📞 Support

- Issues: https://github.com/Ardameon/h323p/issues
- Repository: https://github.com/Ardameon/h323p

---

*Version: 0.1.0 | Updated: 2026-03-22*
