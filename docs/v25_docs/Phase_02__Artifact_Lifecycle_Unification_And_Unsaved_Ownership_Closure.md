# Phase 02: Artifact Lifecycle Unification And Unsaved Ownership Closure

## Phase Intent

Finish the migration from pseudo-file and surface-local creation paths to one authoritative artifact lifecycle.

## Release-Ready Exit Criteria

- no release-path surface creates fake untitled files directly,
- artifact metadata, state transitions, and unsaved ownership are canonical,
- text, notebook, and canvas creation all route through the same shell-owned lifecycle spine.

## Task Count

3

## Task P02-T01

- Phase ID: P02
- Task ID: P02-T01
- Task Title: Remove direct untitled creation from tabs, menus, and tree surfaces
- Priority: P0
- Category: Release Blocker
- Objective: eliminate direct pseudo-file creation and require all new artifact creation to go through `ArtifactCreationService` and `ArtifactRegistry`.
- Why This Matters Now: this remains the most visible release-path integrity gap and is still explicitly present in the UI layer.
- Release Gap Statement: different shell surfaces still create artifacts in incompatible ways.
- User / Product Impact: new file behavior remains inconsistent, leading to save, restore, and recovery mistrust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.h`
- Prior Plan References: `v20 Phase 01`; `v20 Phase 02`; `v24 Phase 02`
- Scope: file new actions; tab new actions; file-tree new file actions; notebook new; canvas new entry points.
- Out of Scope: release-deferred domain-specific creation wizards.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: artifact registry; creation service; shell commands; workspace tree
- Current Behavior: `TabBar` still manufactures `Untitled-*` tabs directly and tree/menu paths still diverge.
- Intended Release-Ready Behavior: all artifact creation produces registry-backed records with canonical metadata and unsaved ownership.
- Missing Pieces: full adoption of creation service; removal of fallback UI-local creation logic; release-path tests.
- Technical Approach: route every create entry point through a shell-level creation controller backed by `ArtifactCreationService`.
- Implementation Steps:
1. Inventory and replace every release-path create entry point.
2. Remove UI-local untitled naming and direct file writes where inappropriate.
3. Add integration tests proving identical creation semantics across surfaces.
- Validation Steps:
1. Create text, notebook, and canvas artifacts from menu, tab, palette, and tree paths.
2. Verify registry state, display names, source metadata, and unsaved status.
- Acceptance Criteria: no release-path UI creates a pseudo-path artifact directly.
- Dependencies: P01-T01
- Parallelization Notes: foundational for notebook and canvas hardening; should not be deferred.
- Risks / Failure Modes: hidden create paths left behind; tree-specific flows regress.
- Release Notes / Cleanup Notes: retire old untitled-path compatibility code after migration.
- Observability / Diagnostics Notes: log artifact creation source and resulting registry ID.
- Rollback / Safety Notes: keep temporary adapters only long enough to preserve user workflows during migration.
- References / Context: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_02__Artifact_Lifecycle_And_Unsaved_Document_Spine.md`

## Task P02-T02

- Phase ID: P02
- Task ID: P02-T02
- Task Title: Finish unsaved buffer adoption and artifact state transition correctness
- Priority: P0
- Category: Artifact Lifecycle
- Objective: make `UnsavedDocumentBufferManager`, registry states, and save promotion the canonical unsaved-document behavior.
- Why This Matters Now: the infrastructure exists but the migration remains incomplete.
- Release Gap Statement: unsaved artifacts are still split between old pseudo-path assumptions and newer registry/buffer models.
- User / Product Impact: dirty state, close confirmation, autosave, and recovery remain less trustworthy than they need to be.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactLifecycleValidator.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p02_artifact_lifecycle.cpp`
- Prior Plan References: `v20 Phase 02`; `v23 Phase 03`; `v24 Phase 02`
- Scope: text artifact unsaved state, save promotion, delete/discard, dirty tracking, duplicate path prevention.
- Out of Scope: binary artifact editing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactLifecycleValidator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: unsaved buffers; lifecycle validator; save flow; close flow
- Current Behavior: helper types exist, but live UI and persistence flows still carry old assumptions.
- Intended Release-Ready Behavior: unsaved records have no file path, saved records have one path, and all transitions are valid and observable.
- Missing Pieces: wider shell adoption; state transition enforcement; buffer cleanup on save/discard; lifecycle smoke.
- Technical Approach: centralize state mutations through lifecycle APIs and use validator output as a gate.
- Implementation Steps:
1. Replace remaining path-based unsaved checks with artifact-state checks.
2. Ensure save and save-as promote state correctly and remove unsaved buffers.
3. Add validator checks to smoke and signoff paths.
- Validation Steps:
1. Cover unsaved create, edit, save, save as, discard, duplicate, and reopen.
2. Assert validator reports clean state after each scenario.
- Acceptance Criteria: artifact lifecycle validator passes cleanly on all release-path smoke scenarios.
- Dependencies: P02-T01
- Parallelization Notes: can run alongside persistence work once create-entry routing is known.
- Risks / Failure Modes: stale buffer cleanup; duplicate path bugs; invalid state transitions.
- Observability / Diagnostics Notes: emit state transition logs with artifact ID and source.
- Rollback / Safety Notes: preserve compatibility shims only for reading legacy session state, not for new writes.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/ArtifactLifecycleValidator.h`

## Task P02-T03

- Phase ID: P02
- Task ID: P02-T03
- Task Title: Make notebook and canvas creation first-class participants in artifact lifecycle law
- Priority: P0
- Category: Core Workflow Hardening
- Objective: enforce the same creation, activation, naming, and save-target rules for notebooks and canvases as for text artifacts.
- Why This Matters Now: notebooks and canvases cannot remain partial exceptions if the product is approaching release.
- Release Gap Statement: notebook and canvas creation are still split between shell placeholders and deeper model code.
- User / Product Impact: users cannot trust that non-text artifacts behave like real first-class documents.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Prior Plan References: `v20 Phase 03`; `v20 Phase 04`; `v24 Phase 08`; `v24 Phase 09`
- Scope: notebook creation; board creation; active artifact ownership; save request routing.
- Out of Scope: advanced notebook and canvas feature depth unrelated to lifecycle trust.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: notebook shell; canvas shell; artifact creation; active surface routing
- Current Behavior: lifecycle classes exist, but shell entry points and active artifact assumptions still lag.
- Intended Release-Ready Behavior: notebook and canvas creation are indistinguishable from text creation in shell correctness and persistence expectations.
- Missing Pieces: create-entry routing; active artifact synchronization; save-target policy; restore compatibility.
- Technical Approach: unify notebook and canvas shell creation through the same creation controller and registry semantics used by text files.
- Implementation Steps:
1. Route notebook and board creation into the artifact registry and shell activation path.
2. Remove private placeholder creation behavior from shell panels.
3. Add cross-surface artifact activation and save tests.
- Validation Steps:
1. Create notebook and canvas from every visible release-path entry point.
2. Verify active artifact, display name, dirty state, and save prompts behave consistently.
- Acceptance Criteria: notebooks and canvases no longer rely on shell-local placeholder creation behavior.
- Dependencies: P02-T01; P02-T02
- Parallelization Notes: notebook and canvas deeper work can start once this contract is fixed.
- Risks / Failure Modes: shell activation drift; inconsistent save prompts; orphaned artifacts.
- Release Notes / Cleanup Notes: retire release-path exceptions that special-case notebook or board creation.
- Observability / Diagnostics Notes: track artifact kind, source, and active-surface ownership.
- Rollback / Safety Notes: keep import/open compatibility separate from new artifact creation.
- References / Context: `docs/v20_docs/ASSESSMENT__MarkAmp_V20_Master_Assessment.md`
