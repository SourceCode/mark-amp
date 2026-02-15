# Phase 35: OTLP Export and Crash Event Export

## Metadata

| Field | Value |
|---|---|
| Phase ID | 35 |
| Prerequisites | Phase 33 (telemetry service), Phase 34 (spans/metrics) |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PII-26 (OTLP SDK), PII-31 (remote telemetry fail-safe), PII-32 (backpressure on export), PII-35 (crash event export) |

---

## Objective

Add the optional OTLP exporter (gRPC/HTTP) for remote telemetry, with backpressure handling, fail-safe local queuing, and crash event export that attaches recent spans to crash reports. This phase adds the `opentelemetry-cpp` dependency to `vcpkg.json`.

---

## Background

The PRD mandates OTLP SDK integration (PII-26), remote telemetry fail-safe (PII-31), backpressure on telemetry export (PII-32), and crash event export (PII-35). Phase 33 built the abstraction layer and Phase 34 added instrumentation. This phase implements the `ITelemetryExporter` interface with OpenTelemetry Protocol (OTLP) export, enabling remote observability when configured.

---

## Scope

### Tasks

1. **Create `src/core/OTLPExporter.h` / `OTLPExporter.cpp`**:
   ```cpp
   namespace markamp::core {

   struct OTLPConfig {
       std::string endpoint{"http://localhost:4317"};  // Default OTLP gRPC endpoint
       std::chrono::seconds export_interval{30};
       size_t max_queue_size{1000};
       std::chrono::seconds connect_timeout{10};
       bool use_grpc{true};     // false = HTTP/protobuf
   };

   class OTLPExporter : public ITelemetryExporter {
   public:
       explicit OTLPExporter(OTLPConfig config = {});
       ~OTLPExporter() override;

       auto export_spans(std::span<const Span> spans) -> Result<void> override;
       auto export_metrics(std::span<const Metric> metrics) -> Result<void> override;
       auto export_events(std::span<const TelemetryEvent> events) -> Result<void> override;
       void flush() override;

       // Connection status
       [[nodiscard]] auto is_connected() const -> bool;
       [[nodiscard]] auto pending_count() const -> size_t;

   private:
       OTLPConfig config_;
       BoundedDeque<Span> pending_spans_;
       BoundedDeque<Metric> pending_metrics_;
       BoundedDeque<TelemetryEvent> pending_events_;
       std::atomic<bool> connected_{false};
       std::thread export_thread_;
       std::atomic<bool> running_{false};

       void export_loop();
       auto try_export() -> Result<void>;
       void apply_backpressure();
   };

   } // namespace markamp::core
   ```
   - **Fail-safe**: If OTLP endpoint is unreachable, queue locally (bounded to 1000 entries per type)
   - **Drop oldest on overflow**: When local queue full, drop oldest entries, increment dropped counter
   - **Never block UI thread**: Export happens on a dedicated background thread
   - **Backpressure**: If network slow, reduce export frequency (double interval), batch more spans
   - **Export thread**: Background thread wakes every `export_interval`, batches and sends pending items
   - **Graceful shutdown**: On app exit, attempt to flush remaining items with a 5-second timeout

2. **Modify `vcpkg.json`**:
   - Add `opentelemetry-cpp` dependency:
     ```json
     {
       "name": "opentelemetry-cpp",
       "features": ["otlp-grpc", "otlp-http"]
     }
     ```
   - Conditional: only built when `MARKAMP_ENABLE_OTLP=ON` (CMake option, default OFF)

3. **Modify `src/core/CrashDump.cpp`**:
   - On crash, attach the last 100 spans from TelemetryService:
     ```cpp
     auto crash_spans = telemetry_service.recent_spans(100);
     report.recent_spans = serialize_spans(crash_spans);
     ```
   - If OTLP is enabled and connected, attempt to export crash report as a final event
   - Crash export has a 2-second timeout — do not delay crash reporting

4. **Create `tests/unit/test_otlp_exporter.cpp`**:
   - TEST_CASE: "OTLPExporter queues spans when endpoint unreachable"
   - TEST_CASE: "OTLPExporter drops oldest on queue overflow"
   - TEST_CASE: "OTLPExporter never blocks caller thread"
   - TEST_CASE: "OTLPExporter flush with timeout"
   - TEST_CASE: "Backpressure increases export interval"
   - TEST_CASE: "Crash report includes recent spans"
   - TEST_CASE: "Telemetry disabled when config says so"
   - TEST_CASE: "Export thread stops cleanly on shutdown"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/OTLPExporter.h` |
| Create | `src/core/OTLPExporter.cpp` |
| Modify | `vcpkg.json` |
| Modify | `src/core/CrashDump.cpp` |
| Create | `tests/unit/test_otlp_exporter.cpp` |

---

## Implementation Notes

- **Conditional compilation**: Wrap all OTLP SDK usage in `#ifdef MARKAMP_ENABLE_OTLP`. When disabled, `OTLPExporter` is a stub that stores locally.
- **OpenTelemetry C++ SDK**: Use `opentelemetry::sdk::trace::TracerProvider` and `opentelemetry::exporter::otlp::OtlpGrpcExporter`. The SDK is header-heavy but well-supported.
- **gRPC vs HTTP**: gRPC is the default. HTTP/protobuf is an alternative for environments where gRPC is not available. Configure via `OTLPConfig::use_grpc`.
- **Backpressure strategy**: Start with 30-second export interval. If export fails, double to 60s, then 120s, capping at 300s. On success, reset to 30s.
- **Crash event export**: This is a best-effort operation. If it fails (network down), the crash is still logged locally. Use a short timeout (2s) to avoid delaying crash reporting.
- **Privacy**: All exported data passes through PrivacyFilter (Phase 33) before export. No user content or absolute paths.
- **Testing**: Unit tests use a mock OTLP endpoint (or test queue behavior without actual network). Integration testing with a real OTLP collector (Jaeger, Grafana) is manual.
- **CMake integration**: Add `option(MARKAMP_ENABLE_OTLP "Enable OpenTelemetry OTLP export" OFF)` to root `CMakeLists.txt`.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] OTLP exporter sends spans/metrics to configurable endpoint
- [ ] Network failure does not crash or stall the application
- [ ] Local queue bounded to 1000 entries per type
- [ ] Queue overflow drops oldest entries
- [ ] Export thread never blocks the UI thread
- [ ] Backpressure reduces export frequency under network pressure
- [ ] Crash report includes last 100 spans
- [ ] Telemetry fully disabled when `config.telemetry_enabled=false`
- [ ] `MARKAMP_ENABLE_OTLP=OFF` builds without opentelemetry-cpp dependency
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test queue behavior without network (verify local queueing works)
- Test queue overflow (push >1000 spans, verify oldest dropped)
- Test backpressure (simulate failed exports, verify interval increases)
- Test crash span attachment (generate crash report, verify spans present)
- Test disabled mode (verify zero network calls)
- Test export thread shutdown (verify clean exit)
- Manual integration test: run with local Jaeger instance, verify spans visible
