# MarkAmp E2E Test Program — Checklists

## 21. Quality Checklist for Adding a New E2E Test

Before submitting a new E2E test, verify all items:

- [ ] Test file follows naming convention: `<surface>_<action>_<qualifier>.spec.ts`
- [ ] Test is in the correct directory for its tier (smoke, workflow, edge_case, resilience, performance, chaos)
- [ ] `describe` block includes tier tag (`@smoke`, `@critical`, `@workflow`, etc.)
- [ ] All element lookups use `ma.*` accessibility selectors via page objects — no raw XPath or fragile predicates
- [ ] No hardcoded delays (`browser.pause()`) — use explicit waits from `waits.ts`
- [ ] Test uses fixture workspace from `fixtures/` — no real user data
- [ ] Workspace is cleaned up in `after` hook via `cleanupWorkspace()`
- [ ] Test includes both positive and negative assertions where applicable
- [ ] Test produces meaningful failure messages (custom `timeoutMsg` on waits)
- [ ] Test runs in ≤ 30s individually (≤ 60s for resilience/chaos)
- [ ] No `any` or `unknown` types — all TypeScript types are explicit
- [ ] Page object methods are used instead of inline selector queries
- [ ] Test is idempotent — running it twice produces the same result
- [ ] Test does not depend on state from another test (no cross-test coupling)
- [ ] PR includes updated coverage map if adding a new surface area

---

## 22. Review Checklist for Selector Additions and Renames

- [ ] Selector follows `ma.<surface>.<control>[.<qualifier>]` convention
- [ ] Surface prefix is registered in `selector_registry.ts`
- [ ] Selector is unique across the entire application (run uniqueness checker)
- [ ] Selector does not contain dynamic content (timestamps, counters, UUIDs, file paths)
- [ ] Corresponding `SetName()` call exists in the C++ source code
- [ ] Selector contract snapshot (`selector_contract.snapshot.json`) is updated
- [ ] Existing tests referencing the old selector (if rename) are updated
- [ ] Page object is updated with the new selector
- [ ] Unit test for selector presence passes

---

## 23. Review Checklist for Wait Logic and Anti-Flake Patterns

- [ ] All waits use `waitForExist`, `waitUntil`, or helpers from `waits.ts` — never `browser.pause()` for synchronization
- [ ] Wait timeouts are explicit and match the timeout policy (not default 0 or unreasonably large)
- [ ] `waitUntil` conditions are pure checks (no side effects)
- [ ] Retry-safe assertions: tests tolerate transient intermediate states
- [ ] No race conditions between action and assertion (appropriate `waitForIdle()` after state-changing operations)
- [ ] Element references are not stale — re-query after page transitions
- [ ] Error messages in `timeoutMsg` identify the spec and the expected condition
- [ ] Tests with inherent timing sensitivity are in `resilience` or `stress` tier with appropriate retry policy

---

## 24. Master Coverage Map

| Product Area       | Smoke | Critical | Workflow | Edge Case | Resilience | Performance |
| ------------------ | ----- | -------- | -------- | --------- | ---------- | ----------- |
| App Shell          | ✓     | ✓        | Phase 03 | ✓         | Phase 09   | Phase 09    |
| File Explorer      | —     | —        | Phase 03 | ✓         | —          | —           |
| Editor             | ✓     | ✓        | Phase 03 | ✓         | Phase 09   | Phase 09    |
| Tabs               | —     | —        | Phase 03 | ✓         | —          | —           |
| Split View         | —     | —        | Phase 03 | —         | —          | —           |
| Command Palette    | —     | ✓        | Phase 04 | —         | —          | Phase 09    |
| Settings           | —     | —        | Phase 04 | —         | —          | Phase 09    |
| Global Search      | —     | —        | Phase 04 | —         | —          | Phase 09    |
| Panel Area         | —     | —        | Phase 04 | —         | —          | Phase 09    |
| Activity Bar       | ✓     | —        | Phase 04 | —         | —          | —           |
| Status Bar         | ✓     | —        | Phase 04 | —         | —          | —           |
| Breadcrumbs        | ✓     | —        | Phase 04 | —         | —          | —           |
| Find/Replace       | —     | —        | Phase 03 | —         | —          | —           |
| Quick Open         | —     | —        | Phase 03 | —         | —          | —           |
| Outline            | —     | —        | Phase 04 | —         | —          | —           |
| Markdown Preview   | ✓     | —        | Phase 03 | —         | —          | Phase 09    |
| Source Control     | —     | —        | Phase 08 | ✓         | —          | Phase 09    |
| Extensions         | —     | —        | Phase 06 | —         | Phase 09   | Phase 09    |
| Theme Gallery      | ✓     | —        | Phase 06 | ✓         | —          | Phase 09    |
| Canvas             | —     | —        | Phase 05 | —         | —          | Phase 09    |
| Knowledge Graph    | —     | —        | Phase 07 | —         | —          | Phase 09    |
| Notebook           | —     | —        | Phase 07 | —         | —          | Phase 09    |
| AV Database        | —     | —        | Phase 07 | —         | —          | —           |
| FSRS Flashcards    | —     | —        | Phase 07 | —         | —          | —           |
| AI Assistant       | —     | —        | Phase 06 | —         | Phase 09   | Phase 09    |
| Cloud Sync         | —     | —        | Phase 08 | —         | Phase 09   | Phase 09    |
| Security/Vault     | —     | —        | Phase 08 | —         | —          | —           |
| Tasks/Calendar     | —     | —        | Phase 08 | —         | —          | —           |
| Presentations      | —     | —        | Phase 08 | —         | —          | —           |
| Import/Export      | —     | —        | Phase 07 | —         | —          | Phase 09    |
| Terminal           | —     | —        | Phase 06 | —         | Phase 09   | —           |
| Debug              | —     | —        | Phase 06 | —         | —          | —           |
| Keyboard Shortcuts | —     | —        | Phase 04 | ✓         | —          | —           |

---

## 25. Integration Test → Missing UI Flow Mapping

| Catch2 Integration Test            | Missing E2E Coverage            | Target Phase |
| ---------------------------------- | ------------------------------- | ------------ |
| `test_e2e.cpp` (basic integration) | Full app lifecycle E2E          | Phase 03     |
| `test_extension_integration`       | Extension install/uninstall E2E | Phase 06     |
| `test_p1_p4_services`              | Service-level UI flows          | Phase 06     |
| `test_build_system`                | Build panel E2E                 | Phase 04     |
| `test_workspace_config`            | Settings persistence E2E        | Phase 04     |
| `test_find_replace_v2`             | Find/replace in-file E2E        | Phase 03     |
| `test_quick_open`                  | Quick open file switch E2E      | Phase 03     |
| `test_outline_panel`               | Outline navigation E2E          | Phase 04     |
| `test_settings_ui_v2`              | Settings UI workflows E2E       | Phase 04     |
| `test_context_menu_v2`             | Context menu E2E                | Phase 04     |
| `test_toolbar_v2`                  | Toolbar actions E2E             | Phase 04     |

---

## 26. Incident Process for Catastrophic E2E Regressions

1. **Detection**: Smoke or critical suite fails on `main` branch.
2. **Notification**: CI bot posts to `#e2e-alerts` channel with affected tests, failure artifacts, and commit SHA.
3. **Triage**: On-call engineer reviews within 4 hours. Classify as:
   - **True regression**: Code change broke a real workflow → file P0/P1 bug.
   - **Infrastructure failure**: Appium/runner/macOS issue → file infra ticket.
   - **Flake**: Known flaky test → quarantine and file fix ticket.
4. **Mitigation**: If true regression on `main`, either revert the commit or hotfix within 1 business day.
5. **Post-mortem**: After resolution, update regression test to cover the specific failure mode.

---

## 27. Waiver Process for Temporary Gate Bypasses

1. **Request**: Engineer opens a "Gate Waiver" issue with:
   - Affected gate (smoke/critical/nightly)
   - Reason for bypass
   - Expected fix date
   - Risk assessment
2. **Approval**: Requires sign-off from subsystem owner AND E2E program lead.
3. **Duration**: Maximum 5 business days. Non-renewable — must fix or remove the test.
4. **Tracking**: Waiver is linked to the PR and visible in the quality scorecard.
5. **Cleanup**: Waiver auto-expires. If test is still failing, it is quarantined and the waiver issue is escalated.
