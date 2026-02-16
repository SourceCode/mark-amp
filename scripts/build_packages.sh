#!/usr/bin/env bash
# build_packages.sh — Phase 38: Build distribution packages
#
# Usage: ./scripts/build_packages.sh [Release|Debug]

set -euo pipefail

BUILD_TYPE=${1:-Release}
BUILD_DIR="build/package"

echo "=== MarkAmp Package Builder ==="
echo "Build type: ${BUILD_TYPE}"
echo ""

cd "$(dirname "$0")/.."

echo "Configuring..."
cmake -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DMARKAMP_BUILD_TESTS=OFF \
    -DMARKAMP_BUILD_BENCHMARKS=OFF

echo "Building..."
cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}"

echo "Packaging..."
cd "${BUILD_DIR}"
cpack -C "${BUILD_TYPE}" --verbose

echo ""
echo "=== Packages built successfully ==="
ls -la MarkAmp-* 2>/dev/null || echo "No packages found (check cpack output above)"
