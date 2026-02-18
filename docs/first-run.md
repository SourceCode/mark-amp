# First Run Guide

> From clone to running MarkAmp and executing tests.

---

## Golden Path (macOS)

```bash
# 1. Clone the repository
git clone https://github.com/markamp/markamp.git
cd markamp

# 2. Configure the debug build
cmake --preset debug

# 3. Build all targets (binary + tests)
cmake --build build/debug -j$(sysctl -n hw.ncpu)

# 4. Run the application
./build/debug/markamp

# 5. Run all tests
cd build/debug && ctest --output-on-failure
```

## What You'll See on First Launch

1. **Midnight Neon** theme applied (dark blue with cyan accents)
2. **Split view** — editor on the left, live preview on the right
3. **File tree sidebar** on the left with sample workspace
4. **Activity bar** along the left edge (Explorer, Search, Settings, Themes, Extensions)
5. **Status bar** at the bottom showing cursor position, encoding, indent mode, zoom, and theme name
6. **Breadcrumb bar** at the top showing the current file path with heading navigation

## Key Shortcuts to Try

| Action          | macOS | Windows/Linux |
| --------------- | ----- | ------------- |
| Command Palette | ⌘⇧P   | Ctrl+Shift+P  |
| Open File       | ⌘O    | Ctrl+O        |
| Open Folder     | ⌘⇧O   | Ctrl+Shift+O  |
| Source View     | ⌘1    | Ctrl+1        |
| Split View      | ⌘2    | Ctrl+2        |
| Preview View    | ⌘3    | Ctrl+3        |
| Toggle Sidebar  | ⌘B    | Ctrl+B        |
| Find            | ⌘F    | Ctrl+F        |
| Settings        | ⌘,    | Ctrl+,        |

## Opening a Workspace

1. **File → Open Folder** (⌘⇧O) to open a project directory
2. The file tree populates automatically
3. Click any `.md` file to open it in the editor
4. The preview updates live as you type

## Changing Themes

1. Click the **Themes** icon in the Activity Bar (paint palette icon)
2. Browse the 8 built-in themes
3. Click any theme card to preview and apply it instantly
4. Press **Escape** to close the Theme Gallery

## Running Tests After Changes

```bash
# Full build + test cycle
cmake --build build/debug -j$(sysctl -n hw.ncpu) && cd build/debug && ctest --output-on-failure
```

## Data Reset

MarkAmp stores configuration and workspace state. To reset:

```bash
# macOS — remove config and cached data
rm -rf ~/Library/Application\ Support/MarkAmp/

# Linux
rm -rf ~/.config/MarkAmp/

# Windows (PowerShell)
Remove-Item -Recurse "$env:APPDATA\MarkAmp"
```
