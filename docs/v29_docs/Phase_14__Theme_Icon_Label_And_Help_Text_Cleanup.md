# Phase 14: Theme Icon Label And Help Text Cleanup

## Task Count

- 2 tasks

## Task V29-P14-T01

- Phase ID: V29-P14
- Task ID: V29-P14-T01
- Task Title: Remove retired-feature icons, labels, tokens, and theme branches from shipped UI assets
- Priority: Medium
- Category: UI Cleanup
- Objective: Delete icons, labels, token hooks, and theme branches that only exist for notebooks, canvas, flashcards, or tasks.
- Why This Matters Now: Even after logic deletion, visual assets and token names can keep the removed product shape visible.
- Removal Gap Statement: Activity bar labels, sidebar headers, status-bar labels, and canvas/notebook token headers still leave a visible and code-level footprint.
- User / Product Impact: Helps the app feel intentionally smaller rather than cosmetically scarred.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/V27CanvasTokens.h`; `/Users/ryanrentfro/code/markamp/src/ui/V27NotebookTokens.h`; `/Users/ryanrentfro/code/markamp/src/ui/V26NotebookCanvasConvergence.h`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_15__Theme_Parity_Token_Adoption_And_Cross_Surface_Correctness.md`; `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_17__Theme_Parity_Token_Normalization_And_Cross_Surface_Correctness.md`
- Scope: Icons, labels, UI strings, feature-specific token headers, and theme branches tied only to retired surfaces.
- Out of Scope: Retained global theme engine.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/V27CanvasTokens.h`; `/Users/ryanrentfro/code/markamp/src/ui/V27NotebookTokens.h`; `/Users/ryanrentfro/code/markamp/src/ui/V26NotebookCanvasConvergence.h`
- Related Features / Systems / Components: Labels, icons, tokens, theme branches.
- Current Behavior / Presence: Removed features still own icon slots, header labels, and themed surface tokens.
- Intended Post-Removal Behavior: No shipped asset or token naming implies the removed product surfaces still exist.
- Removal / Simplification Direction: Delete feature-owned assets and move retained UI to shared neutral tokens where needed.
- Technical Approach: Remove feature tokens and labels after code deletion; rebind retained shell styling to generic tokens.
- Implementation Steps: Remove labels/icons from shell code; delete feature token headers; clean theme mappings; update icon and theme tests.
- Validation Steps: Search UI assets and theme code for retired-feature names; inspect retained shell for visual gaps.
- Acceptance Criteria: No theme or icon asset remains solely for the retired features.
- Dependencies: Phases 04, 06, and 07.
- Parallelization Notes: Can run alongside help-text cleanup.
- Risks / Failure Modes: Deleting shared-looking tokens without checking callers can regress retained surfaces.
- Migration / Compatibility Notes: None.
- UX / Layout Cleanup Notes: Re-label retained navigation and panels so they read as a coherent smaller product.
- Cleanup / Consolidation Notes: Collapse theme branches that only existed for removed surface parity work.
- Rollback / Safety Notes: Keep retained shell visuals stable with screenshot or snapshot checks.
- References / Context: Many prior UI phases created feature-specific token layers that v29 now retires.
- Example Scenarios: No canvas token header or notebook convergence header remains in production code.

## Task V29-P14-T02

- Phase ID: V29-P14
- Task ID: V29-P14-T02
- Task Title: Remove retired-feature tooltips, onboarding text, sample copy, and discoverability hints
- Priority: Medium
- Category: UI Cleanup
- Objective: Delete all user-facing copy that still teaches or hints at the retired features.
- Why This Matters Now: The last mile of ghost references often survives in hints, hover text, and empty-state copy.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/HelpService.h` still includes canvas and notebook tooltips and flashcard/task samples.
- User / Product Impact: Avoids “where did this feature go?” confusion caused by stale guidance.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:177`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:208`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:227`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:236`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h:335`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_16__Startup_Welcome_Walkthrough_Empty_Loading_And_Error_States.md`; `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_16__Startup_Welcome_Onboarding_And_Empty_State_Redesign.md`
- Scope: Tooltips, help copy, onboarding hints, examples, empty states, and discoverability text.
- Out of Scope: Deep docs/code-comment retirement handled in Phase 18.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Tooltips, onboarding, help, empty states.
- Current Behavior / Presence: Removed features still appear in user education copy.
- Intended Post-Removal Behavior: All discoverability text points only at retained experiences.
- Removal / Simplification Direction: Remove the hints rather than replacing them with “coming soon” or “unsupported” clutter.
- Technical Approach: Audit and rewrite user-facing copy after shell and feature code deletion.
- Implementation Steps: Delete tooltip entries; rewrite empty states; remove examples; update UI text fixtures and tests.
- Validation Steps: Search shipped strings for retired-feature terms and review visible empty states.
- Acceptance Criteria: No user-facing hint or help text references retired features.
- Dependencies: Phases 02 through 12.
- Parallelization Notes: Can run in parallel with Phase 16.
- Risks / Failure Modes: Some discoverability text may be generated indirectly from enum-to-label maps.
- Migration / Compatibility Notes: A concise upgrade notice is allowed only during legacy-state downgrade.
- UX / Layout Cleanup Notes: Use this pass to make the remaining UX vocabulary editor-first and consistent.
- Cleanup / Consolidation Notes: Remove dead string keys and localization entries where applicable.
- Rollback / Safety Notes: Keep retained help coverage intact.
- References / Context: Product simplification is not complete until guidance is simplified too.
- Example Scenarios: No tooltip advertises `Open the visual canvas workspace`.
