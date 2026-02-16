#!/usr/bin/env bash
# audit_static_init.sh — Detect non-trivial global constructors in object files
#
# Phase 11: Compile-Time Optimization
#
# Uses nm to find __cxx_global_var_init symbols that indicate
# non-trivial static initialization (runs before main()).
#
# Usage:
#   bash scripts/audit_static_init.sh <build_dir>
#
# Exit codes:
#   0 — No non-trivial global constructors found in src/core/
#   1 — Findings present (or usage error)

set -euo pipefail

BUILD_DIR="${1:-build/debug}"

if [ ! -d "$BUILD_DIR" ]; then
    echo "ERROR: Build directory '$BUILD_DIR' not found."
    echo "Usage: $0 <build_dir>"
    exit 1
fi

echo "=== Static Initialization Audit ==="
echo "Build directory: $BUILD_DIR"
echo ""

# Find all .o files under the core CMake target directory
CORE_OBJ_DIR="$BUILD_DIR/src/CMakeFiles/markamp_core.dir/core"

if [ ! -d "$CORE_OBJ_DIR" ]; then
    echo "WARNING: Core object directory not found at $CORE_OBJ_DIR"
    echo "Searching recursively under $BUILD_DIR for core object files..."
    CORE_OBJ_DIR="$BUILD_DIR"
fi

FINDINGS=0
TOTAL_FILES=0
declare -A FILE_COUNTS 2>/dev/null || true

echo "--- Scanning for __cxx_global_var_init symbols ---"
echo ""

while IFS= read -r -d '' obj_file; do
    TOTAL_FILES=$((TOTAL_FILES + 1))

    # Use nm to find global constructor symbols
    # __cxx_global_var_init = Clang/libc++ non-trivial static init
    # _GLOBAL__sub_I_ = GCC global sub-initializer
    init_count=$(nm -g "$obj_file" 2>/dev/null | grep -cE '(__cxx_global_var_init|_GLOBAL__sub_I_)' || true)

    if [ "$init_count" -gt 0 ]; then
        # Get relative path for readability
        rel_path="${obj_file#$BUILD_DIR/}"
        echo "  [$init_count] $rel_path"
        FINDINGS=$((FINDINGS + init_count))
    fi
done < <(find "$CORE_OBJ_DIR" -name "*.cpp.o" -print0 2>/dev/null | sort -z)

echo ""
echo "--- Summary ---"
echo "Object files scanned: $TOTAL_FILES"
echo "Non-trivial global constructors: $FINDINGS"
echo ""

if [ "$FINDINGS" -eq 0 ]; then
    echo "✅ PASS: No non-trivial static initialization in core objects."
    exit 0
else
    echo "⚠️  FINDINGS: $FINDINGS non-trivial global constructor(s) detected."
    echo ""
    echo "These represent hidden startup latency. Consider converting to:"
    echo "  - constexpr (immutable tables)"
    echo "  - constinit (mutable state with constant initialization)"
    echo "  - Function-local static (lazy init on first use)"
    exit 1
fi
