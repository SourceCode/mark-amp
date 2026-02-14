# Phase 14: Naming Convention Standardization

**Priority:** Medium
**Estimated Scope:** ~20 files affected
**Dependencies:** None

## Objective

Standardize naming conventions across the codebase to eliminate inconsistencies in class names, file names, and identifier casing that have accumulated across multiple development phases.

## Background/Context

Several naming inconsistencies exist:

### Issue 1: "Builtin" vs "BuiltIn" casing

Two different casings are used for the same concept:
- `BuiltinThemes` (file: `core/BuiltinThemes.h`, `core/BuiltinThemes.cpp`)
- `BuiltInPlugins` (file: `core/BuiltInPlugins.h`, `core/BuiltInPlugins.cpp`)

Both are compound words meaning "included by default." The VS Code convention and the more common C++ compound-word pattern capitalize both parts: "BuiltIn" (like "SetUp", "TearDown", "ReadOnly"). The inconsistency confuses developers looking for related code.

### Issue 2: Event naming suffixes

Most events use the pattern `<Action>RequestEvent` or `<Action>Event`, but there are exceptions:
- `ThemeChangedEvent` vs `SettingChangedEvent` (past tense -- consistent)
- `ShowExtensionsBrowserRequestEvent` (uses "Show" prefix + "Request" suffix)
- `TabSwitchedEvent` (past tense, no "Request")
- `FileOpenedEvent` vs `FileOpenRequestEvent` (inconsistent: which is the request, which is the notification?)

A clear convention should be established:
- `*RequestEvent` -- imperative action requested by user/system
- `*Event` or `*ChangedEvent` -- notification that something happened

### Issue 3: Service class naming

Extension services use two patterns:
- `FooService` (e.g., `WorkspaceService`, `ProgressService`, `TerminalService`)
- `FooServiceAPI` (e.g., `NotificationServiceAPI`)

The "API" suffix is redundant since all services expose APIs. It was likely added to distinguish from the UI-layer `NotificationManager`, but a clearer approach would be:
- Core layer: `NotificationService` (drop the "API" suffix)
- UI layer: `NotificationManager` (already named correctly)

### Issue 4: Header-only types in wrong files

`MarkdownDocument.cpp` implements types declared in `Types.h` (specifically `MdNode`), not a `MarkdownDocument` class. This makes the relationship between the header and implementation file unclear.

### Issue 5: Registry naming patterns

Two naming patterns exist for registries:
- `*Registry` (e.g., `FeatureRegistry`, `ThemeRegistry`)
- `*ProviderRegistry` (e.g., `TreeDataProviderRegistry`, `FileSystemProviderRegistry`)
- `*Service` (e.g., `OutputChannelService`, `DiagnosticsService`)

The contribution point classes use mixed naming: some are "Service" (OutputChannel, Diagnostics), some are "Registry" (TreeData, FileSystem, Language), and one is just "Service" (Webview, Decoration). A consistent pattern should be applied.

## Detailed Tasks

### Task 1: Rename BuiltinThemes to BuiltInThemes

**Files to modify:**
- Rename `/Users/ryanrentfro/code/markamp/src/core/BuiltinThemes.h` to `BuiltInThemes.h`
- Rename `/Users/ryanrentfro/code/markamp/src/core/BuiltinThemes.cpp` to `BuiltInThemes.cpp`
- Update class name inside both files from `BuiltinThemes` to `BuiltInThemes`
- Update all `#include "BuiltinThemes.h"` references:
  - `/Users/ryanrentfro/code/markamp/src/core/ThemeRegistry.cpp` (line 3)
  - `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp` (line 3)
  - `/Users/ryanrentfro/code/markamp/src/core/BuiltinThemes.cpp` (line 1)
- Update `src/CMakeLists.txt`:
  - Line 84: `core/BuiltinThemes.cpp` -> `core/BuiltInThemes.cpp`
  - Lines 186-187 in source_group: update both `.h` and `.cpp`
- Update `tests/CMakeLists.txt`:
  - Line 13: `${CMAKE_SOURCE_DIR}/src/core/BuiltinThemes.cpp` -> `BuiltInThemes.cpp`

### Task 2: Rename NotificationServiceAPI to NotificationService

**Files to modify:**
- Rename `/Users/ryanrentfro/code/markamp/src/core/NotificationServiceAPI.h` to `NotificationService.h`
- Rename `/Users/ryanrentfro/code/markamp/src/core/NotificationServiceAPI.cpp` to `NotificationService.cpp`
- Update the class name inside both files
- Update all includes and references in:
  - `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`
  - `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` (lines 61 and source_group)
  - `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` (if referenced)
  - Any test files referencing `NotificationServiceAPI`

### Task 3: Document event naming convention

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

Add a comment block at the top of Events.h documenting the naming convention:

```cpp
// Event Naming Convention:
//
// - FooRequestEvent   -- An imperative action requested by user or system.
//                        Published by the initiator, consumed by the handler.
//                        Example: FileOpenRequestEvent, PrintRequestEvent
//
// - FooEvent          -- A notification that something happened.
//                        Published by the handler, consumed by observers.
//                        Example: ThemeChangedEvent, PluginActivatedEvent
//
// - FooChangedEvent   -- Specific form of notification for state changes.
//                        Example: SettingChangedEvent, ExtensionEnablementChangedEvent
```

Do NOT rename existing events in this phase (that would break all subscribers). Instead, document the convention for future events and note the exceptions.

### Task 4: Rename MarkdownDocument.cpp to MdNode.cpp (optional)

This is a low-impact rename that improves discoverability:

- Rename `/Users/ryanrentfro/code/markamp/src/core/MarkdownDocument.cpp` to `/Users/ryanrentfro/code/markamp/src/core/MdNode.cpp`
- Update `src/CMakeLists.txt` (line 93)
- Update `source_group()` section

Alternatively, add a top-of-file comment explaining the relationship:
```cpp
// MdNode and related AST types are declared in Types.h.
// This file provides their method implementations.
```

### Task 5: Fix duplicate step numbering in MarkAmpApp.cpp

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

Lines 118 and 124 both say "// 10.":
```cpp
// 10. Initialize Mermaid renderer     (line 118)
// 10. Publish app ready event          (line 124)
```

Fix to:
```cpp
// 10. Initialize Mermaid renderer     (line 118)
// 11. Publish app ready event          (line 124)
```

## Acceptance Criteria

1. All files use "BuiltIn" (not "Builtin") consistently
2. `NotificationServiceAPI` is renamed to `NotificationService`
3. Events.h has a documented naming convention
4. MarkAmpApp.cpp step numbering is sequential
5. All existing includes and references are updated
6. Full build succeeds; all tests pass

## Testing Requirements

- Full build succeeds (`cmake --build build/debug`)
- All test targets pass (`ctest --output-on-failure`)
- Grep for old names (`BuiltinThemes`, `NotificationServiceAPI`) returns zero results in source files
