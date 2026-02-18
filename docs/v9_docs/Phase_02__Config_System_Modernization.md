# Phase 02: Config System Modernization

## Overview
The Config system hardcodes 40+ defaults in Config.cpp and lacks scoped configuration (application vs workspace vs project). The SettingsCatalog exists (1748 lines) but the settings UI is not fully connected to it. This phase makes configuration first-class: validated, scoped, deep-linkable, and catalog-driven.

## Prerequisites
- Phase 01 (EventBus hardening, Config error handling)

## Tasks

### Task 1: Migrate Hardcoded Defaults to SettingsCatalog
**Files:** `src/core/Config.cpp`, `src/core/SettingsCatalog.cpp`, `src/core/SettingsCatalog.h`
**Description:** Config.cpp contains 40+ hardcoded default values. Move all defaults into SettingsCatalog entries with metadata (type, default, min/max, description, restart_required). Config should query SettingsCatalog for defaults rather than having its own.
**Acceptance Criteria:**
- Zero hardcoded defaults remain in Config.cpp
- Every setting has a SettingsCatalog entry with full metadata
- `Config::get()` falls back to SettingsCatalog default when key is not set
- Existing behavior is identical

### Task 2: Implement Scoped Configuration (Application / Workspace / Project)
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** Add three configuration scopes: Application (global), Workspace (per .markamp-workspace), and Project (per directory). Implement a cascade: Project overrides Workspace overrides Application. Each scope has its own persistence file.
**Acceptance Criteria:**
- `Config::get(key, scope)` reads from specified scope
- `Config::get(key)` cascades through Project > Workspace > Application > Default
- `Config::set(key, value, scope)` writes to the correct scope file
- Workspace config stored in `.markamp/workspace.json`
- Project config stored in `.markamp/project.json`

### Task 3: Add Config Schema Validation
**Files:** `src/core/Config.cpp`, `src/core/SettingsCatalog.h`
**Description:** Use SettingsCatalog metadata to validate every config value on load and set. Invalid values produce warnings and fall back to defaults. Validate types (string, int, float, bool, enum), ranges, and enum values.
**Acceptance Criteria:**
- `Config::set()` validates against catalog schema before writing
- Invalid values are rejected with a `SettingsValidationErrorEvent`
- Valid values produce a `SettingChangedEvent`
- Load validation reports all invalid entries at startup

### Task 4: Add Config Change Batching
**Files:** `src/core/Config.cpp`, `src/core/Events.h`
**Description:** When multiple settings change at once (e.g., applying a profile), emit a single `SettingsBatchChangedEvent` instead of individual `SettingChangedEvent` for each key. This prevents redundant UI updates.
**Acceptance Criteria:**
- `Config::begin_batch()` / `Config::commit_batch()` API added
- Within a batch, individual SettingChangedEvents are suppressed
- On commit, one SettingsBatchChangedEvent with all changed keys is emitted
- Commit with no changes emits nothing

### Task 5: Add Settings Profiles (Developer / Research / Whiteboard / Notebook)
**Files:** `src/core/Config.h`, `src/core/Config.cpp`, `src/core/SettingsCatalog.cpp`
**Description:** Define named profiles that set a collection of settings at once. Profiles map to common workflows. Switching profiles uses the batch mechanism from Task 4.
**Acceptance Criteria:**
- At least 4 built-in profiles: Developer, Research, Whiteboard, Notebook
- Each profile defines ~10-20 settings relevant to its workflow
- `Config::apply_profile(name)` applies all profile settings in one batch
- Current profile name stored and restorable on restart
- `SettingsProfileChangedEvent` emitted on profile switch

### Task 6: Add Config Export/Import
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** Add ability to export the current configuration to a JSON file and import from a JSON file. This enables sharing settings across machines and backup/restore workflows.
**Acceptance Criteria:**
- `Config::export_to(path)` writes all non-default settings to JSON
- `Config::import_from(path)` reads and validates a JSON file, applying all settings
- Import validates against schema and reports invalid entries
- `SettingsImportedEvent` / `SettingsExportedEvent` emitted

### Task 7: Wire SettingsCatalog to Extension Settings
**Files:** `src/core/PluginManager.h`, `src/core/PluginManager.cpp`, `src/core/SettingsCatalog.cpp`
**Description:** Extensions can contribute settings via their manifest (`contributes.configuration`). Wire `PluginManager::get_all_setting_contributions()` into the SettingsCatalog so extension settings appear alongside core settings.
**Acceptance Criteria:**
- Extension settings appear in SettingsCatalog with group prefix (extension.id.setting_name)
- Extension settings have type, default, and description from manifest
- Catalog rebuild occurs on extension install/uninstall
- Extension settings are persisted in the same config files as core settings

### Task 8: Add Config Deep-Link API
**Files:** `src/core/Config.h`, `src/core/Events.h`
**Description:** Extend `SettingsOpenRequestEvent` to support deep-linking to a specific setting by ID, pre-filled search query, or scope. Wire the event so that command palette commands like "Open Settings: Editor" jump directly to the editor category.
**Acceptance Criteria:**
- `SettingsOpenRequestEvent` with `setting_id` scrolls to and highlights that setting
- `SettingsOpenRequestEvent` with `query` pre-fills the search filter
- `SettingsOpenRequestEvent` with `scope` selects the scope tab
- All three can be combined

### Task 9: Add Config Diff / Modified Settings View
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** Add a method to list all settings that differ from their default values, per scope. This powers the "Modified Settings" filter in the settings UI.
**Acceptance Criteria:**
- `Config::modified_settings(scope)` returns vector of (key, current_value, default_value)
- Works for all three scopes
- Empty vector when no settings have been modified

### Task 10: Add Config Reset Per-Setting and Per-Scope
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** Add ability to reset individual settings to their default value, or reset all settings in a scope. Emit `SettingsResetEvent` on reset.
**Acceptance Criteria:**
- `Config::reset(key, scope)` removes the key from the scope, falling through to default
- `Config::reset_scope(scope)` removes all keys in the scope
- `SettingsResetEvent` emitted with setting_id (or empty for all)
- Settings UI can offer "Reset to Default" per setting and "Reset All" per scope

### Task 11: Add Config File Watcher
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** If the config file is modified externally (e.g., by another editor), detect the change and reload. This prevents config drift when users edit settings files manually.
**Acceptance Criteria:**
- File modification time is checked on `Config::get()` (debounced, max once per second)
- If file has changed, reload and emit `SettingsBatchChangedEvent` for changed keys
- No reload loop (writing config updates the tracked modification time)

### Task 12: Add Config Migration System
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** When settings are renamed or restructured across versions, old config files need migration. Add a migration registry that maps old keys to new keys with optional value transformation.
**Acceptance Criteria:**
- `Config::register_migration(old_key, new_key, transform_fn)` API
- Migrations run on load, before validation
- Migrated settings are persisted in new format
- Migration log entry for each key migrated

### Task 13: Add Settings Dependency Graph
**Files:** `src/core/SettingsCatalog.h`, `src/core/SettingsCatalog.cpp`
**Description:** Some settings depend on others (e.g., "editor.minimap.enabled" gates "editor.minimap.width"). Add dependency metadata to SettingsCatalog so the UI can grey out dependent settings when their parent is disabled.
**Acceptance Criteria:**
- SettingsCatalog entries can specify `depends_on` (setting_id and required value)
- `SettingsCatalog::is_enabled(setting_id)` checks dependency chain
- Circular dependencies are detected and reported at registration time

### Task 14: Add Config Audit Trail
**Files:** `src/core/Config.cpp`
**Description:** Log every setting change with timestamp, old value, new value, and source (user, profile, import, migration, extension). This enables debugging configuration issues.
**Acceptance Criteria:**
- Every `Config::set()` call logs at DEBUG level: key, old value, new value, source
- Audit entries available via `Config::audit_trail()` (last 100 entries)
- Audit trail is in-memory only (not persisted)

### Task 15: Add Config Snapshot and Restore
**Files:** `src/core/Config.h`, `src/core/Config.cpp`
**Description:** Before applying a profile or importing settings, take a snapshot that can be restored. This enables "undo" for settings changes.
**Acceptance Criteria:**
- `Config::take_snapshot()` returns a snapshot ID
- `Config::restore_snapshot(id)` reverts all settings to snapshot state
- Maximum 5 snapshots retained (FIFO eviction)
- Restore emits `SettingsBatchChangedEvent` for all changed keys

### Task 16: Add Restart-Required Setting Detection
**Files:** `src/core/SettingsCatalog.h`, `src/core/Config.cpp`
**Description:** Some settings require an application restart to take effect (e.g., allocator choice, platform integration). When a restart-required setting changes, emit a `NotificationEvent` informing the user.
**Acceptance Criteria:**
- SettingsCatalog `restart_required` flag is checked on setting change
- Notification emitted: "Setting X requires a restart to take effect"
- Notification is info-level, not blocking

### Task 17: Add Config Search Index
**Files:** `src/core/SettingsCatalog.cpp`
**Description:** SettingsCatalog has `keywords` metadata per setting. Build a lightweight search index that supports fuzzy matching on setting name, description, and keywords. This powers the settings search bar.
**Acceptance Criteria:**
- `SettingsCatalog::search(query)` returns matching settings ranked by relevance
- Search matches on name, description, keywords, and group name
- Substring and prefix matching supported
- Results returned in <1ms for the full catalog

### Task 18: Add Config Telemetry Events
**Files:** `src/core/Config.cpp`, `src/core/Events.h`
**Description:** Emit events when settings are accessed (read) for the first time, enabling feature usage tracking. Only emit once per setting per session to avoid noise.
**Acceptance Criteria:**
- `SettingsAccessedEvent` emitted on first read of each setting per session
- Event includes setting_id and scope
- No event on subsequent reads of the same setting
- Disabled in release builds by default (configurable)

### Task 19: Update Config Tests
**Files:** `tests/unit/test_config.cpp`, `tests/unit/test_settings_catalog.cpp`
**Description:** Extend test coverage for all new Config capabilities: scoped config, validation, batching, profiles, export/import, migration, and snapshots.
**Acceptance Criteria:**
- Scoped config: cascade reads correctly, writes to correct scope
- Validation: invalid values rejected with correct events
- Batching: batch commit emits single event
- Profiles: applying profile changes expected settings
- Export/Import: round-trip preserves all settings
- Migration: old keys map to new keys correctly
- Snapshots: restore reverts all changes

### Task 20: Add Config Documentation
**Files:** `docs/api_reference.md`
**Description:** Update the API reference with the new Config system architecture: scopes, validation, profiles, migration, and the catalog-driven approach. Include examples for extension developers.
**Acceptance Criteria:**
- Config scopes documented with precedence rules
- Profile system documented with built-in profiles listed
- Migration API documented with examples
- Extension settings contribution documented

## Testing Requirements
- All existing config tests pass
- New tests for each Config capability (scopes, validation, batching, profiles, etc.)
- Integration test: apply profile, export, restart, verify settings persist
- Test: config file corruption recovery (invalid JSON gracefully handled)

## Phase Completion Criteria
- Config is catalog-driven with zero hardcoded defaults
- Three-scope configuration cascade works correctly
- Settings are validated, batchable, and deep-linkable
- Profiles enable one-click workflow switching
- Export/import enables settings portability
- All tests pass: `cmake --preset debug && cmake --build build/debug -j$(sysctl -n hw.ncpu) && cd build/debug && ctest --output-on-failure`
