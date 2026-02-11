# Phase 28: Cross-Platform Testing and Polish

## Objective

Systematically test MarkAmp on all three target platforms (macOS, Windows, Linux), identify and fix platform-specific issues, ensure visual consistency, and polish the application for release quality. This phase is the final quality gate before packaging.

## Prerequisites

- All phases 01-27 must be substantially complete

## Deliverables

1. Platform-specific bug fixes
2. Visual consistency audit across platforms
3. Font rendering verification
4. DPI/scaling testing on all platforms
5. File system path handling verification
6. Platform-specific feature polishing
7. Cross-platform test matrix documentation

## Tasks

### Task 28.1: Create cross-platform test matrix

Create `/Users/ryanrentfro/code/markamp/docs/v1_docs/test_matrix.md`:

Define the complete test matrix:

**Platforms:**
| Platform | Version | Display | DPI |
|---|---|---|---|
| macOS 12+ (Monterey) | Apple Silicon + Intel | Retina (2x) | 144 DPI |
| macOS 12+ | Apple Silicon + Intel | External (1x) | 72 DPI |
| Windows 10 | x64 | Standard (100%) | 96 DPI |
| Windows 11 | x64 | High DPI (150%) | 144 DPI |
| Windows 11 | x64 | High DPI (200%) | 192 DPI |
| Ubuntu 22.04 | x64, Wayland | Standard | 96 DPI |
| Ubuntu 22.04 | x64, X11 | Standard | 96 DPI |
| Ubuntu 22.04 | x64 | High DPI (2x) | 192 DPI |
| Fedora 38+ | x64, Wayland | Standard | 96 DPI |

**Test categories:**
1. Window management (open, close, minimize, maximize, resize, fullscreen)
2. Custom chrome (drag, resize from edges, window controls)
3. Theme rendering (all 8 themes, visual comparison)
4. Font rendering (JetBrains Mono, Rajdhani, fallbacks)
5. Editor functionality (typing, scrolling, selection, find/replace)
6. Preview rendering (markdown, code blocks, tables, Mermaid)
7. File operations (open, save, file dialog, path handling)
8. Keyboard shortcuts (platform-appropriate modifiers)
9. Accessibility (screen reader, high contrast)
10. Performance (cold start, large files, Mermaid rendering)

**Acceptance criteria:**
- Test matrix is complete and documented
- All cells have pass/fail/N/A status
- Failed cells have associated bug tickets

### Task 28.2: Fix macOS-specific issues

Test and fix macOS-specific behaviors:

**Known areas requiring macOS attention:**
1. **Menu bar integration:**
   - macOS expects an application menu bar (even with custom chrome)
   - Create a native menu bar with File, Edit, View, Window, Help menus
   - Application menu: About, Preferences, Quit (standard macOS items)
2. **Fullscreen mode:**
   - Support native macOS fullscreen (green traffic light button)
   - Custom chrome must adapt to fullscreen (hide window controls)
   - Transition animation should be native
3. **File associations:**
   - Register `.md` and `.markdown` file associations in Info.plist
   - Handle `apple-event` for file open from Finder
4. **Dark mode integration:**
   - Detect macOS dark mode via `NSAppearance`
   - Auto-switch to a dark theme when system switches to dark mode
5. **Touch Bar support** (if applicable):
   - View mode buttons
   - Theme quick-switch
6. **Objective-C++ compilation:**
   - Ensure `.mm` files compile correctly
   - Verify memory management (ARC or manual)
7. **Sandbox entitlements** (for App Store):
   - File access entitlements
   - Network denial entitlement

**Acceptance criteria:**
- All macOS-specific features work correctly
- Application feels native on macOS
- No Objective-C++ memory leaks
- Fullscreen mode works smoothly

### Task 28.3: Fix Windows-specific issues

Test and fix Windows-specific behaviors:

**Known areas requiring Windows attention:**
1. **Window management:**
   - Snap layouts (Windows 11): respond to `WM_GETMINMAXINFO`
   - Taskbar thumbnail preview
   - Jump list for recent files
2. **DPI awareness:**
   - Per-monitor DPI awareness (V2)
   - Handle DPI change events when moving between monitors
   - Scale all custom-drawn elements correctly
3. **High DPI rendering:**
   - Test at 100%, 125%, 150%, 175%, 200% scaling
   - Verify icon sharpness at all scales
   - Verify font rendering at all scales
4. **File paths:**
   - Handle long paths (> 260 characters) with `\\?\` prefix
   - Handle UNC paths (network shares)
   - Handle paths with spaces and Unicode characters
5. **Font rendering:**
   - DirectWrite rendering for anti-aliased text
   - ClearType settings respect
6. **Installer integration:**
   - File association in registry for `.md` files
   - "Open with" context menu integration
   - Uninstall registry entries
7. **Windows Defender / SmartScreen:**
   - Code signing to avoid SmartScreen warnings
   - Manifest with correct compatibility settings

**Acceptance criteria:**
- Application works correctly on Windows 10 and 11
- DPI handling is correct at all scale factors
- File associations work
- No SmartScreen warnings (when properly signed)

### Task 28.4: Fix Linux-specific issues

Test and fix Linux-specific behaviors:

**Known areas requiring Linux attention:**
1. **Display server compatibility:**
   - Test on both X11 and Wayland
   - Custom chrome drag may behave differently on Wayland
   - wxWidgets Wayland support may have limitations
2. **GTK theme integration:**
   - Verify that MarkAmp's custom theme does not conflict with GTK themes
   - File dialogs should use native GTK dialogs
3. **Font fallbacks:**
   - Verify fallback fonts are available (DejaVu Sans Mono, Noto Sans)
   - Handle systems where fonts are not installed
   - Use fontconfig to query available fonts
4. **Desktop integration:**
   - `.desktop` file for application launcher
   - Icon installation to standard XDG icon directories
   - File association via MIME types
   - Follows XDG Base Directory Specification for config/data
5. **Package manager dependencies:**
   - Document required system packages (GTK3, WebKit2GTK, etc.)
   - Handle missing dependencies gracefully (error message, not crash)
6. **Clipboard integration:**
   - Test copy/paste between MarkAmp and other applications
   - Test on both X11 (xclip/xsel) and Wayland (wl-copy/wl-paste)

**Acceptance criteria:**
- Application works on Ubuntu 22.04 and Fedora 38+
- Works on both X11 and Wayland
- File dialogs are native
- Desktop integration files are correct

### Task 28.5: Visual consistency audit

Compare the rendering across all three platforms:

**Process:**
1. Open the same document on each platform
2. Take screenshots with each of the 8 themes
3. Compare: font rendering, colors, spacing, bevel effects, icons
4. Document differences and fix significant inconsistencies

**Areas likely to differ:**
- Font rendering (ClearType vs subpixel vs antialiased)
- Scrollbar appearance (custom scrollbars may render differently)
- Color profiles (display color space may differ)
- Border rendering (sub-pixel differences)
- Icon rendering (anti-aliasing differences)

**Acceptable differences:**
- Slight font rendering differences (platform-native is preferred)
- Native file dialog appearance (expected to differ)
- Scrollbar behavior (native scrollbar momentum on macOS)

**Unacceptable differences:**
- Missing UI elements
- Incorrect colors
- Layout broken (overlapping, clipping)
- Missing functionality

**Acceptance criteria:**
- All platforms render the same content with the same theme
- No missing UI elements on any platform
- Visual differences are documented and accepted or fixed

### Task 28.6: DPI and scaling regression testing

Systematically test all DPI/scaling configurations:

**Test procedure for each configuration:**
1. Launch application
2. Verify: chrome height, toolbar height, status bar height
3. Verify: sidebar width, font sizes, icon sizes
4. Verify: file tree item height, indentation
5. Verify: editor font size, line spacing
6. Verify: preview font size, spacing
7. Verify: theme gallery card sizes, grid layout
8. Move window between monitors with different DPI (if multi-monitor)
9. Change system DPI while application is running (if supported)

**Acceptance criteria:**
- No clipping or overlapping at any DPI
- Text is readable at all supported DPI levels
- Icons are sharp (not blurry) at high DPI
- Layout proportions are maintained

### Task 28.7: Edge case and stress testing

Test unusual scenarios:

1. **Rapid theme switching:** Switch themes 100 times rapidly -- no crash, no memory leak
2. **Large number of themes:** Import 100 custom themes -- gallery handles gracefully
3. **Very long filenames:** 200-character filenames -- sidebar handles with ellipsis
4. **Deeply nested directories:** 20 levels deep -- file tree handles correctly
5. **Empty file:** Open a 0-byte file -- no crash
6. **Binary file:** Open a PNG file as text -- graceful error
7. **No disk space:** Save when disk is full -- error dialog, no crash
8. **Read-only file:** Edit and try to save -- error dialog, offer Save As
9. **File locked by another process:** Open locked file -- error dialog
10. **System font changes:** Change system font while app is running -- app adapts
11. **Screen resolution change:** Change resolution while app is running -- app adapts
12. **Multiple instances:** Run two copies of MarkAmp -- no conflicts

**Acceptance criteria:**
- No crashes from any edge case
- All error conditions show user-friendly messages
- Application recovers gracefully from all tested scenarios

### Task 28.8: Document platform-specific notes

Update `/Users/ryanrentfro/code/markamp/docs/v1_docs/platform_notes.md`:

Document for each platform:
1. Build instructions
2. Known quirks and workarounds
3. Platform-specific features
4. Missing features (if any)
5. Performance characteristics
6. Required system dependencies

**Acceptance criteria:**
- Document is comprehensive and accurate
- A new developer can build and test on each platform using the documentation

## Files Created/Modified

| File | Action |
|---|---|
| Various source files | Bug fixes |
| `src/platform/MacPlatform.mm` | Modified (macOS fixes) |
| `src/platform/WinPlatform.cpp` | Modified (Windows fixes) |
| `src/platform/LinuxPlatform.cpp` | Modified (Linux fixes) |
| `docs/v1_docs/test_matrix.md` | Created |
| `docs/v1_docs/platform_notes.md` | Created |

## Dependencies

- Access to all three platforms for testing
- Multiple display configurations for DPI testing
- Screen readers for accessibility testing (NVDA/JAWS on Windows, VoiceOver on macOS, Orca on Linux)

## Estimated Complexity

**High** -- Cross-platform testing is labor-intensive and frequently reveals subtle issues. Platform-specific fixes often require deep knowledge of the platform's APIs and behaviors. The test matrix is large and must be systematically executed.
