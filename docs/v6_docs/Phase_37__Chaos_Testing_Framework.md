# Phase 37: Chaos Testing Framework

## Metadata

| Field | Value |
|---|---|
| Phase ID | 37 |
| Prerequisites | Phase 09, Phase 36 |
| Estimated Complexity | High |
| Estimated File Count | 7 created, 2 modified |
| PRD Sections | 11.2 Chaos Testing |

---

## Objective

Build a chaos testing framework that injects random failures to verify the application degrades gracefully under adversarial conditions. The system must remain stable under chaos.

---

## Background

The PRD requires: "Inject: Random plugin failures, Filesystem permission errors, Thread scheduling anomalies, OOM simulation. System must degrade gracefully." Chaos testing verifies that the application's error handling paths actually work when real failures occur, not just when unit tests simulate them.

---

## Scope

### Tasks

1. **Create `src/core/ChaosEngine.h` / `ChaosEngine.cpp`**:
   - `ChaosMode` enum: `Off`, `Low` (1% failure), `Medium` (5%), `High` (20%)
   - Injection points (each independently configurable):
     - `plugin_activation_failure`: plugin activate() throws exception
     - `filesystem_permission_error`: file operations fail with EACCES
     - `allocation_failure`: malloc/new returns nullptr (selective, not global)
     - `event_handler_exception`: random event handler throws
     - `thread_scheduling_delay`: artificial sleep(100ms) in worker threads
   - `ChaosEngine::should_fail(injection_point) -> bool`:
     - Uses PRNG with configurable seed for reproducibility
     - Returns true with probability based on ChaosMode
   - `ChaosEngine::set_mode(ChaosMode)`: runtime activation
   - `ChaosEngine::set_seed(uint64_t)`: reproducible chaos
   - Activated via:
     - Config: `chaos.mode = "medium"`
     - Command line: `--chaos=medium`
     - Runtime: via ChaosEngine API

2. **Create injection hooks**:
   - In PluginManager: before `plugin.activate()`, check `ChaosEngine::should_fail(plugin_activation_failure)`
   - In filesystem wrappers: before file operations, check `ChaosEngine::should_fail(filesystem_permission_error)`
   - In EventBus dispatch: before handler call, check `ChaosEngine::should_fail(event_handler_exception)`
   - Hooks are zero-cost when chaos mode is Off (compile out or simple flag check)

3. **Create chaos test scenarios** in `tests/chaos/`:

   **`tests/chaos/chaos_test_runner.cpp`**:
   - Orchestrates scenario execution
   - Sets chaos mode, runs scenario, verifies no crash
   - Reports which injection points triggered

   **`tests/chaos/chaos_scenario_plugin_failure.cpp`**:
   - Activate plugins with 20% failure rate
   - Verify: host doesn't crash, surviving plugins still work, failed plugins are deactivated

   **`tests/chaos/chaos_scenario_fs_error.cpp`**:
   - File operations fail with 10% probability
   - Verify: user sees error messages (not crashes), recovery possible

   **`tests/chaos/chaos_scenario_oom.cpp`**:
   - Selective allocation failures in non-critical paths
   - Verify: no undefined behavior, no double-free, graceful degradation
   - Note: global OOM simulation is dangerous — only inject in controlled locations

4. **Create `scripts/run_chaos_tests.sh`**:
   - Builds chaos test runner
   - Runs all scenarios
   - Reports pass/fail per scenario
   - Accepts seed parameter for reproducibility

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ChaosEngine.h` |
| Create | `src/core/ChaosEngine.cpp` |
| Create | `tests/chaos/chaos_test_runner.cpp` |
| Create | `tests/chaos/chaos_scenario_plugin_failure.cpp` |
| Create | `tests/chaos/chaos_scenario_fs_error.cpp` |
| Create | `tests/chaos/chaos_scenario_oom.cpp` |
| Create | `scripts/run_chaos_tests.sh` |
| Modify | `src/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- The PRNG must be deterministic given a seed, so failures are reproducible. Use `std::mt19937_64` with explicit seed.
- Chaos mode should be completely zero-cost when Off. Use a global `std::atomic<bool>` flag checked with `memory_order_relaxed`.
- **Do NOT inject OOM globally** (replacing malloc). This will crash the runtime, standard library, and everything else. Instead, inject allocation failures at specific call sites using a wrapper.
- Filesystem error injection: wrap `std::filesystem` operations with a chaos check. When triggered, throw `std::filesystem::filesystem_error` with `std::errc::permission_denied`.
- Plugin failure injection: before calling `plugin.activate()`, if chaos triggers, throw `std::runtime_error("Chaos: plugin activation failure")`.
- Each scenario should run for a fixed duration (e.g., 30 seconds) simulating user operations (open file, edit, scroll, save).
- Report format: each scenario reports pass/fail with injection count and any unexpected behavior.

---

## Acceptance Criteria

- [ ] Chaos engine can be activated via config flag
- [ ] Plugin failure injection does not crash host application
- [ ] Filesystem error injection produces graceful error messages (not crashes)
- [ ] OOM injection does not cause undefined behavior
- [ ] All chaos scenarios complete without segfault or abort
- [ ] Application recovers when chaos mode is disabled
- [ ] Chaos results are reproducible with same seed
- [ ] Zero-cost when chaos mode is Off (verified by benchmark)

---

## Testing Strategy

- Run each chaos scenario independently
- Run all scenarios with seed=42 for reproducibility
- Run all scenarios under ASan for memory safety
- Run all scenarios under TSan for thread safety
- Verify that disabling chaos mode returns to normal operation
