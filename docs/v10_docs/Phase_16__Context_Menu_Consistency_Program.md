# Phase 16: Context Menu Consistency Program

## Overview
Unify context menus across controls so actions, ordering, naming, and shortcut labels are consistent.

## Prerequisites
- Phase 04
- Phase 10

## Tasks

### Task 1: Define Context Menu Taxonomy
**Files:** `docs/v10_docs/context_menu_taxonomy.md`
**Description:** Standardize grouping and ordering rules for actions.
**Acceptance Criteria:**
- Menu sections map to consistent action categories

### Task 2: Normalize Labels and Shortcut Display
**Files:** `src/ui/*.cpp`
**Description:** Ensure action names and shortcut strings match menu and palette naming.
**Acceptance Criteria:**
- No duplicate verb variants for same action

### Task 3: Add Dynamic Enablement Rules
**Files:** `src/ui/*.cpp`, `src/core/ContextKeyService.cpp`
**Description:** Drive enable/disable from context keys.
**Acceptance Criteria:**
- Invalid actions are disabled, not hidden unless required

### Task 4: Add Discoverable Empty-State Menus
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/TabBar.cpp`
**Description:** Improve empty-area context menus with useful creation/navigation actions.
**Acceptance Criteria:**
- Empty area menus offer relevant quick starts

### Task 5: Add Context Menu Tests
**Files:** `tests/unit/test_context_menu_enablement.cpp`
**Description:** Verify action visibility and enablement per context.
**Acceptance Criteria:**
- Menu contract regression tests in place

## Testing Requirements
- Keyboard context key invocation
- Cross-platform menu label rendering checks

## Phase Completion Criteria
- Context menus are coherent, predictable, and context-correct
