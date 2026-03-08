# Phase 07 -- Core User Flow Regression Suite

## Objective
Expand from smoke tests to cross-surface workflows that reflect daily MarkAmp usage.

## Tasks

### Task 19 -- Add File Tree to Editor Navigation Workflow
**Files:** `tests/e2e/appium/src/specs/workflows/filetree_open_and_edit.spec.ts` (new)
**Description:** Verify opening files from file tree, tab activation, editing, and save indicators.
**Acceptance Criteria:**
- Test opens multiple files and validates active tab switching
- Unsaved/save state transitions are asserted
- Workflow passes on clean machine without local preconditions

### Task 20 -- Add Command Palette Workflow Automation
**Files:** `tests/e2e/appium/src/specs/workflows/command_palette_execute.spec.ts` (new)
**Description:** Validate opening command palette, filtering commands, and executing a command that changes UI state.
**Acceptance Criteria:**
- Command palette open (`Cmd+Shift+P`) validated
- Typed query returns expected command item
- Post-command UI state is asserted

### Task 21 -- Add Settings Toggle Round-Trip Workflow
**Files:** `tests/e2e/appium/src/specs/workflows/settings_toggle_roundtrip.spec.ts` (new)
**Description:** Toggle one deterministic setting, verify immediate UI effect, then revert setting and verify rollback.
**Acceptance Criteria:**
- Toggle and rollback both asserted in one test
- Config file shows expected value transitions
- Test leaves no persistent side effects
