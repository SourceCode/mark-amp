# Phase 06: Thread Ownership and UI Thread Assertion

## Metadata

| Field | Value |
|---|---|
| Phase ID | 06 |
| Prerequisites | Phase 03 (structured logging) |
| Estimated Complexity | Low |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-23 (strict thread ownership rules), PI-24 (UI thread assertion macro) |

---

## Objective

Establish thread ownership rules and provide runtime assertion macros that prevent cross-thread UI access — a common source of wxWidgets crashes. The main thread ID is registered at startup and assertion macros log structured errors (debug: break, release: log + continue).

---

## Background

wxWidgets is not thread-safe for UI operations. Calling any wxWidgets UI function from a background thread leads to undefined behavior and crashes. The PRD mandates strict thread ownership rules (PI-23) and a UI thread assertion macro (PI-24). Currently there is no runtime enforcement — developers must manually remember which operations are UI-only.

---

## Scope

### Tasks

1. **Create `src/core/ThreadOwnership.h` / `ThreadOwnership.cpp`**:
   ```cpp
   namespace markamp::core {

   // Call once from MarkAmpApp::OnInit()
   void register_main_thread();

   // Query functions
   [[nodiscard]] auto is_main_thread() -> bool;
   [[nodiscard]] auto main_thread_id() -> std::thread::id;
   [[nodiscard]] auto current_thread_name() -> std::string_view;

   // Thread registration for named threads
   void register_thread(std::string name);
   void unregister_thread();

   // Assertion macros
   #define MARKAMP_ASSERT_UI_THREAD() \
       do { \
           if (!::markamp::core::is_main_thread()) { \
               MARKAMP_LOG_ERROR_S(::markamp::core::SubsystemId::Threading, \
                   "UI thread assertion failed: called from thread '{}' at {}:{}", \
                   ::markamp::core::current_thread_name(), __FILE__, __LINE__); \
               assert(false && "MARKAMP_ASSERT_UI_THREAD: not on UI thread"); \
           } \
       } while(0)

   #define MARKAMP_ASSERT_WORKER_THREAD() \
       do { \
           if (::markamp::core::is_main_thread()) { \
               MARKAMP_LOG_ERROR_S(::markamp::core::SubsystemId::Threading, \
                   "Worker thread assertion failed: called from UI thread at {}:{}", \
                   __FILE__, __LINE__); \
               assert(false && "MARKAMP_ASSERT_WORKER_THREAD: on UI thread"); \
           } \
       } while(0)

   } // namespace markamp::core
   ```
   - `register_main_thread()`: stores `std::this_thread::get_id()` in a global atomic
   - `is_main_thread()`: compares `std::this_thread::get_id()` with stored main thread ID
   - `register_thread(name)`: stores thread name in `thread_local` storage for diagnostics
   - In debug builds: `assert()` fires (debugger-friendly)
   - In release builds: `assert()` is no-op, but the log error still fires

2. **Modify `src/app/MarkAmpApp.cpp`**:
   - Add `markamp::core::register_main_thread()` as the first call in `OnInit()`
   - Add `markamp::core::register_thread("main")` immediately after

3. **Create `tests/unit/test_thread_ownership.cpp`**:
   - TEST_CASE: "register_main_thread sets main thread ID"
   - TEST_CASE: "is_main_thread returns true on main thread"
   - TEST_CASE: "is_main_thread returns false on worker thread"
   - TEST_CASE: "register_thread sets thread name"
   - TEST_CASE: "current_thread_name returns registered name"
   - TEST_CASE: "current_thread_name returns 'unnamed' for unregistered threads"
   - TEST_CASE: "unregister_thread clears name"
   - TEST_CASE: "MARKAMP_ASSERT_UI_THREAD logs error from worker thread" (capture log output)

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ThreadOwnership.h` |
| Create | `src/core/ThreadOwnership.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Create | `tests/unit/test_thread_ownership.cpp` |

---

## Implementation Notes

- Main thread ID storage: use `std::atomic<std::thread::id>` for thread-safe reads from any thread.
- Thread name storage: use `thread_local std::string` for per-thread name. Default to `"unnamed"`.
- The assert() in release builds is compiled out by the preprocessor (`NDEBUG`). The structured log error fires in both debug and release builds, providing observability without crashing in production.
- Consider adding a `MARKAMP_ASSERT_THREAD(expected_name)` macro for future use where specific thread ownership can be verified.
- The test for `MARKAMP_ASSERT_UI_THREAD` needs to spawn a worker thread, call the assertion, and verify that a log error was produced (capture log output in test).
- Update `src/CMakeLists.txt` to add `ThreadOwnership.cpp`.
- Update `tests/CMakeLists.txt` to add test target.

---

## Acceptance Criteria

- [ ] `MARKAMP_ASSERT_UI_THREAD()` fires structured log error + debug break if called from worker thread
- [ ] `MARKAMP_ASSERT_WORKER_THREAD()` fires if called from UI thread
- [ ] In release builds, assertion logs but does not abort
- [ ] In debug builds, assertion triggers `assert()` (debugger breaks)
- [ ] Main thread ID registered during `OnInit()` as first operation
- [ ] Thread names available for diagnostic logging
- [ ] `is_main_thread()` is lock-free and safe to call from any thread
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test main thread registration and query from main thread
- Test `is_main_thread()` returns false when called from `std::thread`
- Test thread name registration and retrieval
- Test assertion macro logging by capturing structured log output
- Run under TSan to verify thread-safe access to main thread ID
