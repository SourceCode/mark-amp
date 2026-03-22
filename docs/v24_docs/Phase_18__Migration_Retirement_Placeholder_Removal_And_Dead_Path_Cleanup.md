# Phase 18 - Migration Retirement, Placeholder Removal, And Dead Path Cleanup

## Phase Goal

Remove the obsolete branches, duplicate pathways, placeholder helpers, and transition layers that earlier phases made unnecessary.

## Measurable Outcome

- Duplicate old/new pathways are retired.
- Placeholder and stub production code is no longer reachable.
- Migration shims are minimized and explicitly bounded.

### Task P18-T01

- Phase ID: `P18`
- Task ID: `P18-T01`
- Task Title: Retire old artifact, command, panel, and settings pathways once replacements are verified
- Priority: `P0`
- Category: `Cleanup / De-duplication`
- Objective: Remove legacy branches that would otherwise keep the product in a semi-migrated state.
- Why This Matters Now: Completion is not real until old paths are gone.
- Execution Gap Statement: The repo contains many "new system alongside old system" seams.
- User / Product Impact: Keeping both paths alive preserves inconsistency and regression risk.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v20`, `v21`, `v23` migration-focused phases
- Scope: Old creation paths, local save logic, direct settings mutation, panel placeholder factories, duplicate command registrations
- Out of Scope: Test doubles and fixture-specific compatibility helpers
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`, `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`
- Related Features / Systems / Components: Migration cleanup, old/new overlap
- Current Behavior: Replacement systems often coexist with the systems they were meant to replace.
- Intended Completed Behavior: One authoritative path remains for each finished subsystem.
- Missing Pieces: Removal sequencing, compatibility deadlines, audit confirmation
- Technical Approach: Remove legacy branches only after successor paths pass readiness gates.
- Implementation Steps: Inventory duplicate paths; verify replacement readiness; delete old branches; update tests/docs.
- Validation Steps: Run subsystem smoke and readiness suites after each removal bundle.
- Acceptance Criteria: Finished subsystems no longer rely on parallel legacy code paths.
- Dependencies: All preceding subsystem completion phases
- Parallelization Notes: Cleanup can happen per subsystem after signoff, not only at the very end.
- Risks / Failure Modes: Removing compatibility too early can break legacy restore/config/session data.
- Cleanup / Migration Notes: This is the dedicated retirement phase; avoid leaving "temporary" code in place after signoff.
- Observability / Diagnostics Notes: Track removed-path counts and any fallback hits still occurring.
- Rollback / Safety Notes: Remove in bounded commits with clear fallback plans if regressions surface.
- References / Context: All prior migration seams
- Example scenarios where useful: Removing tab-bar untitled path creation after unsaved artifact records are proven stable.

### Task P18-T02

- Phase ID: `P18`
- Task ID: `P18-T02`
- Task Title: Eliminate explicit placeholder, stub, fake, and deferred production branches still reachable
- Priority: `P0`
- Category: `Placeholder / Stub Removal`
- Objective: Convert the explicit unfinished markers found in `v23` into actual removals or completed implementations.
- Why This Matters Now: Reachable placeholder production code is one of the clearest blockers to calling the product complete.
- Execution Gap Statement: Multiple files still contain reachable placeholder/stub branches even after subsystem work lands.
- User / Product Impact: Users encounter misleading or synthetic behavior instead of product truth.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`, `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`
- Prior Plan References: `v23` Phase 01, 17, 20
- Scope: Reachable explicit placeholders, stubs, fake outputs, simulated production branches
- Out of Scope: Test-only mocks and fixtures
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_v23_service_stub_completion.cpp`
- Related Features / Systems / Components: Completion audit, code cleanup
- Current Behavior: Some explicit unfinished markers still correspond to reachable production logic.
- Intended Completed Behavior: Explicit unfinished markers in production code are either gone or gated off non-release paths.
- Missing Pieces: Final removal inventory and gating verification
- Technical Approach: Re-run unfinished-work audits, then remove or hard-gate every reachable production seam.
- Implementation Steps: Audit markers; classify reachable branches; complete, delete, or gate them; update tests and docs.
- Validation Steps: Re-run audit search and readiness suites to confirm no reachable production placeholders remain.
- Acceptance Criteria: Reachable production code no longer contains placeholder/stub behavior without explicit non-release gating.
- Dependencies: Subsystem completion in Phases 06-17
- Parallelization Notes: Can run continuously per subsystem as work closes.
- Risks / Failure Modes: String-only marker searches can miss implicit placeholders; code review still matters.
- Cleanup / Migration Notes: Preserve marker-free test doubles separately from production code.
- Observability / Diagnostics Notes: Track counts of removed, gated, and still-deferred placeholders.
- Rollback / Safety Notes: Prefer explicit gating over silent retention if immediate completion is unsafe.
- References / Context: `v23` unfinished-code audit
- Example scenarios where useful: A panel can remain hidden in developer mode temporarily, but not ship with an obvious "stub" implementation in production paths.

### Task P18-T03

- Phase ID: `P18`
- Task ID: `P18-T03`
- Task Title: Remove hidden dead UI and misleading affordances left behind after gating
- Priority: `P1`
- Category: `Cleanup / De-duplication`
- Objective: Ensure gating incomplete features does not leave discoverable but dead affordances in the shell.
- Why This Matters Now: Hiding implementation is not enough if labels, actions, shortcuts, or restore state still point to it.
- Execution Gap Statement: Earlier phases may gate unfinished systems, but residual controls and UI labels can still leak through.
- User / Product Impact: Users can still discover dead ends and lose trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Prior Plan References: `v21` dead-UI removal, `v23` Phase 20
- Scope: Menus, buttons, shortcuts, context entries, panel restore state, onboarding/recent links
- Out of Scope: Hidden developer-only diagnostics surfaces
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Features / Systems / Components: Dead UI, shell controls, panel visibility
- Current Behavior: Gated or deferred features can still leave residual UI traces.
- Intended Completed Behavior: Only actionable, real affordances remain in normal product paths.
- Missing Pieces: Residual-affordance audit and cleanup pass
- Technical Approach: Audit visible controls against final readiness state and remove residual shell references.
- Implementation Steps: Reconcile menu maps, shortcuts, panels, startup links, and status items against final readiness policy.
- Validation Steps: Search UI surfaces for gated feature IDs and unreachable actions.
- Acceptance Criteria: No dead or misleading affordance remains in standard release UI.
- Dependencies: `P18-T01`, `P18-T02`
- Parallelization Notes: Best executed after each major gating/removal wave.
- Risks / Failure Modes: Hidden secondary entry points such as shortcuts or restore manifests can be missed.
- Cleanup / Migration Notes: Remove stale documentation labels and tooltip strings alongside UI cleanup.
- Observability / Diagnostics Notes: Use action/panel audits to detect orphaned surface references.
- Rollback / Safety Notes: Keep developer-only access paths explicit and isolated from release mode.
- References / Context: Action and panel audit systems
- Example scenarios where useful: A removed PDF panel also disappears from menus, restore manifests, command palette, and onboarding links.

### Task P18-T04

- Phase ID: `P18`
- Task ID: `P18-T04`
- Task Title: Delete obsolete style fragments, metrics forks, and local visual leftovers after system adoption
- Priority: `P1`
- Category: `Cleanup / De-duplication`
- Objective: Prevent UI-system regressions by removing now-obsolete local visual code.
- Why This Matters Now: Shared visual systems remain fragile while old local styling paths still exist.
- Execution Gap Statement: Even after UI-system adoption, local style branches can remain in critical surfaces.
- User / Product Impact: Visual drift can return quickly if obsolete style paths persist.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel_styling.txt`
- Prior Plan References: `v22` Phase 20, `v24` Phase 07
- Scope: Local style constants, duplicate metric systems, placeholder styling notes
- Out of Scope: Deliberate component-level exceptions documented in the design system
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`
- Related Features / Systems / Components: UI-system cleanup, styling consolidation
- Current Behavior: Obsolete local visual code can remain even after shared-system migration.
- Intended Completed Behavior: Shared visual systems are the only active styling authority on critical surfaces.
- Missing Pieces: Final residue audit and deletion pass
- Technical Approach: Remove dead style code only after token adoption and visual regression checks pass.
- Implementation Steps: Inventory local constants and styling notes; delete unused branches; update affected tests/baselines.
- Validation Steps: Run visual audits and inspect critical shell surfaces after deletion.
- Acceptance Criteria: Critical surfaces no longer retain obsolete visual forks that can be reactivated accidentally.
- Dependencies: `P07-T02`, `P07-T05`
- Parallelization Notes: Can occur incrementally after each surface family is migrated.
- Risks / Failure Modes: Removing style branches too early can expose token gaps.
- Cleanup / Migration Notes: This is the final visual-tech-debt cleanup pass.
- Observability / Diagnostics Notes: Use static searches and token-usage audits to confirm residue removal.
- Rollback / Safety Notes: Delete in isolated commits to ease regression tracing.
- References / Context: Visual-system adoption work
- Example scenarios where useful: Startup panel no longer relies on a sidecar styling note file for pending visual changes.

### Task P18-T05

- Phase ID: `P18`
- Task ID: `P18-T05`
- Task Title: Re-run the completion audit and publish a post-cleanup residual-gap report
- Priority: `P2`
- Category: `Release Readiness`
- Objective: Verify what genuinely remains after the main completion and cleanup waves land.
- Why This Matters Now: The product needs one final evidence-based view of residual work before release gating.
- Execution Gap Statement: Without a second audit pass, teams may assume cleanup is finished when residual seams remain.
- User / Product Impact: Hidden incompletions can survive into release candidates.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/docs/v23_docs/ASSESSMENT__MarkAmp_V23_Unfinished_Feature_And_Completion_Gap_Assessment.md`
- Prior Plan References: `v23` full completion audit
- Scope: Explicit marker scan, implicit workflow audit, residual-gap report
- Out of Scope: Creating a new broad planning generation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v24_docs/ASSESSMENT__MarkAmp_V24_Execution_Synthesis_And_Completion_Assessment.md`
- Related Features / Systems / Components: Completion auditing, release gating
- Current Behavior: Residual-gap visibility fades as many changes land.
- Intended Completed Behavior: One updated residual-gap report drives final signoff and any remaining waivers.
- Missing Pieces: Post-cleanup audit artifact
- Technical Approach: Re-run explicit unfinished-work scans and compare against subsystem done criteria.
- Implementation Steps: Audit repository; classify residual gaps; decide complete/gate/waive; publish report.
- Validation Steps: Verify every residual item maps to an explicit waiver or final task.
- Acceptance Criteria: Residual-gap report is small, explicit, and traceable to release decisions.
- Dependencies: `P18-T01` through `P18-T04`
- Parallelization Notes: Best run late, after major cleanup lands.
- Risks / Failure Modes: Audit can over-report benign comments unless classification rules stay disciplined.
- Cleanup / Migration Notes: Use report to delete any remaining obsolete waivers or stale TODOs.
- Observability / Diagnostics Notes: Record counts by residual-gap category.
- Rollback / Safety Notes: Preserve report history to justify release decisions.
- References / Context: `v23` audit methodology
- Example scenarios where useful: The final residual report shows only intentionally gated advanced domains and no unexpected production stubs.
