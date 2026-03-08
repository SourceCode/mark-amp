# Phase 03 -- macOS Build, Packaging, and Testability

## Objective
Ensure MarkAmp launches deterministically under Appium `mac2` with predictable app artifacts.

## Tasks

### Task 07 -- Standardize Testable macOS App Artifact
**Files:** `src/CMakeLists.txt`, `packaging/`, `scripts/build_debug.sh`
**Description:** Define consistent debug app output path and launch target for local and CI E2E runs.
**Acceptance Criteria:**
- Debug build generates a deterministic `.app` location
- App launch command documented
- Build script exits non-zero on artifact mismatch

### Task 08 -- Add E2E Runtime Flags for Determinism
**Files:** `src/app/MarkAmpApp.cpp`, `src/ui/MainFrame.cpp`, `src/core/Config.cpp`
**Description:** Add startup flags/env support to disable first-run popups, tips, non-deterministic onboarding, and optional animations for E2E runs.
**Acceptance Criteria:**
- `MARKAMP_E2E=1` produces deterministic startup state
- User settings are isolated from normal dev profile
- E2E mode is observable in logs

### Task 09 -- Prepare Stable Test Fixtures Workspace
**Files:** `tests/e2e/fixtures/workspaces/smoke/` (new), `tests/e2e/fixtures/workspaces/editor/` (new)
**Description:** Create seed workspace files and expected outputs for E2E scenarios.
**Acceptance Criteria:**
- Fixture workspaces are immutable templates
- Test runs copy fixtures into temporary run directories
- Seed data supports startup, edit-save, and theme workflows
