# Phase 18: Theme Parity Token Application And Surface Wide Styling Correctness

## Outcome

Make light, dark, and high-contrast themes feel equally complete by expanding token coverage, correcting surface-specific theme drift, and calibrating built-in themes against the full UI.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P18-T01

- Phase ID: P18
- Task ID: P18-T01
- Task Title: Expand theme-token coverage to eliminate local hardcoded colors and styling escape hatches
- Priority: P0
- Category: Color / Theme
- Atomic Improvements Covered: 60
- Objective: Ensure every major visible surface can resolve its styling from the theme system rather than local literals.
- Why This Matters Now: Theme parity is impossible while many surfaces still style themselves locally.
- UI Problem Statement: Files such as [StartupPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp), [ThemeGallery.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp), [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), and others still hardcode visible styling decisions.
- User Experience Impact: Themes feel incomplete, and visual quality varies by surface.
- Scope: New theme tokens, token mapping, shell surfaces, panel surfaces, feedback surfaces, onboarding surfaces, canvas shell, advanced settings tools.
- Out of Scope: Theme marketplace logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: Entire app, especially legacy or locally styled surfaces.
- Current UI Behavior / Appearance: Theme coverage is good in some core areas but incomplete product-wide.
- Intended UI Behavior / Appearance: All visible surfaces participate correctly in the active theme.
- Visual / Interaction Design Direction: Token-complete theming with minimal local exceptions.
- Technical Styling Approach: Audit hardcoded style usage, add missing tokens, map them through the theme engine, and remove local color literals.
- Implementation Steps: Inventory local color/font/style literals; add token coverage; update surfaces to consume tokens; document approved exception cases; remove stale local fallback styling.
- Validation Steps: Switch themes across the product and compare visual consistency before and after token expansion.
- Acceptance Criteria: No major visible surface depends on obvious hardcoded styling when a token should exist.
- Dependencies: Phase 01.
- Risks / Failure Modes: Token proliferation can become unmanageable if not grouped by semantic role.
- Accessibility / Readability Notes: New tokens must support contrast and focus requirements, not only visual variety.
- Theme / Styling Notes: This task is the foundational theme-completeness pass for `v22`.
- Motion / Interaction Notes where relevant: Token expansion should cover animated state colors and overlay treatments where applicable.
- Observability / Diagnostics Notes where relevant: Add audits for local style literals and missing token usages.
- Rollback / Safety Notes: Keep compatibility token aliases during migration to avoid breaking existing themes abruptly.
- References / Context: Theme infrastructure exists; `v22` is about finishing coverage and parity.
- Example scenarios where useful: Startup and canvas surfaces finally respond correctly to theme changes instead of retaining local styling.

### P18-T02

- Phase ID: P18
- Task ID: P18-T02
- Task Title: Calibrate built-in themes across shell, editor, notebook, canvas, panels, and transient surfaces
- Priority: P0
- Category: Color / Theme
- Atomic Improvements Covered: 60
- Objective: Ensure each built-in theme feels intentionally tuned across the whole product, not only a subset of surfaces.
- Why This Matters Now: Themes are only as good as their weakest visible surface.
- UI Problem Statement: Built-in theme infrastructure is rich, but not every visible surface is calibrated equally across shell, editor, notebook, canvas, dialogs, and data views.
- User Experience Impact: Users experience theme quality as inconsistent or incomplete.
- Scope: Built-in theme review, shell/editor parity, panel parity, notebook/canvas parity, sparse-state parity, feedback parity.
- Out of Scope: Marketplace curation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- Related Screens / Components / Surfaces: Entire themed UI, built-in theme library, theme preview surfaces.
- Current UI Behavior / Appearance: Some themes likely look strong in core shell/editor surfaces but weaker in newer or more specialized areas.
- Intended UI Behavior / Appearance: Each shipped theme feels like a complete theme, not a partially mapped one.
- Visual / Interaction Design Direction: Whole-product theme integrity instead of shell-only theme quality.
- Technical Styling Approach: Build whole-product theme review boards and calibrate token relationships by theme family.
- Implementation Steps: Audit major built-in themes; review them across every major surface family; retune tokens where parity breaks; document theme-specific adjustments and constraints.
- Validation Steps: Use representative dark, light, stylized, and high-contrast built-in themes across the entire product surface set.
- Acceptance Criteria: Built-in themes exhibit whole-product visual coherence, not partial coverage.
- Dependencies: P18-T01, Phase 06 through Phase 16 galleries.
- Risks / Failure Modes: Tuning one theme family can unintentionally degrade another if shared tokens are over-relied upon.
- Accessibility / Readability Notes: Each built-in theme must be reviewed for contrast and focus visibility, not only aesthetics.
- Theme / Styling Notes: This task explicitly calibrates theme parity across the full UI.
- Motion / Interaction Notes where relevant: Animated surfaces must also feel right under each built-in theme.
- Observability / Diagnostics Notes where relevant: Add whole-product theme boards for a curated subset of flagship themes.
- Rollback / Safety Notes: Tune themes incrementally and keep before/after comparisons for regression review.
- References / Context: MarkAmp’s theme ambition is already broad; `v22` must make it credible everywhere.
- Example scenarios where useful: A stylized neon theme still looks good in settings, toasts, canvas, and startup, not only in the editor.

### P18-T03

- Phase ID: P18
- Task ID: P18-T03
- Task Title: Achieve true light-theme and high-contrast parity rather than dark-theme-first styling
- Priority: P0
- Category: Color / Theme
- Atomic Improvements Covered: 60
- Objective: Remove the common pattern where dark mode feels “finished” and other theme modes feel secondary.
- Why This Matters Now: Premium theme quality requires parity, not just variety.
- UI Problem Statement: Many custom-painted surfaces are easiest to tune in dark mode first and may still lag in light or high-contrast modes.
- User Experience Impact: Users who prefer or require light/high-contrast themes receive a lower-quality experience.
- Scope: Shell chrome, dense lists, dialogs, editor overlays, notebook/canvas surfaces, onboarding, feedback, sparse states in light and high-contrast modes.
- Out of Scope: Adding new themes.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/resources/themes/v2/light_base.yaml`; `/Users/ryanrentfro/code/markamp/resources/themes/v2/hc_base.yaml`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Screens / Components / Surfaces: All custom-painted UI.
- Current UI Behavior / Appearance: Light and high-contrast quality likely trails dark-mode polish on some custom surfaces.
- Intended UI Behavior / Appearance: Light and high-contrast modes feel equally deliberate and complete.
- Visual / Interaction Design Direction: Full visual confidence in every supported mode, not just the default-heavy one.
- Technical Styling Approach: Build parity review checklists and token adjustments specifically for light and high-contrast modes.
- Implementation Steps: Run parity audits; identify dark-mode assumptions; retune layers, borders, and overlays for lighter backgrounds; refine high-contrast emphasis and focus states.
- Validation Steps: Review all major surface families in light and high-contrast modes with the same rigor used for dark mode.
- Acceptance Criteria: Light and high-contrast modes no longer reveal obvious “secondary mode” quality gaps.
- Dependencies: P18-T01, P18-T02, Phase 17.
- Risks / Failure Modes: Shared token changes can unintentionally flatten the distinctive quality of dark themes.
- Accessibility / Readability Notes: High-contrast review is especially important for focus, severity, and row-state legibility.
- Theme / Styling Notes: This task explicitly prioritizes parity across theme modes, not only token completeness.
- Motion / Interaction Notes where relevant: Hover and focus motion must remain visible in high-contrast mode without relying on soft fades.
- Observability / Diagnostics Notes where relevant: Add mode-parity boards for flagship surfaces.
- Rollback / Safety Notes: Preserve theme identity while fixing parity issues through targeted token adjustments.
- References / Context: A world-class IDE cannot quietly ship second-tier light or high-contrast experiences.
- Example scenarios where useful: The command palette and minimap look equally deliberate in light theme, not washed-out or unfinished.

### P18-T04

- Phase ID: P18
- Task ID: P18-T04
- Task Title: Upgrade theme preview, editing, import, and comparison UI for more trustworthy theme workflows
- Priority: P2
- Category: Color / Theme
- Atomic Improvements Covered: 60
- Objective: Make theme-management tooling visually reliable for both end users and internal designers.
- Why This Matters Now: Theme tooling is part of the UI story and should reinforce, not undermine, confidence in the theme system.
- UI Problem Statement: Theme gallery and editors are useful but need stronger preview accuracy and more premium comparison visuals.
- User Experience Impact: Users and contributors cannot evaluate theme quality as confidently as they should.
- Scope: Theme preview cards, comparison states, token previews, import warnings, before/after preview panes, theme validation feedback.
- Out of Scope: Theme import/export implementation details.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemePreviewCard.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeEditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeTokenEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeValidator.cpp`
- Related Screens / Components / Surfaces: Theme gallery, theme editor, token editor, comparison tools.
- Current UI Behavior / Appearance: Theme tooling exists but can still feel more utilitarian than flagship.
- Intended UI Behavior / Appearance: Theme workflows become highly trustworthy and visually compelling.
- Visual / Interaction Design Direction: Design-tool-grade theme management with accurate previews and clean editing surfaces.
- Technical Styling Approach: Extend theme-preview and settings-tool systems with stronger preview accuracy and comparison variants.
- Implementation Steps: Improve preview fidelity; redesign comparison layouts; add richer token swatches and previews; align warnings and import states with the feedback system.
- Validation Steps: Review multiple themes through gallery, editor, and import flows under representative preview surfaces.
- Acceptance Criteria: Theme-management workflows feel visibly reliable and polished.
- Dependencies: Phase 13, P18-T02.
- Risks / Failure Modes: Preview fidelity work can become expensive if too many surfaces are simulated at once.
- Accessibility / Readability Notes: Theme previews must remain interpretable without depending solely on tiny color swatches.
- Theme / Styling Notes: Theme tooling itself must obey theme parity standards.
- Motion / Interaction Notes where relevant: Preview transitions should be subtle and not obscure comparison.
- Observability / Diagnostics Notes where relevant: Add theme-tool screenshot boards with before/after preview states.
- Rollback / Safety Notes: Keep existing tool functionality stable while preview surfaces are upgraded.
- References / Context: Theme tooling quality directly affects trust in the entire visual system.
- Example scenarios where useful: Importing a theme shows a clear, elegant preview of how it will affect shell, editor, and panel surfaces.

### P18-T05

- Phase ID: P18
- Task ID: P18-T05
- Task Title: Add token-usage audits and surface-specific theme correctness checks
- Priority: P1
- Category: Color / Theme
- Atomic Improvements Covered: 60
- Objective: Detect theme drift mechanically instead of relying only on manual inspection.
- Why This Matters Now: The app is too large to maintain theme correctness by memory.
- UI Problem Statement: Missing token usage, surface-local overrides, and incomplete theme mapping can quietly reappear as UI work continues.
- User Experience Impact: Themes gradually feel less coherent and less trustworthy.
- Scope: Token audit reports, missing-token detection, local override detection, surface correctness reports, theme linting for custom-painted widgets.
- Out of Scope: Theme marketplace moderation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeValidator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeAutoRepair.cpp`; `/Users/ryanrentfro/code/markamp/src/ui`
- Related Screens / Components / Surfaces: All tokenized and custom-painted surfaces.
- Current UI Behavior / Appearance: Theme correctness is partly visible only through manual surface inspection.
- Intended UI Behavior / Appearance: Theme drift is easier to detect and prevent.
- Visual / Interaction Design Direction: Token-discipline governance for a premium UI system.
- Technical Styling Approach: Add audits for missing tokens, local literals, incomplete mappings, and surface-specific theme exceptions.
- Implementation Steps: Build token-usage inventory; detect local style literals; report missing or inconsistent tokens by surface family; wire audits into UI QA outputs.
- Validation Steps: Intentionally introduce a local color override and verify the theme audit flags it.
- Acceptance Criteria: Theme correctness becomes measurable and less dependent on manual code archaeology.
- Dependencies: P18-T01, Phase 20.
- Risks / Failure Modes: Audits can produce noise if semantic token ownership is not defined clearly enough.
- Accessibility / Readability Notes: Audit outputs should include contrast and focus-state implications where possible.
- Theme / Styling Notes: This task makes theme parity sustainable after the `v22` wave.
- Motion / Interaction Notes where relevant: Include checks for animated state color tokens where they exist.
- Observability / Diagnostics Notes where relevant: Publish token and theme-correctness reports alongside screenshot boards.
- Rollback / Safety Notes: Start with warning-level audits before promoting to stricter failure modes.
- References / Context: Theme systems only stay premium if they are enforceable.
- Example scenarios where useful: A hardcoded purple button in startup is flagged immediately as a theme-escape violation.

### P18-T06

- Phase ID: P18
- Task ID: P18-T06
- Task Title: Add whole-product theme review boards and parity gates
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Make theme parity and theme quality a visible release criterion across the whole interface.
- Why This Matters Now: Theme parity is one of the user’s explicit priorities for `v22`.
- UI Problem Statement: The product needs a curated board that shows representative surfaces across themes, not only isolated component captures.
- User Experience Impact: Theme regressions can ship unnoticed until users encounter them across real workflows.
- Scope: Whole-product theme boards, flagship surface journeys, dark/light/high-contrast parity captures, curated theme sets, theme-specific blocker reports.
- Out of Scope: Every marketplace theme.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInThemes.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`
- Related Screens / Components / Surfaces: Entire themed UI, curated representative journeys.
- Current UI Behavior / Appearance: Theme quality is reviewed surface by surface rather than as whole product experiences.
- Intended UI Behavior / Appearance: Theme parity is evaluated through realistic multi-surface journeys and release gates.
- Visual / Interaction Design Direction: Whole-product theme governance with curated experience boards.
- Technical Styling Approach: Capture representative shell/editor/panel/dialog/notebook/canvas journeys under multiple themes and compare them systematically.
- Implementation Steps: Define flagship theme boards; capture major surface journeys; publish parity reports; require review of unresolved theme blockers before release.
- Validation Steps: Introduce a theme regression in one surface family and verify the parity gate catches it in the journey board.
- Acceptance Criteria: Theme quality is reviewable at the level users actually experience the product.
- Dependencies: P18-T01 through P18-T05 and phase galleries from the rest of the `v22` package.
- Risks / Failure Modes: Boards can become unwieldy if too many themes or surfaces are included without curation.
- Accessibility / Readability Notes: Include accessible theme modes explicitly in the journey boards.
- Theme / Styling Notes: This task formalizes whole-product theme parity as a quality gate.
- Motion / Interaction Notes where relevant: Include representative transient and animated surfaces in curated theme journeys.
- Observability / Diagnostics Notes where relevant: Attach parity summaries and blocker counts to the theme board outputs.
- Rollback / Safety Notes: Begin with curated flagship themes before expanding coverage.
- References / Context: Premium theme quality is part of the product identity, not a cosmetic extra.
- Example scenarios where useful: A curated “editor + search + settings + canvas” board reveals a light-theme regression immediately.

