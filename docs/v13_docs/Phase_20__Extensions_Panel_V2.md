# Phase 20: Extensions Panel V2

## Objective

Overhaul the existing `ExtensionsBrowserPanel` into a comprehensive extension marketplace and management experience rivaling VSCode's extensions panel. The current implementation (`ExtensionsBrowserPanel` at `src/ui/ExtensionsBrowserPanel.h`, 95 lines header) provides basic search, installed/search tabs, a scrollable card list, and a detail panel. The `ExtensionCard` widget shows name, publisher, version, description, and install/uninstall action. This phase adds marketplace categories, star ratings, download counts, rich detail views with changelogs, enable/disable per workspace, extension settings, update management, recommended extensions, and workspace-specific extension profiles.

## Prerequisites

- `ExtensionsBrowserPanel` at `src/ui/ExtensionsBrowserPanel.h` / `.cpp`
- `ExtensionCard` at `src/ui/ExtensionCard.h` / `.cpp` with `State` enum (NotInstalled, Installed, UpdateAvailable)
- `ExtensionDetailPanel` at `src/ui/ExtensionDetailPanel.h`
- `core::IExtensionManagementService` and `core::IExtensionGalleryService` interfaces
- `core::ExtensionManifest` with extension metadata
- `core::LocalExtension` for installed extension data
- `core::GalleryExtension` for marketplace extension data
- `SidebarMode::kExtensions` registered
- `ActivityBar` with extensions icon

## Deliverables

An enhanced extensions experience with 25+ tasks covering marketplace browsing, rich cards, detail views, management, and workspace profiles.

---

## Task 1: Marketplace Category Browser

**Title:** Add category-based browsing to the extensions panel

**Description:** Add a category filter bar below the search field that allows filtering extensions by category: Themes, Languages, Debuggers, Linters, Formatters, Snippets, Testing, Notebooks, Visualization, Keymaps, Other.

**Implementation Details:**
- Add a horizontal scrollable chip/pill bar below the search:
  ```
  [All] [Themes] [Languages] [Debuggers] [Linters] [Formatters] [Snippets] [Testing] ...
  ```
- Each chip is a clickable toggle:
  - Active: accent background, white text
  - Inactive: transparent, muted text
  - Multiple categories selectable (OR logic)
- When categories selected, filter both installed and marketplace results
- Categories from `core::GalleryExtension::categories` field
- "All" chip clears category filter

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.h` (add category state)
- `src/ui/ExtensionsBrowserPanel.cpp` (add category bar, filter logic)

**Acceptance Criteria:**
- Category chips scrollable horizontally
- Toggle filtering works
- Multiple categories combinable
- "All" clears filters
- Categories from extension metadata

**Dependencies:** None

---

## Task 2: Enhanced Extension Cards

**Title:** Upgrade extension card design with icon, rating, and downloads

**Description:** Enhance `ExtensionCard` to display: extension icon (loaded from URL or placeholder), star rating (1-5), download count, and a verified publisher badge.

**Implementation Details:**
- Redesign card layout (height from 72px to 88px):
  ```
  [48x48 Icon] [Name]              [Rating: 4.5 stars]
               [Publisher (verified)] [Downloads: 12.5K]
               [Short description text...]
               [Install] or [Installed / Uninstall]
  ```
- Icon: load from extension manifest `icon` URL
  - Cache downloaded icons in temp directory
  - Fallback: generate a colored letter avatar from extension name
  - Display at 48x48 in a rounded rectangle
- Star rating: draw 5 stars (filled/empty/half) using wxGraphicsContext
  - Gold/amber fill color
- Download count: format as "12.5K", "1.2M", etc.
- Verified badge: small checkmark after publisher name for verified publishers

**Files Affected:**
- `src/ui/ExtensionCard.h` (update layout, add icon, rating, downloads)
- `src/ui/ExtensionCard.cpp` (implement new rendering)
- `src/core/GalleryService.h` (add rating, downloads to `GalleryExtension`)

**Acceptance Criteria:**
- Icon displayed (loaded or fallback avatar)
- Star rating with half-star granularity
- Download count formatted
- Verified publisher badge
- Card height accommodates new data

**Dependencies:** None

---

## Task 3: Sort and Filter Options

**Title:** Add sorting options for extension lists

**Description:** Add a sort dropdown for extension results: "Relevance", "Most Downloads", "Highest Rated", "Recently Updated", "Name (A-Z)".

**Implementation Details:**
- Sort dropdown at top-right of results area:
  ```cpp
  enum class ExtensionSortOrder {
      Relevance,    // search relevance (default for search results)
      Downloads,    // most downloads first
      Rating,       // highest rated first
      Updated,      // most recently updated first
      NameAsc       // alphabetical
  };
  ```
- Apply sorting to both search results and installed list
- For installed list: add filter for "Enabled", "Disabled", "Outdated"
- Persist sort preference in config

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add sort dropdown and logic)

**Acceptance Criteria:**
- Sort dropdown with 5 options
- Sorting applies immediately
- Filter for installed extensions
- Sort preference persists

**Dependencies:** None

---

## Task 4: Extension Detail View V2

**Title:** Overhaul the extension detail view with tabbed layout

**Description:** Expand `ExtensionDetailPanel` into a rich, tabbed detail view with tabs for Overview, Changelog, Dependencies, and Settings.

**Implementation Details:**
- Detail view fills the sidebar when an extension is selected
- Header: icon (large, 64x64), name, publisher, version, rating, downloads
- Action buttons: "Install"/"Uninstall", "Enable"/"Disable", "Settings"
- Tabs:
  ```
  [Overview] [Changelog] [Dependencies] [Ratings]
  ```
- **Overview tab**: full description (rendered markdown), feature list, screenshots
- **Changelog tab**: version history with dates and changes
- **Dependencies tab**: list of required extensions/runtime dependencies
- **Ratings tab**: star distribution chart, recent reviews
- Back button to return to list view

**Files Affected:**
- `src/ui/ExtensionDetailPanel.h` (major rewrite)
- `src/ui/ExtensionDetailPanel.cpp` (implement tabbed detail view)

**Acceptance Criteria:**
- All 4 tabs functional
- Description renders markdown
- Changelog shows version history
- Dependencies listed
- Back navigation works
- Large icon in header

**Dependencies:** Task 2

---

## Task 5: Enable/Disable Extensions

**Title:** Implement per-extension enable/disable

**Description:** Allow disabling installed extensions without uninstalling them. Disabled extensions are not loaded at startup but their files remain on disk.

**Implementation Details:**
- Add `bool enabled` flag to `LocalExtension`
- "Disable" button in extension card and detail view
- Disabled extensions:
  - Shown in installed list with "Disabled" badge
  - Grayed out card appearance
  - Not loaded in extension host on next restart
- "Enable" restores the extension
- Publish `ExtensionEnabledEvent` / `ExtensionDisabledEvent`
- Restart prompt: "Restart required to apply changes" with restart button

**Files Affected:**
- `src/core/ExtensionManagement.h` (add enable/disable state)
- `src/core/ExtensionManagement.cpp` (implement enable/disable)
- `src/ui/ExtensionCard.cpp` (add disable visual state)
- `src/ui/ExtensionsBrowserPanel.cpp` (wire enable/disable actions)
- `src/core/Events.h` (add extension state events)

**Acceptance Criteria:**
- Extensions can be disabled without uninstalling
- Disabled extensions not loaded on restart
- Visual distinction for disabled extensions
- Enable restores functionality
- Restart prompt shown when needed

**Dependencies:** None

---

## Task 6: Uninstall Confirmation

**Title:** Add confirmation and cleanup for extension uninstall

**Description:** When uninstalling, show a confirmation dialog listing what will be removed, then clean up extension files and settings.

**Implementation Details:**
- Confirmation dialog:
  ```
  Uninstall "Extension Name"?
  This will remove:
  - Extension files (2.3 MB)
  - Extension settings
  - Extension data

  [Cancel] [Uninstall]
  ```
- On confirm:
  1. Disable the extension
  2. Remove extension files from disk
  3. Clean up extension settings from config
  4. Publish `ExtensionUninstalledEvent`
  5. Refresh extension list
- "Uninstall and Restart" for extensions that are currently loaded

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add uninstall confirmation)
- `src/core/ExtensionManagement.cpp` (implement cleanup)

**Acceptance Criteria:**
- Confirmation dialog with details
- Files removed from disk
- Settings cleaned up
- Extension list refreshes
- Event published

**Dependencies:** Task 5

---

## Task 7: Update Available Indicator

**Title:** Show update indicators and bulk update support

**Description:** When an installed extension has an update available, show a visual indicator on the card and provide "Update" and "Update All" actions.

**Implementation Details:**
- Check for updates: compare installed version with gallery version
- Update indicator on card:
  - Blue "Update" badge
  - Update button replaces install button
  - Current version + arrow + new version: "v1.2.0 -> v1.3.0"
- "Update All" button in panel header when any updates available
- Activity bar badge: show count of available updates
- Auto-check for updates every hour (configurable)
- Notification: "N extension updates available" on check

**Files Affected:**
- `src/ui/ExtensionCard.cpp` (add update state rendering)
- `src/ui/ExtensionsBrowserPanel.cpp` (add update all button)
- `src/core/ExtensionManagement.cpp` (implement update check)
- `src/ui/ActivityBar.cpp` (set update badge)

**Acceptance Criteria:**
- Update available shown on cards
- Update button functional
- Update All bulk action
- Activity bar badge with count
- Auto-check configurable

**Dependencies:** Task 2

---

## Task 8: Extension Settings Quick Link

**Title:** Quick access to extension settings

**Description:** Each installed extension card shows a gear icon that opens the extension's settings section in the settings panel.

**Implementation Details:**
- Gear icon on each installed extension card (shown on hover)
- Click opens the Settings panel filtered to that extension's settings:
  - Navigate to Settings sidebar mode
  - Filter by extension ID
  - Show only that extension's contributed settings
- If extension has no settings: gear icon not shown
- Also accessible from extension detail view "Settings" tab

**Files Affected:**
- `src/ui/ExtensionCard.cpp` (add settings gear icon)
- `src/ui/ExtensionsBrowserPanel.cpp` (wire settings navigation)
- `src/ui/SettingsPanel.cpp` (accept extension filter)

**Acceptance Criteria:**
- Gear icon visible on extensions with settings
- Click navigates to settings panel
- Settings filtered to that extension
- Hidden for extensions without settings

**Dependencies:** None

---

## Task 9: Recommended Extensions

**Title:** Show recommended extensions based on workspace content

**Description:** Analyze the workspace files to determine relevant extension recommendations. For example, if `.py` files exist, recommend Python-related extensions.

**Implementation Details:**
- Recommendation engine:
  ```cpp
  auto GetRecommendations(const std::string& workspace_root) -> std::vector<std::string> {
      std::set<std::string> extensions_seen;
      ScanWorkspaceForFileTypes(workspace_root, extensions_seen);

      std::vector<std::string> recommendations;
      if (extensions_seen.count(".py")) recommendations.push_back("python-linter");
      if (extensions_seen.count(".ts")) recommendations.push_back("typescript-tools");
      if (extensions_seen.count(".rs")) recommendations.push_back("rust-analyzer");
      // etc.
      return recommendations;
  }
  ```
- "Recommended" section at top of extension browser (when no search active)
- Each recommendation card shows: "Recommended based on files in your workspace"
- Dismiss individual recommendations with "x" button
- Also support `.markamp/extensions.json` file for workspace-recommended extensions

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add recommendations section)
- `src/core/ExtensionRecommendations.h` (new)
- `src/core/ExtensionRecommendations.cpp` (new)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Recommendations based on workspace file types
- Shown when no search active
- Dismissible individually
- Workspace extension.json supported
- No duplicate recommendations for already-installed extensions

**Dependencies:** None

---

## Task 10: Workspace-Specific Extensions

**Title:** Support enabling extensions per workspace

**Description:** Allow configuring which extensions are active for a specific workspace, rather than globally. Extensions can be "Enable (Workspace)" or "Disable (Workspace)".

**Implementation Details:**
- Store workspace extension overrides in `.markamp/settings.json`:
  ```json
  {
      "extensions": {
          "workspace_enabled": ["python-linter", "markdown-toc"],
          "workspace_disabled": ["java-support"]
      }
  }
  ```
- Extension card actions: "Enable (Workspace)" / "Disable (Workspace)" in dropdown
- Extension state resolution:
  1. Workspace override (highest priority)
  2. User global setting
  3. Extension default (enabled)
- Visual indicator in card: small "W" badge for workspace-specific overrides

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add workspace toggle)
- `src/core/ExtensionManagement.cpp` (implement workspace-specific state)

**Acceptance Criteria:**
- Extensions can be enabled/disabled per workspace
- Workspace overrides take priority
- "W" badge indicates workspace-specific setting
- Settings stored in workspace config

**Dependencies:** Task 5

---

## Task 11: Extension Search Enhancement

**Title:** Improve search with filters and autocomplete

**Description:** Enhance the search input with prefix filters and autocomplete suggestions.

**Implementation Details:**
- Search prefix filters (inspired by VSCode):
  - `@installed` - show only installed extensions
  - `@enabled` - show enabled extensions
  - `@disabled` - show disabled extensions
  - `@outdated` - show extensions with updates
  - `@category:themes` - filter by category
  - `@tag:python` - filter by tag
  - `@sort:installs` - sort by install count
- Autocomplete dropdown when typing `@`:
  - Show available prefix filters
  - Show recently searched terms
- Search history (from Phase 17 pattern)

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add search prefix parsing)

**Acceptance Criteria:**
- All prefix filters functional
- Autocomplete on "@" prefix
- Filters combinable
- Search history available

**Dependencies:** None

---

## Task 12: Extension Icon Loading

**Title:** Implement async icon loading with caching

**Description:** Load extension icons from their manifest URLs asynchronously, cache them on disk, and display placeholder icons while loading.

**Implementation Details:**
- Icon loading pipeline:
  1. Check disk cache (`.markamp/extension-icons/{id}.png`)
  2. If not cached, download from manifest icon URL
  3. Display placeholder (colored letter avatar) while loading
  4. On download complete: cache to disk, update card
- Placeholder: first letter of extension name on colored background
  - Color derived from hash of extension name (consistent)
- Cache expiry: 7 days
- Handle: missing icons, download failures, invalid image data

**Files Affected:**
- `src/ui/ExtensionCard.cpp` (implement icon display with async loading)
- `src/core/IconCache.h` (new)
- `src/core/IconCache.cpp` (new)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Icons load asynchronously
- Placeholder shown while loading
- Downloaded icons cached to disk
- Cache expires after 7 days
- Download failures handled gracefully

**Dependencies:** Task 2

---

## Task 13: Extension Changelog Viewer

**Title:** Display extension version history

**Description:** In the detail view "Changelog" tab, render the extension's changelog with version headers, dates, and change descriptions.

**Implementation Details:**
- Parse changelog from extension manifest or CHANGELOG.md:
  ```
  ## v1.3.0 (2026-02-15)
  - Added support for custom themes
  - Fixed issue with bracket matching
  - Performance improvement for large files

  ## v1.2.0 (2026-01-20)
  - Initial release
  ```
- Render as scrollable formatted text:
  - Version headers in semibold
  - Dates in muted color
  - Bullet points for changes
  - Collapsible older versions

**Files Affected:**
- `src/ui/ExtensionDetailPanel.cpp` (implement changelog tab)

**Acceptance Criteria:**
- Changelog parsed and rendered
- Version headers prominently displayed
- Dates formatted
- Older versions collapsible
- Scrollable for long changelogs

**Dependencies:** Task 4

---

## Task 14: Extension Dependencies

**Title:** Show and manage extension dependencies

**Description:** Display extension dependencies in the detail view and automatically install required dependencies.

**Implementation Details:**
- Dependencies tab in detail view:
  ```
  REQUIRED DEPENDENCIES
  - markdown-language-server (v2.0+)  [Installed]
  - code-theme-base (v1.0+)          [Not Installed - Install]

  OPTIONAL DEPENDENCIES
  - spelling-checker (v1.5+)         [Not Installed]
  ```
- When installing an extension with unmet required dependencies:
  1. Show dialog: "This extension requires the following: [list]. Install all?"
  2. Install dependencies first, then the extension
- Show dependent extensions: "Used by: Extension A, Extension B"

**Files Affected:**
- `src/ui/ExtensionDetailPanel.cpp` (implement dependencies tab)
- `src/core/ExtensionManagement.cpp` (dependency resolution)

**Acceptance Criteria:**
- Dependencies listed with install status
- Auto-install prompt for missing required deps
- Optional deps shown separately
- Reverse dependencies shown
- Dependency versions checked

**Dependencies:** Task 4

---

## Task 15: Extension Runtime Info

**Title:** Show extension runtime information

**Description:** For installed extensions, show runtime information: activation time, memory usage, API calls, and error count.

**Implementation Details:**
- Runtime section in detail view (below action buttons):
  ```
  Runtime Information
  Status: Active
  Activation Time: 12ms
  Memory: 2.4 MB
  API Calls: 42
  Errors: 0
  Last Updated: 2 hours ago
  ```
- Collect data from extension host runtime:
  - Activation time from `PluginContext` lifecycle
  - Memory estimation from extension allocations
  - API call count from extension API proxy
  - Error count from exception tracking

**Files Affected:**
- `src/ui/ExtensionDetailPanel.cpp` (add runtime info display)
- `src/core/ExtensionManagement.h` (add runtime stats)

**Acceptance Criteria:**
- Runtime stats displayed for active extensions
- Activation time accurate
- Error count tracked
- Refresh button to update stats
- Data only available for running extensions

**Dependencies:** Task 4

---

## Task 16: Trending and Popular Extensions

**Title:** Show trending and popular extensions sections

**Description:** When no search is active, show curated sections: "Trending" (recently popular), "Popular" (all-time most installed), and "Recently Added".

**Implementation Details:**
- Sections shown in the main browse view:
  ```
  TRENDING
  [Card] [Card] [Card]  (horizontal scroll)

  POPULAR
  [Card] [Card] [Card]  (horizontal scroll)

  RECENTLY ADDED
  [Card] [Card] [Card]  (horizontal scroll)
  ```
- Each section: horizontally scrollable row of compact cards
- Data from gallery service API
- Cache results for 30 minutes
- "See All" link for each section opens full filtered view

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add curated sections)
- `src/core/GalleryService.h` (add trending/popular queries)

**Acceptance Criteria:**
- Three curated sections displayed
- Horizontal scrolling card rows
- Cached for 30 minutes
- "See All" opens full list
- Sections hidden when searching

**Dependencies:** None

---

## Task 17: Extension Install Progress

**Title:** Show installation progress with detailed status

**Description:** Display download and installation progress for each extension being installed.

**Implementation Details:**
- During install, card shows:
  - Progress bar (download: 0-100%)
  - Status text: "Downloading...", "Installing...", "Installed!"
  - Cancel button during download
- Multiple concurrent installs supported
- On error: show error message on card with "Retry" button
- Success: animate card transition from "Installing" to "Installed"

**Files Affected:**
- `src/ui/ExtensionCard.cpp` (add progress state)
- `src/core/ExtensionManagement.cpp` (publish install progress events)

**Acceptance Criteria:**
- Download progress shown
- Status text updates during phases
- Cancel during download
- Error with retry
- Success animation

**Dependencies:** None

---

## Task 18: Extension Views Panel

**Title:** Tab bar for Installed/Marketplace/Recommended views

**Description:** Replace the current simple "Installed"/"Search" tabs with a proper tab bar: "Installed", "Marketplace", "Recommended", "Enabled", "Disabled".

**Implementation Details:**
- Tab bar below search field:
  ```
  [Installed (12)] [Marketplace] [Recommended (3)] [Enabled (10)] [Disabled (2)]
  ```
- Each tab shows count badge
- Marketplace: shows curated sections (Task 16) and search results
- Installed: all installed extensions with enable/disable state
- Recommended: workspace recommendations (Task 9)
- Enabled/Disabled: filtered views of installed

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (replace dual-tab with full tab bar)

**Acceptance Criteria:**
- All 5 tabs functional
- Count badges on tabs
- Correct content per tab
- Tab state persists

**Dependencies:** Tasks 5, 9, 16

---

## Task 19: Extension Profile Management

**Title:** Save and load extension profiles

**Description:** Allow saving the current set of enabled extensions as a named profile that can be restored later. Useful for switching between project contexts (e.g., "Web Dev", "C++ Dev", "Writing").

**Implementation Details:**
- Profile management in settings:
  ```yaml
  extension_profiles:
    - name: "C++ Development"
      extensions:
        enabled: ["clang-format", "cmake-tools", "cpp-debugger"]
        disabled: ["python-linter", "web-tools"]
    - name: "Web Development"
      extensions:
        enabled: ["typescript-tools", "prettier", "eslint"]
        disabled: ["cpp-debugger", "cmake-tools"]
  ```
- "Save Current Profile" button
- "Load Profile" dropdown with saved profiles
- Switching profiles: batch enable/disable extensions
- Import/export profiles as JSON files

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add profile management UI)
- `src/core/Config.h` (add profile storage)

**Acceptance Criteria:**
- Profiles can be saved and loaded
- Batch enable/disable on profile switch
- Import/export as JSON
- Profiles listed in dropdown
- Current profile indicated

**Dependencies:** Task 5

---

## Task 20: Extension Conflict Detection

**Title:** Detect and warn about conflicting extensions

**Description:** Detect when two installed extensions might conflict (e.g., two formatters for the same language, two theme extensions active simultaneously) and warn the user.

**Implementation Details:**
- Conflict detection rules:
  - Two extensions contributing the same language server
  - Two formatter extensions for the same file type
  - Multiple active color themes
  - Extensions declaring mutual exclusivity in manifest
- Warning display:
  - Yellow warning badge on conflicting extension cards
  - Tooltip: "May conflict with: {other extension}"
  - Detail view shows conflict details
- Suggestions: "Disable one of the conflicting extensions"

**Files Affected:**
- `src/core/ExtensionManagement.cpp` (add conflict detection)
- `src/ui/ExtensionCard.cpp` (add conflict warning display)

**Acceptance Criteria:**
- Conflicts detected automatically
- Warning badge on conflicting cards
- Tooltip explains conflict
- Suggestions provided
- Updated when extensions change

**Dependencies:** Task 5

---

## Task 21: Extension Commands Registration

**Title:** Show commands contributed by each extension

**Description:** In the extension detail view, list all commands that the extension contributes to the command palette.

**Implementation Details:**
- Commands section in detail view:
  ```
  CONTRIBUTED COMMANDS
  - Format Document          Cmd+Shift+I
  - Format Selection         Cmd+K Cmd+F
  - Toggle Format on Save
  ```
- Commands listed with their keyboard shortcuts
- Click command to execute it
- Also show in command palette with extension name prefix

**Files Affected:**
- `src/ui/ExtensionDetailPanel.cpp` (add commands section)

**Acceptance Criteria:**
- All contributed commands listed
- Keyboard shortcuts shown
- Clickable to execute
- Organized by category

**Dependencies:** Task 4

---

## Task 22: Extension Marketplace Cache

**Title:** Cache marketplace data for offline browsing

**Description:** Cache marketplace search results and extension metadata for offline access and faster repeated browsing.

**Implementation Details:**
- Cache levels:
  1. Memory cache: recent search results (5 minute TTL)
  2. Disk cache: extension metadata + icons (24 hour TTL)
  3. Offline mode: if network unavailable, show cached data with "Offline" badge
- Cache storage: `.markamp/marketplace-cache/`
- Cache invalidation: manual refresh button, TTL expiry, on extension install/uninstall
- Size limit: 50MB for disk cache, auto-evict oldest entries

**Files Affected:**
- `src/core/GalleryService.cpp` (add caching layer)
- `src/core/MarketplaceCache.h` (new)
- `src/core/MarketplaceCache.cpp` (new)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Search results cached in memory
- Metadata cached on disk
- Offline mode shows cached data
- Cache size limited
- Manual refresh available

**Dependencies:** None

---

## Task 23: Extension Panel Keyboard Navigation

**Title:** Full keyboard support for extensions panel

**Description:** Enable keyboard navigation through extension cards, categories, and actions.

**Implementation Details:**
- Tab order: search input -> category chips -> sort dropdown -> extension cards -> detail view
- In card list:
  - Up/Down arrows navigate between cards
  - Enter opens detail view for focused card
  - Space triggers install/uninstall action
  - Right arrow opens card context menu
- In detail view:
  - Tab cycles through tabs
  - Escape returns to list view
- Cmd+Shift+X focuses extensions panel from anywhere

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add keyboard navigation)
- `src/ui/ExtensionCard.cpp` (add focus state)
- `src/ui/MainFrame.cpp` (register Cmd+Shift+X)

**Acceptance Criteria:**
- Full keyboard navigation
- Focus ring visible on cards
- Arrow keys navigate card list
- Enter opens detail, Space triggers action
- Escape returns from detail

**Dependencies:** None

---

## Task 24: Extensions Panel Theme

**Title:** Full theme integration for extensions panel

**Description:** Ensure all extensions panel elements use theme colors correctly.

**Implementation Details:**
- Color mappings:
  - Panel background: `BgPanel`
  - Card background: `BgApp`
  - Card hover: `BgApp.ChangeLightness(105)`
  - Card name: `TextMain`
  - Card publisher: `TextMuted`
  - Card description: `TextMuted`
  - Install button: `AccentPrimary` background
  - Uninstall button: `ErrorColor` tint
  - Star rating: `WarningColor` (gold)
  - Category chips active: `AccentPrimary`
  - Update badge: blue
  - Disabled overlay: 50% opacity

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (theme all elements)
- `src/ui/ExtensionCard.cpp` (theme card rendering)
- `src/ui/ExtensionDetailPanel.cpp` (theme detail view)

**Acceptance Criteria:**
- All elements themed
- Dark and light themes work
- Card hover states visible
- Button colors appropriate
- Updates on theme change

**Dependencies:** Tasks 1-18

---

## Task 25: Extensions Panel Accessibility

**Title:** Accessibility support for extensions panel

**Description:** Ensure the extensions panel is fully accessible with screen reader support and keyboard navigation.

**Implementation Details:**
- Search input: role "searchbox", aria-label "Search Extensions"
- Category chips: role "tab", aria-selected state
- Card list: role "listbox" with "option" items
- Each card: accessible name "{name} by {publisher}, {rating} stars, {downloads} downloads, {state}"
- Install/uninstall buttons: aria-label with extension name
- Detail view: proper heading hierarchy
- Screen reader announcements: "Installing {name}...", "Installed {name}", "N updates available"

**Files Affected:**
- `src/ui/ExtensionsBrowserPanel.cpp` (add accessibility)
- `src/ui/ExtensionCard.cpp` (add accessible names)
- `src/ui/ExtensionDetailPanel.cpp` (add accessibility)

**Acceptance Criteria:**
- Full keyboard navigation
- Screen reader announces card details
- State changes announced
- Proper ARIA-equivalent roles
- Tab order logical

**Dependencies:** Tasks 1-18

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Category Browser (Task 1) | Medium |
| Enhanced Cards (Task 2) | Medium |
| Sort/Filter (Task 3) | Low |
| Detail View V2 (Task 4) | High |
| Enable/Disable (Task 5) | Medium |
| Uninstall Confirm (Task 6) | Low |
| Update Indicator (Task 7) | Medium |
| Settings Link (Task 8) | Low |
| Recommendations (Task 9) | High |
| Workspace Extensions (Task 10) | Medium |
| Search Enhancement (Task 11) | Medium |
| Icon Loading (Task 12) | High |
| Changelog (Task 13) | Medium |
| Dependencies (Task 14) | High |
| Runtime Info (Task 15) | Medium |
| Trending/Popular (Task 16) | Medium |
| Install Progress (Task 17) | Medium |
| View Tabs (Task 18) | Medium |
| Profiles (Task 19) | Medium |
| Conflict Detection (Task 20) | Medium |
| Commands List (Task 21) | Low |
| Marketplace Cache (Task 22) | High |
| Keyboard Nav (Task 23) | Medium |
| Theme (Task 24) | Low |
| Accessibility (Task 25) | Medium |

## Files Created

- `src/core/ExtensionRecommendations.h`
- `src/core/ExtensionRecommendations.cpp`
- `src/core/IconCache.h`
- `src/core/IconCache.cpp`
- `src/core/MarketplaceCache.h`
- `src/core/MarketplaceCache.cpp`

## Files Modified

- `src/ui/ExtensionsBrowserPanel.h`
- `src/ui/ExtensionsBrowserPanel.cpp`
- `src/ui/ExtensionCard.h`
- `src/ui/ExtensionCard.cpp`
- `src/ui/ExtensionDetailPanel.h`
- `src/ui/ExtensionDetailPanel.cpp`
- `src/core/ExtensionManagement.h`
- `src/core/ExtensionManagement.cpp`
- `src/core/GalleryService.h`
- `src/core/GalleryService.cpp`
- `src/core/Events.h`
- `src/core/Config.h`
- `src/ui/ActivityBar.cpp`
- `src/ui/LayoutManager.cpp`
- `src/ui/SettingsPanel.cpp`
- `src/ui/MainFrame.cpp`
- `CMakeLists.txt`
