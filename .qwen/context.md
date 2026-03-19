# h323p — H.323 Test Utility

## Project Status

- **Phase:** Stage 2 Documentation Completed ✅
- **Version:** 0.2.0
- **Last updated:** 2026-03-19
- **Repository:** https://github.com/Ardameon/h323p

## Description

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
| **CLI output** | English | `"Starting load test"`, `"Total calls: 100"` |
| **Documentation** | Russian | Explanations, descriptions |

---

## Project Structure

```
h323p/
├── src/
│   ├── cli/           # CLI parser, commands ✅
│   ├── core/          # Call Manager, Call (stubs) ✅
│   ├── h323/          # H323Plus wrapper (Stage 2)
│   ├── media/         # RTP, codecs, generator (Stage 3)
│   ├── scenario/      # XML parser, interpreter (Stage 4)
│   ├── load/          # Load testing (Stage 5)
│   ├── fuzz/          # Fuzzing (RTP, H.225) (Stage 6)
│   ├── stats/         # Statistics, CDR, reports (Stage 5)
│   └── utils/         # Logging, timers, utilities ✅
├── tests/             # Unit/integration tests ✅
├── scenarios/         # XML examples
└── docs/              # Documentation
```

---

## Documentation

| File | Description | Lines |
|------|-------------|-------|
| `STAGE1.md` | Infrastructure (CMake, CLI, logging) | ~1700 |
| `STAGE2.md` | H.323 Stack (H.225, Call Manager) | ~3140 |
| `STAGE3.md` | H.245 and Media (RTP, codecs) | ~1900 |
| `STAGE4.md` | XML Scenarios (parser, interpreter) | ~800 |
| `STAGE5.md` | Load Testing | ~700 |
| `STAGE6.md` | Fuzzing, Video, H.239, TLS | ~750 |
| `STAGE7.md` | Documentation and Release | ~650 |
| `ARCH_PROJECT.md` | Full architectural document | ~1450 |
| **Total** | | **~11,090 lines** |

---

## Key Architectural Decisions

### Critical (must implement)

| # | Decision | Stage | Status |
|---|----------|-------|--------|
| 1 | H323Plus Error Handling | 2 | ⏳ Pending |
| 2 | Call Manager Thread Safety | 2 | ⏳ Pending |
| 3 | PSmartPointer for H323Plus | 2 | ⏳ Pending |
| 4 | Password Protection in CLI | 2 | ⏳ Pending |
| 5 | Rate Limiting for Listen | 2 | ⏳ Pending |

### Important (improve stability)

| # | Decision | Stage | Status |
|---|----------|-------|--------|
| 6 | Async Logging | 1 | ✅ Implemented |
| 7 | Integration Tests | 1-7 | ✅ Implemented |
| 8 | Jitter Buffer with Sorting | 3 | ⏳ Pending |
| 9 | Thread Pool for Load | 5 | ⏳ Pending |
| 10 | Graceful Shutdown | 1 | ✅ Implemented |

### Useful (before release)

| # | Decision | Stage | Status |
|---|----------|-------|--------|
| 11 | PCAP Export | 3 | ⏳ Pending |
| 12 | Prometheus Metrics | 5 | ⏳ Pending |
| 13 | H.460 Support | 6 | ⏳ Pending |

---

## Implementation Plan

| Stage | Duration | Status |
|-------|----------|--------|
| 1. Infrastructure | 2-3 weeks | ✅ **Completed** |
| 2. H.323 Stack (H.225) | 3-4 weeks | 📋 **Documentation Ready** |
| 3. H.245 and Media | 2-3 weeks | ⏳ Pending |
| 4. XML Scenarios | 2 weeks | ⏳ Pending |
| 5. Load Testing | 2 weeks | ⏳ Pending |
| 6. Fuzzing and Extensions | 1-2 weeks | ⏳ Pending |
| 7. Documentation and Release | 1 week | ⏳ Pending |
| **Total** | **15-19 weeks** | |

---

## Quick Commands

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
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make -j4
ctest --output-on-failure
./tests/h323p_tests
```

---

## Stage 1 Completed Features

- [x] CMake build system with H323Plus/PTLib support
- [x] CLI parser with 6 commands (call, listen, load, fuzz, register, info)
- [x] Async logging system with background thread
- [x] Timer utilities (ElapsedTimer, CountdownTimer)
- [x] String and file utilities
- [x] Core stubs for Stage 2 (CallManager, Call, Endpoint)
- [x] Unit tests with CppUTest (42 tests passing)
- [x] Integration tests
- [x] Graceful Shutdown handler (SIGINT/SIGTERM)
- [x] CI/CD workflow (GitHub Actions)

## 📝 Stage 2 Documentation Completed

- [x] STAGE2.md expanded with 4 new subsections (+778 lines)
- [x] 2.1.1 Async H323Plus Events (H323EventHandler class)
- [x] 2.2.1 Operation Timeouts (H323TimeoutConfig, executeWithTimeout)
- [x] 2.2.2 Retry Logic with Exponential Backoff (RetryConfig, retryWithBackoff)
- [x] 2.3.1 Q.931 Information Elements Logging (Q931Logger class)
- [x] Updated completion checklist (13 criteria)

---

## Resources

- **H323Plus:** https://www.h323plus.org/
- **callgen323:** https://github.com/willamowius/callgen323
- **SIPP:** https://github.com/SIPp/sipp
- **CppUTest:** https://cpputest.github.io/

---

## Next Steps

**Start Stage 2 Implementation (H.323 Stack)**

1. Implement H323Plus wrapper with error handling
2. Implement H.225 RAS (Gatekeeper registration)
3. Implement H.225 Q.931 (call signaling)
4. Implement Call Manager with thread safety
5. Implement `call` and `listen` commands

---

*Version: 0.2.0 | Updated: 2026-03-19*
