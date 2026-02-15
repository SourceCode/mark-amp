# Phase 29: Structured Logging Framework

## Metadata

| Field | Value |
|---|---|
| Phase ID | 29 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 3 modified |
| PRD Sections | 4.1 Structured Logging Framework (Enterprise Observability) |

---

## Objective

Replace ad-hoc spdlog text logging with structured JSON logging, supporting file and console sinks with configurable levels. Add FATAL log level.

---

## Background

The PRD section 4.1 (Enterprise Observability) requires: "Structured JSON logging" with levels TRACE, DEBUG, INFO, WARN, ERROR, FATAL, supporting file logging, console logging, and optional remote log streaming. The current `Logger.h` wraps spdlog with text formatting. Structured JSON logging enables machine-parseable log analysis and integration with log aggregation tools.

---

## Scope

### Tasks

1. **Refactor `src/core/Logger.h` and `Logger.cpp`**:
   - Add `MARKAMP_LOG_FATAL` level for unrecoverable errors
   - Add `MARKAMP_LOG_TRACE` level for fine-grained tracing
   - Support two output formats:
     - Console: human-readable text (existing format, default)
     - File: structured JSON (one JSON object per line, newline-delimited)
   - Both formats include: timestamp (ISO 8601), thread_id, level, component, message
   - Configurable via Config: `log.console_format` (text/json), `log.file_format` (text/json), `log.file_path`, `log.level`
   - Maintain backward compatibility with all existing `MARKAMP_LOG_*` macros

2. **Create `src/core/StructuredLogger.h`**:
   - `LogEntry` struct with typed fields:
     - `timestamp`: ISO 8601 string
     - `thread_id`: uint64_t
     - `level`: string ("TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL")
     - `component`: string (e.g., "EventBus", "PluginManager", "Renderer")
     - `message`: string
     - `extra`: optional key-value pairs (`std::unordered_map<std::string, std::string>`)
   - `log_structured(component, level, message, key_value_pairs...)` function
   - JSON serialization for LogEntry (hand-rolled or using existing JSON dependency)
   - Compile-time format string validation where possible
   - Thread-safe (spdlog handles this, ensure JSON formatting is also safe)

3. **Update Logger initialization**:
   - Read log configuration from Config at startup
   - Support runtime log level changes via ConfigChangedEvent
   - Default: console=text at INFO, file=json at DEBUG

4. **Create `tests/unit/test_structured_logging.cpp`**:
   - Test JSON output is valid JSON (parseable)
   - Test each log level is correctly formatted
   - Test component field is included
   - Test extra key-value pairs are serialized
   - Test FATAL level is recorded and formatted
   - Test runtime log level change takes effect
   - Test file output produces valid NDJSON (newline-delimited JSON)

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/Logger.h` |
| Modify | `src/core/Logger.cpp` |
| Create | `src/core/StructuredLogger.h` |
| Create | `tests/unit/test_structured_logging.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- spdlog supports custom formatters. Create a JSON formatter that produces one JSON object per log line.
- JSON format example:
  ```json
  {"ts":"2026-02-15T10:30:00.123Z","tid":12345,"level":"INFO","component":"EventBus","msg":"Published event","event_type":"ConfigChanged"}
  ```
- NDJSON (newline-delimited JSON) is preferred over JSON arrays because it supports append-only writing and streaming parsing.
- The `component` field should be set per logging call site. Use the `MARKAMP_LOG_*` macros to capture this:
  ```cpp
  #define MARKAMP_LOG_INFO(msg, ...) logger.log(Level::INFO, __FILE__, msg, ##__VA_ARGS__)
  ```
  Or use a logger instance per component.
- Keep text format as default for console to maintain readability during development.
- For file logging, use spdlog's rotating file sink to prevent unbounded log file growth.

---

## Acceptance Criteria

- [ ] JSON log output is valid JSON (parseable by `jq` or similar tool)
- [ ] Each log entry includes: timestamp, thread_id, level, component, message
- [ ] `MARKAMP_LOG_FATAL` level works and is recorded
- [ ] `MARKAMP_LOG_TRACE` level works and is filterable
- [ ] Existing `MARKAMP_LOG_*` macros still work (backward compatible)
- [ ] Log level is configurable at runtime via Config
- [ ] `test_structured_logging` validates format, filtering, and all levels
- [ ] Console output remains human-readable (text format default)
- [ ] File output uses JSON format by default

---

## Testing Strategy

- Run test_structured_logging for format and level validation
- Parse file log output with `jq` to verify valid JSON
- Test runtime level change: set to ERROR, verify DEBUG messages are suppressed
- Run full test suite to verify no logging regressions
