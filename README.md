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

### Build

```bash
mkdir build && cd build
cmake .. -DH323PLUS_DIR=/path/to/h323plus
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

**Phase:** Stage 1 Implementation (In Progress)
**Version:** 0.1.0

### Implementation Progress

| Stage | Description | Status |
|-------|-------------|--------|
| 1 | Infrastructure (CMake, CLI, logging) | 🔄 In Progress |
| 2 | H.323 Stack (H.225 RAS/Q.931) | ⏳ Pending |
| 3 | H.245 and Media (RTP, codecs) | ⏳ Pending |
| 4 | XML Scenarios | ⏳ Pending |
| 5 | Load Testing | ⏳ Pending |
| 6 | Fuzzing and Extensions | ⏳ Pending |
| 7 | Documentation and Release | ⏳ Pending |

## 🔧 Dependencies

### Required
- C++17 compiler
- CMake 3.16+
- OpenSSL

### Optional (for full functionality)
- H323Plus + PTLib (Stage 2+)
- CppUTest (testing)
- CLI11 (CLI parsing)
- pugixml (XML parsing)

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

*Version: 0.1.0 | Updated: 2026-03-16*
