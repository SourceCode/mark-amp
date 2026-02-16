**Phase 18 Prompt (Code Intelligence UX for Editor + Preview)**

1. Add a diagnostics-first editing loop in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.h`: inline squiggles, gutter severity icons, hover cards, and keyboard navigation for next/previous issue.

2. Integrate quick-fix and code-action affordances using `/Volumes/SecondDrive/code2/mark-amp/src/core/DiagnosticsService.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/TextEditorService.h`, and `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`, then surface actions in editor context menu and command palette.

3. Add a “peek problem” inline panel in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp` and connect it to `/Volumes/SecondDrive/code2/mark-amp/src/ui/ProblemsPanel.cpp` so issue context never forces full panel switching.

4. Improve preview error attribution in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` by mapping render errors back to source lines and offering “jump to source” actions.

5. Ensure all diagnostics/actions are accessible through `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` menus and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp`.

6. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_controls.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_enhancements.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_panel_data_flow.cpp` for diagnostics rendering, quick-fix dispatch, and panel synchronization.

---

**Phase 19 Prompt (Preview Interactivity + Author Workflow Parity)**

1. Implement interactive preview block controls in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`: copy code, collapse/expand sections, open source, and block-level “edit this section” actions.

2. Add richer markdown object behaviors in `/Volumes/SecondDrive/code2/mark-amp/src/rendering/HtmlRenderer.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`: sortable tables, collapsible callouts, and navigable footnotes.

3. Expand Mermaid and math UX in `/Volumes/SecondDrive/code2/mark-amp/src/rendering/MermaidBlockRenderer.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/MermaidRenderer.h`, and `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` with consistent theming, better error messages, and retry/render controls.

4. Add preview navigation overlays in `/Volumes/SecondDrive/code2/mark-amp/src/ui/BreadcrumbBar.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` so heading path and active section are always visible.

5. Add tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_preview_panel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_html_renderer.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_live_preview.cpp` for block actions, interactive elements, and stable scroll/state retention.

---

**Phase 20 Prompt (Large-File Performance + Fluidity Upgrade)**

1. Optimize editor paint and input paths in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp` using existing performance primitives (`generation counters`, `viewport caching`, `async pipeline`) to preserve low-latency typing in very large documents.

2. Add progressive preview rendering in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp`: first paint quickly with structural placeholders, then hydrate heavy blocks (code, Mermaid, media) asynchronously.

3. Improve split-mode throughput in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.cpp` by adaptive sync throttling under load, preventing editor typing jank while preview updates.

4. Add stress budgets and regression tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_phase20_perf.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_live_preview.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_crash_regressions.cpp` for large docs, rapid edits, and repeated theme/split transitions.

5. Acceptance for this phase: no perceptible input lag on large markdown files, preview stays responsive during continuous typing, and no stability regressions under heavy editing sessions.

---

**Phase 21 Prompt (Professional Polish + Power User Controls)**

1. Add editor “productivity modes” in `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`: writing mode, review mode, and refactor mode (each with defined line spacing, minimap, wrap, diagnostics density, and preview sync behavior).

2. Add preview “reading profiles” in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`: documentation, presentation, and print-ready profiles with explicit typography and spacing presets.

3. Add one-click mode switching in `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp`, fully mirrored in menu entries and shortcuts.

4. Add usability tests in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_editor_qol.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_keyboard_navigation.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_accessibility.cpp` to validate keyboard-only workflows and profile switching consistency.

5. Completion gate for the whole set: build and test pass with  
   `cmake --preset debug`  
   `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`  
   `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.
