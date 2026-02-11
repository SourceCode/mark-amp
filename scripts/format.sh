#!/usr/bin/env bash
# Format all C++ source files, or check formatting in CI mode.
# Usage: ./scripts/format.sh [--check]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Find all C++ source and header files
FILES=$(find "$PROJECT_ROOT/src" "$PROJECT_ROOT/tests" \
    -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cc' \) \
    2>/dev/null || true)

if [[ -z "$FILES" ]]; then
    echo "No source files found."
    exit 0
fi

FILE_COUNT=$(echo "$FILES" | wc -l | tr -d ' ')

if [[ "${1:-}" == "--check" ]]; then
    echo "Checking formatting of $FILE_COUNT files..."
    FAILED=0
    while IFS= read -r file; do
        if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
            FAILED=$((FAILED + 1))
        fi
    done <<< "$FILES"

    if [[ $FAILED -gt 0 ]]; then
        echo "ERROR: $FAILED file(s) have formatting issues."
        exit 1
    fi
    echo "All $FILE_COUNT files are correctly formatted."
else
    echo "Formatting $FILE_COUNT files..."
    echo "$FILES" | xargs clang-format -i
    echo "Done. $FILE_COUNT files formatted."
fi
