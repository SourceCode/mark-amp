# Phase 05: Event Type Deduplication

**Priority:** High
**Estimated Scope:** ~8 files affected
**Dependencies:** Phase 01

## Objective

Remove the 7+ duplicate event types identified in Phase 01 and update all subscribers/publishers to use the canonical name. Also consolidate near-duplicate patterns like `SelectToBracketRequestEvent` vs `SelectToMatchingBracketRequestEvent`.

## Background/Context

Events.h contains multiple events describing identical actions, created during different development rounds (R6-R12). Each round added events without checking for existing equivalents.

## Detailed Tasks

### Task 1: Map all duplicate event pairs and their usage

Search all `.cpp` files for each duplicate pair to determine which name is used in subscribers vs publishers.

**Duplicate pairs (canonical -> remove):**
1. `ToggleMinimapRequestEvent` -> remove `ToggleMinimapR11RequestEvent`
2. `ReverseLinesRequestEvent` -> remove `ReverseSelectedLinesRequestEvent`
3. `TransposeCharsRequestEvent` -> remove `TransposeCharactersRequestEvent`
4. `FoldCurrentRequestEvent` -> remove `FoldCurrentRegionRequestEvent`
5. `UnfoldCurrentRequestEvent` -> remove `UnfoldCurrentRegionRequestEvent`
6. `JumpToBracketRequestEvent` -> remove `JumpToMatchingBracketRequestEvent`
7. `DeleteLineRequestEvent` -> remove `DeleteCurrentLineRequestEvent`
8. `SelectToBracketRequestEvent` -> check vs `SelectToMatchingBracketRequestEvent`

### Task 2: Update all publishers

**Files to modify:**
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` -- menu items that publish events
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` -- context menu items
- `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` -- palette commands

For each duplicate, search for the removed name and replace with canonical name.

### Task 3: Update all subscribers

**Files to modify:**
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` -- event subscriptions (~1500 lines of wiring)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` -- event handlers

### Task 4: Remove dead event types from Events.h

**File:** `/Users/ryanrentfro/code/markamp/src/core/Events.h`

Delete the 7 removed event struct definitions.

### Task 5: Verify no remaining references

Run a project-wide search for each removed event name to ensure zero remaining references.

## Acceptance Criteria

1. Zero duplicate event types in Events.h
2. All menu items, palette commands, and subscriptions use canonical names
3. No compile errors
4. All tests pass

## Testing Requirements

- Existing test suite passes
- Manual verification: each deduplicated action still works (e.g., fold current region, toggle minimap)
