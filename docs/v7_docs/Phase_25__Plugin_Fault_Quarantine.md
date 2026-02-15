# Phase 25: Plugin Fault Quarantine

## Metadata

| Field | Value |
|---|---|
| Phase ID | 25 |
| Prerequisites | Phase 22 (plugin error counter) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PII-5 (plugin fault quarantine) |

---

## Objective

Implement a quarantine mechanism that prevents repeatedly-crashing plugins from auto-loading on subsequent startups. Quarantine persists across sessions and can be manually cleared by the user.

---

## Background

The PRD mandates plugin fault quarantine (PII-5). Phase 22 adds per-session error counting and auto-disable. But if a plugin crashes the extension host 5 times across different sessions, it should be quarantined — not loaded at all on subsequent startups — until the user explicitly un-quarantines it. This prevents a boot loop where a broken plugin crashes the host, recovers, reloads, and crashes again.

---

## Scope

### Tasks

1. **Create `src/core/PluginQuarantine.h` / `PluginQuarantine.cpp`**:
   ```cpp
   namespace markamp::core {

   struct QuarantineEntry {
       std::string extension_id;
       std::string reason;
       std::string last_stack_trace;
       std::chrono::system_clock::time_point quarantined_at;
       size_t total_failures{0};
   };

   class PluginQuarantine {
   public:
       explicit PluginQuarantine(std::filesystem::path quarantine_file);

       // Check if a plugin is quarantined
       [[nodiscard]] auto is_quarantined(std::string_view extension_id) const -> bool;

       // Quarantine a plugin
       void quarantine(std::string_view extension_id,
                       std::string_view reason,
                       std::string_view stack_trace = "");

       // Un-quarantine a plugin (manual user action)
       void release(std::string_view extension_id);

       // Get all quarantined plugins
       [[nodiscard]] auto entries() const -> std::vector<QuarantineEntry>;

       // Record a failure for a plugin (may trigger quarantine)
       void record_failure(std::string_view extension_id,
                           std::string_view error_message);

       // Load quarantine list from disk
       auto load() -> Result<void>;

       // Save quarantine list to disk
       auto save() -> Result<void>;

   private:
       std::filesystem::path quarantine_file_;  // ~/.markamp/quarantine.json
       std::unordered_map<std::string, QuarantineEntry> entries_;
       mutable std::mutex mutex_;

       static constexpr size_t kQuarantineThreshold = 5;  // failures across sessions
   };

   } // namespace markamp::core
   ```
   - Quarantine triggered after 5 failures across sessions (configurable)
   - Quarantine persisted to `~/.markamp/quarantine.json`
   - File format: JSON array of QuarantineEntry objects
   - Use AtomicWriter (Phase 31 if available, or temp+rename pattern) for safe persistence
   - Log when a plugin is quarantined with full details

2. **Modify `src/core/PluginManager.cpp`**:
   - During `activate_all()`, check quarantine before loading each plugin:
     ```cpp
     for (auto& plugin : discovered_plugins) {
         if (quarantine.is_quarantined(plugin.id())) {
             MARKAMP_LOG_INFO_S(SubsystemId::ExtensionHost,
                 "Skipping quarantined plugin: {}", plugin.id());
             continue;
         }
         // ... activate plugin ...
     }
     ```
   - When Phase 22's error counter marks a plugin as faulted, call `quarantine.record_failure()`
   - After session ends (app exit), save quarantine state

3. **Create `tests/unit/test_plugin_quarantine.cpp`**:
   - TEST_CASE: "Plugin not quarantined by default"
   - TEST_CASE: "Plugin quarantined after threshold failures"
   - TEST_CASE: "Quarantined plugin skipped during activate_all"
   - TEST_CASE: "Manual release un-quarantines plugin"
   - TEST_CASE: "Quarantine persists to file"
   - TEST_CASE: "Quarantine loads from file"
   - TEST_CASE: "Quarantine survives app restart (save + load)"
   - TEST_CASE: "Quarantine entry includes reason and timestamp"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/PluginQuarantine.h` |
| Create | `src/core/PluginQuarantine.cpp` |
| Modify | `src/core/PluginManager.cpp` |
| Create | `tests/unit/test_plugin_quarantine.cpp` |

---

## Implementation Notes

- **Persistence format**: Use JSON for the quarantine file. Example:
  ```json
  [
    {
      "extension_id": "bad-plugin",
      "reason": "Activation failed 5 times",
      "last_stack_trace": "...",
      "quarantined_at": "2026-02-15T10:30:00Z",
      "total_failures": 5
    }
  ]
  ```
- **Cross-session failure counting**: `record_failure()` increments the failure count and persists immediately. This ensures failures are tracked even if the app crashes before normal exit.
- **Quarantine file location**: `~/.markamp/quarantine.json`. Create the directory if it doesn't exist.
- **Safe file I/O**: Use temp file + rename pattern for atomic writes. Load with bounded parsing (Phase 09) to prevent malformed quarantine files from crashing.
- **User un-quarantine**: The `release()` method is called from the Settings UI. After release, the plugin will be attempted on next startup.
- **Thread safety**: The quarantine is accessed from the plugin activation thread and potentially from UI. Mutex-protect all access.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Quarantined plugin skipped during `activate_all()`
- [ ] Quarantine triggered after 5 failures across sessions
- [ ] Quarantine list survives app restart (persisted to disk)
- [ ] Manual un-quarantine via `release()` works
- [ ] Quarantine entry includes extension_id, reason, timestamp, failure count
- [ ] Quarantine file written atomically (no corruption on crash)
- [ ] Quarantine log includes plugin ID and reason
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test quarantine lifecycle: failures → quarantine → persist → load → skip → release → activate
- Test persistence with temp file (write, read back, verify contents)
- Test that quarantine threshold is correct (4 failures = not quarantined, 5 = quarantined)
- Test with corrupted quarantine file (should load gracefully, treat as empty)
- Test thread safety with concurrent record_failure calls
