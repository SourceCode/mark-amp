# Phase 14 - Startup Welcome Onboarding And Empty State Elegance

## Goal

Make startup, welcome, onboarding, and empty states premium, restrained, and deeply coherent with the rest of the product.

## Definition Of Done

- startup and welcome surfaces feel aspirational but disciplined
- empty states across the product share a calm, premium language
- first-run and low-content moments no longer expose visual roughness or duplicated shell logic

## Tasks

### Phase ID: V26-P14
### Task ID: V26-P14-T01
### Task Title: Upgrade Startup And Welcome Surface To Premium Product-Level Quality
- Priority: P0
- Category: Empty State / Onboarding UI
- Objective: Turn startup into a first-class premium surface with refined hierarchy, recent-items presentation, and clear create/open affordances.
- Why This Matters Now: First impression matters, and startup still shows signs of duplicated logic and local styling.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp` duplicates shell logic and includes fallback accent handling and local paint decisions.
- User Experience Impact: A strong startup surface raises trust before any document is opened.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Prior Plan References: `v22` onboarding/startup, `v25` visible shell closure
- Scope: welcome layout, recent-items design, create/open actions, typography, background treatment, empty-workspace framing
- Out of Scope: recent-item backend behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Screens / Components / Surfaces: startup screen, welcome surface, recent items
- Current UI Behavior / Appearance: startup is functional but still carries local styling debt and duplicated shell patterns.
- Intended UI Behavior / Appearance: startup feels aspirational, calm, and premium while staying productivity-oriented.
- Visual / Interaction Design Direction: restrained welcome surface with strong hierarchy, excellent spacing, and subtle product character.
- Technical Styling Approach: align startup with shell tokens and shared control/card patterns; remove local fallback styling.
- Implementation Steps:
  1. Remove duplicated or special-case shell styling where possible.
  2. Refine welcome layout and recent-item cards/rows.
  3. Normalize create/open action styling and metadata.
  4. Validate light/dark and empty/non-empty startup states.
- Validation Steps:
  1. Open the app with and without recent items.
  2. Compare startup with settings and shell chrome.
  3. Review typography and spacing at multiple window sizes.
- Acceptance Criteria: startup reads as a flagship premium surface and no longer exposes local or fallback visual debt.
- Dependencies: V26-P01-T01, V26-P01-T02, V26-P10-T03
- Parallelization Notes: can run once shared control and shell standards are stable.
- Risks / Failure Modes: too much atmosphere can conflict with the product’s professional tone.
- Accessibility / Readability Notes: recent-item metadata and action labels must remain crisp and readable.
- Theme / Styling Notes: startup should not rely on special-case accent fallbacks.
- Motion / Interaction Notes where relevant: any welcome-surface motion must remain subtle and optional.
- Cleanup / Consolidation Notes where relevant: remove duplicated shell logic and local startup-only styling where shared systems work.
- Rollback / Safety Notes: keep startup behavior stable while improving presentation.
- References / Context: startup is one of the most visible high-impact surfaces in the entire app.
- Example scenarios where useful: opening MarkAmp with no documents should still feel premium and purposeful.

### Phase ID: V26-P14
### Task ID: V26-P14-T02
### Task Title: Define A Shared Premium Empty-State System Across Panels, Notebook, Canvas, And Structured Surfaces
- Priority: P0
- Category: Empty State / Onboarding UI
- Objective: Give the product one consistent empty-state language instead of a mix of sparse placeholders and one-off messages.
- Why This Matters Now: Empty states reveal visual debt immediately and are still uneven across multiple subsystems.
- UI Gap Statement: Explorer sections, notebook, canvas, startup, and structured panels likely use different empty-state conventions and maturity levels.
- User Experience Impact: Consistent empty states make the product feel complete and easier to learn.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Prior Plan References: `v22` empty-state work, `v25` release-path closure of visible rough states
- Scope: empty-state structure, iconography, typography, supporting text, action placement, compact versus full-page variants
- Out of Scope: onboarding tours or behavioral walkthrough systems
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Screens / Components / Surfaces: startup, canvas empty state, notebook empty state, empty lists/tables/panels
- Current UI Behavior / Appearance: empty states are likely a mix of custom text, placeholders, and thin shells.
- Intended UI Behavior / Appearance: all empty states feel part of one premium product, scaled appropriately for context.
- Visual / Interaction Design Direction: restrained guidance with excellent hierarchy and minimal visual noise.
- Technical Styling Approach: define shared empty-state templates and adapt them for full-page, panel, and compact contexts.
- Implementation Steps:
  1. Catalog empty-state variants across major surfaces.
  2. Define full-page, panel, and compact empty-state templates.
  3. Replace temporary or inconsistent variants.
  4. Validate theme parity and content-length flexibility.
- Validation Steps:
  1. Trigger empty states across startup, notebook, canvas, explorer, and AV surfaces.
  2. Compare layout, hierarchy, and action placement.
  3. Check readability in narrow and full-page contexts.
- Acceptance Criteria: empty states across the app feel clearly related, premium, and complete.
- Dependencies: V26-P01-T02, V26-P10-T03, V26-P16-T01
- Parallelization Notes: can run in parallel with startup polish after template direction is agreed.
- Risks / Failure Modes: empty-state patterns can become repetitive if not scaled appropriately for context.
- Accessibility / Readability Notes: iconography must support, not replace, textual explanation.
- Theme / Styling Notes: empty-state emphasis must remain calm in both themes.
- Motion / Interaction Notes where relevant: minimal appearance motion only; avoid onboarding theatrics.
- Cleanup / Consolidation Notes where relevant: retire placeholder empty-state fragments and duplicate template code.
- Rollback / Safety Notes: do not remove context-specific clarity in the name of consistency.
- References / Context: this task connects many other phases and reduces visible roughness quickly.
- Example scenarios where useful: an empty explorer section and an empty canvas should feel related without being visually identical.

### Phase ID: V26-P14
### Task ID: V26-P14-T03
### Task Title: Refine Onboarding Cues, Educational Hints, And Discoverability Surfaces With Restraint
- Priority: P1
- Category: Empty State / Onboarding UI
- Objective: Improve educational cues and first-use guidance without creating a noisy or consumerish UI.
- Why This Matters Now: Discoverability is important, but premium products solve it with restraint.
- UI Gap Statement: Onboarding and hint surfaces often accumulate ad hoc styling and uneven tone.
- User Experience Impact: Better hints help new users without making the app feel cluttered or over-instructive.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Prior Plan References: `v22` onboarding and discoverability work
- Scope: subtle first-use hints, contextual education blocks, discoverability cues in startup/canvas/command surfaces
- Out of Scope: analytics-driven onboarding flows
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Related Screens / Components / Surfaces: startup hints, canvas onboarding, command discovery cues
- Current UI Behavior / Appearance: hints and discoverability surfaces likely vary and may feel either sparse or locally improvised.
- Intended UI Behavior / Appearance: educational cues are polished, subtle, and integrated into the visual system.
- Visual / Interaction Design Direction: confident, quiet guidance with low interruption and high clarity.
- Technical Styling Approach: build on empty-state and feedback components rather than inventing new visual categories.
- Implementation Steps:
  1. Identify visible first-use hint surfaces.
  2. Standardize typography, iconography, and action placement for those cues.
  3. Reduce any overly loud or underdesigned variants.
  4. Validate against startup and canvas empty-state systems.
- Validation Steps:
  1. Review onboarding cues as a first-time user.
  2. Check that hints are discoverable but not overbearing.
  3. Verify visual coherence with feedback and empty states.
- Acceptance Criteria: onboarding cues feel intentional, premium, and restrained across core surfaces.
- Dependencies: V26-P14-T02, V26-P13-T01
- Parallelization Notes: can run after the shared empty-state system is defined.
- Risks / Failure Modes: over-designed hints can clutter the UI; too subtle can hide guidance.
- Accessibility / Readability Notes: keep hint copy readable and action affordances explicit.
- Theme / Styling Notes: hints should remain supportive and low-noise in both themes.
- Motion / Interaction Notes where relevant: any hint emphasis motion should be very subtle and optional.
- Cleanup / Consolidation Notes where relevant: merge duplicate hint styles into the shared system.
- Rollback / Safety Notes: do not remove important discovery cues without equivalent replacements.
- References / Context: this task closes the gap between polished empties and polished first-use guidance.
- Example scenarios where useful: a new user opening the command palette should get guidance that feels elegant rather than tutorial-heavy.
