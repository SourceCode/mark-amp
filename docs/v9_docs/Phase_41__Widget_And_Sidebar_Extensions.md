# Phase 41: Widget and Sidebar Extensions

## Overview
WidgetManager exists in core. WebviewHostPanel, TreeViewHost, and the contribution point registries (FileSystem, Language, TreeData, Webview, Decoration, Output Channel) exist but are not fully exercised. Extensions cannot easily create custom sidebars, widgets, or panels. This phase completes the extension UI surface area.

## Prerequisites
- Phase 04 (Plugin system maturation)
- Phase 18 (Tool window system)
- Phase 14 (Canvas extensibility)

## Tasks

### Task 1: Wire WidgetManager for Dashboard Widgets
**Files:** `src/core/WidgetManager.cpp`, `src/core/WidgetManager.h`
**Description:** WidgetManager exists. Wire it to manage dashboard widgets: registration, layout, state persistence, and data refresh.
**Acceptance Criteria:**
- Widget registration: ID, title, size, render function
- Widget layout: grid-based positioning
- Widget state persisted per workspace
- Widget refresh on configurable interval
- `WidgetRegisteredEvent` emitted
- Built-in widgets: word count, recent files, tasks due

### Task 2: Wire Dashboard Panel
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/WidgetManager.cpp`
**Description:** Dashboard panel in workspace: shows registered widgets in a configurable grid layout. Widgets resizable and rearrangeable.
**Acceptance Criteria:**
- Dashboard accessible from activity bar
- Grid layout with configurable columns (2, 3, 4)
- Widgets draggable to rearrange
- Widgets resizable (1x1, 2x1, 1x2, 2x2)
- "Add Widget" button shows available widgets
- Remove widget via context menu

### Task 3: Wire WebviewHostPanel for Extension Panels
**Files:** `src/ui/WebviewHostPanel.cpp`, `src/core/WebviewService.cpp`
**Description:** WebviewHostPanel and WebviewService exist. Wire them so extensions can register webview panels that render HTML content with message passing.
**Acceptance Criteria:**
- Extensions register webview panels via manifest
- Panel renders HTML/CSS/JS content from extension
- Message passing: postMessage from extension to webview and back
- Content Security Policy enforced
- Panel state preserved across hide/show
- Panel registered in View menu

### Task 4: Wire TreeDataProviderRegistry for Custom Trees
**Files:** `src/core/TreeDataProviderRegistry.cpp`, `src/ui/TreeViewHost.cpp`
**Description:** TreeDataProviderRegistry exists. Wire it so extensions can register custom tree views in the sidebar (e.g., project structure, outline, dependencies).
**Acceptance Criteria:**
- Extension registers tree data provider via manifest
- Tree renders in sidebar with expand/collapse
- Tree items have icons, labels, and context menus
- Tree refresh on data change
- Tree search/filter
- Multiple tree views stackable in sidebar

### Task 5: Wire DecorationProviderRegistry for Editor Decorations
**Files:** `src/core/DecorationProviderRegistry.cpp`, `src/ui/EditorPanel.cpp`
**Description:** DecorationProviderRegistry exists. Wire it so extensions can add visual decorations to the editor: inline annotations, gutter icons, line highlights.
**Acceptance Criteria:**
- Extensions register decoration providers
- Inline annotations: text appended after line content
- Gutter decorations: custom icons in gutter
- Line highlight: background color for specific lines
- Decorations update on file change
- Multiple decoration providers compose

### Task 6: Wire Custom Sidebar Panels
**Files:** `src/ui/LayoutManager.cpp`, `src/core/PluginManager.cpp`
**Description:** Extensions can register custom sidebar panels that appear as sections in the activity bar sidebar.
**Acceptance Criteria:**
- Extension manifest: `views` contribution with `sidebar` location
- Custom panel appears in sidebar
- Panel content from webview or tree data provider
- Panel icon in activity bar
- Panel position configurable (above/below built-in panels)
- Panel visibility toggleable

### Task 7: Wire Custom Status Bar Items from Extensions
**Files:** `src/core/StatusBarItemService.cpp`, `src/core/PluginManager.cpp`
**Description:** StatusBarItemService exists. Wire it for extension-contributed status bar items with text, tooltip, command, and alignment.
**Acceptance Criteria:**
- Extension registers status bar item via API
- Item: text, tooltip, click command, alignment (left/right)
- Item priority determines order
- Item text updateable at runtime
- Item removable on extension deactivate
- Theme-aware styling

### Task 8: Wire Custom Context Menu Items
**Files:** `src/core/PluginManager.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Extensions can contribute context menu items: editor context menu, file explorer context menu, canvas context menu.
**Acceptance Criteria:**
- Extension manifest: `menus` contribution point
- Editor context menu: items with when-clause
- File explorer context menu: items for files/folders
- Canvas context menu: items for canvas objects
- Menu items separated from built-in items
- When-clause controls visibility

### Task 9: Wire Custom Editor Actions
**Files:** `src/core/TextEditorService.cpp`, `src/core/PluginManager.cpp`
**Description:** Extensions can register editor actions: code actions (quick fix), format actions, refactoring actions.
**Acceptance Criteria:**
- Extension registers code action provider
- Code actions shown via light bulb icon
- Actions apply text edits to document
- Actions can prompt for input
- Actions undoable
- Actions context-aware via when-clause

### Task 10: Wire Custom Language Support
**Files:** `src/core/LanguageProviderRegistry.cpp`, `src/core/PluginManager.cpp`
**Description:** LanguageProviderRegistry exists. Wire it so extensions can add syntax highlighting, completion, and diagnostics for custom languages.
**Acceptance Criteria:**
- Extension registers language with file extensions
- Syntax highlighting: TextMate grammar import
- Completion provider: suggest items on trigger
- Diagnostic provider: publish warnings/errors
- Hover provider: show info on hover
- Multiple providers compose for same language

### Task 11: Wire Custom File System Providers
**Files:** `src/core/FileSystemProviderRegistry.cpp`, `src/core/PluginManager.cpp`
**Description:** FileSystemProviderRegistry exists. Wire virtual file system: extensions provide file content from non-disk sources (remote, database, generated).
**Acceptance Criteria:**
- Extension registers file system with URI scheme
- Virtual files open in editor like real files
- Virtual directories shown in explorer
- Read, write, delete operations delegated to provider
- File watcher events from provider
- URI format: `provider://path/to/file`

### Task 12: Wire Custom Output Channels
**Files:** `src/core/OutputChannelProviderRegistry.cpp`, `src/core/PluginManager.cpp`
**Description:** OutputChannelProviderRegistry exists. Wire it for extension-created output channels in the Output panel.
**Acceptance Criteria:**
- Extension creates named output channel
- Channel appears in Output panel dropdown
- Extension can append text, clear, and show channel
- Channel persists for extension lifecycle
- Channel removed on extension deactivate
- Channel content scrollable and searchable

### Task 13: Wire Extension Settings Contribution
**Files:** `src/core/PluginManager.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Extensions can contribute settings: define settings in manifest with type, default, description. Settings appear in Settings panel under extension section.
**Acceptance Criteria:**
- Extension manifest: `configuration` contribution point
- Settings appear in Settings panel under extension name
- Setting types: string, number, boolean, enum, array
- Default values from manifest
- Setting changes: `ConfigChangedEvent` emitted
- Extension reads settings via PluginContext

### Task 14: Wire Extension Keybinding Contribution
**Files:** `src/core/PluginManager.cpp`, `src/core/ShortcutManager.cpp`
**Description:** Extensions can contribute default keybindings for their commands.
**Acceptance Criteria:**
- Extension manifest: `keybindings` contribution point
- Keybindings registered with extension commands
- User can override extension keybindings
- Platform-specific bindings (mac, win, linux)
- When-clause support for context
- Conflict detection with existing bindings

### Task 15: Wire Extension Theme Contribution
**Files:** `src/core/PluginManager.cpp`, `src/core/ThemeRegistry.cpp`
**Description:** Extensions can contribute themes: color themes and icon themes packaged in extensions.
**Acceptance Criteria:**
- Extension manifest: `themes` contribution point
- Color themes registered in ThemeRegistry
- Icon themes registered in icon theme system
- Theme preview from extension
- Themes available in theme picker
- Uninstalling extension removes themes

### Task 16: Wire Extension API Documentation
**Files:** `src/core/PluginContext.h`
**Description:** Document all extension API surfaces: contribution points, PluginContext methods, events, and UI surfaces.
**Acceptance Criteria:**
- All contribution points documented with schema
- PluginContext methods: signature, description, example
- Available events: name, fields, usage
- UI surfaces: webview, tree, decoration, status bar
- Permission model: required scopes
- Example extension manifest

### Task 17: Wire Extension Development Tools
**Files:** `src/core/PluginManager.cpp`, `src/ui/MainFrame.cpp`
**Description:** Development tools for extension authors: "Reload Extension", "Show Extension Log", "Inspect Extension", "Generate Extension Scaffold".
**Acceptance Criteria:**
- "Extension: Reload" reloads specific extension without restart
- "Extension: Show Log" opens extension's output channel
- "Extension: Inspect" shows extension state (settings, contributions)
- "Extension: Generate Scaffold" creates starter extension project
- Developer mode: verbose extension logging
- Extension host process diagnostics

### Task 18: Wire Widget Theme Integration
**Files:** `src/core/WidgetManager.cpp`
**Description:** Dashboard widgets and extension panels use theme tokens. Extensions receive theme data for styling.
**Acceptance Criteria:**
- Widget framework provides theme CSS variables
- Extensions access theme colors via API
- Theme change notifies extensions
- Dashboard widgets adapt to theme
- Extension webviews receive theme update message
- All 64 themes work with extensions

### Task 19: Wire Extension Marketplace Discovery
**Files:** `src/core/GalleryService.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** Extension browser shows marketplace with categories, search, install, and ratings.
**Acceptance Criteria:**
- Extension browser in sidebar
- Categories: Themes, Language, Tools, Canvas, Notebooks
- Search with fuzzy matching
- Install/uninstall with one click
- Rating and download count
- Extension detail page with readme

### Task 20: Add Extension UI Tests
**Files:** `tests/unit/test_extension_ui.cpp`
**Description:** Test extension UI surfaces: webview, tree view, decorations, status bar, context menu.
**Acceptance Criteria:**
- Webview panel: create, message passing, state persistence
- Tree view: register, render, refresh, search
- Decoration: register, apply, update, remove
- Status bar: register, update, click, remove
- Context menu: register, when-clause, execute
- Extension settings: read, write, notify

## Testing Requirements
- Extension UI registration and rendering
- Message passing between extension and webview
- Decoration lifecycle and composition
- Settings contribution and access

## Phase Completion Criteria
- Dashboard with configurable widgets
- Webview panels for extensions
- Tree views for custom data
- Editor decorations from extensions
- Custom status bar items
- Extension settings in Settings panel
- All tests pass
