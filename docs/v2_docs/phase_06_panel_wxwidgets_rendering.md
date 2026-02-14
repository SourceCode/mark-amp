# Phase 06: Panel wxWidgets Rendering

**Priority:** High
**Estimated Scope:** ~10 files affected
**Dependencies:** Phase 02

## Objective

Convert the 5 data-only UI panels (OutputPanel, ProblemsPanel, TreeViewHost, WebviewHostPanel, WalkthroughPanel) into real wxWidgets panels with proper rendering, layout, and theme integration.

## Background/Context

These 5 panels were added in v1.9.12 as pure C++ data classes. They have headers and implementations but:
- Do NOT inherit from wxPanel
- Have NO OnPaint, Create, or layout code
- Are NOT added to any sizer or parent window
- Cannot be displayed in the UI

They serve as data models that pass unit tests, but they are invisible to the user. They need to become real wxWidgets panels like ExtensionsBrowserPanel (which correctly inherits wxPanel and has full rendering).

## Detailed Tasks

### Task 1: Convert OutputPanel to wxPanel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

Change from:
```cpp
class OutputPanel { ... };
```
To:
```cpp
class OutputPanel : public wxPanel { ... };
```

Add:
- Constructor taking `wxWindow* parent`, `core::ThemeEngine&`, `core::EventBus&`, `core::OutputChannelService*`
- `CreateLayout()` -- creates a channel selector dropdown (wxChoice), a text display area (wxTextCtrl with wxTE_MULTILINE | wxTE_READONLY), and clear/lock-scroll buttons
- `ApplyTheme()` -- applies theme colors to background, text, and controls
- `OnPaint(wxPaintEvent&)` -- theme-aware background painting
- Wire channel selector change to update displayed content
- Auto-scroll behavior (scroll to bottom on new content unless locked)

### Task 2: Convert ProblemsPanel to wxPanel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

Add:
- Constructor taking `wxWindow* parent`, `core::ThemeEngine&`, `core::EventBus&`, `core::DiagnosticsService*`
- `CreateLayout()` -- creates a filter bar (error/warning/info checkboxes), a scrollable list of ProblemItem rows
- Each row: severity icon + file path + line number + message
- Click-to-navigate: publish a `FileOpenedEvent` or `GoToLineRequestEvent` on click
- `ApplyTheme()` -- severity colors (red for errors, yellow for warnings, blue for info)
- Summary bar at bottom showing counts: "0 Errors, 0 Warnings, 0 Info"

### Task 3: Convert TreeViewHost to wxPanel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/TreeViewHost.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/TreeViewHost.cpp`

Add:
- Constructor taking `wxWindow* parent`, `core::ThemeEngine&`, `core::TreeDataProviderRegistry*`
- `CreateLayout()` -- creates a wxTreeCtrl for each registered view
- `Refresh()` -- rebuilds tree from provider data
- Collapsible sections for each view (like VS Code's sidebar sections)
- `ApplyTheme()` -- theme-aware tree rendering

### Task 4: Convert WalkthroughPanel to wxPanel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp`

Add:
- Constructor taking `wxWindow* parent`, `core::ThemeEngine&`, `core::EventBus&`
- `CreateLayout()` -- scrollable list of walkthrough steps with:
  - Checkmark icons for completed steps
  - Step title and description text
  - Progress bar showing completion percentage
- `ApplyTheme()` -- theme-aware rendering
- Click on a step triggers its associated command

### Task 5: Convert WebviewHostPanel to wxPanel

**File:** `/Users/ryanrentfro/code/markamp/src/ui/WebviewHostPanel.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/WebviewHostPanel.cpp`

Add:
- Constructor taking `wxWindow* parent`, `core::ThemeEngine&`, `core::WebviewService*`
- For now, display the HTML content as rendered text using wxHtmlWindow
- `ApplyTheme()` -- theme-aware background
- Note: Full webview embedding (WebKit/CEF) is a future enhancement; wxHtmlWindow is sufficient for MVP

### Task 6: Integrate panels into LayoutManager

**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
**File:** `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

Add bottom panel area (like VS Code's terminal/output area) that can show OutputPanel, ProblemsPanel, or WalkthroughPanel. Add sidebar sections for TreeViewHost. Add tab-like switching between bottom panels.

## Acceptance Criteria

1. All 5 panels render visible content when shown
2. OutputPanel displays channel content and supports switching
3. ProblemsPanel shows diagnostic items with severity icons
4. TreeViewHost renders tree data from providers
5. WalkthroughPanel shows steps with completion tracking
6. All panels respond to theme changes
7. Existing tests pass (data-layer API unchanged)

## Testing Requirements

- Manual visual testing of each panel
- Add automated tests for panel data flow (service -> panel -> display)
- Existing test suite passes
