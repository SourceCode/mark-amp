# Phase 34: Trace Spans and Metrics Collection

## Metadata

| Field | Value |
|---|---|
| Phase ID | 34 |
| Prerequisites | Phase 33 (telemetry service) |
| Estimated Complexity | Medium |
| Estimated File Count | 1 created, 4 modified, 1 test |
| PRD Sections | PII-27 (structured trace spans), PII-28 (OTLP metrics collection), PII-29 (OTLP error events), PII-33 (correlation ID propagation) |

---

## Objective

Instrument key subsystems with trace spans and metrics collection points. Provide RAII span macros (`MARKAMP_SPAN`) that automatically record duration, thread ID, and correlation ID. Wire TelemetryService into plugin activation, file operations, rendering, and event dispatch.

---

## Background

The PRD mandates structured trace spans (PII-27), metrics collection (PII-28), error events (PII-29), and correlation ID propagation (PII-33). Phase 33 built the telemetry infrastructure. This phase adds the instrumentation points throughout the codebase — the "sensors" that generate observability data.

---

## Scope

### Tasks

1. **Create `src/core/SpanMacros.h`**:
   ```cpp
   namespace markamp::core {

   // RAII span helper
   class ScopedSpan {
   public:
       ScopedSpan(TelemetryService& telemetry,
                  std::string_view name,
                  SubsystemId subsystem);
       ~ScopedSpan();

       void set_error(std::string_view message);
       void add_attribute(std::string_view key, std::string_view value);

   private:
       TelemetryService& telemetry_;
       uint64_t span_id_;
       std::string error_message_;
   };

   } // namespace markamp::core

   // Convenience macros
   #define MARKAMP_SPAN(telemetry, name) \
       ::markamp::core::ScopedSpan _span_##__LINE__(telemetry, name, ::markamp::core::SubsystemId::Unknown)

   #define MARKAMP_SPAN_S(telemetry, name, subsystem) \
       ::markamp::core::ScopedSpan _span_##__LINE__(telemetry, name, subsystem)
   ```
   - **RAII**: Span automatically ends when `ScopedSpan` is destroyed
   - **Exception safe**: If an exception is thrown, the span records the error and ends normally
   - **Zero overhead when disabled**: If `TelemetryMode::Disabled`, `start_span` returns immediately

2. **Modify `src/core/PluginManager.cpp`**:
   - Add spans for plugin activation and deactivation:
     ```cpp
     void PluginManager::activate(const ExtensionManifest& manifest) {
         MARKAMP_SPAN_S(telemetry_, "plugin.activate", SubsystemId::ExtensionHost);
         _span_.add_attribute("extension_id", manifest.id());
         // ... existing activation code ...
     }
     ```
   - Record metric: `plugin_activation_time_ms` (histogram)

3. **Modify `src/core/EventBus.cpp`**:
   - Add spans for slow event dispatch (>10ms):
     ```cpp
     // In publish():
     auto start = steady_clock::now();
     handler(event);
     auto elapsed = steady_clock::now() - start;
     if (elapsed > 10ms) {
         telemetry_.record_event("slow_event_dispatch", SubsystemId::Editor,
             Severity::Warning, fmt::format("Handler took {}ms", duration_cast<ms>(elapsed).count()));
     }
     ```
   - Record metric: `event_queue_depth` (gauge, sampled every second)

4. **Modify `src/rendering/HtmlRenderer.cpp`**:
   - Add span for rendering pass:
     ```cpp
     MARKAMP_SPAN_S(telemetry_, "rendering.pass", SubsystemId::Rendering);
     ```
   - Record metric: `rendering_pass_duration_ms` (histogram)

5. **Modify `src/core/Config.cpp`**:
   - Add spans for config load and save
   - Record metric: `config_load_duration_ms` (histogram)

6. **Create `tests/unit/test_span_macros.cpp`**:
   - TEST_CASE: "ScopedSpan records start and end time"
   - TEST_CASE: "ScopedSpan records subsystem"
   - TEST_CASE: "ScopedSpan records error on set_error"
   - TEST_CASE: "ScopedSpan records error on exception in scope"
   - TEST_CASE: "MARKAMP_SPAN macro creates and ends span"
   - TEST_CASE: "Span includes correlation ID"
   - TEST_CASE: "Span includes thread ID"
   - TEST_CASE: "Span attributes recorded"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/SpanMacros.h` |
| Modify | `src/core/PluginManager.cpp` |
| Modify | `src/core/EventBus.cpp` |
| Modify | `src/rendering/HtmlRenderer.cpp` |
| Modify | `src/core/Config.cpp` |
| Create | `tests/unit/test_span_macros.cpp` |

---

## Implementation Notes

- **RAII span**: The destructor of `ScopedSpan` calls `telemetry_.end_span()`. If an exception is being unwound (`std::uncaught_exceptions()` check), set the error message automatically.
- **Zero overhead**: When telemetry is disabled, `start_span` returns a sentinel ID and `end_span` is a no-op. The RAII object still exists but does nothing.
- **Selective instrumentation**: Not every function needs a span. Focus on high-value points: plugin activation (slow, error-prone), rendering passes (performance-sensitive), event dispatch (throughput indicator), config operations (crash-prone).
- **Slow event threshold**: 10ms for event handlers. This is well above normal dispatch time (<1ms) but catches genuinely slow handlers.
- **Metric sampling**: `event_queue_depth` should be sampled periodically (e.g., every second) not on every event. Use a timer or sample every Nth event.
- **Correlation ID**: Spans automatically capture the current thread's correlation ID from Phase 03. This enables tracing a user action through multiple subsystems.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Spans automatically record start/end time, thread ID, correlation ID
- [ ] RAII macro ensures span is ended even if exception thrown
- [ ] Slow event dispatch (>10ms) creates a span entry
- [ ] Plugin activation/deactivation instrumented with spans
- [ ] Rendering pass instrumented with span
- [ ] Config load/save instrumented with spans
- [ ] Metrics queryable from TelemetryService
- [ ] Zero overhead when telemetry disabled
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test ScopedSpan lifecycle: create → set attributes → destroy → verify span recorded
- Test exception safety: throw in scope, verify span still recorded with error
- Test zero-overhead mode: disabled telemetry, verify no spans recorded
- Test metric recording accuracy
- Verify correlation ID propagation through spans
- Performance test: 10,000 span creations/destructions should complete in <100ms
