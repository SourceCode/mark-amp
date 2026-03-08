# MarkAmp E2E Test Program — Standards

## 11. Test Naming Conventions

### File Naming

```
<surface>_<action>_<qualifier>.spec.ts
```

| Component | Convention                      | Examples                                      |
| --------- | ------------------------------- | --------------------------------------------- |
| Surface   | Lowercase, matches product area | `editor`, `canvas`, `filetree`, `search`      |
| Action    | Verb-noun pair                  | `open_close`, `create_edit`, `save_lifecycle` |
| Qualifier | Optional scope clarifier        | `from_explorer`, `via_shortcut`, `with_undo`  |

### Test Suite Naming (`describe`)

```typescript
describe('<Surface> — <Workflow Description>', () => { ... });
```

Example: `describe('Editor — Save via keyboard shortcut', () => { ... });`

### Test Case Naming (`it`)

```typescript
it('should <expected behavior> when <trigger>', async () => { ... });
```

Example: `it('should mark tab as dirty when content is modified', async () => { ... });`

### Tagging Convention

Tests are tagged with Mocha `grep`-compatible prefixes in the `describe` block:

- `@smoke` — Smoke tier
- `@critical` — Critical tier
- `@workflow` — Workflow tier
- `@resilience` — Resilience tier
- `@perf` — Performance measurement
- `@chaos` — Chaos simulation
- `@pending-feature` — Feature not yet shipped

---

## 12. Folder Structure

```
tests/e2e/appium/
├── config/
│   ├── caps.mac2.json              # mac2 driver capabilities
│   └── caps.ci.json                # CI-specific capability overrides
├── docs/
│   ├── governance.md               # Mission, scope, severity model
│   ├── standards.md                 # This file
│   ├── checklists.md               # Review checklists
│   ├── policies.md                  # Privacy, branch, ownership
│   ├── onboarding.md               # Developer/maintainer guides
│   └── adr/                         # Architecture decision records
│       ├── 001-appium-mac2-stack.md
│       ├── 002-wdio-typescript-runner.md
│       ├── 003-page-object-standards.md
│       ├── 004-fixture-lifecycle-strategy.md
│       └── 005-artifact-storage-retention.md
├── fixtures/
│   ├── workspaces/                  # Per-test workspace templates
│   ├── editor/                      # Synthetic editor content
│   ├── canvas/                      # Synthetic canvas content
│   ├── graph_notebook/              # Synthetic graph/notebook content
│   └── screenshots/                 # Visual baseline screenshots
├── scripts/
│   ├── bootstrap_mac2.sh            # Setup script
│   ├── ci_run_mac2.sh               # CI runner
│   ├── preflight_check.sh           # Host prerequisite validation
│   └── quarantine_manager.sh        # Auto-quarantine automation
├── src/
│   ├── pages/                       # Page objects (1 per surface)
│   ├── specs/
│   │   ├── smoke/                   # Must-pass launch/selector/e2e-mode tests
│   │   ├── workflows/
│   │   │   ├── workbench/           # App shell, file, tab workflows
│   │   │   ├── editor/              # Editor command workflows
│   │   │   ├── search/              # Search, find/replace workflows
│   │   │   ├── navigation/          # Go-to, breadcrumbs, history workflows
│   │   │   ├── productivity/        # Shortcuts, settings, panel workflows
│   │   │   ├── canvas/              # Canvas tool workflows
│   │   │   ├── extensions/          # Extension lifecycle workflows
│   │   │   ├── ai/                  # AI assistant workflows
│   │   │   ├── theme/               # Theme system workflows
│   │   │   ├── integration/         # Terminal, debug, task runner workflows
│   │   │   ├── knowledge/           # Knowledge graph workflows
│   │   │   ├── notebook/            # Notebook workflows
│   │   │   ├── av/                  # AV database workflows
│   │   │   ├── fsrs/                # Flashcard/FSRS workflows
│   │   │   ├── import_export/       # Document pipeline workflows
│   │   │   ├── git/                 # Source control workflows
│   │   │   ├── sync/                # Cloud sync workflows
│   │   │   ├── security/            # Vault, privacy, sanitizer workflows
│   │   │   ├── tasks/               # Task/calendar/Gantt workflows
│   │   │   └── presentation/        # Presentation workflows
│   │   ├── edge_cases/              # Error handling, stress, boundaries
│   │   ├── resilience/              # Chaos and recovery tests
│   │   ├── performance/             # Latency and resource measurements
│   │   └── chaos/                   # Chaos simulation tests
│   └── support/                     # Shared utilities
│       ├── artifacts.ts
│       ├── assertions.ts
│       ├── chaos.ts
│       ├── deterministic.ts
│       ├── e2e_mode.ts
│       ├── isolation.ts
│       ├── keyboard.ts
│       ├── observability.ts
│       ├── perf.ts
│       ├── preflight.ts
│       ├── resource_monitor.ts
│       ├── retries.ts
│       ├── selector_registry.ts
│       ├── session.ts
│       ├── waits.ts
│       └── workspace.ts
├── artifacts/                       # Runtime failure artifact output
├── reports/                         # Generated coverage/perf reports
├── wdio.conf.ts
├── tsconfig.json
├── package.json
└── README.md
```

---

## 13. Selector Naming Convention

All accessibility identifiers follow the pattern:

```
ma.<surface>[.<control>[.<qualifier>]]
```

| Level                                | Purpose                        | Examples                                                  |
| ------------------------------------ | ------------------------------ | --------------------------------------------------------- |
| `ma.<surface>`                       | Top-level UI region            | `ma.shell`, `ma.editor`, `ma.canvas`                      |
| `ma.<surface>.<control>`             | Specific control within region | `ma.editor.panel`, `ma.activitybar.explorer`              |
| `ma.<surface>.<control>.<qualifier>` | Disambiguate similar controls  | `ma.editor.tab.close_btn`, `ma.search.input.regex_toggle` |

### Registered Surface Prefixes

| Prefix              | Surface               |
| ------------------- | --------------------- |
| `ma.shell`          | Main window frame     |
| `ma.activitybar`    | Activity bar          |
| `ma.editor`         | Editor area           |
| `ma.filetree`       | File explorer tree    |
| `ma.settings`       | Settings panel/dialog |
| `ma.commandpalette` | Command palette       |
| `ma.statusbar`      | Status bar            |
| `ma.panelarea`      | Bottom panel area     |
| `ma.tabbar`         | Tab bar               |
| `ma.breadcrumb`     | Breadcrumb bar        |
| `ma.toolbar`        | Toolbar               |
| `ma.search`         | Global search panel   |
| `ma.findreplace`    | In-file find/replace  |
| `ma.preview`        | Markdown preview      |
| `ma.outline`        | Outline panel         |
| `ma.sourcecontrol`  | Source control panel  |
| `ma.extensions`     | Extension browser     |
| `ma.themegallery`   | Theme gallery         |
| `ma.terminal`       | Terminal panel        |
| `ma.debug`          | Debug panel/toolbar   |
| `ma.canvas`         | Canvas workspace      |
| `ma.graph`          | Knowledge graph       |
| `ma.notebook`       | Notebook              |
| `ma.av`             | AV database           |
| `ma.fsrs`           | Flashcard/FSRS        |
| `ma.ai`             | AI assistant panel    |
| `ma.sync`           | Cloud sync            |
| `ma.vault`          | Security vault        |
| `ma.tasks`          | Task panel            |
| `ma.calendar`       | Calendar view         |
| `ma.gantt`          | Gantt timeline        |
| `ma.presentation`   | Presentation view     |
| `ma.notification`   | Notification center   |
| `ma.welcome`        | Welcome screen        |

### Rules

1. Selectors **must** be static strings — no dynamic content (timestamps, counters, file paths).
2. Selectors **must** be unique across the entire application.
3. Selectors **must** use the registered prefix for their surface.
4. New surfaces **must** register their prefix in `selector_registry.ts` before adding selectors.

---

## 14. Deterministic-Data Policy

- All test data **must** come from fixtures in `fixtures/` — never from the user's real filesystem.
- Fixture content **must** be committed to version control.
- File names in fixtures **must** use ASCII characters only (no Unicode edge cases in standard fixtures).
- Fixture workspaces **must** contain a known, enumerable set of files.
- Tests **must not** depend on modification timestamps or file ordering from the OS.

---

## 15. Test Isolation Policy

| Resource              | Isolation Method                                              |
| --------------------- | ------------------------------------------------------------- |
| Workspace files       | Copy fixture to `$TMPDIR/markamp_e2e_<fixture>/` per suite    |
| User config           | Redirect to `$TMPDIR/markamp_e2e/config/` via `MARKAMP_E2E=1` |
| Cache                 | Redirect to `$TMPDIR/markamp_e2e/cache/`                      |
| Extension storage     | Redirect to `$TMPDIR/markamp_e2e/extensions/`                 |
| Clipboard             | Reset before sensitive tests                                  |
| Environment variables | Set via capability profile, never mutate host env             |

---

## 16. Retry Policy by Suite Tier

| Tier        | Max Retries | Retry Delay | Notes                            |
| ----------- | ----------- | ----------- | -------------------------------- |
| Smoke       | 0           | —           | Must always pass first try       |
| Critical    | 1           | 1000ms      | Single retry for timing issues   |
| Workflow    | 1           | 1000ms      | Single retry for timing issues   |
| Regression  | 1           | 1000ms      | Consistent with workflow         |
| Resilience  | 2           | 2000ms      | Inherently variable              |
| Performance | 0           | —           | Measurements must be single-pass |

---

## 17. Timeout Policy by Interaction Type

| Interaction             | Default Timeout | Max Timeout |
| ----------------------- | --------------- | ----------- |
| Element exist check     | 10s             | 15s         |
| Element disappear check | 5s              | 10s         |
| App launch ready        | 15s             | 30s         |
| Dialog open/close       | 5s              | 10s         |
| File save operation     | 5s              | 10s         |
| Search results populate | 10s             | 20s         |
| Canvas render           | 10s             | 20s         |
| Network/sync operation  | 15s             | 30s         |
| Export operation        | 15s             | 30s         |

---

## 18. Startup/Shutdown Policy

### Startup

1. Set `MARKAMP_E2E=1` environment variable.
2. Run preflight checks (accessibility permissions, host tools).
3. Start Appium server on port 4723.
4. Wait for Appium health endpoint.
5. Create session with mac2 capabilities.
6. Wait for `mainWindow` to exist (≤ 30s timeout).
7. Dismiss any first-run dialogs.

### Shutdown

1. Close all open dialogs via Escape.
2. Capture final page source for debugging.
3. Delete Appium session.
4. Kill any stale MarkAmp processes.
5. Clean up temp workspace directories.

---

## 19. Environment Matrix

| Variable                      | Local Default | CI Default           |
| ----------------------------- | ------------- | -------------------- |
| `MARKAMP_E2E`                 | `1`           | `1`                  |
| `APPIUM_HOST`                 | `127.0.0.1`   | `127.0.0.1`          |
| `APPIUM_PORT`                 | `4723`        | `4723`               |
| `MARKAMP_BUILD_TYPE`          | `Debug`       | `Debug`              |
| `E2E_TIMEOUT_MULTIPLIER`      | `1`           | `1.5`                |
| `E2E_ARTIFACT_RETENTION_DAYS` | `—`           | `7`                  |
| Node.js                       | ≥ 20          | ≥ 20 (from `.nvmrc`) |
| macOS                         | ≥ 14.x        | macos-14 runner      |

---

## 20. macOS Host Prerequisites Checklist

- [ ] macOS ≥ 14.0 (Sonoma or later)
- [ ] Xcode Command Line Tools installed
- [ ] Node.js ≥ 20.x installed
- [ ] Appium 2.x installed globally
- [ ] `appium-mac2-driver` installed via `appium driver install mac2`
- [ ] Terminal app has Accessibility permission (System Settings → Privacy & Security → Accessibility)
- [ ] MarkAmp debug build available at expected path
- [ ] At least 4 GB free RAM
- [ ] Screen resolution ≥ 1440×900
