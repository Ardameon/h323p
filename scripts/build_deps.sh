#!/bin/bash
#
# Build PTLib and H323Plus from submodules
# Usage: ./scripts/build_deps.sh [debug|release]
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
DEPS_DIR="$PROJECT_DIR/deps"

BUILD_TYPE="${1:-debug}"

echo "============================================"
echo "Building PTLib and H323Plus"
echo "============================================"
echo "Build type: $BUILD_TYPE"
echo "Dependencies directory: $DEPS_DIR"
echo ""

# Check if submodules are initialized
if [ ! -d "$DEPS_DIR/ptlib/.git" ]; then
    echo "ERROR: PTLib submodule not found!"
    echo "Run: git submodule update --init --recursive"
    exit 1
fi

if [ ! -d "$DEPS_DIR/h323plus/.git" ]; then
    echo "ERROR: H323Plus submodule not found!"
    echo "Run: git submodule update --init --recursive"
    exit 1
fi

# Build PTLib
echo "============================================"
echo "Building PTLib..."
echo "============================================"
cd "$DEPS_DIR/ptlib"

case "$BUILD_TYPE" in
    debug)
        CONFIG_OPTS="--enable-debug"
        ;;
    release)
        CONFIG_OPTS="--disable-debug"
        ;;
    *)
        CONFIG_OPTS="--enable-debug"
        ;;
esac

./configure \
    $CONFIG_OPTS \
    --enable-ipv6 \
    --disable-odbc \
    --disable-sdl \
    --disable-lua \
    --disable-jpeg \
    --disable-tiff \
    --disable-alsa \
    --disable-dc1394 \
    --disable-v4l2 \
    --disable-x11 \
    --disable-gtk2 \
    --disable-ffmpeg \
    --disable-avc1394 \
    --disable-mpeg2enc \
    --disable-libpng \
    --disable-libtiff \
    --disable-libjpeg \
    --disable-sdl2 \
    --without-openssl \
    --without-libjpeg \
    --without-libpng \
    --without-libtiff \
    --without-alsa \
    --without-capi \
    --without-dc1394 \
    --without-v4l2 \
    --without-x11 \
    --without-gtk2 \
    --without-ffmpeg \
    --without-avc1394 \
    --without-mpeg2enc \
    2>&1 | tail -20

echo "Building PTLib libraries..."
make debugnoshared -j$(nproc) 2>&1 | tail -10

export PTLIBDIR="$DEPS_DIR/ptlib"
echo "PTLib built successfully!"
echo "PTLIBDIR=$PTLIBDIR"
echo ""

# Build H323Plus
echo "============================================"
echo "Building H323Plus..."
echo "============================================"
cd "$DEPS_DIR/h323plus"

./configure \
    --enable-h235 \
    --enable-h46017 \
    --enable-h46019m \
    --disable-debug \
    2>&1 | tail -20

echo "Building H323Plus libraries..."
make debugnoshared -j$(nproc) 2>&1 | tail -10

export OPENH323DIR="$DEPS_DIR/h323plus"
echo "H323Plus built successfully!"
echo "OPENH323DIR=$OPENH323DIR"
echo ""

# Summary
echo "============================================"
echo "Build Complete!"
echo "============================================"
echo ""
echo "Environment variables set:"
echo "  PTLIBDIR=$PTLIBDIR"
echo "  OPENH323DIR=$OPENH323DIR"
echo ""
echo "To build h323p with H323Plus support, run:"
echo "  cd $PROJECT_DIR"
echo "  mkdir build && cd build"
echo "  cmake .. -DUSE_H323PLUS=ON -DH323PLUS_DIR=$OPENH323DIR"
echo "  make -j\$(nproc)"
echo ""
