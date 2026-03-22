# Installation Guide

This guide describes how to build and install h323p from source.

## Prerequisites

### Build Tools

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libssl-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install -y gcc-c++ cmake git openssl-devel
```

**macOS:**
```bash
xcode-select --install
brew install cmake openssl
```

> **Note:** The only system dependency is OpenSSL. All other dependencies (CppUTest, CLI11, pugixml, PTLib, H323Plus) are built from git submodules automatically during the build process.

---

## Quick Start

### Clone with Submodules

```bash
# Clone repository with all submodules
git clone --recurse-submodules https://github.com/Ardameon/h323p.git
cd h323p
```

Or for existing repository:
```bash
git clone https://github.com/Ardameon/h323p.git
cd h323p
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

### Build with Tests

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j$(nproc)
```

### Run Tests

```bash
# Run all tests
ctest --output-on-failure

# Run with verbose output
ctest -V
```

---

## Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | Build type (`Debug` or `Release`) |
| `BUILD_TESTS` | `ON` | Build unit tests |
| `BUILD_DOCS` | `OFF` | Build documentation (requires Doxygen) |
| `USE_H323PLUS` | `OFF` | Enable H323Plus support (Stage 2+) |

### Example Configurations

**Debug build with tests:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
```

**Release build without tests:**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
```

---

## H323Plus Support (Stage 2+)

H323Plus is required for full H.323 functionality (Stage 2+). The submodule is included but requires manual build.

### Build PTLib

```bash
cd deps/ptlib
./configure --enable-ipv6 --disable-odbc --disable-sdl --disable-lua
make debugnoshared
export PTLIBDIR=$(pwd)
```

### Build H323Plus

```bash
cd ../h323plus
./configure --enable-h235 --enable-h46017 --enable-h46019m
make debugnoshared
export OPENH323DIR=$(pwd)
```

### Build h323p with H323Plus

```bash
cd ../../
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DUSE_H323PLUS=ON \
    -DH323PLUS_DIR=${OPENH323DIR}
make -j$(nproc)
```

---

## Install (Optional)

```bash
sudo make install
```

This installs:
- Binary to `/usr/local/bin/h323p`
- Scenarios to `/usr/local/share/h323p/scenarios/`

---

## Verification

After building, verify the installation:

```bash
# Check version
./h323p info --version

# Check help
./h323p --help

# Run a test command
./h323p call 192.168.1.100
```

---

## Troubleshooting

### Submodules not initialized

```
FATAL_ERROR: PTLib submodule not found. Run: git submodule update --init --recursive
```

**Solution:**
```bash
git submodule update --init --recursive
```

### CMake cannot find OpenSSL

```
Could NOT find OpenSSL
```

**Solution (Ubuntu/Debian):**
```bash
sudo apt-get install -y libssl-dev
```

**Solution (Fedora/RHEL):**
```bash
sudo dnf install -y openssl-devel
```

**Solution (macOS):**
```bash
brew install openssl
export OPENSSL_ROOT_DIR=$(brew --prefix openssl)
```

### Build fails with C++ errors

Ensure you have a C++17 compatible compiler:
```bash
g++ --version  # Should be 7.0+ or equivalent
```

---

## Submodule Management

### Update all submodules

```bash
git submodule update --remote --recursive
```

### Update specific submodule

```bash
cd deps/cli11
git pull origin main
cd ../..
git add deps/cli11
git commit -m "Update CLI11 submodule"
```

### Check submodule status

```bash
git submodule status
```

---

## Next Steps

After successful installation:
1. Read [STAGE1.md](STAGE1.md) for Stage 1 features
2. Check [README.md](README.md) for usage examples
3. See [ARCH_PROJECT.md](ARCH_PROJECT.md) for architecture details

---

*Last updated: 2026-03-22*
