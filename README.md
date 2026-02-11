# MarkAmp

**v1.1.3** — A powerful, cross-platform Markdown editor built with C++23 and wxWidgets.

## Features

- **Cross-Platform**: Runs natively on macOS, Windows, and Linux
- **High Performance**: Built with C++23 for speed and efficiency
- **Rich Editing**: Syntax highlighting for 15+ languages, GFM support, Mermaid diagrams
- **Command Palette**: Fuzzy-searchable command launcher (Ctrl+Shift+P)
- **Zen Mode**: Distraction-free editing with a single keypress
- **Themeable**: 8 built-in retro-futuristic themes with markdown-based theme authoring
- **Accessible**: WCAG AA contrast, screen reader support, full keyboard navigation
- **Session Restore**: Pick up exactly where you left off

## Build Instructions

### Prerequisites

- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022)
- CMake 3.30+
- Ninja build system
- vcpkg (for dependency management)

### Building

```bash
# Configure (uses CMake presets)
cmake --preset debug

# Build
cmake --build build/debug -j$(sysctl -n hw.ncpu)
```

## Testing

```bash
cd build/debug && ctest --output-on-failure
```

## Documentation

- [User Guide](docs/user_guide.md)
- [Architecture](docs/architecture.md)
- [API Reference](docs/api_reference.md)
- [Security Audit](docs/security_audit.md)
- [Changelog](CHANGELOG.md)
- [Release History](HISTORY.md)

## License

See LICENSE file.
