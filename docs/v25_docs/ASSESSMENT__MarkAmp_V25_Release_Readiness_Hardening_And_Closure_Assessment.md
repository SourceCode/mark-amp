# MarkAmp V25 Release Readiness Hardening And Closure Assessment

## Purpose

`v25` is the release-readiness, hardening, and closure pass after `v24`.

It is not a fresh discovery wave.
It is not a broad backlog expansion pass.
It converts the strongest remaining repository realities into the next tightly sequenced execution package for pushing MarkAmp toward release-candidate quality.

## Assessment Method

- Re-reviewed repository evidence in `src/`, `tests/`, and visible release-path UI files.
- Re-reviewed prior planning artifacts in `docs/v18_docs` through `docs/v24_docs`.
- Compared the `v24` planning thesis against current repository state and recently added `v24`-era governance code and tests.
- Focused on what still blocks trustworthy end-to-end workflows, subsystem signoff, and release validation.

## Current Repository Reality

The repository is no longer blocked by absence of systems. It is blocked by incomplete production adoption of systems that already exist.

The strongest recurring release-path pattern is:

- helper and audit infrastructure exists,
- subsystem models exist,
- tests exist for the helper layer,
- but the visible shell, persistence, and recovery paths still bypass or outpace those contracts.

## What Appears To Have Advanced Since V24

The repository now contains meaningful completion-governance additions that were not present in earlier waves:

- `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.h`
- `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.h`
- `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.h`
- `/Users/ryanrentfro/code/markamp/src/core/ArtifactLifecycleValidator.h`
- `/Users/ryanrentfro/code/markamp/src/core/DependencyGraphMapper.h`
- `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p01_execution_ledger.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p20_release_signoff.cpp`

This means `v24` should be treated as partially executed, not merely planned.

## What Is Still Blocking Release Candidate Quality

### 1. Artifact Lifecycle Is Still Split Across UI Surfaces

Repository evidence:

- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` still manufactures `Untitled-*` tabs directly.
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still owns a large amount of save, save-as, autosave, dirty-state, restore, and recovery logic.
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` still exposes separate file-creation behavior.
- `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h` proves the migration away from fake untitled paths is still an active seam.

Implication:
MarkAmp still does not have one fully adopted artifact lifecycle for text files, notebooks, and canvases.

### 2. Governance Infrastructure Exists Faster Than Product Adoption

Repository evidence:

- `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.h`

Implication:
The repository can describe dead, stubbed, or placeholder behaviors, but the shell still allows them to remain visible or reachable.

### 3. Multiple Core Services Still Short-Circuit Real Product Value

Repository evidence:

- `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp` still returns empty keyword, phrase, and regex results.
- `/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp` still simulates kernel lifecycle.
- `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp` is still explicitly stubbed for testability and generates fake hashes.
- `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp` still leaves snapshot diffing and archive handling stubbed.
- `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp` still contains XOR placeholder encryption and stub KDF comments.
- `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp` still has placeholder export behavior.

Implication:
MarkAmp remains breadth-rich but depth-incomplete in several release-critical services.

### 4. Notebook And Canvas Are Still Not Fully First-Class Shell Artifacts

Repository evidence:

- `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`

Key problems:

- notebook lifecycle exists, but save, open, restore, kernel, and output trust still lag behind shell-grade completion,
- `CanvasWorkspacePanel` still uses local placeholder UI and constructs a private event bus for its canvas panel,
- shell authority still trails workbench capability.

### 5. Panels, Settings, And Specialized Surfaces Still Over-Promise

Repository evidence:

- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`

Key problems:

- staged settings UX still competes with direct config mutation,
- settings deep links and scope flows are not fully authoritative,
- some visible panels remain thin or stubbed on the release path.

### 6. UI Quality Is Still Coupled To Incomplete Surface Ownership

Repository evidence:

- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` still uses Unicode placeholder icons and placeholder inspector content.
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still contains inline placeholder surface registrations.
- `src/ui` and `src/core` still contain a high concentration of placeholder and stub markers relative to other directories.

Implication:
Final polish is still blocked by unresolved ownership and dead-path cleanup, not only by styling effort.

## Marker Concentration Snapshot

A repository scan of unfinished markers shows the concentration still sits primarily in the release-path directories:

- `src/core`: highest concentration
- `src/ui`: second-highest concentration
- smaller but still meaningful pockets in `src/canvas`, `src/node_editor`, and `src/rendering`

This supports a `v25` plan that prioritizes shell, lifecycle, settings, services, and visible release surfaces over broad subsystem expansion.

## V24 Status Reclassification

### Treat As Materially Advanced

- governance ledger and dependency mapping
- subsystem done-criteria scaffolding
- action-manifest foundation
- lifecycle validation helpers
- smoke-runner and phase-readiness helpers
- v24 unit-test scaffolding across all 20 phases

### Treat As Partial, Not Finished

- artifact lifecycle adoption in the live shell
- release-gate enforcement over actual menus, panels, and settings
- notebook shell promotion
- canvas shell promotion
- validation as a true release blocker rather than support tooling

### Treat As Still Blocking

- direct untitled creation paths
- save/autosave/restore/recovery split ownership
- placeholder core services
- visible thin panels and dead affordances
- cloud/security transport placeholder behavior
- end-to-end regression and recovery proof

## What V25 Changes From V24

`v24` correctly organized the major workstreams, but it still behaved like a broad completion sequence.

`v25` tightens that into a release-oriented operating model:

- it explicitly counts governance infrastructure as partially landed work,
- it shifts focus from building more auditors to forcing shell adoption and gate enforcement,
- it trims non-critical expansion from advanced domains and pushes them behind release gating or de-scope decisions,
- it turns UI polish into a completion consequence of authoritative surface ownership,
- it makes subsystem definitions of done operational and testable.

## Release-Critical User Journeys That Still Need End-To-End Trust

- create new file, edit, autosave, save as, close, reopen, restore after restart
- create new notebook, execute cells, persist outputs, reopen, restore kernel/output state safely
- create new canvas, edit, save, duplicate, rename, reopen, restore shell state
- open workspace, navigate explorer/search/recent items, restore panels and active artifacts
- apply settings, persist, deep-link, workspace-scope override, restart, validate restored behavior
- run search/preview/diagnostics flows from editor and sidebars
- review source control status, diff, history, and repository snapshots without synthetic data paths
- run build/debug/terminal flows without dead buttons or misleading panel affordances
- perform recovery after autosave conflict, external file change, crash, or invalid state

## Subsystem Release-Ready Definitions

### Artifact Lifecycle

Release-ready when all release-path entry points create, save, rename, duplicate, move, delete, restore, and recover artifacts through one authoritative registry and persistence path.

### Shell Actions And Panels

Release-ready when every visible action is manifest-backed, enablement-correct, handler-backed, and every release-path panel has a real factory, restore lifecycle, and non-placeholder content.

### Editor / Notebook / Canvas

Release-ready when each surface is shell-owned, persistence-safe, restore-safe, command-routed, and covered by smoke plus restart validation.

### Settings / Workspace

Release-ready when staging, apply, cancel, deep link, scope, export/import, and restart persistence are coherent and validated.

### Core Services

Release-ready when search, notebook execution, Git, repository history, and cloud/security paths no longer rely on synthetic or placeholder production behavior.

### Validation / Recovery

Release-ready when smoke, integration, restart, recovery, and gate reports can fail the release wave and are tied to visible workflows.

## V25 Operating Principle

The next execution package should prefer:

- blocker closure over subsystem breadth,
- authoritative path adoption over new helper layers,
- testable definitions of done over open-ended quality aspirations,
- real shell behavior over local widget fixes,
- explicit de-scope or gating of non-critical depth over pretending every subsystem must fully mature before release.

## Assessment Conclusion

MarkAmp is now close enough to completeness that the remaining work must be handled as release-hardening, closure, and signoff preparation.

The repository does not need another broad planning pass.
It needs a disciplined execution package that:

- closes split ownership,
- removes remaining synthetic release-path behavior,
- forces audit systems to become product law,
- proves recovery and restart trust,
- and defines exactly what must pass before a release candidate can be called credible.
