#!/usr/bin/env bash
# run_fuzz.sh — Phase 36: Run all fuzz harnesses
#
# Usage: ./scripts/run_fuzz.sh [duration_seconds]

set -euo pipefail

DURATION=${1:-30}
FUZZ_DIR="$(cd "$(dirname "$0")/.." && pwd)/fuzz"
BUILD_DIR="$(cd "$(dirname "$0")/.." && pwd)/build/fuzz"

echo "=== MarkAmp Fuzz Testing ==="
echo "Duration per target: ${DURATION}s"
echo ""

# Build fuzz targets if not yet built
if [ ! -d "$BUILD_DIR" ]; then
    echo "Building fuzz targets..."
    cmake -B "$BUILD_DIR" -S "${FUZZ_DIR}/.." \
        -DCMAKE_BUILD_TYPE=Debug \
        -DMARKAMP_BUILD_FUZZ=ON \
        -DCMAKE_CXX_FLAGS="-fsanitize=fuzzer,address"
    cmake --build "$BUILD_DIR"
fi

TARGETS=(
    "fuzz_markdown_parser"
    "fuzz_syntax_highlighter"
    "fuzz_html_renderer"
    "fuzz_config_parser"
    "fuzz_constexpr_map"
    "fuzz_incremental_tokenizer"
)

CORPUS_DIR="${FUZZ_DIR}/corpus"
mkdir -p "$CORPUS_DIR"

for target in "${TARGETS[@]}"; do
    echo "--- Running ${target} for ${DURATION}s ---"
    target_corpus="${CORPUS_DIR}/${target}"
    mkdir -p "$target_corpus"

    if [ -f "${BUILD_DIR}/${target}" ]; then
        "${BUILD_DIR}/${target}" "$target_corpus" \
            -max_total_time="${DURATION}" \
            -max_len=65536 \
            2>&1 | tail -5
    else
        echo "SKIP: ${target} not built"
    fi
    echo ""
done

echo "=== Fuzz testing complete ==="
