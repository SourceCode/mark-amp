# Phase 21: Thread Panic Escalation Policy

## Metadata

| Field | Value |
|---|---|
| Phase ID | 21 |
| Prerequisites | Phase 20 (thread health registry), Phase 04 (crash barrier) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PII-15 (thread panic escalation policy) |

---

## Objective

Define and implement the escalation policy when a background thread fails: log, restart, notify user — never terminate the process. Three escalation levels handle increasing severity from transient errors to persistent thread failures.

---

## Background

The PRD mandates a thread panic escalation policy (PII-15) that ensures background thread failures never terminate the process. Currently, if a background thread (e.g., AsyncFileLoader, AsyncHighlighter) throws an unhandled exception, the `std::terminate` handler is called. This phase adds a structured escalation policy that retries, disables features, and ultimately dumps state — but never crashes.

---

## Scope

### Tasks

1. **Create `src/core/ThreadPanicPolicy.h` / `ThreadPanicPolicy.cpp`**:
   ```cpp
   namespace markamp::core {

   enum class PanicLevel {
       Retry,          // Level 1: Log + automatic retry
       DisableFeature, // Level 2: Log + disable feature + notify user
       DumpAndOffer    // Level 3: Log + dump thread state + offer restart
   };

   struct ThreadPanicConfig {
       size_t max_retries{3};                              // Retries before escalation
       std::chrono::seconds retry_window{60};               // Window for counting retries
       std::chrono::milliseconds retry_delay{500};          // Delay between retries
   };

   class ThreadPanicPolicy {
   public:
       explicit ThreadPanicPolicy(ThreadPanicConfig config = {});

       // Report a thread failure and get the escalation response
       [[nodiscard]] auto on_thread_failure(
           std::string_view thread_name,
           const std::exception& ex
       ) -> PanicLevel;

       // Execute a function with panic policy protection
       template<typename F>
       void execute_with_policy(std::string_view thread_name, F&& fn);

       // Reset failure counts for a thread (e.g., after successful recovery)
       void reset(std::string_view thread_name);

       // Get failure history for diagnostics
       [[nodiscard]] auto failure_history(std::string_view thread_name)
           -> std::vector<std::string>;

   private:
       ThreadPanicConfig config_;
       struct ThreadState {
           size_t failure_count{0};
           std::chrono::steady_clock::time_point first_failure;
           std::vector<std::string> error_messages;
       };
       std::mutex mutex_;
       std::unordered_map<std::string, ThreadState> states_;
   };

   } // namespace markamp::core
   ```

   **Escalation levels:**
   | Level | Condition | Action |
   |---|---|---|
   | 1 (Retry) | failure_count <= max_retries | Log error, wait retry_delay, retry function |
   | 2 (DisableFeature) | failure_count > max_retries within window | Log error, emit FeatureDisabledEvent, notify user |
   | 3 (DumpAndOffer) | Level 2 failure persists after feature restart | Log error, dump thread state, emit FaultRecoveryEvent |

2. **Modify `src/core/AsyncFileLoader.cpp`**:
   - Wrap the async loading function with `ThreadPanicPolicy::execute_with_policy()`
   - On Level 2: disable file loading feature, show notification
   - On Level 3: log dump, offer safe mode restart

3. **Modify `src/core/AsyncHighlighter.cpp`**:
   - Wrap highlighting function with panic policy
   - On Level 2: disable syntax highlighting, show notification

4. **Create `tests/unit/test_thread_panic.cpp`**:
   - TEST_CASE: "First failure triggers retry (Level 1)"
   - TEST_CASE: "Retry succeeds and resets count"
   - TEST_CASE: "3 consecutive failures escalate to Level 2"
   - TEST_CASE: "Level 2 emits FeatureDisabledEvent"
   - TEST_CASE: "Reset clears failure history"
   - TEST_CASE: "Failure outside window resets count"
   - TEST_CASE: "execute_with_policy retries on failure"
   - TEST_CASE: "Process never terminates from background thread failure"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ThreadPanicPolicy.h` |
| Create | `src/core/ThreadPanicPolicy.cpp` |
| Modify | `src/core/AsyncFileLoader.cpp` |
| Modify | `src/core/AsyncHighlighter.cpp` |
| Create | `tests/unit/test_thread_panic.cpp` |

---

## Implementation Notes

- **Retry window**: Failures are counted within a sliding window (default 60 seconds). If a failure occurs after the window expires, the counter resets. This prevents a single transient error from permanently escalating.
- **Retry delay**: Use `std::this_thread::sleep_for()` between retries to avoid rapid retry storms. The delay doubles on each retry (exponential backoff: 500ms, 1s, 2s).
- **execute_with_policy**: This template function wraps the provided callable in a try/catch, calls `on_thread_failure()` to determine the escalation level, and takes the appropriate action. At Level 1, it retries. At Level 2+, it stops retrying and reports to the error reporting service.
- **Thread state dump**: At Level 3, dump the thread's state including: thread name, failure count, recent error messages, last known correlation ID, and stack trace from the crash barrier.
- **Events**: Define `FeatureDisabledEvent` and `FaultRecoveryEvent` in `Events.h` using `MARKAMP_DECLARE_EVENT`.
- **Integration with Phase 37**: Level 3 escalation triggers the fault recovery dialog from Phase 37.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Background thread crash triggers retry at Level 1
- [ ] 3 consecutive crashes within window escalate to Level 2
- [ ] Level 2 disables the affected feature and notifies user
- [ ] Level 3 dumps thread state and offers recovery options
- [ ] Failure outside retry window resets the counter
- [ ] Process never terminates from background thread failure
- [ ] Retry delay increases exponentially
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test escalation progression: single failure → retry → multiple failures → Level 2
- Test window expiration resets counter
- Test execute_with_policy with functions that throw on first N calls then succeed
- Test that Level 2 events are emitted correctly
- Verify process survives repeated thread failures
- Run under TSan to verify thread safety of panic state
