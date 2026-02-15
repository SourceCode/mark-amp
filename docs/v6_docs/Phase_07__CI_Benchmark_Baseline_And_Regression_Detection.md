# Phase 07: CI Benchmark Baseline & Regression Detection

## Metadata

| Field | Value |
|---|---|
| Phase ID | 07 |
| Prerequisites | Phase 03, Phase 06 |
| Estimated Complexity | Low |
| Estimated File Count | 3 created |
| PRD Sections | 8 CI Performance Enforcement |

---

## Objective

Create the benchmark regression detection infrastructure that CI can use to compare against stored baselines. The system flags any benchmark regression exceeding 5% as specified in the PRD.

---

## Background

The PRD mandates: "CI must run benchmarks, compare against baseline JSON, fail if regression >5%." This requires a baseline schema, comparison tooling, and a baseline update mechanism. The benchmark output comes from Google Benchmark's JSON format (Phase 06).

---

## Scope

### Tasks

1. **Create `benchmarks/baseline/schema.json`**:
   - JSON Schema defining the expected baseline format
   - Fields: benchmark name, iterations, real_time, cpu_time, time_unit
   - Version field for schema evolution
   - Metadata: commit hash, timestamp, platform

2. **Create `scripts/benchmark_compare.py`**:
   - Loads current benchmark JSON output (from `markamp_bench --benchmark_format=json`)
   - Loads baseline JSON from `benchmarks/baseline/`
   - For each benchmark:
     - Compute percentage change in `cpu_time`
     - Flag regressions >5% (configurable threshold via `--threshold` flag)
     - Flag improvements >10% (informational)
   - Output human-readable diff report to stdout
   - Exit code 0 for pass, 1 for regression detected
   - Handle missing benchmarks gracefully (new benchmarks don't fail, removed benchmarks warn)

3. **Create `scripts/benchmark_update_baseline.sh`**:
   - Runs `markamp_bench --benchmark_format=json --benchmark_out=benchmarks/baseline/baseline.json`
   - Adds commit hash and timestamp metadata
   - Prints confirmation message

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `benchmarks/baseline/schema.json` |
| Create | `scripts/benchmark_compare.py` |
| Create | `scripts/benchmark_update_baseline.sh` |

---

## Implementation Notes

- Google Benchmark JSON output contains an array of `benchmarks` with fields: `name`, `iterations`, `real_time`, `cpu_time`, `time_unit`, and optional `bytes_per_second`.
- The comparison should match benchmarks by name. If a benchmark exists in current but not baseline, skip (new benchmark). If exists in baseline but not current, warn (removed benchmark).
- The 5% threshold should be configurable via `--threshold` argument to allow future adjustment.
- Use Python 3 standard library only (json, argparse, sys) — no external dependencies required.
- The script should produce output like:
  ```
  PASS  BM_EventBus_Publish: 1245ns -> 1230ns (-1.2%)
  FAIL  BM_FrameArena_Alloc: 50ns -> 58ns (+16.0%) [threshold: 5%]
  INFO  BM_NewBenchmark: no baseline (new)
  ```

---

## Acceptance Criteria

- [ ] `benchmark_compare.py` correctly detects a simulated 10% regression (exit code 1)
- [ ] `benchmark_compare.py` passes when results are within 5% of baseline (exit code 0)
- [ ] Script outputs clear human-readable diff with pass/fail per benchmark
- [ ] Threshold is configurable via `--threshold` argument
- [ ] New benchmarks (not in baseline) do not cause failure
- [ ] Removed benchmarks (in baseline but not current) produce a warning
- [ ] `benchmark_update_baseline.sh` generates valid baseline JSON
- [ ] Baseline schema validates actual benchmark output

---

## Testing Strategy

- Create a mock baseline JSON and a mock current JSON with known deltas
- Run `benchmark_compare.py` and verify correct pass/fail detection
- Test edge cases: empty baseline, missing benchmark, zero-time benchmark
- Run `benchmark_update_baseline.sh` and verify output file is valid
