# CI Performance Gates — Phase 34

## Overview

MarkAmp uses CI performance gates to prevent performance regressions.
Two GitHub Actions workflows enforce quality:

### ci-benchmark.yml

- Runs on every PR to `main` and `develop`
- Builds benchmarks in Release mode
- Executes all Google Benchmark targets
- Outputs JSON results as a build artifact
- Future: automated comparison against baseline

### ci-perf.yml

- Runs sanitizer builds (ASan, UBSan, TSan) on every PR
- Matrix strategy tests all three sanitizer modes
- Ensures no memory leaks, undefined behavior, or data races

## How to Add a New Benchmark

1. Create `benchmarks/bench_<name>.cpp`
2. Add it to `benchmarks/CMakeLists.txt`
3. The CI will automatically pick it up

## Baseline Management

Benchmark baselines are stored as JSON artifacts. To update:

1. Merge to `main`
2. CI run produces new baseline
3. Subsequent PRs compare against latest baseline
