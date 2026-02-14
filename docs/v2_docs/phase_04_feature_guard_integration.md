# Phase 04: Feature Guard Integration

**Priority:** Critical
**Estimated Scope:** ~15 files affected
**Dependencies:** Phase 03

## Objective

Wire the FeatureRegistry into all UI components that correspond to built-in features, so that disabling a feature actually hides/disables its UI. Currently, 7 features are registered but never checked.

## Background/Context

`register_builtin_plugins()` registers these features in the FeatureRegistry:
- `mermaid` -- Mermaid diagram rendering
- `table-editor` -- Table editing overlay
- `format-bar` -- Floating format bar
- `theme-gallery` -- Theme gallery panel
- `link-preview` -- Link preview popover
- `image-preview` -- Image preview popover
- `breadcrumb` -- Breadcrumb bar

However, no UI code calls `feature_registry_->is_enabled("mermaid")` (or any other feature ID). The FeatureRegistry is instantiated but never passed to any UI component. Features cannot actually be toggled.

## Detailed Tasks

### Task 1: Pass FeatureRegistry to MainFrame

**File:** `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`

MainFrame already receives `FeatureRegistry*` in its constructor (confirmed in MarkAmpApp.cpp line 113). Verify it stores and forwards it to LayoutManager.

### Task 2: Pass FeatureRegistry to LayoutManager

**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

Add `core::FeatureRegistry*` parameter. LayoutManager creates EditorPanel, PreviewPanel, BreadcrumbBar, etc. It must pass the registry or check features before creating/showing components.

### Task 3: Guard BreadcrumbBar visibility

**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

Before creating/showing BreadcrumbBar, check:
```cpp
bool show_breadcrumb = feature_registry_ && feature_registry_->is_enabled("breadcrumb");
```

Subscribe to `FeatureToggledEvent` and show/hide the breadcrumb bar when toggled:
```cpp
feature_toggle_sub_ = event_bus_.subscribe<events::FeatureToggledEvent>(
    [this](const events::FeatureToggledEvent& e) {
        if (e.feature_id == "breadcrumb") {
            breadcrumb_bar_->Show(e.enabled);
            Layout();
        }
    });
```

### Task 4: Guard FloatingFormatBar visibility

**File:** `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

The format bar appears on text selection. Before showing it, check:
```cpp
if (feature_registry_ && !feature_registry_->is_enabled("format-bar")) return;
```

### Task 5: Guard LinkPreviewPopover and ImagePreviewPopover

**File:** `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

These popovers appear on hover/dwell events. Before showing:
```cpp
if (feature_registry_ && !feature_registry_->is_enabled("link-preview")) return;
if (feature_registry_ && !feature_registry_->is_enabled("image-preview")) return;
```

### Task 6: Guard TableEditorOverlay

**File:** `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

Before showing the table editor overlay:
```cpp
if (feature_registry_ && !feature_registry_->is_enabled("table-editor")) return;
```

### Task 7: Guard Mermaid rendering

**File:** `/Users/ryanrentfro/code/markamp/src/rendering/HtmlRenderer.cpp`

At line 325 where mermaid blocks are rendered, check:
```cpp
// Only render mermaid if the feature is enabled
if (feature_registry_ && feature_registry_->is_enabled("mermaid")) {
    output += mermaid_block.render(...);
} else {
    output += mermaid_block.render_placeholder(node.text_content);
}
```

This requires passing FeatureRegistry* to HtmlRenderer or checking at a higher level in PreviewPanel.

### Task 8: Guard ThemeGallery in ActivityBar/LayoutManager

**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

When the Themes activity bar item is selected, check if the theme gallery feature is enabled before showing the gallery panel.

### Task 9: Add feature toggle commands to Settings Panel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`

The SettingsPanel already renders settings. Verify that the built-in plugin settings (e.g., `mermaid.enabled`, `table-editor.enabled`) appear in the settings UI and that changing them calls `feature_registry_->set_enabled()`.

### Task 10: Subscribe to FeatureToggledEvent in affected components

Each guarded component should listen for `FeatureToggledEvent` and dynamically show/hide without requiring a restart.

## Acceptance Criteria

1. Disabling "breadcrumb" in settings hides the breadcrumb bar immediately
2. Disabling "format-bar" prevents the floating format bar from appearing
3. Disabling "link-preview" / "image-preview" prevents popovers
4. Disabling "mermaid" shows placeholder instead of rendered diagrams
5. Disabling "table-editor" prevents the table overlay
6. All feature toggles persist across restarts (via Config)
7. Existing tests pass

## Testing Requirements

- Add integration test verifying feature disable -> UI hidden
- Add test verifying FeatureToggledEvent triggers UI update
- Existing test suite passes
