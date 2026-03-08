# MarkAmp E2E Test Suite — Appium mac2

End-to-end UI testing for MarkAmp using Appium 2.x with mac2 driver.

## Prerequisites

- **Node.js** ≥ 20.x (see `.nvmrc`)
- **macOS** with Accessibility permissions granted to your terminal
- **MarkAmp** built (debug `.app` bundle)

## Quick Start

```bash
# 1. Bootstrap Appium + mac2 driver
bash scripts/bootstrap_mac2.sh

# 2. Run smoke tests
npm run test:smoke

# 3. Run all tests
npm test
```

## Architecture

```
tests/e2e/appium/
├── config/                       # mac2 driver capabilities
├── docs/                         # Governance, standards, ADRs
│   ├── governance.md             # Mission, scope, severity model
│   ├── standards.md              # Naming, selectors, policies
│   ├── checklists.md             # Review checklists, coverage map
│   ├── policies.md               # Privacy, ownership, CI gates
│   ├── onboarding.md             # Developer & maintainer guides
│   └── adr/                      # Architecture decision records
├── fixtures/                     # Test workspace/fixture data
├── scripts/                      # Setup & CI scripts
├── src/
│   ├── pages/                    # Page objects (accessibility selectors)
│   ├── specs/
│   │   ├── smoke/                # Must-pass launch/save/theme tests
│   │   ├── workflows/            # Core user flow regressions
│   │   │   ├── workbench/        # App shell, file, tab workflows
│   │   │   ├── editor/           # Editor command workflows
│   │   │   ├── search/           # Search & find workflows
│   │   │   ├── navigation/       # Navigation workflows
│   │   │   ├── productivity/     # Shortcuts, settings workflows
│   │   │   ├── canvas/           # Canvas tool workflows
│   │   │   ├── extensions/       # Extension lifecycle workflows
│   │   │   ├── ai/               # AI assistant workflows
│   │   │   ├── theme/            # Theme system workflows
│   │   │   ├── integration/      # Terminal, debug, task runner
│   │   │   ├── knowledge/        # Knowledge graph workflows
│   │   │   ├── notebook/         # Notebook workflows
│   │   │   ├── av/               # AV database workflows
│   │   │   ├── fsrs/             # Flashcard/FSRS workflows
│   │   │   ├── import_export/    # Document pipeline workflows
│   │   │   ├── git/              # Source control workflows
│   │   │   ├── sync/             # Cloud sync workflows
│   │   │   ├── security/         # Vault, privacy workflows
│   │   │   ├── tasks/            # Task/calendar workflows
│   │   │   └── presentation/     # Presentation workflows
│   │   ├── edge_cases/           # Error handling, stress tests
│   │   ├── resilience/           # Chaos and recovery tests
│   │   ├── performance/          # Latency measurements
│   │   └── chaos/                # Chaos simulation tests
│   └── support/                  # Shared utilities
├── artifacts/                    # Runtime failure artifact output
├── reports/                      # Generated coverage/perf reports
├── wdio.conf.ts                  # WDIO config
├── tsconfig.json
└── package.json
```

## Selector Contract

All E2E tests locate elements via **accessibility names** following the `ma.<surface>[.<control>]` convention:

| Selector              | Component         |
| --------------------- | ----------------- |
| `ma.shell.main_frame` | Main window frame |
| `ma.activitybar`      | Activity bar      |
| `ma.editor.panel`     | Editor panel      |
| `ma.filetree.ctrl`    | File tree control |
| `ma.settings.panel`   | Settings panel    |
| `ma.commandpalette`   | Command palette   |
| `ma.statusbar`        | Status bar        |

See [standards.md](docs/standards.md) for the full selector naming convention and registered surface prefixes.

## E2E Mode

Set `MARKAMP_E2E=1` to enable deterministic runtime:

- Isolated config directory (`$TMPDIR/markamp_e2e/`)
- No animations or tips
- Predictable initial state

## Test Tiers

| Tier       | Command                   | Purpose                     |
| ---------- | ------------------------- | --------------------------- |
| Smoke      | `npm run test:smoke`      | Launch, selectors, basic UI |
| Workflows  | `npm run test:workflows`  | Full user flow coverage     |
| Edge Cases | `npm run test:edge-cases` | Error handling, stress      |
| Regression | `npm run test:regression` | Combined workflow + edge    |
| All        | `npm test`                | Everything                  |

## Documentation

- [Governance](docs/governance.md) — Mission, scope, severity model, quality gates
- [Standards](docs/standards.md) — Naming, selectors, policies, timeouts
- [Checklists](docs/checklists.md) — Review checklists, coverage map
- [Policies](docs/policies.md) — Privacy, ownership, CI gates, release certification
- [Onboarding](docs/onboarding.md) — Developer guide, maintainer guide, examples
- [ADR-001: Appium mac2 Stack](docs/adr/001-appium-mac2-stack.md)
- [ADR-002: WDIO + TypeScript Runner](docs/adr/002-wdio-typescript-runner.md)
- [ADR-003: Page Object Standards](docs/adr/003-page-object-standards.md)
- [ADR-004: Fixture Lifecycle Strategy](docs/adr/004-fixture-lifecycle-strategy.md)
- [ADR-005: Artifact Storage Retention](docs/adr/005-artifact-storage-retention.md)
