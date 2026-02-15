# Phase 33: OpenTelemetry Abstraction Layer

## Metadata

| Field | Value |
|---|---|
| Phase ID | 33 |
| Prerequisites | Phase 03 (structured logging), Phase 01 (error types) |
| Estimated Complexity | High |
| Estimated File Count | 6 created, 0 modified, 1 test |
| PRD Sections | PII-26 (OpenTelemetry SDK — abstraction layer), PII-30 (privacy-aware telemetry), PII-31 (remote telemetry fail-safe) |

---

## Objective

Build the telemetry abstraction layer (`ITelemetryExporter`, `TelemetryService`) that supports spans, metrics, and events in local-only mode. This phase does NOT add the OTLP SDK dependency — it builds the interface and local implementation that Phase 35 will extend with OTLP export.

---

## Background

The PRD mandates OpenTelemetry SDK integration (PII-26), privacy-aware telemetry (PII-30), and remote telemetry fail-safe (PII-31). The telemetry system is designed in layers: this phase builds the abstraction and local implementation. Phase 34 adds instrumentation spans to key subsystems. Phase 35 adds the optional OTLP exporter. By default, telemetry is local-only with no network dependency.

---

## Scope

### Tasks

1. **Create `src/core/TelemetryTypes.h`**:
   ```cpp
   namespace markamp::core {

   struct Span {
       std::string name;
       SubsystemId subsystem;
       std::string correlation_id;
       std::thread::id thread_id;
       std::chrono::steady_clock::time_point start_time;
       std::chrono::steady_clock::time_point end_time;
       std::string error_message;  // Empty if no error
       std::unordered_map<std::string, std::string> attributes;
   };

   struct Metric {
       std::string name;
       double value;
       std::string unit;
       std::chrono::system_clock::time_point timestamp;
       std::unordered_map<std::string, std::string> labels;
   };

   struct TelemetryEvent {
       std::string name;
       SubsystemId subsystem;
       Severity severity;
       std::string message;
       std::chrono::system_clock::time_point timestamp;
       std::unordered_map<std::string, std::string> attributes;
   };

   enum class TelemetryMode {
       Disabled,   // No telemetry collected
       LocalOnly,  // Collected but not exported
       Remote      // Collected and exported via OTLP
   };

   } // namespace markamp::core
   ```

2. **Create `src/core/ITelemetryExporter.h`**:
   ```cpp
   namespace markamp::core {

   class ITelemetryExporter {
   public:
       virtual ~ITelemetryExporter() = default;

       virtual auto export_spans(std::span<const Span> spans) -> Result<void> = 0;
       virtual auto export_metrics(std::span<const Metric> metrics) -> Result<void> = 0;
       virtual auto export_events(std::span<const TelemetryEvent> events) -> Result<void> = 0;
       virtual void flush() = 0;
   };

   } // namespace markamp::core
   ```

3. **Create `src/core/PrivacyFilter.h` / `PrivacyFilter.cpp`**:
   ```cpp
   namespace markamp::core {

   class PrivacyFilter {
   public:
       // Redact absolute file paths to relative form
       [[nodiscard]] auto redact_path(std::string_view path) const -> std::string;

       // Redact user content (replace with "[redacted]")
       [[nodiscard]] auto redact_content(std::string_view content) const -> std::string;

       // Filter span attributes for privacy
       [[nodiscard]] auto filter_span(Span span) const -> Span;

       // Filter metric labels for privacy
       [[nodiscard]] auto filter_metric(Metric metric) const -> Metric;

       // Set the workspace root for relative path conversion
       void set_workspace_root(const std::filesystem::path& root);

   private:
       std::filesystem::path workspace_root_;
   };

   } // namespace markamp::core
   ```
   - Absolute paths converted to relative (from workspace root)
   - User document content never included in telemetry
   - Extension IDs included (not personal data)
   - File names optionally included (configurable)

4. **Create `src/core/TelemetryService.h` / `TelemetryService.cpp`**:
   ```cpp
   namespace markamp::core {

   class TelemetryService {
   public:
       explicit TelemetryService(TelemetryMode mode = TelemetryMode::LocalOnly);

       // Span management
       [[nodiscard]] auto start_span(std::string_view name, SubsystemId subsystem) -> uint64_t;
       void end_span(uint64_t span_id, std::string_view error_message = "");
       void add_span_attribute(uint64_t span_id, std::string_view key, std::string_view value);

       // Metric recording
       void record_gauge(std::string_view name, double value, std::string_view unit = "");
       void record_histogram(std::string_view name, double value, std::string_view unit = "");
       void record_counter(std::string_view name, double increment = 1.0);

       // Event recording
       void record_event(std::string_view name, SubsystemId subsystem,
                         Severity severity, std::string_view message);

       // Export control
       void set_exporter(std::unique_ptr<ITelemetryExporter> exporter);
       void flush();
       void set_mode(TelemetryMode mode);

       // Query (for local display / Health Panel)
       [[nodiscard]] auto recent_spans(size_t count = 100) const -> std::vector<Span>;
       [[nodiscard]] auto current_metrics() const -> std::vector<Metric>;

       // Privacy
       void set_privacy_filter(PrivacyFilter filter);

   private:
       TelemetryMode mode_;
       BoundedVector<Span> spans_;          // Max 5000
       BoundedVector<Metric> metrics_;      // Max 5000
       BoundedVector<TelemetryEvent> events_; // Max 5000
       std::unique_ptr<ITelemetryExporter> exporter_;
       PrivacyFilter privacy_filter_;
       mutable std::mutex mutex_;
       uint64_t next_span_id_{1};
   };

   } // namespace markamp::core
   ```

5. **Create `tests/unit/test_telemetry_service.cpp`**:
   - TEST_CASE: "TelemetryService starts in local-only mode"
   - TEST_CASE: "start_span and end_span record duration"
   - TEST_CASE: "Span includes correlation ID from thread-local"
   - TEST_CASE: "Span includes thread ID"
   - TEST_CASE: "record_gauge records metric"
   - TEST_CASE: "record_counter increments"
   - TEST_CASE: "record_event creates event entry"
   - TEST_CASE: "PrivacyFilter redacts absolute paths"
   - TEST_CASE: "PrivacyFilter redacts content"
   - TEST_CASE: "Telemetry buffers bounded to 5000"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/TelemetryTypes.h` |
| Create | `src/core/ITelemetryExporter.h` |
| Create | `src/core/PrivacyFilter.h` |
| Create | `src/core/PrivacyFilter.cpp` |
| Create | `src/core/TelemetryService.h` |
| Create | `src/core/TelemetryService.cpp` |
| Create | `tests/unit/test_telemetry_service.cpp` |

---

## Implementation Notes

- **No OTLP dependency yet**: This phase only builds the abstraction layer. The `ITelemetryExporter` interface is designed to be implemented by the OTLP exporter in Phase 35. For now, `TelemetryService` stores data locally.
- **Local-only mode**: In local-only mode, spans/metrics/events are stored in bounded containers for local display (Health Panel from v6). No network calls.
- **Disabled mode**: When `TelemetryMode::Disabled`, all recording methods are no-ops (early return). Zero overhead.
- **Thread safety**: All public methods are thread-safe (mutex protected). The mutex should be fine-grained — consider separate mutexes for spans, metrics, and events.
- **Privacy by default**: The PrivacyFilter is always applied, even in local-only mode. This establishes the pattern before remote export is enabled.
- **Span IDs**: Simple incrementing uint64. No need for UUID — spans are local identifiers.
- **Bounded containers**: Use `BoundedVector<T>` from Phase 15.
- **Constructor injection**: `TelemetryService` is a standalone service that can be injected into other services via constructor.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] TelemetryService works in local-only mode (no network dependency)
- [ ] Spans recorded with start/end time, thread ID, correlation ID, subsystem
- [ ] Metrics recorded with name, value, unit, timestamp
- [ ] Events recorded with name, subsystem, severity, message
- [ ] Privacy filter redacts absolute paths to relative form
- [ ] Privacy filter never includes user document content
- [ ] Telemetry buffers bounded to 5000 entries each
- [ ] Disabled mode has zero overhead (early return)
- [ ] ITelemetryExporter interface ready for OTLP implementation
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test span lifecycle: start → add attributes → end
- Test span duration accuracy (start/end time within tolerance)
- Test metric recording: gauge, histogram, counter
- Test privacy filter with absolute paths, content strings
- Test bounded buffer eviction at capacity
- Test disabled mode (verify no data recorded)
- Test thread safety with concurrent span creation
