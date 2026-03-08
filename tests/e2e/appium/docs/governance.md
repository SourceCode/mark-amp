# MarkAmp E2E Test Program — Governance

## 1. Mission, Scope, and Non-Goals

### Mission

Provide automated, repeatable validation of every user-facing workflow in MarkAmp through Appium `mac2` E2E tests, catching regressions before they reach users.

### Scope

- All interactive UI surfaces: app shell, editor, file tree, tabs, panels, dialogs, canvas, notebooks, AV database, graph view, extensions, AI assistant, theme system, settings, source control, sync, tasks, presentations
- macOS desktop application via Appium mac2 driver
- Smoke, critical workflow, regression, and resilience test tiers

### Non-Goals

- Mobile or web platform testing (macOS-only)
- Performance benchmarking of the C++ rendering engine (covered by `benchmarks/`)
- Unit-level logic testing (covered by Catch2 unit tests in `tests/unit/`)
- Third-party service integration testing (API mocks only)

---

## 2. Appium-vs-Catch2 Test Boundary

| Layer                                   | Owner                 | Framework              | Location                |
| --------------------------------------- | --------------------- | ---------------------- | ----------------------- |
| Business logic, data models, parsers    | C++ unit tests        | Catch2                 | `tests/unit/`           |
| Service integration, event bus, IPC     | C++ integration tests | Catch2                 | `tests/integration/`    |
| UI rendering, layout, control behavior  | C++ component tests   | Catch2 + wxTest        | `tests/unit/test_*`     |
| **User workflows, cross-surface flows** | **E2E tests**         | **Appium mac2 + WDIO** | **`tests/e2e/appium/`** |
| Accessibility compliance                | E2E + unit tests      | Both                   | Both locations          |

**Rule**: If a test requires clicking, typing, or navigating between UI surfaces, it belongs in E2E. If it tests a function's return value, it belongs in Catch2.

---

## 3. Application Workflow Inventory

### Core Surfaces

| #   | Surface            | Key Workflows                                                 |
| --- | ------------------ | ------------------------------------------------------------- |
| 1   | App Shell          | Launch, shutdown, window management, menu bar                 |
| 2   | Welcome Screen     | Recent files, open folder, new file, getting started          |
| 3   | File Explorer      | Browse, create, rename, delete, drag-drop, context menu       |
| 4   | Editor             | Open, edit, save, undo/redo, find/replace, folding, word wrap |
| 5   | Tabs               | Open, close, switch, reorder, close-all, reopen-closed        |
| 6   | Split View         | Create, navigate, sync, resize                                |
| 7   | Command Palette    | Open, filter, execute, dismiss                                |
| 8   | Settings           | Categories, toggle, persistence, search                       |
| 9   | Search             | Global search, regex, case, whole-word, replace               |
| 10  | Panel Area         | Output, Problems, Debug Console, Terminal                     |
| 11  | Activity Bar       | Section switch, badges, context menu                          |
| 12  | Status Bar         | Mode, encoding, line/col, branch display                      |
| 13  | Breadcrumbs        | Heading navigation, dropdown                                  |
| 14  | Find/Replace       | In-file find, replace, replace-all                            |
| 15  | Quick Open         | File switch, recent files                                     |
| 16  | Outline            | Symbol outline, navigation                                    |
| 17  | Markdown Preview   | Toggle, sync scroll, render                                   |
| 18  | Source Control     | Stage, unstage, commit, branch, diff, blame                   |
| 19  | Extensions         | Browse, install, uninstall, enable/disable                    |
| 20  | Theme Gallery      | Browse, switch, persist, import, export                       |
| 21  | Canvas             | Board CRUD, object tools, styling, export                     |
| 22  | Knowledge Graph    | View, filter, navigate, backlinks                             |
| 23  | Notebook           | Cell CRUD, execute, export                                    |
| 24  | AV Database        | Table/gallery/kanban views, columns, formulas                 |
| 25  | FSRS Flashcards    | Extraction, review, scheduling                                |
| 26  | AI Assistant       | Panel, actions (summarize, translate, etc.), providers        |
| 27  | Cloud Sync         | Config, manual sync, conflict resolution                      |
| 28  | Security/Vault     | Encryption, vault lock/unlock, privacy mode                   |
| 29  | Tasks/Calendar     | Create, recurrence, kanban, calendar, Gantt                   |
| 30  | Presentations      | Create, navigate, theme, export                               |
| 31  | Import/Export      | Markdown, HTML, pandoc, PDF, batch export                     |
| 32  | Keyboard Shortcuts | Overlay, rebind, conflict, reset                              |
| 33  | Terminal           | Open, execute, clear                                          |
| 34  | Debug              | Start, toolbar, console                                       |

---

## 4. Workflow Risk Ranking

| Risk Tier         | Criteria                     | Example Workflows                                |
| ----------------- | ---------------------------- | ------------------------------------------------ |
| **P0 — Critical** | Data loss, crash, corruption | File save, undo/redo, crash recovery, vault lock |
| **P1 — High**     | Core workflow blocked        | Open file, edit, tabs, search, command palette   |
| **P2 — Medium**   | Feature degraded             | Canvas export, theme switch, extension install   |
| **P3 — Low**      | Cosmetic, minor UX           | Badge count, tooltip text, animation timing      |

---

## 5. Severity Model for E2E Defects

| Severity | Definition                                                  | Triage SLA                    | Resolution SLA             |
| -------- | ----------------------------------------------------------- | ----------------------------- | -------------------------- |
| **P0**   | Data loss, crash, security breach, complete feature failure | Triage within 4 hours         | Fix within 1 business day  |
| **P1**   | Core workflow blocked, no workaround                        | Triage within 8 hours         | Fix within 3 business days |
| **P2**   | Feature degraded, workaround exists                         | Triage within 2 business days | Fix within 1 sprint        |
| **P3**   | Cosmetic issue, minor inconvenience                         | Triage within 1 week          | Fix within 2 sprints       |

---

## 6. Test-Tier Taxonomy

| Tier           | Purpose                                              | Trigger      | Max Runtime | Retry Policy | Gate                                   |
| -------------- | ---------------------------------------------------- | ------------ | ----------- | ------------ | -------------------------------------- |
| **Smoke**      | App launches, basic accessibility, selector contract | Every PR     | 2 min       | 0 retries    | Blocking                               |
| **Critical**   | Must-pass daily workflows (save, edit, search, tabs) | Every PR     | 5 min       | 1 retry      | Blocking                               |
| **Workflow**   | Full user journey coverage by surface                | Nightly      | 20 min      | 1 retry      | Informational                          |
| **Regression** | All workflow + edge case specs                       | Nightly + RC | 45 min      | 1 retry      | Informational (nightly), Blocking (RC) |
| **Resilience** | Chaos simulation, error recovery, performance        | Weekly + RC  | 30 min      | 2 retries    | Informational                          |

---

## 7. Suite Runtime Budgets

| Environment             | Smoke   | Critical | Workflow | Regression | Resilience |
| ----------------------- | ------- | -------- | -------- | ---------- | ---------- |
| **Local (dev)**         | ≤ 2 min | ≤ 5 min  | ≤ 20 min | ≤ 45 min   | ≤ 30 min   |
| **CI (GitHub Actions)** | ≤ 3 min | ≤ 8 min  | ≤ 30 min | ≤ 60 min   | ≤ 45 min   |

---

## 8. Flake Thresholds and Auto-Quarantine Rules

- **Flake rate threshold**: A test is considered flaky if it fails in ≥ 3 of the last 20 runs without a corresponding code change.
- **Auto-quarantine**: After 5 flaky failures in a 7-day window, the test is automatically moved to the `quarantine/` directory and tagged `@quarantined`.
- **Quarantine duration**: Maximum 14 days. If not fixed within 14 days, the test must be either fixed or permanently removed with a tracking issue.
- **Quarantine owner**: The subsystem owner listed in the ownership model is responsible for quarantined tests.

---

## 9. Baseline Pass-Rate and Quality Gate Metrics

| Metric               | Target | Minimum         |
| -------------------- | ------ | --------------- |
| Smoke pass rate      | 100%   | 100% (blocking) |
| Critical pass rate   | 100%   | 98% (blocking)  |
| Workflow pass rate   | ≥ 98%  | 95%             |
| Regression pass rate | ≥ 95%  | 90%             |
| Resilience pass rate | ≥ 90%  | 85%             |
| Overall flake rate   | ≤ 2%   | ≤ 5%            |

---

## 10. Required Failure Artifacts

Every failing test **must** produce the following artifacts:

| Artifact           | Format | Purpose                                |
| ------------------ | ------ | -------------------------------------- |
| Screenshot         | PNG    | Visual state at failure point          |
| Page Source        | XML    | Full accessibility tree snapshot       |
| App Logs           | JSON   | Application log lines from test window |
| Appium Server Logs | TXT    | Driver-level communication logs        |
| Workspace Snapshot | TAR.GZ | Temp workspace state for repro         |
| Test Timeline      | JSON   | Step-by-step timing with durations     |

Artifacts are stored in `tests/e2e/appium/artifacts/<test_name>_<timestamp>/` and uploaded to CI artifact storage with 7-day retention.
