# Phase 24: Plugin Isolation Hardening -- Timeout & Memory Caps

## Metadata

| Field | Value |
|---|---|
| Phase ID | 24 |
| Prerequisites | Phase 17 |
| Estimated Complexity | High |
| Estimated File Count | 4 created, 5 modified |
| PRD Sections | 5.2 Harden Plugin Isolation |

---

## Objective

Implement plugin call timeout detection and per-plugin soft memory caps. Ensure no plugin can crash the host application.

---

## Background

The PRD mandates: "All plugin calls wrapped, Timeout detection for long-running tasks, Memory caps per plugin (soft enforcement)." A misbehaving plugin (infinite loop, excessive allocation, unhandled exception) must be contained without affecting the host or other plugins.

---

## Scope

### Tasks

1. **Create `src/core/PluginSafeCall.h`**:
   - `safe_call<R>(plugin_id, callable, timeout_ms) -> std::expected<R, PluginError>`:
     - Wraps the callable in a typed exception boundary (uses ThreadBoundary from Phase 22)
     - Starts a monitoring timer
     - If callable exceeds timeout:
       - Log warning with plugin ID and elapsed time
       - Return `PluginError::Timeout`
       - Do NOT kill the thread (unsafe) — mark plugin as "slow" and skip future events until it returns
     - If callable throws:
       - Catch typed exception, log with plugin ID
       - Return `PluginError::Exception` with message
     - If callable completes normally:
       - Return result
   - Timeout detection via `std::async` + `std::future::wait_for()` or deadline checking

2. **Create `src/core/PluginMemoryTracker.h` / `PluginMemoryTracker.cpp`**:
   - Per-plugin allocation tracking:
     - Approximate tracking using tagged arena or allocation hooks
     - Track: current_bytes, peak_bytes, allocation_count
   - Configurable soft cap per plugin (default: 50MB, from Config)
   - Warning threshold: 80% of soft cap
   - Hard threshold: 2x soft cap (200% = 100MB default)
   - On warning: publish `PluginMemoryWarningEvent` with plugin ID and usage
   - On hard exceed: forcefully deactivate plugin, publish `PluginDeactivatedEvent`
   - Periodic check (1Hz) for memory tracking

3. **Integrate into PluginManager**:
   - All event handler dispatches to plugins use `safe_call()`
   - Plugin activation calls use `safe_call()` with activation timeout (30s default)
   - Plugin deactivation calls use `safe_call()` with short timeout (5s default)
   - Track per-plugin call latency for diagnostics

4. **Add events to `Events.h`**:
   - `PluginTimeoutEvent`: { plugin_id, elapsed_ms, operation }
   - `PluginMemoryWarningEvent`: { plugin_id, current_bytes, cap_bytes }
   - `PluginDeactivatedEvent`: { plugin_id, reason }

5. **Create `tests/unit/test_plugin_isolation.cpp`**:
   - Test timeout detection: plugin call exceeding timeout is caught
   - Test memory cap warning: plugin approaching cap triggers warning
   - Test memory cap hard limit: plugin exceeding 2x cap is deactivated
   - Test exception isolation: plugin exception doesn't crash host
   - Test normal operation: well-behaved plugin is unaffected

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/PluginSafeCall.h` |
| Create | `src/core/PluginMemoryTracker.h` |
| Create | `src/core/PluginMemoryTracker.cpp` |
| Modify | `src/core/PluginManager.h` |
| Modify | `src/core/PluginManager.cpp` |
| Modify | `src/core/Events.h` |
| Create | `tests/unit/test_plugin_isolation.cpp` |
| Modify | `src/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- Timeout detection is tricky in C++. `std::async` + `wait_for()` is the safest approach but has thread pool overhead. Alternative: check elapsed time periodically within the call (cooperative timeout).
- Do NOT use `std::thread::cancel()` or `pthread_cancel()` — these are unsafe and can leave the process in an inconsistent state. Instead, use cooperative timeouts and mark the plugin as "unresponsive."
- Memory tracking per plugin is approximate. If plugins allocate through the global allocator (which mimalloc replaces), exact per-plugin tracking requires a custom allocator per plugin. For v6, use approximate tracking based on allocation hooks or arena tagging.
- The 50MB default cap is generous for most plugins. It can be adjusted per-plugin via Config.
- Deactivation on hard memory exceed is a last resort. The plugin's event handlers are unsubscribed and it no longer receives events.

---

## Acceptance Criteria

- [ ] Plugin call exceeding timeout (5s default) is detected and logged
- [ ] Plugin exceeding memory soft cap (80%) generates warning event
- [ ] Plugin exceeding memory hard cap (2x) is forcefully deactivated
- [ ] `test_plugin_isolation` validates timeout, memory, and exception isolation
- [ ] Existing plugins are not affected (caps are generous defaults)
- [ ] Host application remains stable when a plugin misbehaves
- [ ] `PluginDeactivatedEvent` includes reason for deactivation
- [ ] Deactivated plugin's event subscriptions are cleaned up

---

## Testing Strategy

- Run test_plugin_isolation with simulated slow/leaky/crashing plugins
- Run full test suite to verify no impact on well-behaved plugins
- Run under TSan to verify thread safety of timeout detection
- Manual test: create a test plugin that allocates excessive memory, verify deactivation
