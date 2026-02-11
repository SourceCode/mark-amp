# Phase 30: Integration Testing, Documentation, and Release Prep

## Objective

Conduct comprehensive end-to-end integration testing of all features, write user-facing documentation, create a complete test plan, fix any remaining issues, and prepare the v1.0.0 release. This is the final phase before the public release of MarkAmp.

## Prerequisites

- Phase 29 (Build Pipeline, Packaging, and Distribution)

## Deliverables

1. Complete integration test suite covering all user workflows
2. End-to-end smoke test automation
3. User documentation (README, user guide, keyboard shortcuts reference)
4. Developer documentation (architecture guide, contributing guide)
5. v1.0.0 release notes
6. Final release candidate build

## Tasks

### Task 30.1: Create end-to-end integration test plan

Create `/Users/ryanrentfro/code/markamp/tests/integration/test_plan.md`:

**Test scenarios covering all major user workflows:**

**Workflow 1: First Launch Experience**
1. Launch MarkAmp for the first time
2. Verify: default theme (Midnight Neon) is applied
3. Verify: sample file tree is loaded in sidebar
4. Verify: README.md is selected and displayed
5. Verify: split view mode is active
6. Verify: status bar shows "READY", cursor position, encoding

**Workflow 2: File Editing**
1. Click a file in the sidebar
2. Verify: file content loads in the editor
3. Type text in the editor
4. Verify: preview updates after debounce
5. Verify: status bar shows updated cursor position
6. Verify: file is marked as modified (dirty indicator)
7. Save the file (Ctrl+S)
8. Verify: dirty indicator clears
9. Undo the save (Ctrl+Z)
10. Verify: dirty indicator returns

**Workflow 3: Theme Management**
1. Open the theme gallery (toolbar button)
2. Verify: all 8 themes are displayed
3. Click a different theme
4. Verify: entire application updates to new theme
5. Verify: editor, preview, chrome, sidebar all use new colors
6. Export a theme (click export button)
7. Verify: JSON file is saved
8. Import the exported theme
9. Verify: theme appears in gallery
10. Close gallery (Escape)

**Workflow 4: View Mode Switching**
1. Click SRC button
2. Verify: only editor is visible
3. Click SPLIT button
4. Verify: editor and preview are side-by-side
5. Click VIEW button
6. Verify: only preview is visible
7. Verify: content is consistent across all modes
8. Use keyboard shortcuts (Ctrl+1, Ctrl+2, Ctrl+3)
9. Verify: same behavior as clicking buttons

**Workflow 5: Markdown Features**
1. Open a file with headings, lists, code blocks, tables, task lists
2. Verify: all elements render correctly in preview
3. Open a file with Mermaid diagrams
4. Verify: diagrams render as SVG
5. Edit a Mermaid block with invalid syntax
6. Verify: error overlay appears
7. Fix the syntax
8. Verify: diagram renders again

**Workflow 6: File Management**
1. Open a folder (Ctrl+Shift+O)
2. Verify: file tree populates with folder contents
3. Navigate the file tree with keyboard (arrow keys)
4. Expand and collapse folders
5. Select a file
6. Create a new file (if supported)
7. Save as to a new location (Ctrl+Shift+S)

**Workflow 7: Find and Replace**
1. Open find bar (Ctrl+F)
2. Type a search term
3. Verify: matches are highlighted
4. Navigate with Next/Previous
5. Open replace mode (Ctrl+H)
6. Replace a match
7. Replace all
8. Close find bar (Escape)

**Workflow 8: Sidebar Management**
1. Toggle sidebar (Ctrl+B or sidebar toggle button)
2. Verify: sidebar hides, content area expands
3. Toggle again
4. Verify: sidebar appears at previous width
5. Drag sidebar divider
6. Verify: sidebar resizes
7. Verify: width persists after toggle hide/show

**Workflow 9: Keyboard Navigation**
1. Use Tab to cycle focus through zones
2. Navigate sidebar with arrow keys
3. Open a file with Enter
4. Use Ctrl+F for find
5. Use Escape to close find
6. Use F1 for shortcut reference
7. Verify: all operations work without mouse

**Workflow 10: Performance Under Load**
1. Open a 10,000-line markdown file
2. Type in the editor
3. Verify: no perceptible lag
4. Scroll through the preview
5. Verify: smooth scrolling
6. Switch themes multiple times
7. Verify: instant switching

**Acceptance criteria:**
- All 10 workflows documented with specific verification steps
- Each step has a clear pass/fail criterion

### Task 30.2: Implement automated integration tests

Create `/Users/ryanrentfro/code/markamp/tests/integration/test_e2e.cpp`:

Implement automated versions of the test workflows using wxWidgets test utilities:

```cpp
#include <catch2/catch_all.hpp>
#include <wx/uiaction.h>

TEST_CASE("End-to-end: First Launch", "[e2e]")
{
    // Create app in test mode
    TestApp app;
    app.Initialize();

    SECTION("Default theme is applied")
    {
        auto& engine = app.GetThemeEngine();
        REQUIRE(engine.current_theme().id == "midnight-neon");
    }

    SECTION("Sample files are loaded")
    {
        auto& state = app.GetAppState();
        REQUIRE(state.file_tree.size() > 0);
        REQUIRE(state.active_file_id == "readme");
    }

    SECTION("Split view is default")
    {
        auto& state = app.GetAppState();
        REQUIRE(state.view_mode == ViewMode::Split);
    }
}

TEST_CASE("End-to-end: Theme Switching", "[e2e]")
{
    TestApp app;
    app.Initialize();

    for (const auto& theme_info : app.GetThemeEngine().available_themes()) {
        SECTION("Apply theme: " + theme_info.name)
        {
            app.GetThemeEngine().apply_theme(theme_info.id);
            REQUIRE(app.GetThemeEngine().current_theme().id == theme_info.id);
            // Verify no exceptions or crashes
        }
    }
}

TEST_CASE("End-to-end: Edit and Save", "[e2e]")
{
    TestApp app;
    app.Initialize();

    // Set content
    app.GetEditor().SetContent("# Test\n\nHello world");
    REQUIRE(app.GetEditor().IsModified());

    // Verify preview updates (after debounce)
    app.ProcessPendingEvents();
    // ... verify preview content

    // Save
    // ... verify modified flag clears
}
```

**Test infrastructure:**
- Create a `TestApp` class that initializes the application without showing a window
- Or use `wxAppConsole` for headless testing
- Mock file system for deterministic tests
- Use `wxUIActionSimulator` for simulating user input (where available)

**Acceptance criteria:**
- At least 30 automated integration test assertions
- Tests cover all major workflows
- Tests can run in CI (headless)
- No test depends on external state (deterministic)

### Task 30.3: Create smoke test script

Create `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`:

A quick sanity check that runs against a built binary:

```bash
#!/usr/bin/env bash
set -euo pipefail

BINARY="./build/markamp"

echo "=== MarkAmp Smoke Test ==="

# Test 1: Binary exists and is executable
echo "Test 1: Binary exists..."
test -x "$BINARY" || { echo "FAIL: Binary not found"; exit 1; }

# Test 2: Version output
echo "Test 2: Version output..."
"$BINARY" --version 2>/dev/null | grep -q "MarkAmp v" || echo "WARN: No version output"

# Test 3: Help output
echo "Test 3: Help output..."
"$BINARY" --help 2>/dev/null | grep -q "Usage" || echo "WARN: No help output"

# Test 4: Open and close without crash
echo "Test 4: Launch and exit..."
timeout 5 "$BINARY" --headless --quit-after-init 2>/dev/null || echo "INFO: Headless mode may not be supported"

# Test 5: Process sample files
echo "Test 5: Process sample files..."
for f in resources/sample_files/*.md; do
    "$BINARY" --render-to-html "$f" > /dev/null 2>&1 || echo "WARN: Failed to process $f"
done

echo "=== Smoke Test Complete ==="
```

Note: This requires implementing `--version`, `--help`, `--headless`, `--quit-after-init`, and `--render-to-html` CLI flags for the binary (useful for testing and automation).

**Acceptance criteria:**
- Smoke test runs without crashing
- Basic functionality is verified
- Script works in CI

### Task 30.4: Write user documentation

Create `/Users/ryanrentfro/code/markamp/docs/user_guide.md`:

**Contents:**

1. **Getting Started**
   - Installation (macOS, Windows, Linux)
   - First launch
   - Opening a file or folder
   - Understanding the interface

2. **The Editor**
   - Writing markdown
   - View modes (Source, Split, Preview)
   - Find and replace
   - Line numbers and word wrap
   - Undo/redo

3. **Markdown Features**
   - Headings
   - Emphasis (bold, italic, strikethrough)
   - Lists (ordered, unordered, task lists)
   - Code blocks (with syntax highlighting)
   - Tables
   - Blockquotes
   - Links and images
   - Mermaid diagrams
   - Footnotes

4. **Themes**
   - Built-in themes
   - Switching themes
   - Importing themes
   - Exporting themes
   - Creating custom themes

5. **Keyboard Shortcuts**
   - Complete reference table
   - Platform-specific notes

6. **Settings**
   - Editor settings
   - Appearance settings
   - File settings

7. **Troubleshooting**
   - Common issues and solutions
   - Reporting bugs
   - Getting support

**Acceptance criteria:**
- User guide covers all features
- Written in clear, non-technical language
- Includes screenshots or descriptions of key UI elements
- Organized for quick reference

### Task 30.5: Write developer documentation

Create the following developer-facing documents:

**`/Users/ryanrentfro/code/markamp/docs/architecture.md`:**
- System architecture overview
- Module dependency diagram
- Event bus and message flow
- Theme system architecture
- Rendering pipeline architecture
- File system integration

**`/Users/ryanrentfro/code/markamp/CONTRIBUTING.md`:**
- How to set up the development environment
- Code style and conventions
- How to add new features
- How to add new themes
- How to add new syntax highlighting languages
- Testing requirements
- Pull request process

**`/Users/ryanrentfro/code/markamp/docs/api_reference.md`:**
- Core interfaces (IThemeEngine, IFileSystem, IMarkdownParser, IMermaidRenderer)
- Event types and their data
- Configuration keys and defaults
- Theme JSON format specification

**Acceptance criteria:**
- Architecture is clearly documented
- A new contributor can understand the codebase structure
- API contracts are documented

### Task 30.6: Create release notes

Create `/Users/ryanrentfro/code/markamp/CHANGELOG.md`:

```markdown
# Changelog

## [1.0.0] - 2026-XX-XX

### Added
- Cross-platform Markdown editor built with C++26 and wxWidgets 3.2.9
- GitHub Flavored Markdown support (headings, emphasis, lists, tables, code blocks, task lists, footnotes)
- Mermaid diagram rendering with error handling
- 8 built-in themes (Midnight Neon, Cyber Night, Solarized Dark, Classic Mono, High Contrast Blue, Matrix Core, Classic Amp, Vapor Wave)
- Theme import/export (JSON format)
- Theme gallery with live preview cards
- Custom retro-futuristic window chrome
- Three view modes: Source, Split, Preview
- Sidebar file tree with folder navigation
- Syntax highlighting for 15+ programming languages
- Find and replace
- Line numbers, word wrap, bracket matching
- Auto-indent for markdown structures
- Keyboard shortcuts for all major operations
- Accessibility: screen reader support, high contrast, scalable UI
- Status bar with cursor position, encoding, Mermaid status, theme name
- Custom scrollbar styling
- JetBrains Mono + Rajdhani font pairing
- Sub-500ms cold start
- Available for macOS, Windows, and Linux

### Known Issues
- (Document any known issues here)
```

**Acceptance criteria:**
- Changelog lists all features
- Follows Keep a Changelog format
- Known issues are honestly documented

### Task 30.7: Final release candidate build

Produce the release candidate:

1. Run the full test suite on all platforms
2. Run performance benchmarks and verify targets
3. Run security tests
4. Run accessibility audit
5. Build release packages for all platforms
6. Test each package on a clean system:
   - macOS: fresh macOS VM or clean user account
   - Windows: fresh Windows VM
   - Linux: clean Docker container
7. Verify all features work in the installed version
8. Document any remaining issues

**Release checklist:**
- [ ] All unit tests pass on macOS, Windows, Linux
- [ ] All integration tests pass
- [ ] Performance targets met
- [ ] Security tests pass
- [ ] Accessibility audit complete
- [ ] macOS .dmg installs and runs on clean system
- [ ] **macOS App Store .pkg installs and runs in sandboxed mode**
- [ ] **macOS App Store .pkg passes `xcrun altool --validate-app`**
- [ ] **No App Sandbox violations in Console.app**
- [ ] Windows installer installs and runs on clean system
- [ ] **Windows MSIX package passes Windows App Certification Kit (WACK)**
- [ ] **Windows MSIX installs from package and runs correctly**
- [ ] Linux .deb installs and runs on clean system
- [ ] Linux AppImage runs on clean system
- [ ] File associations work on all platforms (including Store-installed versions)
- [ ] Keyboard shortcuts work on all platforms
- [ ] All 8 themes render correctly on all platforms
- [ ] Mermaid diagrams render on all platforms
- [ ] Theme import/export works cross-platform
- [ ] **UI visually matches React reference implementation across all 8 themes (screenshot comparison)**
- [ ] No memory leaks (ASAN clean)
- [ ] No undefined behavior (UBSAN clean)
- [ ] Documentation is complete and accurate
- [ ] CHANGELOG is up to date
- [ ] Version numbers are correct everywhere
- [ ] **Mac App Store metadata prepared (screenshots, description, keywords)**
- [ ] **Microsoft Store metadata prepared (screenshots, description, category)**
- [ ] **Privacy policy page published at support URL**

**Acceptance criteria:**
- Release candidate passes all checklist items
- No P0 or P1 bugs remain open
- Documentation is complete

### Task 30.8: Submit to Mac App Store

1. Build the Mac App Store .pkg with sandbox entitlements
2. Validate with `xcrun altool --validate-app`
3. Upload to App Store Connect via `xcrun altool --upload-app` or Transporter
4. In App Store Connect:
   - Fill in app description, keywords, categories
   - Upload 5 screenshots (2560x1600 Retina) showing different themes
   - Set pricing (Free or paid)
   - Set availability (all territories)
   - Submit for review
5. Monitor review status and respond to any reviewer feedback

**Acceptance criteria:**
- App passes App Store Connect validation
- App is submitted for review
- All metadata and screenshots are complete
- Response plan exists for common rejection reasons

### Task 30.9: Submit to Microsoft Store

1. Build the MSIX package
2. Run Windows App Certification Kit (WACK) and fix any failures
3. In Microsoft Partner Center:
   - Create app submission
   - Upload MSIX package
   - Fill in Store listing (description, screenshots, category)
   - Set pricing and availability
   - Submit for certification
4. Monitor certification status and respond to any feedback

**Acceptance criteria:**
- MSIX passes WACK certification
- App is submitted to Partner Center
- All Store listing metadata is complete
- Response plan exists for common certification failures

### Task 30.10: Tag and publish v1.0.0 to GitHub

Final release steps:

1. Bump version to 1.0.0 in all files
2. Commit the version bump
3. Create git tag `v1.0.0`
4. Push tag to trigger release workflow
5. Verify all packages are built and uploaded to GitHub Releases
6. Write GitHub Release description with:
   - Highlight features
   - Download links for each platform (DMG, NSIS, portable ZIP, .deb, AppImage)
   - Links to Mac App Store and Microsoft Store listings
   - Installation instructions
   - Link to documentation
   - Link to changelog
7. Announce the release

**Acceptance criteria:**
- Git tag `v1.0.0` exists
- GitHub Release has all platform packages
- Release description is complete and helpful
- Download links work
- Mac App Store and Microsoft Store links are included
- Installation instructions are accurate

## Files Created/Modified

| File | Action |
|---|---|
| `tests/integration/test_plan.md` | Created |
| `tests/integration/test_e2e.cpp` | Created |
| `scripts/smoke_test.sh` | Created |
| `docs/user_guide.md` | Created |
| `docs/architecture.md` | Created |
| `docs/api_reference.md` | Created |
| `CONTRIBUTING.md` | Created |
| `CHANGELOG.md` | Created |
| `tests/CMakeLists.txt` | Modified (add integration tests) |

## Dependencies

- All prior phases complete
- Clean test environments for each platform
- GitHub repository for release hosting

## Estimated Complexity

**High** -- The scope of integration testing is large. Writing comprehensive documentation takes significant effort. The release process must be executed carefully across all platforms.
