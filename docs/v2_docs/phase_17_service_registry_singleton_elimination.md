# Phase 17: ServiceRegistry Singleton Elimination

**Priority:** Low
**Estimated Scope:** ~5 files affected
**Dependencies:** None

## Objective

Replace the `ServiceRegistry` singleton pattern with explicit dependency injection, consistent with how all other services in the codebase are wired. The singleton is used in only 2 call sites and creates an invisible coupling that contradicts the explicit DI pattern used everywhere else.

## Background/Context

### Current State

`ServiceRegistry` (`/Users/ryanrentfro/code/markamp/src/core/ServiceRegistry.h`) is a thread-safe service locator with a Meyers singleton (`instance()`). It is used in exactly 2 places:

1. **Registration** -- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp` line 120:
   ```cpp
   core::ServiceRegistry::instance().register_service<core::IMermaidRenderer>(mermaid_renderer_);
   ```

2. **Retrieval** -- `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp` line 776:
   ```cpp
   auto mermaid = core::ServiceRegistry::instance().get<core::IMermaidRenderer>();
   ```

### Why This Is a Problem

Every other service in the codebase is passed explicitly:
- EventBus is passed as `EventBus*` or `EventBus&` through constructors
- Config is passed as `Config*` through constructors
- ThemeEngine is passed as `ThemeEngine*` or `IThemeEngine&`
- FeatureRegistry is passed as a pointer
- PluginManager receives services through its ExtensionServices struct

The ServiceRegistry singleton creates:
1. **Hidden dependency** -- PreviewPanel's dependency on IMermaidRenderer is not visible in its constructor signature
2. **Testing friction** -- Unit tests must remember to populate the global singleton before testing PreviewPanel
3. **Inconsistency** -- It is the only global mutable state in the architecture
4. **Unnecessary complexity** -- A full type-erased service locator exists for a single service

### Proposed Solution

Pass `IMermaidRenderer*` (or `std::shared_ptr<IMermaidRenderer>`) directly to PreviewPanel through its constructor, the same way EventBus and ThemeEngine are passed. Delete ServiceRegistry entirely.

## Detailed Tasks

### Task 1: Add IMermaidRenderer to PreviewPanel constructor

**File:** `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.h`

Add an `IMermaidRenderer*` parameter to the PreviewPanel constructor and store it as a member:

```cpp
class PreviewPanel : public wxPanel
{
public:
    PreviewPanel(wxWindow* parent,
                 core::EventBus& event_bus,
                 core::Config& config,
                 core::ThemeEngine& theme_engine,
                 core::IMermaidRenderer* mermaid_renderer);  // NEW

private:
    core::IMermaidRenderer* mermaid_renderer_{nullptr};  // NEW
    // ... existing members ...
};
```

### Task 2: Update PreviewPanel to use the injected renderer

**File:** `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`

Replace the ServiceRegistry lookup at line 776:
```cpp
// Before:
auto mermaid = core::ServiceRegistry::instance().get<core::IMermaidRenderer>();

// After:
auto* mermaid = mermaid_renderer_;
```

Remove the `#include "core/ServiceRegistry.h"` at line 8.

### Task 3: Update LayoutManager to pass renderer to PreviewPanel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` and `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

LayoutManager creates PreviewPanel. It needs to receive and forward the renderer pointer:

```cpp
// In LayoutManager constructor or initialization:
LayoutManager::LayoutManager(wxWindow* parent,
                              core::EventBus& event_bus,
                              core::Config& config,
                              core::ThemeEngine& theme_engine,
                              core::IMermaidRenderer* mermaid_renderer,
                              // ... other params ...)
```

If LayoutManager is created by MainFrame, then MainFrame also needs the pointer:

### Task 4: Thread IMermaidRenderer through MainFrame

**File:** `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h` and `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`

Add `IMermaidRenderer*` to MainFrame's constructor or provide a setter that is called after the renderer is created:

```cpp
// Option A: Constructor parameter
MainFrame::MainFrame(const wxString& title,
                      const wxPoint& pos,
                      const wxSize& size,
                      core::EventBus* event_bus,
                      core::Config* config,
                      core::RecentWorkspaces* recent_workspaces,
                      platform::PlatformAbstraction* platform,
                      core::ThemeEngine* theme_engine,
                      core::FeatureRegistry* feature_registry,
                      core::IMermaidRenderer* mermaid_renderer);  // NEW

// Option B: Post-init setter (if renderer created after frame)
void MainFrame::set_mermaid_renderer(core::IMermaidRenderer* renderer);
```

Looking at MarkAmpApp.cpp, the MainFrame is created at line 105 (step 9) and the MermaidRenderer at line 119 (step 10). Since the frame is created first, use **Option B** or reorder initialization:

```cpp
// Reorder: create MermaidRenderer before MainFrame
// Step 9: Create Mermaid renderer
mermaid_renderer_ = std::make_shared<core::MermaidRenderer>();

// Step 10: Create MainFrame (now with renderer available)
auto* frame = new ui::MainFrame("MarkAmp",
                                wxDefaultPosition,
                                wxSize(kDefaultWidth, kDefaultHeight),
                                event_bus_.get(),
                                config_.get(),
                                recent_workspaces_.get(),
                                platform_.get(),
                                theme_engine_.get(),
                                feature_registry_.get(),
                                mermaid_renderer_.get());
```

### Task 5: Remove ServiceRegistry

**File:** `/Users/ryanrentfro/code/markamp/src/core/ServiceRegistry.h` -- Delete this file
**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp` -- Remove `#include "core/ServiceRegistry.h"` (line 14) and the `register_service` call (line 120)
**File:** `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Remove `core/ServiceRegistry.h` from the source_group

### Task 6: Update tests

**File:** `/Users/ryanrentfro/code/markamp/tests/unit/test_crash_regressions.cpp` or any test that references ServiceRegistry

Search for `ServiceRegistry` in test files and remove or replace the references.

## Acceptance Criteria

1. `ServiceRegistry.h` is deleted
2. No file in the codebase references `ServiceRegistry`
3. `IMermaidRenderer` is passed via constructor injection to PreviewPanel
4. MermaidRenderer initialization order in MarkAmpApp.cpp is correct (before MainFrame)
5. Full build succeeds; all tests pass

## Testing Requirements

- Full build succeeds
- All tests pass (update any that used ServiceRegistry directly)
- Manual testing: Mermaid diagrams render correctly in the preview panel
- Grep for `ServiceRegistry` returns zero matches in source files
