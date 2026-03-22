# Phase 17: Accessibility Visual Clarity Contrast And Reduced Motion

## Outcome

Strengthen visual accessibility across the product through better contrast, stronger focus visibility, clearer hit targets, reduced-motion parity, and more inclusive state communication.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P17-T01

- Phase ID: P17
- Task ID: P17-T01
- Task Title: Audit and raise contrast quality across shell, content, and transient surfaces
- Priority: P0
- Category: Accessibility Visuals
- Atomic Improvements Covered: 60
- Objective: Make the interface easier to read and operate under real-world viewing conditions.
- Why This Matters Now: Premium design and accessibility both fail when contrast discipline is weak.
- UI Problem Statement: Many surfaces rely on nuanced muted text, layered backgrounds, and subtle state fills that need systematic contrast review.
- User Experience Impact: Low-vision, glare-heavy, and long-session users experience fatigue and ambiguity.
- Scope: Text contrast, border contrast, icon contrast, selected/hover/focus contrast, disabled-state legibility, shell region separation.
- Out of Scope: Full assistive-technology integration beyond visual clarity.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Screens / Components / Surfaces: Entire product, especially shell chrome, editor overlays, panels, dialogs, settings.
- Current UI Behavior / Appearance: Contrast is reasonable in many places but not yet systematically verified or tuned.
- Intended UI Behavior / Appearance: Contrast quality feels deliberate and inclusive across all major surface families.
- Visual / Interaction Design Direction: Strong readability without sacrificing aesthetic restraint.
- Technical Styling Approach: Add contrast audits to theme and component systems, then tune tokens and local overrides that fall short.
- Implementation Steps: Audit contrast across core surfaces; identify weak text/icon/border cases; retune token relationships; fix local overrides; document accessible contrast floors.
- Validation Steps: Run contrast audits and manual visual reviews across dark, light, and high-contrast themes.
- Acceptance Criteria: Critical text, controls, and state cues meet stronger contrast expectations product-wide.
- Dependencies: Phase 01, Phase 18.
- Risks / Failure Modes: Raising contrast indiscriminately can make the UI visually harsh or noisy.
- Accessibility / Readability Notes: This task is directly about low-vision and readability quality.
- Theme / Styling Notes: Contrast tuning must respect each theme’s character without compromising legibility.
- Motion / Interaction Notes where relevant: None directly, beyond ensuring motion is not used to compensate for poor contrast.
- Observability / Diagnostics Notes where relevant: Add automated contrast reporting and screenshot annotations to the UI QA stack.
- Rollback / Safety Notes: Use staged token adjustments to avoid abrupt visual shocks.
- References / Context: Premium UI and accessibility are aligned here; better contrast improves both.
- Example scenarios where useful: Muted metadata in side panels remains readable on low-brightness displays without losing hierarchy.

### P17-T02

- Phase ID: P17
- Task ID: P17-T02
- Task Title: Redesign focus rings, keyboard selection cues, and current-target visibility
- Priority: P0
- Category: Accessibility Visuals
- Atomic Improvements Covered: 60
- Objective: Make keyboard and focus navigation visually obvious everywhere.
- Why This Matters Now: The app already has many focusable custom controls, and weak focus visibility would undermine the entire premium UI wave.
- UI Problem Statement: Custom-painted controls and dense row surfaces rely on varied focus treatments and sometimes under-express keyboard focus.
- User Experience Impact: Keyboard users can lose orientation and trust in interactive surfaces.
- Scope: Focus rings, focus outlines, current-row focus, tab focus, list focus, toolbar focus, dialog focus, overlay focus.
- Out of Scope: Full keyboard command routing semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h`
- Related Screens / Components / Surfaces: All focusable controls, row systems, tabs, overlays, dialogs.
- Current UI Behavior / Appearance: Focus cues exist but are not yet guaranteed to be equally strong and elegant everywhere.
- Intended UI Behavior / Appearance: Keyboard focus is always easy to locate and visually consistent.
- Visual / Interaction Design Direction: Clear focus presence with refined, non-clumsy visual emphasis.
- Technical Styling Approach: Standardize focus ring tokens and focus-visible variants across all custom control families.
- Implementation Steps: Audit focus-visible states; normalize ring geometry and contrast; improve current-target cues in rows and tabs; align keyboard-selection visuals with focus treatments.
- Validation Steps: Navigate the app entirely by keyboard across shell, dialogs, panels, settings, and overlays.
- Acceptance Criteria: Users can reliably see where keyboard focus is at all times.
- Dependencies: Phase 01, Phase 04, Phase 15.
- Risks / Failure Modes: Overly aggressive focus styling can make the UI feel noisy if not tuned carefully.
- Accessibility / Readability Notes: Focus visibility is a core accessibility requirement and must not rely solely on color.
- Theme / Styling Notes: Focus cues need strong contrast on both light and dark backgrounds.
- Motion / Interaction Notes where relevant: Focus transitions should be subtle and not create distracting animation.
- Observability / Diagnostics Notes where relevant: Add focus-state screenshot fixtures for every major control family.
- Rollback / Safety Notes: Keep all existing focus mechanics while visuals are standardized.
- References / Context: Keyboardability is only as good as focus visibility in a custom UI.
- Example scenarios where useful: Moving through settings categories and rows by keyboard feels crystal clear.

### P17-T03

- Phase ID: P17
- Task ID: P17-T03
- Task Title: Improve hit-target visibility, pointer affordance clarity, and touchpad-friendly interaction visuals
- Priority: P1
- Category: Accessibility Visuals
- Atomic Improvements Covered: 60
- Objective: Make interactive areas easier to notice and easier to trust visually.
- Why This Matters Now: Many dense IDE controls are visually compact and need excellent affordance discipline.
- UI Problem Statement: Small action targets in tabs, toolbars, panel headers, trees, and canvas tools still vary in visibility and target framing.
- User Experience Impact: Users miss actions or feel uncertain about what is clickable or draggable.
- Scope: Icon-only buttons, close affordances, drag handles, splitter grips, row actions, disclosure controls, tiny badges and chips.
- Out of Scope: Underlying pointer event logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SplitterBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: Tabs, toolbars, panel headers, splitters, canvas tools, rows.
- Current UI Behavior / Appearance: Many controls are interactive, but their affordance clarity is inconsistent.
- Intended UI Behavior / Appearance: Interactive targets are easier to discover without adding visual clutter.
- Visual / Interaction Design Direction: Quiet but unmistakable affordance design.
- Technical Styling Approach: Define target-visibility rules and use hover/focus/shape treatment to improve discoverability.
- Implementation Steps: Audit small targets; increase optical clarity; improve icon-only control framing; adjust hover and active affordances; refine grip visuals for resize and drag.
- Validation Steps: Use pointer and trackpad interactions on dense shell and panel surfaces at multiple scales.
- Acceptance Criteria: Users can more easily identify interactive targets without the UI becoming busier.
- Dependencies: Phase 01, Phase 19, Phase 20.
- Risks / Failure Modes: Making targets too visually obvious can clutter dense surfaces.
- Accessibility / Readability Notes: Small controls must meet stronger hit-target and focus-visibility expectations.
- Theme / Styling Notes: Hover and target framing must retain contrast across all themes.
- Motion / Interaction Notes where relevant: Hover reveal should not hide the target too completely at rest if discoverability is harmed.
- Observability / Diagnostics Notes where relevant: Add target-density and affordance screenshots for small-control surfaces.
- Rollback / Safety Notes: Preserve current hit areas while visual framing is adjusted.
- References / Context: Premium UI depends on obvious but tasteful affordance cues.
- Example scenarios where useful: The close affordance on a tab is easy to find without becoming visually loud.

### P17-T04

- Phase ID: P17
- Task ID: P17-T04
- Task Title: Implement reduced-motion visual parity across shell, overlays, editor, notebook, and canvas transitions
- Priority: P1
- Category: Accessibility Visuals
- Atomic Improvements Covered: 60
- Objective: Ensure reduced-motion users still experience a polished interface rather than a degraded one.
- Why This Matters Now: Motion is expanding across the product and needs inclusive handling before it becomes more pervasive.
- UI Problem Statement: Motion tokens exist, but reduced-motion behavior is not yet clearly systematized across all visible surfaces.
- User Experience Impact: Users sensitive to motion can experience discomfort or lower visual stability.
- Scope: Palette open/close, dialog fade, tooltips, toast motion, minimap transitions, tab motion, sidebar transitions, canvas guides, notebook execution motion.
- Out of Scope: Operating-system preference detection internals beyond visible behavior requirements.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Screens / Components / Surfaces: Shell transitions, overlays, editor and canvas interaction cues, toasts, dialogs.
- Current UI Behavior / Appearance: Motion is implemented locally across surfaces and reduced-motion parity is not yet visibly guaranteed.
- Intended UI Behavior / Appearance: Reduced-motion mode preserves quality, clarity, and responsiveness with calmer transitions.
- Visual / Interaction Design Direction: Motion restraint with equal aesthetic dignity for motion-reduced experiences.
- Technical Styling Approach: Map motion tokens to reduced-motion alternates and provide static-state variants for animated UI families.
- Implementation Steps: Inventory animated surfaces; define reduced-motion alternates; eliminate motion-only state communication; tune static-state affordances; add preference-aware visual tests.
- Validation Steps: Compare animated and reduced-motion UI states across shell, overlays, editor, notebook, and canvas surfaces.
- Acceptance Criteria: Reduced-motion mode still feels polished and complete, not merely stripped down.
- Dependencies: Phase 01, Phase 19.
- Risks / Failure Modes: Static alternatives can feel abrupt if state design is not improved in tandem.
- Accessibility / Readability Notes: State changes must remain understandable without animation.
- Theme / Styling Notes: Reduced-motion alternatives still need full theme parity.
- Motion / Interaction Notes where relevant: This task directly governs motion reduction standards.
- Observability / Diagnostics Notes where relevant: Add reduced-motion screenshot and timing references to the atlas.
- Rollback / Safety Notes: Keep motion feature flags configurable while reduced-motion design matures.
- References / Context: Motion quality includes the quality of its absence when users request less of it.
- Example scenarios where useful: Opening the command palette in reduced-motion mode still feels premium and responsive without scale or fade theatrics.

### P17-T05

- Phase ID: P17
- Task ID: P17-T05
- Task Title: Strengthen low-vision readability through clearer spacing, weight, and non-color state differentiation
- Priority: P1
- Category: Accessibility Visuals
- Atomic Improvements Covered: 60
- Objective: Improve readability for users who benefit from stronger structural cues, not just higher contrast.
- Why This Matters Now: Elite UI quality should improve comprehension, not only aesthetics.
- UI Problem Statement: Dense surfaces often rely on subtle color and tight spacing rather than stronger structural hierarchy and typography.
- User Experience Impact: Low-vision and fatigue-sensitive users may struggle to parse dense content efficiently.
- Scope: Typography weight, metadata differentiation, row spacing, current-item emphasis, severity shape coding, icon plus text pairing, state borders.
- Out of Scope: Full zoom feature implementation beyond visible design implications.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Screens / Components / Surfaces: Dense lists, settings, status bar, metadata-heavy panels, editor overlays.
- Current UI Behavior / Appearance: Many dense surfaces could become easier to parse through stronger structure rather than only more color.
- Intended UI Behavior / Appearance: The UI becomes easier to read and navigate through clearer visual structure.
- Visual / Interaction Design Direction: More legible hierarchy and stronger non-color state cues without losing refinement.
- Technical Styling Approach: Tune density and hierarchy tokens to allow a more readable mode and stronger structural defaults where needed.
- Implementation Steps: Audit low-vision stress points; improve weight and spacing differentiation; add non-color state cues; strengthen current-item and severity shape treatments; refine metadata tiers.
- Validation Steps: Review dense surfaces at common zoom levels and under simulated low-vision conditions.
- Acceptance Criteria: Dense surfaces become more parseable without becoming clumsy or overbuilt.
- Dependencies: Phase 01, Phase 15.
- Risks / Failure Modes: Increasing structure can make some surfaces feel heavier if not calibrated carefully.
- Accessibility / Readability Notes: This task is explicitly about low-vision and fatigue-aware clarity.
- Theme / Styling Notes: Light themes may need different non-color cue strategies than dark themes.
- Motion / Interaction Notes where relevant: Static structure should carry meaning so motion is not required.
- Observability / Diagnostics Notes where relevant: Add low-vision review boards and structural-hierarchy checklists.
- Rollback / Safety Notes: Keep readable-mode adjustments incremental and measured against the premium design bar.
- References / Context: Accessibility quality should elevate the whole interface, not sit beside it as a concession.
- Example scenarios where useful: A dense settings category becomes easier to scan through stronger hierarchy rather than just brighter colors.

### P17-T06

- Phase ID: P17
- Task ID: P17-T06
- Task Title: Add accessibility-visual review boards and gating criteria for UI acceptance
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Make visual accessibility a release criterion rather than a late audit.
- Why This Matters Now: A UI-only wave is the correct time to encode visual accessibility into the quality bar.
- UI Problem Statement: Accessibility visual quality is currently not represented strongly enough in the UI review and regression workflow.
- User Experience Impact: Important readability and focus problems can survive until users encounter them directly.
- Scope: Contrast reports, focus-state galleries, reduced-motion captures, low-vision review boards, non-color cue audits, hit-target visual reviews.
- Out of Scope: Full assistive-technology integration testing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FocusRingRenderer.h`
- Related Screens / Components / Surfaces: Entire app.
- Current UI Behavior / Appearance: Accessibility visual quality is not yet consistently represented in product-wide review assets.
- Intended UI Behavior / Appearance: Visual accessibility quality is measurable, documented, and release-relevant.
- Visual / Interaction Design Direction: Inclusive polish as a first-class design criterion.
- Technical Styling Approach: Add accessibility-specific screenshot suites, reports, and acceptance thresholds to the UI QA stack.
- Implementation Steps: Define review criteria; create fixture boards; wire contrast and focus reports into CI artifacts; require review for regressions in key surfaces.
- Validation Steps: Intentionally weaken a focus ring or contrast ratio and verify the review pipeline catches it.
- Acceptance Criteria: Accessibility visual regressions are surfaced and actionable before release.
- Dependencies: P17-T01 through P17-T05, Phase 18, Phase 20.
- Risks / Failure Modes: Overly rigid thresholds can generate noise if not calibrated by surface family.
- Accessibility / Readability Notes: This task formalizes the accessibility review bar for all later UI work.
- Theme / Styling Notes: Reviews must cover every supported major theme mode.
- Motion / Interaction Notes where relevant: Reduced-motion captures and criteria are part of the acceptance stack.
- Observability / Diagnostics Notes where relevant: Publish structured accessibility-visual reports alongside screenshot boards.
- Rollback / Safety Notes: Start with high-signal critical surfaces before widening to the whole product if needed.
- References / Context: Inclusive visual quality should be part of “elite UI,” not separate from it.
- Example scenarios where useful: A regression that makes selected rows too subtle in light mode is caught during UI review instead of by users.

