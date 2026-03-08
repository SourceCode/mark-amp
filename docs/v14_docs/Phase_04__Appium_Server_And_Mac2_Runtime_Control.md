# Phase 04 -- Appium Server and mac2 Runtime Control

## Objective
Create a repeatable and version-pinned Appium `mac2` runtime with explicit capabilities.

## Tasks

### Task 10 -- Create Isolated Appium Tooling Workspace
**Files:** `tests/e2e/appium/package.json` (new), `tests/e2e/appium/.nvmrc` (new), `tests/e2e/appium/README.md` (new)
**Description:** Add dedicated Node workspace for E2E tooling with pinned versions.
**Acceptance Criteria:**
- `npm ci` is reproducible
- Appium and related libs are version pinned
- Setup instructions include Node version and macOS prerequisites

### Task 11 -- Pin and Validate mac2 Driver Setup
**Files:** `tests/e2e/appium/scripts/bootstrap_mac2.sh` (new)
**Description:** Automate Appium install and `mac2` driver install with health checks.
**Acceptance Criteria:**
- Script validates `appium driver list --installed`
- Script fails fast on missing prerequisites
- Script prints actionable remediation steps

### Task 12 -- Define Baseline mac2 Capabilities and Session Policy
**Files:** `tests/e2e/appium/config/caps.mac2.json` (new), `tests/e2e/appium/README.md`
**Description:** Define standard capabilities (`automationName=mac2`, app path, timeouts, retries, no-reset policy) and session cleanup rules.
**Acceptance Criteria:**
- Capability file supports local and CI modes
- Timeouts are explicitly documented
- Session teardown removes stale app/process state
