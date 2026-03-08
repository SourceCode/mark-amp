# Phase 09 -- CI Integration and Quality Gates

## Objective
Integrate Appium `mac2` E2E tests into CI with enforceable merge gates.

## Tasks

### Task 24 -- Add macOS E2E Workflow in CI
**Files:** `.github/workflows/e2e_mac2.yml` (new), `tests/e2e/appium/scripts/ci_run_mac2.sh` (new)
**Description:** Create CI job for build, Appium server startup, E2E execution, and artifact publishing.
**Acceptance Criteria:**
- CI runs on macOS runner and executes smoke suite at minimum
- Failures upload screenshot/log/source artifacts
- Merge gate enforces smoke pass requirement
