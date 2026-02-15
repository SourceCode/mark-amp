# Phase 22: Thread Safety Hardening -- Exception Boundaries

## Metadata

| Field | Value |
|---|---|
| Phase ID | 22 |
| Prerequisites | Phase 20 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 4 modified |
| PRD Sections | 5.1 Strict Exception Policy |

---

## Objective

Ensure all thread boundaries have typed exception handlers and that no exception can escape a worker thread unhandled. Eliminate all `catch(...)` in the codebase.

---

## Background

The PRD mandates: "No `catch(...)`, only catch specific types, never allow exceptions to cross threads." CLAUDE.md convention also states: "Never use `catch(...)` — always use typed exception handlers." All worker threads must have exception boundaries that catch specific exception types and log them, preventing `std::terminate()` from unhandled exceptions.

---

## Scope

### Tasks

1. **Create `src/core/ThreadBoundary.h`**:
   - `thread_safe_invoke(callable)` wrapper template:
     - Invokes callable inside typed exception handlers
     - Catches `std::filesystem::filesystem_error`, `std::runtime_error`, `std::logic_error`, `std::bad_alloc`, `std::exception`
     - Logs exception type, message, and thread ID via `MARKAMP_LOG_ERROR`
     - Returns `std::expected<ReturnType, std::string>` for error propagation
     - Never uses `catch(...)`
   - `thread_entry_guard(callable)` for thread entry points:
     - Same as above but logs to FATAL and includes stack trace
     - Designed for `std::thread` entry functions

2. **Audit all worker thread entry points**:
   - `AsyncHighlighter` worker thread
   - `AsyncFileLoader` worker thread
   - `AsyncPipeline` template worker threads
   - Any `std::thread` in extension services
   - Any `std::async` calls
   - Document each entry point and its current exception handling

3. **Wrap all thread entry points with `thread_entry_guard()`**:
   - Replace bare try/catch blocks with `thread_entry_guard` wrapper
   - Ensure consistent exception handling across all threads

4. **Eliminate all `catch(...)` in the codebase**:
   - Search for `catch(...)` or `catch (...)` patterns
   - Replace each with typed handlers for the specific exceptions that can occur
   - If the exception types are unknown, catch `std::exception` (covers all standard exceptions)

5. **Create `scripts/check_catch_all.sh`**:
   - Searches entire `src/` directory for `catch\s*\(\s*\.\.\.\s*\)` regex
   - Reports file and line number for each occurrence
   - Exits non-zero if any found
   - Intended for CI gate usage

6. **Create `tests/unit/test_thread_boundary.cpp`**:
   - Test `thread_safe_invoke` catches specific exception types
   - Test error propagation via `std::expected`
   - Test `thread_entry_guard` logs exceptions correctly
   - Test that non-exception completion returns success
   - Test nested exception handling

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ThreadBoundary.h` |
| Modify | `src/core/AsyncPipeline.h` |
| Modify | `src/core/AsyncHighlighter.cpp` |
| Modify | `src/core/AsyncFileLoader.cpp` |
| Create | `scripts/check_catch_all.sh` |
| Create | `tests/unit/test_thread_boundary.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `std::expected<T, E>` (C++23) is the ideal return type for fallible operations. If not available, use a simple `Result<T>` type.
- The exception handler chain should be ordered from most specific to most general:
  1. `std::filesystem::filesystem_error` (filesystem operations)
  2. `std::bad_alloc` (memory allocation failures)
  3. `std::runtime_error` (runtime errors from library code)
  4. `std::logic_error` (programming errors — should not happen in production)
  5. `std::exception` (catch-all for standard exceptions)
- Thread ID logging: use `std::this_thread::get_id()` for identification.
- For `AsyncPipeline`, the template wraps user-provided callables. The exception boundary should be inside the pipeline, wrapping the user callable.
- The `check_catch_all.sh` script should exclude test files that intentionally test exception handling (if any).

---

## Acceptance Criteria

- [ ] Zero `catch(...)` in entire `src/` directory (verified by `check_catch_all.sh`)
- [ ] All worker threads have typed exception handlers at boundary
- [ ] `ThreadBoundary::thread_safe_invoke` catches and logs specific exception types
- [ ] `ThreadBoundary::thread_entry_guard` prevents exceptions from escaping threads
- [ ] `test_thread_boundary` validates error handling for each exception type
- [ ] No `std::terminate()` from unhandled exceptions in worker threads
- [ ] `check_catch_all.sh` exits 0 on clean codebase

---

## Testing Strategy

- Run check_catch_all.sh to verify no catch(...) remains
- Run test_thread_boundary for unit validation
- Run full test suite under TSan for thread safety
- Deliberately throw exceptions in worker threads and verify graceful handling
