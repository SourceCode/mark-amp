#!/usr/bin/env bash
# run_load_tests.sh — Phase 39: Run load testing scenarios
#
# Usage: ./scripts/run_load_tests.sh

set -euo pipefail

BUILD_DIR="$(cd "$(dirname "$0")/.." && pwd)/build/debug"
TEST_BIN="${BUILD_DIR}/tests/load/test_load_harness"

echo "=== MarkAmp Load Testing ==="

if [ ! -f "$TEST_BIN" ]; then
    echo "Building load test targets..."
    cd "$(dirname "$0")/.."
    cmake --build build/debug --target test_load_harness 2>&1
fi

echo "Running load harness tests..."
"$TEST_BIN" --reporter compact 2>&1

echo ""
echo "=== Load testing complete ==="
