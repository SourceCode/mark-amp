# Changelog

All notable changes to MarkAmp are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-02-11

### Added

#### Core Editor

- Cross-platform Markdown editor built with C++23 and wxWidgets 3.2.9
- Three view modes: Source, Split (default), Preview — switchable via toolbar or Ctrl+1/2/3
- GitHub Flavored Markdown support: headings, emphasis, lists, tables, code blocks, task lists, blockquotes, footnotes, horizontal rules, strikethrough
- Syntax highlighting for 15+ programming languages (C, C++, Python, JavaScript, TypeScript, Rust, Go, Java, Ruby, Shell, SQL, JSON, YAML, HTML, CSS)
- Mermaid diagram rendering with inline error overlay for syntax errors
- Find and replace with match highlighting and navigation
- Line numbers, word wrap, bracket matching, auto-indent
- Undo/redo via command pattern
- Custom retro-futuristic window chrome

#### Theme System

- 8 built-in themes: Midnight Neon, Cyber Night, Solarized Dark, Classic Mono, High Contrast Blue, Matrix Core, Classic Amp, Vapor Wave
- Theme Gallery with live preview cards
- Theme import/export (JSON format)
- Custom theme creation with full color token specification
- Theme validation: structural integrity, color parsing, WCAG AA contrast checking

#### File Management

- Sidebar file tree with folder navigation
- Open individual files or entire folders
- Keyboard-navigable file tree (arrow keys, Enter to open)
- Resizable sidebar with persistent width

#### Accessibility

- Screen reader support with ARIA-compatible patterns
- High Contrast Blue theme meets WCAG AA
- Full keyboard navigation (Tab cycles focus zones)
- Scalable UI elements
- F1 keyboard shortcut reference

#### Performance

- Sub-500ms cold start
- Debounced preview updates
- Smooth scrolling for large documents
- Instant theme switching (<100ms)

#### UI & Design

- JetBrains Mono + Rajdhani font pairing
- Custom styled scrollbars
- Status bar: cursor position, encoding, Mermaid status, active theme name
- Responsive layout adapts to window size

#### Platform Support

- macOS: .dmg installer, Mac App Store .pkg with sandbox entitlements
- Windows: NSIS installer, portable ZIP, MSIX for Microsoft Store
- Linux: .deb, AppImage, .tar.gz with desktop integration

#### Build & CI

- CMake 3.30+ with presets (debug, release, release-static)
- GitHub Actions CI: 3-platform matrix, vcpkg caching, automated tests
- GitHub Actions Release: tag-triggered packaging and GitHub Release creation
- Dev scripts: build, test, clean, version bump

### Known Issues

- Mermaid rendering requires WebView component; may not render in headless environments
- Custom scrollbar styling limited on some Linux desktop environments
- Large Mermaid diagrams (>200 nodes) may render slowly
