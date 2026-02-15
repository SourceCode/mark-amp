# Phase 40: Chaos Testing Framework and Final Validation

## Metadata

| Field | Value |
|---|---|
| Phase ID | 40 |
| Prerequisites | All previous phases (01-39) |
| Estimated Complexity | High |
| Estimated File Count | 4 created, 1 modified, 1 test |
| PRD Sections | PII-43 (continuous fault injection), PII-44 (chaos plugin harness), PII-45 (synthetic event flood testing), PI-X Cultural Rules (all ten mandates), PII-46 (all public API calls guarded) |

---

## Objective

Build the continuous fault injection framework, chaos plugin harness, and synthetic event flood testing. Run the final validation suite against all 10 Definition of Resilience criteria from the PRD. This phase proves that MarkAmp is resilient.

---

## Background

The PRD mandates continuous fault injection (PII-43), a chaos plugin harness (PII-44), and synthetic event flood testing (PII-45). It also defines the "Definition of Resilience" — 10 criteria that must all be true:

1. Any plugin can fail without killing the host
2. Any malformed file can be opened without crash
3. Any user input can be rejected safely
4. Any background task can be canceled safely
5. Any thread failure is isolated
6. Any unexpected exception is logged and contained
7. Any corrupted config can be detected and recovered
8. Any rendering fault falls back safely
9. Any OOM results in controlled degradation
10. The application never terminates unexpectedly

---

## Scope

### Tasks

1. **Create `src/core/ChaosEngine.h` / `ChaosEngine.cpp`**:
   ```cpp
   namespace markamp::core {

   // Only available when MARKAMP_ENABLE_CHAOS is ON
   #ifdef MARKAMP_ENABLE_CHAOS

   enum class ChaosAction {
       ThrowException,     // Random std::runtime_error
       SimulateOOM,        // Throw std::bad_alloc
       ThreadInterrupt,    // Cancel a random async task
       FilePermissionError,// Simulate file permission denial
       SlowOperation,      // Add artificial delay (simulates slow I/O)
       CorruptConfig       // Write garbage to config file
   };

   struct ChaosConfig {
       double failure_probability{0.01};  // 1% chance per operation
       std::vector<ChaosAction> enabled_actions;
       bool target_plugins{true};
       bool target_io{true};
       bool target_rendering{true};
   };

   class ChaosEngine {
   public:
       explicit ChaosEngine(ChaosConfig config = {});

       // Inject fault at current location (probabilistic)
       void maybe_inject(SubsystemId subsystem);

       // Force inject a specific fault (for testing)
       void force_inject(ChaosAction action, SubsystemId subsystem);

       // Enable/disable
       void enable();
       void disable();
       [[nodiscard]] auto is_enabled() const -> bool;

       // Stats
       [[nodiscard]] auto injections_count() const -> size_t;
       [[nodiscard]] auto actions_log() const -> std::vector<std::string>;

   private:
       ChaosConfig config_;
       std::atomic<bool> enabled_{false};
       std::mt19937 rng_;
       std::atomic<size_t> injection_count_{0};
       std::vector<std::string> actions_log_;
       std::mutex mutex_;
   };

   #else
   // Stub when chaos is disabled
   class ChaosEngine {
   public:
       void maybe_inject(SubsystemId) {}
       void enable() {}
       void disable() {}
   };
   #endif

   } // namespace markamp::core
   ```
   - **Probabilistic injection**: At 1% probability, inject a fault matching the subsystem
   - **Actions**: throw exception, simulate OOM, cancel tasks, deny file access, add delays
   - **Disabled by default**: Only active when `MARKAMP_ENABLE_CHAOS=ON` at build time AND `enable()` is called

2. **Create `src/core/ChaosPlugin.h` / `ChaosPlugin.cpp`**:
   ```cpp
   namespace markamp::core {

   #ifdef MARKAMP_ENABLE_CHAOS

   // A deliberately misbehaving plugin for testing host resilience
   class ChaosPlugin {
   public:
       // Plugin behaviors
       void throw_on_activate();     // Throws during activation
       void throw_on_deactivate();   // Throws during deactivation
       void throw_on_event();        // Throws in event handler
       void infinite_loop();          // Enters infinite loop (tests watchdog)
       void allocate_forever();       // Allocates until OOM
       void flood_events(EventBus& bus, size_t count);  // Flood event bus
   };

   #endif

   } // namespace markamp::core
   ```

3. **Modify `CMakeLists.txt` (root)**:
   - Add: `option(MARKAMP_ENABLE_CHAOS "Enable chaos testing framework" OFF)`
   - Add compile definition: `if(MARKAMP_ENABLE_CHAOS) add_definitions(-DMARKAMP_ENABLE_CHAOS) endif()`

4. **Create `tests/unit/test_chaos_engine.cpp`**:
   - **DoD Criterion Tests** (the core validation):
     ```
     TEST_CASE("DoD 1: Plugin crash does not kill host") {
         // Activate ChaosPlugin that throws on activate
         // Verify other plugins continue to work
         // Verify process is still running
     }

     TEST_CASE("DoD 2: Malformed YAML file opens without crash") {
         // Attempt to open file with deeply nested YAML, binary content, etc.
         // Verify no crash, error reported gracefully
     }

     TEST_CASE("DoD 3: Invalid user input rejected safely") {
         // Set config values with extreme values, empty strings, binary data
         // Verify all rejected without crash
     }

     TEST_CASE("DoD 4: Background task cancelled safely") {
         // Start async file load, cancel immediately
         // Verify clean shutdown, no dangling threads
     }

     TEST_CASE("DoD 5: Thread failure is isolated") {
         // Crash a background thread, verify UI thread continues
         // Verify error logged with structured format
     }

     TEST_CASE("DoD 6: Unexpected exception logged and contained") {
         // Throw in event handler, verify log entry created
         // Verify other handlers still execute
     }

     TEST_CASE("DoD 7: Corrupted config detected and recovered") {
         // Write corrupted config, load it
         // Verify checksum mismatch detected, defaults loaded
     }

     TEST_CASE("DoD 8: Rendering fault falls back safely") {
         // Request rendering with missing theme tokens
         // Verify fallback colors used, no crash
     }

     TEST_CASE("DoD 9: OOM results in controlled degradation") {
         // Simulate bad_alloc in safe_make_unique
         // Verify error returned, no crash
     }

     TEST_CASE("DoD 10: Application never terminates unexpectedly") {
         // Run chaos engine for 10 seconds with all actions enabled
         // Verify process is still running at the end
     }
     ```
   - **Chaos-specific tests:**
     - TEST_CASE: "ChaosPlugin throw_on_activate does not crash host"
     - TEST_CASE: "ChaosPlugin flood_events handled by backpressure"
     - TEST_CASE: "ChaosEngine probabilistic injection works"
     - TEST_CASE: "ChaosEngine disabled has zero injections"
     - TEST_CASE: "ChaosEngine actions logged for diagnostics"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ChaosEngine.h` |
| Create | `src/core/ChaosEngine.cpp` |
| Create | `src/core/ChaosPlugin.h` |
| Create | `src/core/ChaosPlugin.cpp` |
| Modify | `CMakeLists.txt` |
| Create | `tests/unit/test_chaos_engine.cpp` |

---

## Implementation Notes

- **Build isolation**: When `MARKAMP_ENABLE_CHAOS` is OFF, the entire chaos engine compiles to stubs. Zero overhead, zero risk of accidental chaos in production.
- **Random number generator**: Seed `std::mt19937` with `std::random_device`. At 1% probability per call to `maybe_inject()`, most operations proceed normally but failures occur regularly enough to test resilience.
- **ChaosPlugin**: This is a mock extension that deliberately misbehaves. It's used in tests to verify that the host survives all possible plugin failure modes.
- **Event flood**: `flood_events()` publishes 10,000 events per second. The backpressure system (Phase 19) should handle this without OOM.
- **Infinite loop**: `infinite_loop()` enters a tight loop. The UIWatchdog (Phase 20) and CancellationToken (Phase 17) should detect and recover from this.
- **OOM simulation**: `allocate_forever()` allocates memory in a loop. The safe allocation wrappers (Phase 16) and bounded containers (Phase 15) should prevent this from crashing the host.
- **DoD test 10**: The "never terminates unexpectedly" test runs the chaos engine for 10 seconds with all actions enabled at 5% probability. This is the ultimate resilience test.
- **CI integration**: Add a CI step that builds with `MARKAMP_ENABLE_CHAOS=ON` and runs the chaos tests. This should NOT be part of the normal test suite.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] All 10 Definition of Resilience criteria pass as automated tests
- [ ] ChaosPlugin throwing on activation does not crash the host
- [ ] ChaosPlugin flooding events handled by backpressure (no OOM)
- [ ] ChaosEngine disabled has zero overhead (stubs compile out)
- [ ] ChaosEngine enabled with probabilistic injection works at configured rate
- [ ] Simulated OOM triggers controlled degradation (not process termination)
- [ ] All chaos actions logged for diagnostics
- [ ] `cmake -DMARKAMP_ENABLE_CHAOS=ON` builds and runs without errors
- [ ] `cmake -DMARKAMP_ENABLE_CHAOS=OFF` builds without any chaos code
- [ ] All 15+ test cases pass

---

## Testing Strategy

- Run each DoD criterion test independently to verify isolation
- Run all DoD tests together to verify no cross-test interference
- Run chaos engine for extended duration (60 seconds) to verify stability
- Run under ASan to verify no memory corruption during chaos
- Run under TSan to verify no data races during chaos
- Verify that chaos-disabled build has zero chaos code (check binary symbols)
- CI: run chaos tests in a separate pipeline with extended timeout

---

## Final Validation Checklist

This phase serves as the v7 Definition of Done. All previous 39 phases must be complete. The following must be verified:

| # | Criterion | Verified By |
|---|---|---|
| 1 | Plugin crash isolated | DoD Test 1 |
| 2 | Malformed file safe | DoD Test 2 |
| 3 | Invalid input rejected | DoD Test 3 |
| 4 | Background task cancellable | DoD Test 4 |
| 5 | Thread failure isolated | DoD Test 5 |
| 6 | Exception logged and contained | DoD Test 6 |
| 7 | Corrupted config recovered | DoD Test 7 |
| 8 | Rendering fault falls back | DoD Test 8 |
| 9 | OOM controlled degradation | DoD Test 9 |
| 10 | Never terminates unexpectedly | DoD Test 10 |

When all 10 criteria pass, MarkAmp v7 is resilience-complete.
