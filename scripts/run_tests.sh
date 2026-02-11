#!/usr/bin/env bash
set -euo pipefail

# Build Debug + run all tests
# Usage: ./scripts/run_tests.sh [ctest args...]
#   e.g. ./scripts/run_tests.sh -R test_theme  (run only theme tests)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== MarkAmp Test Runner ==="

# Ensure debug build is up to date
"${SCRIPT_DIR}/build_debug.sh"

echo ""
echo "--- Running tests ---"
cd "${PROJECT_DIR}/build/debug"
ctest --output-on-failure "$@"

echo "=== All tests complete ==="
