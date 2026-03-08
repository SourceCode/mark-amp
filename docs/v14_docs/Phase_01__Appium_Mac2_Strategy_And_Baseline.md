# Phase 01 -- Appium mac2 Strategy and Baseline

## Objective
Define a production-ready Appium `mac2` testing strategy for MarkAmp and baseline current E2E gaps.

## Tasks

### Task 01 -- Establish E2E Test Charter
**Files:** `docs/v14_docs/Phase_01__Appium_Mac2_Strategy_And_Baseline.md`
**Description:** Define scope boundaries: Appium `mac2` for desktop UI workflows, Catch2 for non-UI logic/integration, and clear ownership between both layers.
**Acceptance Criteria:**
- E2E ownership model documented
- Test pyramid for MarkAmp documented
- Out-of-scope items documented (pixel-perfect visual diffs, low-level renderer internals)

### Task 02 -- Baseline Existing Test Coverage Against Real User Workflows
**Files:** `tests/integration/test_e2e.cpp`, `docs/v14_docs/Phase_01__Appium_Mac2_Strategy_And_Baseline.md`
**Description:** Map existing integration tests to real workflows and identify automation gaps requiring Appium UI automation.
**Acceptance Criteria:**
- Workflow matrix includes startup, editor, theme, file tree, settings, command palette
- Each workflow marked as `Covered by Catch2`, `Needs Appium`, or `Hybrid`
- Priority-1 UI automation backlog identified

### Task 03 -- Define Definition of Done for Appium Stack
**Files:** `docs/v14_docs/Phase_01__Appium_Mac2_Strategy_And_Baseline.md`
**Description:** Lock rollout quality gates for local dev and CI.
**Acceptance Criteria:**
- DoD includes pass rate threshold, flake threshold, runtime budget, failure artifact requirements
- DoD requires deterministic data setup and teardown
- DoD approved for v14 execution
