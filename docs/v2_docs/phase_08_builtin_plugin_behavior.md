# Phase 08: BuiltIn Plugin Behavioral Activation

**Priority:** High
**Estimated Scope:** ~4 files affected
**Dependencies:** Phase 03, Phase 04

## Objective

Give the 7 built-in plugins actual runtime behavior by registering command handlers, event subscriptions, and feature-specific logic during activate().

## Background/Context

All 7 built-in plugins (MermaidPlugin, TableEditorPlugin, FormatBarPlugin, ThemeGalleryPlugin, LinkPreviewPlugin, ImagePreviewPlugin, BreadcrumbPlugin) use the generic `BuiltInPlugin::activate()` which only sets `active_ = true` and logs. They register zero command handlers and subscribe to zero events.

Each plugin should use its PluginContext to:
1. Register commands that appear in the command palette
2. Subscribe to events to implement feature logic
3. Use services (FeatureRegistry, NotificationService) for runtime behavior

## Detailed Tasks

### Task 1: Add command handlers to MermaidPlugin

**File:** `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`

Override `activate()` in MermaidPlugin:
```cpp
void activate(PluginContext& ctx) override {
    BuiltInPlugin::activate(ctx);
    ctx.register_command_handler("mermaid.toggle", [ctx]() {
        // Toggle mermaid feature
        // This requires access to FeatureRegistry -- pass via ctx or event
    });
    ctx.register_command_handler("mermaid.refreshDiagrams", [ctx]() {
        // Force re-render of all mermaid blocks
    });
}
```

Add commands to the manifest's ContributionPoints:
```cpp
.contributes = {
    .commands = {
        {.id = "mermaid.toggle", .title = "Toggle Mermaid Diagrams", .category = "Features"},
        {.id = "mermaid.refreshDiagrams", .title = "Refresh Mermaid Diagrams", .category = "Mermaid"},
    },
    .settings = { ... existing ... }
}
```

### Task 2: Add command handlers to FormatBarPlugin

Register toggle command:
```cpp
ctx.register_command_handler("format-bar.toggle", [ctx]() {
    // Toggle format bar feature
});
```

### Task 3: Add command handlers to each remaining plugin

Each plugin should register at minimum a `<feature>.toggle` command and any feature-specific commands:

- `table-editor.toggle`, `table-editor.insertTable`
- `theme-gallery.open`
- `link-preview.toggle`
- `image-preview.toggle`
- `breadcrumb.toggle`

### Task 4: Wire toggle commands to FeatureRegistry

Since PluginContext does not directly expose FeatureRegistry, use the EventBus to publish a `FeatureToggledEvent` or add FeatureRegistry as a service pointer in PluginContext.

Recommended approach: Add `FeatureRegistry*` to PluginContext (it is an application service, not an extension service):
```cpp
// In PluginContext.h
FeatureRegistry* feature_registry{nullptr};
```

Then in activate():
```cpp
ctx.register_command_handler("mermaid.toggle", [&ctx]() {
    if (ctx.feature_registry) {
        ctx.feature_registry->toggle("mermaid");
    }
});
```

### Task 5: Add deactivate() cleanup

Override deactivate() to unsubscribe from events:
```cpp
void deactivate() override {
    // Cancel any subscriptions
    subscriptions_.clear();
    BuiltInPlugin::deactivate();
}
```

## Acceptance Criteria

1. Each built-in plugin registers at least 1 command handler
2. Command palette shows "Toggle Mermaid Diagrams", "Toggle Table Editor", etc.
3. Invoking toggle commands actually enables/disables the feature
4. Plugin deactivation cleans up subscriptions
5. All existing tests pass

## Testing Requirements

- Add tests to `test_builtin_plugins.cpp` verifying command handler registration
- Add tests verifying toggle commands modify feature state
- Existing test suite passes
