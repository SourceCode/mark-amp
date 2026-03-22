# Phase 10: Canvas Workspace Tools Object Chrome And Interaction Visuals

## Outcome

Transform canvas presentation from a functional shell into a premium visual workspace with elite tool chrome, object interaction cues, inspector treatment, and collaboration-grade polish.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P10-T01

- Phase ID: P10
- Task ID: P10-T01
- Task Title: Replace primitive canvas shell chrome with a premium workspace frame and toolbar system
- Priority: P0
- Category: Canvas UI
- Atomic Improvements Covered: 60
- Objective: Make the canvas root surface feel sophisticated, intentional, and product-grade.
- Why This Matters Now: The current canvas shell visibly lags the rest of the product’s ambition.
- UI Problem Statement: [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) still uses Unicode tool icons, basic buttons, a simple title strip, and a placeholder inspector/minimap layout.
- User Experience Impact: The canvas mode feels less premium and less trustworthy than the editor should.
- Scope: Canvas top bar, title area, tool rail shell, shell backgrounds, shell spacing, shell empty/loading states, shell-specific metadata strips.
- Out of Scope: Canvas model behavior or persistence logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp`
- Related Screens / Components / Surfaces: Canvas workspace, canvas root shell, tool zones, properties regions.
- Current UI Behavior / Appearance: Canvas shell is basic and visibly provisional in high-value areas.
- Intended UI Behavior / Appearance: The canvas workspace feels like a flagship visual mode equal to the editor and better than mainstream whiteboard tooling.
- Visual / Interaction Design Direction: Spacious, confident visual board environment with technically refined chrome.
- Technical Styling Approach: Define canvas shell tokens and custom component variants for the top bar, tool rail, inspector frame, and minimap dock.
- Implementation Steps: Replace placeholder icons; redesign canvas shell layout; align top bar and tool rail to the design system; retune shell spacing and layers; create canvas-specific empty/loading states.
- Validation Steps: Review canvas shell in empty, populated, and collaboration-rich scenarios across themes.
- Acceptance Criteria: Canvas no longer feels visually like a placeholder or side feature.
- Dependencies: Phase 01, Phase 02, Phase 18.
- Risks / Failure Modes: Over-chroming the canvas can reduce the sense of open creative space.
- Accessibility / Readability Notes: Tool controls and shell status cues must remain high-contrast and clearly targetable.
- Theme / Styling Notes: Canvas shell layering needs careful balancing so the board remains primary.
- Motion / Interaction Notes where relevant: Shell transitions should be subtle and not reduce spatial stability.
- Observability / Diagnostics Notes where relevant: Add canvas shell screenshots and mode captures to the UI atlas.
- Rollback / Safety Notes: Stage shell improvements independently from canvas model changes.
- References / Context: [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) is one of the clearest UI-quality gaps in the repository.
- Example scenarios where useful: Opening canvas mode immediately feels like entering a premium visual workspace rather than a utility panel.

### P10-T02

- Phase ID: P10
- Task ID: P10-T02
- Task Title: Rebuild canvas tool palettes, tool states, and zoom/navigation controls
- Priority: P0
- Category: Canvas UI
- Atomic Improvements Covered: 60
- Objective: Make canvas tools feel precise, discoverable, and visually refined.
- Why This Matters Now: Tool palettes are core to canvas usability and currently one of the most visibly underdesigned areas.
- UI Problem Statement: Tool descriptions in [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) still use Unicode placeholders and basic buttons, while richer canvas tooling exists elsewhere.
- User Experience Impact: Tool selection feels more basic and less confidence-inspiring than it should.
- Scope: Tool rail buttons, active tool states, hover/focus/pressed feedback, grouped tools, mode toggles, zoom controls, navigator surfaces, minimap interactions.
- Out of Scope: Tool command correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/MinimapPanel.cpp`
- Related Screens / Components / Surfaces: Tool rail, zoom controls, navigation widgets, canvas minimap.
- Current UI Behavior / Appearance: Tooling is visible, but not yet premium in iconography, grouping, or state expression.
- Intended UI Behavior / Appearance: Canvas tools look like high-end creative-software controls with excellent affordance clarity.
- Visual / Interaction Design Direction: Tight tool systems with confident state cues and low-clutter grouping.
- Technical Styling Approach: Create canvas-tool families that reuse icon and control-state systems while preserving mode specificity.
- Implementation Steps: Replace placeholder iconography; define tool group shells; refine state feedback; redesign zoom/nav controls; align minimap and navigator visuals.
- Validation Steps: Switch tools rapidly, zoom in/out, and compare idle/hover/active states across themes.
- Acceptance Criteria: Tooling is visually clear and feels purpose-built for creative work.
- Dependencies: Phase 01, Phase 19, Phase 20.
- Risks / Failure Modes: Overly dense tool grouping can reduce first-use discoverability.
- Accessibility / Readability Notes: Tool states and zoom levels must be readable without relying on subtle color shifts.
- Theme / Styling Notes: Tool icons and active states need strong contrast against varied canvas shell backgrounds.
- Motion / Interaction Notes where relevant: Tool activation and zoom interactions should feel responsive and tactile.
- Observability / Diagnostics Notes where relevant: Add canvas tool family screenshots and active/hover matrices.
- Rollback / Safety Notes: Keep keyboard shortcuts and tool semantics stable while tool visuals evolve.
- References / Context: Tool palette quality is central to whether canvas mode feels elite or provisional.
- Example scenarios where useful: Selecting the Connector tool feels as polished as selecting a premium vector tool in professional design software.

### P10-T03

- Phase ID: P10
- Task ID: P10-T03
- Task Title: Elevate object selection visuals, handles, snap guides, and spatial interaction cues
- Priority: P0
- Category: Canvas UI
- Atomic Improvements Covered: 60
- Objective: Make direct manipulation on the canvas feel precise, premium, and visually trustworthy.
- Why This Matters Now: Selection and transform cues are the core of perceived canvas quality.
- UI Problem Statement: Canvas interaction infrastructure exists, but the visible selection and manipulation language needs stronger design discipline.
- User Experience Impact: Object editing can feel rough, ambiguous, or less tactile than it should.
- Scope: Selection outlines, bounding boxes, resize handles, rotation handles, snap guides, hover previews, drag shadows, multi-selection framing.
- Out of Scope: Hit-testing or geometric correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp`
- Related Screens / Components / Surfaces: Board interactions, object selection, transform cues, guide overlays.
- Current UI Behavior / Appearance: Selection and manipulation capabilities exist, but visible cues are not yet at creative-tool premium quality.
- Intended UI Behavior / Appearance: Selection and transform states feel precise, readable, and satisfying.
- Visual / Interaction Design Direction: Technical precision with clear spatial confidence and restrained visual noise.
- Technical Styling Approach: Define selection and transform token families for strokes, fills, handles, guides, and drag states.
- Implementation Steps: Audit selection visuals; refine handle geometry; improve guide contrast and animation; align multi-selection treatment; calibrate hover and drag previews.
- Validation Steps: Manipulate single and grouped objects across zoom levels and dense boards.
- Acceptance Criteria: Canvas interactions feel materially more polished and easier to trust.
- Dependencies: Phase 01, Phase 19.
- Risks / Failure Modes: Overly prominent handles or guides can clutter the canvas at small scales.
- Accessibility / Readability Notes: Handles and guides must remain visible at multiple zoom levels and for low-vision users.
- Theme / Styling Notes: Selection and guide colors must work on both dark and light board backgrounds.
- Motion / Interaction Notes where relevant: Snap and drag feedback should feel responsive but not jumpy.
- Observability / Diagnostics Notes where relevant: Add interaction-state captures for selection, guide, and transform scenarios.
- Rollback / Safety Notes: Maintain object interaction hit areas while visual geometry is tuned.
- References / Context: Spatial interaction cues are where high-end whiteboard and canvas tools distinguish themselves most visibly.
- Example scenarios where useful: Snapping an object into alignment shows crisp guides and precise handles without overwhelming the board.

### P10-T04

- Phase ID: P10
- Task ID: P10-T04
- Task Title: Redesign canvas object chrome, inline editors, inspector surfaces, and metadata cards
- Priority: P1
- Category: Canvas UI
- Atomic Improvements Covered: 60
- Objective: Improve the visible treatment of canvas objects and supporting inspectors so the board feels premium even when densely populated.
- Why This Matters Now: Object chrome and inspector presentation determine whether canvas work feels serious or toy-like.
- UI Problem Statement: [CanvasInspector.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp), [InspectorModel.cpp](/Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp), [CanvasTextEditor.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp), and the placeholder inspector in [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp) need a real visual system.
- User Experience Impact: Object editing and metadata inspection feel less powerful and less polished than they should.
- Scope: Object shells, inline text editing, metadata cards, inspector rows, style swatches, layer cards, object badges, comments and labels.
- Out of Scope: Inspector logic and object schema correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/InspectorModel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasTextEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: Inspector sidebar, object labels, inline editors, metadata strips.
- Current UI Behavior / Appearance: The inspector and object-level editing surfaces are not yet visually mature enough.
- Intended UI Behavior / Appearance: Object chrome and inspector UIs feel sophisticated, readable, and integrated with the board.
- Visual / Interaction Design Direction: Clean creative-tool sidecar UI with excellent hierarchy and unobtrusive object metadata.
- Technical Styling Approach: Reuse settings/panel row systems where appropriate, with canvas-specific tokens for swatches, style chips, and object metadata.
- Implementation Steps: Redesign inspector layout; improve row spacing and grouping; style inline text editing; refine object metadata cards; align color and style controls visually.
- Validation Steps: Edit text, shapes, connectors, and metadata-heavy objects and compare inspector and inline-editing states.
- Acceptance Criteria: Inspector and object chrome no longer feel placeholder-backed or visually generic.
- Dependencies: Phase 01, Phase 11, Phase 13, Phase 15.
- Risks / Failure Modes: Too much inspector chrome can distract from the board itself.
- Accessibility / Readability Notes: Small style controls, swatches, and metadata need strong focus and contrast cues.
- Theme / Styling Notes: Inspector shells must balance board separation with shell cohesion.
- Motion / Interaction Notes where relevant: Inline editor activation and inspector focus changes should be smooth and predictable.
- Observability / Diagnostics Notes where relevant: Add object-edit and inspector-state screenshots to the canvas UI gallery.
- Rollback / Safety Notes: Preserve existing edit flows while presentation is upgraded.
- References / Context: Premium canvas tooling depends on both board interaction and supporting sidecar quality.
- Example scenarios where useful: Selecting a sticky note or diagram node yields a beautifully organized inspector rather than a placeholder properties panel.

### P10-T05

- Phase ID: P10
- Task ID: P10-T05
- Task Title: Improve canvas onboarding, collaboration overlays, and empty-board presentation
- Priority: P2
- Category: Canvas UI
- Atomic Improvements Covered: 60
- Objective: Make first-use and low-content canvas experiences feel aspirational and helpful.
- Why This Matters Now: Canvas mode is especially vulnerable to looking empty or underpowered when the board is blank.
- UI Problem Statement: Empty and low-content canvas states currently rely on minimal shell chrome and do not fully express the richness implied by the broader canvas subsystem.
- User Experience Impact: New users may perceive canvas mode as less complete than it is.
- Scope: Empty-board states, starter templates, collaboration indicators, remote cursor styling, minimap onboarding, help overlays, first-object affordances.
- Out of Scope: Collaboration protocol correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/RemoteCursorOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/RemoteSelectionOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp`
- Related Screens / Components / Surfaces: Empty boards, collaboration overlays, help overlays, first-use guidance.
- Current UI Behavior / Appearance: Canvas onboarding and collaboration visuals are not yet a cohesive premium family.
- Intended UI Behavior / Appearance: Empty or collaborative boards still feel rich, inviting, and intentional.
- Visual / Interaction Design Direction: Premium creative-space onboarding with subtle collaboration warmth and zero empty deadness.
- Technical Styling Approach: Define empty-board compositions, template entry cards, and collaboration overlay variants within the canvas visual language.
- Implementation Steps: Audit empty-board experience; design first-use states; refine collaboration cursor/selection visuals; improve template and starter affordances; align hint overlays.
- Validation Steps: Open a blank board, enter collaboration states, and test low-content and template-rich scenarios.
- Acceptance Criteria: Blank or lightly used boards still feel like polished product moments.
- Dependencies: Phase 10 shell work, Phase 16, Phase 19.
- Risks / Failure Modes: Onboarding overlays can feel promotional or distracting if too heavy-handed.
- Accessibility / Readability Notes: Remote-user and onboarding cues must remain legible without overwhelming board content.
- Theme / Styling Notes: Empty-board visuals need strong dark/light theme parity.
- Motion / Interaction Notes where relevant: Collaboration and onboarding motion should be subtle, informative, and reduced-motion aware.
- Observability / Diagnostics Notes where relevant: Add blank-board and collaboration-state visual references.
- Rollback / Safety Notes: Keep onboarding layers dismissible and non-blocking while iterating.
- References / Context: Canvas mode needs visible confidence in both empty and active states.
- Example scenarios where useful: Opening a blank canvas offers elegant starter cues without looking like a tutorial popup exploded onto the board.

### P10-T06

- Phase ID: P10
- Task ID: P10-T06
- Task Title: Add canvas UI galleries and interaction-state regression suites
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect canvas visual quality across shell, tools, board interactions, and inspectors.
- Why This Matters Now: Canvas UI has enough moving parts that visual drift will be easy without explicit review fixtures.
- UI Problem Statement: Canvas currently lacks a dedicated visual QA surface family equivalent to what the editor and shell need.
- User Experience Impact: Canvas polish can lag or regress even while other product areas improve.
- Scope: Shell states, tool states, selection states, drag states, inspector states, empty-board states, collaboration overlays, zoom levels.
- Out of Scope: Canvas engine correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp`
- Related Screens / Components / Surfaces: Entire canvas UI family.
- Current UI Behavior / Appearance: Canvas visual quality is not yet guarded by a dedicated reference board.
- Intended UI Behavior / Appearance: Canvas states are referenceable, comparable, and regression-gated.
- Visual / Interaction Design Direction: Operationally mature canvas design governance.
- Technical Styling Approach: Build fixture boards and screenshot suites that exercise the main canvas visual families.
- Implementation Steps: Define representative boards; capture shell and interaction states; publish galleries; add regression checks for tool, board, and inspector variants.
- Validation Steps: Intentionally perturb selection-handle or tool-rail styling and verify the regression suite catches the change.
- Acceptance Criteria: Canvas UI quality becomes durable rather than episodic.
- Dependencies: P10-T01 through P10-T05.
- Risks / Failure Modes: Fixture boards may miss important states if they are too simple or too specialized.
- Accessibility / Readability Notes: Include high-zoom, low-zoom, low-vision, and reduced-motion captures in the board set.
- Theme / Styling Notes: Cover canvas states on dark and light boards and under major app themes.
- Motion / Interaction Notes where relevant: Include drag and snap interaction-state captures.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with object counts, zoom levels, and state flags.
- Rollback / Safety Notes: Keep canvas QA additive while visual language is still evolving.
- References / Context: Canvas is one of the biggest opportunities for visible UI differentiation in MarkAmp.
- Example scenarios where useful: A regression that weakens snap-guide clarity is caught even though object rendering still technically works.

