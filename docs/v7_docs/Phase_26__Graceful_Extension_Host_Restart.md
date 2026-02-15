# Phase 26: Graceful Extension Host Restart

## Metadata

| Field | Value |
|---|---|
| Phase ID | 26 |
| Prerequisites | Phase 22 (plugin isolation), Phase 25 (quarantine) |
| Estimated Complexity | Medium |
| Estimated File Count | 0 created, 3 modified, 1 test |
| PRD Sections | PI-34 (graceful extension host restart) |

---

## Objective

Enable the extension host to restart after a crash without requiring a full application restart. Implement a deactivate-all → clear state → reactivate-healthy sequence that preserves the user's editing session.

---

## Background

The PRD mandates graceful extension host restart (PI-34). When the extension host crashes (e.g., a plugin causes a memory corruption or infinite loop), the user currently loses all extension functionality until they restart MarkAmp. This phase enables mid-session recovery: deactivate all plugins, clear error state, and reactivate healthy plugins while keeping the editor session intact.

---

## Scope

### Tasks

1. **Modify `src/core/ExtensionHostRecovery.h` / `ExtensionHostRecovery.cpp`**:
   - Add `restart_extension_host()` method:
     ```cpp
     auto restart_extension_host() -> Result<void>;
     ```
   - Restart sequence:
     1. Log restart initiation with correlation ID
     2. Deactivate all plugins (with individual isolation — Phase 22)
     3. Clear error counters for non-quarantined plugins
     4. Record any newly quarantined plugins (from Phase 25)
     5. Clear EventBus subscriptions from plugins
     6. Wait for pending plugin async tasks to cancel (Phase 17)
     7. Re-activate healthy (non-quarantined) plugins
     8. Emit `ExtensionHostRestartedEvent` with list of quarantined plugins
     9. Notify user of restart and any quarantined plugins
   - Add `is_restarting()` flag to prevent re-entrant restarts

2. **Modify `src/core/PluginManager.cpp`**:
   - Support mid-session restart:
     - `deactivate_all()` must handle partially-activated state
     - `activate_healthy()` skips quarantined plugins
     - Plugin subscriptions tracked and removable per-plugin
   - Expose `get_active_plugins()` and `get_quarantined_plugins()` for UI

3. **Modify `src/core/EventBus.h` (or EventBus.cpp)**:
   - Support bulk unsubscription by source:
     ```cpp
     void unsubscribe_by_source(std::string_view source_id);
     ```
   - This enables removing all subscriptions from a specific plugin during restart

4. **Create `tests/unit/test_extension_host_restart.cpp`**:
   - TEST_CASE: "Extension host restart deactivates all plugins"
   - TEST_CASE: "Extension host restart re-activates healthy plugins"
   - TEST_CASE: "Faulted plugin not re-activated after restart"
   - TEST_CASE: "Quarantined plugin not re-activated after restart"
   - TEST_CASE: "ExtensionHostRestartedEvent emitted with quarantine list"
   - TEST_CASE: "Re-entrant restart prevented"
   - TEST_CASE: "Plugin EventBus subscriptions cleared during restart"
   - TEST_CASE: "Pending async tasks cancelled during restart"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/ExtensionHostRecovery.h` |
| Modify | `src/core/ExtensionHostRecovery.cpp` |
| Modify | `src/core/PluginManager.cpp` |
| Create | `tests/unit/test_extension_host_restart.cpp` |

---

## Implementation Notes

- **Deactivation order**: Deactivate in reverse activation order (last activated = first deactivated) to respect dependency ordering.
- **Error counters**: Clear error counters for plugins that are NOT quarantined. Quarantined plugins retain their error history. This gives healthy plugins a fresh start.
- **EventBus cleanup**: Each plugin's subscriptions must be tracked. When the plugin is deactivated, all its subscriptions are removed. This requires either: (a) tracking subscription IDs per plugin, or (b) using the `source_id` parameter from Phase 05 to bulk-remove.
- **Async task cancellation**: Use `CancellationTokenSource::cancel()` from Phase 17 for all plugin-owned async tasks. Wait up to 5 seconds for tasks to finish, then proceed anyway.
- **Re-entrant protection**: Use an `std::atomic<bool> restarting_` flag. If `restart_extension_host()` is called while already restarting, return immediately with a "restart already in progress" error.
- **User notification**: Emit `ExtensionHostRestartedEvent` which includes the list of quarantined plugins. The UI (Phase 37 fault recovery dialog) shows this to the user.
- **Event**: Define `ExtensionHostRestartedEvent` in `Events.h`.
- Update `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Extension host restart completes without app restart
- [ ] All plugins deactivated during restart (with isolation)
- [ ] Faulted/quarantined plugins not re-activated
- [ ] Healthy plugins resume normal operation after restart
- [ ] Plugin EventBus subscriptions cleared during restart
- [ ] Pending async tasks cancelled during restart
- [ ] Re-entrant restart prevented (concurrent restart calls)
- [ ] User notified of restart and quarantined plugins
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test full restart sequence with mix of healthy, faulted, and quarantined plugins
- Test re-entrant protection with concurrent restart calls
- Test that healthy plugins can subscribe to events again after restart
- Test that quarantined plugins are not loaded after restart
- Test EventBus cleanup (no stale subscriptions from previous activation)
- Run under TSan to verify thread safety during restart
