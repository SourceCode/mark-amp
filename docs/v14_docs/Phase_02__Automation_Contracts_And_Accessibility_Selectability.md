# Phase 02 -- Automation Contracts and Accessibility Selectability

## Objective
Make MarkAmp UI reliably discoverable and stable for Appium `mac2` by formalizing automation selectors.

## Tasks

### Task 04 -- Create Selector Contract for All P1 Surfaces
**Files:** `docs/v14_docs/Phase_02__Automation_Contracts_And_Accessibility_Selectability.md`
**Description:** Define naming convention for stable selectors using `SetName`/accessible names (`ma.<surface>.<control>`).
**Acceptance Criteria:**
- Contract includes rules for uniqueness and versioning
- Dynamic labels are banned as selector sources
- Selector review checklist documented

### Task 05 -- Add Automation-Grade Names to Core UI Controls
**Files:** `src/ui/MainFrame.cpp`, `src/ui/EditorPanel.cpp`, `src/ui/ActivityBar.cpp`, `src/ui/FileTreeCtrl.cpp`, `src/ui/SettingsDialog.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Add or normalize control names for startup, workspace navigation, editor operations, and settings workflows.
**Acceptance Criteria:**
- P1 controls have stable names
- Names are discoverable through macOS accessibility tree
- Existing accessibility behavior is not regressed

### Task 06 -- Add Selector Audit Test Coverage
**Files:** `tests/unit/test_phase14_appium_selector_contract.cpp` (new), `tests/CMakeLists.txt`
**Description:** Add unit coverage that validates selector uniqueness/presence for key surfaces.
**Acceptance Criteria:**
- Test fails when required selectors are missing or duplicated
- Test integrated into CTest
- New selector list remains under version control
