# MarkAmp E2E Test Program — Policies

## 28. Test Data Privacy and Secret-Handling Policy

- **No real user data**: Tests must never use actual user files, credentials, or personal information.
- **No secrets in fixtures**: API keys, tokens, and passwords must not appear in fixture files. Use environment variable stubs.
- **Synthetic content only**: All fixture text content must be clearly synthetic (e.g., "Lorem ipsum" or purpose-written test content).
- **No PII**: Test data must not contain names, emails, phone numbers, or addresses that could be mistaken for real PII.
- **Secret masking**: Any test that validates secret display (e.g., API key field) must verify masking behavior, not actual secret values.

---

## 29. Logging and Redaction Policy for Captured Artifacts

- **Screenshots**: Captured on failure. Must not display real user content — tests run against synthetic fixtures only.
- **Page source XML**: May contain accessibility labels. No sensitive content since E2E mode uses synthetic data.
- **App logs**: JSON log capture must not include raw file paths from the user's home directory. The E2E temp directory path is acceptable.
- **Appium server logs**: May be uploaded to CI. Contain only driver protocol traffic — no sensitive app data.
- **Retention**: CI artifacts retained for 7 days (smoke/critical) and 14 days (nightly/RC). Local artifacts are not auto-cleaned.
- **Redaction**: If any artifact inadvertently captures sensitive paths, the CI upload step must redact `$HOME` to `<HOME>`.

---

## 30. Branch Strategy for Test Updates with Feature Rollout

| Scenario                            | Branch Strategy                                                                  |
| ----------------------------------- | -------------------------------------------------------------------------------- |
| New feature + E2E tests             | Feature branch includes both source and test changes. Tests merged with feature. |
| Test improvement (no source change) | Direct PR to `main`, labeled `e2e-only`.                                         |
| Selector rename                     | Coordinated PR: C++ source rename + E2E selector update in the same PR.          |
| Quarantine fix                      | PR to `main` moving test from `quarantine/` back to its original location.       |
| Framework upgrade (WDIO, Appium)    | Dedicated PR with canary test results in description.                            |

---

## 31. Ownership Model by Subsystem

| Subsystem             | Owner             | E2E Spec Directories                         |
| --------------------- | ----------------- | -------------------------------------------- |
| App Shell / Workbench | Core Team         | `smoke/`, `workflows/workbench/`             |
| Editor                | Editor Team       | `workflows/editor/`                          |
| File Explorer         | Core Team         | `workflows/workbench/`                       |
| Search / Navigation   | Core Team         | `workflows/search/`, `workflows/navigation/` |
| Canvas                | Canvas Team       | `workflows/canvas/`                          |
| Extensions            | Platform Team     | `workflows/extensions/`                      |
| AI Assistant          | AI Team           | `workflows/ai/`                              |
| Theme System          | UI Team           | `workflows/theme/`                           |
| Knowledge Graph       | Data Team         | `workflows/knowledge/`                       |
| Notebook              | Data Team         | `workflows/notebook/`                        |
| AV Database           | Data Team         | `workflows/av/`                              |
| FSRS Flashcards       | Data Team         | `workflows/fsrs/`                            |
| Source Control / Git  | Core Team         | `workflows/git/`                             |
| Cloud Sync            | Platform Team     | `workflows/sync/`                            |
| Security / Vault      | Security Team     | `workflows/security/`                        |
| Tasks / Calendar      | Productivity Team | `workflows/tasks/`                           |
| Presentations         | Productivity Team | `workflows/presentation/`                    |
| Import / Export       | Data Team         | `workflows/import_export/`                   |
| Terminal / Debug      | Platform Team     | `workflows/integration/`                     |
| Performance / Chaos   | SRE Team          | `performance/`, `chaos/`, `resilience/`      |
| CI / Infrastructure   | Platform Team     | CI workflows, scripts                        |

---

## 32. Escalation Path for Persistent Flake Clusters

1. **Auto-detection**: Quarantine manager identifies ≥ 3 quarantined tests in the same subsystem.
2. **Notification**: Subsystem owner is notified via issue.
3. **7-day window**: Owner investigates root cause (selector instability, timing, app bug).
4. **If unresolved by day 7**: Escalate to E2E program lead.
5. **If unresolved by day 14**: Escalate to engineering manager. Tests may be permanently removed with tracking issue.

---

## 33. Bug Filing Template

```markdown
## E2E Bug Report

**Test**: `<spec file path>`
**Tier**: <smoke|critical|workflow|resilience>
**Failure Type**: <true regression|flake|infrastructure>

### Environment

- macOS: <version>
- Node.js: <version>
- Appium: <version>
- MarkAmp build: <commit SHA>
- Runner: <local|CI>

### Repro Steps

1. <Step 1>
2. <Step 2>
3. <Step 3>

### Expected Behavior

<What should happen>

### Actual Behavior

<What actually happened>

### Artifacts

- Screenshot: <link>
- Page Source: <link>
- App Logs: <link>
- Video: <link if available>

### Failure Count

- First seen: <date>
- Occurrences: <X of Y runs>
- Quarantined: <yes/no>
```

---

## 34. Definition of Done for Each Phase Deliverable

A phase is considered "done" when:

- [ ] All tasks in the phase document are addressed (implemented, documented, or tagged `@pending-feature`)
- [ ] All new spec files pass TypeScript compilation (`tsc --noEmit`)
- [ ] All new spec files pass linting
- [ ] Page objects for new surfaces are created and use `ma.*` selectors
- [ ] Support utilities follow existing patterns (no `any`/`unknown` types)
- [ ] Selector contract snapshot is updated if new selectors were added
- [ ] Coverage map is updated to reflect new test coverage
- [ ] README or relevant docs are updated
- [ ] PR is reviewed by at least one subsystem owner

---

## 35. Monthly Quality Scorecard

| Metric                   | Formula                                           | Target  |
| ------------------------ | ------------------------------------------------- | ------- |
| Smoke Pass Rate          | (passed ÷ total) × 100                            | 100%    |
| Critical Pass Rate       | (passed ÷ total) × 100                            | ≥ 98%   |
| Flake Rate               | (quarantined ÷ total) × 100                       | ≤ 2%    |
| Coverage Breadth         | (surfaces with ≥ 1 test ÷ total surfaces) × 100   | ≥ 90%   |
| Mean Time to Diagnose    | avg(failure → root cause identification)          | ≤ 4 hrs |
| Defect Escape Rate       | (prod bugs caught by E2E ÷ total prod bugs) × 100 | ≥ 60%   |
| Suite Runtime Compliance | suites within budget ÷ total suites               | 100%    |
| Quarantine Turnover      | resolved quarantines ÷ new quarantines            | ≥ 1.0   |

---

## 36. KPIs

1. **Coverage %**: Percentage of product areas with at least critical-tier E2E coverage.
2. **Defect Escape Rate**: Percentage of production user-facing bugs that were catchable by E2E tests but were not.
3. **Mean Time to Diagnose (MTTD)**: Average time from E2E failure detection to root cause identification.
4. **Flake Rate**: Percentage of test runs that are non-deterministic failures.
5. **Mean Time to Fix Flake (MTTF-Flake)**: Average quarantine duration before resolution.

---

## 37. PR Gate Rules for Smoke and Critical Workflow Suites

- **Smoke suite**: **Blocking**. All smoke tests must pass. No merge without 100% pass rate.
- **Critical suite**: **Blocking**. All critical tests must pass. Waiver required for exceptions (see §27).
- **Workflow suite**: **Informational**. Results posted as PR comment. Not blocking.
- **Required checks**: `e2e-smoke` and `e2e-critical` are registered as required status checks.
- **Bypass**: Only via approved waiver (§27). Force-merge is not permitted.

---

## 38. Nightly Full-Suite Execution Policy

- **Schedule**: Every night at 02:00 UTC via `workflow_dispatch` + `schedule` cron.
- **Scope**: All specs across smoke + critical + workflow + edge case directories.
- **Retry**: 1 retry per failed test.
- **Timeout**: 60-minute overall job timeout.
- **Artifacts**: Full artifact bundle for every failure, plus daily summary report.
- **Notifications**: Summary posted to `#e2e-nightly` channel. Failures trigger subsystem owner notification.

---

## 39. Release-Candidate Certification Checklist

Before a release candidate is promoted to release:

- [ ] All smoke tests pass (100%)
- [ ] All critical tests pass (100%)
- [ ] All workflow tests pass (≥ 98%)
- [ ] All regression tests pass (≥ 95%)
- [ ] All resilience tests pass (≥ 90%)
- [ ] No new quarantined tests since last RC
- [ ] Performance baselines within budget (no regression > 10%)
- [ ] All P0/P1 bugs from E2E are resolved
- [ ] Subsystem owners have signed off
- [ ] Defect burndown shows decreasing trend

---

## 40. Rollback Criteria When E2E Gates Fail Near Release

| Condition                                 | Action                                                                      |
| ----------------------------------------- | --------------------------------------------------------------------------- |
| Smoke suite fails on RC branch            | **Block release**. Revert offending commit or hotfix within 24 hours.       |
| Critical suite fails on RC branch         | **Block release**. Root-cause within 4 hours. Hotfix or revert.             |
| Workflow suite has ≥ 5 new failures on RC | **Review with team**. May block release if failures affect P0/P1 workflows. |
| Resilience suite regression               | **Informational only**. Document in release notes. Do not block release.    |
| Performance regression > 20%              | **Block release**. Investigate before proceeding.                           |
