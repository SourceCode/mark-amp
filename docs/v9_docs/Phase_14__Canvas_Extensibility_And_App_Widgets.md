# Phase 14: Canvas Extensibility and App Widgets

## Overview
CanvasAppManifest, CanvasIntegrationService, and AppWidgetObject exist as infrastructure for extension-contributed canvas apps and data-connected widgets. This phase completes the canvas extension platform so third-party developers can add tools, objects, and integrations.

## Prerequisites
- Phase 04 (Plugin system maturation)
- Phase 11 (Canvas workbench shell)
- Phase 12 (Canvas advanced objects)

## Tasks

### Task 1: Complete Canvas Contribution Point Schema
**Files:** `src/core/ExtensionManifest.h`, `src/core/ExtensionManifest.cpp`, `src/core/CanvasAppManifest.h`
**Description:** Define and validate manifest sections: `canvasTools`, `canvasWidgets`, `canvasTemplates`, `canvasInspectors`, `canvasActions` with schema validation and versioned capability flags.
**Acceptance Criteria:**
- All 5 canvas contribution points defined in manifest schema
- Schema validation on extension load
- Capability version flags for forward compatibility
- Test: invalid manifest produces clear error

### Task 2: Wire CanvasApi for Extension Runtime
**Files:** `src/core/PluginContext.h`, `src/core/PluginManager.cpp`
**Description:** Add `CanvasApi` surface to PluginContext: create/update/delete objects, query selection, subscribe to viewport/selection/tool changes, open inspector tabs, register contextual actions.
**Acceptance Criteria:**
- Extensions can create canvas objects programmatically
- Extensions can query current selection
- Extensions can subscribe to canvas events
- Extensions can register inspector tabs
- All API calls go through permission checks

### Task 3: Wire Canvas Permission Scopes
**Files:** `src/core/ExtensionSandbox.cpp`
**Description:** Enforce canvas-specific permission scopes: `canvas.read`, `canvas.write`, `canvas.selection`, `canvas.comments`, `canvas.templates`, `canvas.network`.
**Acceptance Criteria:**
- Extensions without `canvas.write` cannot create/modify objects
- Extensions without `canvas.read` cannot query objects
- Permission denial produces clear error event
- Scope required declaration in manifest

### Task 4: Wire AppWidgetObject Rendering
**Files:** `src/canvas/AppWidgetObject.cpp`, `src/canvas/CanvasRenderer.cpp`
**Description:** AppWidgetObject exists. Wire rendering: extension-provided HTML content rendered inside a canvas object frame with header bar showing app name and sync status.
**Acceptance Criteria:**
- App widgets render HTML content in canvas frame
- Header shows app name, sync indicator, collapse button
- Widgets resizable
- Widget content refreshes on sync

### Task 5: Wire CanvasIntegrationService Sync Pipeline
**Files:** `src/core/CanvasIntegrationService.cpp`
**Description:** CanvasIntegrationService has TODO for display name. Complete the sync pipeline: provider connections, sync jobs with retry, rate limiting, and status emission.
**Acceptance Criteria:**
- Sync jobs execute with configurable retry (3 attempts, exponential backoff)
- Rate limiting prevents API abuse
- Sync status emitted via events
- Failure events include error details

### Task 6: Wire Board Serialization for App Widgets
**Files:** `src/canvas/BoardSerializer.cpp`
**Description:** App widget state, provider metadata, and sync links must persist and restore deterministically in board serialization.
**Acceptance Criteria:**
- App widget state saved in board JSON
- Provider metadata (app_id, config) preserved
- Sync link URLs preserved
- Load restores widget with correct provider connection

### Task 7: Wire Apps Drawer UI
**Files:** `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Build an "Apps Drawer" panel listing installed canvas-capable extensions. Support pin/unpin for quick access. Launch app panels through WebviewHostPanel.
**Acceptance Criteria:**
- Apps drawer accessible from canvas toolbar
- Lists installed canvas-capable extensions
- Pin/unpin for favorites
- Click launches app in sidebar or floating panel

### Task 8: Wire Extension Template Packs
**Files:** `src/canvas/BoardTemplate.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Extension-contributed board templates merged with built-in templates. Template gallery shows source filter: Built-in, Workspace, Extension.
**Acceptance Criteria:**
- Extension templates appear in template gallery
- Source filter distinguishes built-in vs extension
- Extension templates install with extension
- Uninstalling extension removes templates

### Task 9: Add Canvas App Marketplace Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register commands: "Canvas Apps Marketplace", "Insert App Widget", "Sync All Widgets", "Manage Canvas App Permissions".
**Acceptance Criteria:**
- All commands registered in command palette
- "Insert App Widget" shows widget picker
- "Sync All Widgets" triggers sync for all active widgets
- "Manage Permissions" opens permission settings

### Task 10: Wire Canvas App Diagnostics Output
**Files:** `src/core/OutputChannelService.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Integration logs published to output channel. Active sync state shown in status bar.
**Acceptance Criteria:**
- Each canvas app has an output channel
- Sync start/complete/fail logged
- Status bar shows active sync count
- Error badge on widget when sync fails

### Task 11: Add Extension-Contributed Tool Registration
**Files:** `src/canvas/CanvasInputManager.cpp`, `src/core/PluginManager.cpp`
**Description:** Extensions can register custom canvas tools that appear in the tool rail alongside built-in tools.
**Acceptance Criteria:**
- Extensions register tools via manifest `canvasTools`
- Custom tools appear in tool rail
- Tool lifecycle (activate, use, deactivate) follows same pattern
- Custom tool icon from extension

### Task 12: Add Extension-Contributed Object Renderers
**Files:** `src/canvas/CanvasRenderer.cpp`
**Description:** Extensions can register custom object renderers for new object types using the IObjectRenderer pattern.
**Acceptance Criteria:**
- Extensions register renderers via `canvasWidgets` contribution
- Custom renderers invoked for matching object types
- Fallback to default renderer if extension unavailable
- Renderer isolation: crash in extension renderer doesn't crash canvas

### Task 13: Add Extension-Contributed Inspector Widgets
**Files:** `src/canvas/MetadataPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Extensions can contribute inspector tabs for their object types. Inspector shows extension-provided UI when their objects are selected.
**Acceptance Criteria:**
- Extension inspector tabs appear when custom objects selected
- Tabs show extension-provided HTML content
- Message passing between inspector and extension
- Tab hidden when no matching objects selected

### Task 14: Add Canvas Extension Lifecycle Hooks
**Files:** `src/core/PluginManager.cpp`, `src/core/Events.h`
**Description:** Extensions receive lifecycle hooks: board-open, board-close, tool-change, selection-change. These enable extensions to react to canvas state changes.
**Acceptance Criteria:**
- `onBoardOpen` hook called when board loads
- `onBoardClose` hook called when board closes
- `onToolChange` hook called on tool switch
- `onSelectionChange` hook called on selection change

### Task 15: Add Canvas Extension Security Sandbox
**Files:** `src/core/ExtensionSandbox.cpp`
**Description:** Canvas extensions run in a sandboxed context: no direct DOM access, limited canvas API surface, resource limits enforced.
**Acceptance Criteria:**
- Extensions cannot access canvas DC directly
- API calls rate-limited
- Memory usage per extension tracked
- Extension crash contained (does not crash canvas)

### Task 16: Add Widget Auto-Refresh Scheduling
**Files:** `src/core/CanvasIntegrationService.cpp`
**Description:** Widgets can configure auto-refresh intervals. The integration service schedules periodic sync jobs.
**Acceptance Criteria:**
- Widget manifest specifies refresh interval (min 30 seconds)
- Integration service schedules periodic syncs
- Manual refresh available via context menu
- Refresh paused when board not visible

### Task 17: Add Canvas App Permission UI
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/ExtensionSandbox.cpp`
**Description:** Settings page for managing canvas app permissions per extension. Users can grant/revoke scopes.
**Acceptance Criteria:**
- Per-extension permission list in settings
- Toggle for each scope
- Changes take effect immediately
- Revoking scope deactivates dependent features

### Task 18: Add Canvas Extension Error Handling
**Files:** `src/core/PluginSafeCall.h`, `src/canvas/CanvasRenderer.cpp`
**Description:** Errors in extension code (renderers, tools, lifecycle hooks) are caught and reported without disrupting the canvas.
**Acceptance Criteria:**
- Extension errors logged with extension ID
- Error count tracked per extension
- Error badge shown on widget
- Excessive errors quarantine extension

### Task 19: Add Canvas Extension Tests
**Files:** `tests/unit/test_canvas_apps.cpp`, `tests/unit/test_extension_manifest.cpp`
**Description:** Test canvas extension system: manifest parsing, permission enforcement, widget lifecycle, sync pipeline, and error handling.
**Acceptance Criteria:**
- Manifest parsing for all canvas contribution points
- Permission enforcement for all scopes
- Widget create/sync/destroy lifecycle
- Sync retry and failure handling
- Error containment

### Task 20: Add Canvas Extension Documentation
**Files:** `docs/api_reference.md`
**Description:** Document canvas extension API: contribution points, CanvasApi methods, permission scopes, lifecycle hooks, and example manifest.
**Acceptance Criteria:**
- All canvas contribution points documented
- CanvasApi methods with parameters and return types
- Permission scopes with descriptions
- Example extension manifest for canvas app
- Best practices for performance

## Testing Requirements
- Extension manifest validation for all canvas contribution points
- Permission enforcement for all scopes
- Widget lifecycle: create, sync, destroy, error
- Sync pipeline: success, retry, failure

## Phase Completion Criteria
- Extensions can contribute tools, widgets, templates, and inspectors
- Canvas API provides safe, sandboxed access to canvas operations
- Widget sync pipeline with retry and error handling
- Apps drawer UI for discovering and managing canvas extensions
- All tests pass
