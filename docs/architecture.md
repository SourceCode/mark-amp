# MarkAmp Architecture Guide

Technical overview of the MarkAmp codebase for developers and contributors.

---

## System Overview

MarkAmp is a cross-platform desktop Markdown editor built with **C++23** and **wxWidgets 3.2.9**. The architecture follows an event-driven pattern with clear separation between core logic (platform-independent) and UI (wxWidgets-dependent).

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                     │
│  MarkAmpApp → MainFrame → CustomChrome                  │
├─────────────────────────────────────────────────────────┤
│  ┌─────────┐  ┌──────────┐  ┌─────────┐  ┌──────────┐  │
│  │ Editor  │  │ Preview  │  │ Sidebar │  │ StatusBar│  │
│  │  Panel  │  │  Panel   │  │  Tree   │  │          │  │
│  └────┬────┘  └────┬─────┘  └────┬────┘  └────┬─────┘  │
│       │            │              │             │        │
│       └────────────┴──────┬───────┴─────────────┘        │
│                           │                              │
│                      EventBus                            │
├───────────────────────────┼──────────────────────────────┤
│                    Core Layer                            │
│  ┌──────────┐  ┌──────────┐  ┌───────────┐              │
│  │ AppState │  │  Theme   │  │ Markdown  │              │
│  │ Manager  │  │  Engine  │  │  Parser   │              │
│  └──────────┘  └──────────┘  └───────────┘              │
│  ┌──────────┐  ┌──────────┐  ┌───────────┐              │
│  │  Config  │  │  Command │  │ FileNode  │              │
│  │          │  │  History  │  │   Tree    │              │
│  └──────────┘  └──────────┘  └───────────┘              │
├─────────────────────────────────────────────────────────┤
│                  Rendering Pipeline                      │
│  MarkdownParser → HtmlRenderer → FootnotePreprocessor   │
│  CodeBlockRenderer │ MermaidBlockRenderer                │
├─────────────────────────────────────────────────────────┤
│                  Platform Abstraction                    │
│  PlatformAbstraction.h                                   │
│  MacPlatform │ WinPlatform │ LinuxPlatform               │
└─────────────────────────────────────────────────────────┘
```

---

## Module Breakdown

### `src/core/` — Core Logic (platform-independent)

| File                   | Responsibility                                        |
| ---------------------- | ----------------------------------------------------- |
| `EventBus.h/cpp`       | Type-safe pub/sub with RAII subscriptions             |
| `Events.h`             | All event types (ViewModeChanged, ThemeChanged, etc.) |
| `AppState.h/cpp`       | Centralized state; mutations publish events           |
| `Config.h/cpp`         | JSON-backed persistent configuration                  |
| `Theme.h/cpp`          | Theme data structures with typed color fields         |
| `ThemeEngine.h/cpp`    | Runtime theme switching, wxWidgets resource caching   |
| `ThemeRegistry.h/cpp`  | Theme storage: built-in + user-imported               |
| `ThemeValidator.h/cpp` | Schema + color + WCAG contrast validation             |
| `MarkdownParser.h/cpp` | md4c wrapper → `MarkdownDocument` AST                 |
| `FileNode.h/cpp`       | Tree structure for sidebar file tree                  |
| `Command.h/cpp`        | Undo/redo command pattern                             |
| `Logger.h/cpp`         | Structured logging utility                            |
| `Color.h/cpp`          | Color parsing, conversion, contrast calculation       |
| `HtmlSanitizer.h/cpp`  | Security: sanitize HTML output                        |
| `Types.h`              | Shared type definitions                               |

### `src/rendering/` — Rendering Pipeline

| File                         | Responsibility                            |
| ---------------------------- | ----------------------------------------- |
| `HtmlRenderer.h/cpp`         | AST → HTML conversion with image handling |
| `CodeBlockRenderer.h/cpp`    | Syntax highlighting for code blocks       |
| `MermaidBlockRenderer.h/cpp` | Mermaid diagram → SVG rendering           |

### `src/ui/` — wxWidgets UI Layer

| File                 | Responsibility                                 |
| -------------------- | ---------------------------------------------- |
| `MainFrame.h/cpp`    | Top-level window, layout, keyboard shortcuts   |
| `CustomChrome.h/cpp` | Retro-futuristic title bar and window controls |
| `EditorPanel.h/cpp`  | Markdown text editor with syntax highlighting  |
| `PreviewPanel.h/cpp` | HTML preview via wxWebView                     |
| `SidebarTree.h/cpp`  | File tree navigation                           |
| `StatusBar.h/cpp`    | Bottom bar: cursor, encoding, theme, status    |
| `ThemeGallery.h/cpp` | Theme selection overlay                        |
| `FindBar.h/cpp`      | Find and replace UI                            |

### `src/platform/` — OS Abstraction

| File                    | Responsibility                             |
| ----------------------- | ------------------------------------------ |
| `PlatformAbstraction.h` | Interface for platform-specific operations |
| `MacPlatform.mm`        | macOS: native file dialogs, appearance     |
| `WinPlatform.cpp`       | Windows: registry, file associations       |
| `LinuxPlatform.cpp`     | Linux: XDG compliance, GTK integration     |

---

## Event Flow

All inter-component communication uses the `EventBus`. Components subscribe to events they care about and receive typed callbacks.

```
User action (e.g. click theme)
    │
    ▼
UI Component (ThemeGallery)
    │
    ▼ calls ThemeEngine::apply_theme()
    │
ThemeEngine
    │
    ▼ publishes ThemeChangedEvent
    │
EventBus ──► subscribers
    │            │
    ├── EditorPanel: update syntax colors
    ├── PreviewPanel: regenerate CSS
    ├── CustomChrome: repaint title bar
    ├── SidebarTree: update highlight colors
    └── StatusBar: update theme name display
```

**Key events:**

| Event                      | Published when             |
| -------------------------- | -------------------------- |
| `ThemeChangedEvent`        | Theme switched or imported |
| `ViewModeChangedEvent`     | SRC/SPLIT/VIEW toggled     |
| `ActiveFileChangedEvent`   | Different file selected    |
| `ContentChangedEvent`      | Editor content modified    |
| `CursorChangedEvent`       | Cursor position changes    |
| `SidebarToggleEvent`       | Sidebar shown/hidden       |
| `EditorScrollChangedEvent` | Editor scrolled            |

---

## Rendering Pipeline

```
Markdown text
    │
    ▼
FootnotePreprocessor
    │ extracts [^N] references and definitions
    ▼
MarkdownParser (md4c)
    │ produces MarkdownDocument AST
    ▼
HtmlRenderer
    ├── CodeBlockRenderer (syntax highlighting)
    ├── MermaidBlockRenderer (diagram → SVG)
    ├── Image resolver (local files → data URIs)
    └── HtmlSanitizer (XSS prevention)
    │
    ▼
HTML string → wxWebView preview
```

---

## Theme System

Themes are structured with typed color fields (not raw strings):

```cpp
struct ThemeColors {
    Color bg_app, bg_panel, bg_header, bg_input;
    Color text_main, text_muted;
    Color accent_primary, accent_secondary;
    Color border_light, border_dark;
    // ... additional tokens
};
```

**Three layers of validation:**

1. **Structural:** Required fields present, correct types
2. **Color validity:** All color values parseable
3. **Contrast:** WCAG AA minimum contrast ratios (4.5:1)

**Theme lifecycle:** JSON file → `ThemeRegistry` (load/parse) → `ThemeValidator` (validate) → `ThemeEngine` (apply to wxWidgets) → EventBus notification

---

## Build System

- **CMake 3.30+** with presets (debug, release, release-static)
- **Ninja** generator
- **vcpkg** for dependency management
- **Catch2** for testing

See `docs/v1_docs/build_guide.md` for complete build instructions.
