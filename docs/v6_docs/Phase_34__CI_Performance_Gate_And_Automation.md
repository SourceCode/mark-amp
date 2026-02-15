# Phase 34: CI Performance Gate & Automation

## Metadata

| Field | Value |
|---|---|
| Phase ID | 34 |
| Prerequisites | Phase 07 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created |
| PRD Sections | 8 CI Performance Enforcement |

---

## Objective

Create CI configuration that runs sanitizers, benchmarks, and static analysis on every commit, failing on regressions. Establish the automated quality gate described in the PRD.

---

## Background

The PRD mandates: "CI must run benchmarks, compare against baseline JSON, fail if regression >5%. Store benchmark results in versioned artifacts." This phase creates the CI workflow definitions and documentation for the performance enforcement pipeline.

---

## Scope

### Tasks

1. **Create `.github/workflows/ci-perf.yml`**:
   - Triggers: push to main, pull requests to main
   - Matrix strategy:
     - `debug-asan`: ASan + UBSan — run all tests, fail on any finding
     - `debug-tsan`: TSan — run all tests, fail on any finding
     - `debug-lsan`: LSan — run all tests, fail on any finding (Linux only)
   - Steps per matrix entry:
     1. Checkout code
     2. Install vcpkg dependencies
     3. Configure with sanitizer preset
     4. Build
     5. Run tests with `ctest --output-on-failure`
   - Additional steps (not matrix):
     - Run `scripts/check_catch_all.sh` — fail if any `catch(...)` found
     - Run `scripts/audit_static_init.sh` — fail if non-trivial global constructors in `src/core/`
     - Run clang-tidy build (optional, may be slow)

2. **Create `.github/workflows/ci-benchmark.yml`**:
   - Triggers: push to main only (not PRs, to avoid noisy results on shared runners)
   - Steps:
     1. Checkout code
     2. Install dependencies
     3. Configure with Release preset
     4. Build `markamp_bench`
     5. Run benchmarks: `./markamp_bench --benchmark_format=json --benchmark_out=results.json`
     6. Compare against baseline: `python3 scripts/benchmark_compare.py --baseline benchmarks/baseline/baseline.json --current results.json --threshold 5`
     7. Fail if regression detected
     8. Upload results as artifact
   - On main branch merge: optionally update baseline
   - Cache vcpkg packages between runs

3. **Create `docs/v6_docs/ci_performance.md`**:
   - Document CI pipeline architecture
   - Explain each workflow and its triggers
   - Document how to:
     - Add a new sanitizer configuration
     - Update the benchmark baseline
     - Interpret CI failures
     - Run CI checks locally
   - Document known limitations (e.g., shared runner variability for benchmarks)
   - Document the quality gates and their thresholds

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `.github/workflows/ci-perf.yml` |
| Create | `.github/workflows/ci-benchmark.yml` |
| Create | `docs/v6_docs/ci_performance.md` |

---

## Implementation Notes

- Benchmark results on shared CI runners are inherently noisy. Consider:
  - Running benchmarks multiple times and averaging
  - Using a generous threshold (5% as specified)
  - Pinning to specific runner types if available
  - Storing results with commit hash for trend analysis
- vcpkg caching is essential for CI performance. Use `actions/cache` with the vcpkg binary cache path.
- Sanitizer builds are slower than normal builds (2-5x). Consider running sanitizer tests in parallel jobs.
- The `ci-benchmark.yml` should only run on main merges to avoid benchmark noise from PR builds. PRs should still run sanitizers and static analysis.
- TSan and ASan are mutually exclusive — they MUST be in separate CI jobs.
- Consider adding a "benchmark bot" that comments on PRs with performance impact (future enhancement).

---

## Acceptance Criteria

- [ ] CI config files are syntactically valid YAML
- [ ] All three sanitizer presets are tested in matrix
- [ ] Benchmark regression >5% would fail the build
- [ ] `check_catch_all.sh` failure blocks merge
- [ ] `audit_static_init.sh` failure blocks merge
- [ ] Benchmark results are uploaded as CI artifacts
- [ ] `ci_performance.md` documents the full pipeline
- [ ] vcpkg dependencies are cached between runs
- [ ] CI jobs complete in reasonable time (<30 minutes for sanitizer builds)

---

## Testing Strategy

- Validate YAML syntax with `yamllint` or GitHub Actions validator
- Manually trigger workflows to verify they execute correctly
- Simulate a benchmark regression and verify CI catches it
- Verify artifact upload contains benchmark JSON
- Review ci_performance.md for completeness
