# Phase 03: Toolbar Status Bar And Global Button Wiring

## Outcome

Make top-level buttons and status actions reliable, context-aware, and backed by canonical control state rather than local widget code.

## Improvement Count

126 atomic improvements across 6 execution tasks.

### P03-T01

- Phase ID: P03
- Task ID: P03-T01
- Task Title: Rebase the visible toolbar on `ToolbarModel`
- Priority: P0
- Category: Toolbar Actions
- Atomic Improvements Covered: 21
- Objective: Replace the hardcoded visible toolbar behavior with a model-driven toolbar.
- Why This Matters Now: The visible toolbar and toolbar model currently live as separate systems.
- Problem Statement: `Toolbar.cpp` hardcodes left/right buttons while `ToolbarModel.cpp` is not the driving source.
- User Impact: Toolbar behavior can drift from commands, context, and intended affordances.
- Scope: Button definitions, toggle state, visibility, overflow, context slots, icon and tooltip sourcing.
- Out of Scope: Full visual redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp`
- Related Systems / Components: Toolbar, commands, context state, design system.
- Current Behavior: Toolbar buttons are wired directly in the widget.
- Intended Behavior: Toolbar renders canonical buttons from `ToolbarModel` and dispatches canonical commands.
- Technical Approach: Feed toolbar visual state and actions from `ToolbarModel` plus command metadata.
- Implementation Steps: Adapt model to visible toolbar; remove local button assumptions; connect context slots and overflow.
- Validation Steps: Compare toolbar states across editor, notebook, canvas, and settings contexts.
- Acceptance Criteria: Toolbar button set and state are model-driven and reproducible.
- Dependencies: Phase 01.
- Risks / Failure Modes: Existing toolbar animation code may rely on local button indices.
- UX Notes: Preserve responsiveness and current compact-mode affordances.
- Settings / Panel / Control Notes where relevant: Settings button should be just another canonical toolbar action.
- Observability / Diagnostics Notes: Log toolbar action ID and model state snapshot.
- Rollback / Safety Notes: Migrate button groups incrementally to avoid a full rewrite in one step.
- References / Context: `Toolbar.cpp` and `ToolbarModel.cpp` are visibly out of sync as architecture.
- Example scenarios where useful: Toolbar shows notebook actions only when notebook context is active.

### P03-T02

- Phase ID: P03
- Task ID: P03-T02
- Task Title: Audit every global toolbar button for real execution and state feedback
- Priority: P0
- Category: Button Wiring
- Atomic Improvements Covered: 21
- Objective: Ensure each visible toolbar button has correct click behavior, checked state, disabled state, tooltip, and post-action feedback.
- Why This Matters Now: Toolbar buttons are some of the highest-frequency controls in the app.
- Problem Statement: Save, themes, settings, and view-mode buttons are still driven locally.
- User Impact: Daily workflows feel inconsistent or shallow.
- Scope: Save, view mode buttons, focus mode, themes, settings, build indicators, overflow, notifications.
- Out of Scope: Full notification system redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
- Related Systems / Components: Toolbar, commands, build/run events, settings.
- Current Behavior: Toolbar click behavior mixes command dispatch and local side effects.
- Intended Behavior: Toolbar behavior is correct, observable, and matches other entry points.
- Technical Approach: Bind toolbar buttons to canonical commands and state derivation rules.
- Implementation Steps: Audit button set; connect command IDs; normalize disabled and active states; validate tooltips and visual feedback.
- Validation Steps: Click every toolbar button under multiple contexts and compare traces.
- Acceptance Criteria: No visible toolbar button is a no-op or local-only special case without reason.
- Dependencies: P03-T01.
- Risks / Failure Modes: Over-centralization may break polished microfeedback if not preserved.
- UX Notes: Keep save pulse, hover, and active feedback while fixing behavior.
- Settings / Panel / Control Notes where relevant: Settings button must open the real settings host, not just publish a weak event.
- Observability / Diagnostics Notes: Add per-button execution and no-op metrics.
- Rollback / Safety Notes: Preserve animation hooks as adapters around canonical actions.
- References / Context: `Toolbar.cpp` currently publishes save and settings events directly.
- Example scenarios where useful: Save button is disabled when nothing saveable is active and enabled when a dirty artifact is active.

### P03-T03

- Phase ID: P03
- Task ID: P03-T03
- Task Title: Make status bar actions real and context-aware
- Priority: P1
- Category: Button Wiring
- Atomic Improvements Covered: 21
- Objective: Ensure status bar items are not just display text but valid actions with proper routing and enablement where intended.
- Why This Matters Now: Status bars often accumulate dead or misleading affordances.
- Problem Statement: `StatusBarPanel.cpp` rebuilds many items from events, but action semantics must be verified end-to-end.
- User Impact: Users click status items expecting navigation or configuration but may get inconsistent results.
- Scope: Sidebar mode, encoding, line/column, theme, git branch, AI status, save flash, notifications.
- Out of Scope: Full status bar redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/StatusBarAdapter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/StatusBarItemService.cpp`
- Related Systems / Components: Status bar, commands, active context, diagnostics.
- Current Behavior: Status items are event-fed, but not all interaction behavior is obviously canonical.
- Intended Behavior: Every clickable status item either routes to a real action or is rendered clearly non-clickable.
- Technical Approach: Introduce explicit action metadata per status item and bind through canonical command dispatch.
- Implementation Steps: Audit status items; mark actionable vs informational; wire actions; add hover/focus/tooltip correctness.
- Validation Steps: Click and keyboard-trigger each actionable status item.
- Acceptance Criteria: No ambiguous or misleading status-bar affordance remains.
- Dependencies: Phase 01.
- Risks / Failure Modes: Some items may not have stable action targets yet.
- UX Notes: Informational items should not masquerade as buttons.
- Settings / Panel / Control Notes where relevant: Theme and encoding items often need settings/dialog hooks.
- Observability / Diagnostics Notes: Log actionable status-item activations separately from passive updates.
- Rollback / Safety Notes: Disable clickability explicitly until a real action exists.
- References / Context: `StatusBarPanel.cpp` subscribes to many events and needs a matching action audit.
- Example scenarios where useful: Clicking the encoding item opens a real encoding selector or does nothing visibly clickable at all.

### P03-T04

- Phase ID: P03
- Task ID: P03-T04
- Task Title: Audit startup, welcome, walkthrough, and empty-state CTA buttons
- Priority: P1
- Category: Button Wiring
- Atomic Improvements Covered: 21
- Objective: Ensure onboarding and empty-state controls trigger real workflows, not partial shell transitions.
- Why This Matters Now: Empty-state CTAs are often the first controls new users encounter.
- Problem Statement: Startup and welcome surfaces contain bespoke click handlers and partial workflows.
- User Impact: First-run experience can feel broken immediately.
- Scope: Startup panel cards, welcome tab actions, walkthrough actions, open folder/recent workspace CTAs, panel empty-state actions.
- Out of Scope: Full onboarding content rewrite.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`
- Related Systems / Components: Startup, welcome, recent workspaces, panel empty states.
- Current Behavior: These controls are scattered and likely bypass canonical action routing.
- Intended Behavior: All onboarding CTAs invoke the same trustworthy action system as the rest of the app.
- Technical Approach: Rebind CTA surfaces to canonical commands and add completion/failure feedback.
- Implementation Steps: Inventory CTA actions; map to commands; fix no-op states; add disabled or hidden rules when unavailable.
- Validation Steps: Exercise startup and empty-state surfaces from a clean profile.
- Acceptance Criteria: No onboarding CTA remains misleading or dead.
- Dependencies: P01-T02.
- Risks / Failure Modes: Legacy startup flows may assume direct `onOpenFolder` invocation.
- UX Notes: CTAs should be obvious, low-friction, and trustworthy.
- Settings / Panel / Control Notes where relevant: Empty settings and panel CTAs should open real hosts.
- Observability / Diagnostics Notes: Track CTA source and completion result.
- Rollback / Safety Notes: Keep a fallback command path for workspace-open if startup surface changes mid-migration.
- References / Context: `StartupPanel.cpp` and `ExplorerPanel.cpp` both wire action-like CTAs directly.
- Example scenarios where useful: “Open Folder” from the explorer empty state and startup screen uses the same command.

### P03-T05

- Phase ID: P03
- Task ID: P03-T05
- Task Title: Standardize global toggle controls and checked-state feedback
- Priority: P1
- Category: Enablement / Visibility Logic
- Atomic Improvements Covered: 21
- Objective: Make all toggles and checked controls reflect real state reliably.
- Why This Matters Now: View, panel, and settings toggles currently mix widget state and system state.
- Problem Statement: Checked items in menus and toggles in controls may not derive from one shared state model.
- User Impact: Users cannot trust whether a feature is actually on or off.
- Scope: View toggles, sidebar toggles, minimap toggles, auto-save, focus mode, panel visibility toggles, settings checkboxes.
- Out of Scope: Full theme token polish for toggles.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedToggleSwitch.cpp`
- Related Systems / Components: Menus, toolbar, settings, shell layout state.
- Current Behavior: Toggle state is partly initialized in UI and partly event-driven.
- Intended Behavior: Checked state always reflects authoritative model state.
- Technical Approach: Feed toggles from shell state, settings state, or command-state predicates, not startup defaults alone.
- Implementation Steps: Audit toggle surfaces; replace static `Check(...)` defaults with state binding; sync menu, toolbar, and settings toggles.
- Validation Steps: Toggle features from multiple surfaces and confirm all others update.
- Acceptance Criteria: No toggle remains visually out of sync with actual state.
- Dependencies: P01-T03, Phase 08.
- Risks / Failure Modes: Circular updates if state propagation is not one-way.
- UX Notes: Toggle feedback should be immediate and unambiguous.
- Settings / Panel / Control Notes where relevant: Settings toggles should show staged vs applied state correctly.
- Observability / Diagnostics Notes: Add state-drift detectors for shared toggles.
- Rollback / Safety Notes: Prefer state subscriptions over manual back-propagation.
- References / Context: `MainFrame.cpp` currently hard-checks many view toggles on creation.
- Example scenarios where useful: Turning off minimap from the menu updates toolbar and settings instantly.

### P03-T06

- Phase ID: P03
- Task ID: P03-T06
- Task Title: Add global-control smoke tests for toolbar, status bar, and onboarding surfaces
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Make high-frequency global controls release-gated.
- Why This Matters Now: These controls are touched constantly and regressions are expensive.
- Problem Statement: Toolbar/status/onboarding regressions are easy to introduce during command unification.
- User Impact: Core navigation and trust degrade quickly.
- Scope: Toolbar clicks, status-item clicks, startup/welcome CTAs, toggle-state synchronization.
- Out of Scope: Every downstream feature’s deep behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Systems / Components: Toolbar, status bar, startup, diagnostics.
- Current Behavior: Global controls are not obviously protected by focused smoke coverage.
- Intended Behavior: CI catches wrong-target, no-op, and out-of-sync control regressions.
- Technical Approach: Build test flows around canonical action IDs and state snapshots.
- Implementation Steps: Define smoke cases; add action assertions; add toggle-sync checks; integrate into CI.
- Validation Steps: Run smoke suite across a clean and an active workspace state.
- Acceptance Criteria: Global control regressions block release.
- Dependencies: P03-T01 through P03-T05.
- Risks / Failure Modes: Tests may ignore animation-timing concerns if hooks are weak.
- UX Notes: Validate both pointer and keyboard activation when relevant.
- Settings / Panel / Control Notes where relevant: Include settings-button flow from the toolbar and status bar if applicable.
- Observability / Diagnostics Notes: Capture control traces with focus and context snapshots.
- Rollback / Safety Notes: Keep smoke tests deterministic and isolated.
- References / Context: Toolbar and status bar are highly visible and currently partially local in logic.
- Example scenarios where useful: Clicking Settings from the toolbar opens the same host as the Preferences menu and palette command.
