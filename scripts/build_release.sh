#!/usr/bin/env bash
set -euo pipefail

# Build MarkAmp in Release mode (tests off)
# Usage: ./scripts/build_release.sh [extra cmake build args...]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== MarkAmp Release Build ==="
echo "Project: ${PROJECT_DIR}"

# Configure if not already configured
if [[ ! -d "${PROJECT_DIR}/build/release" ]]; then
    echo "--- Configuring (release preset) ---"
    cmake --preset release -S "${PROJECT_DIR}"
fi

# Build
PARALLEL_JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"
echo "--- Building (${PARALLEL_JOBS} parallel jobs) ---"
cmake --build "${PROJECT_DIR}/build/release" --parallel "${PARALLEL_JOBS}" "$@"

echo "=== Release build complete ==="
