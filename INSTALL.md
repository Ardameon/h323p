# Installation Guide

This guide describes how to build and install h323p from source.

## Prerequisites

### Build Tools

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git
```

**Fedora/RHEL:**
```bash
sudo dnf install -y gcc-c++ cmake git
```

**macOS:**
```bash
xcode-select --install
brew install cmake
```

### Required Libraries

**Ubuntu/Debian:**
```bash
sudo apt-get install -y libssl-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install -y openssl-devel
```

**macOS:**
```bash
brew install openssl
```

### Optional Dependencies

**For testing (CppUTest):**
```bash
# Ubuntu/Debian
sudo apt-get install -y libcpputest-dev

# Or build from source
git clone https://github.com/cpputest/cpputest.git
cd cpputest
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
sudo make install
```

**For CLI parsing (CLI11):**
```bash
# Ubuntu/Debian (if available)
sudo apt-get install -y libcli11-dev

# Or install via package manager
# macOS
brew install cli11
```

**For XML parsing (pugixml):**
```bash
# Ubuntu/Debian
sudo apt-get install -y libpugixml-dev
```

## Building H323Plus (Optional - for Stage 2+)

H323Plus is required for full H.323 functionality (Stage 2+).

```bash
# Clone PTLib
git clone https://github.com/willamowius/ptlib.git
cd ptlib
./configure --enable-ipv6 --disable-odbc --disable-sdl --disable-lua
make debugnoshared
export PTLIBDIR=$(pwd)

# Clone H323Plus
cd ..
git clone https://github.com/willamowius/h323plus.git
cd h323plus
./configure --enable-h235 --enable-h46017 --enable-h46019m
make debugnoshared
export OPENH323DIR=$(pwd)
```

## Building h323p

### Clone the Repository

```bash
git clone https://github.com/Ardameon/h323p.git
cd h323p
```

### Configure with CMake

**Basic build (Stage 1 - no H323Plus):**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

**Full build (with H323Plus):**
```bash
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DH323PLUS_DIR=$OPENH323DIR \
    -DUSE_H323PLUS=ON
```

**With tests:**
```bash
cmake .. -DBUILD_TESTS=ON
```

**With documentation:**
```bash
cmake .. -DBUILD_DOCS=ON
```

### Build

```bash
cmake --build . -j$(nproc)
```

Or:
```bash
make -j$(nproc)
```

### Install (Optional)

```bash
sudo make install
```

This installs:
- Binary to `/usr/local/bin/h323p`
- Scenarios to `/usr/local/share/h323p/scenarios/`

## Running Tests

```bash
# Run all tests
ctest --output-on-failure

# Run specific test group
ctest -R UtilsTest --output-on-failure

# Run with verbose output
ctest -V
```

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

## Troubleshooting

### CMake cannot find H323Plus

Make sure you have set the correct path:
```bash
cmake .. -DH323PLUS_DIR=/path/to/h323plus
```

Or set the environment variable:
```bash
export OPENH323DIR=/path/to/h323plus
```

### Build fails with C++ errors

Ensure you have a C++17 compatible compiler:
```bash
g++ --version  # Should be 7.0+ or equivalent
```

### Tests fail to run

Make sure CppUTest is installed and CMake found it:
```bash
cmake .. -DBUILD_TESTS=ON
# Check output for "CppUTest found" message
```

## Next Steps

After successful installation:
1. Read [STAGE1.md](STAGE1.md) for Stage 1 features
2. Check [README.md](README.md) for usage examples
3. See [ARCH_PROJECT.md](ARCH_PROJECT.md) for architecture details

---

*Last updated: 2026-03-16*
