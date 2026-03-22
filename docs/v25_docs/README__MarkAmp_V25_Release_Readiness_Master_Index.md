# MarkAmp V25 Release Readiness Master Index

## Purpose

`v25` is the release-readiness, hardening, and closure pass.

It is the next execution package after `v24`.
It is not a fresh backlog wave.
It is not a duplicate of `v24`.
It is the operational plan for closing the remaining high-value gaps between a broad, partially completed IDE and a release-candidate-quality product.

## What Changed From Prior Planning Generations

- `v18` and `v19` identified integration and shell-cohesion failures.
- `v20` elevated artifact lifecycle and creation failures into hard blockers.
- `v21` elevated command, panel, and settings trust into hard blockers.
- `v22` elevated UI-system inconsistency into a product-quality blocker.
- `v23` proved unfinished work is explicit in the codebase itself.
- `v24` merged those findings into one integrated completion sequence and began introducing governance helpers and tests.
- `v25` treats the new governance layer as partially landed and shifts the program toward production adoption, subsystem signoff, release gating, and blocker retirement.

## What Was Merged, Retired, De-Scoped, Or Re-Sequenced From V24

### Merged

- editor search, preview, and diagnostics hardening now operate as one trustworthiness stream instead of separate polish tracks,
- notebook lifecycle and notebook runtime are treated as one release path,
- canvas shell, canvas persistence, and canvas interaction hardening are treated as one release path,
- recovery, observability, smoke, and restart validation are grouped into a single release-gate stream.

### Retired

- broad UI-only polish backlog items that are not on the release path,
- repeated backlog language about discovering split ownership that is already proven by the repository and earlier plans,
- repeated inventory-only placeholder audits that do not change execution order.

### De-Scoped Or Gated

- non-critical advanced-domain depth that does not materially affect the release candidate,
- broad aesthetic upgrades that are not attached to authoritative release surfaces,
- feature-expansion work that is not required for trustworthy shell, persistence, recovery, or subsystem signoff.

### Re-Sequenced

- governance and definition-of-done work comes first, but only as shell-enforcement groundwork,
- artifact, persistence, command, panel, and settings closure remain the core critical path,
- notebook and canvas move earlier as release-path trust tracks once artifact and shell contracts stabilize,
- advanced domains shift later and are triaged by release necessity rather than by subsystem ambition.

## Highest-Priority Next Implementation Moves

1. Finish shell adoption of the artifact lifecycle so every create/open/save/recover path uses one authoritative model.
2. Make the action manifest, panel readiness rules, and done-criteria systems enforce release-path behavior rather than merely report on it.
3. Replace synthetic implementations in search, notebook execution, Git, repository diff/history, settings export, and cloud security.
4. Promote notebook and canvas to fully trustworthy shell artifacts with restore, recovery, persistence, and command integrity.
5. Convert validation, restart, and recovery coverage into real release blockers.

## Critical Path Dependencies

- `Phase 01` defines release law, done criteria, and evidence ownership.
- `Phase 02` through `Phase 05` establish the release spine: artifacts, persistence, shell authority, panels, and settings host behavior.
- `Phase 06` through `Phase 10` finish the highest-risk user workflows that depend on that spine.
- `Phase 11` through `Phase 16` close the remaining release-path services and specialized surfaces.
- `Phase 17` through `Phase 20` triage non-core depth, retire dead paths, enforce validation, and execute final signoff.

## Parallelizable Workstreams

- notebook and canvas can run in parallel after artifact and shell adoption settle,
- source control and build/terminal can run in parallel after workspace and panel contracts stabilize,
- settings and plugin/extension completion can run in parallel once command and panel authority are enforced,
- rendering/import/export and cloud/security can run in parallel after persistence and diagnostics expectations stop changing,
- validation work should begin early, but its strict gating role lands in the final third of the wave.

## Remaining Release Blockers

- split artifact lifecycle ownership across `TabBar`, `LayoutManager`, and tree/shell paths,
- incomplete save/autosave/restore/recovery convergence,
- synthetic search, kernel, Git, repository, and cloud-security behavior,
- visible placeholder or stubbed panels on the release path,
- notebook and canvas still trailing first-class shell ownership,
- insufficient restart/recovery/regression proof for release-grade trust.

## Highest-Risk Workflow Gaps

- new/open/save/save as/rename/move/delete/duplicate still diverge by entry point,
- notebook execution/output persistence still lacks dependable end-to-end trust,
- canvas shell behavior still lags behind canvas model capability,
- settings apply/cancel/scope/export/deep-link flows remain inconsistent,
- explorer/search/recent-items/panel-restore continuity still needs release-grade validation.

## Highest-Risk Technical Debt Still On The Release Path

- local widget callbacks bypassing manifest-backed command routing,
- placeholder shell surfaces that imply depth not yet delivered,
- governance helpers not yet wired into actual release blocking,
- duplicate or transitional lifecycle paths left alive during migration,
- local styling and placeholder content still attached to visible shell surfaces.

## Highest-Risk Regression And Validation Gaps

- missing end-to-end artifact lifecycle smoke across text, notebook, and canvas,
- incomplete restart and recovery verification,
- tests that validate helper layers without proving real product adoption,
- insufficient release-gate enforcement over dead actions, placeholder panels, and synthetic services.

## Overall Acceptance Criteria For The V25 Wave

- one authoritative artifact lifecycle governs text, notebook, and canvas release paths,
- visible actions, menus, toolbars, context menus, and panel toggles are manifest-backed and release-gated,
- notebook and canvas behave as first-class shell artifacts with save, restore, recovery, and command integrity,
- release-path services no longer rely on synthetic search, kernel, Git, repository diff, or cloud-security behavior,
- release-path panels and settings flows no longer expose dead, misleading, or placeholder behavior,
- smoke, recovery, restart, and regression gates can block the release wave,
- each major subsystem has explicit release-ready signoff criteria and evidence expectations.

## Task Counts

| Phase | Task Count |
| --- | ---: |
| Phase 01 | 3 |
| Phase 02 | 3 |
| Phase 03 | 3 |
| Phase 04 | 3 |
| Phase 05 | 3 |
| Phase 06 | 3 |
| Phase 07 | 3 |
| Phase 08 | 3 |
| Phase 09 | 3 |
| Phase 10 | 3 |
| Phase 11 | 3 |
| Phase 12 | 3 |
| Phase 13 | 3 |
| Phase 14 | 3 |
| Phase 15 | 3 |
| Phase 16 | 3 |
| Phase 17 | 3 |
| Phase 18 | 3 |
| Phase 19 | 3 |
| Phase 20 | 3 |
| Total | 60 |

## Phase Summaries

1. [Phase_01__Release_Spine_Done_Criteria_And_Gate_Enforcement.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_01__Release_Spine_Done_Criteria_And_Gate_Enforcement.md): turn v24 governance helpers into release law.
2. [Phase_02__Artifact_Lifecycle_Unification_And_Unsaved_Ownership_Closure.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_02__Artifact_Lifecycle_Unification_And_Unsaved_Ownership_Closure.md): eliminate split create/open/save ownership.
3. [Phase_03__Persistence_Autosave_Restore_Recovery_And_Session_Trust.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_03__Persistence_Autosave_Restore_Recovery_And_Session_Trust.md): harden restart, autosave, external change, and restore paths.
4. [Phase_04__Shell_Action_Menu_Toolbar_And_Context_Authority.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_04__Shell_Action_Menu_Toolbar_And_Context_Authority.md): make manifest-backed action routing authoritative.
5. [Phase_05__Panels_Explorer_And_Settings_Host_Release_Closure.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_05__Panels_Explorer_And_Settings_Host_Release_Closure.md): remove placeholder panel debt from the release path.
6. [Phase_06__Editor_Search_Preview_And_Diagnostics_Trustworthiness.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_06__Editor_Search_Preview_And_Diagnostics_Trustworthiness.md): finish real editor-side search and diagnostics behavior.
7. [Phase_07__Syntax_Theme_Icon_And_Visible_UI_Closure.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_07__Syntax_Theme_Icon_And_Visible_UI_Closure.md): close visual inconsistencies only after authoritative surfaces are stable.
8. [Phase_08__Notebook_Lifecycle_Runtime_And_Output_Integrity.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_08__Notebook_Lifecycle_Runtime_And_Output_Integrity.md): promote notebooks to trustworthy first-class artifacts.
9. [Phase_09__Canvas_Shell_Workbench_Persistence_And_Interaction_Integrity.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_09__Canvas_Shell_Workbench_Persistence_And_Interaction_Integrity.md): close the canvas shell/workbench split.
10. [Phase_10__Workspace_Project_Navigation_And_Reopen_Continuity.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_10__Workspace_Project_Navigation_And_Reopen_Continuity.md): make workspace continuity and navigation dependable.
11. [Phase_11__Source_Control_Repository_History_And_Diff_Hardening.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_11__Source_Control_Repository_History_And_Diff_Hardening.md): replace synthetic repo behavior on the release path.
12. [Phase_12__Build_Run_Terminal_Debug_And_Output_Reliability.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_12__Build_Run_Terminal_Debug_And_Output_Reliability.md): harden execution-surfaces users rely on for IDE credibility.
13. [Phase_13__Settings_Config_Scope_Deep_Link_And_Export_Closure.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_13__Settings_Config_Scope_Deep_Link_And_Export_Closure.md): finish settings ownership and persistence.
14. [Phase_14__Plugin_Extension_Contribution_And_Gating_Completion.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_14__Plugin_Extension_Contribution_And_Gating_Completion.md): finish only the extension work required for release-path trust.
15. [Phase_15__Cloud_Security_Vault_And_Transport_Release_Hardening.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_15__Cloud_Security_Vault_And_Transport_Release_Hardening.md): remove placeholder crypto and transport assumptions.
16. [Phase_16__Rendering_Import_Export_PDF_Media_And_Print_Reliability.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_16__Rendering_Import_Export_PDF_Media_And_Print_Reliability.md): close fidelity gaps that still affect visible release behavior.
17. [Phase_17__Advanced_Domain_Release_Triage_And_Gating.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_17__Advanced_Domain_Release_Triage_And_Gating.md): explicitly finish, gate, or de-scope advanced domains.
18. [Phase_18__Migration_Stub_And_Dead_Path_Retirement.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_18__Migration_Stub_And_Dead_Path_Retirement.md): remove duplicate and transitional release-path logic.
19. [Phase_19__Integrated_Validation_Recovery_Observability_And_Regression_Gates.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_19__Integrated_Validation_Recovery_Observability_And_Regression_Gates.md): make validation and recovery evidence block release.
20. [Phase_20__Release_Candidate_Signoff_Packaging_And_Final_Closure.md](/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_20__Release_Candidate_Signoff_Packaging_And_Final_Closure.md): execute the final release-candidate closure wave.

## This Pass In One Sentence

`v25` is the authoritative next-step plan for taking MarkAmp from “broad and partially completed” to “tightened, hardened, validated, and ready for serious release-candidate execution.”
