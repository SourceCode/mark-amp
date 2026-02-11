#!/usr/bin/env bash
# Run clang-tidy and cppcheck on the project.
# Usage: ./scripts/lint.sh [--fix]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Look for compile_commands.json in common build directories
COMPILE_DB=""
for dir in "$PROJECT_ROOT/build/debug" "$PROJECT_ROOT/build/release" "$PROJECT_ROOT/build"; do
    if [[ -f "$dir/compile_commands.json" ]]; then
        COMPILE_DB="$dir/compile_commands.json"
        break
    fi
done

if [[ -z "$COMPILE_DB" ]]; then
    echo "ERROR: compile_commands.json not found."
    echo "Run 'cmake --preset debug' first to generate the compile commands database."
    exit 1
fi

COMPILE_DB_DIR="$(dirname "$COMPILE_DB")"

# Find all source files
FILES=$(find "$PROJECT_ROOT/src" "$PROJECT_ROOT/tests" \
    -type f \( -name '*.cpp' -o -name '*.cc' \) \
    2>/dev/null || true)

if [[ -z "$FILES" ]]; then
    echo "No source files found."
    exit 0
fi

FILE_COUNT=$(echo "$FILES" | wc -l | tr -d ' ')
EXIT_CODE=0

FIX_FLAG=""
if [[ "${1:-}" == "--fix" ]]; then
    FIX_FLAG="--fix"
fi

# --- clang-tidy ---
echo "=== Running clang-tidy on $FILE_COUNT files ==="
TIDY_ERRORS=0
while IFS= read -r file; do
    if ! clang-tidy "$file" -p "$COMPILE_DB_DIR" $FIX_FLAG 2>&1; then
        TIDY_ERRORS=$((TIDY_ERRORS + 1))
    fi
done <<< "$FILES"

if [[ $TIDY_ERRORS -gt 0 ]]; then
    echo "clang-tidy: $TIDY_ERRORS file(s) with warnings/errors."
    EXIT_CODE=1
else
    echo "clang-tidy: All files clean."
fi

# --- cppcheck ---
echo ""
echo "=== Running cppcheck ==="
if command -v cppcheck &>/dev/null; then
    SUPPRESSIONS="$PROJECT_ROOT/cppcheck-suppressions.txt"
    CPPCHECK_FLAGS="--enable=all --quiet --inline-suppr"
    if [[ -f "$SUPPRESSIONS" ]]; then
        CPPCHECK_FLAGS="$CPPCHECK_FLAGS --suppressions-list=$SUPPRESSIONS"
    fi
    if ! cppcheck --project="$COMPILE_DB" $CPPCHECK_FLAGS 2>&1; then
        echo "cppcheck: Issues found."
        EXIT_CODE=1
    else
        echo "cppcheck: All files clean."
    fi
else
    echo "cppcheck not found -- skipping."
fi

echo ""
echo "=== Lint Summary ==="
if [[ $EXIT_CODE -eq 0 ]]; then
    echo "All checks passed."
else
    echo "Issues found. See output above for details."
fi

exit $EXIT_CODE
