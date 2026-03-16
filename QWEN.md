# h323p — H.323 Test Utility

## 📌 Project Status

- **Phase:** Design completed (100%)
- **Version:** 1.0.0 (ready for implementation)
- **Last updated:** 2026-03-16
- **Repository:** https://github.com/Ardameon/h323p

## 📖 Description

**h323p** — H.323 call testing utility similar to SIPP for SIP.

### Goals
- Load testing of endpoints/gateways/gatekeepers
- Functional protocol testing
- Fuzzing testing
- Support for audio, video, H.239

### Technical Decisions
| Decision | Choice |
|----------|--------|
| **Language** | C++17 |
| **H.323 Stack** | H323Plus + PTLib |
| **Build** | CMake |
| **Scenarios** | XML (SIPP-like) |
| **Tests** | CppUTest |

### Language Policy
**IMPORTANT:** All code must use **English only**.

| Component | Language | Example |
|-----------|----------|---------|
| **Code comments** | English | `// Logging`, `// Call Manager` |
| **String literals** | English | `"Error"`, `"Call completed"` |
| **Identifiers** | English | `callManager`, `rtpSession` |
| **CLI output** | English | `"Call completed"`, `"Registration failed"` |
| **Documentation** | Russian | Explanations, descriptions |

---

## 📁 Project Structure

```
h323p/
├── src/
│   ├── cli/           # CLI parser, commands
│   ├── core/          # Call Manager, Call
│   ├── h323/          # H323Plus wrapper, RAS, Q.931, H.245
│   ├── media/         # RTP, codecs, generator
│   ├── scenario/      # XML parser, interpreter
│   ├── load/          # Load testing
│   ├── fuzz/          # Fuzzing (RTP, H.225)
│   ├── stats/         # Statistics, CDR, reports
│   └── utils/         # Logging, timers, utilities
├── scenarios/         # XML examples
├── tests/             # Unit/integration tests
└── docs/              # Documentation
```

---

## 📚 Documentation (in h323_test_util/)

| File | Description | Lines |
|------|-------------|-------|
| `STAGE1.md` | Infrastructure (CMake, CLI, logging) | ~1700 |
| `STAGE2.md` | H.323 Stack (H.225, Call Manager) | ~2400 |
| `STAGE3.md` | H.245 and Media (RTP, codecs) | ~1900 |
| `STAGE4.md` | XML Scenarios (parser, interpreter) | ~800 |
| `STAGE5.md` | Load Testing | ~700 |
| `STAGE6.md` | Fuzzing, Video, H.239, TLS | ~750 |
| `STAGE7.md` | Documentation and Release | ~650 |
| `ARCH_PROJECT.md` | Full architectural document | ~1450 |
| **Total** | | **~10,350 lines** |

---

## 🔧 Key Architectural Decisions

### Critical (must implement)

| # | Decision | Stage | Description |
|---|----------|-------|-------------|
| 1 | H323Plus Error Handling | 2 | Map error codes to H323Error |
| 2 | Call Manager Thread Safety | 2 | `recursive_mutex` for all methods |
| 3 | PSmartPointer for H323Plus | 2 | Auto memory management |
| 4 | Password Protection in CLI | 2 | `--password-prompt`, `--password-file` |
| 5 | Rate Limiting for Listen | 2 | DDoS protection via calls |

### Important (improve stability)

| # | Decision | Stage | Description |
|---|----------|-------|-------------|
| 6 | Async Logging | 1 | Non-blocking file write |
| 7 | Integration Tests | 1-7 | Tests for each stage |
| 8 | Jitter Buffer with Sorting | 3 | RTP buffer with ordering |
| 9 | Thread Pool for Load | 5 | Instead of 1000 threads |
| 10 | Graceful Shutdown | 1 | Correct call completion |

### Useful (before release)

| # | Decision | Stage | Description |
|---|----------|-------|-------------|
| 11 | PCAP Export | 3 | Traffic capture for Wireshark |
| 12 | Prometheus Metrics | 5 | Load test monitoring |
| 13 | H.460 Support | 6 | NAT traversal |

---

## 📅 Implementation Plan

| Stage | Duration | Status |
|-------|----------|--------|
| 1. Infrastructure | 2-3 weeks | ✅ Ready |
| 2. H.323 Stack (H.225) | 3-4 weeks | ✅ Ready |
| 3. H.245 and Media | 2-3 weeks | ✅ Ready |
| 4. XML Scenarios | 2 weeks | ✅ Ready |
| 5. Load Testing | 2 weeks | ✅ Ready |
| 6. Fuzzing and Extensions | 1-2 weeks | ✅ Ready |
| 7. Documentation and Release | 1 week | ✅ Ready |
| **Total** | **15-19 weeks** | |

---

## 🚀 Quick Commands

```bash
# Build
mkdir build && cd build
cmake .. -DH323PLUS_DIR=/path/to/h323plus
make -j4

# Run
./h323p call 192.168.1.100          # Basic call
./h323p load 192.168.1.100 -c 100   # Load test
./h323p listen                      # Listen mode
./h323p fuzz 192.168.1.100 -T rtp   # Fuzzing
./h323p info --version              # Information

# Tests
ctest --output-on-failure
```

---

## 🔗 Resources

- **H323Plus:** https://www.h323plus.org/
- **callgen323:** https://github.com/willamowius/callgen323
- **SIPP:** https://github.com/SIPp/sipp
- **CppUTest:** https://cpputest.github.io/

---

## ⏭️ Next Steps

**Start Stage 1 Implementation (Infrastructure)**

1. Create project structure
2. Configure CMakeLists.txt
3. Implement basic CLI
4. Setup async logging (recommendation #6)
5. Add graceful shutdown (recommendation #12)
6. Write integration tests (recommendation #7)

---

*Version: 1.0.0 | Updated: 2026-03-16*
