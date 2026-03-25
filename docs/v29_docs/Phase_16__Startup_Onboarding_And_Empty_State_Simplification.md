# Phase 16: Startup Onboarding And Empty State Simplification

## Task Count

- 2 tasks

## Task V29-P16-T01

- Phase ID: V29-P16
- Task ID: V29-P16-T01
- Task Title: Rewrite startup and onboarding flows around the retained editor-first product
- Priority: Medium
- Category: Navigation Simplification
- Objective: Reframe startup and onboarding around retained workflows after removing the four retired product families.
- Why This Matters Now: Without an explicit rewrite, startup can feel like a diminished version of the old product rather than an intentionally focused one.
- Removal Gap Statement: Earlier startup and onboarding plans assumed canvas and notebook were signature surfaces worth showcasing.
- User / Product Impact: Makes the smaller product feel coherent and deliberate on first contact.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v26_docs/Phase_14__Startup_Welcome_Onboarding_And_Empty_State_Elegance.md`; `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_16__Startup_Welcome_Onboarding_And_Empty_State_Redesign.md`
- Scope: Startup copy, onboarding hints, first-run guidance, and workflow emphasis.
- Out of Scope: Deep shell architecture changes already handled elsewhere.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HelpService.h`
- Related Features / Systems / Components: Startup, onboarding, help, retained navigation.
- Current Behavior / Presence: Startup plans and supporting copy still carry assumptions from a broader multi-surface product.
- Intended Post-Removal Behavior: Startup introduces a smaller Markdown-first editor and its retained supporting tools only.
- Removal / Simplification Direction: Replace breadth messaging with focused retained-workflow guidance.
- Technical Approach: Rewrite the copy and CTA structure after deleted feature entry points are gone.
- Implementation Steps: Update startup copy; remove retired-workflow references; emphasize retained document editing, search, graph, AV/data, and source control where applicable.
- Validation Steps: Review first-run and no-workspace states for ghost references or awkward empty gaps.
- Acceptance Criteria: Startup feels deliberate and does not mention removed features.
- Dependencies: Phases 02, 12, and 14.
- Parallelization Notes: Can run alongside empty-state cleanup.
- Risks / Failure Modes: Copy-only cleanup may miss generated labels coming from enums or registries.
- Migration / Compatibility Notes: If upgrade notices exist, keep them separate from onboarding.
- UX / Layout Cleanup Notes: Remove “secondary product pillar” messaging and keep first-run hierarchy calm.
- Cleanup / Consolidation Notes: Simplify startup CTA count and information density.
- Rollback / Safety Notes: Preserve startup navigation and recent-workspace functionality.
- References / Context: This phase converts startup polish plans into startup simplification.
- Example Scenarios: A new user sees editor-centric guidance, not notebook or canvas showcases.

## Task V29-P16-T02

- Phase ID: V29-P16
- Task ID: V29-P16-T02
- Task Title: Rebuild empty states after panel and workflow removal so no ghost affordances remain
- Priority: Medium
- Category: UI Cleanup
- Objective: Remove empty-state text and controls that still reference notebooks, boards, flashcards, or tasks and ensure surviving panels have intentional fallback states.
- Why This Matters Now: Empty states are a common place for removed actions like `Create a board` or `Run Cell` to linger.
- Removal Gap Statement: `LayoutManager` currently builds several feature placeholder panels with feature-specific CTA lists and empty-state copy.
- User / Product Impact: Avoids users encountering dead-end prompts in seldom-used panels.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2693`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2710`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2893`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:3030`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_16__Startup_Welcome_Walkthrough_Empty_Loading_And_Error_States.md`
- Scope: Empty states, CTA rows, placeholder panels, fallback copy, and no-results/no-selection messaging for removed and retained surfaces.
- Out of Scope: Full visual redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Features / Systems / Components: Sidebars, startup, empty states.
- Current Behavior / Presence: Several removed features are represented by placeholder panels with active CTA lists.
- Intended Post-Removal Behavior: Those panels disappear entirely, and surviving surfaces have focused retained empty states.
- Removal / Simplification Direction: Delete removed empty states and reduce placeholder usage overall.
- Technical Approach: Remove feature-owned placeholder factories and clean retained fallback states.
- Implementation Steps: Delete placeholder panels for retired features; review retained panels for leftover references; update copy and layout spacing.
- Validation Steps: Trigger empty states in retained shell surfaces and confirm there are no ghost buttons or labels.
- Acceptance Criteria: No empty state invites users into retired workflows.
- Dependencies: Phase 04.
- Parallelization Notes: Works well after panel deletion stabilizes.
- Risks / Failure Modes: Shared placeholder helpers may still inject retired labels if not fully cleaned.
- Migration / Compatibility Notes: None.
- UX / Layout Cleanup Notes: This is the point where the app should stop feeling like feature slots are missing.
- Cleanup / Consolidation Notes: Remove generic placeholder helpers that only served removed panels.
- Rollback / Safety Notes: Keep retained panels usable without needing feature placeholders.
- References / Context: Empty-state quality is a removal-completeness issue, not only a polish issue.
- Example Scenarios: No sidebar panel says `Create a new board to start designing`.
