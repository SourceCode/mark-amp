# Phase 04: Global Exception Barriers and Crash Dump

## Metadata

| Field | Value |
|---|---|
| Phase ID | 04 |
| Prerequisites | Phase 01 (error types), Phase 03 (structured logging) |
| Estimated Complexity | High |
| Estimated File Count | 4 created, 2 modified, 1 test |
| PRD Sections | PI-1 (global exception barrier), PI-2 (thread entry guard), PI-44 (crash dump generation), PII-50 (every crash produces actionable data) |

---

## Objective

Wrap `main()`, `wxApp::OnInit()`, and all thread entry points with exception barriers that log structured errors, generate crash dumps with stack traces, and show recovery dialogs instead of terminating. Install platform signal handlers for SIGSEGV, SIGABRT, and SIGFPE.

---

## Background

The PRD's #1 item is a global top-level exception barrier. Currently, an unhandled exception in any thread terminates the process silently. This phase ensures that every possible crash path is caught, logged with actionable data, and recovered from gracefully. The crash dump includes stack trace, subsystem context, recent log entries, and config state hash.

---

## Scope

### Tasks

1. **Create `src/core/CrashBarrier.h` / `CrashBarrier.cpp`**:
   ```cpp
   namespace markamp::core {

   // Wraps any callable in a crash barrier
   template<typename F>
   auto crash_barrier(std::string_view context_name, F&& fn) -> int;

   // Thread entry guard macro
   #define MARKAMP_THREAD_GUARD(name) \
       ::markamp::core::ThreadGuardScope _thread_guard_##__LINE__(name)

   class ThreadGuardScope {
   public:
       explicit ThreadGuardScope(std::string_view name);
       ~ThreadGuardScope();
   };

   } // namespace markamp::core
   ```
   - `crash_barrier()` catches:
     - `std::exception` — logs message, subsystem, stack trace
     - `...` — logs "unknown exception" with stack trace (NOTE: this is the ONLY place `catch(...)` is permitted per CLAUDE.md convention — document this clearly)
   - On crash: writes structured log entry, generates crash dump, shows wxMessageDialog if UI is available
   - Returns exit code (0 = success, 1 = crashed)

2. **Create `src/core/CrashDump.h` / `CrashDump.cpp`**:
   ```cpp
   namespace markamp::core {

   struct CrashReport {
       std::string timestamp;
       SubsystemId subsystem;
       std::string stack_trace;
       std::string exception_message;
       std::string config_state_hash;
       std::vector<std::string> recent_log_entries;  // Last 50
       std::string correlation_id;
   };

   class CrashDump {
   public:
       static void install_signal_handlers();
       static auto generate(const std::exception* ex = nullptr) -> CrashReport;
       static void write_to_file(const CrashReport& report);
   };

   } // namespace markamp::core
   ```
   - Platform-specific stack trace capture:
     - macOS/Linux: `backtrace()` + `backtrace_symbols()` from `<execinfo.h>`
     - Windows: `CaptureStackBackTrace()` + `SymFromAddr()`
   - Signal handlers for SIGSEGV, SIGABRT, SIGFPE (POSIX) / structured exception handling (Windows)
   - Crash dump written to `~/.markamp/crashes/crash_YYYYMMDD_HHMMSS.json`

3. **Modify `src/main.cpp`**:
   - Wrap `wxEntry()` call in `crash_barrier("main")`:
     ```cpp
     int main(int argc, char* argv[]) {
         markamp::core::CrashDump::install_signal_handlers();
         return markamp::core::crash_barrier("main", [&]() {
             return wxEntry(argc, argv);
         });
     }
     ```

4. **Modify `src/app/MarkAmpApp.cpp`**:
   - Wrap `OnInit()` body in crash barrier:
     ```cpp
     bool MarkAmpApp::OnInit() {
         return markamp::core::crash_barrier("OnInit", [this]() {
             // ... existing OnInit body ...
             return true;
         }) == 0;
     }
     ```
   - Wrap `OnUnhandledException()` to produce crash dump

5. **Create `tests/unit/test_crash_barrier.cpp`**:
   - TEST_CASE: "crash_barrier catches std::exception"
   - TEST_CASE: "crash_barrier returns 0 on success"
   - TEST_CASE: "crash_barrier returns 1 on exception"
   - TEST_CASE: "crash_barrier logs exception message"
   - TEST_CASE: "CrashReport includes stack trace"
   - TEST_CASE: "CrashReport includes correlation ID"
   - TEST_CASE: "CrashDump writes JSON to file"
   - TEST_CASE: "ThreadGuardScope catches exceptions in scope"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/CrashBarrier.h` |
| Create | `src/core/CrashBarrier.cpp` |
| Create | `src/core/CrashDump.h` |
| Create | `src/core/CrashDump.cpp` |
| Modify | `src/main.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Create | `tests/unit/test_crash_barrier.cpp` |

---

## Implementation Notes

- **Signal handler safety**: Signal handlers can only call async-signal-safe functions. The signal handler should set a flag and write a minimal crash dump. Full crash dump generation happens in the crash barrier catch block, not in the signal handler.
- **Stack trace on macOS**: `backtrace()` and `backtrace_symbols()` from `<execinfo.h>` are available. For better symbol resolution, link with `-rdynamic` flag or use `atos` for demangling.
- **Stack trace on Linux**: Same `<execinfo.h>` API. Consider `libunwind` for better accuracy in future.
- **Windows**: Use `CaptureStackBackTrace()` and `SymFromAddr()` from `<DbgHelp.h>`.
- **The ONE catch(...)**: Document clearly that `CrashBarrier.cpp` is the only file permitted to use `catch(...)`, as mandated by CLAUDE.md's "never use catch(...)" convention. This is the safety net of last resort.
- **Crash dump directory**: Create `~/.markamp/crashes/` on first crash. Use atomic write (temp + rename) from Phase 31 pattern.
- **Recent log entries**: Store last 50 log entries in a ring buffer accessible to CrashDump.
- **Config state hash**: Compute SHA256 or CRC32 of serialized config for diagnostic correlation.
- Platform-specific code should use `#ifdef __APPLE__`, `#ifdef _WIN32`, `#ifdef __linux__`.

---

## Acceptance Criteria

- [ ] Unhandled `std::exception` in `main()` produces structured log entry + crash dump file
- [ ] Thread entry guard catches and logs exceptions without terminating the process
- [ ] Crash dump JSON includes: subsystem, stack trace, recent log entries, config state hash, correlation ID
- [ ] Signal handlers installed for SIGSEGV, SIGABRT, SIGFPE on POSIX platforms
- [ ] Crash dump file written to `~/.markamp/crashes/` with timestamp in filename
- [ ] `crash_barrier()` returns appropriate exit code (0 success, 1 crash)
- [ ] `MARKAMP_THREAD_GUARD` macro is ergonomic and works in any thread function
- [ ] All 8+ test cases pass
- [ ] No segfault in signal handler (async-signal-safe only)

---

## Testing Strategy

- Test crash barrier with intentionally throwing functions
- Test CrashReport serialization (verify JSON is parseable)
- Test that stack trace contains at least the test function name
- Test ThreadGuardScope RAII behavior (exception caught, scope exits cleanly)
- Do NOT test signal handlers in unit tests (unreliable) — validate manually
- Run under ASan to verify no memory issues in crash path
