# Phase 22: Plugin Error Counter and Auto-Disable

## Metadata

| Field | Value |
|---|---|
| Phase ID | 22 |
| Prerequisites | Phase 01 (error types), Phase 03 (structured logging), Phase 05 (EventBus) |
| Estimated Complexity | Medium |
| Estimated File Count | 0 created, 3 modified, 1 test |
| PRD Sections | PI-29 (per-plugin error counter), PI-30 (execution time monitoring), PI-5 (plugin activation isolation), PI-6 (plugin deactivation isolation) |

---

## Objective

Enhance the existing `ExtensionHostRecovery` with per-plugin error counters, execution time monitoring, and structured error reporting. Wire plugin activation and deactivation into isolation barriers so a single plugin failure cannot prevent other plugins from activating or the app from shutting down.

---

## Background

The PRD mandates per-plugin error counters (PI-29), execution time monitoring (PI-30), plugin activation isolation (PI-5), and plugin deactivation isolation (PI-6). `ExtensionHostRecovery` already exists in the codebase with basic recovery capabilities. This phase enhances it with the structured logging and error taxonomy from Phase 01/03 and adds timing/counting instrumentation.

---

## Scope

### Tasks

1. **Modify `src/core/ExtensionHostRecovery.h` / `ExtensionHostRecovery.cpp`**:
   - Add per-plugin execution time tracking:
     ```cpp
     struct PluginHealthMetrics {
         std::string extension_id;
         size_t total_errors{0};
         size_t consecutive_errors{0};
         bool faulted{false};
         std::chrono::milliseconds total_execution_time{0};
         std::chrono::milliseconds max_execution_time{0};
         std::chrono::steady_clock::time_point last_error_time;
         std::string last_error_message;
         std::string last_correlation_id;
     };
     ```
   - Enhance `execute_safely()`:
     - Record execution start/end time
     - Log slow operations (>1 second) with structured logger
     - Increment error counters on failure
     - Mark plugin as faulted after N consecutive errors (default 3)
     - Include correlation ID from Phase 03 in error logs
   - Add `get_plugin_health(extension_id) -> PluginHealthMetrics`
   - Add `reset_plugin_health(extension_id)` for manual recovery
   - Add `re_enable_plugin(extension_id)` to clear faulted state

2. **Modify `src/core/PluginManager.cpp`**:
   - Wrap each plugin's `activate()` in isolation barrier:
     ```cpp
     for (auto& plugin : plugins) {
         auto result = recovery.execute_safely(plugin.id(), [&]() {
             plugin.activate(context);
         });
         if (!result) {
             MARKAMP_LOG_WARN_S(SubsystemId::ExtensionHost,
                 "Plugin '{}' activation failed: {}", plugin.id(), result.error().message);
             // Mark as faulted, continue to next plugin
         }
     }
     ```
   - Wrap each plugin's `deactivate()` in isolation barrier:
     - Deactivation failure logged but does NOT prevent other plugins from deactivating
     - Deactivation failure does NOT prevent app shutdown
   - Skip faulted plugins during event dispatch
   - Emit `PluginFaultedEvent` when a plugin reaches faulted state

3. **Create `tests/unit/test_plugin_isolation.cpp`**:
   - TEST_CASE: "Plugin throwing during activation is marked faulted"
   - TEST_CASE: "Faulted plugin does not prevent other plugins from activating"
   - TEST_CASE: "Plugin throwing during deactivation does not prevent shutdown"
   - TEST_CASE: "Slow plugin (>1s) logged with timing"
   - TEST_CASE: "Error counter incremented per failure"
   - TEST_CASE: "Consecutive error counter resets on success"
   - TEST_CASE: "Plugin disabled after 3 consecutive failures"
   - TEST_CASE: "Disabled plugin can be re-enabled"
   - TEST_CASE: "Plugin health metrics include correlation ID"
   - TEST_CASE: "PluginFaultedEvent emitted on fault"
   - TEST_CASE: "Faulted plugin skipped during event dispatch"
   - TEST_CASE: "reset_plugin_health clears all counters"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/ExtensionHostRecovery.h` |
| Modify | `src/core/ExtensionHostRecovery.cpp` |
| Modify | `src/core/PluginManager.cpp` |
| Create | `tests/unit/test_plugin_isolation.cpp` |

---

## Implementation Notes

- **Existing ExtensionHostRecovery**: This file already exists. Read it first to understand the current API before modifying. Enhance rather than replace.
- **Execution timing**: Use `std::chrono::steady_clock` for duration measurement. Track both total and max execution time per plugin.
- **Slow operation threshold**: Default 1 second. Configurable via `Config::get_int("extension.slow_threshold_ms", 1000)`.
- **Correlation ID integration**: Use `CorrelationScope` from Phase 03 to set a correlation ID before executing plugin code. This ID appears in all structured logs within the plugin execution scope.
- **Faulted state**: A faulted plugin is not destroyed — it remains in the plugin list but is skipped during dispatch. The user can re-enable it from settings.
- **Event**: Define `PluginFaultedEvent` in `Events.h`:
  ```cpp
  MARKAMP_DECLARE_EVENT(PluginFaultedEvent, std::string extension_id; std::string reason;)
  ```
- **Backward compatibility**: Existing `execute_safely()` API must continue to work. Add new methods alongside.
- Update `tests/CMakeLists.txt` for new test.

---

## Acceptance Criteria

- [ ] Plugin throwing during activation is marked faulted, other plugins continue to activate
- [ ] Plugin throwing during deactivation does not cascade or prevent app shutdown
- [ ] Slow plugin (>1s execution) logged with timing and correlation ID
- [ ] Error counter tracks per-plugin total and consecutive errors
- [ ] Plugin disabled after 3 consecutive failures
- [ ] Disabled plugin can be re-enabled via `re_enable_plugin()`
- [ ] `PluginFaultedEvent` emitted when plugin reaches faulted state
- [ ] Plugin health metrics queryable for diagnostics
- [ ] All 12+ test cases pass

---

## Testing Strategy

- Test with mock plugins that throw on activation
- Test with mock plugins that throw on deactivation
- Test with mock plugins that are slow (sleep >1s)
- Test error counter progression: success resets consecutive count
- Test faulting threshold: 3 consecutive errors triggers fault
- Test re-enable flow: fault → re-enable → successful execution
- Verify structured log output includes extension_id and correlation_id
