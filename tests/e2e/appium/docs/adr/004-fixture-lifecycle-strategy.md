# ADR-004: Fixture Lifecycle Strategy

## Status

Accepted

## Date

2026-03-06

## Context

E2E tests require deterministic file system state. Tests that modify files (create, rename, delete, save) must operate on isolated copies to prevent cross-test contamination and ensure repeatability.

### Options Considered

1. **Per-suite temp copy** — Copy fixture directory to `$TMPDIR` before each suite. Clean up in `after()`.
2. **Per-test temp copy** — Copy fixture before each test. Higher isolation but slower.
3. **Shared workspace with rollback** — Use a shared workspace and undo changes via git reset.
4. **In-memory filesystem** — Mock filesystem. Not viable for E2E (real app needs real files).

## Decision

Use **per-suite temp copy** as the default, with **per-test temp copy** available for destructive tests.

### Lifecycle

```
before():
  1. Copy fixtures/<name>/ → $TMPDIR/markamp_e2e_<name>_<random>/
  2. Open workspace in MarkAmp via Appium
  3. Wait for workspace to be ready

afterEach():
  1. Close all dialogs (Escape)
  2. Close all tabs (Cmd+Shift+W)

after():
  1. Close workspace
  2. Delete temp directory
```

### Fixture Layout

```
fixtures/
├── workspaces/
│   ├── basic/              # Simple workspace with a few .md files
│   ├── large/              # Large workspace for performance tests
│   ├── nested/             # Deeply nested folder structure
│   └── multi_type/         # Mixed file types (.md, .json, .html, .css)
├── editor/
│   ├── sample.md           # Standard markdown content
│   ├── large_doc.md        # 10K+ line document for large file tests
│   ├── code_blocks.md      # Document with code blocks for folding tests
│   └── tables.md           # Document with tables for editing tests
├── canvas/
│   ├── simple_board.json   # Basic canvas with a few objects
│   ├── complex_board.json  # Canvas with many object types
│   └── template.json       # Board template
└── graph_notebook/
    ├── notes/              # Interlinked notes for graph tests
    ├── notebook.ipynb      # Sample notebook with cells
    └── flashcards.json     # FSRS flashcard deck
```

### API

```typescript
// workspace.ts
prepareWorkspace(fixtureName: string): string     // Returns temp path
cleanupWorkspace(workspacePath: string): void      // Removes temp dir
prepareTestWorkspace(fixtureName: string): string  // Per-test isolation
```

## Consequences

- **Positive**: Complete isolation — no test can corrupt another test's workspace.
- **Positive**: Deterministic — same fixtures, same initial state, every time.
- **Positive**: Simple cleanup — just delete the temp directory.
- **Negative**: Copy overhead (~50–100ms per suite for typical fixtures).
- **Negative**: Large fixture workspaces increase CI disk usage.
- **Mitigation**: Keep fixtures small. Use `large/` workspace only for performance tests.
