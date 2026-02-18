# Phase 38: Vault Style and Workspace Customization

## Overview
VaultStyleService exists in core for per-workspace styling. BuiltInThemes and ThemeMarketplace exist but workspace-level customization (custom CSS, workspace-specific themes, publish templates) is not wired. This phase enables deep workspace personalization.

## Prerequisites
- Phase 03 (Theme engine completion)
- Phase 20 (File management and workspace)
- Phase 05 (Settings UI)

## Tasks

### Task 1: Wire VaultStyleService for Per-Workspace CSS
**Files:** `src/core/VaultStyleService.cpp`, `src/core/VaultStyleService.h`
**Description:** VaultStyleService exists. Wire it to apply workspace-specific CSS overrides: `.markamp/style.css` loaded on workspace open, applied to preview and UI panels.
**Acceptance Criteria:**
- `.markamp/style.css` loaded on workspace open
- CSS applied to preview panel rendering
- CSS changes: live reload on file save
- Invalid CSS: error notification with line number
- CSS scoped to workspace (does not affect other workspaces)
- `VaultStyleUpdatedEvent` emitted on change

### Task 2: Wire Workspace Theme Override
**Files:** `src/core/VaultStyleService.cpp`, `src/core/ThemeEngine.cpp`
**Description:** Workspace can specify a theme override in `.markamp/config.yaml`: theme name that overrides the global theme when this workspace is active.
**Acceptance Criteria:**
- `theme: "Dracula"` in workspace config overrides global theme
- Theme applied on workspace open
- Theme reverts to global on workspace close
- Invalid theme name: fallback to global
- "Workspace: Set Theme" command
- Theme override indicator in status bar

### Task 3: Wire Custom CSS Editor
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/VaultStyleService.cpp`
**Description:** In-app CSS editor for workspace styles: syntax-highlighted CSS editing with live preview.
**Acceptance Criteria:**
- "Workspace: Edit Custom CSS" opens CSS file in editor
- CSS syntax highlighting
- Live preview: changes reflected in real-time
- CSS snippets: built-in snippets for common customizations
- "Reset" button restores default (empty) CSS
- Documentation: available CSS selectors

### Task 4: Wire Theme Token Override
**Files:** `src/core/VaultStyleService.cpp`, `src/core/ThemeEngine.cpp`
**Description:** Workspace config can override individual theme tokens: `theme_overrides: { accent: "#FF6B6B", editor_bg: "#1A1A2E" }`.
**Acceptance Criteria:**
- `theme_overrides` in workspace config parsed
- Individual tokens overridden while keeping rest of theme
- Live preview on config change
- Invalid token names: warning
- Invalid color values: warning with fallback
- "Reset Token" restores theme default

### Task 5: Wire Workspace Profile System
**Files:** `src/core/Config.h`, `src/core/WorkspaceService.cpp`
**Description:** Workspace profiles: save complete workspace customization (theme, CSS, settings, layout) as a named profile. Switch between profiles.
**Acceptance Criteria:**
- "Save Profile" captures: theme, CSS, settings, layout
- "Load Profile" applies saved configuration
- Built-in profiles: Writing, Coding, Canvas, Minimal
- Custom profiles saved in `.markamp/profiles/`
- Profile selector in command palette
- Profile import/export

### Task 6: Wire Font Customization
**Files:** `src/core/ThemeEngine.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Per-workspace font customization: editor font, UI font, preview font. Font family, size, weight, and line height configurable.
**Acceptance Criteria:**
- Editor font: family, size (8-72pt), weight, line height
- UI font: family, size
- Preview font: family, size
- Font picker with preview
- System font enumeration
- Font changes apply immediately

### Task 7: Wire Custom Icon Theme
**Files:** `src/core/ThemeEngine.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** Custom icon themes for file explorer: different icons for file types. Support VS Code icon theme format.
**Acceptance Criteria:**
- Icon theme setting in preferences
- Built-in icon theme with file type icons
- VS Code icon theme import (best effort)
- Icons for: .md, .json, .yaml, .py, .js, folders
- Folder icon changes on expand/collapse
- Custom icon themes in `.markamp/icons/`

### Task 8: Wire Publish Template Customization
**Files:** `src/core/ExportService.cpp`, `src/core/VaultStyleService.cpp`
**Description:** Workspace-specific publish templates: HTML/CSS templates for export that reflect workspace branding.
**Acceptance Criteria:**
- Publish templates in `.markamp/publish-templates/`
- Template includes: HTML layout, CSS styling, header, footer
- Variables: `{{title}}`, `{{content}}`, `{{date}}`, `{{nav}}`
- "Publish: Use Workspace Template" default
- Preview template before publishing
- Built-in templates: blog, documentation, report

### Task 9: Wire CSS Snippet Library
**Files:** `src/core/VaultStyleService.cpp`
**Description:** Library of CSS snippets for common customizations: font pairing, color overrides, layout tweaks, print styles.
**Acceptance Criteria:**
- Snippet library accessible from CSS editor
- Categories: Typography, Colors, Layout, Print
- Each snippet: name, description, CSS code, preview
- Insert snippet into workspace CSS
- At least 20 snippets
- Community snippets importable

### Task 10: Wire Theme Marketplace Integration
**Files:** `src/core/ThemeMarketplace.cpp`, `src/ui/ThemeGallery.cpp`
**Description:** ThemeMarketplace exists. Wire it to browse, preview, and install community themes from a marketplace.
**Acceptance Criteria:**
- Theme gallery shows available themes
- Live preview: hover theme shows preview
- Install: one-click install
- Uninstall: remove installed theme
- Rating and download count
- Search and filter themes

### Task 11: Wire ThemePreviewCard
**Files:** `src/ui/ThemePreviewCard.cpp`, `src/ui/ThemeGallery.cpp`
**Description:** ThemePreviewCard exists. Wire it to show rich theme previews: editor colors, canvas colors, syntax highlighting sample.
**Acceptance Criteria:**
- Preview card shows: editor sample, sidebar sample, canvas sample
- Syntax highlighting preview for 3 languages
- Color palette overview (10 key colors)
- Light/dark mode indicator
- Author and version info
- "Apply" button

### Task 12: Wire ThemeTokenEditor for Advanced Users
**Files:** `src/ui/ThemeTokenEditor.cpp`, `src/ui/ThemeTokenEditor.h`
**Description:** ThemeTokenEditor exists. Wire it as an advanced theme customization tool: edit individual tokens with color picker, see live preview.
**Acceptance Criteria:**
- All theme tokens listed with current values
- Color picker for each token
- Live preview updates as tokens change
- Search tokens by name
- "Save as Custom Theme" exports modifications
- "Reset" reverts to original theme values

### Task 13: Wire Workspace-Specific Snippets
**Files:** `src/core/SnippetEngine.cpp`, `src/core/Config.h`
**Description:** Workspace-specific snippets in `.markamp/snippets/`: Markdown snippets scoped to the workspace.
**Acceptance Criteria:**
- Snippets in `.markamp/snippets/*.json`
- Snippet format: prefix, body (with tab stops), description
- Workspace snippets merge with global snippets
- Workspace snippets override globals on conflict
- Live reload on snippet file change
- "Snippets: Edit Workspace Snippets" command

### Task 14: Wire Workspace-Specific Keybindings
**Files:** `src/core/ShortcutManager.cpp`, `src/core/Config.h`
**Description:** Per-workspace keybindings in `.markamp/keybindings.json`: override global shortcuts for this workspace.
**Acceptance Criteria:**
- `.markamp/keybindings.json` loaded on workspace open
- Workspace bindings override global bindings
- Format matches global keybindings format
- "Shortcuts: Edit Workspace Shortcuts" command
- Workspace shortcuts revert on workspace close
- Conflict detection with global shortcuts

### Task 15: Wire Layout Persistence per Profile
**Files:** `src/ui/WorkspaceLayout.cpp`, `src/core/Config.h`
**Description:** Window layout (panel positions, sizes, sidebar width, split views) persisted per workspace profile.
**Acceptance Criteria:**
- Layout saved on profile save
- Layout restored on profile load
- Layout includes: sidebar width, panel positions, split view config
- Default layout per profile type (Writing: wide editor, Canvas: full canvas)
- Layout reset: restore profile default

### Task 16: Wire Custom Status Bar Items
**Files:** `src/core/StatusBarItemService.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** StatusBarItemService exists. Wire it for extensions and workspace config to add custom status bar items.
**Acceptance Criteria:**
- Extensions register status bar items via API
- Item: text, tooltip, click command, alignment (left/right)
- Workspace config can add static items
- Items sorted by priority
- Theme-aware styling
- Click triggers associated command

### Task 17: Wire Workspace Branding
**Files:** `src/core/VaultStyleService.cpp`, `src/ui/StartupPanel.cpp`
**Description:** Workspace branding: custom logo, welcome message, and accent color. Shown on startup and in about dialog.
**Acceptance Criteria:**
- `.markamp/branding.yaml` defines workspace branding
- Custom logo: shown in sidebar header
- Welcome message: shown on startup panel
- Accent color override: workspace-level accent
- Branding optional (defaults to MarkAmp branding)

### Task 18: Wire Workspace Customization Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register customization commands: "Workspace: Edit Custom CSS", "Workspace: Set Theme", "Workspace: Save Profile", "Workspace: Load Profile", "Workspace: Edit Tokens", "Theme: Browse Gallery".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Workspace:" and "Theme:" prefixes
- Commands context-aware
- Keyboard shortcuts for theme switching

### Task 19: Wire Customization Validation
**Files:** `src/core/VaultStyleService.cpp`
**Description:** Validate all workspace customization on load: CSS syntax, theme token names, color values, font availability.
**Acceptance Criteria:**
- CSS: syntax validation with error line numbers
- Theme tokens: validate names exist in schema
- Colors: validate hex, rgb, hsl formats
- Fonts: warn if font not available on system
- Validation errors in Problems panel
- Graceful fallback for all validation failures

### Task 20: Add Workspace Customization Tests
**Files:** `tests/unit/test_vault_style.cpp`, `tests/unit/test_theme_marketplace.cpp`
**Description:** Test workspace customization: CSS loading, theme override, profile management, token editing.
**Acceptance Criteria:**
- CSS: load, apply, live reload
- Theme override: workspace theme applied
- Token override: individual tokens changed
- Profile: save, load, switch
- Validation: invalid CSS, invalid tokens handled
- Font customization applied

## Testing Requirements
- CSS loading and application
- Theme and token override
- Profile save/load round-trip
- Validation of all customization inputs

## Phase Completion Criteria
- Per-workspace CSS customization
- Theme and token override per workspace
- Workspace profiles with save/load
- Font customization
- Theme marketplace integration
- Custom snippets and keybindings per workspace
- All tests pass
