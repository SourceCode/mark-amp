# Phase 20: Release Closure And Smaller Product Signoff

## Task Count

- 2 tasks

## Task V29-P20-T01

- Phase ID: V29-P20
- Task ID: V29-P20-T01
- Task Title: Execute the final removal signoff checklist across UI, build, migration, and retained workflows
- Priority: Critical
- Category: Release / Product Simplification
- Objective: Run the final cross-functional signoff proving the four retired feature families are gone and the retained product is stable.
- Why This Matters Now: v29 is a high-risk retirement wave and needs a formal closure gate, not only code deletion.
- Removal Gap Statement: Earlier release phases focused on completion of these domains; v29 needs a release gate proving successful retirement instead.
- User / Product Impact: Reduces the chance of shipping a half-removed product with residual ghost references or broken upgrades.
- Repository Evidence: Entire v29 package plus retained shell/build/test surfaces.
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_20__Release_Readiness_Subsystem_Signoff_And_Final_Completion_Gates.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_20__Release_Candidate_Signoff_Packaging_And_Final_Closure.md`
- Scope: Final signoff checklist, smoke routes, upgrade fixtures, menu and navigation audit, build audit, and docs audit.
- Out of Scope: New feature work.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v29_docs`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`
- Related Features / Systems / Components: Release gating, QA, build, migration, docs.
- Current Behavior / Presence: Signoff expectations are still framed around broader feature completeness from earlier passes.
- Intended Post-Removal Behavior: Release closure explicitly validates the smaller product and removal completeness.
- Removal / Simplification Direction: Convert signoff from “did we finish these features?” to “did we fully retire them and stabilize what remains?”
- Technical Approach: Use a fixed signoff checklist covering UI absence, build absence, migration safety, retained-flow stability, and doc cleanup.
- Implementation Steps: Define checklist; run shell audits; run targeted builds/tests; review docs; record signoff results in v29 docs or release notes.
- Validation Steps: Confirm each acceptance area passes with evidence links or test outputs.
- Acceptance Criteria: The release checklist shows all removal gates passing and all temporary compatibility exceptions documented.
- Dependencies: Phases 01 through 19.
- Parallelization Notes: Mostly final integration work.
- Risks / Failure Modes: Teams may stop after code compiles and skip UX or migration audits.
- Migration / Compatibility Notes: Signoff must explicitly list any remaining temporary downgrade readers and planned retirement date.
- UX / Layout Cleanup Notes: Include a visual sweep for ghost labels and empty shell gaps.
- Cleanup / Consolidation Notes: Confirm no deferred “remove later” backlog remains for these domains without explicit justification.
- Rollback / Safety Notes: If a gate fails, do not ship partial removal.
- References / Context: This is the formal endpoint for the v29 retirement wave.
- Example Scenarios: Final shell audit confirms no notebook/canvas/task/flashcard entry point exists anywhere visible.

## Task V29-P20-T02

- Phase ID: V29-P20
- Task ID: V29-P20-T02
- Task Title: Publish the post-removal operating baseline for future implementation work
- Priority: Medium
- Category: Release / Product Simplification
- Objective: Record the new canonical product baseline so future implementation does not rebuild removed assumptions.
- Why This Matters Now: After a large retirement wave, teams need a fresh baseline for what MarkAmp now is and what shared abstractions remain valid.
- Removal Gap Statement: Prior architecture and planning context heavily emphasized removed domains and could continue to distort future decisions.
- User / Product Impact: Keeps future work aligned with the simplified product and reduces reintroduction risk.
- Repository Evidence: v29 package plus retained core surfaces.
- Prior Plan References: This v29 package supersedes earlier notebook/canvas/flashcard/task expansion work.
- Scope: Baseline statement, retained-scope summary, compatibility remainder list, and future-planning constraints.
- Out of Scope: Detailed roadmap for unrelated retained features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v29_docs/README__MarkAmp_V29_Feature_Removal_Master_Index.md`; `/Users/ryanrentfro/code/markamp/docs/v29_docs/ASSESSMENT__MarkAmp_V29_Feature_Removal_And_Product_Simplification.md`
- Related Features / Systems / Components: Planning, contributor guidance, future execution.
- Current Behavior / Presence: Historical feature-expansion context still dominates long-form planning.
- Intended Post-Removal Behavior: Future work begins from the smaller-product baseline documented in v29.
- Removal / Simplification Direction: Make product simplification durable in planning, not only in code.
- Technical Approach: Publish a concise baseline statement and cross-link it from active planning entry points.
- Implementation Steps: Update active planning readmes; summarize retained scope; list no-go assumptions; identify any temporary compatibility debt left.
- Validation Steps: Spot-check active contributor docs and planning entry points for alignment with v29.
- Acceptance Criteria: A future coding agent reading active docs will not assume Canvas, Notebook, Flashcards, or Tasks are still in scope to finish.
- Dependencies: V29-P20-T01.
- Parallelization Notes: Small final documentation task.
- Risks / Failure Modes: If skipped, future planning can drift back into feature resurrection by inertia.
- Migration / Compatibility Notes: Document any remaining temporary compatibility behavior explicitly.
- UX / Layout Cleanup Notes: None.
- Cleanup / Consolidation Notes: This closes the planning loop created by v18-v27.
- Rollback / Safety Notes: Documentation-only.
- References / Context: v29 is the authoritative next-step operating plan for these removals.
- Example Scenarios: A future roadmap references v29 before proposing any new shell or artifact abstractions.
