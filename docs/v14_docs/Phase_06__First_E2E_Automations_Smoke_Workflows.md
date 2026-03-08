# Phase 06 -- First E2E Automations: Smoke Workflows

## Objective
Ship the first meaningful Appium `mac2` workflow automations and make them runnable locally.

## Tasks

### Task 16 -- Automate Launch and Shell Readiness Workflow
**Files:** `tests/e2e/appium/src/specs/smoke/launch_and_shell_ready.spec.ts` (new)
**Description:** Verify app launch, core shell controls visibility, and idle readiness state.
**Acceptance Criteria:**
- Test launches MarkAmp from deterministic app artifact
- Asserts Activity Bar, Editor, and Status Bar controls are present via stable selectors
- Captures screenshot + page source on failure

### Task 17 -- Automate Create/Edit/Save Markdown Workflow
**Files:** `tests/e2e/appium/src/specs/smoke/create_edit_save_note.spec.ts` (new)
**Description:** Open seeded workspace file, edit markdown, save, and verify persisted content.
**Acceptance Criteria:**
- Uses keyboard-first path where possible (`Cmd+S`)
- Verifies file content change from filesystem assertion
- Asserts no blocking error dialogs

### Task 18 -- Automate Theme Switch and Persistence Workflow
**Files:** `tests/e2e/appium/src/specs/smoke/theme_switch_persistence.spec.ts` (new)
**Description:** Change theme in settings, restart app session, and verify selected theme persists.
**Acceptance Criteria:**
- Test validates settings navigation and selection behavior
- Theme persistence verified after relaunch
- Test resets modified user config during teardown

## First Automation Command Set
```bash
cd tests/e2e/appium
npm ci
npm run appium:start
npm run e2e:mac -- --suite smoke
```
