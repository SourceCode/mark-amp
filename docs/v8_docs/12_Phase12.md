**Phase 35 Prompt (Two-Surface Link Router + Deep-Link Schema)**

1. Goal: make traversal between any two active surfaces deterministic (`Editor`, `Preview`, `Canvas`, `Graph`) with one routing path.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/SurfaceLink.h`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/SurfaceLink.cpp`.
3. Implement: add canonical payload (`from_surface`, `to_surface`, `entity_type`, `entity_id`, `file_path`, `line`, `column`, `board_id`, `object_id`, `cell_id`, `reason`); add resolver result + failure reason enums; remove ad-hoc reveal/open paths in UI and route everything through `NavigationService`.
4. UX result: “Go to related” behaves the same no matter which two surfaces are involved.
5. Acceptance: every cross-surface jump emits a resolved event or an explicit failure event, never silent failure.

**Phase 36 Prompt (Smooth Transition Pipeline + State Handoff)**

1. Goal: eliminate visual jumps and focus loss during surface transitions.
2. Files: new `/Volumes/SecondDrive/code2/mark-amp/src/ui/SurfaceTransitionCoordinator.h`, new `/Volumes/SecondDrive/code2/mark-amp/src/ui/SurfaceTransitionCoordinator.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.
3. Implement: pre-transition snapshot (`scroll`, `cursor`, `selection`, `zoom`, `active object`), preload target data, perform short container transition, restore focus and anchor in target, keep cancellation token for rapid repeated jumps.
4. UX result: transitions feel continuous even when switching rapidly between two panels.
5. Acceptance: no blank intermediate frame, no lost keyboard focus, target anchor always visible after transition.

**Phase 37 Prompt (Paired Traverse Mode for “The Two” Surfaces)**

1. Goal: let users lock two surfaces into a paired workflow and traverse them together.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/SplitView.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/StatusBarPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h`.
3. Implement: add “Pair Surfaces” mode (`Editor+Preview`, `Editor+Canvas`, `Preview+Graph`, etc.); synchronize anchor navigation, optional synchronized scrolling, and bi-directional reveal; add explicit pair state in status bar and toolbar.
4. UX result: users can stay in one cognitive lane while moving across two linked views.
5. Acceptance: pair mode preserves alignment after edits and after manual pane resize/mode changes.

**Phase 38 Prompt (Cross-Surface Breadcrumb + Traversal Timeline)**

1. Goal: make orientation obvious while moving across surfaces.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/BreadcrumbBar.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/BreadcrumbBar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.h`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/NavigationService.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp`.
3. Implement: add traversal breadcrumb segments containing surface + anchor labels; add global timeline entries and quick-jump palette command (`Open Recent Context`); add back/forward across surfaces, not just in-document history.
4. UX result: users always know where they came from and can jump back precisely.
5. Acceptance: cross-surface back/forward is stable and deterministic across at least 100 navigation events.

**Phase 39 Prompt (Traversal Reliability + Performance Gates)**

1. Goal: lock in smoothness and correctness under heavy usage.
2. Files: new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_surface_linking.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_surface_transitions.cpp`, update `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_split_view_advanced.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_live_preview.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_canvas_input.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_keyboard_navigation.cpp`.
3. Implement: test link resolution, fallback behavior, focus restoration, paired mode sync, and transition timing budgets; add regression tests for rapid repeated toggling between two surfaces.
4. UX result: traversal remains smooth and predictable in real-world sessions.
5. Acceptance: all tests pass and transition latency/focus metrics stay within budget in debug CI.

**Phase 40 Prompt: VSCode Theme Ingestion Engine**

1. Objective: fully ingest VSCode-compatible theme JSON and extension-contributed themes without manual conversion.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeLoader.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeLoader.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/VsCodeThemeAdapter.h`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/VsCodeThemeAdapter.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeRegistry.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionManifest.cpp`.
3. Implement: parse VSCode `colors`, `tokenColors`, `semanticTokenColors`, and `include`; resolve inheritance chains; map to internal layered theme model; preserve unknown VSCode keys in metadata for forward compatibility.
4. UX outcome: users can import common VSCode themes directly and get usable results immediately.
5. Acceptance: importing raw VSCode theme JSON or VSIX theme contributions produces a selectable theme with no crash and clear warnings for unsupported fields.

**Phase 41 Prompt: Token Coverage + Component Theme Correctness**

1. Objective: eliminate theme leaks and ensure every major UI surface is token-driven.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/Theme.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeEngine.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeEngine.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/LayoutManager.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/TabBar.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/StatusBarPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/CommandPalette.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ActivityBar.cpp`.
3. Implement: add missing semantic tokens for interactive states (`hover`, `active`, `pressed`, `focus`, `error`, `warning`, `info`, `success`); replace remaining hardcoded UI colors; add token fallback rules when VSCode theme omits values.
4. UX outcome: theme switching feels coherent across IDE-like and canvas-like surfaces.
5. Acceptance: runtime theme switch updates all chrome elements consistently and no touched component uses fixed RGB literals for interactive states.

**Phase 42 Prompt: VSCode-Grade Syntax and Semantic Highlighting**

1. Objective: match VSCode’s visual fidelity for code coloring.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/SyntaxHighlighter.h`, `/Volumes/SecondDrive/code2/mark-amp/src/core/SyntaxHighlighter.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeScopeMapper.h`, new `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeScopeMapper.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/EditorPanel.cpp`.
3. Implement: add TextMate scope matching priority logic; apply semantic token overrides over TextMate tokens; support font style flags (italic, bold, underline) from VSCode theme rules.
4. UX outcome: imported themes look “correct” in editor code regions, not approximate.
5. Acceptance: top VSCode themes render with expected keyword/string/comment/function/type distinctions and semantic overrides apply predictably.

**Phase 43 Prompt: Theme UX Flow (Import, Preview, Repair, Apply)**

1. Objective: make theme adoption easy for end users and deterministic for AI agents.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/ui/ThemeGallery.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ThemeTokenEditor.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.
3. Implement: add “Import VSCode Theme…” entry in menu/settings/theme gallery; show compatibility report (`fully supported`, `mapped with fallbacks`, `partial`); add one-click “auto-fix contrast” and “repair missing tokens”.
4. UX outcome: users can evaluate and apply themes quickly without breaking readability.
5. Acceptance: every imported theme shows a compatibility badge, contrast summary, and reversible apply flow.

**Phase 44 Prompt: VSCode Theme Pack + Extension Theme Discovery**

1. Objective: treat VSCode extension themes as first-class discoverable assets.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionScanner.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ExtensionManagement.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/core/ThemeRegistry.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ExtensionsBrowserPanel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/src/ui/ThemeGallery.cpp`.
3. Implement: auto-index installed extension themes from `contributes.themes`; expose “Apply Theme” directly from extension details; keep theme source metadata (`built-in`, `custom`, `extension`, `vscode-import`).
4. UX outcome: VSCode-style theme ecosystem feels native inside your app.
5. Acceptance: installing an extension with themes makes those themes immediately available in theme gallery and settings.

**Phase 45 Prompt: Theme QA and Regression Gates**

1. Objective: lock quality so future UI work never regresses theme fidelity.
2. Files: `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_theme_loader.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_theme_engine.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_theme_phase4.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_vscode_theme_adapter.cpp`, new `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_theme_compatibility.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_accessibility.cpp`.
3. Implement: add real VSCode-theme fixture corpus, mapping coverage tests, semantic token precedence tests, fallback/repair tests, and contrast checks per theme.
4. UX outcome: high confidence that theme UX stays stable as you expand editor/canvas/notebook features.
5. Acceptance: build/tests pass and VSCode theme compatibility fixtures meet target support threshold before release.
