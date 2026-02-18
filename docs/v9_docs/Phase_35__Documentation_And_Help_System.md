# Phase 35: Documentation and Help System

## Overview
WalkthroughPanel and StartupPanel exist in UI. ShortcutOverlay exists for keyboard shortcut reference. However, there is no integrated help system: no in-app documentation, no command reference, no feature tutorials, no tooltip help. This phase builds a complete help and documentation experience.

## Prerequisites
- Phase 06 (Workbench navigation for help sidebar)
- Phase 18 (Tool window system for help panel)

## Tasks

### Task 1: Wire WalkthroughPanel for Interactive Tutorials
**Files:** `src/ui/WalkthroughPanel.cpp`, `src/ui/WalkthroughPanel.h`
**Description:** WalkthroughPanel exists. Wire it as an interactive tutorial system: step-by-step guides that highlight UI elements and guide user actions.
**Acceptance Criteria:**
- Walkthrough has steps with title, description, and action
- Step highlights relevant UI element (spotlight effect)
- "Try It" button performs the action
- Progress tracked per walkthrough
- Skip and restart options
- Walkthroughs: Getting Started, Editor Basics, Canvas, Notebooks, Graph

### Task 2: Wire StartupPanel Welcome Experience
**Files:** `src/ui/StartupPanel.cpp`, `src/ui/StartupPanel.h`
**Description:** StartupPanel exists. Wire it as the welcome screen: shown on launch with recent workspaces, quick actions, and getting started content.
**Acceptance Criteria:**
- Recent workspaces with click to open
- Quick actions: New Workspace, Open Folder, Clone Repository
- Getting Started section with walkthrough links
- "What's New" for current version changelog
- "Don't show again" toggle
- Theme-aware with product branding

### Task 3: Wire ShortcutOverlay for Keyboard Reference
**Files:** `src/ui/ShortcutOverlay.cpp`, `src/ui/ShortcutOverlay.h`
**Description:** ShortcutOverlay exists. Wire it as a comprehensive keyboard shortcut reference overlay (Cmd+K Cmd+S equivalent).
**Acceptance Criteria:**
- Full-screen overlay with all shortcuts
- Categorized: General, Editor, Canvas, Notebook, Graph, Navigation
- Search to filter shortcuts
- Current context shortcuts highlighted
- Custom shortcut display (user-modified shown differently)
- Print-friendly version

### Task 4: Wire In-App Command Reference
**Files:** `src/ui/MainFrame.cpp`
**Description:** "Help: Command Reference" shows all registered commands with descriptions, keyboard shortcuts, and when-conditions.
**Acceptance Criteria:**
- All commands listed alphabetically
- Each command: name, description, shortcut, when-clause
- Search and filter commands
- "Run" button to execute command
- Category filter (Editor, Canvas, Notebook, etc.)
- Export command list as Markdown

### Task 5: Wire Tooltip Help System
**Files:** `src/ui/ThemeAwareWindow.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** All toolbar buttons, panel tabs, and interactive elements show helpful tooltips with keyboard shortcut hints.
**Acceptance Criteria:**
- Tooltip delay: 500ms hover
- Tooltip content: action description + keyboard shortcut
- Extended tooltip on prolonged hover (1.5s): adds explanation
- Tooltips follow mouse position
- Tooltips use theme styling
- Tooltips accessible to screen readers

### Task 6: Wire Contextual Help Panel
**Files:** `src/ui/ToolWindowHost.cpp`
**Description:** Help panel in tool window area: shows contextual help for the currently focused element or active mode.
**Acceptance Criteria:**
- Help panel shows context-relevant documentation
- Editor mode: Markdown syntax reference
- Canvas mode: tool usage guide
- Notebook mode: magic command reference
- Graph mode: filter syntax help
- Content updates on focus change

### Task 7: Wire Markdown Syntax Reference
**Files:** `src/ui/ToolWindowHost.cpp`
**Description:** Built-in Markdown syntax reference: all supported syntax with live examples. Accessible from Help menu and command palette.
**Acceptance Criteria:**
- All supported Markdown elements documented
- Live examples with rendered preview
- MarkAmp extensions: wiki-links, task metadata, flashcard syntax, embeds
- Search within reference
- Copy example syntax
- Accessible via "Help: Markdown Reference"

### Task 8: Wire Feature Discovery Hints
**Files:** `src/core/Config.h`, `src/ui/NotificationManager.cpp`
**Description:** Contextual feature discovery: suggest features the user hasn't tried. Non-intrusive hints that appear based on usage patterns.
**Acceptance Criteria:**
- Hints triggered by user actions (e.g., "Try split view for side-by-side editing")
- Maximum 1 hint per session
- Hints dismissable and "don't show this again"
- Track shown hints to avoid repetition
- Hints for: split view, canvas, notebooks, graph, templates, keyboard shortcuts
- Configurable: enable/disable hints

### Task 9: Wire Release Notes Viewer
**Files:** `src/ui/MainFrame.cpp`
**Description:** "Help: What's New" shows release notes for the current version. Notes include: new features, improvements, bug fixes with screenshots.
**Acceptance Criteria:**
- Release notes rendered as Markdown
- Shown automatically after update (once)
- Accessible from Help menu
- Version history: browse previous versions
- Feature highlights with descriptions
- "Learn More" links to walkthroughs

### Task 10: Wire Settings Documentation
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Each setting in the Settings panel has a description, default value, and link to documentation.
**Acceptance Criteria:**
- Setting description shown below each setting
- Default value indicator
- "Learn more" link for complex settings
- Search settings by description text
- Settings categorized with section headers
- Reset to default button per setting

### Task 11: Wire Error Help Integration
**Files:** `src/core/DiagnosticsService.cpp`, `src/ui/ProblemsPanel.cpp`
**Description:** Error messages in Problems panel include "Learn More" links that open documentation explaining the error and how to fix it.
**Acceptance Criteria:**
- Each diagnostic type has a help article
- "Learn More" link in diagnostic message
- Help opens in contextual help panel
- Help content: what the error means, how to fix it, examples
- At least 20 documented error types
- Unknown errors: generic troubleshooting guide

### Task 12: Wire Keyboard Shortcut Customization UI
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Keyboard shortcut editor in settings: view all shortcuts, search, customize, reset to defaults. Conflict detection.
**Acceptance Criteria:**
- All shortcuts listed with current binding
- Click binding to record new shortcut
- Conflict detection: warn if shortcut already used
- "Reset" reverts individual shortcut
- "Reset All" reverts all shortcuts
- Custom shortcuts saved in `.markamp/keybindings.json`

### Task 13: Wire Interactive Playground
**Files:** `src/ui/MainFrame.cpp`
**Description:** "Help: Open Playground" creates a temporary workspace with interactive examples for each feature: Markdown formatting, wiki-links, tasks, canvas, notebooks.
**Acceptance Criteria:**
- Playground creates temporary workspace
- Pre-populated with interactive examples
- Each example: instructions, try-it area, expected result
- Covers: basic Markdown, extended syntax, wiki-links, tasks, templates
- Playground data not persisted (temporary)
- "Close Playground" returns to workspace

### Task 14: Wire API Documentation for Extensions
**Files:** `docs/api_reference.md`
**Description:** Comprehensive API documentation for extension developers: PluginContext methods, event types, contribution points, manifest schema.
**Acceptance Criteria:**
- PluginContext: all 25+ service pointers documented
- Event types: all events with fields and semantics
- Contribution points: all manifest sections
- Extension manifest: full schema with examples
- Best practices: performance, error handling, permissions
- Code examples for common patterns

### Task 15: Wire Help Search
**Files:** `src/ui/MainFrame.cpp`
**Description:** "Help: Search" searches across all documentation: command reference, syntax reference, settings descriptions, error help, walkthroughs.
**Acceptance Criteria:**
- Unified search across all help content
- Results categorized: Commands, Settings, Syntax, Errors, Tutorials
- Fuzzy search for typo tolerance
- Results ranked by relevance
- Keyboard shortcut: Cmd+Shift+?
- Search history maintained

### Task 16: Wire Onboarding Flow for New Users
**Files:** `src/ui/StartupPanel.cpp`, `src/ui/WalkthroughPanel.cpp`
**Description:** First-launch onboarding: welcome, choose theme, import settings, quick tour of features.
**Acceptance Criteria:**
- First launch: onboarding wizard
- Step 1: Welcome with product overview
- Step 2: Choose theme (light/dark + accent)
- Step 3: Import settings (from VS Code, Obsidian, or skip)
- Step 4: Quick feature tour (5 slides)
- Step 5: Open sample workspace or create new
- "Skip" available at any step

### Task 17: Wire Sample Content
**Files:** `src/core/SampleFiles.cpp`
**Description:** Sample content for demonstrations: Markdown showcase, canvas board examples, notebook examples, graph-ready linked documents.
**Acceptance Criteria:**
- Markdown Showcase: all supported syntax demonstrated
- Canvas Showcase: board with various object types
- Notebook Showcase: code cells with outputs
- Knowledge Base: 10+ linked documents for graph demo
- Flashcard Demo: sample deck with cards
- All samples well-written and informative

### Task 18: Wire Help Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register help commands: "Help: Getting Started", "Help: Keyboard Shortcuts", "Help: Markdown Reference", "Help: What's New", "Help: Search", "Help: Report Issue", "Help: Open Playground".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Help:" prefix
- Commands accessible from Help menu
- "Report Issue" opens issue template
- Keyboard shortcut for "Keyboard Shortcuts" (Cmd+K Cmd+S)

### Task 19: Wire Help Theme Integration
**Files:** `src/ui/WalkthroughPanel.cpp`, `src/ui/StartupPanel.cpp`, `src/ui/ShortcutOverlay.cpp`
**Description:** All help UI uses theme tokens: walkthrough panels, startup screen, shortcut overlay, help panel.
**Acceptance Criteria:**
- Help content uses theme text and background colors
- Code examples use theme syntax highlighting
- Spotlight effect uses theme accent
- All 64 themes render help correctly
- Dark mode: help content readable

### Task 20: Add Help System Tests
**Files:** `tests/unit/test_help_system.cpp`
**Description:** Test help system: walkthrough steps, shortcut overlay content, sample files creation, onboarding flow.
**Acceptance Criteria:**
- Walkthrough: all steps have valid targets
- Shortcut overlay: all shortcuts present
- Sample files: created correctly in workspace
- Help search: returns relevant results
- Onboarding: all steps complete without error

## Testing Requirements
- Walkthrough steps target valid UI elements
- Shortcut overlay matches ShortcutManager
- Sample files are valid Markdown/JSON
- Help search returns relevant results

## Phase Completion Criteria
- Interactive walkthroughs for major features
- Welcome screen with recent workspaces
- Keyboard shortcut overlay and customization
- Contextual help panel
- Markdown syntax reference
- Onboarding flow for new users
- All tests pass
