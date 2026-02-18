# MarkAmp

**v2.3.16** — A powerful, cross-platform Markdown editor built with C++23 and wxWidgets. Canvas whiteboarding, knowledge graphs, AI writing assistance, plugin architecture, and 50 feature phases in a retro-futuristic package.

---

## Features

- **Cross-Platform** — Runs natively on macOS, Windows, and Linux
- **High Performance** — C++23 with arena allocators, viewport caching, lock-free queues, and 20+ rendering primitives
- **Rich Editing** — 60+ editor actions, syntax highlighting for 15+ languages, GFM support, Mermaid diagrams, LaTeX math
- **Infinite Canvas** — Whiteboard with sticky notes, shapes, connectors, mind maps, Kanban boards, and collaboration
- **Knowledge Graph** — Backlink visualization, wiki links, graph analytics, and local graph engine
- **AI Integration** — Writing assistance, inline completion, tag/link suggestions, multi-provider support
- **Attribute Views** — Notion-style databases with table, gallery, kanban, and timeline views
- **Flashcard System** — FSRS spaced repetition with cloze deletions and study streak tracking
- **Plugin System** — VS Code-inspired extensibility with manifest-driven contribution points, lazy activation, and 15 built-in plugins
- **Extension Marketplace** — Gallery search, VSIX install/uninstall, sandboxing, and host recovery
- **Presentation Mode** — Slide-based presentations from Markdown with export to PDF/HTML
- **Version Control** — Git integration with blame, stash, commit graph, and conflict resolution
- **Cloud Sync** — Multi-provider sync (S3, WebDAV) with end-to-end encryption
- **WYSIWYG Mode** — Rich-text editing with mode switching and state persistence
- **Themeable** — 8 built-in retro-futuristic themes with VS Code theme import
- **Command Palette** — Fuzzy-searchable command launcher (Ctrl+Shift+P / ⌘⇧P)
- **Multi-Window** — Editor groups, pane management, and workspace layout persistence
- **Localization** — Multi-language support with RTL/LTR text direction
- **Security** — AES-256 encryption, vault management, content security policies, and audit logging
- **Accessibility** — WCAG AA contrast, screen reader support, full keyboard navigation

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        Application Layer                        │
│                    MarkAmpApp → MainFrame                       │
├─────────────┬──────────────┬────────────┬───────────────────────┤
│  UI (88)    │ Canvas (98)  │ Render(36) │   Platform (4)        │
├─────────────┴──────────────┴────────────┴───────────────────────┤
│                    Core Engine (512 headers)                     │
│   EventBus · Config · Theme · Plugin · Extension · AI · Export  │
│   Search · Graph · Notebook · Tasks · Cloud · Security · FSRS   │
└─────────────────────────────────────────────────────────────────┘
```

→ [Architecture Deep Dive](docs/architecture.md) · [Implementation Details](docs/implementation.md)

---

## Tech Stack

| Layer           | Technology       | Version              |
| --------------- | ---------------- | -------------------- |
| Language        | C++23            | C++26-ready patterns |
| UI Toolkit      | wxWidgets        | 3.2.9                |
| Build           | CMake + Ninja    | 3.30+                |
| Package Manager | vcpkg            | manifest mode        |
| Test Framework  | Catch2           | latest               |
| Markdown        | md4c             | latest               |
| JSON            | nlohmann-json    | latest               |
| Logging         | spdlog + fmt     | latest               |
| Config          | yaml-cpp         | latest               |
| Database        | SQLite3 (FTS5)   | latest               |
| Crypto          | OpenSSL          | latest               |
| HTTP            | cpp-httplib      | latest               |
| Profiling       | Tracy            | latest               |
| Allocator       | mimalloc         | latest               |
| Icons           | Lucide (nanosvg) | latest               |

---

## Quick Start

```bash
# Clone
git clone https://github.com/markamp/markamp.git
cd markamp

# Configure (debug build with tests)
cmake --preset debug

# Build
cmake --build build/debug -j$(sysctl -n hw.ncpu)

# Run
./build/debug/markamp

# Test
cd build/debug && ctest --output-on-failure
```

→ [Install Guide](docs/install.md) · [Setup Guide](docs/setup.md) · [First Run](docs/first-run.md)

---

## Testing & Coverage

**451 test targets** across unit, integration, and fuzz tests.

```bash
# Run all tests
cd build/debug && ctest --output-on-failure

# Run specific test
ctest -R test_event_bus --output-on-failure

# Run with sanitizers
cmake --preset debug-asan
cmake --build build/debug-asan -j$(sysctl -n hw.ncpu)
cd build/debug-asan && ctest --output-on-failure
```

→ [Testing Guide](docs/testing.md) · [Coverage Report](docs/coverage.md)

---

## Configuration

Settings are stored in a platform-appropriate config file (YAML):

| Platform | Path                                                |
| -------- | --------------------------------------------------- |
| macOS    | `~/Library/Application Support/MarkAmp/config.yaml` |
| Windows  | `%APPDATA%\MarkAmp\config.yaml`                     |
| Linux    | `~/.config/MarkAmp/config.yaml`                     |

60+ configuration keys. Full reference in [Setup Guide](docs/setup.md).

---

## Documentation

| Document                                   | Description                                   |
| ------------------------------------------ | --------------------------------------------- |
| [Documentation Hub](docs/README.md)        | Central index of all documentation            |
| [Install Guide](docs/install.md)           | Prerequisites, dependencies, build steps      |
| [Setup Guide](docs/setup.md)               | Configuration, environment variables, secrets |
| [First Run](docs/first-run.md)             | First launch walkthrough                      |
| [Functionality](docs/functionality.md)     | Feature reference by module                   |
| [Architecture](docs/architecture.md)       | System design and data flow                   |
| [API Reference](docs/api_reference.md)     | Core interfaces and type definitions          |
| [Implementation](docs/implementation.md)   | Repository layout and conventions             |
| [Schema](docs/schema.md)                   | Data models and persistence                   |
| [Integrations](docs/integrations.md)       | Third-party integrations                      |
| [Testing](docs/testing.md)                 | Test strategy and execution                   |
| [Coverage](docs/coverage.md)               | Test coverage analysis                        |
| [Security](docs/security.md)               | Security controls and audit                   |
| [Troubleshooting](docs/troubleshooting.md) | Common issues and solutions                   |
| [Contributing](CONTRIBUTING.md)            | Development workflow                          |
| [User Guide](docs/user_guide.md)           | End-user documentation                        |
| [Changelog](CHANGELOG.md)                  | Release history                               |

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for coding conventions, commit standards, and the PR process.

**Key rules:**

- C++23 standard — no `any` or `unknown` types
- All warnings are errors (`-Wall -Wextra -Werror`)
- All new features must have unit tests
- Code formatted with clang-format before commit

---

## License

See [LICENSE](LICENSE) file.
