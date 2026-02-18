# Phase 33: Structured Logging and Observability

## Overview
StructuredLogger.h, OutputChannelService, and the MARKAMP_LOG_* macro system exist but structured logging is not consistently wired. Log output goes to stdout/stderr without structure. This phase builds comprehensive observability: structured logs, metrics, traces, and health monitoring.

## Prerequisites
- Phase 01 (EventBus for event observation)
- Phase 18 (Tool window system for Output panel)

## Tasks

### Task 1: Wire StructuredLogger Implementation
**Files:** `src/core/StructuredLogger.h`, `src/core/StructuredLogger.cpp`
**Description:** StructuredLogger.h exists as a header. Wire a full implementation: structured log entries with timestamp, level, source, message, and key-value metadata. JSON output format.
**Acceptance Criteria:**
- Log entry: timestamp, level, source (module), message, metadata (key-value)
- Levels: DEBUG, INFO, WARN, ERROR, FATAL
- JSON output format: `{"ts":"...", "level":"INFO", "src":"editor", "msg":"...", "data":{...}}`
- Thread-safe: logging from any thread
- File rotation: max 10MB per file, 5 files retained
- Log directory: `.markamp/logs/`

### Task 2: Wire MARKAMP_LOG Macros to StructuredLogger
**Files:** `src/core/StructuredLogger.h`, all source files using MARKAMP_LOG_*
**Description:** MARKAMP_LOG_* macros exist throughout the codebase. Wire them to output through StructuredLogger instead of raw stdout/stderr.
**Acceptance Criteria:**
- MARKAMP_LOG_DEBUG routes to StructuredLogger at DEBUG level
- MARKAMP_LOG_INFO routes to INFO level
- MARKAMP_LOG_WARN routes to WARN level
- MARKAMP_LOG_ERROR routes to ERROR level
- Source module auto-detected from __FILE__
- Zero overhead when level is filtered out (compile-time check)

### Task 3: Wire Log Level Configuration
**Files:** `src/core/StructuredLogger.h`, `src/core/Config.h`
**Description:** Log level configurable globally and per module. Default: INFO in release, DEBUG in debug builds.
**Acceptance Criteria:**
- Global level: DEBUG, INFO, WARN, ERROR
- Per-module level override: e.g., `log.editor=DEBUG, log.sync=WARN`
- Runtime level change via command: "Log: Set Level"
- Level persisted in config
- Debug builds default to DEBUG
- Release builds default to INFO

### Task 4: Wire OutputChannelService for Named Log Channels
**Files:** `src/core/OutputChannelService.cpp`, `src/ui/OutputPanel.cpp`
**Description:** OutputChannelService exists. Wire named channels: "General", "Editor", "Canvas", "Sync", "Performance", "Security", "Extensions". Each channel visible in Output panel dropdown.
**Acceptance Criteria:**
- Named channels registered on module init
- Channel selector in Output panel
- Each channel has independent scroll and content
- Channel content cleared independently
- "Show Channel" command opens Output with specific channel
- Channel content exportable

### Task 5: Wire Metrics Collection
**Files:** `src/core/StructuredLogger.h`
**Description:** Metrics tracking: counters, gauges, and histograms for key application metrics.
**Acceptance Criteria:**
- Counter: event count (file opens, saves, searches)
- Gauge: current value (memory usage, open tabs, object count)
- Histogram: distribution (frame time, search latency, load time)
- Metrics available via "Metrics: Show Dashboard" command
- Metrics exportable as JSON
- Metrics reset on session start

### Task 6: Wire Request Tracing
**Files:** `src/core/StructuredLogger.h`, `src/core/EventBus.cpp`
**Description:** Trace requests across the system: event published -> handlers invoked -> response. Each trace has a correlation ID.
**Acceptance Criteria:**
- Each event gets a trace ID (UUID)
- Handler execution logged with trace ID
- Handler duration logged
- Slow handler warning: > 100ms
- Trace timeline viewable in Output panel
- Trace ID propagated to child events

### Task 7: Wire Health Check System
**Files:** `src/core/Watchdog.h`, `src/ui/StatusBarPanel.cpp`
**Description:** Health check monitors key subsystems: EventBus (draining), services (responding), memory (within budget), disk (space available).
**Acceptance Criteria:**
- Health checks every 30 seconds
- Subsystems: EventBus, Config, ThemeEngine, PluginManager, VaultService
- Status: healthy, degraded, unhealthy
- Degraded: warning in status bar
- Unhealthy: notification with recovery suggestion
- Health status in "Health" output channel

### Task 8: Wire Error Rate Tracking
**Files:** `src/core/StructuredLogger.h`
**Description:** Track error rates per module. Alert when error rate exceeds threshold.
**Acceptance Criteria:**
- Error count per module per minute
- Alert threshold: > 10 errors/minute (configurable)
- Error rate shown in health dashboard
- Spike detection: sudden increase in errors
- Error rate resets on module restart
- Historical error rates viewable

### Task 9: Wire Performance Span Tracking
**Files:** `src/core/StructuredLogger.h`
**Description:** Track performance spans: named regions of code with start/end timing. Spans can be nested.
**Acceptance Criteria:**
- `MARKAMP_SPAN("operation_name")` RAII macro
- Span records: name, start time, duration, parent span
- Nested spans create tree structure
- Slow span alert: configurable threshold
- Span data in Performance output channel
- Span export for external analysis

### Task 10: Wire Diagnostic Report Generation
**Files:** `src/core/StructuredLogger.h`, `src/ui/MainFrame.cpp`
**Description:** "Generate Diagnostic Report" command: collects system info, recent logs, metrics, health status, and configuration into a report file.
**Acceptance Criteria:**
- Report includes: OS version, app version, build info
- Recent log entries (last 1000)
- Current metrics snapshot
- Health check results
- Active extensions and versions
- Configuration (with credentials redacted)
- Report saved as JSON in `.markamp/diagnostics/`

### Task 11: Wire Extension Telemetry
**Files:** `src/core/PluginManager.cpp`, `src/core/StructuredLogger.h`
**Description:** Track extension behavior: load time, API call count, error count, memory usage. Extensions with poor health flagged.
**Acceptance Criteria:**
- Per-extension metrics: load time, API calls/sec, error count
- Memory usage per extension tracked
- Slow extension warning: load > 2 seconds
- Error-prone extension warning: > 5 errors/minute
- Extension metrics in "Extensions" output channel
- Extension health visible in Extension browser

### Task 12: Wire User Action Logging
**Files:** `src/core/StructuredLogger.h`, `src/ui/MainFrame.cpp`
**Description:** Optional user action logging for diagnostics: command executed, file opened, surface switched, search performed. No content logged, only action types.
**Acceptance Criteria:**
- Log: command name, file path, surface transition, search type
- No file content logged (privacy)
- No search query content logged
- Action log in "Activity" output channel
- Action log disabled by default (opt-in)
- Action log exportable for bug reports

### Task 13: Wire Log Search and Filtering
**Files:** `src/ui/OutputPanel.cpp`
**Description:** Output panel supports log search and filtering: search within channel, filter by level, filter by time range, highlight matches.
**Acceptance Criteria:**
- Search bar in Output panel
- Filter by log level (dropdown)
- Filter by time range (last 5min, 1hr, all)
- Highlight matches in log text
- Line count after filter
- Export filtered logs

### Task 14: Wire Startup Diagnostic
**Files:** `src/app/MarkAmpApp.cpp`, `src/core/StructuredLogger.h`
**Description:** On startup, log comprehensive diagnostic: system info, display info, workspace info, extension count, config summary.
**Acceptance Criteria:**
- System: OS, version, architecture, CPU, RAM
- Display: resolution, DPI, monitor count
- Workspace: path, file count, index status
- Extensions: count, loaded, failed
- Config: key settings summary
- All logged at INFO level on startup

### Task 15: Wire Crash Context Capture
**Files:** `src/core/StructuredLogger.h`, `src/app/MarkAmpApp.cpp`
**Description:** Maintain a circular buffer of recent log entries. On crash, write buffer to crash report file for debugging.
**Acceptance Criteria:**
- Circular buffer: last 500 log entries
- Buffer written to crash report on unhandled exception
- Buffer includes: logs, recent user actions, metrics
- Crash report includes buffer contents
- Buffer memory: < 5MB
- Buffer thread-safe

### Task 16: Wire External Log Shipping (Optional)
**Files:** `src/core/StructuredLogger.h`, `src/core/Config.h`
**Description:** Optional log shipping to external systems: file, syslog, HTTP endpoint. For enterprise deployments.
**Acceptance Criteria:**
- File output: structured JSON logs
- Syslog: RFC 5424 format
- HTTP: POST JSON logs to configurable endpoint
- Shipping disabled by default
- Filter: only ship WARN and above
- Backpressure: buffer if endpoint slow

### Task 17: Wire Log Redaction
**Files:** `src/core/StructuredLogger.h`
**Description:** Sensitive data redacted in logs: file paths shortened, credentials masked, user content excluded.
**Acceptance Criteria:**
- File paths: show relative path only
- Credentials: always masked "***"
- Search queries: length only, not content
- File content: never logged
- Extension data: redacted unless debug mode
- Redaction configurable per field

### Task 18: Wire Observability Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register observability commands: "Log: Show Output", "Log: Set Level", "Metrics: Show Dashboard", "Health: Show Status", "Diagnostic: Generate Report".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Log:", "Metrics:", "Health:", "Diagnostic:"
- "Generate Report" creates downloadable file
- "Show Dashboard" opens metrics overview
- "Health Status" shows subsystem health

### Task 19: Wire Observability Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Observability settings: log level, log retention, metrics collection, health check interval, diagnostic report auto-generation.
**Acceptance Criteria:**
- Log level: global and per-module
- Log retention: days (default 7)
- Metrics: enable/disable
- Health check interval: 10/30/60 seconds
- Auto diagnostic on crash: enable/disable

### Task 20: Add Observability Tests
**Files:** `tests/unit/test_structured_logging.cpp`, `tests/unit/test_structured_logger.cpp`
**Description:** Test observability: structured log output, metrics, health checks, and diagnostic report.
**Acceptance Criteria:**
- Structured log produces valid JSON
- Log level filtering works correctly
- Metrics: counter increment, gauge set, histogram record
- Health check: detect healthy and unhealthy states
- Diagnostic report: all sections present
- Log redaction: sensitive data masked

## Testing Requirements
- Structured log format validity
- Log level filtering and per-module override
- Metrics collection accuracy
- Health check subsystem detection

## Phase Completion Criteria
- Structured logging with JSON output
- Named output channels for each subsystem
- Metrics: counters, gauges, histograms
- Health monitoring with alerts
- Performance span tracking
- Diagnostic report generation
- All tests pass
