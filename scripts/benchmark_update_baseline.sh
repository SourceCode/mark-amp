#!/usr/bin/env bash
# benchmark_update_baseline.sh — Run benchmarks and update the baseline JSON.
#
# Usage:
#   ./scripts/benchmark_update_baseline.sh [build_dir]
#
# The script runs markamp_bench, adds commit metadata, and saves the
# result to benchmarks/baseline/baseline.json.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${1:-$PROJECT_ROOT/build/debug}"
BASELINE_DIR="$PROJECT_ROOT/benchmarks/baseline"
BASELINE_FILE="$BASELINE_DIR/baseline.json"
BENCH_EXECUTABLE="$BUILD_DIR/benchmarks/markamp_bench"

# Verify the benchmark executable exists
if [[ ! -x "$BENCH_EXECUTABLE" ]]; then
    echo "Error: markamp_bench not found at $BENCH_EXECUTABLE"
    echo "  Build with -DMARKAMP_BUILD_BENCHMARKS=ON first."
    exit 1
fi

# Ensure baseline directory exists
mkdir -p "$BASELINE_DIR"

echo "Running benchmarks..."
"$BENCH_EXECUTABLE" \
    --benchmark_format=json \
    --benchmark_out="$BASELINE_FILE" \
    2>&1 | tail -5

# Add metadata to the baseline file
COMMIT_HASH=$(git -C "$PROJECT_ROOT" rev-parse --short HEAD 2>/dev/null || echo "unknown")
TIMESTAMP=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
PLATFORM=$(uname -s -m)

# Use Python to inject metadata into the JSON context
python3 -c "
import json, sys
with open('$BASELINE_FILE') as f:
    data = json.load(f)
ctx = data.setdefault('context', {})
ctx['commit_hash'] = '$COMMIT_HASH'
ctx['timestamp'] = '$TIMESTAMP'
ctx['platform'] = '$PLATFORM'
data['schema_version'] = 1
with open('$BASELINE_FILE', 'w') as f:
    json.dump(data, f, indent=2)
print(f'Baseline updated: {len(data.get(\"benchmarks\", []))} benchmarks recorded')
print(f'  Commit:    {ctx[\"commit_hash\"]}')
print(f'  Timestamp: {ctx[\"timestamp\"]}')
print(f'  File:      $BASELINE_FILE')
"

echo "Done."
