# Phase 09: Premium Visual System Polish And MUI Icon Completion

## Outcome

Complete the shared visual language of the IDE, including shell chrome, controls, tabs, trees, dialogs, status surfaces, icons, and state treatments, so the product feels intentionally designed and finished.

## Improvement Count

210 atomic improvements across 6 execution tasks.

### P09-T01

- Phase ID: P09
- Task ID: P09-T01
- Task Title: Define and apply a shared shell visual system
- Priority: P0
- Category: Styling / Visual Design
- Atomic Improvements Covered: 35
- Objective: Standardize shell background layers, panel chrome, borders, hover states, shadows, and surface hierarchy.
- Why This Matters Now: The app still feels like adjacent modules rather than one system.
- Problem Statement: Shell surfaces vary in chrome density, border rules, and state treatment.
- User Impact: The product looks unfinished even when features are present.
- Scope: Activity/sidebar/header/status/tab/panel/dialog/popover/toast visual hierarchy and state tokens.
- Out of Scope: Full brand redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/themes`
- Related Systems / Components: Theme engine, shell UI, panels, dialogs, tabs, status bar.
- Current Behavior: Surface styling varies by component and history.
- Intended Behavior: The shell reads as one product with clear hierarchy and calm polish.
- Technical Approach: Define layered shell tokens and migrate major chrome surfaces to them.
- Implementation Steps: Inventory shell surfaces; define layer tokens; normalize states; remove one-off border/shadow hacks; validate across themes.
- Validation Steps: Walk every major shell screen and compare hierarchy, spacing, and state transitions.
- Acceptance Criteria: Shell chrome is materially more cohesive and premium-looking.
- Dependencies: Phase 08.
- Risks / Failure Modes: Overstandardization may flatten useful distinctions.
- UX Notes: Hierarchy should guide attention without visual noise.
- Styling / Highlighting Notes where relevant: Syntax surfaces must sit within the shell cleanly without color clashes.
- Observability / Diagnostics Notes: Generate a shell style inventory and token-usage report.
- Rollback / Safety Notes: Keep old token aliases during migration only.
- References / Context: Prior passes already found inconsistent chrome across panels, toolbars, and status areas.
- Example scenarios where useful: Switching from explorer to search to notebook to canvas still feels like one coherent application.

### P09-T02

- Phase ID: P09
- Task ID: P09-T02
- Task Title: Complete the MUI icon migration and remove mixed icon semantics
- Priority: P0
- Category: Styling / Visual Design
- Atomic Improvements Covered: 35
- Objective: Finish icon normalization across the workbench.
- Why This Matters Now: Mixed icon systems instantly signal incomplete product integration.
- Problem Statement: Legacy icon residue and placeholder glyphs remain across major surfaces.
- User Impact: Surfaces feel inconsistent and sometimes visually confusing.
- Scope: Icon inventory cleanup, semantic mapping, sizes, spacing, theming, accessibility labels, interactive states.
- Out of Scope: Custom illustration system.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/MuiIconPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Systems / Components: Icon pipeline, shell UI, canvas tool rail, explorer, menus.
- Current Behavior: Mixed icon families and Unicode placeholders remain in use.
- Intended Behavior: Iconography is semantically consistent, theme-aware, and accessibility-friendly.
- Technical Approach: Finish icon inventory, map semantics by action/state, and retire legacy loaders and glyph placeholders.
- Implementation Steps: Audit all icons; map semantic roles; replace placeholders; normalize sizes and padding; add labels/tooltips; remove residue loaders.
- Validation Steps: Review icon usage in toolbars, trees, tabs, panels, dialogs, and canvas rail.
- Acceptance Criteria: No major surface uses legacy icon residue or ambiguous icon semantics.
- Dependencies: None.
- Risks / Failure Modes: Overly literal icons reduce scan speed.
- UX Notes: Favor semantic consistency over novelty.
- Styling / Highlighting Notes where relevant: Icon color rules must align with hover, active, disabled, and error states.
- Observability / Diagnostics Notes: Emit icon-residue reports and missing-label checks.
- Rollback / Safety Notes: Keep a minimal compatibility shim for unported plugins only.
- References / Context: Canvas tool rail still uses Unicode placeholders; prior icon work remains incomplete.
- Example scenarios where useful: New file, new notebook, and new canvas affordances use distinct but clearly related icon semantics across all surfaces.

### P09-T03

- Phase ID: P09
- Task ID: P09-T03
- Task Title: Polish tabs, trees, toolbars, dialogs, and transient feedback surfaces
- Priority: P1
- Category: Styling / Visual Design
- Atomic Improvements Covered: 35
- Objective: Eliminate obvious fit-and-finish gaps in the most frequently used chrome.
- Why This Matters Now: These surfaces are the daily texture of the product.
- Problem Statement: Tabs, explorer rows, toolbars, dialogs, and toasts still vary in density and state quality.
- User Impact: Frequent interactions feel less refined than premium IDE competitors.
- Scope: Tab geometry, dirty markers, hover/close affordances, tree rows, toolbar spacing, dialogs, sheets, toasts, progress bars.
- Out of Scope: New navigation paradigms.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: Tabs, explorer, toolbar, notifications, status bar.
- Current Behavior: High-traffic surfaces still show inconsistent polish.
- Intended Behavior: Every frequent interaction surface feels tuned and dependable.
- Technical Approach: Apply shared metrics and state tokens, then refine interaction behaviors surface by surface.
- Implementation Steps: Retune tabs; refine tree row states; clean toolbar grouping; redesign dialogs/toasts; improve progress and save indicators.
- Validation Steps: Use the app for common daily workflows and compare state treatment across surfaces.
- Acceptance Criteria: The shell no longer has obvious “unfinished” hotspots in daily-use chrome.
- Dependencies: P09-T01, P09-T02.
- Risks / Failure Modes: Micro-polish work can drift without metrics.
- UX Notes: Frequent surfaces should stay low-friction and fast to scan.
- Styling / Highlighting Notes where relevant: Dirty, active, search-hit, and error states must remain visually distinct.
- Observability / Diagnostics Notes: Add style inventory checks for off-scale controls and wrong icon sizes.
- Rollback / Safety Notes: Keep visual changes token-driven for easy rollback.
- References / Context: `TabBar.cpp` currently carries creation logic and likely corresponding chrome inconsistencies.
- Example scenarios where useful: A dirty tab, active explorer row, and save toast all feel like parts of the same system.

### P09-T04

- Phase ID: P09
- Task ID: P09-T04
- Task Title: Improve empty, loading, error, and recovery states across the product
- Priority: P1
- Category: Accessibility
- Atomic Improvements Covered: 35
- Objective: Make non-happy-path states clear, polished, and useful.
- Why This Matters Now: Broken creation and persistence work have already shown the cost of weak failure states.
- Problem Statement: Many surfaces likely have sparse or misleading empty/error states.
- User Impact: Users are left uncertain when workflows fail or require next steps.
- Scope: Welcome states, empty editors, empty notebooks, empty boards, loading workspaces, save failures, restore failures, missing-artifact recovery.
- Out of Scope: Full guided onboarding.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Systems / Components: Shell, notebooks, canvas, persistence, notifications.
- Current Behavior: Many empty and failure states are likely minimal or inconsistent.
- Intended Behavior: Users always understand what happened, what is safe, and what to do next.
- Technical Approach: Create a shared state-design pattern library and apply it to high-risk flows first.
- Implementation Steps: Inventory state gaps; define state templates; add retry/help/actions; align iconography and typography; verify keyboard reachability.
- Validation Steps: Trigger empty and error states intentionally across core workflows.
- Acceptance Criteria: Empty and failure states feel deliberate, consistent, and actionable.
- Dependencies: Phase 02 through Phase 05, P09-T01, P09-T02.
- Risks / Failure Modes: Too much explanation may clutter compact surfaces.
- UX Notes: Recovery-focused actions should be more prominent than decorative language.
- Styling / Highlighting Notes where relevant: Error and warning colors must retain contrast without overwhelming the rest of the UI.
- Observability / Diagnostics Notes: Track top-empty-state CTAs and recovery-state retry outcomes.
- Rollback / Safety Notes: Keep state templates data-driven for quick iteration.
- References / Context: Current creation and save failures often rely on logs or generic message boxes.
- Example scenarios where useful: Failed notebook save offers retry, Save As, and recovery copy options in a polished dialog.

### P09-T05

- Phase ID: P09
- Task ID: P09-T05
- Task Title: Improve accessibility, keyboardability, and discoverability of premium styling work
- Priority: P1
- Category: Accessibility
- Atomic Improvements Covered: 35
- Objective: Ensure visual polish strengthens, rather than harms, usability.
- Why This Matters Now: Premium UI that reduces accessibility is not premium.
- Problem Statement: Icon-only actions, subtle states, and dense chrome risk becoming undiscoverable or inaccessible.
- User Impact: Keyboard users and low-vision users may be blocked from key workflows.
- Scope: Focus rings, accessible labels, keyboard order, hover-independent affordances, contrast, tooltip quality, mnemonic consistency.
- Out of Scope: Full screen-reader implementation redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui`; `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`; `/Users/ryanrentfro/code/markamp/themes`
- Related Systems / Components: Shell UI, icon system, themes, commands.
- Current Behavior: Accessibility likely trails visual work in multiple surfaces.
- Intended Behavior: Premium polish remains keyboard-first, labeled, and legible.
- Technical Approach: Add accessibility checks to visual-system migration and enforce labels/focus/contrast rules.
- Implementation Steps: Audit labels and focus order; add missing labels; improve focus rings; test tooltip discoverability; verify keyboard reachability.
- Validation Steps: Navigate create/save/search flows by keyboard only and with reduced-contrast sensitivity checks.
- Acceptance Criteria: Core workflows remain fully discoverable and operable without a mouse.
- Dependencies: P09-T01 through P09-T04.
- Risks / Failure Modes: Visual simplification may be needed to preserve accessibility.
- UX Notes: Discoverability should come from structure and labels, not only tooltips.
- Styling / Highlighting Notes where relevant: Focus, selection, and syntax contrast must meet stronger readability targets.
- Observability / Diagnostics Notes: Add automated missing-label and contrast scans.
- Rollback / Safety Notes: Fail CI on unlabeled icon actions in core surfaces.
- References / Context: The MUI icon completion work explicitly requires accessibility labeling and state behavior validation.
- Example scenarios where useful: Keyboard user can create a canvas, save it, and return focus to the board without ambiguity.

### P09-T06

- Phase ID: P09
- Task ID: P09-T06
- Task Title: Add premium-polish review gates and visual debt inventory tracking
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Prevent visual debt from silently re-accumulating after cleanup.
- Why This Matters Now: Without gates, the product will slide back toward inconsistency.
- Problem Statement: The app has enough breadth that one-off styling regressions can reappear quickly.
- User Impact: Cohesion erodes over time even after substantial polish work.
- Scope: Visual debt inventory, style linting, icon linting, screenshot review cadence, polish checklist for new work.
- Out of Scope: Subjective design council process.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/core/IconInventory.cpp`
- Related Systems / Components: Testing, docs, theme system, icon pipeline, UI components.
- Current Behavior: Visual consistency likely depends too much on manual memory.
- Intended Behavior: Visual-system integrity becomes a maintained engineering concern.
- Technical Approach: Add style inventory tooling, review checklists, and targeted screenshot gates.
- Implementation Steps: Create debt ledger; add lint checks for icons and tokens; define review baselines; require screenshot updates for key shell changes.
- Validation Steps: Run visual debt tools and confirm they flag residue after intentional regressions.
- Acceptance Criteria: New visual drift becomes measurable and reviewable.
- Dependencies: P09-T01 through P09-T05.
- Risks / Failure Modes: Tooling too noisy to be used.
- UX Notes: Focus on issues users actually feel first: state clarity, consistency, readability.
- Styling / Highlighting Notes where relevant: Include syntax and content surfaces in visual debt reviews, not just shell chrome.
- Observability / Diagnostics Notes: Produce per-surface debt reports and icon-residue counts.
- Rollback / Safety Notes: Keep checklists concise so teams actually follow them.
- References / Context: The breadth of MarkAmp UI makes ungated polish unsustainable.
- Example scenarios where useful: A new toolbar action cannot ship without icon label, sizing, focus-state, and screenshot review coverage.

