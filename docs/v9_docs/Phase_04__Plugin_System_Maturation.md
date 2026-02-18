# Phase 04: Plugin System Maturation

## Overview
The plugin system has extensive infrastructure (PluginManager 858 lines, PluginContext 25+ service pointers, ExtensionManifest, ExtensionSandbox, activation events, dependency resolution) but many PluginContext fields are null at runtime and the sandbox is incomplete. This phase makes the plugin system production-ready.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 02 (Config modernization for extension settings)

## Tasks

### Task 1: Audit and Wire All PluginContext Service Pointers
**Files:** `src/core/PluginContext.h`, `src/app/MarkAmpApp.cpp`, `src/core/PluginManager.cpp`
**Description:** PluginContext has 25+ service pointers but only a fraction are populated at runtime. For each pointer: verify it is set in MarkAmpApp::OnInit, verify the service is functional (not a stub), and either wire it properly or mark it explicitly as `nullptr` with a `// NOT_IMPLEMENTED` comment.
**Acceptance Criteria:**
- Every non-null PluginContext pointer points to a functional service
- Stub services (TaskRunnerService, TerminalService) are explicitly documented
- Test: enumerate all PluginContext fields and verify non-null for implemented ones

### Task 2: Complete ExtensionSandbox Permission Enforcement
**Files:** `src/core/ExtensionSandbox.h`, `src/core/ExtensionSandbox.cpp`
**Description:** ExtensionSandbox defines scopes (`canvas.read`, `canvas.write`, etc.) but enforcement is incomplete. Add runtime permission checks before every service API call from an extension. Blocked calls emit `CanvasWidgetPermissionDeniedEvent`.
**Acceptance Criteria:**
- Every PluginContext service call goes through permission check
- Extensions without required scope get blocked with clear error
- Permission denial emits event and returns error to extension
- Test: extension without `canvas.write` scope cannot create canvas objects

### Task 3: Add Plugin Safe-Call Error Aggregation
**Files:** `src/core/PluginSafeCall.h`, `src/core/PluginManager.cpp`
**Description:** `PluginSafeCall.h` exists for safe callback invocation. Add error aggregation: track per-plugin error counts, and quarantine plugins that exceed a threshold (e.g., 10 errors in 60 seconds).
**Acceptance Criteria:**
- Per-plugin error counter tracks callback failures
- Plugins exceeding 10 errors in 60 seconds are deactivated
- Quarantine state is reported via health panel
- Manual reactivation possible via command palette

### Task 4: Add Extension Activation Event Coverage
**Files:** `src/core/PluginManager.cpp`, `src/core/ExtensionManifest.h`
**Description:** Activation events (onLanguage, onCommand, workspaceContains, etc.) exist in the manifest but only some are implemented. Complete the activation event system so plugins can be lazily activated by precise triggers.
**Acceptance Criteria:**
- `onLanguage:X` activates when a file with language X is opened
- `onCommand:X` activates when command X is executed
- `workspaceContains:glob` activates when workspace matches glob
- `*` (star) activates on startup
- Test for each activation event type

### Task 5: Add Extension Hot-Reload
**Files:** `src/core/PluginManager.h`, `src/core/PluginManager.cpp`
**Description:** Currently extensions require a restart to update. Add hot-reload: deactivate, update files, reactivate without losing workspace state. This is critical for extension development workflow.
**Acceptance Criteria:**
- `PluginManager::reload_extension(id)` deactivates and reactivates
- Extension state (workspace_state, global_state) is preserved across reload
- Event handlers are re-subscribed
- UI contributions (commands, menus) are refreshed

### Task 6: Add Extension API Version Checking
**Files:** `src/core/ExtensionManifest.h`, `src/core/PluginManager.cpp`
**Description:** Extensions should declare a minimum API version they support. If the current API version is incompatible, the extension should not activate. Add `engines.markamp` field to manifest.
**Acceptance Criteria:**
- Manifest supports `engines: { markamp: ">=2.2.0" }` semver constraint
- Incompatible extensions are not activated, with clear log message
- Extension browser shows compatibility status

### Task 7: Add Extension Dependency Resolution Improvements
**Files:** `src/core/PluginManager.cpp`
**Description:** Topological dependency resolution exists but lacks: circular dependency detection with clear error message, optional dependencies, and dependency version constraints.
**Acceptance Criteria:**
- Circular dependencies detected at activation time with named cycle in error
- Optional dependencies: `extensionDependencies` vs `extensionOptionalDependencies`
- Version constraints: `dependency@>=1.0.0` syntax
- Test for each scenario

### Task 8: Add Extension Resource Tracking
**Files:** `src/core/ExtensionResourceTracker.h`, `src/core/PluginManager.cpp`
**Description:** ExtensionResourceTracker header exists. Wire it to track: memory usage per extension, event subscription count, registered commands, and contributed UI elements. Enforce per-extension resource limits.
**Acceptance Criteria:**
- Memory usage per extension tracked (approx, via allocator hooks)
- Event subscription count per extension tracked
- Extensions exceeding limits get warning, then deactivation
- Resource usage visible in extension detail panel

### Task 9: Add Extension Storage Encryption
**Files:** `src/core/ExtensionStorage.h`, `src/core/ExtensionStorage.cpp`
**Description:** Extension storage (workspace_state, global_state) is currently plain text. Add optional encryption for sensitive extension data using the existing EncryptionService.
**Acceptance Criteria:**
- Extensions can request encrypted storage via manifest flag
- Encrypted storage uses EncryptionService with per-extension key
- Unencrypted migration path for existing data
- Test: encrypted values are not readable as plain text on disk

### Task 10: Add Extension Contribution Point Validation
**Files:** `src/core/ExtensionManifest.cpp`
**Description:** Extension manifests declare contribution points (commands, menus, keybindings, etc.) but validation is incomplete. Add schema validation for all contribution types with clear error messages.
**Acceptance Criteria:**
- Commands: validate `command`, `title`, `category`, `icon` fields
- Keybindings: validate `key`, `command`, `when` clause
- Menus: validate `group`, `when` clause, `command` reference
- Configuration: validate `type`, `default`, `description` against allowed types

### Task 11: Add Extension Output Channel Integration
**Files:** `src/core/OutputChannelService.cpp`, `src/ui/OutputPanel.cpp`
**Description:** Extensions can create output channels but the OutputPanel (data-only, no wxPanel) never displays them. Wire OutputChannelService to the output panel so extension logs are visible.
**Acceptance Criteria:**
- Each extension's output channel appears as a tab in the output panel
- Output panel shows real-time log entries from extensions
- Channel can be cleared and filtered
- Output panel is accessible from command palette

### Task 12: Add Extension Diagnostics Integration
**Files:** `src/core/DiagnosticsService.cpp`, `src/ui/ProblemsPanel.cpp`
**Description:** DiagnosticsService exists but ProblemsPanel is data-only. Wire them so extensions that push diagnostics have their issues visible in the problems panel.
**Acceptance Criteria:**
- Extension-contributed diagnostics appear in the problems panel
- Diagnostics are grouped by source (extension ID)
- Clicking a diagnostic navigates to the source location
- Problems panel shows severity counts in its tab badge

### Task 13: Add Extension TreeDataProvider Rendering
**Files:** `src/core/TreeDataProviderRegistry.cpp`, `src/ui/TreeViewHost.cpp`
**Description:** TreeDataProviderRegistry and TreeViewHost exist but are not wired for actual rendering. Extensions that register tree data providers should have their trees visible in the sidebar.
**Acceptance Criteria:**
- Extension-contributed tree views appear in the sidebar
- Tree data refreshes when the provider emits changes
- Tree items support icons, labels, and context menus
- Collapse/expand state is preserved

### Task 14: Add Extension Webview Panel Support
**Files:** `src/core/WebviewService.cpp`, `src/ui/WebviewHostPanel.cpp`
**Description:** WebviewService and WebviewHostPanel exist but webview rendering is not functional. Add basic HTML content rendering for extension-contributed webview panels.
**Acceptance Criteria:**
- Extensions can create webview panels with HTML content
- Webview panel renders HTML via wxWebView
- Message passing between extension and webview (postMessage/onMessage)
- Webview panels can be placed in sidebar or editor area

### Task 15: Add Extension Telemetry Dashboard
**Files:** `src/core/ExtensionTelemetry.cpp`, `src/ui/ExtensionDetailPanel.cpp`
**Description:** ExtensionTelemetry exists but data is not surfaced. Add an extension health dashboard showing: activation time, error count, resource usage, and API call frequency per extension.
**Acceptance Criteria:**
- Extension detail panel shows health metrics
- Activation time, error count, memory usage visible
- API call frequency per service shown
- Unhealthy extensions highlighted

### Task 16: Add Built-In Plugin Test Coverage
**Files:** `tests/unit/test_builtin_plugin_behavior.cpp`, `tests/unit/test_builtin_plugins.cpp`
**Description:** 7 built-in plugins exist (Mermaid, Table Editor, Format Bar, Theme Gallery, Link Preview, Image Preview, Breadcrumb) but behavioral test coverage is thin. Add tests verifying each plugin's activate/deactivate lifecycle and feature toggling.
**Acceptance Criteria:**
- Each built-in plugin tested: activate, verify features, deactivate, verify cleanup
- Feature toggling verified for each plugin
- Plugin isolation verified: deactivating one doesn't affect others

### Task 17: Add Extension VSIX Install Verification
**Files:** `src/core/VsixService.cpp`, `src/core/VsixService.h`
**Description:** VSIX install exists but needs verification: manifest checksum, file integrity, and rollback on failure. Add post-install validation.
**Acceptance Criteria:**
- VSIX install verifies manifest is valid before extraction
- File integrity checked after extraction
- Failed install rolls back extracted files
- Success/failure reported via notification

### Task 18: Add Extension Marketplace Search
**Files:** `src/core/GalleryService.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** GalleryService has HTTP client integration. Add search functionality to the extensions browser: search by name, filter by category, sort by popularity/date.
**Acceptance Criteria:**
- Search bar in extensions browser filters by name
- Category filters (themes, languages, tools, canvas)
- Sort options (name, date, relevance)
- Results show name, description, author, install count

### Task 19: Add Extension Recommendation Engine Improvements
**Files:** `src/core/ExtensionRecommendations.cpp`
**Description:** ExtensionRecommendations exists. Improve it to recommend based on: file types in workspace, installed extensions' complementary extensions, and user workflow patterns.
**Acceptance Criteria:**
- Recommendations based on workspace file types
- Complementary extensions suggested (e.g., Mermaid suggests Flowchart)
- Recommendations appear in extensions browser sidebar
- User can dismiss recommendations

### Task 20: Add Extension API Reference Documentation
**Files:** `docs/api_reference.md`
**Description:** Document the complete extension API surface: PluginContext services, activation events, contribution points, manifest schema, and sandbox permissions.
**Acceptance Criteria:**
- Every PluginContext service documented with API methods
- Manifest schema fully documented with examples
- Activation events listed with trigger conditions
- Sandbox permissions listed with scope descriptions
- Example extension manifest included

## Testing Requirements
- All existing plugin tests pass
- New tests for: permission enforcement, error aggregation, activation events
- Integration test: install VSIX, activate, use features, deactivate, uninstall
- Sandbox test: extension cannot access restricted APIs without permission

## Phase Completion Criteria
- All PluginContext service pointers properly wired or documented as stubs
- Sandbox permissions enforced at runtime
- Extension error handling prevents cascade failures
- Extension UI contributions (output, problems, tree, webview) are functional
- All tests pass
