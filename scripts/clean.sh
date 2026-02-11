#!/usr/bin/env bash
set -euo pipefail

# Remove all build artifacts
# Usage: ./scripts/clean.sh

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== MarkAmp Clean ==="

DIRS_TO_CLEAN=(
    "${PROJECT_DIR}/build"
    "${PROJECT_DIR}/out"
    "${PROJECT_DIR}/dist"
)

for dir in "${DIRS_TO_CLEAN[@]}"; do
    if [[ -d "${dir}" ]]; then
        echo "Removing ${dir}/"
        rm -rf "${dir}"
    fi
done

echo "=== Clean complete ==="
