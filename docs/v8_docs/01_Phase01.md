Phase 1 Prompt (Theme Fidelity + Project-First Shell)

Implement a production-grade UI foundation pass focused on theme correctness, project context, and discoverability of notebook/canvas workflows.

1. Remove non-theme color leaks and enforce token-only rendering.

- Replace hard-coded extension dot colors in `/Volumes/SecondDrive/code2/mark-amp/src/ui/FileTreeCtrl.cpp` with theme-derived colors.
- Replace fixed tab-group pastel palette in `/Volumes/SecondDrive/code2/mark-amp/src/ui/TabBar.cpp` with tint generation derived from current theme accent tokens.
- Replace the hard-coded save flash green in `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp` with theme success color.
- Replace fixed white active text in `/Volumes/SecondDrive/code2/mark-amp/src/ui/ExtensionsBrowserPanel.cpp` with contrast-aware foreground selection.
- Replace hard-coded reset button colors in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp` with theme error/foreground tokens.
- Replace hard-coded notification severity colors in `/Volumes/SecondDrive/code2/mark-amp/src/ui/NotificationManager.cpp` with theme-derived semantic colors.
- Replace hard-coded Mermaid/preview error reds in `/Volumes/SecondDrive/code2/mark-amp/src/ui/PreviewPanel.cpp` and `/Volumes/SecondDrive/code2/mark-amp/src/rendering/MermaidBlockRenderer.cpp` with theme tokens.
- Remove hard-coded frame startup background in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`; use theme engine color.

2. Fix theme hot-swap inconsistencies in sidebar chrome.

- In `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, persist pointers for sidebar header/footer/collapse controls and re-apply colors in `OnThemeChanged`.
- Ensure sidebar paint effects use theme-derived values only (no raw black constants) in `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`.
- Ensure explorer search, header, footer, and section controls all refresh correctly after runtime theme switch.

3. Make explorer states visually correct and legible.

- In `/Volumes/SecondDrive/code2/mark-amp/src/ui/FileTreeCtrl.cpp`, stop using `AccentPrimary.ChangeLightness(80)` for selected text; use a contrast-safe selected text color.
- Use `list_hover` and `list_selected` theme semantics for row backgrounds and maintain WCAG-friendly text contrast.
- Keep metadata and filter-match colors harmonized with theme tokens.

4. Establish project-first information hierarchy in sidebar header.

- In `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, preserve project name from workspace root and stop overwriting it when sidebar mode changes.
- Show project identity and section identity separately (project title + section label).
- Keep this fully theme-aware and consistent with custom themes.

5. Expose notebook/canvas workflows in command surface (Phase 1 discoverability).

- Add command palette entries in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` for project workflows: open explorer, open extensions, open graph view, open canvas view, notebook actions (create/run/export placeholders if UI route is not fully wired).
- Wire to existing events where available in `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`; add minimal new events only if required.
- Do not build full new panels in this phase; prioritize discoverability and navigation hooks.

6. Add guardrail tests for theme fidelity.

- Add/extend unit tests to ensure key UI color decisions are token-derived and theme-switch safe in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_theme_engine.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_layout.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_file_tree.cpp`, and `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_visual_polish.cpp`.
- Add at least one regression test for runtime theme switching affecting sidebar header/footer and explorer row states.

7. Acceptance criteria.

- No hard-coded UI state colors remain in touched components except print-only CSS.
- Switching themes at runtime updates sidebar, explorer, tabs, toolbar, notifications, preview/Mermaid states without stale colors.
- Explorer selection/hover/text states are contrast-safe under all built-in themes.
- Project identity remains visible while switching sidebar sections.
- New project/notebook/canvas commands are reachable from command palette and execute valid event paths.
- Build and tests pass with:
  `cmake --preset debug`
  `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`
  `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`
