# Phase 18: Dead Code Docs And Legacy Reference Retirement

## Task Count

- 2 tasks

## Task V29-P18-T01

- Phase ID: V29-P18
- Task ID: V29-P18-T01
- Task Title: Remove dead source files, includes, comments, TODOs, and generator scripts tied to retired features
- Priority: Medium
- Category: Cleanup / Dead Code Retirement
- Objective: Retire the code comments, TODOs, helper scripts, and dead source leftovers that will remain after primary deletion work.
- Why This Matters Now: Large codebases often keep removed features alive in comments and helper scripts long after runtime deletion.
- Removal Gap Statement: The repository still contains feature-specific planning and generation scripts such as `/Users/ryanrentfro/code/markamp/scripts/generate_v17_canvas_ux_plan.py` plus code comments and headers naming removed domains.
- User / Product Impact: Keeps internal maintenance and future planning aligned with the smaller product.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/scripts/generate_v17_canvas_ux_plan.py`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v23_docs/Phase_20__Final_Placeholder_Eradication_Dead_Path_Removal_And_Release_Gates.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_18__Migration_Retirement_Placeholder_Removal_And_Dead_Path_Cleanup.md`
- Scope: Dead files, stale includes, comments, TODOs, planning-generator scripts, and obsolete inline references.
- Out of Scope: User-facing help text already handled in earlier phases.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/generate_v17_canvas_ux_plan.py`; `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`
- Related Features / Systems / Components: Dead code, developer docs, scripts, comments.
- Current Behavior / Presence: Removed-feature references are likely to persist in internal support files and comments even after code deletion.
- Intended Post-Removal Behavior: Internal repository guidance no longer points engineers toward retired work.
- Removal / Simplification Direction: Remove stale internal references aggressively once compatibility work is resolved.
- Technical Approach: Repository-wide search for retired feature names, then triage by production relevance and safe deletion.
- Implementation Steps: Delete dead scripts; remove stale comments/TODOs; update file headers; remove obsolete includes and phase markers.
- Validation Steps: Search repository excluding historical docs for retired feature names and verify remaining hits are intentional.
- Acceptance Criteria: No non-historical internal support file or comment still implies the retired features are active roadmap items.
- Dependencies: Phases 06 through 15.
- Parallelization Notes: Broad but low-risk cleanup once code deletion stabilizes.
- Risks / Failure Modes: Historical docs should not be deleted accidentally; scope the cleanup to active source/support files.
- Migration / Compatibility Notes: Historical v18-v27 docs remain as historical context, not active implementation guidance.
- UX / Layout Cleanup Notes: None.
- Cleanup / Consolidation Notes: This is where the repository mentally catches up with the smaller product.
- Rollback / Safety Notes: Keep changes reviewable by separating source cleanup from historical-doc preservation.
- References / Context: This phase distinguishes active-code cleanup from historical archival docs.
- Example Scenarios: No active script remains dedicated to future canvas UX work.

## Task V29-P18-T02

- Phase ID: V29-P18
- Task ID: V29-P18-T02
- Task Title: Update active documentation and planning references so future work does not resurrect retired features
- Priority: Medium
- Category: Cleanup / Dead Code Retirement
- Objective: Retire active documentation references to removed features and mark prior plans as superseded by v29 where appropriate.
- Why This Matters Now: Future agents and contributors will follow active docs unless the repository clearly states that these features were intentionally removed.
- Removal Gap Statement: Prior planning artifacts across v18-v27 repeatedly instruct implementation of notebook/canvas/task/flashcard work and can mislead future execution.
- User / Product Impact: Reduces roadmap confusion and prevents accidental reintroduction of removed work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_07__Notebook_Host_Execution_And_Document_Trust.md`; `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_08__Canvas_Workbench_And_Cross_Surface_Integration.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_08__Notebook_Lifecycle_Runtime_And_Output_Integrity.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_09__Canvas_Shell_Workbench_Persistence_And_Interaction_Integrity.md`
- Prior Plan References: All prior v18-v27 docs cited above plus this v29 package.
- Scope: Active README-style docs, roadmap docs, and cross-links from current documentation into retired feature plans.
- Out of Scope: Historical archival docs kept for provenance.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs`; `/Users/ryanrentfro/code/markamp/AGENTS.md`; `/Users/ryanrentfro/code/markamp/docs/v29_docs/README__MarkAmp_V29_Feature_Removal_Master_Index.md`
- Related Features / Systems / Components: Documentation, planning references, contributor guidance.
- Current Behavior / Presence: Historical docs dominate feature-specific planning narratives and do not yet have a superseding retirement reference.
- Intended Post-Removal Behavior: Active documentation points contributors to v29 as the authoritative retirement blueprint.
- Removal / Simplification Direction: Preserve history but clearly supersede it.
- Technical Approach: Add supersession notes in active docs and remove active-roadmap references to the retired features.
- Implementation Steps: Update active planning indexes and contributor guidance; add “superseded by v29” where useful; remove current-roadmap references to retired features.
- Validation Steps: Search active docs and contributor docs for retired features and verify context is archival or removal-oriented only.
- Acceptance Criteria: Future agents can clearly distinguish archival feature plans from active implementation direction.
- Dependencies: v29 package completion.
- Parallelization Notes: Can run late in the wave once the package is stable.
- Risks / Failure Modes: Over-editing historical docs can erase provenance; prefer supersession notes over destructive rewrite.
- Migration / Compatibility Notes: None.
- UX / Layout Cleanup Notes: None.
- Cleanup / Consolidation Notes: Reduces roadmap entropy after the removal wave.
- Rollback / Safety Notes: Keep archival planning docs intact.
- References / Context: This phase keeps the repository’s planning memory coherent.
- Example Scenarios: A future agent sees that v25 notebook work is retired by v29 rather than still pending.
