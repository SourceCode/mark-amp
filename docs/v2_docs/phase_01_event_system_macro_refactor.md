# Phase 01: Event System Macro Refactor & Deduplication

**Priority:** Critical
**Estimated Scope:** ~5 files affected
**Dependencies:** None

## Objective

Eliminate 1200+ lines of boilerplate in `Events.h` by introducing a macro or template pattern for event declaration. Remove duplicate event types that describe the same action.

## Background/Context

`src/core/Events.h` is 1513 lines long. Every event struct follows an identical pattern:
```cpp
struct FooRequestEvent : Event {
    [[nodiscard]] auto type_name() const -> std::string_view override {
        return "FooRequestEvent";
    }
};
```

This results in ~8 lines per event for ~120+ events. A compile-time macro can reduce each to 1 line. Additionally, several events are duplicated under slightly different names (identified below).

## Detailed Tasks

### Task 1: Create MARKAMP_DECLARE_EVENT macro

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

Add at the top of the file (after includes):
```cpp
/// Macro to declare a simple event type with no fields.
#define MARKAMP_DECLARE_EVENT(EventName)                                      \
    struct EventName : Event                                                  \
    {                                                                         \
        [[nodiscard]] auto type_name() const -> std::string_view override     \
        {                                                                     \
            return #EventName;                                                \
        }                                                                     \
    }

/// Macro to declare an event type with custom fields.
/// Usage: MARKAMP_DECLARE_EVENT_BEGIN(MyEvent) ... fields ... MARKAMP_DECLARE_EVENT_END(MyEvent)
#define MARKAMP_DECLARE_EVENT_BEGIN(EventName)                                \
    struct EventName : Event                                                  \
    {                                                                         \
        [[nodiscard]] auto type_name() const -> std::string_view override     \
        {                                                                     \
            return #EventName;                                                \
        }

#define MARKAMP_DECLARE_EVENT_END()                                           \
    }
```

### Task 2: Convert all simple (fieldless) events to macro form

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

Replace approximately 90 simple event structs. For example:
```cpp
// Before (8 lines):
struct FindRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FindRequestEvent";
    }
};

// After (1 line):
MARKAMP_DECLARE_EVENT(FindRequestEvent);
```

Events to convert (all that have no data fields):
- All R6-R13 editor action events (FindRequestEvent through ConvertEolCrlfRequestEvent)
- AppReadyEvent, AppShutdownEvent
- SettingsOpenRequestEvent
- GoToLineRequestEvent
- WorkspaceRefreshRequestEvent, ShowStartupRequestEvent
- ShowExtensionsBrowserRequestEvent, ShowExplorerRequestEvent
- NewFileRequestEvent, PrintDocumentRequestEvent
- And ~70 more

### Task 3: Convert events with fields to BEGIN/END macro form

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

Events with data fields keep their fields but use the macro for the type_name() boilerplate:
```cpp
MARKAMP_DECLARE_EVENT_BEGIN(ThemeChangedEvent)
    std::string theme_id;
MARKAMP_DECLARE_EVENT_END();
```

Events requiring this treatment:
- ThemeChangedEvent (theme_id)
- FileOpenedEvent (file_path, content)
- FileContentChangedEvent (file_id, new_content)
- FileSavedEvent (file_path)
- ActiveFileChangedEvent (file_id)
- ViewModeChangedEvent (mode, constructor)
- CursorPositionChangedEvent (line, column, selection_length)
- EditorContentChangedEvent (content)
- EditorStatsChangedEvent (word_count, char_count, line_count, selection_length)
- EditorScrollChangedEvent (scroll_fraction)
- NotificationEvent (message, level, duration_ms, constructor)
- TabSwitchedEvent, TabCloseRequestEvent, TabSaveRequestEvent, etc. (file_path)
- SettingChangedEvent (key, value)
- PluginActivatedEvent, PluginDeactivatedEvent (plugin_id)
- FeatureToggledEvent (feature_id, enabled)
- ExtensionInstalledEvent, ExtensionUninstalledEvent, ExtensionEnablementChangedEvent
- FocusModeChangedEvent (active)
- TabDuplicateRequestEvent (file_path)
- ActivityBarSelectionEvent (item)
- SidebarToggleEvent (visible)
- UIScaleChangedEvent (scale_factor)
- InputModeChangedEvent (using_keyboard)
- OpenFolderRequestEvent (path)
- WorkspaceOpenRequestEvent (path)
- ScrollSyncModeChangedEvent (mode)

### Task 4: Identify and consolidate duplicate events

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

The following events are semantically identical and should be consolidated:

| Keep | Remove (alias or redirect) | Reason |
|------|---------------------------|--------|
| `ToggleMinimapRequestEvent` | `ToggleMinimapR11RequestEvent` | R11 is a version suffix, not semantic |
| `ReverseLinesRequestEvent` | `ReverseSelectedLinesRequestEvent` | Same action |
| `TransposeCharsRequestEvent` | `TransposeCharactersRequestEvent` | Same action, different abbreviation |
| `FoldCurrentRequestEvent` | `FoldCurrentRegionRequestEvent` | Same action |
| `UnfoldCurrentRequestEvent` | `UnfoldCurrentRegionRequestEvent` | Same action |
| `JumpToBracketRequestEvent` | `JumpToMatchingBracketRequestEvent` | Same action |
| `DeleteLineRequestEvent` | `DeleteCurrentLineRequestEvent` | Same action |

For each duplicate, search all `.cpp` files for usage and update references to use the canonical name.

**Files to search:**
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` (event subscriptions)
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` (menu item event publishing)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (event handlers)
- `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (palette commands)

### Task 5: Add SettingsBatchChangedEvent

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

HISTORY.md v1.8.11 documents a `SettingsBatchChangedEvent` but it does not exist. Add it:
```cpp
MARKAMP_DECLARE_EVENT_BEGIN(SettingsBatchChangedEvent)
    std::vector<std::string> changed_keys;
MARKAMP_DECLARE_EVENT_END();
```

## Acceptance Criteria

1. `Events.h` is reduced from ~1513 lines to ~400 lines
2. All existing tests pass without modification (macro produces identical types)
3. No duplicate event names exist
4. `SettingsBatchChangedEvent` is defined and usable
5. `grep -c "struct.*: Event" src/core/Events.h` returns 0 (all use macros)

## Testing Requirements

- Run existing test suite: all 21+ targets must pass
- Verify event type_name() returns correct strings by adding a compile-time test
- Add a test case in `test_event_bus.cpp` for `SettingsBatchChangedEvent`
