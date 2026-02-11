# MarkAmp C++26 / wxWidgets 3.2.9 -- Master Implementation Plan

## Project Overview

MarkAmp is a cross-platform desktop Markdown viewer and editor with a highly stylized, skinnable UI inspired by classic media players and IDE hybrids. This plan details the complete port from a React/TypeScript prototype to a production-grade C++26 application using wxWidgets 3.2.9 for cross-platform native UI.

## Target Technology Stack

| Component | Technology | Version |
|---|---|---|
| Language | C++26 | Latest standard (modules, concepts, ranges, coroutines, std::expected, std::format) |
| UI Framework | wxWidgets | 3.2.9 |
| Build System | CMake | 3.30+ with presets |
| Package Manager | vcpkg | Latest |
| Markdown Parser | md4c | Latest (C library with C++ wrapper) |
| Mermaid Rendering | Embedded JS (QuickJS) or SVG generation | Latest |
| JSON | nlohmann/json | 3.x |
| Syntax Highlighting | Tree-sitter (30+ language grammars) | Latest |
| Testing | Catch2 | 3.x |
| Code Quality | clang-tidy, clang-format, cppcheck, ASAN/UBSAN/TSAN, iwyu | Latest |
| Platforms | macOS, Windows, Linux | macOS 12+, Windows 10+, Ubuntu 22.04+ |

## Reference Implementation Feature Map

The React/TypeScript prototype provides these features that must be faithfully ported:

1. Custom window chrome (no OS default frame)
2. File tree sidebar (collapsible, recursive, with folder expand/collapse)
3. Three view modes: Source (SRC), Split, Preview (VIEW)
4. GitHub Flavored Markdown rendering (headings, emphasis, lists, blockquotes, tables, code blocks, task lists, links, images)
5. Mermaid diagram rendering with error handling
6. 8 built-in themes with instant hot-swap via 10 CSS-variable equivalents
7. Theme import/export (JSON files)
8. Theme gallery modal with live miniature previews
9. Status bar (ready state, cursor position, encoding, theme name, mermaid status)
10. Beveled panel effects for retro-futuristic aesthetic
11. Custom scrollbar styling
12. JetBrains Mono + Rajdhani font pairing
13. Toolbar with view mode toggle, save, theme gallery, settings buttons

## Phase Summary Table

| Phase | Title | Complexity | Prerequisites |
|---|---|---|---|
| 01 | Project Scaffolding and CMake Setup | Medium | None |
| 02 | Code Quality Tooling Pipeline | Medium | Phase 01 |
| 03 | Dependency Management with vcpkg | Medium | Phase 01 |
| 04 | wxWidgets Integration and Basic Window | High | Phases 01, 03 |
| 05 | Application Architecture and Module Design | High | Phase 04 |
| 06 | Custom Window Chrome | High | Phase 05 |
| 07 | Theme Data Model and JSON Serialization | Medium | Phase 05 |
| 08 | Theme Engine and Runtime Switching | High | Phases 06, 07 |
| 09 | Layout Manager -- Sidebar, Content Area, Status Bar | High | Phases 06, 08 |
| 10 | Sidebar File Tree Component | High | Phase 09 |
| 11 | File System Integration | Medium | Phases 05, 10 |
| 12 | Text Editor Component | Critical | Phase 09 |
| 13 | Markdown Parsing Engine Integration | High | Phases 03, 05 |
| 14 | Markdown-to-Rendered-View Pipeline | High | Phases 13, 08 |
| 15 | GFM Extensions -- Tables, Task Lists, Footnotes | High | Phase 14 |
| 16 | Code Block Rendering with Syntax Highlighting | High | Phase 14 |
| 17 | Mermaid Diagram Integration | Critical | Phase 14 |
| 18 | Preview Component with Theme-Aware Rendering | High | Phases 14, 15, 16, 17 |
| 19 | Split View Mode Implementation | High | Phases 12, 18 |
| 20 | Editor Enhancements -- Line Numbers, Word Wrap, Large Files | Medium | Phase 12 |
| 21 | Theme Gallery UI | High | Phases 08, 09 |
| 22 | Theme Import and Export | Medium | Phases 07, 21 |
| 23 | Status Bar Implementation | Medium | Phases 09, 12 |
| 24 | Keyboard Navigation and Shortcuts | Medium | Phases 12, 19 |
| 25 | Accessibility Features | Medium | Phases 24, 18 |
| 26 | Performance Optimization | High | Phases 19, 20, 18 |
| 27 | Security Hardening | Medium | Phases 14, 17, 11 |
| 28 | Cross-Platform Testing and Polish | High | All prior phases |
| 29 | Build Pipeline, Packaging, App Store Distribution | Critical | Phase 28 |
| 30 | Integration Testing, Store Submission, and Release | Critical | Phase 29 |

## Dependency Graph (Simplified)

```
Phase 01 ──> Phase 02
Phase 01 ──> Phase 03
Phase 01 + 03 ──> Phase 04
Phase 04 ──> Phase 05
Phase 05 ──> Phase 06
Phase 05 ──> Phase 07
Phase 06 + 07 ──> Phase 08
Phase 06 + 08 ──> Phase 09
Phase 09 ──> Phase 10
Phase 05 + 10 ──> Phase 11
Phase 09 ──> Phase 12
Phase 03 + 05 ──> Phase 13
Phase 13 + 08 ──> Phase 14
Phase 14 ──> Phase 15
Phase 14 ──> Phase 16
Phase 14 ──> Phase 17
Phase 14 + 15 + 16 + 17 ──> Phase 18
Phase 12 + 18 ──> Phase 19
Phase 12 ──> Phase 20
Phase 08 + 09 ──> Phase 21
Phase 07 + 21 ──> Phase 22
Phase 09 + 12 ──> Phase 23
Phase 12 + 19 ──> Phase 24
Phase 24 + 18 ──> Phase 25
Phase 19 + 20 + 18 ──> Phase 26
Phase 14 + 17 + 11 ──> Phase 27
All 01-27 ──> Phase 28
Phase 28 ──> Phase 29
Phase 29 ──> Phase 30
```

## Theme System Mapping (React CSS Variables to C++ Color Tokens)

| React CSS Variable | C++ Token Name | Usage |
|---|---|---|
| `--bg-app` | `kBgApp` | Main application background |
| `--bg-panel` | `kBgPanel` | Sidebar, toolbar, panel backgrounds |
| `--bg-header` | `kBgHeader` | Window chrome, section headers |
| `--bg-input` | `kBgInput` | Text editor background, input fields |
| `--text-main` | `kTextMain` | Primary text color |
| `--text-muted` | `kTextMuted` | Secondary/muted text |
| `--accent-primary` | `kAccentPrimary` | Primary accent (buttons, active states, headings) |
| `--accent-secondary` | `kAccentSecondary` | Secondary accent (hover states, borders) |
| `--border-light` | `kBorderLight` | Light border / bevel highlight |
| `--border-dark` | `kBorderDark` | Dark border / bevel shadow |

## Built-in Themes (Ported from Reference)

1. **Midnight Neon** -- Deep dark with neon pink/cyan accents
2. **Cyber Night** -- Dark blue with red accents
3. **Solarized Dark** -- Classic Solarized palette
4. **Classic Mono** -- Light theme, black on white
5. **High Contrast Blue** -- Accessibility-oriented high contrast
6. **Matrix Core** -- Terminal green on black
7. **Classic Amp** -- Winamp-inspired green/gold
8. **Vapor Wave** -- Purple/cyan vaporwave aesthetic

## File Structure Convention

```
markamp/
  CMakeLists.txt
  CMakePresets.json
  vcpkg.json
  .clang-tidy
  .clang-format
  src/
    main.cpp
    app/
      MarkAmpApp.h / .cpp
    core/
      Theme.h / .cpp
      ThemeEngine.h / .cpp
      ThemeRegistry.h / .cpp
      FileNode.h / .cpp
      FileSystem.h / .cpp
      MarkdownParser.h / .cpp
      MermaidRenderer.h / .cpp
      SyntaxHighlighter.h / .cpp
      EventBus.h / .cpp
    ui/
      MainFrame.h / .cpp
      CustomChrome.h / .cpp
      LayoutManager.h / .cpp
      Sidebar.h / .cpp
      FileTreeCtrl.h / .cpp
      EditorPanel.h / .cpp
      PreviewPanel.h / .cpp
      SplitView.h / .cpp
      StatusBar.h / .cpp
      Toolbar.h / .cpp
      ThemeGallery.h / .cpp
      BevelPanel.h / .cpp
    rendering/
      MarkdownRenderer.h / .cpp
      HtmlRenderer.h / .cpp
      MermaidBlock.h / .cpp
      CodeBlockRenderer.h / .cpp
      TableRenderer.h / .cpp
    platform/
      PlatformAbstraction.h
      MacPlatform.h / .cpp
      WinPlatform.h / .cpp
      LinuxPlatform.h / .cpp
  resources/
    themes/
      midnight_neon.json
      cyber_night.json
      solarized_dark.json
      classic_mono.json
      high_contrast_blue.json
      matrix_core.json
      classic_amp.json
      vapor_wave.json
    fonts/
      JetBrainsMono-Regular.ttf
      JetBrainsMono-Bold.ttf
      Rajdhani-Regular.ttf
      Rajdhani-SemiBold.ttf
      Rajdhani-Bold.ttf
    icons/
      markamp.ico
      markamp.icns
      markamp.png
    sample_files/
      README.md
      architecture.md
      sequence.md
      TODO.md
  tests/
    unit/
      test_theme.cpp
      test_file_node.cpp
      test_markdown_parser.cpp
      test_mermaid_renderer.cpp
      test_syntax_highlighter.cpp
      test_event_bus.cpp
    integration/
      test_theme_engine.cpp
      test_layout.cpp
      test_editor_preview.cpp
    CMakeLists.txt
  packaging/
    macos/
      Info.plist
      entitlements/
        markamp.entitlements
        markamp-inherit.entitlements
    windows/
      AppxManifest.xml
      assets/ (Store logos: 44x44, 150x150, 310x150, StoreLogo)
      installer.nsi
    linux/
      markamp.desktop
  docs/
    v1_docs/
      00_master_plan.md
      00_visual_fidelity_reference.md  (CRITICAL: pixel-perfect UI spec)
      phase_01-30 docs
      app_store_submission_guide.md
      build_guide.md
  scripts/
    format.sh
    lint.sh
    package_macos.sh
    package_windows.sh
    package_linux.sh
```

## Estimated Timeline

| Phases | Description | Estimated Effort |
|---|---|---|
| 01-03 | Foundation (build, tooling, deps) | 1-2 weeks |
| 04-06 | Core UI framework (window, chrome, architecture) | 2-3 weeks |
| 07-08 | Theme system | 1-2 weeks |
| 09-12 | Layout, sidebar, file system, editor | 3-4 weeks |
| 13-18 | Markdown pipeline and rendering | 4-5 weeks |
| 19-22 | View modes, theme gallery, import/export | 2-3 weeks |
| 23-25 | Status bar, keyboard, accessibility | 1-2 weeks |
| 26-27 | Performance and security | 2-3 weeks |
| 28-30 | Testing, packaging, release | 2-3 weeks |
| **Total** | | **18-27 weeks** |

## Critical Design Requirements

### Visual Fidelity: PIXEL-PERFECT Match
The C++26/wxWidgets UI MUST look **EXACTLY** like the React/TypeScript reference implementation. Every measurement, color value, font weight, border style, hover effect, and spacing value must be reproduced precisely. See `00_visual_fidelity_reference.md` for the complete pixel-level specification extracted from the reference source code.

### Editor Quality: Highest Standard
The text editor must provide **the highest quality editing experience possible**. This means:
- Professional-grade syntax highlighting using **Tree-sitter** grammars (not simple regex tokenization)
- Theme-aware highlighting that updates instantly
- Support for 30+ programming languages
- Bracket matching, auto-indent, code folding
- Smooth cursor movement and selection
- Sub-frame input latency

### App Store Distribution
MarkAmp will be published to:
- **Mac App Store** (requires: App Sandbox entitlements, provisioning profiles, App Store Connect, Apple review compliance)
- **Microsoft Store** (requires: MSIX packaging, Windows App Certification Kit, Store listing)
- **GitHub Releases** (DMG, NSIS installer, portable ZIP, .deb, AppImage)

See Phase 29 for detailed App Store requirements.

## Guiding Principles

1. **Pixel-Perfect Fidelity**: The C++ application must be visually IDENTICAL to the React prototype. See `00_visual_fidelity_reference.md` for exact specifications. Every color, size, font, border, and spacing must match.
2. **C++26 Idioms**: Use modern C++ features aggressively -- modules, concepts, ranges, std::expected, std::format, structured bindings, constexpr where possible.
3. **wxWidgets Best Practices**: Leverage wxWidgets' event system, sizer-based layout, and custom drawing (wxDC, wxGraphicsContext) for the retro-futuristic UI.
4. **Performance First**: The C++ version should be dramatically faster than Electron -- sub-500ms cold start, zero-lag editing, instant theme switching.
5. **Cross-Platform Parity**: Feature-complete on macOS, Windows, and Linux from day one.
6. **App Store Ready**: All code must comply with Mac App Store and Microsoft Store sandboxing, entitlement, and content policies from the start -- not bolted on later.
7. **Testable Architecture**: Every core module must have unit tests. Integration tests must cover all user workflows.
8. **AI-Agent Optimized**: Each phase document contains tasks specifically written for AI coding agents with clear acceptance criteria, file paths, and verification steps.
