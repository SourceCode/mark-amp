Phase 14 Prompt (Editor UX Core: VSCode/JetBrains Parity)

1. Rebuild the editor interaction model in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.h` around explicit state layers: `cursor`, `selection`, `diagnostics`, `active-line`, `search`, `code-actions`, and `write-mode`, so visuals never conflict.

2. Add semantic editor/chrome tokens in `/Volumes/SecondDrive/code2/mark-amp/src/core/Theme.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeEngine.h` for `editor_active_line`, `editor_gutter_marker_error/warn/info`, `editor_match_highlight`, `editor_find_hit`, and `editor_quick_fix`, then remove any remaining hard-coded editor/preview colors in touched files.

3. Upgrade gutter usability in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`: add change markers, diagnostics markers, fold affordances, and hover tooltips with clear hit areas and keyboard equivalents.

4. Redesign Find/Replace into a docked, keyboard-first panel in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp` with history, regex toggle persistence, replace-preview count, and deterministic focus traversal.

5. Improve command ergonomics in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp`: add high-priority editor actions (`quick fix`, `go to symbol`, `next/prev diagnostic`, `toggle focus mode`) with stable shortcuts and better ranking when editor is focused.

6. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_enhancements.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_controls.cpp` for gutter markers, find/replace behavior, and keyboard navigation.

Phase 15 Prompt (Preview Fidelity + Authoring Loop)

1. Refactor preview styling in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` to use only render tokens from `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeEngine.h` (`RenderHeading`, `RenderLink`, `RenderCodeBg`, etc.), including Mermaid error/unavailable states.

2. Add source mapping attributes from `/Volumes/SecondDrive/code2/mark-amp/src/rendering/HtmlRenderer.cpp` to preview DOM blocks, then implement click-to-source and reveal-in-editor wiring in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`.

3. Improve live-preview responsiveness in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` by incremental render decisions (small edits do partial update path, large edits do full pass) and tighter debounce rules per content size.

4. Add robust scroll-sync modes in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`: proportional, heading-anchored, and cursor-anchored, with explicit user toggle and state persistence.

5. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_preview_panel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_live_preview.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_crash_regressions.cpp` for source mapping, scroll sync accuracy, and stability under rapid edits.

Phase 16 Prompt (Split Workflow + Pro Authoring Experience)

1. Upgrade split ergonomics in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.h` with workspace presets (`Editor Focus`, `Balanced`, `Review`, `Preview Focus`) and animated but interruptible transitions.

2. Add synchronized navigation aids: linked heading breadcrumbs, visible section focus, and selection mirrors between editor and preview in `/Volumes/SecondDrive/code2/mark-amp/src/ui/BreadcrumbBar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`.

3. Add quick actions in `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` for `open in side`, `toggle sync mode`, `pin preview`, `export current render`.

4. Ensure all controls are menu-accessible and command-palette-accessible by wiring View/Edit menu items in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.

5. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_split_view.cpp` and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_split_view_advanced.cpp` for preset switching, focus retention, and sync behavior.

Phase 17 Prompt (Quality Bar: Better Than VSCode/JetBrains for Markdown Workflows)

1. Define measurable UX/perf budgets for editor and preview in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_phase20_perf.cpp` and related tests: input latency, render latency, scroll smoothness, and large-file behavior.

2. Add a markdown-author quality suite across `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_qol.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_gfm_extensions.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_html_renderer.cpp` covering tables, callouts, code fences, Mermaid, LaTeX, and anchor behavior.

3. Add accessibility checks for editor/preview contrast and keyboard-only operation in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_accessibility.cpp` and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_keyboard_navigation.cpp`.

4. Acceptance criteria: editor and preview are fully theme-consistent, menu/shortcut/command surfaces are equivalent, split sync is reliable, no hard-coded UI colors are introduced, and build/tests pass with:
   `cmake --preset debug`
   `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`
   `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`
