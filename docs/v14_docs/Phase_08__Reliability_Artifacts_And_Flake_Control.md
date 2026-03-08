# Phase 08 -- Reliability, Artifacts, and Flake Control

## Objective
Harden the Appium suite to reduce flakiness and improve diagnostics quality.

## Tasks

### Task 22 -- Implement Failure Artifact Bundle Pipeline
**Files:** `tests/e2e/appium/src/support/artifacts.ts` (new), `tests/e2e/appium/wdio.conf.ts`
**Description:** On any failure, capture screenshot, Appium logs, page source, and copied workspace snapshot.
**Acceptance Criteria:**
- Artifact bundle is generated per failed test
- Artifacts include timestamps and spec/test names
- Bundle paths are printed in failure output

### Task 23 -- Add Flake Mitigation Rules and Stability Checks
**Files:** `tests/e2e/appium/src/support/waits.ts` (new), `tests/e2e/appium/src/support/retries.ts` (new), `docs/v14_docs/Phase_08__Reliability_Artifacts_And_Flake_Control.md`
**Description:** Standardize explicit waits, retry policy, and anti-pattern bans (`pause` abuse, unstable selectors).
**Acceptance Criteria:**
- Max retry policy defined per test tier
- `pause` calls are lint-banned except allowlisted debug helpers
- Stability report shows pass/fail + rerun behavior
