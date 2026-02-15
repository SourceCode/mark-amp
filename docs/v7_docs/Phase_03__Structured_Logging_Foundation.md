# Phase 03: Structured Logging Foundation

## Metadata

| Field | Value |
|---|---|
| Phase ID | 03 |
| Prerequisites | Phase 01 (SubsystemId, Error types) |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 2 modified, 1 test |
| PRD Sections | PI-43 (structured log format), PI-50/PII-33 (correlation IDs), PII-34 (structured JSON log mirror), PII-49 (no silent recovery) |

---

## Objective

Replace the simple spdlog macro wrappers with structured JSON logging that includes timestamp, thread ID, subsystem, severity, and correlation ID. This is the observability foundation for all subsequent phases. All existing `MARKAMP_LOG_*` macros continue to work (backward compatible) while routing through the new structured logger.

---

## Background

The PRD requires structured log format with JSON output (PI-43), correlation IDs for tracing failure chains (PI-50/PII-33), and the mandate that all recoveries must log (PII-49). Currently MarkAmp uses `MARKAMP_LOG_*` macros that wrap spdlog. This phase extends them to emit both human-readable console output and machine-parseable JSON log files.

---

## Scope

### Tasks

1. **Create `src/core/CorrelationId.h` / `CorrelationId.cpp`**:
   - UUID-based correlation ID generation
   - Thread-local storage for propagation:
     ```cpp
     namespace markamp::core {

     class CorrelationId {
     public:
         static auto generate() -> std::string;
         static auto current() -> std::string_view;
         static void set(std::string id);
         static void clear();
     };

     // RAII scope guard
     class CorrelationScope {
     public:
         explicit CorrelationScope(std::string id);
         ~CorrelationScope();
     private:
         std::string previous_id_;
     };

     } // namespace markamp::core
     ```
   - UUID generation: use `<random>` with `std::mt19937` seeded from `std::random_device`
   - Format: 8-character hex string (compact, sufficient for local correlation)

2. **Create `src/core/StructuredLogger.h` / `StructuredLogger.cpp`**:
   - StructuredLogger class that emits JSON log entries:
     ```cpp
     namespace markamp::core {

     struct LogEntry {
         std::string timestamp;      // ISO 8601
         std::string level;          // DEBUG, INFO, WARN, ERROR, FATAL
         SubsystemId subsystem;
         std::thread::id thread_id;
         std::string correlation_id;
         std::string message;
         std::string error_code;     // Optional
         std::string context_json;   // Optional additional context
     };

     class StructuredLogger {
     public:
         void log(Severity level, SubsystemId subsystem,
                  std::string_view message,
                  std::string_view error_code = "");

         void set_json_output_path(const std::filesystem::path& path);
         void set_max_file_size(size_t bytes);  // Default 10 MB
         void flush();

     private:
         void write_json_entry(const LogEntry& entry);
         void rotate_if_needed();
     };

     } // namespace markamp::core
     ```
   - JSON log entry format:
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
   - Log rotation at configurable file size (default 10 MB)
   - Keep last 3 rotated files

3. **Modify `src/core/Logger.h`**:
   - Extend `MARKAMP_LOG_*` macros to route through StructuredLogger
   - Add subsystem-aware variants:
     ```cpp
     #define MARKAMP_LOG_ERROR_S(subsystem, msg, ...) ...
     #define MARKAMP_LOG_WARN_S(subsystem, msg, ...) ...
     #define MARKAMP_LOG_INFO_S(subsystem, msg, ...) ...
     #define MARKAMP_LOG_DEBUG_S(subsystem, msg, ...) ...
     ```
   - Original macros default to `SubsystemId::Unknown`

4. **Modify `src/core/Logger.cpp`**:
   - Wire StructuredLogger initialization during app startup
   - Set JSON output path to `~/.markamp/logs/markamp.json`
   - Ensure console output remains human-readable (spdlog unchanged)

5. **Create `tests/unit/test_structured_logger.cpp`**:
   - TEST_CASE: "StructuredLogger emits JSON to file"
   - TEST_CASE: "LogEntry includes timestamp in ISO 8601"
   - TEST_CASE: "LogEntry includes thread ID"
   - TEST_CASE: "LogEntry includes subsystem name"
   - TEST_CASE: "LogEntry includes correlation ID from thread-local"
   - TEST_CASE: "CorrelationId::generate produces unique IDs"
   - TEST_CASE: "CorrelationScope restores previous ID"
   - TEST_CASE: "CorrelationScope nests correctly"
   - TEST_CASE: "Log rotation creates new file at size limit"
   - TEST_CASE: "MARKAMP_LOG_* macros still work (backward compat)"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/CorrelationId.h` |
| Create | `src/core/CorrelationId.cpp` |
| Create | `src/core/StructuredLogger.h` |
| Create | `src/core/StructuredLogger.cpp` |
| Modify | `src/core/Logger.h` |
| Modify | `src/core/Logger.cpp` |
| Create | `tests/unit/test_structured_logger.cpp` |

---

## Implementation Notes

- JSON formatting must be manual (no nlohmann::json dependency) for zero-allocation hot path. Use a simple `std::ostringstream` or `std::format` for JSON construction.
- Timestamp: use `std::chrono::system_clock::now()` with `std::format` or manual formatting to ISO 8601 with millisecond precision.
- Thread ID: use `std::this_thread::get_id()` and convert to numeric via `std::hash`.
- Log rotation: when current file exceeds max size, rename to `.1`, `.2`, `.3` and delete oldest.
- The StructuredLogger must be thread-safe. Use a mutex for file writes. Consider using a lock-free queue for high-throughput logging in the future (Phase 19 backpressure may add this).
- CorrelationId thread-local storage: use `thread_local` variable. The `CorrelationScope` RAII guard enables nested correlation contexts.
- Existing spdlog integration: keep spdlog for console output. StructuredLogger is an additional sink, not a replacement.

---

## Acceptance Criteria

- [ ] All existing `MARKAMP_LOG_*` macros continue to work (backward compatible)
- [ ] JSON log output written to file alongside human-readable console output
- [ ] Each JSON log entry includes: timestamp, level, subsystem, thread_id, correlation_id, message
- [ ] Correlation ID propagated via thread-local storage
- [ ] `CorrelationScope` restores previous ID on destruction (nesting works)
- [ ] Log rotation occurs at 10 MB file size
- [ ] Rotated files preserved (last 3)
- [ ] Subsystem-aware macros (`MARKAMP_LOG_ERROR_S`) work correctly
- [ ] All 10+ test cases pass
- [ ] No performance regression in logging hot path

---

## Testing Strategy

- Unit tests verify JSON output format by parsing log file entries
- Test correlation ID propagation across simulated async operations
- Test log rotation with artificially low file size limit (1 KB)
- Verify backward compatibility by calling original `MARKAMP_LOG_*` macros
- Test thread safety with concurrent log writes from multiple threads
