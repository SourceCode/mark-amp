# Phase 20: Deadlock Detection and UI Watchdog

## Metadata

| Field | Value |
|---|---|
| Phase ID | 20 |
| Prerequisites | Phase 06 (thread ownership), Phase 03 (structured logging) |
| Estimated Complexity | High |
| Estimated File Count | 4 created, 0 modified, 1 test |
| PRD Sections | PI-25 (deadlock detection), PII-11 (thread health registry), PII-12 (watchdog timer), PII-13 (deadlock suspicion detector) |

---

## Objective

Add debug-mode deadlock detection with an `InstrumentedMutex` that logs when locks are held >100ms, a `UIWatchdog` timer that detects UI thread stalls, and a `ThreadHealthRegistry` that tracks heartbeats from all named threads.

---

## Background

The PRD mandates deadlock detection (PI-25), a thread health registry (PII-11), a watchdog timer for the UI thread (PII-12), and a deadlock suspicion detector (PII-13). Deadlocks are one of the hardest bugs to diagnose in multi-threaded applications. A watchdog timer that detects UI stalls provides critical observability for debugging hangs.

---

## Scope

### Tasks

1. **Create `src/core/ThreadHealthRegistry.h` / `ThreadHealthRegistry.cpp`**:
   ```cpp
   namespace markamp::core {

   struct ThreadInfo {
       std::thread::id id;
       std::string name;
       std::chrono::steady_clock::time_point last_heartbeat;
       std::chrono::steady_clock::time_point registered_at;
       bool stalled{false};
   };

   class ThreadHealthRegistry {
   public:
       // Register a named thread
       void register_thread(std::thread::id id, std::string name);

       // Unregister a thread
       void unregister_thread(std::thread::id id);

       // Record heartbeat from a thread
       void heartbeat(std::thread::id id);

       // Convenience: heartbeat from current thread
       void heartbeat();

       // Get threads that haven't sent a heartbeat within threshold
       [[nodiscard]] auto stalled_threads(std::chrono::milliseconds threshold)
           -> std::vector<ThreadInfo>;

       // Get all registered threads
       [[nodiscard]] auto all_threads() const -> std::vector<ThreadInfo>;

   private:
       mutable std::mutex mutex_;
       std::unordered_map<std::thread::id, ThreadInfo> threads_;
   };

   } // namespace markamp::core
   ```

2. **Create `src/core/DeadlockDetector.h` / `DeadlockDetector.cpp`**:
   ```cpp
   namespace markamp::core {

   // Instrumented mutex that logs long lock holds (debug builds only)
   class InstrumentedMutex {
   public:
       explicit InstrumentedMutex(std::string_view name = "unnamed");

       void lock();
       void unlock();
       [[nodiscard]] auto try_lock() -> bool;

       // For std::lock_guard / std::unique_lock compatibility
       // (lock/unlock interface is sufficient)

   private:
       std::mutex mutex_;
       std::string name_;
   #ifndef NDEBUG
       std::thread::id holder_;
       std::chrono::steady_clock::time_point acquired_at_;
       std::source_location acquired_location_;
   #endif
   };

   // UI thread watchdog
   class UIWatchdog {
   public:
       // Start the watchdog with a stall threshold
       void start(std::chrono::milliseconds stall_threshold = std::chrono::seconds{2});

       // Pet the watchdog (call from UI thread OnIdle)
       void pet();

       // Stop the watchdog
       void stop();

       // Check if UI is currently stalled
       [[nodiscard]] auto is_stalled() const -> bool;

   private:
       std::atomic<std::chrono::steady_clock::time_point::rep> last_pet_;
       std::chrono::milliseconds threshold_;
       std::atomic<bool> running_{false};
       std::thread watchdog_thread_;

       void watchdog_loop();
   };

   } // namespace markamp::core
   ```
   - **InstrumentedMutex**:
     - In debug builds: records acquisition time and holder thread ID
     - On `unlock()`: if held >100ms, log warning with mutex name, holder, hold duration
     - In release builds: delegates directly to `std::mutex` (zero overhead)
   - **UIWatchdog**:
     - Runs a background thread that checks if the UI thread has petted recently
     - If UI thread stall detected (>threshold): log stack dump, emit `UIThreadStallEvent`
     - Pet should be called from `OnIdle()` handler

3. **Create `tests/unit/test_thread_health.cpp`**:
   - TEST_CASE: "ThreadHealthRegistry tracks registered threads"
   - TEST_CASE: "ThreadHealthRegistry heartbeat updates timestamp"
   - TEST_CASE: "ThreadHealthRegistry detects stalled threads"
   - TEST_CASE: "ThreadHealthRegistry unregister removes thread"
   - TEST_CASE: "InstrumentedMutex locks and unlocks"
   - TEST_CASE: "InstrumentedMutex logs long hold in debug mode"
   - TEST_CASE: "InstrumentedMutex works with lock_guard"
   - TEST_CASE: "UIWatchdog detects stall"
   - TEST_CASE: "UIWatchdog does not fire when petted"
   - TEST_CASE: "UIWatchdog stop terminates cleanly"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ThreadHealthRegistry.h` |
| Create | `src/core/ThreadHealthRegistry.cpp` |
| Create | `src/core/DeadlockDetector.h` |
| Create | `src/core/DeadlockDetector.cpp` |
| Create | `tests/unit/test_thread_health.cpp` |

---

## Implementation Notes

- **InstrumentedMutex zero overhead in release**: Use `#ifndef NDEBUG` to compile out all instrumentation in release builds. The release version is just a thin wrapper around `std::mutex`.
- **Long hold detection**: 100ms threshold is configurable. Log includes: mutex name, holder thread ID, hold duration, and acquisition location (via `std::source_location`).
- **UIWatchdog thread**: The watchdog thread sleeps for `threshold/2`, then checks the atomic timestamp. If elapsed > threshold, it logs a stall warning. The pet() call atomically stores the current time.
- **Stack dump on stall**: On macOS/Linux, the watchdog can attempt to dump the UI thread's stack. This requires platform-specific code (e.g., `pthread_kill(ui_thread, SIGUSR1)` with a signal handler that captures backtrace). Keep this optional and debug-only.
- **Deadlock suspicion**: In the future, InstrumentedMutex can be extended to track lock ordering and detect circular acquisition patterns (PII-13). This is a complex feature — for now, the long-hold detection catches most practical deadlock symptoms.
- **Thread safety**: `ThreadHealthRegistry` uses a mutex for its internal map. `UIWatchdog` uses atomics for the pet timestamp.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Mutex held >100ms logs warning with mutex name and holder in debug builds
- [ ] InstrumentedMutex is zero overhead in release builds
- [ ] InstrumentedMutex works with `std::lock_guard`
- [ ] UI thread stall >2 seconds logs stack dump
- [ ] Watchdog does not fire false positives during normal operation
- [ ] ThreadHealthRegistry tracks last heartbeat per thread
- [ ] ThreadHealthRegistry correctly identifies stalled threads
- [ ] UIWatchdog::stop() terminates the watchdog thread cleanly
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test ThreadHealthRegistry with multiple threads registering/heartbeating
- Test stall detection with artificial delay (sleep in thread, check stalled_threads())
- Test InstrumentedMutex by holding a lock for >100ms and capturing log output
- Test UIWatchdog with simulated stall (don't pet for >threshold)
- Test UIWatchdog clean shutdown
- Run under TSan to verify no data races in watchdog thread
