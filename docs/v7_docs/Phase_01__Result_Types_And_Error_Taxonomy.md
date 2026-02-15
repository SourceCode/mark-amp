# Phase 01: Result Types and Error Taxonomy

## Metadata

| Field | Value |
|---|---|
| Phase ID | 01 |
| Prerequisites | None (first phase) |
| Estimated Complexity | Medium |
| Estimated File Count | 6 created, 0 modified, 1 test |
| PRD Sections | PI-X Cultural Rules (no silent catch, explicit failure types), PII-48 (std::expected/Result<T>), PII-49 (no silent recovery) |

---

## Objective

Establish the foundational error handling primitives that every subsequent phase depends on. Define `Result<T>`, `Error`, `ErrorCode`, `SubsystemId`, and the `MARKAMP_GUARD` family of macros. This replaces exceptions as the primary error propagation mechanism for routine failures.

---

## Background

The PRD's Cultural Rules mandate "explicit failure return types" and "no silent recovery." Currently MarkAmp uses a mix of exceptions, boolean returns, and optional values for error handling. This phase introduces a unified `Result<T>` type based on `std::expected<T, Error>` (already confirmed available -- `Config.h` uses `std::expected`). All subsequent phases depend on these types.

---

## Scope

### Tasks

1. **Create `src/core/SubsystemId.h`**:
   - Scoped enum identifying every fault domain in the system
   - Values: `Rendering`, `ExtensionHost`, `Markdown`, `YAML`, `Workspace`, `Network`, `Editor`, `Config`, `Canvas`, `Search`, `Threading`, `Persistence`, `Telemetry`, `UI`, `Unknown`
   - `auto to_string(SubsystemId) -> std::string_view` helper

2. **Create `src/core/ErrorCode.h`**:
   - Scoped enum of error codes organized by subsystem prefix
   - Groups: `General` (InvalidArgument, NullPointer, Timeout, Cancelled, NotFound, AlreadyExists, PermissionDenied), `IO` (FileNotFound, ReadFailed, WriteFailed, PathTraversal, SymlinkEscape), `Parse` (InvalidYAML, InvalidJSON, InvalidManifest, NestingDepthExceeded, SizeLimitExceeded), `Extension` (ActivationFailed, DeactivationFailed, Quarantined, ApiVersionMismatch, SandboxViolation), `Memory` (AllocationFailed, ContainerOverflow, ResourceLimitReached), `Thread` (CancellationRequested, DeadlockSuspected, ThreadPanic)
   - `auto to_string(ErrorCode) -> std::string_view` helper

3. **Create `src/core/Error.h` / `Error.cpp`**:
   ```cpp
   namespace markamp::core {

   enum class Severity { Debug, Info, Warning, Error, Fatal };

   struct Error {
       ErrorCode code;
       SubsystemId subsystem;
       Severity severity{Severity::Error};
       std::string message;
       std::string correlation_id;
       std::source_location location{std::source_location::current()};

       [[nodiscard]] auto to_string() const -> std::string;
       [[nodiscard]] auto to_json() const -> std::string;
   };

   } // namespace markamp::core
   ```

4. **Create `src/core/Result.h`**:
   ```cpp
   namespace markamp::core {

   template<typename T>
   using Result = std::expected<T, Error>;

   // Factory functions
   template<typename T>
   auto make_ok(T&& value) -> Result<T>;

   auto make_error(ErrorCode code, SubsystemId subsystem,
                   std::string message,
                   std::source_location loc = std::source_location::current()) -> Error;

   auto make_void_error(ErrorCode code, SubsystemId subsystem,
                        std::string message,
                        std::source_location loc = std::source_location::current()) -> Result<void>;

   } // namespace markamp::core
   ```

5. **Create `src/core/Guards.h`**:
   ```cpp
   // MARKAMP_GUARD(expr) -- if expr is false, return Error
   #define MARKAMP_GUARD(expr, code, subsystem, msg) \
       do { if (!(expr)) return ::markamp::core::make_error(code, subsystem, msg); } while(0)

   // MARKAMP_GUARD_PTR(ptr) -- if ptr is null, return Error
   #define MARKAMP_GUARD_PTR(ptr, subsystem) \
       do { if ((ptr) == nullptr) return ::markamp::core::make_error( \
           ::markamp::core::ErrorCode::NullPointer, subsystem, \
           "Null pointer: " #ptr); } while(0)

   // MARKAMP_TRY(result_expr) -- propagate error if result is unexpected
   #define MARKAMP_TRY(var, result_expr) \
       auto _tmp_##var = (result_expr); \
       if (!_tmp_##var.has_value()) return std::unexpected(_tmp_##var.error()); \
       auto var = std::move(_tmp_##var.value())
   ```

6. **Create `tests/unit/test_result_types.cpp`**:
   - TEST_CASE: "Result<T> holds success value"
   - TEST_CASE: "Result<T> holds error"
   - TEST_CASE: "Result<T> works with move-only types"
   - TEST_CASE: "Error includes subsystem and severity"
   - TEST_CASE: "Error::to_string produces readable output"
   - TEST_CASE: "Error::to_json produces valid JSON"
   - TEST_CASE: "MARKAMP_GUARD returns error on false condition"
   - TEST_CASE: "MARKAMP_GUARD passes on true condition"
   - TEST_CASE: "MARKAMP_GUARD_PTR returns error on nullptr"
   - TEST_CASE: "MARKAMP_GUARD_PTR passes on valid pointer"
   - TEST_CASE: "MARKAMP_TRY propagates error"
   - TEST_CASE: "MARKAMP_TRY extracts value on success"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/SubsystemId.h` |
| Create | `src/core/ErrorCode.h` |
| Create | `src/core/Error.h` |
| Create | `src/core/Error.cpp` |
| Create | `src/core/Result.h` |
| Create | `src/core/Guards.h` |
| Create | `tests/unit/test_result_types.cpp` |

---

## Implementation Notes

- `std::expected` is confirmed available -- `Config.h` already uses it in this codebase.
- Use `std::source_location::current()` as a default argument for automatic file/line capture.
- `MARKAMP_TRY` should generate unique variable names using `__LINE__` or `__COUNTER__` to avoid shadowing.
- `Error::to_json()` should use manual string formatting (not nlohmann::json dependency) to keep this header lightweight.
- `SubsystemId` values must match the fault domain names used in Phase 38.
- `Result<void>` needs special handling -- use `std::expected<void, Error>`.
- All new code in `namespace markamp::core`.
- Update `src/CMakeLists.txt` to add `Error.cpp` to the build.
- Update `tests/CMakeLists.txt` to add the test target.

---

## Acceptance Criteria

- [ ] `Result<T>` compiles and works with move-only types (e.g., `Result<std::unique_ptr<int>>`)
- [ ] `Result<void>` works for functions that return success/failure without a value
- [ ] `Error` includes subsystem, severity, code, message, correlation_id, source_location
- [ ] `Error::to_string()` produces human-readable output
- [ ] `Error::to_json()` produces parseable JSON
- [ ] `MARKAMP_GUARD_PTR` returns Error on nullptr without crash
- [ ] `MARKAMP_TRY` propagates errors like Rust's `?` operator
- [ ] All 12+ test cases pass
- [ ] Code compiles with C++23 standard
- [ ] No new warnings introduced

---

## Testing Strategy

- Unit tests cover all success and failure paths for Result<T>
- Test with move-only types to verify no accidental copies
- Test Error serialization round-trip (to_json -> parse -> verify fields)
- Test MARKAMP_TRY with nested function calls that chain errors
- Build with -Werror to catch any implicit conversion issues
