#!/usr/bin/env bash
set -euo pipefail

# Build MarkAmp in Debug mode with tests enabled
# Usage: ./scripts/build_debug.sh [extra cmake build args...]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== MarkAmp Debug Build ==="
echo "Project: ${PROJECT_DIR}"

# Configure if not already configured
if [[ ! -d "${PROJECT_DIR}/build/debug" ]]; then
    echo "--- Configuring (debug preset) ---"
    cmake --preset debug -S "${PROJECT_DIR}"
fi

# Build
PARALLEL_JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
echo "--- Building (${PARALLEL_JOBS} parallel jobs) ---"
cmake --build "${PROJECT_DIR}/build/debug" --parallel "${PARALLEL_JOBS}" "$@"

echo "=== Debug build complete ==="
