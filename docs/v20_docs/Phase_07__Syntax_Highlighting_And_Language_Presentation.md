# Phase 07: Syntax Highlighting And Language Presentation

## Outcome

Upgrade MarkAmp from a markdown-leaning editor with partial highlighter infrastructure into a real IDE language presentation system with strong token quality, theme integration, and artifact-wide consistency.

## Improvement Count

210 atomic improvements across 6 execution tasks.

### P07-T01

- Phase ID: P07
- Task ID: P07-T01
- Task Title: Define one canonical language-resolution and tokenization contract
- Priority: P0
- Category: Syntax Highlighting
- Atomic Improvements Covered: 35
- Objective: Resolve the split between `wxStyledTextCtrl` lexers and the custom tokenization stack.
- Why This Matters Now: Syntax quality cannot become premium while the architecture is split.
- Problem Statement: `EditorPanel.cpp` is largely markdown-lexer driven while `SyntaxHighlighter.cpp` and `AsyncHighlighter.cpp` exist separately.
- User Impact: Language presentation quality is uneven and difficult to improve systematically.
- Scope: Language detection, file extension mapping, artifact metadata mapping, tokenizer selection, fallback policies.
- Out of Scope: Full LSP semantic-token implementation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`
- Related Systems / Components: Editor panel, syntax highlighter, async highlighting, themes.
- Current Behavior: The editor and custom token stack do not clearly share one source of truth.
- Intended Behavior: One language-resolution pipeline decides tokenization and styling everywhere.
- Technical Approach: Introduce a language service that maps artifact metadata to syntax and theme token streams.
- Implementation Steps: Define language contract; map file extensions and notebook cell languages; wire editor/notebook/markdown/canvas embeds; define graceful fallbacks.
- Validation Steps: Open multiple languages and verify the same language ID and token source drive all surfaces.
- Acceptance Criteria: Tokenization source and language choice are explicit, deterministic, and inspectable.
- Dependencies: Phase 01 and Phase 02.
- Risks / Failure Modes: Forcing one engine too early may regress languages currently handled acceptably by Scintilla.
- UX Notes: Users should be able to see and override detected language when needed.
- Styling / Highlighting Notes where relevant: Language choice must immediately affect theme token mapping and editor chrome.
- Observability / Diagnostics Notes: Add language-resolution traces and tokenization-source metrics.
- Rollback / Safety Notes: Allow staged per-language backend rollout.
- References / Context: `EditorPanel.cpp` currently calls `SetLexer(wxSTC_LEX_MARKDOWN)` directly.
- Example scenarios where useful: Opening `.cpp`, `.py`, `.json`, and markdown files uses one common language-selection contract.

### P07-T02

- Phase ID: P07
- Task ID: P07-T02
- Task Title: Raise baseline token quality and language coverage for core IDE languages
- Priority: P0
- Category: Syntax Highlighting
- Atomic Improvements Covered: 35
- Objective: Improve tokenization quality for C++, Python, JavaScript/TypeScript, JSON, YAML, shell, and markdown.
- Why This Matters Now: The product claims IDE ambition and must handle common code well.
- Problem Statement: Current highlighting evidence is too markdown-centric and too uneven for code-heavy work.
- User Impact: Poor colorization reduces readability, trust, and perceived quality.
- Scope: Keywords, strings, comments, numbers, operators, punctuation, preprocessor tokens, fenced-code tokens, notebook cell language mapping.
- Out of Scope: Full parsing correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GrammarEngine.h`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Systems / Components: Syntax highlighter, grammar layer, editor theme application.
- Current Behavior: Language support exists but is not clearly integrated or premium enough.
- Intended Behavior: Core languages look deliberate, readable, and consistent.
- Technical Approach: Improve tokenizer definitions, extension mapping, and fallback semantics for embedded code.
- Implementation Steps: Audit top languages; improve token classes; add regression files; align token names with theme scope rules.
- Validation Steps: Compare representative code samples in light and dark themes.
- Acceptance Criteria: Core language samples render with strong readability and expected distinctions.
- Dependencies: P07-T01.
- Risks / Failure Modes: Token overclassification creates noisy visuals.
- UX Notes: Syntax colors should aid comprehension, not create rainbow clutter.
- Styling / Highlighting Notes where relevant: Contrast and semantic consistency matter more than novelty.
- Observability / Diagnostics Notes: Count unsupported languages and fallback tokenization usage.
- Rollback / Safety Notes: Ship per-language overrides for problematic grammars.
- References / Context: `SyntaxHighlighter.cpp` already contains built-in language registration, but product integration remains weak.
- Example scenarios where useful: C++ templates, Python decorators, JSON keys, and markdown fenced code blocks all remain readable and distinct.

### P07-T03

- Phase ID: P07
- Task ID: P07-T03
- Task Title: Finish theme-token mapping from syntax tokens to visual presentation
- Priority: P0
- Category: Syntax Highlighting
- Atomic Improvements Covered: 35
- Objective: Connect tokenization output to the theme scope machinery already present in the repository.
- Why This Matters Now: Better tokenization is wasted if styling remains flat or inconsistent.
- Problem Statement: `ThemeScopeMapper` and `ThemeEngine` show intent, but surface use is incomplete.
- User Impact: Users see inconsistent colors across languages and surfaces.
- Scope: Token-to-scope mapping, semantic overrides, font-style mapping, fallback token colors, theme import compatibility.
- Out of Scope: Theme marketplace UX.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeScopeMapper.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/VsCodeThemeAdapter.cpp`
- Related Systems / Components: Theme engine, theme scope mapper, VS Code theme adapter, editor panel.
- Current Behavior: Theme machinery exists but is not clearly driving all syntax presentation.
- Intended Behavior: Syntax tokens map consistently into theme-driven styles across the app.
- Technical Approach: Normalize token classes and make theme scope mapping the authoritative style bridge.
- Implementation Steps: Define token class vocabulary; bind mapper to editor/notebook/canvas code surfaces; validate VS Code theme conversion fidelity.
- Validation Steps: Switch themes and compare token classes across editor and notebook cells.
- Acceptance Criteria: Theme changes preserve semantic consistency instead of random color shifts.
- Dependencies: P07-T01, P07-T02.
- Risks / Failure Modes: Theme mappings may become too lossy for imported themes.
- UX Notes: Theme identity should remain recognizable while improving consistency.
- Styling / Highlighting Notes where relevant: Support italics/bold carefully and legibly.
- Observability / Diagnostics Notes: Add scope-mapping inspection tools and unmapped-token counters.
- Rollback / Safety Notes: Keep fallback color tables for incomplete themes.
- References / Context: `ThemeEngine.cpp` rebuilds a `ThemeScopeMapper`, but editor use is not yet decisive.
- Example scenarios where useful: A VS Code theme import should style keywords similarly in text editor and notebook code cells.

### P07-T04

- Phase ID: P07
- Task ID: P07-T04
- Task Title: Improve large-file and mixed-language degradation paths without dropping to dead highlighting
- Priority: P1
- Category: Performance
- Atomic Improvements Covered: 35
- Objective: Keep big files usable while preserving as much syntax quality as possible.
- Why This Matters Now: Current large-file handling appears to disable highlighting completely.
- Problem Statement: `ApplyLargeFileOptimizations()` sets `wxSTC_LEX_NULL`.
- User Impact: Large files become visually poorer exactly when users need structure and orientation most.
- Scope: Incremental tokenization, partial highlight windows, budget-based highlighting, mixed-language block fallback, cache strategy.
- Out of Scope: Full AST indexing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IncrementalTokenizer.h`
- Related Systems / Components: Editor rendering, async highlighter, incremental tokenizer, performance monitor.
- Current Behavior: Large-file optimization sacrifices too much syntax quality.
- Intended Behavior: Degradation is graceful, visible, and still useful.
- Technical Approach: Use line-window tokenization, budgeted background work, and mixed-language block prioritization.
- Implementation Steps: Add highlight budgets; preserve essential tokens; prioritize viewport lines; add user setting and diagnostics.
- Validation Steps: Open large source files and mixed markdown/code docs and observe responsiveness and token quality.
- Acceptance Criteria: Large files remain performant without collapsing into flat text by default.
- Dependencies: P07-T01 through P07-T03.
- Risks / Failure Modes: Background tokenization churn may affect typing latency.
- UX Notes: If reduced highlighting is used, explain it subtly and honestly.
- Styling / Highlighting Notes where relevant: Preserve comments, keywords, strings, search hits, and diagnostics even in degraded mode.
- Observability / Diagnostics Notes: Track highlight latency, dropped regions, and degraded-mode activation.
- Rollback / Safety Notes: Provide hard-disable fallback for extreme cases.
- References / Context: `EditorPanel.cpp` currently uses `wxSTC_LEX_NULL` for large-file optimization.
- Example scenarios where useful: A 20k-line C++ file still shows readable syntax in the visible viewport.

### P07-T05

- Phase ID: P07
- Task ID: P07-T05
- Task Title: Bring syntax highlighting to notebook code cells, markdown fences, diffs, and canvas embedded code
- Priority: P1
- Category: Syntax Highlighting
- Atomic Improvements Covered: 35
- Objective: Eliminate surface-to-surface highlighting inconsistency for code content.
- Why This Matters Now: The app is multi-surface by design and code appearance must travel with content.
- Problem Statement: Notebook, markdown, diff, and canvas code presentation are likely diverging.
- User Impact: Users see the same language rendered differently depending on where it appears.
- Scope: Notebook code cells, markdown previews or editors, diff panels, embedded code blocks on canvas.
- Out of Scope: Rich executable canvas widgets beyond code-block presentation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`
- Related Systems / Components: Notebooks, markdown rendering, diffs, canvas embeds, theme engine.
- Current Behavior: Diff panel explicitly uses `wxSTC_LEX_NULL`, and other surfaces likely diverge.
- Intended Behavior: Code content uses one shared highlighting language and style model across surfaces.
- Technical Approach: Expose shared highlight services to all code-bearing surfaces.
- Implementation Steps: Audit code-bearing surfaces; connect tokenizer + theme mapping; add embedded-language metadata; align diff token styling.
- Validation Steps: Compare same code snippet in editor, notebook cell, markdown fence, diff, and canvas embed.
- Acceptance Criteria: Code content has coherent semantic color treatment regardless of host surface.
- Dependencies: P07-T01 through P07-T04, Phase 08.
- Risks / Failure Modes: Surface-specific rendering constraints may require adapters.
- UX Notes: Embedded code surfaces should stay visually integrated with host context while preserving syntax clarity.
- Styling / Highlighting Notes where relevant: Token contrast must remain strong in smaller embedded surfaces.
- Observability / Diagnostics Notes: Track which surfaces still use fallback styling only.
- Rollback / Safety Notes: Allow per-surface staged rollout if necessary.
- References / Context: `DiffPanel.cpp` currently sets lexer null; notebook and canvas integration remain weaker than editor integration.
- Example scenarios where useful: A Python snippet looks the same in a markdown note, notebook code cell, and diff view.

### P07-T06

- Phase ID: P07
- Task ID: P07-T06
- Task Title: Add syntax-highlighting regression corpus and visual quality gates
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Keep language presentation quality from regressing after the architecture is cleaned up.
- Why This Matters Now: Highlighting changes can quietly break readability across many surfaces.
- Problem Statement: There is no visible release gate for token quality, theme mapping, or cross-surface consistency.
- User Impact: Perceived editor quality can regress sharply without functional tests noticing.
- Scope: Language corpus, theme-snapshot tests, surface-comparison tests, contrast checks, fallback-path tests.
- Out of Scope: Pixel-perfect theme snapshots for every theme.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/themes`; `/Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp`
- Related Systems / Components: Tests, themes, highlighter, editor rendering.
- Current Behavior: Syntax quality likely lacks direct regression protection.
- Intended Behavior: CI can detect token regressions, unreadable contrast, and theme-mapping drift.
- Technical Approach: Create representative corpora and token/style assertions, plus selective screenshot tests.
- Implementation Steps: Build sample corpora; define expected token classes; add theme contrast assertions; add cross-surface snapshot checks.
- Validation Steps: Run highlighting suite on major themes and core languages.
- Acceptance Criteria: Highlighting regressions become observable and release-blocking.
- Dependencies: P07-T01 through P07-T05.
- Risks / Failure Modes: Snapshot drift may create noisy failures if contracts are too loose or too strict.
- UX Notes: Include readability checks, not just token correctness.
- Styling / Highlighting Notes where relevant: Contrast thresholds and semantic consistency should be part of the gate.
- Observability / Diagnostics Notes: Persist tokenization stats and visual diffs for failures.
- Rollback / Safety Notes: Allow corpus expansion without breaking existing stable expectations.
- References / Context: Current syntax stack is rich in components but weak in product-grade integration testing.
- Example scenarios where useful: A dark theme change that makes comments unreadable is caught before release.

