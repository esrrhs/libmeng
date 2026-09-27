#!/bin/sh
set -e

BUILD_TYPE="Debug"
if [ "$1" = "release" ]; then
    BUILD_TYPE="Release"
fi

BUILD_DIR="build"

echo "=== Configuring libmeng ($BUILD_TYPE) ==="
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DMENG_BUILD_TESTS=ON \
    -DMENG_BUILD_BENCHMARKS=ON

echo "=== Building libmeng ==="
cmake --build "$BUILD_DIR" -j"$(nproc 2>/dev/null || echo 4)"

echo "=== Running Tests ==="
ctest --test-dir "$BUILD_DIR" --output-on-failure

# Symlink or mirror output to top-level bin for backward compatibility
mkdir -p bin
cp -rf "$BUILD_DIR/bin/"* bin/ 2>/dev/null || true
cp -rf "$BUILD_DIR/libmeng.a" bin/ 2>/dev/null || true

echo "=== Build and Tests Succeeded! ==="
