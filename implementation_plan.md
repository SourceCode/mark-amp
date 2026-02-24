# Implementation Plan: Phase 06 Tasks 5-10 (Presets, Persistence, and Migration)

## Goal Description

The goal is to continue Phase 06 by implementing Layout Presets (Task 5), State Persistence (Task 6), and migrating the legacy `LayoutManager` to utilize the newly built `WorkbenchShell` (Tasks 7-10).

## User Review Required

Please review the proposed architectural migration. We are gutting major portions of `LayoutManager.cpp` and replacing its bespoke raw layout creation with the structured 5-zone `WorkbenchShell`.

## Proposed Changes

### 1. Model Enhancements

#### [MODIFY] [WorkbenchLayoutModel.h](file:///Users/ryanrentfro/code/markamp/src/ui/layout/WorkbenchLayoutModel.h)

#### [MODIFY] [WorkbenchLayoutModel.cpp](file:///Users/ryanrentfro/code/markamp/src/ui/layout/WorkbenchLayoutModel.cpp)

- Add `struct WorkbenchLayoutPreset` referencing a set of `WorkbenchZoneState` overrides.
- Add serialization and deserialization methods capable of converting the zone `states_` to and from JSON using `nlohmann/json.hpp`.
- Add `apply_preset(const std::string& preset_id)` implementation.

### 2. WorkbenchShell API Enhancements

#### [MODIFY] [WorkbenchShell.h](file:///Users/ryanrentfro/code/markamp/src/ui/layout/WorkbenchShell.h)

#### [MODIFY] [WorkbenchShell.cpp](file:///Users/ryanrentfro/code/markamp/src/ui/layout/WorkbenchShell.cpp)

- Provide a getter: `[[nodiscard]] auto get_zone_panel(WorkbenchZoneId id) const -> wxPanel*;` to allow parent orchestrators to inject components into the managed zones.
- Forward layout save/load requests downstream to the layout model.

### 3. LayoutManager Migration

#### [MODIFY] [LayoutManager.h](file:///Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h)

#### [MODIFY] [LayoutManager.cpp](file:///Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp)

- Remove manual `wxBoxSizer`, `SplitterBar`, `sidebar_panel_`, etc., properties.
- Add `WorkbenchShell* shell_;` as the sole child of `LayoutManager`.
- In `CreateLayout()`, instantiate `WorkbenchShell`.
- Task 8: Instantiate the `ActivityBar` and assign it to `shell_->get_zone_panel(WorkbenchZoneId::kActivityBar)`.
- Task 9: Extract the `explorer_panel_` configuration (with the search bar and `FileTreeCtrl`) and assign it to `shell_->get_zone_panel(WorkbenchZoneId::kPrimarySidebar)`.
- Task 10: Extract the `SplitView` (Editor) initialization and assign it to `shell_->get_zone_panel(WorkbenchZoneId::kEditorArea)`.

### 4. Integration with Config

- Integrate `WorkbenchLayoutModel::load_state_from_json()` into `LayoutManager::RestoreLayoutState()` to persist the zone states automatically across application restarts based on the `Config` registry.

## Verification Plan

### Automated Tests

- Supplement `tests/unit/test_phase06_workbench_zones.cpp` to verify JSON serialization works symmetrically.

### Manual Verification

- Launch the application and ensure the UI hierarchy accurately delegates the content into the new shell architecture.
- Adjust sidebar width, restart the application, and verify the width and visibility state are persistently reconstructed according to the restored JSON.
