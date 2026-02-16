#!/usr/bin/env bash
# run_chaos_tests.sh — Phase 37: Run chaos testing scenarios
#
# Usage: ./scripts/run_chaos_tests.sh

set -euo pipefail

BUILD_DIR="$(cd "$(dirname "$0")/.." && pwd)/build/debug"
TEST_BIN="${BUILD_DIR}/tests/chaos/test_chaos_engine"

echo "=== MarkAmp Chaos Testing ==="

if [ ! -f "$TEST_BIN" ]; then
    echo "Building chaos test targets..."
    cd "$(dirname "$0")/.."
    cmake --build build/debug --target test_chaos_engine 2>&1
fi

echo "Running chaos engine tests..."
"$TEST_BIN" --reporter compact 2>&1

echo ""
echo "=== Chaos testing complete ==="
