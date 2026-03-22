# Phase 12: Terminal Output Problems Build Debug And Source Control Panels

## Outcome

Make the utility-heavy bottom and workbench panels feel as polished as the editor through better typography, row systems, local controls, severity treatments, and dense-information hierarchy.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P12-T01

- Phase ID: P12
- Task ID: P12-T01
- Task Title: Redesign terminal visual presentation for readability, density, and shell integration
- Priority: P0
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make the terminal feel like a first-class premium workbench surface rather than a basic console embed.
- Why This Matters Now: Terminal quality is a major credibility signal in a professional IDE.
- UI Problem Statement: [TerminalPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp), [TerminalTabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.cpp), and related terminal surfaces still need stronger typography, spacing, and control polish.
- User Experience Impact: Command-line workflows feel visually less mature than the rest of the shell should.
- Scope: Terminal typography, line spacing, selection colors, prompt contrast, local toolbar controls, tab bar, search UI, context menu feel.
- Out of Scope: Shell execution or PTY behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`
- Related Screens / Components / Surfaces: Terminal buffer, terminal tabs, terminal search, terminal context menu.
- Current UI Behavior / Appearance: Terminal is featureful, but not yet visibly elite in typography and local chrome.
- Intended UI Behavior / Appearance: Terminal reading comfort and local control polish match the quality bar of the editor.
- Visual / Interaction Design Direction: Crisp, high-legibility technical console with refined shell integration.
- Technical Styling Approach: Create terminal-specific typography and state variants aligned with the editor and panel systems.
- Implementation Steps: Audit terminal rendering and local controls; tune font metrics and line spacing; improve local chrome and tab treatment; refine selection and prompt contrast.
- Validation Steps: Review terminal with long output, command prompts, selection states, and multiple tabs across themes.
- Acceptance Criteria: Terminal use feels premium and visually comfortable for long sessions.
- Dependencies: Phase 01, Phase 03, Phase 18.
- Risks / Failure Modes: Changing terminal typography too aggressively can affect expected compactness for power users.
- Accessibility / Readability Notes: Monospace readability, cursor visibility, and selection clarity are critical.
- Theme / Styling Notes: Terminal contrast must stay strong without becoming harsh or neon-heavy.
- Motion / Interaction Notes where relevant: Cursor and panel-local status changes should be restrained.
- Observability / Diagnostics Notes where relevant: Add terminal typography and state screenshots to the utility-panel gallery.
- Rollback / Safety Notes: Keep terminal content metrics user-configurable if defaults are adjusted.
- References / Context: Terminal presentation quality is core IDE credibility work, not secondary polish.
- Example scenarios where useful: A dense terminal session with selections and search remains easy to read and visually refined.

### P12-T02

- Phase ID: P12
- Task ID: P12-T02
- Task Title: Upgrade Output and log-channel surfaces for better scanability and lower visual clutter
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make output and logging panels feel more intentional and easier to parse under heavy output volume.
- Why This Matters Now: Logs and output channels often become some of the messiest-looking surfaces in an IDE if not curated carefully.
- UI Problem Statement: [OutputPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp) and [OutputPanelRenderer.cpp](/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.cpp) need stronger visual hierarchy and channel treatment.
- User Experience Impact: Build and runtime logs can feel noisy and tiring to scan.
- Scope: Channel selector styling, row spacing, timestamp treatment, severity styling, wrap states, search highlights, empty-state composition, filter chips.
- Out of Scope: Output-channel service logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp`
- Related Screens / Components / Surfaces: Output panel, channel selector, log rows, output search.
- Current UI Behavior / Appearance: Output data is visible, but the panel does not yet maximize visual hierarchy and reading comfort.
- Intended UI Behavior / Appearance: Output and log channels become calmer, cleaner, and easier to scan at scale.
- Visual / Interaction Design Direction: Dense operational output with better grouping and lighter visual noise.
- Technical Styling Approach: Introduce log-row variants and output metadata styles, coordinated with terminal and diagnostics surface families.
- Implementation Steps: Audit output row styling; refine typography and metadata channels; improve channel selector and filters; retune highlight and severity cues; clean up empty and no-channel states.
- Validation Steps: Review long outputs with timestamps, wrapped lines, searches, and mixed channel content.
- Acceptance Criteria: The Output panel feels significantly more deliberate and readable under stress.
- Dependencies: Phase 01, Phase 14, Phase 15.
- Risks / Failure Modes: Over-styling log rows can reduce high-density efficiency.
- Accessibility / Readability Notes: Severity and current-row cues must remain readable without relying solely on hue.
- Theme / Styling Notes: Output backgrounds and log accents require careful contrast control in dark themes.
- Motion / Interaction Notes where relevant: Auto-scroll and new-entry visual cues should be subtle and not visually jumpy.
- Observability / Diagnostics Notes where relevant: Add long-output screenshot sets and channel-state fixtures.
- Rollback / Safety Notes: Preserve the utility-first character of logs while increasing polish.
- References / Context: Utility panels deserve the same design care as content surfaces.
- Example scenarios where useful: A build log with warnings and informational lines remains scannable without looking like a wall of undifferentiated text.

### P12-T03

- Phase ID: P12
- Task ID: P12-T03
- Task Title: Refine Problems, Build, and Debug panel severity hierarchy and action-row clarity
- Priority: P0
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Improve visual trustworthiness and readability in panels centered on actionable technical state.
- Why This Matters Now: Problems and build/debug panels are where users decide whether the product feels like a serious IDE.
- UI Problem Statement: [ProblemsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp), [BuildPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp), [DebugConsolePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp), and related surfaces need stronger severity and hierarchy design.
- User Experience Impact: Errors, warnings, stack traces, and execution states can feel visually rough or inconsistently urgent.
- Scope: Severity rows, icons, counts, current-item treatment, stack-trace formatting, action buttons, result grouping, build status strips.
- Out of Scope: Problem navigation or debug correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuildOutputParser.cpp`
- Related Screens / Components / Surfaces: Problems panel, build panel, debug console, stack views.
- Current UI Behavior / Appearance: These panels are useful but not yet consistently high-finish or severity-balanced.
- Intended UI Behavior / Appearance: Technical state panels become easier to scan and more visually trustworthy.
- Visual / Interaction Design Direction: Dense operational dashboards with well-disciplined urgency and grouping.
- Technical Styling Approach: Reuse diagnostics severity and log-row systems while defining panel-specific metadata and stack-trace variants.
- Implementation Steps: Audit severity visuals; refine grouping and counts; improve stack and trace formatting; align action-row controls; normalize empty and success states.
- Validation Steps: Review error-heavy, warning-heavy, and all-clear states across Problems, Build, and Debug panels.
- Acceptance Criteria: Severity-heavy panels feel more controlled, readable, and premium.
- Dependencies: Phase 08, Phase 14, Phase 15.
- Risks / Failure Modes: Too much visual softness can reduce urgency for true failures.
- Accessibility / Readability Notes: Error and warning distinction must remain strong for color-blind users and low-vision conditions.
- Theme / Styling Notes: Red, amber, and success cues need especially careful calibration.
- Motion / Interaction Notes where relevant: Live build/debug updates should not produce distracting churn.
- Observability / Diagnostics Notes where relevant: Add severity-state and build/debug panel screenshot boards.
- Rollback / Safety Notes: Preserve current severity semantics and grouping logic during visual redesign.
- References / Context: These panels are daily credibility surfaces for professional development workflows.
- Example scenarios where useful: A panel showing one failed build, multiple warnings, and actionable rows remains clean and highly legible.

### P12-T04

- Phase ID: P12
- Task ID: P12-T04
- Task Title: Redesign source control, git log, and run/debug configuration surfaces for stronger metadata hierarchy
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make version-control and run-configuration surfaces visually richer and easier to parse.
- Why This Matters Now: Metadata-heavy panels are where weak hierarchy and poor density become especially visible.
- UI Problem Statement: [SourceControlPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp), [GitLogPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp), and [RunConfigDropdown.cpp](/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.cpp) need a more polished visual grammar.
- User Experience Impact: VCS and run-config workflows feel less premium and less glanceable.
- Scope: Change rows, staged/unstaged grouping, commit metadata, branch indicators, dropdown shells, run target cards, inline actions, diff badges.
- Out of Scope: Git and run-config semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.cpp`; `/Users/ryanrentfro/code/markamp/src/core/RunConfigService.cpp`
- Related Screens / Components / Surfaces: Source control panel, Git log, run config dropdowns, branch metadata surfaces.
- Current UI Behavior / Appearance: These surfaces contain useful information but do not yet maximize hierarchy, metadata readability, or polished action framing.
- Intended UI Behavior / Appearance: VCS and run-config surfaces feel like crisp operational dashboards rather than raw lists.
- Visual / Interaction Design Direction: Dense metadata presentation with clear status grouping and premium inline actions.
- Technical Styling Approach: Apply shared list/table metadata rules and command-surface refinements to VCS and run-config families.
- Implementation Steps: Audit row and card designs; improve metadata spacing and typography; refine badges and action icons; align dropdown and panel variants.
- Validation Steps: Review source-control states with multiple file groups and git log states with dense metadata.
- Acceptance Criteria: VCS and run-config surfaces feel noticeably more organized and premium.
- Dependencies: Phase 01, Phase 04, Phase 15.
- Risks / Failure Modes: Too much metadata hierarchy can make rows visually over-engineered.
- Accessibility / Readability Notes: File-state and branch-state cues must remain readable without relying only on color.
- Theme / Styling Notes: Status badges and metadata separators need careful opacity tuning.
- Motion / Interaction Notes where relevant: Dropdown and row-action transitions should remain crisp and understated.
- Observability / Diagnostics Notes where relevant: Add VCS and run-config screenshot fixtures.
- Rollback / Safety Notes: Preserve current data presentation order while style and hierarchy are improved.
- References / Context: Professional IDE quality depends heavily on metadata-dense side panels looking disciplined.
- Example scenarios where useful: A Git log with commit metadata and branch markers feels elegant rather than cramped.

### P12-T05

- Phase ID: P12
- Task ID: P12-T05
- Task Title: Improve bottom-panel tab strip, local headers, and cross-panel visual consistency
- Priority: P1
- Category: Panel UI
- Atomic Improvements Covered: 60
- Objective: Make the bottom panel feel like a coherent workbench zone rather than a collection of unrelated utilities.
- Why This Matters Now: Utility panels should still feel like one integrated part of the product.
- UI Problem Statement: Bottom-panel host chrome is shared, but content panels still vary too much in local shell feel.
- User Experience Impact: Switching among utility panels can feel like moving between mini-tools instead of remaining inside one polished workbench.
- Scope: Bottom panel tab strip, shared header rhythm, local toolbar style, panel-region background, region separators, empty and no-content states.
- Out of Scope: Panel lifecycle logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PanelTabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: Bottom panel host, utility panel region, tab strip.
- Current UI Behavior / Appearance: Shared host chrome exists, but local variation weakens the sense of one family.
- Intended UI Behavior / Appearance: The bottom panel feels like one premium region with specialized content inside.
- Visual / Interaction Design Direction: Unified utility-workbench zone with crisp regional identity.
- Technical Styling Approach: Tighten bottom-panel host tokens and align local panel shells to that host more consistently.
- Implementation Steps: Audit bottom-panel host and panel-local shells; refine tab strip and header rhythm; normalize region separators and background layering; align empty/sparse states.
- Validation Steps: Switch among Terminal, Output, Problems, Build, and Debug panels and compare visual continuity.
- Acceptance Criteria: Bottom-panel switching no longer feels like switching among different UI families.
- Dependencies: Phase 03, Phase 12 panel-family tasks.
- Risks / Failure Modes: Too much host emphasis can flatten useful panel distinctions.
- Accessibility / Readability Notes: Host and active-tab cues must remain obvious for keyboard users.
- Theme / Styling Notes: Region background contrast must separate the bottom area from the editor without becoming heavy.
- Motion / Interaction Notes where relevant: Tab switching and panel activation transitions should be quiet and stable.
- Observability / Diagnostics Notes where relevant: Add bottom-panel region screenshots with multiple active tabs.
- Rollback / Safety Notes: Preserve current host functionality and sizing while visual unification proceeds.
- References / Context: Shared region quality is as important as individual panel quality.
- Example scenarios where useful: Jumping from Terminal to Problems to Build still feels like staying inside one expertly designed utility zone.

### P12-T06

- Phase ID: P12
- Task ID: P12-T06
- Task Title: Add utility-panel family visual galleries and regression coverage
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect utility-panel polish with explicit family-wide references.
- Why This Matters Now: Utility panels are dense, varied, and easy to neglect once they look “good enough.”
- UI Problem Statement: There is no single board comparing terminal, output, problems, build, debug, and source-control surface quality.
- User Experience Impact: Visual debt can quietly re-enter these high-frequency work areas.
- Scope: Terminal, output, problems, build, debug console, source control, git log, run config, bottom-host chrome.
- Out of Scope: Deep feature correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: Utility panel family.
- Current UI Behavior / Appearance: Utility-panel quality is reviewed panel-by-panel rather than comparatively.
- Intended UI Behavior / Appearance: Utility-panel families are referenceable and regression-gated as one product area.
- Visual / Interaction Design Direction: Comparative utility-panel governance.
- Technical Styling Approach: Build fixture datasets and screenshot suites that cover dense, sparse, error, and active states for each utility panel.
- Implementation Steps: Define representative panel fixtures; capture state boards; publish comparisons; wire regression checks for critical panels.
- Validation Steps: Perturb terminal typography or Problems severity visuals and verify the gallery/regression stack catches it.
- Acceptance Criteria: Utility-panel polish becomes durable and easier to maintain.
- Dependencies: P12-T01 through P12-T05.
- Risks / Failure Modes: Too much fixture volume can make review heavy unless grouped by panel family and use case.
- Accessibility / Readability Notes: Include focus, compact density, and high-contrast panel captures.
- Theme / Styling Notes: Capture all utility panels across dark, light, and high-contrast theme modes.
- Motion / Interaction Notes where relevant: Include representative live-update and active-tab states.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with panel ID, data density, and severity mix.
- Rollback / Safety Notes: Keep the gallery additive while panel improvements land progressively.
- References / Context: Utility surfaces are part of everyday use and deserve equal polish governance.
- Example scenarios where useful: A change that improves Output but harms Problems row clarity is visible immediately in the utility-panel board.

