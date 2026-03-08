# Phase 05 -- WDIO Framework and Test Utilities

## Objective
Stand up a maintainable WebdriverIO (TypeScript) test framework for Appium `mac2`.

## Tasks

### Task 13 -- Scaffold WDIO + TypeScript Runner
**Files:** `tests/e2e/appium/wdio.conf.ts` (new), `tests/e2e/appium/tsconfig.json` (new), `tests/e2e/appium/src/` (new)
**Description:** Configure WDIO to run Appium `mac2` tests with Mocha and typed helpers.
**Acceptance Criteria:**
- `npm run e2e:mac` starts and executes sample spec
- WDIO config supports parallelization toggle and retries
- Logs and screenshots are captured on failures

### Task 14 -- Implement Page Objects for P1 Workflows
**Files:** `tests/e2e/appium/src/pages/AppShellPage.ts` (new), `tests/e2e/appium/src/pages/EditorPage.ts` (new), `tests/e2e/appium/src/pages/SettingsPage.ts` (new)
**Description:** Create page objects for app shell, editor, and settings with selector contract integration.
**Acceptance Criteria:**
- No raw selectors inside test specs
- Page methods include explicit waits and guard assertions
- Methods encapsulate reusable high-value actions

### Task 15 -- Implement Shared Test Utilities and Data Lifecycle
**Files:** `tests/e2e/appium/src/support/session.ts` (new), `tests/e2e/appium/src/support/workspace.ts` (new), `tests/e2e/appium/src/support/assertions.ts` (new)
**Description:** Add robust setup/teardown, fixture copy, and common assertions utilities.
**Acceptance Criteria:**
- Each test runs in isolated workspace temp dir
- Setup/teardown always execute on pass/fail
- Assertion helpers produce clear error output
