# Phase 07: Syntax Highlighting Code Readability And Token Presentation

## Outcome

Make code color hierarchy, semantic emphasis, comment treatment, markdown styling, and embedded-language presentation feel elite and readable across editor, notebook, canvas, and preview surfaces.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P07-T01

- Phase ID: P07
- Task ID: P07-T01
- Task Title: Recalibrate syntax-token hierarchy for premium readability and lower visual noise
- Priority: P0
- Category: Syntax Highlighting Presentation
- Atomic Improvements Covered: 60
- Objective: Improve code readability by making token distinctions more purposeful, balanced, and theme-aware.
- Why This Matters Now: Syntax highlighting is central to the user’s daily visual experience of the product.
- UI Problem Statement: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp), [ThemeTokens.h](/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h), [TokenThemeMapper.cpp](/Users/ryanrentfro/code/markamp/src/core/TokenThemeMapper.cpp), and [ThemeScopeMapper.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp) show ambition, but the visible token hierarchy still needs stronger curation.
- User Experience Impact: Code can read as too flat, too markdown-centric, or too locally tuned rather than expertly balanced.
- Scope: Keyword/operator/function/type/number/string/comment hierarchy, saturation balance, weight relationships, semantic highlight emphasis, default foreground cadence.
- Out of Scope: Language-server correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/core/TokenThemeMapper.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp`
- Related Screens / Components / Surfaces: Code editor, code cells, embedded code blocks, code previews.
- Current UI Behavior / Appearance: Token colors exist, but hierarchy and semantic emphasis still lean on local tuning and incomplete mapping.
- Intended UI Behavior / Appearance: Code feels easier to parse, calmer to read, and more intentionally color-ranked.
- Visual / Interaction Design Direction: Premium restrained syntax color language that emphasizes structure over novelty.
- Technical Styling Approach: Expand token hierarchy rules, retune mapping intensity, and create theme calibration matrices for high-frequency languages and markdown.
- Implementation Steps: Audit current token usage; define hierarchy goals; retune token weights and contrast; align mapper output with the new hierarchy; verify across representative languages.
- Validation Steps: Review code samples of multiple languages under dark, light, and high-contrast themes for structural readability.
- Acceptance Criteria: Token families are more distinguishable where needed and quieter where not.
- Dependencies: Phase 01, Phase 18.
- Risks / Failure Modes: Over-calibration for one language family can hurt another if the hierarchy is not broad enough.
- Accessibility / Readability Notes: Token distinctions must not rely solely on hue; contrast and luminosity differences matter.
- Theme / Styling Notes: Different theme modes need separate calibration, not one universal saturation recipe.
- Motion / Interaction Notes where relevant: None beyond ensuring token changes do not conflict with selection and search overlays.
- Observability / Diagnostics Notes where relevant: Add reference code snippets and theme screenshots for token review.
- Rollback / Safety Notes: Ship mapper changes behind theme calibration review rather than one giant unreviewed flip.
- References / Context: `v20` identified syntax quality as a major weakness; `v22` addresses its visual side in depth.
- Example scenarios where useful: A C++ file and a markdown file with embedded code both read clearly without either feeling oversaturated.

### P07-T02

- Phase ID: P07
- Task ID: P07-T02
- Task Title: Improve comment, doc-comment, string, and literal styling for readability and tone
- Priority: P1
- Category: Syntax Highlighting Presentation
- Atomic Improvements Covered: 60
- Objective: Make secondary and narrative token classes more readable without flattening structural code cues.
- Why This Matters Now: Comments and literals dominate long reading sessions and strongly influence eye comfort.
- UI Problem Statement: Current comment and literal presentation is tokenized but not yet holistically tuned for readability and emotional tone.
- User Experience Impact: Comments may become too faint, strings too loud, or documentation blocks too hard to scan.
- Scope: Comments, doc comments, strings, regex, escapes, numbers, constants, markdown emphasis inside code-adjacent contexts.
- Out of Scope: Parser or lexer changes outside visible mapping.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/core/TokenThemeMapper.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: Code editor, notebook code cells, preview/code fences.
- Current UI Behavior / Appearance: Narrative token classes exist but are not yet tuned as part of one reading-comfort strategy.
- Intended UI Behavior / Appearance: Comments and literals support scanning, comprehension, and reduced fatigue.
- Visual / Interaction Design Direction: Softer narrative channels that remain legible and dignified.
- Technical Styling Approach: Add calibration rules for low-frequency token families and their interactions with default foreground.
- Implementation Steps: Review comment and literal families by theme mode; retune contrast and saturation; align doc-comment styles with readability goals; verify escape and regex differentiation.
- Validation Steps: Compare code samples with dense comments, documentation blocks, JSON, and string-heavy sections.
- Acceptance Criteria: Secondary token classes are easier to read and less likely to disappear or shout.
- Dependencies: P07-T01.
- Risks / Failure Modes: Improving comments too much can make them compete with executable code.
- Accessibility / Readability Notes: Comments must remain readable for low-vision users and not depend on subtle hue shifts.
- Theme / Styling Notes: Light themes often need different contrast tuning than dark themes for comments and strings.
- Motion / Interaction Notes where relevant: None.
- Observability / Diagnostics Notes where relevant: Include comment-heavy fixture files in the syntax review suite.
- Rollback / Safety Notes: Keep token mappings reversible per theme while calibration settles.
- References / Context: Comment and literal styling often determine long-session comfort more than headline token colors do.
- Example scenarios where useful: Dense explanatory comments remain readable without overpowering the code around them.

### P07-T03

- Phase ID: P07
- Task ID: P07-T03
- Task Title: Elevate markdown, code-fence, and mixed-language styling across text and code surfaces
- Priority: P0
- Category: Syntax Highlighting Presentation
- Atomic Improvements Covered: 60
- Objective: Make markdown-driven and mixed-content files feel as intentional as pure code editors.
- Why This Matters Now: MarkAmp still carries strong markdown heritage, and mixed-content presentation is part of its identity.
- UI Problem Statement: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) configures extensive markdown styles, but the visual contract between prose, headings, code fences, links, quotes, and embedded languages still needs premium refinement.
- User Experience Impact: Mixed-content documents can feel less balanced than either pure prose or pure code.
- Scope: Headings, inline code, fenced code blocks, blockquotes, links, lists, task markers, tables, embedded language blocks, markdown-in-notebooks.
- Out of Scope: Markdown parser feature expansion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/rendering`; `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`
- Related Screens / Components / Surfaces: Markdown editor, preview, notebook markdown cells, code fences.
- Current UI Behavior / Appearance: Rich markdown styling exists, but prose/code hierarchy and mixed-language transitions still need stronger finish.
- Intended UI Behavior / Appearance: Mixed documents feel editorially excellent and technically precise at the same time.
- Visual / Interaction Design Direction: Premium mixed-content presentation with clear mode shifts between narrative and executable content.
- Technical Styling Approach: Define mixed-content token and layout rules shared by editor, preview, and notebook markdown surfaces.
- Implementation Steps: Audit markdown visual grammar; retune heading and link hierarchy; improve code-fence framing; align prose and code color balance; tune task and table visuals.
- Validation Steps: Review long markdown documents, technical notes with code fences, and notebook markdown cells in multiple themes.
- Acceptance Criteria: Mixed-content surfaces feel cohesive, readable, and premium rather than like layered defaults.
- Dependencies: Phase 06, Phase 09, Phase 18.
- Risks / Failure Modes: Over-designing markdown can make prose feel themed rather than professional.
- Accessibility / Readability Notes: Link and emphasis styling must remain distinguishable without depending on subtle hue shifts alone.
- Theme / Styling Notes: Markdown color hierarchy should adapt by theme while preserving structural relationships.
- Motion / Interaction Notes where relevant: Inline interactions like link hover should remain subtle.
- Observability / Diagnostics Notes where relevant: Add mixed-content reference documents to the visual QA suite.
- Rollback / Safety Notes: Keep prose-first defaults calm even as code-fence styling becomes more expressive.
- References / Context: MarkAmp’s product identity benefits directly from excellent mixed-content presentation.
- Example scenarios where useful: A notebook markdown cell with code fences looks as intentionally styled as a dedicated documentation editor.

### P07-T04

- Phase ID: P07
- Task ID: P07-T04
- Task Title: Improve semantic overlays for search hits, diagnostics, word highlights, and execution cues
- Priority: P1
- Category: Syntax Highlighting Presentation
- Atomic Improvements Covered: 60
- Objective: Make non-token overlays coexist with syntax colors without creating visual mud.
- Why This Matters Now: Search, diagnostics, execution markers, and word highlights are critical but often visually overbearing.
- UI Problem Statement: [EditorPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) defines many indicators and overlays whose colors and alpha values need more systematic calibration.
- User Experience Impact: Important transient information can become either too weak or too visually aggressive.
- Scope: Find hits, current match, diagnostics underlines, hint indicators, execution markers, word highlights, frontmatter/task indicators, trailing whitespace, quick-fix affordances.
- Out of Scope: Backend search or diagnostics logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`; `/Users/ryanrentfro/code/markamp/src/ui/OverviewRulerPanel.cpp`
- Related Screens / Components / Surfaces: Editor overlays, notebook code cells, preview cues.
- Current UI Behavior / Appearance: Many overlays exist, but their interaction with syntax colors is not yet globally tuned.
- Intended UI Behavior / Appearance: Overlays convey urgency and interaction state cleanly without overwhelming the code.
- Visual / Interaction Design Direction: Layered information with precise emphasis and low visual mud.
- Technical Styling Approach: Introduce overlay hierarchy rules and alpha/contrast bands separate from base token hierarchy.
- Implementation Steps: Inventory overlays; rank urgency and persistence; retune alpha and stroke patterns; unify diagnostic and search overlays; verify coexistence with selection and active line states.
- Validation Steps: Review files containing search hits, errors, warnings, hints, and execution states simultaneously.
- Acceptance Criteria: Overlay stacks remain readable and visually disciplined under compound states.
- Dependencies: Phase 06, Phase 18.
- Risks / Failure Modes: Too much restraint can cause important errors to disappear inside dense syntax.
- Accessibility / Readability Notes: Error and warning cues must remain legible for color-blind users through shape or stroke differences.
- Theme / Styling Notes: Overlay intensity must be theme-calibrated and not copied blindly across dark and light modes.
- Motion / Interaction Notes where relevant: Execution-state flashes or pulses must not compete with error cues.
- Observability / Diagnostics Notes where relevant: Add compound-state screenshots for overlay tuning.
- Rollback / Safety Notes: Keep user-visible severity distinctions intact while overlay hierarchy is rebalanced.
- References / Context: The editor already supports many overlay types; their visual coexistence is now the main problem.
- Example scenarios where useful: A selected line with a warning underline and a find hit still reads clearly.

### P07-T05

- Phase ID: P07
- Task ID: P07-T05
- Task Title: Define syntax presentation degradation paths for large files and low-power states
- Priority: P2
- Category: Syntax Highlighting Presentation
- Atomic Improvements Covered: 60
- Objective: Ensure degraded highlight modes still look intentionally designed rather than abruptly unfinished.
- Why This Matters Now: Large-file fallback quality is part of UI quality, not just performance engineering.
- UI Problem Statement: Earlier assessment work noted that large-file behavior can fall back to simpler modes; visually, those degradations need a first-class design.
- User Experience Impact: Users opening large files can feel like the product suddenly loses quality and trust.
- Scope: Large-file color simplification, minimap fallback visuals, disabled expensive overlays, progressive highlight states, degraded-but-readable modes.
- Out of Scope: Performance engine internals beyond visible consequences.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HighlightDegradation.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`
- Related Screens / Components / Surfaces: Large-file editor, minimap, overview, code cells if applicable.
- Current UI Behavior / Appearance: Degraded modes prioritize performance but not always visible polish.
- Intended UI Behavior / Appearance: Large-file states still look deliberate, calm, and readable.
- Visual / Interaction Design Direction: Graceful degradation rather than abrupt stylistic collapse.
- Technical Styling Approach: Define simplified highlight palettes and reduced-information states that remain coherent with the main editor.
- Implementation Steps: Audit fallback visuals; design degraded token subsets; simplify overlays and minimap elegantly; signal reduced-detail mode without panic UI.
- Validation Steps: Open large files and inspect highlight, minimap, and overlay appearance under degraded modes.
- Acceptance Criteria: Large-file editing still feels intentional and premium enough to maintain trust.
- Dependencies: P07-T01 through P07-T04.
- Risks / Failure Modes: Over-signaling degraded mode could make performance optimizations feel like failures.
- Accessibility / Readability Notes: Degraded modes must preserve baseline contrast and structure.
- Theme / Styling Notes: Simplified palettes need separate dark/light calibration.
- Motion / Interaction Notes where relevant: Reduced-motion and degraded rendering states should cooperate rather than stack awkwardly.
- Observability / Diagnostics Notes where relevant: Add fixture files and screenshots for degraded-mode review.
- Rollback / Safety Notes: Maintain safe visual fallbacks if performance constraints force simpler rendering on some systems.
- References / Context: Performance-driven fallback quality is part of elite UI polish.
- Example scenarios where useful: A 20,000-line file opens with a simpler but still handsome syntax presentation instead of a bare-looking fallback.

### P07-T06

- Phase ID: P07
- Task ID: P07-T06
- Task Title: Add syntax-presentation calibration suites and theme review boards
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Make syntax-quality decisions reviewable across themes, languages, and content types.
- Why This Matters Now: Syntax color tuning is notoriously subjective without structured review assets.
- UI Problem Statement: Token choices and overlay tuning need consistent evaluation across many languages and theme modes.
- User Experience Impact: Inconsistent syntax tuning makes the editor feel unstable and less premium over time.
- Scope: Reference language fixtures, theme matrices, mixed-content fixtures, large-file degraded fixtures, notebook/canvas embed fixtures.
- Out of Scope: Runtime theme marketplace management.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/TokenThemeMapper.cpp`
- Related Screens / Components / Surfaces: Editor, notebook cells, canvas code blocks, preview/code fences.
- Current UI Behavior / Appearance: Syntax tuning decisions are hard to compare systematically.
- Intended UI Behavior / Appearance: The team can review syntax quality with representative content and explicit visual criteria.
- Visual / Interaction Design Direction: Operationally disciplined syntax-quality management.
- Technical Styling Approach: Create a syntax review board with code fixtures, theme matrices, and screenshot comparisons tied to token mappings.
- Implementation Steps: Define fixture documents; capture screenshots for key themes and languages; publish review criteria; integrate into the visual QA workflow.
- Validation Steps: Intentionally perturb keyword or comment intensity and verify the review board exposes the regression.
- Acceptance Criteria: Syntax-presentation quality becomes reviewable and maintainable across the full product.
- Dependencies: P07-T01 through P07-T05, Phase 18.
- Risks / Failure Modes: Too many fixtures can dilute focus if the review board is not organized by decision type.
- Accessibility / Readability Notes: Include low-vision and color-blind review criteria in the syntax board.
- Theme / Styling Notes: Every syntax review must include dark and light baselines at minimum.
- Motion / Interaction Notes where relevant: None.
- Observability / Diagnostics Notes where relevant: Store fixture metadata with token families and theme IDs.
- Rollback / Safety Notes: Keep the review board additive and versioned as themes evolve.
- References / Context: Syntax highlighting is too central to be tuned opportunistically.
- Example scenarios where useful: A theme change that improves markdown but harms C++ readability is visible immediately in the syntax review board.

