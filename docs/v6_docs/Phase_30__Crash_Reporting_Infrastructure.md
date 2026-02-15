# Phase 30: Crash Reporting Infrastructure

## Metadata

| Field | Value |
|---|---|
| Phase ID | 30 |
| Prerequisites | Phase 03 |
| Estimated Complexity | High |
| Estimated File Count | 6 created, 3 modified |
| PRD Sections | 4.2 Crash Reporting Infrastructure (Enterprise Observability) |

---

## Objective

Add crash dump generation and symbolicated stack trace capture for post-mortem debugging across all three platforms (macOS, Linux, Windows).

---

## Background

The PRD requires: "Crash dump generation, symbolicated stack traces, minidump generation on Windows, backtrace capture on Linux/macOS." When the application crashes, a crash dump must be written to disk containing enough information to diagnose the issue without a debugger attached.

---

## Scope

### Tasks

1. **Create `src/core/CrashReporter.h` / `CrashReporter.cpp`**:
   - `CrashReporter::install()`: registers signal handlers for SIGSEGV, SIGABRT, SIGFPE, SIGBUS
   - `CrashReporter::set_crash_dir(path)`: configures output directory
   - On signal:
     1. Capture stack trace
     2. Write crash dump file
     3. Log crash info to stderr (may not have logging infrastructure)
     4. Call previous signal handler (chain)
   - Crash dump format (text): signal info, timestamp, version, stack trace with function names
   - Filename format: `crash_YYYYMMDD_HHMMSS_<pid>.txt`
   - Signal-safe operations only in the handler (no malloc, no I/O buffering)

2. **Create `src/platform/CrashHandler_Mac.cpp`**:
   - Use `backtrace()` and `backtrace_symbols()` for stack capture
   - Use `dladdr()` for symbol resolution
   - Optional: use `atos` subprocess for better symbolication (post-crash, not in handler)
   - Objective-C++ if needed for NSException handling

3. **Create `src/platform/CrashHandler_Linux.cpp`**:
   - Use `backtrace()` and `backtrace_symbols()` for stack capture
   - Use `addr2line` for symbolication (post-crash)
   - Write crash file using `write()` syscall (signal-safe)

4. **Create `src/platform/CrashHandler_Win.cpp`**:
   - Use `SetUnhandledExceptionFilter()` for structured exception handling
   - Use `MiniDumpWriteDump()` for minidump generation
   - Use `StackWalk64()` for stack trace capture
   - Use `SymFromAddr()` for symbol resolution

5. **Register handlers in `MarkAmpApp::OnInit()`**:
   - Call `CrashReporter::install()` early in startup
   - Set crash directory to `<app_data>/crash_reports/`
   - Create directory if it doesn't exist

6. **Create `tests/unit/test_crash_reporter.cpp`**:
   - Test crash dump file creation (trigger SIGABRT in child process, verify dump exists)
   - Test dump file contains stack trace
   - Test dump file contains version info
   - Test crash directory creation
   - Note: actual crash tests must run in a subprocess to avoid killing the test runner

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/CrashReporter.h` |
| Create | `src/core/CrashReporter.cpp` |
| Create | `src/platform/CrashHandler_Mac.cpp` |
| Create | `src/platform/CrashHandler_Linux.cpp` |
| Create | `src/platform/CrashHandler_Win.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/CMakeLists.txt` |
| Create | `tests/unit/test_crash_reporter.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- **Signal safety is critical**: The signal handler runs in an interrupted context. Only async-signal-safe functions may be called: `write()`, `_exit()`, `signal()`, `sigaction()`. No `malloc()`, no `printf()`, no C++ exceptions, no mutex locks.
- Pre-allocate the crash dump buffer before any crash can occur. Write using raw `write()` syscall.
- Stack trace depth: capture up to 64 frames (sufficient for most stacks).
- On macOS, `backtrace()` is reliable and available in `<execinfo.h>`. Symbol names may be mangled — demangle with `abi::__cxa_demangle()`.
- On Windows, `MiniDumpWriteDump` is the standard mechanism. It produces a .dmp file that can be opened in Visual Studio or WinDbg.
- The test must fork a child process to test the crash handler, since SIGABRT terminates the process. Use `fork()` on Unix, `CreateProcess()` on Windows.
- Consider integrating with the structured logger (Phase 29) for pre-crash context logging.

---

## Acceptance Criteria

- [ ] Crash generates a dump file in the designated directory
- [ ] Dump file includes readable stack trace with function names
- [ ] Dump file includes signal type, timestamp, and application version
- [ ] Signal handlers do not interfere with normal operation
- [ ] `test_crash_reporter` validates dump file creation (via subprocess)
- [ ] Works on macOS (primary development platform)
- [ ] Platform-specific handlers compile on their respective platforms
- [ ] No memory allocation in signal handler (signal-safe only)

---

## Testing Strategy

- Run test_crash_reporter which forks and triggers SIGABRT, then verifies dump file
- Manually trigger a crash (dereference nullptr) in debug build, verify dump
- Verify dump file is parseable and contains useful information
- Run under ASan to ensure no issues from signal handler
