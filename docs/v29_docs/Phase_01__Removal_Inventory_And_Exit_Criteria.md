# Phase 01: Removal Inventory And Exit Criteria

## Task Count

- 2 tasks

## Task V29-P01-T01

- Phase ID: V29-P01
- Task ID: V29-P01-T01
- Task Title: Build the canonical removal ledger for Canvas, Notebook, Flashcards, and Tasks
- Priority: Critical
- Category: Feature Inventory Audit
- Objective: Produce the repository-backed list of every remaining UI surface, service, model, command, test, build entry, and persistence seam tied to the four removed features.
- Why This Matters Now: Direct deletion will be incomplete and unstable unless the team is working from one authoritative removal ledger.
- Removal Gap Statement: Current evidence is spread across `MainFrame`, `LayoutManager`, artifact services, events, plugins, and build files; no single removal ledger exists.
- User / Product Impact: Prevents ghost references, dead-end controls, and startup regressions after staged deletion.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v21_docs/ASSESSMENT__MarkAmp_V21_Control_Panels_And_Settings_Assessment.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_18__Migration_Retirement_Placeholder_Removal_And_Dead_Path_Cleanup.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_18__Migration_Stub_And_Dead_Path_Retirement.md`
- Scope: Source tree search, build registration, test registration, menus, activity bar, sidebar modes, shell modes, persistence, startup, help, plugins, and AI-related seams.
- Out of Scope: Implementing the removals themselves.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarMode.h`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`; `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`; `/Users/ryanrentfro/code/markamp/src/core/TaskService.h`
- Related Features / Systems / Components: Shell, navigation, artifact lifecycle, event bus, plugin system, tests, build graph.
- Current Behavior / Presence: All four feature families are still discoverable and partially wired as shipping features.
- Intended Post-Removal Behavior: The codebase has one audited map of what must be deleted, what must be migrated, and what remains in scope.
- Removal / Simplification Direction: Treat removed features as a retirement program with explicit ownership and evidence, not ad hoc file deletion.
- Technical Approach: Generate a machine-actionable matrix keyed by feature, surface type, file path, removal owner, migration need, and validation status.
- Implementation Steps: Search feature terms and type names; classify by user-facing surface versus backend dependency; flag shared infrastructure; mark migration-required items; store the ledger in v29 docs.
- Validation Steps: Reconcile ledger counts against `src/CMakeLists.txt`, `tests/CMakeLists.txt`, activity bar items, sidebar modes, artifact kinds, and event families.
- Acceptance Criteria: Every later v29 task can cite an explicit ledger entry and no major removal class is discovered late.
- Dependencies: None.
- Parallelization Notes: Can run in parallel with targeted code reading, but must complete before direct deletion phases are finalized.
- Risks / Failure Modes: Missing hidden affordances in startup, help text, or plugins will create residual ghost references.
- Migration / Compatibility Notes: Ledger must mark which persisted-state readers need to survive temporarily.
- UX / Layout Cleanup Notes: Ledger must identify post-removal shell gaps, not just feature-owned files.
- Cleanup / Consolidation Notes: Also record shared abstractions that can collapse after removals.
- Rollback / Safety Notes: This task changes no product behavior.
- References / Context: v29 assessment and this master package.
- Example Scenarios: Old workspace reopens a notebook tab; activity bar still shows Canvas; command palette still finds `Canvas: New Board`.

## Task V29-P01-T02

- Phase ID: V29-P01
- Task ID: V29-P01-T02
- Task Title: Lock the removal definition, compatibility policy, and retained-product boundaries
- Priority: Critical
- Category: Release / Product Simplification
- Objective: Define what “fully removed” means for each feature and document the exact temporary compatibility seams allowed during the wave.
- Why This Matters Now: Earlier plans assumed these features would be completed; v29 needs a different done-state or work will drift back toward hiding instead of removing.
- Removal Gap Statement: Without explicit exit criteria, teams can stop at menu hiding and leave artifact kinds, events, tests, or restore paths intact.
- User / Product Impact: Gives implementation agents and reviewers one unambiguous standard for done.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_01__Program_Spine_Backlog_Deduplication_And_Definition_Of_Finished.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_01__Release_Spine_Done_Criteria_And_Gate_Enforcement.md`
- Scope: Done definitions, retained-scope statement, migration-only exceptions, review checklist, and signoff requirements.
- Out of Scope: Low-level implementation details for each feature family.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`
- Related Features / Systems / Components: Product scope, QA, code review, release gates.
- Current Behavior / Presence: The repository still encodes notebook and canvas as first-class product surfaces and still documents flashcards and tasks as valid workflows.
- Intended Post-Removal Behavior: Reviewers can mechanically answer whether a feature is truly gone or still present in hidden form.
- Removal / Simplification Direction: Favor full deletion with narrow migration exceptions only.
- Technical Approach: Define per-feature done criteria plus cross-cutting criteria for UI, commands, persistence, build, docs, and tests.
- Implementation Steps: Write definitions; list allowed temporary compatibility readers; list disallowed partial-removal patterns; add phase exit criteria.
- Validation Steps: Use the definitions in every later phase acceptance section.
- Acceptance Criteria: Every phase file in v29 inherits the same removal standard and retained-product scope.
- Dependencies: V29-P01-T01.
- Parallelization Notes: Can be drafted while the ledger is being finalized, but signoff must wait for the ledger.
- Risks / Failure Modes: Ambiguous compatibility policy can keep dead subsystems alive indefinitely.
- Migration / Compatibility Notes: Only graceful ignore, conversion, or notification paths are allowed; no hidden runtime re-enablement.
- UX / Layout Cleanup Notes: Done criteria must require intentional shell simplification after feature removal.
- Cleanup / Consolidation Notes: Establish that shared abstractions should be collapsed, not left with empty branches.
- Rollback / Safety Notes: Documentation only.
- References / Context: This file and the v29 master index.
- Example Scenarios: A code review removes the `Canvas` menu but leaves `ArtifactKind::kCanvas`; this policy should mark that as incomplete.
