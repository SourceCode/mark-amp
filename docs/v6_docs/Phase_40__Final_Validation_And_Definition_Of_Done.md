# Phase 40: Final Validation & Definition of Done

## Metadata

| Field | Value |
|---|---|
| Phase ID | 40 |
| Prerequisites | Phase 34, Phase 37, Phase 39 |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 1 modified |
| PRD Sections | 13 Definition of Done, 14 Final Mandate |

---

## Objective

Run the complete validation suite to verify all PRD requirements are met. Document results and produce the final v6 release report with a per-requirement pass/fail matrix.

---

## Background

The PRD's Definition of Done states: "MarkAmp qualifies as performance-stable when: All sanitizers clean, No UB detected, Startup <150ms, No benchmark regression >5%, No memory leaks, All frame budgets respected, No plugin can crash host." This phase is the final validation gate that confirms all 39 preceding phases have achieved their objectives.

---

## Scope

### Tasks

1. **Run complete validation suite**:

   **Sanitizer validation**:
   - Build with `debug-asan`: ASan + UBSan
   - Run all unit tests → must pass with zero findings
   - Build with `debug-tsan`: ThreadSanitizer
   - Run all unit tests → must pass with zero findings
   - Build with `debug-lsan`: LeakSanitizer
   - Run all unit tests → must pass with zero findings

   **Benchmark validation**:
   - Build with Release preset
   - Run `markamp_bench` → all benchmarks within 5% of baseline
   - Run `scripts/benchmark_compare.py` → exit code 0

   **Startup validation**:
   - Build with Release preset
   - Run application and capture StartupTimer report
   - Cold start must be <150ms
   - Warm start must be <50ms (application already in OS disk cache)

   **Frame budget validation**:
   - Run `bench_rendering_pipeline` → single-edit frame time <16ms
   - Run `bench_incremental_tokenization` → incremental tokenize <8ms
   - Run `load_continuous_editing` → p95 frame time <16ms for 60 seconds

   **Memory budget validation**:
   - Run application idle → RSS <150MB
   - Open large file (10MB) → RSS <2x file size overhead above idle
   - Memory budget monitor does not trigger MemoryExceededEvent

   **Fuzz testing validation**:
   - Run all 6 fuzz targets for 10 minutes each → zero crashes

   **Load testing validation**:
   - Run all load test scenarios → all pass

   **Chaos testing validation**:
   - Run all chaos scenarios → application degrades gracefully, no crashes

   **Static analysis validation**:
   - `check_catch_all.sh` → zero `catch(...)` in codebase
   - `audit_static_init.sh` → zero non-trivial global constructors in `src/core/`
   - clang-tidy → clean build

2. **Create `docs/v6_docs/validation_report.md`**:
   - Per-requirement pass/fail matrix:
     | Requirement | Target | Actual | Status |
     |---|---|---|---|
     | Cold start | <150ms | _measured_ | PASS/FAIL |
     | Warm start | <50ms | _measured_ | PASS/FAIL |
     | Frame time | <16ms | _measured_ | PASS/FAIL |
     | Memory idle | <150MB | _measured_ | PASS/FAIL |
     | ASan clean | 0 findings | _count_ | PASS/FAIL |
     | UBSan clean | 0 findings | _count_ | PASS/FAIL |
     | TSan clean | 0 findings | _count_ | PASS/FAIL |
     | LSan clean | 0 findings | _count_ | PASS/FAIL |
     | Benchmark regression | <5% | _max%_ | PASS/FAIL |
     | Fuzz testing | 0 crashes | _count_ | PASS/FAIL |
     | Load tests | all pass | _results_ | PASS/FAIL |
     | Chaos tests | graceful | _results_ | PASS/FAIL |
     | catch(...) | 0 | _count_ | PASS/FAIL |
     | clang-tidy | clean | _findings_ | PASS/FAIL |
   - Benchmark results table with all benchmarks and their values
   - Startup timing breakdown per phase
   - Memory budget compliance details
   - Known limitations and future work recommendations

3. **Create `docs/v6_docs/performance_tuning_guide.md`**:
   - How to use Tracy for profiling MarkAmp
   - How to run the benchmark suite
   - How to run fuzz tests
   - How to interpret the Health Panel
   - How to adjust performance budgets (Config keys)
   - How to run chaos testing
   - How to investigate benchmark regressions
   - How to add new benchmarks
   - Troubleshooting common performance issues

4. **Update `CHANGELOG.md`**:
   - Add v6 release notes summarizing all 40 phases
   - Highlight key improvements:
     - Startup time improvement (before vs after)
     - Frame time improvement
     - Memory usage improvement
     - New tooling: Tracy, Google Benchmark, fuzz testing, chaos testing
     - New features: Health Panel, structured logging, crash reporting, watchdog
     - Infrastructure: CI performance gates, packaging

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `docs/v6_docs/validation_report.md` |
| Create | `docs/v6_docs/performance_tuning_guide.md` |
| Modify | `CHANGELOG.md` |

---

## Implementation Notes

- This phase is primarily documentation and validation — no code changes beyond CHANGELOG.
- The validation suite should be run on reference hardware (document the hardware specs in the report).
- If any validation step fails, the fix belongs in the relevant earlier phase — do not create workarounds in Phase 40.
- The performance tuning guide should be written for developers who will maintain the codebase after v6.
- The CHANGELOG entry should be comprehensive — this is a major release.
- Consider including before/after screenshots of the Health Panel showing improvement.

---

## Acceptance Criteria

- [ ] Every PRD requirement has a documented pass/fail result in validation_report.md
- [ ] Cold start measured at <150ms on reference hardware
- [ ] Zero sanitizer findings across all four sanitizers
- [ ] Zero benchmark regressions >5%
- [ ] All fuzz targets survive 10-minute runs
- [ ] All load tests pass
- [ ] All chaos tests complete without crashes
- [ ] `validation_report.md` is complete with all metrics filled in
- [ ] `performance_tuning_guide.md` covers all tooling introduced in v6
- [ ] CHANGELOG.md includes comprehensive v6 release notes
- [ ] All 40 phases marked complete

---

## Testing Strategy

- Execute the full validation suite as described above
- Have a second person review the validation report for accuracy
- Run the validation suite on at least two different machines to verify portability
- Verify the performance tuning guide is followable by running its instructions
