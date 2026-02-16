Phase 13 Prompt (Canvas Apps Marketplace + Data-Connected Widgets)

1. Add a canvas-app contribution model in `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionManifest.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionManifest.cpp` with new sections: `canvasTools`, `canvasWidgets`, `canvasTemplates`, `canvasInspectors`, and `canvasActions`, including schema validation and versioned capability flags.

2. Extend plugin runtime APIs in `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginContext.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginManager.cpp` with a `CanvasApi` surface: create/update/delete objects, query selection, subscribe to viewport/selection/tool changes, open inspector tabs, and register contextual actions.

3. Add permission scopes for canvas integrations in `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionSandbox.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionSandbox.cpp` (example scopes: `canvas.read`, `canvas.write`, `canvas.selection`, `canvas.comments`, `canvas.templates`, `canvas.network`) and block activation when required scopes are not granted.

4. Build an “Apps Drawer” UI in `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp` that lists installed canvas-capable extensions, supports pin/unpin, and launches app panels through `/Volumes/SecondDrive/code2/mark-amp/src/ui/WebviewHostPanel.cpp`.

5. Add in-canvas widget host objects in `/Volumes/SecondDrive/code2/mark-amp/src/canvas/AppWidgetObject.h` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/AppWidgetObject.cpp` plus renderer support in `/Volumes/SecondDrive/code2/mark-amp/src/canvas/CanvasRenderer.cpp` for embeddable app cards (data table cards, issue cards, embeds, custom dashboards).

6. Extend board serialization for app objects in `/Volumes/SecondDrive/code2/mark-amp/src/canvas/BoardSerializer.h` and `/Volumes/SecondDrive/code2/mark-amp/src/canvas/BoardSerializer.cpp` so app widget state, provider metadata, and sync links persist and restore deterministically.

7. Add a canvas integration orchestration service in `/Volumes/SecondDrive/code2/mark-amp/src/core/CanvasIntegrationService.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/CanvasIntegrationService.cpp` to manage provider connections, sync jobs, rate limits, retry strategy, and status emission to UI.

8. Add canvas integration events in `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`: `CanvasAppRegisteredEvent`, `CanvasWidgetInsertedEvent`, `CanvasWidgetSyncStartedEvent`, `CanvasWidgetSyncCompletedEvent`, `CanvasWidgetSyncFailedEvent`, and `CanvasWidgetPermissionDeniedEvent`.

9. Expand template workflows by merging extension template packs into `/Volumes/SecondDrive/code2/mark-amp/src/canvas/BoardTemplate.cpp` and exposing category/source filters (“Built-in”, “Workspace”, “Extension”) in `/Volumes/SecondDrive/code2/mark-amp/src/ui/CanvasWorkspacePanel.cpp`.

10. Add menu and command access in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` for `Canvas Apps Marketplace`, `Insert App Widget`, `Sync All Widgets`, and `Manage Canvas App Permissions`, with corresponding shortcuts and command palette entries.

11. Wire observability and output diagnostics by publishing integration logs to `/Volumes/SecondDrive/code2/mark-amp/src/core/OutputChannelService.cpp` and showing active sync state in `/Volumes/SecondDrive/code2/mark-amp/src/ui/StatusBarPanel.cpp` and canvas top bar.

12. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_extension_manifest.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_extension_management.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_board_serializer.cpp`, and new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_apps.cpp` for manifest parsing, permission enforcement, widget persistence, sync lifecycle, and event correctness; acceptance requires app widgets install/render/sync through extension APIs with no hard-coded UI colors and passing build/tests.
