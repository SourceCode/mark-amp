#!/usr/bin/env bash
# dead_code_report.sh — Dead Code Detection Script
#
# Phase 15: Analyzes the Release binary for unreferenced symbols
# using nm and link-time dead code analysis.
#
# Usage:
#   ./scripts/dead_code_report.sh [build_dir]
#
# If build_dir is not specified, defaults to build/release.

set -euo pipefail

BUILD_DIR="${1:-build/release}"
BINARY=""

# Find the main binary
if [[ -f "$BUILD_DIR/src/markamp.app/Contents/MacOS/markamp" ]]; then
    BINARY="$BUILD_DIR/src/markamp.app/Contents/MacOS/markamp"
elif [[ -f "$BUILD_DIR/src/markamp" ]]; then
    BINARY="$BUILD_DIR/src/markamp"
else
    echo "❌ ERROR: Could not find markamp binary in $BUILD_DIR"
    echo "   Build with: cmake --preset release && cmake --build build/release"
    exit 1
fi

echo "═══════════════════════════════════════════════════════════"
echo " Dead Code Report"
echo " Binary: $BINARY"
echo " Size:   $(du -h "$BINARY" | cut -f1)"
echo "═══════════════════════════════════════════════════════════"
echo ""

# --- Section 1: Symbol summary ---
echo "── Symbol Summary ──"
TOTAL_SYMBOLS=$(nm -U "$BINARY" 2>/dev/null | wc -l | tr -d ' ')
TEXT_SYMBOLS=$(nm -U "$BINARY" 2>/dev/null | grep -c ' [Tt] ' || true)
DATA_SYMBOLS=$(nm -U "$BINARY" 2>/dev/null | grep -c ' [DdBb] ' || true)
echo "  Total defined symbols: $TOTAL_SYMBOLS"
echo "  Text (code) symbols:   $TEXT_SYMBOLS"
echo "  Data symbols:          $DATA_SYMBOLS"
echo ""

# --- Section 2: Unused global constructors ---
echo "── Global Constructors ──"
INIT_COUNT=$(nm "$BINARY" 2>/dev/null | grep -cE '(__cxx_global_var_init|_GLOBAL__sub_I_)' || true)
if [ "$INIT_COUNT" -eq 0 ]; then
    echo "  ✅ No non-trivial global constructors detected."
else
    echo "  ⚠️  $INIT_COUNT global constructor(s) found:"
    nm "$BINARY" 2>/dev/null | grep -E '(__cxx_global_var_init|_GLOBAL__sub_I_)' | head -20
fi
echo ""

# --- Section 3: Large symbols (potential dead code targets) ---
echo "── Largest Symbols (top 20) ──"
nm -S -U "$BINARY" 2>/dev/null | \
    awk '$2 ~ /^[0-9a-fA-F]+$/ { printf "%8d %s %s\n", strtonum("0x"$2), $3, $4 }' | \
    sort -rn | head -20 || echo "  (nm -S not supported on this platform)"
echo ""

# --- Section 4: markamp-specific dead function candidates ---
echo "── Potential Dead Functions (unreferenced markamp:: symbols) ──"
# Look for markamp symbols that have no incoming references
# This is heuristic: symbols that appear only in the text section (defined)
# but nowhere in the data section (no vtable/function pointer references)
DEAD_CANDIDATES=0
while IFS= read -r sym; do
    # Check if the symbol name appears anywhere in data relocations
    ref_count=$(nm -U "$BINARY" 2>/dev/null | grep -c "$sym" || true)
    if [ "$ref_count" -le 1 ]; then
        echo "  $sym"
        DEAD_CANDIDATES=$((DEAD_CANDIDATES + 1))
    fi
done < <(nm -U "$BINARY" 2>/dev/null | grep ' [Tt] ' | grep 'markamp' | \
         awk '{print $3}' | grep -v '__' | head -50)

echo ""
echo "  Found $DEAD_CANDIDATES candidate(s) for dead code review."
echo ""

# --- Section 5: Linker dead-strip verification ---
echo "── Linker Dead-Strip Verification ──"
if nm -m "$BINARY" 2>/dev/null | grep -q 'dead stripped'; then
    echo "  ✅ Dead stripping is active (symbols marked as stripped found)."
else
    echo "  ℹ️  No 'dead stripped' markers found."
    echo "      Ensure -dead_strip (macOS) or --gc-sections (Linux) is enabled."
fi
echo ""

# --- Summary ---
echo "═══════════════════════════════════════════════════════════"
echo " Report complete."
echo "═══════════════════════════════════════════════════════════"
