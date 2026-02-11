# MarkAmp Cross-Platform Test Matrix

## Platform Configurations

| ID  | Platform     | Version       | Display           | DPI | Status      |
| --- | ------------ | ------------- | ----------------- | --- | ----------- |
| M1  | macOS 12+    | Apple Silicon | Retina (2×)       | 144 | Primary dev |
| M2  | macOS 12+    | Apple Silicon | External (1×)     | 72  | —           |
| W1  | Windows 10   | x64           | Standard (100%)   | 96  | —           |
| W2  | Windows 11   | x64           | High DPI (150%)   | 144 | —           |
| W3  | Windows 11   | x64           | High DPI (200%)   | 192 | —           |
| L1  | Ubuntu 22.04 | x64           | Standard, Wayland | 96  | —           |
| L2  | Ubuntu 22.04 | x64           | Standard, X11     | 96  | —           |
| L3  | Ubuntu 22.04 | x64           | High DPI (2×)     | 192 | —           |
| L4  | Fedora 38+   | x64           | Standard, Wayland | 96  | —           |

## Test Categories

### 1. Window Management

| Test Case          | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ------------------ | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Open/close         |     |     |     |     |     |     |     |     |     |
| Minimize/restore   |     |     |     |     |     |     |     |     |     |
| Maximize/restore   |     |     |     |     |     |     |     |     |     |
| Resize (all edges) |     |     |     |     |     |     |     |     |     |
| Fullscreen toggle  |     |     |     |     |     |     |     |     |     |
| Multi-monitor move |     |     |     |     |     |     |     |     |     |

### 2. Custom Chrome

| Test Case              | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ---------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Title bar drag         |     |     |     |     |     |     |     |     |     |
| Edge resize handles    |     |     |     |     |     |     |     |     |     |
| Close/min/max buttons  |     |     |     |     |     |     |     |     |     |
| Double-click title bar |     |     |     |     |     |     |     |     |     |
| Chrome height correct  |     |     |     |     |     |     |     |     |     |

### 3. Theme Rendering

| Test Case                     | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ----------------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Midnight Neon                 |     |     |     |     |     |     |     |     |     |
| Cyber Night                   |     |     |     |     |     |     |     |     |     |
| Solarized Dark                |     |     |     |     |     |     |     |     |     |
| Classic Mono                  |     |     |     |     |     |     |     |     |     |
| High Contrast Blue            |     |     |     |     |     |     |     |     |     |
| Matrix Core                   |     |     |     |     |     |     |     |     |     |
| Classic Amp                   |     |     |     |     |     |     |     |     |     |
| Vapor Wave                    |     |     |     |     |     |     |     |     |     |
| Instant hot-swap (no flicker) |     |     |     |     |     |     |     |     |     |

### 4. Font Rendering

| Test Case               | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ----------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| JetBrains Mono (editor) |     |     |     |     |     |     |     |     |     |
| Rajdhani (UI text)      |     |     |     |     |     |     |     |     |     |
| Fallback font (missing) |     |     |     |     |     |     |     |     |     |
| Anti-aliasing quality   |     |     |     |     |     |     |     |     |     |
| Bold/italic variants    |     |     |     |     |     |     |     |     |     |

### 5. Editor Functionality

| Test Case              | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ---------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Typing (ASCII)         |     |     |     |     |     |     |     |     |     |
| Typing (Unicode/CJK)   |     |     |     |     |     |     |     |     |     |
| Selection (mouse)      |     |     |     |     |     |     |     |     |     |
| Selection (keyboard)   |     |     |     |     |     |     |     |     |     |
| Scrolling (large file) |     |     |     |     |     |     |     |     |     |
| Line numbers visible   |     |     |     |     |     |     |     |     |     |
| Word wrap toggle       |     |     |     |     |     |     |     |     |     |
| Find/replace           |     |     |     |     |     |     |     |     |     |

### 6. Preview Rendering

| Test Case               | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ----------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Headings (h1–h6)        |     |     |     |     |     |     |     |     |     |
| Code blocks + syntax HL |     |     |     |     |     |     |     |     |     |
| Tables (GFM)            |     |     |     |     |     |     |     |     |     |
| Task lists              |     |     |     |     |     |     |     |     |     |
| Images (local)          |     |     |     |     |     |     |     |     |     |
| Mermaid diagrams        |     |     |     |     |     |     |     |     |     |
| Footnotes               |     |     |     |     |     |     |     |     |     |
| Links (clickable)       |     |     |     |     |     |     |     |     |     |

### 7. File Operations

| Test Case               | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ----------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Open file (dialog)      |     |     |     |     |     |     |     |     |     |
| Save file               |     |     |     |     |     |     |     |     |     |
| Open folder (file tree) |     |     |     |     |     |     |     |     |     |
| Path with spaces        |     |     |     |     |     |     |     |     |     |
| Path with Unicode       |     |     |     |     |     |     |     |     |     |
| Long path (200+ chars)  |     |     |     |     |     |     |     |     |     |

### 8. Keyboard Shortcuts

| Test Case            | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| -------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Cmd/Ctrl+O (open)    |     |     |     |     |     |     |     |     |     |
| Cmd/Ctrl+S (save)    |     |     |     |     |     |     |     |     |     |
| Cmd/Ctrl+F (find)    |     |     |     |     |     |     |     |     |     |
| View mode toggle     |     |     |     |     |     |     |     |     |     |
| Theme gallery        |     |     |     |     |     |     |     |     |     |
| Shortcut overlay (?) |     |     |     |     |     |     |     |     |     |

### 9. Accessibility

| Test Case                | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ------------------------ | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Screen reader announces  |     |     |     |     |     |     |     |     |     |
| High contrast mode       |     |     |     |     |     |     |     |     |     |
| Reduced motion respected |     |     |     |     |     |     |     |     |     |
| Tab navigation           |     |     |     |     |     |     |     |     |     |
| ARIA labels on controls  |     |     |     |     |     |     |     |     |     |

### 10. Performance

| Test Case              | M1  | M2  | W1  | W2  | W3  | L1  | L2  | L3  | L4  |
| ---------------------- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Cold start < 500ms     |     |     |     |     |     |     |     |     |     |
| Large file (10MB) open |     |     |     |     |     |     |     |     |     |
| Mermaid render < 2s    |     |     |     |     |     |     |     |     |     |
| Theme switch < 50ms    |     |     |     |     |     |     |     |     |     |
| Scroll 60fps           |     |     |     |     |     |     |     |     |     |

## Legend

| Symbol | Meaning                    |
| ------ | -------------------------- |
| ✅     | Pass                       |
| ❌     | Fail (bug filed)           |
| ⚠️     | Pass with known limitation |
| N/A    | Not applicable             |
| —      | Not yet tested             |

## Visual Consistency Notes

### Acceptable Differences

- Font rendering (ClearType vs subpixel vs macOS native)
- Native file dialog appearance
- Scrollbar momentum (macOS native)

### Unacceptable Differences

- Missing UI elements on any platform
- Incorrect theme colors
- Layout broken (overlapping, clipping)
- Missing functionality
