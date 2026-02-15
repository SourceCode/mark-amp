# MarkAmp v7 Resilience Hardening -- 40-Phase Implementation Plan

## Plan Overview

This document maps the ~100 PRD items from the MarkAmp v7 Resilience Hardening PRD (Phase I: 50 Defensive Programming Improvements + Phase II: 50 Advanced Containment & OTLP items) into exactly 40 implementation phases optimized for AI coding agents.

Each phase targets 3-12 files of production code (200-800 lines) plus one Catch2 test file.

## PRD Gap Analysis & Validation

### Coverage Verification

All 100 PRD items are mapped. The following items have natural overlaps that are consolidated:

| PRD Overlap | Resolution |
|---|---|
| PI-50 (Correlation IDs) + PII-33 (Correlation ID Propagation) | Consolidated into Phase 08 |
| PI-45 (Safe Mode) + PII-36/37/38 (Multi-tier Safe Mode) | Consolidated into Phase 36 |
| PI-29 (Per-plugin error counter) partially exists in ExtensionHostRecovery.h | Phase 22 enhances existing code |
| PI-16 (Bounded containers) + PII-10 (Hard caps) | Phase 15 covers both |
| PI-26 (Queue backpressure) + PII-8 (Dynamic backpressure) + PII-25 (Rate limiting) | Phase 19 covers all |
| PI-3 (EventBus isolation) already partially implemented in EventBus::publish() | Phase 05 hardens and universalizes |

### Identified PRD Gaps

The following concerns are NOT addressed in the PRD but SHOULD be. These are included in the plan where they naturally fit:

1. **Signal handling** -- No mention of SIGTERM/SIGABRT/SIGSEGV handlers for crash dump generation. Added to Phase 04.
2. **Startup crash loop detection** -- PII-37 mentions auto-safe-mode but does not specify crash counter persistence. Added to Phase 36.
3. **Config migration safety** -- Config::migrate_from_json exists but has no corruption guard. Added to Phase 31.
4. **EventBus queue unbounded** -- `queued_events_` is an unbounded `std::vector<>`. Not called out by name. Added to Phase 05.
5. **PluginContext null-check convention** -- CLAUDE.md says "check pointer validity before dereferencing" but there is no centralized guard macro. Added to Phase 01.
6. **TextSpan lifetime (PI-22)** overlaps with GenerationCounter.h already in codebase -- plan validates and extends rather than reimplements.

### Architectural Risks

| Risk | Mitigation |
|---|---|
| OTLP C++ SDK is heavyweight (~50 headers, gRPC dependency) | Phase 33 wraps behind ITelemetryExporter interface; local-only mode is default |
| WAL for workspace state (PII-16/17) conflicts with existing Config save pattern | Phase 32 builds WAL alongside Config, does not replace it |
| `std::expected` requires C++23 -- verify all build targets support it | Config.h already uses `std::expected`; confirmed safe |
| Thread watchdog (PII-12) on macOS needs platform-specific implementation | Phase 20 uses wxWidgets timer + platform abstraction |
| Chaos testing (PII-43-45) can destabilize CI | Phase 39 uses CMake option `MARKAMP_ENABLE_CHAOS` disabled by default |

---

## Dependency Graph Summary

```
Phase 01 (Result/Error types) -----> ALL subsequent phases
Phase 02 (Validation utilities) ---> Phases 07-14, 22-26
Phase 03 (Structured logging) -----> Phases 04-40
Phase 04 (Crash dump/signals) -----> Phases 36, 37
Phase 05 (EventBus hardening) -----> Phases 06, 19, 22
Phase 06 (Thread guards) ----------> Phases 17-20
Phase 07-14 (Input validation) ----> Independent of each other
Phase 15-16 (Memory safety) -------> Phases 27, 28
Phase 17-20 (Threading) -----------> Phases 22-26
Phase 22-26 (Plugin isolation) ----> Phase 34
Phase 27-30 (Rendering/UI) -------> Phase 36
Phase 31-32 (State/persistence) --> Phase 36
Phase 33-35 (OTLP) ---------------> Phase 37
Phase 36-38 (Safe mode/recovery) -> Phase 40
Phase 39 (Chaos testing) ---------> Phase 40
Phase 40 (Final validation) -------> None
```

---

## Phase Inventory

### TIER 1: FOUNDATION (Phases 01-03)

---

### Phase 01 -- Result Types and Error Taxonomy

**Objective:** Establish the foundational error handling primitives that every subsequent phase depends on. Define `Result<T>`, `Error`, `ErrorCode`, `SubsystemId`, and the `MARKAMP_GUARD` family of macros.

**PRD Items:** PI-X Cultural Rules (no silent catch, explicit failure types), PII-48 (std::expected/Result<T>), PII-49 (no silent recovery)

**Prerequisites:** None (first phase)

**Deliverables:**
- `src/core/Error.h` -- Error struct with code, message, subsystem, severity, correlation_id
- `src/core/Error.cpp` -- Error formatting, to_string, to_json
- `src/core/Result.h` -- `Result<T>` alias over `std::expected<T, Error>`, convenience factories
- `src/core/SubsystemId.h` -- Enum of all subsystem identifiers (Rendering, ExtensionHost, Markdown, YAML, Workspace, Network, Editor, Config, Canvas, Search)
- `src/core/ErrorCode.h` -- Scoped enum of error codes per subsystem
- `src/core/Guards.h` -- `MARKAMP_GUARD(expr)`, `MARKAMP_GUARD_PTR(ptr)`, `MARKAMP_TRY(result_expr)` macros
- `tests/unit/test_result_types.cpp` -- 12+ test cases

**Acceptance Criteria:**
- `Result<T>` compiles and works with move-only types
- Error includes subsystem, severity, code, message, correlation_id
- `MARKAMP_GUARD_PTR` returns Error on nullptr without crash
- `MARKAMP_TRY` propagates errors like Rust's `?` operator
- All tests pass

**Files Created:** 6 production + 1 test = 7 files
**Estimated Complexity:** Medium (400 lines production code)

---

### Phase 02 -- Centralized Input Validation Utilities

**Objective:** Build the shared validation library that all input-facing code will use. Replaces scattered validation with consistent, tested validators.

**PRD Items:** PI-7 (centralized validators), PI-8 (all external data untrusted), PI-14 (enum validation)

**Prerequisites:** Phase 01

**Deliverables:**
- `src/core/Validation.h` -- Namespace `markamp::core::validation` with validators
- `src/core/Validation.cpp` -- Implementation
- `src/core/PathValidator.h` -- Safe path canonicalization, traversal rejection, symlink escape detection
- `src/core/PathValidator.cpp` -- Implementation using `std::error_code` overloads
- `tests/unit/test_validation.cpp` -- 15+ test cases

**Validators to implement:**
```cpp
auto validate_path(std::string_view path, std::string_view workspace_root) -> Result<std::filesystem::path>;
auto validate_yaml_key(std::string_view key) -> Result<std::string>;
auto validate_string_length(std::string_view s, size_t max_len) -> Result<std::string_view>;
auto validate_numeric_range(int value, int min, int max) -> Result<int>;
auto validate_enum_string(std::string_view value, std::span<const std::string_view> valid_values) -> Result<std::string_view>;
auto canonicalize_path(const std::filesystem::path& path, const std::filesystem::path& root) -> Result<std::filesystem::path>;
```

**Acceptance Criteria:**
- Path traversal attacks (../../etc/passwd) rejected
- Symlink escapes detected on all platforms
- YAML keys validated for length, character set
- Enum string validation rejects unknown values
- All tests pass

**Files Created/Modified:** 4 production + 1 test = 5 files
**Estimated Complexity:** Medium (500 lines production code)

---

### Phase 03 -- Structured Logging Foundation

**Objective:** Replace the simple spdlog macro wrappers with structured JSON logging that includes timestamp, thread ID, subsystem, severity, and correlation ID. This is the observability foundation for all subsequent phases.

**PRD Items:** PI-43 (structured log format), PI-50/PII-33 (correlation IDs), PII-34 (structured JSON log mirror), PII-49 (no silent recovery -- all recoveries must log)

**Prerequisites:** Phase 01 (SubsystemId, Error types)

**Deliverables:**
- `src/core/StructuredLogger.h` -- StructuredLogger class with JSON output
- `src/core/StructuredLogger.cpp` -- Implementation
- `src/core/CorrelationId.h` -- UUID-based correlation ID generation and thread-local propagation
- `src/core/CorrelationId.cpp` -- Implementation
- Modify `src/core/Logger.h` -- Extend MARKAMP_LOG_* macros to route through structured logger
- Modify `src/core/Logger.cpp` -- Wire structured logger initialization
- `tests/unit/test_structured_logger.cpp` -- 10+ test cases

**Log entry schema:**
```json
{
  "ts": "2026-02-15T10:30:00.123Z",
  "level": "WARN",
  "subsystem": "ExtensionHost",
  "thread_id": 12345,
  "correlation_id": "a1b2c3d4",
  "message": "Plugin activation failed",
  "error_code": "EXT_ACTIVATION_FAILED",
  "context": {}
}
```

**Acceptance Criteria:**
- All existing MARKAMP_LOG_* macros continue to work (backward compatible)
- JSON log output to file alongside human-readable console output
- Correlation ID propagated via thread-local storage
- Log rotation at 10 MB file size
- All tests pass

**Files Created/Modified:** 4 new + 2 modified + 1 test = 7 files
**Estimated Complexity:** Medium (450 lines production code)

---

### TIER 2: CRASH CONTAINMENT (Phases 04-06)

---

### Phase 04 -- Global Exception Barriers and Crash Dump

**Objective:** Wrap main(), wxApp::OnInit(), and thread entry points with exception barriers that log, generate crash dumps, and show recovery dialogs instead of terminating.

**PRD Items:** PI-1 (global exception barrier), PI-2 (thread entry guard), PI-44 (crash dump generation), PII-50 (every crash produces actionable data)

**Prerequisites:** Phase 03 (structured logging), Phase 01 (error types)

**Deliverables:**
- `src/core/CrashBarrier.h` -- `crash_barrier(fn)` template, `MARKAMP_THREAD_GUARD(name)` macro
- `src/core/CrashBarrier.cpp` -- Implementation with platform signal handlers
- `src/core/CrashDump.h` -- Crash dump generation (stack trace, context snapshot)
- `src/core/CrashDump.cpp` -- Platform-specific implementation (backtrace on POSIX, MiniDumpWriteDump on Windows)
- Modify `src/main.cpp` -- Wrap wxEntry() in crash barrier
- Modify `src/app/MarkAmpApp.cpp` -- Wrap OnInit() in crash barrier
- `tests/unit/test_crash_barrier.cpp` -- 8+ test cases

**Acceptance Criteria:**
- Unhandled exception in main() produces structured log + crash dump file
- Thread entry guard catches and logs exceptions without terminating process
- Crash dump includes: subsystem, stack trace, recent log entries, config state hash
- Signal handlers installed for SIGSEGV, SIGABRT, SIGFPE on POSIX
- All tests pass

**Files Created/Modified:** 4 new + 2 modified + 1 test = 7 files
**Estimated Complexity:** High (600 lines production code, platform-specific)

---

### Phase 05 -- EventBus Resilience Hardening

**Objective:** Harden the EventBus -- the backbone of all inter-component communication -- with subscriber isolation, queue bounds, and error reporting. Currently `publish()` and `publish_fast()` have basic try/catch but `queued_events_` is unbounded and there is no subscriber identification in error logs.

**PRD Items:** PI-3 (EventBus subscriber isolation), PI-4 (UI message loop guard), PI-16 partial (bounded event queue)

**Prerequisites:** Phase 01 (error types), Phase 03 (structured logging), Phase 04 (crash barrier)

**Deliverables:**
- Modify `src/core/EventBus.h` -- Add subscriber name, bounded queue, error counting
- Modify `src/core/EventBus.cpp` -- Enhanced publish/publish_fast with subscriber isolation
- `src/core/EventBusMetrics.h` -- Metrics struct (events dispatched, errors, queue depth)
- `tests/unit/test_eventbus_resilience.cpp` -- 12+ test cases

**Key changes:**
- `subscribe()` accepts optional `std::string_view subscriber_name` for diagnostics
- `queued_events_` bounded to configurable max (default 4096)
- Each handler invocation wrapped with try/catch that logs subscriber name + event type
- Handler error count tracked per subscriber
- Handler disabled after N consecutive failures (configurable, default 10)

**Acceptance Criteria:**
- Throwing handler does not prevent other handlers from executing
- Subscriber name appears in error logs
- Queue overflow drops oldest events and logs warning
- Handler auto-disable after error threshold
- Existing subscribe/publish API backward compatible
- All tests pass

**Files Created/Modified:** 2 modified + 1 new + 1 test = 4 files
**Estimated Complexity:** Medium (350 lines changes)

---

### Phase 06 -- Thread Ownership and UI Thread Assertion

**Objective:** Establish thread ownership rules and provide runtime assertion macros that prevent cross-thread UI access -- a common source of wxWidgets crashes.

**PRD Items:** PI-23 (strict thread ownership rules), PI-24 (UI thread assertion macro)

**Prerequisites:** Phase 03 (structured logging)

**Deliverables:**
- `src/core/ThreadOwnership.h` -- `MARKAMP_ASSERT_UI_THREAD()`, `MARKAMP_ASSERT_WORKER_THREAD()`, thread registration
- `src/core/ThreadOwnership.cpp` -- Implementation (stores main thread ID at init)
- Modify `src/app/MarkAmpApp.cpp` -- Register main thread ID on startup
- `tests/unit/test_thread_ownership.cpp` -- 8+ test cases

**Key APIs:**
```cpp
void register_main_thread();
auto is_main_thread() -> bool;
#define MARKAMP_ASSERT_UI_THREAD() ...
#define MARKAMP_ASSERT_WORKER_THREAD() ...
```

**Acceptance Criteria:**
- `MARKAMP_ASSERT_UI_THREAD()` fires structured log error + debug break if called from worker thread
- `MARKAMP_ASSERT_WORKER_THREAD()` fires if called from UI thread
- In release builds, assertion logs but does not abort
- Main thread ID registered during OnInit()
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Low (200 lines production code)

---

### TIER 3: INPUT VALIDATION (Phases 07-14)

---

### Phase 07 -- String Length Guards

**Objective:** Enforce maximum length limits on all user-facing strings to prevent memory exhaustion and display corruption.

**PRD Items:** PI-9 (length guards on all strings)

**Prerequisites:** Phase 02 (validation utilities)

**Deliverables:**
- `src/core/StringGuards.h` -- `truncate_safe()`, `validate_display_string()`, length constants
- `src/core/StringGuards.cpp` -- Implementation
- Modify `src/core/OutputChannelService.cpp` -- Apply string length guards
- Modify `src/core/NotificationService.cpp` -- Apply string length guards
- Modify `src/core/Command.cpp` -- Apply string length guards to palette entries
- `tests/unit/test_string_guards.cpp` -- 10+ test cases

**Length limits:**
| Context | Max Length |
|---|---|
| Output channel message | 1 MB |
| YAML key | 256 chars |
| Notification message | 4096 chars |
| Command palette entry | 512 chars |
| Config value string | 64 KB |
| Extension ID | 256 chars |
| File path | 4096 chars |

**Acceptance Criteria:**
- Strings exceeding limits are truncated with "[truncated]" suffix
- Truncation logged at DEBUG level
- Existing functionality unchanged for normal-length strings
- All tests pass

**Files Created/Modified:** 2 new + 3 modified + 1 test = 6 files
**Estimated Complexity:** Low (250 lines production code)

---

### Phase 08 -- Filesystem Canonicalization Wrapper

**Objective:** Create a safe path resolution layer that prevents directory traversal, detects symlink escapes, and uses error_code overloads exclusively.

**PRD Items:** PI-10 (filesystem canonicalization), PI-31 (sandbox filesystem restrictions)

**Prerequisites:** Phase 02 (PathValidator)

**Deliverables:**
- `src/core/SafePath.h` -- SafePath wrapper class
- `src/core/SafePath.cpp` -- Implementation
- `tests/unit/test_safe_path.cpp` -- 12+ test cases

**Key APIs:**
```cpp
auto resolve(const fs::path& requested, const fs::path& allowed_root) -> Result<fs::path>;
auto is_within_boundary(const fs::path& path, const fs::path& boundary) -> bool;
auto detect_symlink_escape(const fs::path& path, const fs::path& boundary) -> Result<fs::path>;
```

**Acceptance Criteria:**
- `../../../etc/passwd` rejected
- Symlink pointing outside workspace detected
- All fs operations use error_code overloads (no exceptions)
- Works correctly on macOS (case-insensitive HFS+), Linux (case-sensitive), Windows (NTFS)
- All tests pass

**Files Created/Modified:** 2 new + 1 test = 3 files
**Estimated Complexity:** Medium (350 lines production code)

---

### Phase 09 -- Bounded JSON/YAML Parsing

**Objective:** Enforce size and depth limits on all JSON/YAML parsing to prevent OOM from malicious or corrupted config files.

**PRD Items:** PI-11 (bounded JSON/YAML parsing)

**Prerequisites:** Phase 01 (error types), Phase 02 (validation)

**Deliverables:**
- `src/core/BoundedParser.h` -- Safe YAML/JSON parsing wrappers with limits
- `src/core/BoundedParser.cpp` -- Implementation
- Modify `src/core/Config.cpp` -- Use bounded parsing for config load
- Modify `src/core/ExtensionManifest.cpp` -- Use bounded parsing for manifest load
- `tests/unit/test_bounded_parser.cpp` -- 10+ test cases

**Limits:**
| Parameter | Default Limit |
|---|---|
| File size | 10 MB |
| YAML nesting depth | 20 levels |
| JSON nesting depth | 50 levels |
| Number of keys | 10,000 |
| String value length | 1 MB |

**Acceptance Criteria:**
- 100 MB YAML file rejected before fully loading
- Deeply nested YAML (100 levels) rejected
- Error includes specific limit that was exceeded
- Config loading continues to work for all valid configs
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (300 lines production code)

---

### Phase 10 -- Safe Regex Execution

**Objective:** Wrap all regex operations with timeout guards and exception handling to prevent catastrophic backtracking.

**PRD Items:** PI-12 (safe regex execution)

**Prerequisites:** Phase 01 (Result types)

**Deliverables:**
- `src/core/SafeRegex.h` -- `safe_regex_match()`, `safe_regex_search()`, `safe_regex_replace()` with timeout
- `src/core/SafeRegex.cpp` -- Implementation using std::chrono deadline
- Modify `src/core/IncrementalSearcher.cpp` -- Use safe regex
- Modify `src/core/SearchQueryParser.cpp` -- Use safe regex
- `tests/unit/test_safe_regex.cpp` -- 10+ test cases

**Key APIs:**
```cpp
auto safe_regex_search(std::string_view input, const std::regex& pattern,
                       std::chrono::milliseconds timeout = 100ms) -> Result<std::smatch>;
```

**Acceptance Criteria:**
- Catastrophic backtracking pattern (e.g., `(a+)+b` on "aaaa...") returns timeout error
- std::regex_error caught and wrapped in Result
- Default timeout of 100ms per operation
- Existing search functionality unchanged for normal patterns
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (300 lines production code)

---

### Phase 11 -- Numeric Range Enforcement

**Objective:** Clamp all user-configurable numeric values to valid ranges, preventing integer overflow in downstream calculations.

**PRD Items:** PI-13 (numeric range enforcement), PI-20 (integer overflow guards), PI-21 (underflow prevention)

**Prerequisites:** Phase 01 (Result types)

**Deliverables:**
- `src/core/NumericGuards.h` -- `clamp_config()`, `checked_add()`, `checked_multiply()`, `safe_subtract()`
- `src/core/NumericGuards.cpp` -- Implementation
- Modify `src/core/Config.cpp` -- Apply clamping on get_int/get_double
- `tests/unit/test_numeric_guards.cpp` -- 12+ test cases

**Clamping ranges:**
| Setting | Min | Max |
|---|---|---|
| font_size | 6 | 128 |
| tab_size | 1 | 16 |
| edge_column | 1 | 500 |
| auto_save_interval_seconds | 5 | 3600 |
| cursor_width | 1 | 10 |
| word_wrap_column | 20 | 1000 |
| letter_spacing | -5.0 | 20.0 |

**Acceptance Criteria:**
- font_size=0 clamped to 6 with log warning
- font_size=9999 clamped to 128 with log warning
- `checked_add(INT_MAX, 1)` returns overflow error
- `safe_subtract(0u, 1u)` returns 0 (not wrap-around)
- Existing config values unchanged if within range
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Low (250 lines production code)

---

### Phase 12 -- Extension Manifest Schema Validation

**Objective:** Validate extension manifests against a strict schema before loading, rejecting malformed or incompatible extensions early.

**PRD Items:** PI-15 (strict schema validation for ExtensionManifest), PI-32 (extension API contract versioning)

**Prerequisites:** Phase 02 (validation), Phase 09 (bounded parsing)

**Deliverables:**
- `src/core/ManifestValidator.h` -- Schema validation for ExtensionManifest
- `src/core/ManifestValidator.cpp` -- Implementation
- Modify `src/core/ExtensionManifest.cpp` -- Integrate validation into parse()
- `tests/unit/test_manifest_validator.cpp` -- 12+ test cases

**Validation rules:**
- Required fields present (name, publisher, version, engines.markamp)
- Version strings valid semver
- Activation events match known patterns
- Permission strings match ExtensionPermission enum
- API version compatibility check
- No unknown contribution point types

**Acceptance Criteria:**
- Manifest missing "name" field rejected with clear error
- Invalid semver "1.2.banana" rejected
- Unknown permission "hack_everything" rejected
- API version incompatibility detected
- Valid manifests continue to load successfully
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Medium (350 lines production code)

---

### Phase 13 -- MIME Validation and Markdown Parsing Limits

**Objective:** Add content-type validation for embedded content and hard limits on markdown parsing to prevent abuse.

**PRD Items:** PII-21 (strict MIME validation), PII-22 (markdown parsing hard limits), PII-23 (sandboxed HTML rendering), PII-24 (time-limited parsing)

**Prerequisites:** Phase 01 (Result types), Phase 10 (safe regex)

**Deliverables:**
- `src/core/ContentValidator.h` -- MIME validation, parsing limits
- `src/core/ContentValidator.cpp` -- Implementation
- Modify `src/core/MarkdownParser.cpp` -- Apply nesting depth / table size limits
- Modify `src/core/HtmlSanitizer.cpp` -- Enforce inline script blocking
- `tests/unit/test_content_validator.cpp` -- 10+ test cases

**Limits:**
| Parameter | Limit |
|---|---|
| Markdown nesting depth | 100 |
| Table columns | 200 |
| Table rows | 50,000 |
| Mermaid node count | 500 |
| Parse timeout | 5 seconds |
| Embedded content types | allowlist: image/*, text/*, application/pdf |

**Acceptance Criteria:**
- 200-level nested blockquotes rejected safely
- Table with 1 million rows rejected before OOM
- Parsing timeout produces partial result + warning
- `<script>` tags in markdown preview removed
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (350 lines production code)

---

### Phase 14 -- Configuration Value Validation Layer

**Objective:** Add comprehensive validation to Config::set() to reject invalid values before they are persisted and propagated.

**PRD Items:** PI-8 (all external data untrusted), PI-14 (enum validation), PI-13 (numeric clamping)

**Prerequisites:** Phase 02, Phase 07, Phase 11

**Deliverables:**
- `src/core/ConfigValidator.h` -- Per-key validation rules
- `src/core/ConfigValidator.cpp` -- Implementation
- Modify `src/core/Config.cpp` -- Integrate validation into set() methods
- `tests/unit/test_config_validator.cpp` -- 10+ test cases

**Acceptance Criteria:**
- `config.set("theme", "")` rejected (empty theme ID)
- `config.set("view_mode", "banana")` rejected (invalid enum)
- `config.set("font_size", -5)` clamped to minimum
- Valid values pass through unchanged
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Low (250 lines production code)

---

### TIER 4: MEMORY SAFETY (Phases 15-16)

---

### Phase 15 -- Bounded Containers and Resource Caps

**Objective:** Add explicit capacity limits to all dynamically-growing containers: event queues, logs, diagnostics buffers, and telemetry. Also enforce hard caps from PII-10.

**PRD Items:** PI-16 (all containers bounded), PII-10 (hard caps with graceful degradation)

**Prerequisites:** Phase 01 (Result types), Phase 05 (EventBus bounded queue already done)

**Deliverables:**
- `src/core/BoundedContainer.h` -- `BoundedVector<T>`, `BoundedDeque<T>` with configurable max and eviction policy
- `src/core/BoundedContainer.cpp` -- Implementation (header-mostly, .cpp for non-template helpers)
- `src/core/ResourceLimits.h` -- Centralized limit constants
- Modify `src/core/DiagnosticsService.cpp` -- Apply bounded diagnostics buffer
- Modify `src/core/OutputChannelService.cpp` -- Apply bounded output channel
- `tests/unit/test_bounded_container.cpp` -- 10+ test cases

**Limits:**
| Container | Max Size |
|---|---|
| Diagnostics entries | 10,000 |
| Output channel lines | 50,000 |
| Extension telemetry events | 5,000 |
| Open file buffers | 100 |
| Extension count | 200 |

**Acceptance Criteria:**
- BoundedVector drops oldest when at capacity
- Eviction logged at DEBUG level
- No std::bad_alloc from runaway container growth
- Hard caps produce user-visible warning when hit
- All tests pass

**Files Created/Modified:** 3 new + 2 modified + 1 test = 6 files
**Estimated Complexity:** Medium (400 lines production code)

---

### Phase 16 -- Safe Allocation and Pointer Discipline

**Objective:** Add allocation failure guards at subsystem boundaries and audit for raw owning pointers. Implement TextSpan lifetime validation.

**PRD Items:** PI-17 (safe allocation wrapper), PI-18 (no raw owning pointers), PI-22 (TextSpan validation)

**Prerequisites:** Phase 01 (Result types)

**Deliverables:**
- `src/core/SafeAlloc.h` -- `safe_make_unique<T>()`, `safe_make_shared<T>()` wrappers
- `src/core/SafeAlloc.cpp` -- Implementation (catches bad_alloc, returns Result)
- Modify `src/core/TextSpan.h` -- Add generation counter validation
- `tests/unit/test_safe_alloc.cpp` -- 8+ test cases

**Key APIs:**
```cpp
template<typename T, typename... Args>
auto safe_make_unique(Args&&... args) -> Result<std::unique_ptr<T>>;

template<typename T, typename... Args>
auto safe_make_shared(Args&&... args) -> Result<std::shared_ptr<T>>;
```

**Acceptance Criteria:**
- `safe_make_unique` catches bad_alloc and returns error
- TextSpan validates generation counter before access
- No new raw owning pointers introduced
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Low (250 lines production code)

---

### TIER 5: THREADING HARDENING (Phases 17-21)

---

### Phase 17 -- Async Task Cancellation Tokens

**Objective:** Implement a universal cancellation token pattern for all async operations, replacing the per-class atomic<bool> pattern used in AsyncFileLoader.

**PRD Items:** PI-27 (async task cancellation tokens), PII-14 (safe task cancellation enforcement)

**Prerequisites:** Phase 06 (thread ownership)

**Deliverables:**
- `src/core/CancellationToken.h` -- CancellationToken class with token source
- `src/core/CancellationToken.cpp` -- Implementation
- Modify `src/core/AsyncFileLoader.h` -- Adopt CancellationToken
- Modify `src/core/AsyncFileLoader.cpp` -- Replace atomic<bool> with token
- Modify `src/core/AsyncHighlighter.h` -- Adopt CancellationToken
- `tests/unit/test_cancellation_token.cpp` -- 10+ test cases

**Key APIs:**
```cpp
class CancellationTokenSource {
    auto token() -> CancellationToken;
    void cancel();
};
class CancellationToken {
    [[nodiscard]] auto is_cancelled() const -> bool;
    void throw_if_cancelled() const;
    auto on_cancelled(std::function<void()> callback) -> Subscription;
};
```

**Acceptance Criteria:**
- Token cancellation is thread-safe
- Callback fires on cancellation
- AsyncFileLoader uses CancellationToken instead of raw atomic
- Multiple consumers can share one token
- All tests pass

**Files Created/Modified:** 2 new + 4 modified + 1 test = 7 files
**Estimated Complexity:** Medium (350 lines production code)

---

### Phase 18 -- Generation-Based Invalidation

**Objective:** Extend the existing GenerationCounter pattern to be universally available and enforce generation checks at all cross-thread data access points.

**PRD Items:** PI-28 (generation-based invalidation), PI-19 (no cross-thread shared raw references)

**Prerequisites:** Phase 06 (thread ownership)

**Deliverables:**
- Modify `src/core/GenerationCounter.h` -- Enhance with `ValidatedRef<T>` wrapper
- `src/core/ValidatedRef.h` -- Generation-validated reference that returns error on stale access
- `src/core/ValidatedRef.cpp` -- Implementation
- `tests/unit/test_validated_ref.cpp` -- 10+ test cases

**Key APIs:**
```cpp
template<typename T>
class ValidatedRef {
    auto get() const -> Result<const T&>;
    auto is_valid() const -> bool;
};
```

**Acceptance Criteria:**
- Accessing a ValidatedRef after source invalidation returns error (not crash)
- Generation counter is atomic and lock-free
- Works with immutable snapshots pattern used in EventBus
- All tests pass

**Files Created/Modified:** 1 modified + 2 new + 1 test = 4 files
**Estimated Complexity:** Medium (300 lines production code)

---

### Phase 19 -- Queue Backpressure and Rate Limiting

**Objective:** Add backpressure to the EventBus and all async pipelines. Implement event coalescing for high-frequency events and rate limiting for flood protection.

**PRD Items:** PI-26 (queue backpressure), PII-8 (dynamic backpressure scaling), PII-25 (rate-limited event flood protection)

**Prerequisites:** Phase 05 (EventBus hardening)

**Deliverables:**
- `src/core/Backpressure.h` -- BackpressurePolicy, EventCoalescer, RateLimiter
- `src/core/Backpressure.cpp` -- Implementation
- Modify `src/core/EventBus.h` -- Add backpressure support to queue()
- Modify `src/core/EventBus.cpp` -- Implement coalescing for high-frequency events
- `tests/unit/test_backpressure.cpp` -- 10+ test cases

**Coalescing rules:**
- CursorPositionChangedEvent: keep only latest
- EditorScrollChangedEvent: keep only latest
- FileContentChangedEvent: keep only latest per file_id
- Debounce threshold: 16ms for UI events

**Acceptance Criteria:**
- 10,000 scroll events per second reduced to ~60 dispatches
- Rate limiter blocks > 1000 events/sec per event type from plugins
- Backpressure metrics exposed (dropped count, coalesced count)
- Normal event flow unaffected
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** High (500 lines production code)

---

### Phase 20 -- Deadlock Detection and UI Watchdog

**Objective:** Add debug-mode deadlock detection for mutex holds >100ms and a watchdog timer for the UI thread.

**PRD Items:** PI-25 (deadlock detection), PII-11 (thread health registry), PII-12 (watchdog timer), PII-13 (deadlock suspicion detector)

**Prerequisites:** Phase 06 (thread ownership), Phase 03 (structured logging)

**Deliverables:**
- `src/core/ThreadHealthRegistry.h` -- Thread health monitoring
- `src/core/ThreadHealthRegistry.cpp` -- Implementation
- `src/core/DeadlockDetector.h` -- Instrumented mutex wrapper, UI watchdog
- `src/core/DeadlockDetector.cpp` -- Implementation
- `tests/unit/test_thread_health.cpp` -- 10+ test cases

**Key APIs:**
```cpp
class InstrumentedMutex {
    void lock();    // Records acquisition time, logs if held >100ms
    void unlock();
};
class UIWatchdog {
    void pet();     // Called from OnIdle
    void start(std::chrono::milliseconds threshold);
};
class ThreadHealthRegistry {
    void register_thread(std::string name);
    void heartbeat(std::thread::id tid);
    auto stalled_threads(std::chrono::milliseconds threshold) -> std::vector<ThreadInfo>;
};
```

**Acceptance Criteria:**
- Mutex held >100ms logs warning with holder stack in debug builds
- UI thread stall >2 seconds logs stack dump
- Thread health registry tracks last heartbeat per thread
- Watchdog does not fire false positives during normal operation
- InstrumentedMutex is no-op in release builds (zero overhead)
- All tests pass

**Files Created/Modified:** 4 new + 1 test = 5 files
**Estimated Complexity:** High (500 lines production code)

---

### Phase 21 -- Thread Panic Escalation Policy

**Objective:** Define and implement the escalation policy when a background thread fails: log, restart, notify user -- never terminate the process.

**PRD Items:** PII-15 (thread panic escalation policy)

**Prerequisites:** Phase 20 (thread health registry), Phase 04 (crash barrier)

**Deliverables:**
- `src/core/ThreadPanicPolicy.h` -- Escalation policy engine
- `src/core/ThreadPanicPolicy.cpp` -- Implementation
- Modify `src/core/AsyncFileLoader.cpp` -- Apply panic policy
- Modify `src/core/AsyncHighlighter.cpp` -- Apply panic policy
- `tests/unit/test_thread_panic.cpp` -- 8+ test cases

**Escalation levels:**
1. Log + retry (automatic)
2. Log + disable feature + notify user
3. Log + dump thread state + offer restart

**Acceptance Criteria:**
- Background thread crash triggers retry (level 1)
- 3 consecutive crashes escalate to level 2
- Thread state dumped at level 3
- Process never terminates from background thread failure
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (350 lines production code)

---

### TIER 6: PLUGIN ISOLATION (Phases 22-26)

---

### Phase 22 -- Plugin Error Counter and Auto-Disable

**Objective:** Enhance the existing ExtensionHostRecovery with per-plugin error counters, execution time monitoring, and structured error reporting. ExtensionHostRecovery already exists but needs to be wired to the structured logging and error taxonomy from Phase 01.

**PRD Items:** PI-29 (per-plugin error counter), PI-30 (execution time monitoring), PI-5 (plugin activation isolation), PI-6 (plugin deactivation isolation)

**Prerequisites:** Phase 01 (error types), Phase 03 (structured logging), Phase 05 (EventBus)

**Deliverables:**
- Modify `src/core/ExtensionHostRecovery.h` -- Add execution time tracking, structured error reporting
- Modify `src/core/ExtensionHostRecovery.cpp` -- Integrate with StructuredLogger
- Modify `src/core/PluginManager.cpp` -- Wrap activate/deactivate in isolation barriers
- `tests/unit/test_plugin_isolation.cpp` -- 12+ test cases

**Key enhancements:**
- `execute_safely()` records execution time, logs slow operations (>1s)
- Plugin activation failure marks plugin as faulted, continues startup
- Plugin deactivation failure does not cascade to other plugins
- Error history includes correlation IDs from Phase 03

**Acceptance Criteria:**
- Plugin throwing during activation is marked faulted, other plugins activate
- Plugin throwing during deactivation does not prevent app shutdown
- Slow plugin (>1s execution) logged with timing
- Plugin disabled after 3 failures, can be re-enabled
- All tests pass

**Files Created/Modified:** 3 modified + 1 test = 4 files
**Estimated Complexity:** Medium (350 lines changes)

---

### Phase 23 -- Plugin Filesystem Sandbox Enforcement

**Objective:** Enforce that extensions can only access files within their allowed boundaries (extension directory, workspace storage, global storage).

**PRD Items:** PI-31 (sandbox filesystem restrictions), PI-33 (fail-closed policy)

**Prerequisites:** Phase 08 (SafePath), Phase 22 (plugin isolation)

**Deliverables:**
- Modify `src/core/ExtensionSandbox.h` -- Add filesystem boundary enforcement
- Modify `src/core/ExtensionSandbox.cpp` -- Implement path validation per plugin
- `tests/unit/test_plugin_sandbox.cpp` -- 10+ test cases

**Key changes:**
- `ExtensionSandbox` gains `validate_file_access(extension_id, path)` method
- Path validation uses SafePath from Phase 08
- Extension without kFilesystem permission cannot access any filesystem API
- Fail-closed: if permission check fails, access denied

**Acceptance Criteria:**
- Extension cannot read files outside its extension_path and workspace_storage_path
- Extension without kFilesystem permission cannot call any file API
- Symlink escape from extension directory detected and blocked
- All tests pass

**Files Created/Modified:** 2 modified + 1 test = 3 files
**Estimated Complexity:** Medium (300 lines changes)

---

### Phase 24 -- Extension API Contract Versioning

**Objective:** Add API version negotiation so incompatible plugins are rejected at load time rather than crashing at runtime.

**PRD Items:** PI-32 (extension API contract versioning), PI-33 (fail-closed -- reject incompatible)

**Prerequisites:** Phase 12 (manifest validator)

**Deliverables:**
- `src/core/ApiVersion.h` -- Semver comparison, compatibility matrix
- `src/core/ApiVersion.cpp` -- Implementation
- Modify `src/core/PluginManager.cpp` -- Check API version before activation
- `tests/unit/test_api_version.cpp` -- 8+ test cases

**Acceptance Criteria:**
- Plugin requiring API v2.0 rejected on host v1.x
- Plugin requiring API v1.2 accepted on host v1.5 (minor version compatible)
- API version mismatch produces clear user-facing error
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Low (250 lines production code)

---

### Phase 25 -- Plugin Fault Quarantine

**Objective:** Implement a quarantine mechanism that prevents repeatedly-crashing plugins from auto-loading on subsequent startups.

**PRD Items:** PII-5 (plugin fault quarantine)

**Prerequisites:** Phase 22 (plugin error counter)

**Deliverables:**
- `src/core/PluginQuarantine.h` -- Quarantine list persistence and management
- `src/core/PluginQuarantine.cpp` -- Implementation
- Modify `src/core/PluginManager.cpp` -- Check quarantine before activation
- `tests/unit/test_plugin_quarantine.cpp` -- 8+ test cases

**Quarantine rules:**
- Plugin moved to quarantine after 5 failures across sessions
- Quarantine persisted to `~/.markamp/quarantine.json`
- User can manually un-quarantine from Settings
- Quarantined plugin logged with reason + last stack trace

**Acceptance Criteria:**
- Quarantined plugin skipped during activate_all()
- Quarantine list survives app restart
- Manual un-quarantine works
- All tests pass

**Files Created/Modified:** 2 new + 1 modified + 1 test = 4 files
**Estimated Complexity:** Medium (300 lines production code)

---

### Phase 26 -- Graceful Extension Host Restart

**Objective:** Enable the extension host to restart after a crash without requiring a full application restart.

**PRD Items:** PI-34 (graceful extension host restart)

**Prerequisites:** Phase 22 (plugin isolation), Phase 25 (quarantine)

**Deliverables:**
- Modify `src/core/ExtensionHostRecovery.h` -- Add restart_extension_host() method
- Modify `src/core/ExtensionHostRecovery.cpp` -- Implement deactivate-all + reactivate-healthy
- Modify `src/core/PluginManager.cpp` -- Support mid-session restart
- `tests/unit/test_extension_host_restart.cpp` -- 8+ test cases

**Restart sequence:**
1. Deactivate all plugins (with isolation)
2. Clear error counters for non-quarantined plugins
3. Re-activate healthy plugins
4. Notify user of quarantined plugins

**Acceptance Criteria:**
- Extension host restart completes without app restart
- Faulted plugin not re-activated (quarantined)
- Healthy plugins resume normal operation
- User notified of restart and quarantined plugins
- All tests pass

**Files Created/Modified:** 3 modified + 1 test = 4 files
**Estimated Complexity:** Medium (400 lines changes)

---

### TIER 7: RENDERING & UI ROBUSTNESS (Phases 27-30)

---

### Phase 27 -- Rendering Pipeline Safety

**Objective:** Add null-safety, damage rect validation, and font/theme fallbacks to the entire rendering pipeline.

**PRD Items:** PI-35 (damage rect validation), PI-36 (null-safe rendering), PI-37 (safe glyph cache)

**Prerequisites:** Phase 01 (error types)

**Deliverables:**
- `src/rendering/RenderGuards.h` -- Damage rect clamping, null-safe theme lookup
- `src/rendering/RenderGuards.cpp` -- Implementation
- Modify `src/rendering/HtmlRenderer.cpp` -- Apply render guards
- Modify `src/core/ThemeEngine.cpp` -- Add fallback for missing tokens
- `tests/unit/test_render_guards.cpp` -- 10+ test cases

**Key guards:**
- Damage rect clamped to viewport bounds before painting
- Font lookup returns fallback if requested font missing
- Theme token lookup returns default color if token undefined
- Glyph cache returns zero-width for missing glyphs (not crash)

**Acceptance Criteria:**
- Negative damage rect coordinates clamped to 0
- Missing font returns system default
- Missing theme token returns #FF00FF (debug) or #333333 (release)
- No null pointer dereference in rendering pipeline
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (350 lines production code)

---

### Phase 28 -- IME and Scroll Safety

**Objective:** Guard IME composition ranges and scroll offset calculations against invalid values.

**PRD Items:** PI-38 (IME defensive handling), PI-39 (safe scroll calculations)

**Prerequisites:** Phase 11 (numeric guards)

**Deliverables:**
- `src/ui/IMEGuard.h` -- Safe IME composition range validation
- `src/ui/IMEGuard.cpp` -- Implementation
- `src/ui/ScrollGuard.h` -- Safe scroll offset clamping
- `src/ui/ScrollGuard.cpp` -- Implementation
- Modify `src/core/IMECompositionOverlay.h` -- Apply IME guards
- `tests/unit/test_ime_scroll_safety.cpp` -- 10+ test cases

**Acceptance Criteria:**
- IME composition range outside document bounds clamped
- Scroll offset negative value clamped to 0
- Scroll past end clamped to max
- Fractional scroll offsets handled correctly
- All tests pass

**Files Created/Modified:** 4 new + 1 modified + 1 test = 6 files
**Estimated Complexity:** Medium (300 lines production code)

---

### Phase 29 -- Fallback Theme Safety

**Objective:** Ensure the application always has a valid theme, even if all theme files are corrupted or deleted.

**PRD Items:** PI-40 (fallback theme safety), PII-2 (soft-fail rendering mode)

**Prerequisites:** Phase 27 (rendering pipeline safety)

**Deliverables:**
- `src/core/FallbackTheme.h` -- Hardcoded minimal theme that compiles into the binary
- `src/core/FallbackTheme.cpp` -- Implementation
- Modify `src/core/ThemeEngine.cpp` -- Load fallback if all themes fail
- Modify `src/core/ThemeRegistry.cpp` -- Detect theme corruption, trigger fallback
- `tests/unit/test_fallback_theme.cpp` -- 8+ test cases

**Acceptance Criteria:**
- Deleting all theme files still produces a usable UI
- Corrupted theme YAML triggers fallback + notification
- Fallback theme has all required tokens (10 base colors minimum)
- User notified that fallback theme is active
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (300 lines production code)

---

### Phase 30 -- Error Reporting Service and Toast Notifications

**Objective:** Build the central error reporting service that aggregates errors from all subsystems and presents user-facing non-blocking error toasts.

**PRD Items:** PI-41 (central error reporting service), PI-42 (user-facing non-blocking error toasts), PII-3 (graceful feature degradation)

**Prerequisites:** Phase 01 (error types), Phase 03 (structured logging)

**Deliverables:**
- `src/core/ErrorReportingService.h` -- Central error sink, aggregation, deduplication
- `src/core/ErrorReportingService.cpp` -- Implementation
- `src/ui/ErrorToast.h` -- Non-blocking error toast UI component
- `src/ui/ErrorToast.cpp` -- wxWidgets implementation
- `tests/unit/test_error_reporting.cpp` -- 10+ test cases

**Key APIs:**
```cpp
class ErrorReportingService {
    void report(Error error);
    void report_and_notify(Error error);  // Also shows toast
    auto recent_errors(int count = 50) -> std::vector<Error>;
    auto error_count_by_subsystem() -> std::unordered_map<SubsystemId, int>;
};
```

**Acceptance Criteria:**
- Duplicate errors within 5 seconds deduplicated (count incremented)
- Toast appears without blocking UI
- Toast auto-dismisses after 5 seconds (errors stay 10 seconds)
- Error history bounded to 1000 entries
- All tests pass

**Files Created/Modified:** 4 new + 1 test = 5 files
**Estimated Complexity:** Medium (450 lines production code)

---

### TIER 8: STATE & PERSISTENCE SAFETY (Phases 31-32)

---

### Phase 31 -- Atomic Config Writes and Corruption Detection

**Objective:** Make all config and state persistence atomic (temp file + fsync + rename) and add checksum-based corruption detection.

**PRD Items:** PI-46 (atomic config writes), PI-48 (corruption detection -- checksum), PI-49 (transactional save operations), PI-47 (workspace state snapshot versioning)

**Prerequisites:** Phase 01 (error types)

**Deliverables:**
- `src/core/AtomicWriter.h` -- Atomic file write (temp + fsync + rename)
- `src/core/AtomicWriter.cpp` -- Implementation
- `src/core/ChecksumValidator.h` -- CRC32/SHA256 checksum for persistence files
- `src/core/ChecksumValidator.cpp` -- Implementation
- Modify `src/core/Config.cpp` -- Use AtomicWriter for save()
- `tests/unit/test_atomic_writer.cpp` -- 10+ test cases

**Acceptance Criteria:**
- Config save never produces a half-written file (crash mid-write leaves old file intact)
- Corrupted config detected by checksum mismatch
- Checksum stored in file header or companion .checksum file
- Workspace state includes schema version number
- All tests pass

**Files Created/Modified:** 4 new + 1 modified + 1 test = 6 files
**Estimated Complexity:** Medium (400 lines production code)

---

### Phase 32 -- Write-Ahead Logging for Workspace State

**Objective:** Implement WAL for workspace state mutations so incomplete operations can be recovered after a crash.

**PRD Items:** PII-16 (write-ahead logging), PII-17 (crash-safe recovery replay), PII-18 (state snapshot versioning), PII-19 (incremental state validation), PII-20 (corruption isolation mode)

**Prerequisites:** Phase 31 (atomic writer, checksum)

**Deliverables:**
- `src/core/WriteAheadLog.h` -- WAL for workspace state
- `src/core/WriteAheadLog.cpp` -- Implementation
- `src/core/StateRecovery.h` -- Crash recovery replay engine
- `src/core/StateRecovery.cpp` -- Implementation
- `tests/unit/test_wal.cpp` -- 12+ test cases

**WAL entry format:**
```cpp
struct WALEntry {
    uint64_t sequence_number;
    SubsystemId subsystem;
    std::string operation;      // "set_config", "save_document", etc.
    std::string payload_json;   // Serialized operation data
    uint32_t checksum;          // CRC32 of payload
    bool committed{false};
};
```

**Acceptance Criteria:**
- WAL entries written before state mutation
- Uncommitted WAL entries replayed on startup
- Corrupted WAL entries skipped with log warning
- Corruption isolation mode opens workspace read-only
- WAL file bounded to 10 MB, old entries pruned
- All tests pass

**Files Created/Modified:** 4 new + 1 test = 5 files
**Estimated Complexity:** High (600 lines production code)

---

### TIER 9: OBSERVABILITY & OTLP (Phases 33-35)

---

### Phase 33 -- OpenTelemetry Abstraction Layer

**Objective:** Build the telemetry abstraction layer (ITelemetryExporter, TelemetryService) that can work in local-only mode or with OTLP. This phase does NOT add the OTLP SDK dependency -- it builds the interface and local implementation.

**PRD Items:** PII-26 (OpenTelemetry SDK integration -- abstraction layer), PII-30 (privacy-aware telemetry), PII-31 (remote telemetry fail-safe)

**Prerequisites:** Phase 03 (structured logging), Phase 01 (error types)

**Deliverables:**
- `src/core/ITelemetryExporter.h` -- Interface for telemetry export
- `src/core/TelemetryService.h` -- Telemetry service with spans, metrics, events
- `src/core/TelemetryService.cpp` -- Implementation (local-only mode)
- `src/core/TelemetryTypes.h` -- Span, Metric, TelemetryEvent structs
- `src/core/PrivacyFilter.h` -- Path redaction, content redaction
- `src/core/PrivacyFilter.cpp` -- Implementation
- `tests/unit/test_telemetry_service.cpp` -- 10+ test cases

**Privacy controls:**
- File paths redacted to relative form
- User content never included
- Extension IDs included (not personal data)
- Full opt-in required (default: local-only)

**Acceptance Criteria:**
- TelemetryService works in local-only mode (no network)
- Spans recorded with duration, thread ID, correlation ID
- Privacy filter redacts absolute paths
- Bounded telemetry buffer (5000 entries)
- All tests pass

**Files Created/Modified:** 6 new + 1 test = 7 files
**Estimated Complexity:** High (550 lines production code)

---

### Phase 34 -- Trace Spans and Metrics Collection

**Objective:** Instrument key subsystems with trace spans and metrics collection points. Wire the TelemetryService into plugin activation, file operations, rendering, and event dispatch.

**PRD Items:** PII-27 (structured trace spans), PII-28 (OTLP metrics collection), PII-29 (OTLP error events), PII-33 (correlation ID propagation)

**Prerequisites:** Phase 33 (telemetry service)

**Deliverables:**
- `src/core/SpanMacros.h` -- `MARKAMP_SPAN(name)`, `MARKAMP_SPAN_SUBSYSTEM(name, subsystem)` RAII macros
- Modify `src/core/PluginManager.cpp` -- Add activation/deactivation spans
- Modify `src/core/EventBus.cpp` -- Add dispatch span for slow events
- Modify `src/rendering/HtmlRenderer.cpp` -- Add rendering pass span
- Modify `src/core/Config.cpp` -- Add config load/save spans
- `tests/unit/test_span_macros.cpp` -- 8+ test cases

**Metrics collected:**
- Event queue depth (gauge)
- Plugin activation time (histogram)
- Rendering pass duration (histogram)
- File open duration (histogram)
- Error count per subsystem (counter)

**Acceptance Criteria:**
- Spans automatically record start/end time, thread ID, correlation ID
- RAII macro ensures span is ended even if exception thrown
- Slow event dispatch (>10ms) creates a span entry
- Metrics queryable from TelemetryService
- Zero overhead when telemetry disabled
- All tests pass

**Files Created/Modified:** 1 new + 4 modified + 1 test = 6 files
**Estimated Complexity:** Medium (400 lines production/changes)

---

### Phase 35 -- OTLP Export and Crash Event Export

**Objective:** Add the optional OTLP exporter (gRPC/HTTP) for remote telemetry, with backpressure, fail-safe queuing, and crash event export. This phase adds the opentelemetry-cpp dependency to vcpkg.json.

**PRD Items:** PII-26 (OTLP SDK), PII-31 (remote telemetry fail-safe), PII-32 (backpressure on export), PII-35 (crash event export)

**Prerequisites:** Phase 33, Phase 34 (telemetry service + spans)

**Deliverables:**
- `src/core/OTLPExporter.h` -- OTLP exporter implementing ITelemetryExporter
- `src/core/OTLPExporter.cpp` -- Implementation
- Modify `vcpkg.json` -- Add opentelemetry-cpp dependency
- Modify `src/core/CrashDump.cpp` -- Attach recent spans to crash report
- `tests/unit/test_otlp_exporter.cpp` -- 8+ test cases

**Fail-safe behavior:**
- If OTLP endpoint unreachable: queue locally (bounded 1000 entries)
- Drop oldest on overflow
- Never block UI thread
- Reduce export frequency under network pressure

**Acceptance Criteria:**
- OTLP exporter sends spans/metrics to configurable endpoint
- Network failure does not crash or stall the app
- Crash report includes last 100 spans
- Telemetry fully disabled when config.telemetry_enabled=false
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** High (500 lines production code)

---

### TIER 10: SAFE MODE & RECOVERY (Phases 36-38)

---

### Phase 36 -- Multi-Tier Safe Mode

**Objective:** Implement safe mode startup with multiple tiers (--safe, --safe-no-extensions, --safe-no-preview, --safe-minimal-ui) and automatic safe mode trigger after N crashes.

**PRD Items:** PI-45 (safe mode startup), PII-36 (multi-tier safe mode), PII-37 (automatic safe mode trigger)

**Prerequisites:** Phase 04 (crash dump), Phase 31 (atomic writer for crash counter)

**Deliverables:**
- `src/core/SafeMode.h` -- Safe mode tiers, crash counter, auto-trigger
- `src/core/SafeMode.cpp` -- Implementation
- Modify `src/app/MarkAmpApp.cpp` -- Parse --safe flags, check crash counter
- Modify `src/main.cpp` -- Pass command-line args to SafeMode
- `tests/unit/test_safe_mode.cpp` -- 10+ test cases

**Safe mode tiers:**
| Tier | Flag | Behavior |
|---|---|---|
| 1 | `--safe` | Extensions disabled, default theme |
| 2 | `--safe-no-extensions` | Extensions disabled only |
| 3 | `--safe-no-preview` | Preview panel disabled |
| 4 | `--safe-minimal-ui` | Minimal UI: editor only, no sidebar |

**Crash counter:**
- Persisted in `~/.markamp/crash_counter.json`
- Incremented on unclean shutdown
- Reset on clean exit
- 3 crashes in 5 minutes triggers auto-safe-mode

**Acceptance Criteria:**
- `--safe` flag launches with extensions disabled and default theme
- Crash counter persists across sessions
- 3 rapid crashes trigger auto-safe-mode on next launch
- Safe mode notification shown to user
- All tests pass

**Files Created/Modified:** 2 new + 2 modified + 1 test = 5 files
**Estimated Complexity:** Medium (400 lines production code)

---

### Phase 37 -- Fault Recovery Dialog

**Objective:** Build the fault recovery dialog that appears when a subsystem resets or the app recovers from a crash, offering the user diagnostic information and restart options.

**PRD Items:** PII-38 (fault recovery dialog), PII-4 (self-healing state reset)

**Prerequisites:** Phase 30 (error reporting), Phase 36 (safe mode)

**Deliverables:**
- `src/ui/FaultRecoveryDialog.h` -- wxDialog for fault recovery
- `src/ui/FaultRecoveryDialog.cpp` -- Implementation
- `src/core/SubsystemReset.h` -- Self-healing state reset per subsystem
- `src/core/SubsystemReset.cpp` -- Implementation
- `tests/unit/test_fault_recovery.cpp` -- 8+ test cases

**Dialog options:**
- "Continue" -- dismiss and keep working
- "Restart in Safe Mode" -- relaunch with --safe
- "View Logs" -- open log file
- "Reset [Subsystem]" -- reinitialize the faulted subsystem

**Acceptance Criteria:**
- Dialog appears when subsystem fault detected
- "Restart in Safe Mode" relaunches the app correctly
- Subsystem reset reinitializes without app restart
- Dialog shows recent error summary
- All tests pass

**Files Created/Modified:** 4 new + 1 test = 5 files
**Estimated Complexity:** Medium (400 lines production code)

---

### Phase 38 -- Subsystem Fault Domains

**Objective:** Explicitly define fault domain boundaries so that failure in one subsystem (rendering, extensions, markdown parsing, etc.) cannot cascade to others.

**PRD Items:** PII-1 (subsystem fault domains), PII-47 (no cross-subsystem direct coupling)

**Prerequisites:** Phase 30 (error reporting), Phase 37 (subsystem reset)

**Deliverables:**
- `src/core/FaultDomain.h` -- FaultDomain class with health tracking per subsystem
- `src/core/FaultDomain.cpp` -- Implementation
- `src/core/FaultDomainRegistry.h` -- Registry of all fault domains
- `src/core/FaultDomainRegistry.cpp` -- Implementation
- `tests/unit/test_fault_domains.cpp` -- 10+ test cases

**Fault domains:**
1. Rendering -- HtmlRenderer, CodeBlockRenderer, ThemeEngine
2. ExtensionHost -- PluginManager, all extensions
3. MarkdownEngine -- MarkdownParser, Md4cWrapper
4. YAMLParsing -- Config, ExtensionManifest
5. Workspace -- VaultService, DocumentFileSystem
6. Network -- HttpClient, GalleryService

**Acceptance Criteria:**
- Each domain tracks error count, last error, health status
- Domain fault triggers graceful degradation (not crash)
- Unhealthy domain can be reset independently
- Cross-domain communication only via EventBus (enforced by audit)
- All tests pass

**Files Created/Modified:** 4 new + 1 test = 5 files
**Estimated Complexity:** Medium (400 lines production code)

---

### TIER 11: ENTERPRISE & ADVANCED (Phase 39)

---

### Phase 39 -- Enterprise Runtime Protections

**Objective:** Add runtime policy enforcement, extension network interception, immutable execution mode, and per-extension resource attribution.

**PRD Items:** PII-39 (runtime policy engine), PII-40 (extension network interceptor), PII-41 (immutable execution mode), PII-42 (extension resource attribution)

**Prerequisites:** Phase 22 (plugin isolation), Phase 33 (telemetry)

**Deliverables:**
- `src/core/RuntimePolicy.h` -- Policy enforcement engine
- `src/core/RuntimePolicy.cpp` -- Implementation
- `src/core/NetworkInterceptor.h` -- Extension network call interception
- `src/core/NetworkInterceptor.cpp` -- Implementation
- `src/core/ExtensionResourceTracker.h` -- Per-extension resource tracking
- `src/core/ExtensionResourceTracker.cpp` -- Implementation
- `tests/unit/test_enterprise_runtime.cpp` -- 10+ test cases

**Policy enforcement:**
- `--immutable` flag: prevent config changes, disable extension install
- Network interceptor: block outbound calls unless extension has kNetwork permission
- Resource tracker: per-extension memory estimate, event count, error count

**Acceptance Criteria:**
- Immutable mode prevents Config::set() and extension install
- Extension without kNetwork permission cannot make HTTP calls
- Resource attribution exposed via diagnostics (per-extension stats)
- Policy violations logged and surfaced via ErrorReportingService
- All tests pass

**Files Created/Modified:** 6 new + 1 test = 7 files
**Estimated Complexity:** High (600 lines production code)

---

### TIER 12: TESTING & VALIDATION (Phase 40)

---

### Phase 40 -- Chaos Testing Framework and Final Validation

**Objective:** Build the continuous fault injection framework, chaos plugin harness, and synthetic event flood testing. Run the final validation suite against all DoD criteria.

**PRD Items:** PII-43 (continuous fault injection), PII-44 (chaos plugin harness), PII-45 (synthetic event flood testing), PI-X Cultural Rules (all ten mandates), PII-46 (all public API calls guarded)

**Prerequisites:** All previous phases

**Deliverables:**
- `src/core/ChaosEngine.h` -- Fault injection engine (guarded by MARKAMP_ENABLE_CHAOS)
- `src/core/ChaosEngine.cpp` -- Implementation
- `src/core/ChaosPlugin.h` -- Misbehaving plugin for testing
- `src/core/ChaosPlugin.cpp` -- Implementation (infinite loop, OOM, throwing handlers)
- `tests/unit/test_chaos_engine.cpp` -- 15+ test cases covering all DoD criteria
- Modify `CMakeLists.txt` (root) -- Add MARKAMP_ENABLE_CHAOS option

**Fault injection types:**
1. Random std::runtime_error in plugin handlers
2. Simulated OOM (bad_alloc) at subsystem boundaries
3. Simulated thread interruption
4. File permission errors on config save
5. Infinite loop plugin (tests watchdog)
6. 10,000 events/second flood (tests backpressure)

**DoD Validation Test Cases:**
```
1. Plugin crash does not kill host
2. Malformed YAML file opens without crash
3. Invalid user input rejected safely
4. Background task cancelled safely
5. Thread failure isolated
6. Unexpected exception logged and contained
7. Corrupted config detected and recovered
8. Rendering fault falls back safely
9. OOM results in controlled degradation
10. Application never terminates unexpectedly
```

**Acceptance Criteria:**
- All 10 DoD criteria pass as automated tests
- Chaos plugin does not crash the host
- Event flood handled by backpressure (no OOM)
- Simulated OOM triggers controlled degradation
- All chaos tests can be run in CI with `cmake -DMARKAMP_ENABLE_CHAOS=ON`
- All tests pass

**Files Created/Modified:** 4 new + 1 modified + 1 test = 6 files
**Estimated Complexity:** High (600 lines production code)

---

## PRD Item to Phase Mapping

### Phase I Items (50)

| PRD Item | Phase | Description |
|---|---|---|
| PI-1 | 04 | Global exception barrier in main() |
| PI-2 | 04 | Thread entry guard pattern |
| PI-3 | 05 | EventBus subscriber isolation |
| PI-4 | 05 | UI message loop guard |
| PI-5 | 22 | Plugin activation isolation |
| PI-6 | 22 | Plugin deactivation isolation |
| PI-7 | 02 | Centralized input validation utilities |
| PI-8 | 02, 14 | All external data untrusted |
| PI-9 | 07 | Length guards on all strings |
| PI-10 | 08 | Filesystem canonicalization wrapper |
| PI-11 | 09 | Bounded JSON/YAML parsing |
| PI-12 | 10 | Safe regex execution |
| PI-13 | 11 | Numeric range enforcement |
| PI-14 | 02, 12 | Enum validation |
| PI-15 | 12 | Strict schema validation for ExtensionManifest |
| PI-16 | 05, 15 | All containers bounded |
| PI-17 | 16 | Safe allocation wrapper |
| PI-18 | 16 | No raw owning pointers |
| PI-19 | 18 | No cross-thread shared raw references |
| PI-20 | 11 | Integer overflow guards |
| PI-21 | 11 | Underflow prevention |
| PI-22 | 16 | TextSpan validation |
| PI-23 | 06 | Strict thread ownership rules |
| PI-24 | 06 | UI thread assertion macro |
| PI-25 | 20 | Deadlock detection debug mode |
| PI-26 | 19 | Queue backpressure mechanism |
| PI-27 | 17 | Async task cancellation tokens |
| PI-28 | 18 | Generation-based invalidation |
| PI-29 | 22 | Per-plugin error counter |
| PI-30 | 22 | Per-plugin execution time monitoring |
| PI-31 | 23 | Sandbox filesystem restrictions |
| PI-32 | 24 | Extension API contract versioning |
| PI-33 | 23, 24 | Fail-closed policy |
| PI-34 | 26 | Graceful extension host restart |
| PI-35 | 27 | Damage rect validation |
| PI-36 | 27 | Null-safe rendering pipeline |
| PI-37 | 27 | Safe glyph cache access |
| PI-38 | 28 | IME defensive handling |
| PI-39 | 28 | Safe scroll calculations |
| PI-40 | 29 | Fallback theme safety |
| PI-41 | 30 | Central error reporting service |
| PI-42 | 30 | User-facing error toasts |
| PI-43 | 03 | Structured log format |
| PI-44 | 04 | Crash dump generation |
| PI-45 | 36 | Safe mode startup |
| PI-46 | 31 | Atomic config writes |
| PI-47 | 31 | Workspace state snapshot versioning |
| PI-48 | 31 | Corruption detection (checksum) |
| PI-49 | 31 | Transactional save operations |
| PI-50 | 03 | Correlation IDs for subsystems |

### Phase II Items (50)

| PRD Item | Phase | Description |
|---|---|---|
| PII-1 | 38 | Subsystem fault domains |
| PII-2 | 29 | Soft-fail rendering mode |
| PII-3 | 30 | Graceful feature degradation |
| PII-4 | 37 | Self-healing state reset |
| PII-5 | 25 | Plugin fault quarantine |
| PII-6 | 15 | Memory pressure monitor (via ResourceLimits) |
| PII-7 | 15 | Fail-safe memory reserve |
| PII-8 | 19 | Dynamic backpressure scaling |
| PII-9 | 15 | Fragmentation monitoring |
| PII-10 | 15 | Hard caps with graceful degradation |
| PII-11 | 20 | Thread health registry |
| PII-12 | 20 | Watchdog timer for UI thread |
| PII-13 | 20 | Deadlock suspicion detector |
| PII-14 | 17 | Safe task cancellation enforcement |
| PII-15 | 21 | Thread panic escalation policy |
| PII-16 | 32 | Write-ahead logging for workspace state |
| PII-17 | 32 | Crash-safe recovery replay |
| PII-18 | 32 | State snapshot versioning |
| PII-19 | 32 | Incremental state validation |
| PII-20 | 32 | Corruption isolation mode |
| PII-21 | 13 | Strict MIME validation |
| PII-22 | 13 | Markdown parsing hard limits |
| PII-23 | 13 | Sandboxed HTML rendering |
| PII-24 | 13 | Time-limited parsing |
| PII-25 | 19 | Rate-limited event flood protection |
| PII-26 | 33, 35 | OpenTelemetry SDK integration |
| PII-27 | 34 | Structured trace spans |
| PII-28 | 34 | OTLP metrics collection |
| PII-29 | 34 | OTLP error events |
| PII-30 | 33 | Privacy-aware telemetry layer |
| PII-31 | 35 | Remote telemetry fail-safe |
| PII-32 | 35 | Backpressure on telemetry export |
| PII-33 | 03, 34 | Correlation ID propagation |
| PII-34 | 03 | Structured JSON log mirror |
| PII-35 | 35 | Crash event export |
| PII-36 | 36 | Multi-tier safe mode |
| PII-37 | 36 | Automatic safe mode trigger |
| PII-38 | 37 | Fault recovery dialog |
| PII-39 | 39 | Runtime policy enforcement engine |
| PII-40 | 39 | Extension network interceptor |
| PII-41 | 39 | Immutable execution mode |
| PII-42 | 39 | Extension resource attribution |
| PII-43 | 40 | Continuous fault injection framework |
| PII-44 | 40 | Chaos plugin harness |
| PII-45 | 40 | Synthetic event flood testing |
| PII-46 | 40 | All public API calls guarded |
| PII-47 | 38 | No cross-subsystem direct coupling |
| PII-48 | 01 | Explicit failure return types (Result<T>) |
| PII-49 | 01, 03 | No silent recovery |
| PII-50 | 04 | Every crash produces actionable data |

### Cultural Rules (PI Section X)

| Rule | Enforced By Phase(s) |
|---|---|
| No unchecked casts | Code review convention (CLAUDE.md) |
| No silent catch(...) | Phase 01 (MARKAMP_GUARD), CLAUDE.md already mandates |
| No unbounded containers | Phase 05, 15 |
| No implicit narrowing conversions | Compiler warnings (-Wconversion), Phase 11 |
| No blocking I/O on UI thread | Phase 06 (MARKAMP_ASSERT_UI_THREAD) |
| No external call without guard | Phase 04 (crash barrier), Phase 22 (plugin isolation) |
| No shared mutable global state | Phase 06, 18 (thread ownership, generation counters) |
| All async code cancelable | Phase 17 (CancellationToken) |
| All failure paths logged | Phase 03 (structured logging) |
| All logs actionable | Phase 03 (structured log format with correlation IDs) |

---

## Summary Statistics

| Metric | Value |
|---|---|
| Total phases | 40 |
| Total new files (production) | ~90 |
| Total modified files | ~35 |
| Total test files | 40 |
| Estimated total production code | ~15,000 lines |
| Estimated total test code | ~6,000 lines |
| New vcpkg dependencies | 1 (opentelemetry-cpp, Phase 35) |
| Phases with platform-specific code | 2 (Phase 04, Phase 20) |
| Highest-risk phases | 04 (signal handlers), 32 (WAL), 35 (OTLP SDK) |

---

## Execution Notes for AI Coding Agents

1. **Always read Phase 01 first** -- it defines Result<T>, Error, SubsystemId, and Guards that every other phase depends on.

2. **Each phase is self-contained** -- after completing a phase, all its tests must pass before moving to the next.

3. **Namespace convention** -- All new code in `markamp::core` unless it is UI (then `markamp::ui`) or rendering (then `markamp::rendering`).

4. **Constructor injection** -- All new services accept their dependencies via constructor. No singletons, no ServiceRegistry.

5. **Event types** -- New events go in `src/core/Events.h` using `MARKAMP_DECLARE_EVENT` macros.

6. **CMakeLists.txt** -- Each phase must update both `src/CMakeLists.txt` (add_executable) and `tests/CMakeLists.txt` (test target). New files also need `source_group()` entries.

7. **Backward compatibility** -- Existing APIs must not break. New guards, validations, and error paths are additive. Existing callers continue to work unchanged.

8. **Code style** -- C++23, trailing return types, 4-space indent, Allman braces, `[[nodiscard]]` on queries, `MARKAMP_LOG_*` macros for logging.
