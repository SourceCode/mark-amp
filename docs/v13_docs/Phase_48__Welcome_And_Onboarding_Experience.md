# Phase 48 -- Welcome and Onboarding Experience

## Objective

Create a polished welcome and onboarding experience that matches VSCode's Getting Started quality. Extend the existing StartupPanel, WalkthroughPanel, and FirstRunWizard into a cohesive onboarding flow with a welcome tab, interactive tutorials, recent files/workspaces with rich previews, what's new changelog, tip of the day, sample project opener, and telemetry opt-in prompt. The goal is a professional first impression that helps new users discover features and returning users resume work quickly.

## Prerequisites

- Phase 47 complete (Peek View System)
- `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.h` -- existing startup panel with recent workspaces
- `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.h` -- existing walkthrough with steps, spotlight, built-in walkthroughs
- `/Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.h` -- existing 4-page wizard (Welcome, Theme, Profile, Extensions)
- `/Users/ryanrentfro/code/markamp/src/core/RecentWorkspaces.h` -- recent workspace tracking
- `/Users/ryanrentfro/code/markamp/src/core/RecentFiles.h` -- recent file tracking
- Phase 41 components (ThemedButton, Badge, ThemedDropdown, etc.)
- Phase 44 WizardDialog for FirstRunWizard migration

## VSCode Reference Behavior

- Welcome tab: full-screen tab with logo, quick actions, recent files, walkthroughs
- "Show on startup" checkbox in bottom-left
- Getting Started walkthroughs with step-by-step guides and checkmarks
- Recent files list with file icons, paths, and timestamps
- Quick links: New File, Open File, Clone Repository, Documentation
- Keyboard shortcut cheat sheet link
- "What's New" section showing changelog highlights
- Interactive tutorial that highlights UI elements with overlays
- Theme picker on first run

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/InteractiveTutorial.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/InteractiveTutorial.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/TipOfTheDay.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/TipOfTheDay.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_welcome_onboarding.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Create WelcomeTab as Full-Screen Editor Tab

**Description:** Create a WelcomeTab that opens as a regular editor tab (not a dialog or popup), matching VSCode's Welcome tab behavior.

**Implementation Details:** WelcomeTab extends ThemeAwareWindow and registers as a special document type in the TabBar (pinned, non-closable if "show on startup" is enabled). The tab title is "Welcome" with a home icon. The tab content is a scrollable panel divided into two columns: left column (60%) contains the header, quick actions, and walkthroughs; right column (40%) contains recent files/workspaces. The WelcomeTab opens on startup (unless disabled) and can be reopened via Command Palette "Help: Welcome". The tab persists across sessions if "show on startup" is checked.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (create)

**Acceptance Criteria:**
- WelcomeTab opens as a regular editor tab
- Two-column layout renders correctly
- Tab shows "Welcome" title with home icon
- Tab opens on startup when "show on startup" is enabled
- Tab is reopenable via Command Palette

**Dependencies:** None

---

### Task 02 -- Implement Welcome Header with Logo and Version

**Description:** Render the application header in the Welcome tab with logo, name, version, and a brief tagline.

**Implementation Details:** Header section at the top of the left column: application icon (48x48), "MarkAmp Studio" title (24px bold), version string (14px muted), tagline "A Modern Markdown IDE" (14px, accent color). Below the header, a horizontal divider (1px border_default). The version string reads from the build configuration. The header respects theme colors (adapts to dark/light).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Logo, title, and version render correctly
- Version matches build configuration
- Tagline uses accent color
- Divider separates header from content
- Theme switch updates colors

**Dependencies:** Task 01

---

### Task 03 -- Implement Quick Actions Section

**Description:** Create a Quick Actions section with buttons for common startup operations: New File, Open File, Open Folder, Clone Repository, Open Recent.

**Implementation Details:** Render 5 action buttons vertically in the left column below the header. Each button is a full-width row (40px height) with: icon (20x20), label (14px), and keyboard shortcut hint (12px, muted, right-aligned). Buttons: "New File" (Cmd+N), "Open File" (Cmd+O), "Open Folder" (Cmd+Shift+O), "Clone Git Repository" (no shortcut), "Open Recent" (Cmd+R). Each button fires the corresponding command via EventBus. Buttons use Ghost variant from ThemedButton.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- All 5 actions render with icons and labels
- Keyboard shortcut hints show correctly
- Clicking executes the corresponding command
- Buttons have hover/press state feedback
- Actions are functional (New File creates file, etc.)

**Dependencies:** Task 01, Phase 41 Task 02 (ThemedButton)

---

### Task 04 -- Implement Quick Links Section

**Description:** Create a Quick Links section with links to Documentation, Release Notes, Extensions Marketplace, Settings, and Keyboard Shortcuts.

**Implementation Details:** Render 5 link buttons below the Quick Actions section. Use LinkButton from Phase 41. Links: "Documentation" (opens docs in browser or internal viewer), "Release Notes" (opens changelog panel), "Browse Extensions" (opens extensions browser panel), "Settings" (opens settings panel), "Keyboard Shortcuts" (opens shortcut overlay). Each link has a subtle icon and underlines on hover.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- All 5 links render with icons
- Clicking navigates to the correct panel/page
- Links underline on hover
- Links use accent color
- All link targets are reachable

**Dependencies:** Task 01, Phase 41 Task 04 (LinkButton)

---

### Task 05 -- Create RecentItemsList Component

**Description:** Create a reusable component that renders a list of recent files and workspaces with icons, paths, timestamps, and click-to-open behavior.

**Implementation Details:** RecentItemsList extends ThemeAwareWindow. It renders recent items as rows (32px height each). Each row: file/folder icon (16x16, based on file type), filename (13px bold), relative path (12px, muted), timestamp (12px, muted, right-aligned, relative format: "2h ago", "Yesterday"). Click opens the file/workspace. Right-click shows context menu: "Open", "Open in New Window", "Remove from Recent". The list shows the most recent 20 items. Empty state: "No Recent Items" centered text. Items come from RecentFiles and RecentWorkspaces services.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/RecentItemsList.cpp` (create)

**Acceptance Criteria:**
- Recent items render with correct icons and metadata
- Click opens the file/workspace
- Right-click shows context menu
- Timestamps show relative format
- Empty state message displays when no items

**Dependencies:** Task 01

---

### Task 06 -- Integrate RecentItemsList in WelcomeTab

**Description:** Place the RecentItemsList in the right column of the WelcomeTab, showing both recent files and recent workspaces.

**Implementation Details:** Right column contains a tabbed section with two tabs: "Recent Files" and "Recent Workspaces". Each tab shows a RecentItemsList populated from the respective service. The "Recent Workspaces" tab is selected by default (since most users work with projects). A "More..." link at the bottom of each list opens the full recent items dialog. A search filter input at the top of the list allows filtering by name.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Recent files and workspaces show in tabbed sections
- Default tab is "Recent Workspaces"
- Search filter narrows the list
- "More..." link opens full recent dialog
- List updates when new files/workspaces are opened

**Dependencies:** Task 05

---

### Task 07 -- Implement "Show on Startup" Checkbox

**Description:** Add a "Show Welcome tab on startup" checkbox to the bottom of the Welcome tab.

**Implementation Details:** ThemedCheckbox (Phase 41) at the bottom-left of the WelcomeTab. Checked by default on first install. State persists via `app.showWelcomeOnStartup` config key. When unchecked, the Welcome tab does not open on subsequent launches. When checked, it opens as the first tab. The checkbox is also available in Settings under "Application > Startup". The Welcome tab can always be opened manually via "Help: Welcome" command.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/Config.h` (modify if needed)

**Acceptance Criteria:**
- Checkbox state persists across restarts
- Unchecked prevents Welcome tab on startup
- Setting also appears in Settings panel
- Manual open always works regardless of checkbox
- Default is checked on first install

**Dependencies:** Task 01, Phase 41 Task 07 (ThemedCheckbox)

---

### Task 08 -- Integrate WalkthroughPanel in WelcomeTab

**Description:** Embed the existing WalkthroughPanel in the Welcome tab below the Quick Actions, showing Getting Started guides.

**Implementation Details:** Embed the WalkthroughPanel (already has built-in walkthroughs for Getting Started, Editor, Canvas, Notebooks, Graph) in the left column. Show walkthrough cards as collapsible sections. Each card shows: title, description, progress bar (completion percentage), step count. Clicking a card expands it to show individual steps. Completed steps show green checkmarks. Incomplete steps show empty circles. The "Getting Started" walkthrough is expanded by default for new users.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp` (modify)

**Acceptance Criteria:**
- Walkthroughs render as collapsible cards
- Progress bars show completion percentage
- Step checkmarks reflect completion state
- "Getting Started" expanded by default for new users
- Completing all steps shows "Completed" indicator

**Dependencies:** Task 01

---

### Task 09 -- Create SpotlightOverlay for Interactive Tutorials

**Description:** Create a spotlight overlay that highlights specific UI elements during interactive tutorials, dimming everything else.

**Implementation Details:** SpotlightOverlay is a full-screen transparent wxPanel that covers the MainFrame. It renders: (1) semi-transparent dark overlay (70% black) covering everything, (2) a "cut-out" rectangle that reveals the target element (no overlay in that area), (3) a tooltip/callout near the cut-out with instructional text and a "Next" button. The cut-out is positioned by finding the target element's screen rect (via wxWindow::GetScreenRect()). The callout positions itself above, below, left, or right of the cut-out based on available space. The spotlight animates when transitioning between targets (300ms slide).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.cpp` (create)

**Acceptance Criteria:**
- Overlay dims entire window except target element
- Cut-out precisely matches target element bounds
- Callout renders near the cut-out with instructional text
- "Next" button advances to next tutorial step
- Spotlight animates between targets

**Dependencies:** None

---

### Task 10 -- Create InteractiveTutorial Engine

**Description:** Create a tutorial engine that drives interactive step-by-step tutorials using SpotlightOverlay, coordinating between steps, validating user actions, and tracking completion.

**Implementation Details:** InteractiveTutorial manages a sequence of `TutorialStep` objects: `{ target_element_id, instruction_text, action_type (click/type/shortcut), validation_fn, on_complete_fn }`. The engine: (1) activates SpotlightOverlay on the target, (2) shows instruction text, (3) waits for user action (or "Skip" click), (4) validates the action, (5) advances to next step. Built-in tutorials: "Your First Note" (5 steps: click New File, type content, save, open preview, switch theme), "Using the File Tree" (4 steps), "Keyboard Shortcuts" (6 steps). Tutorial progress is saved per-user.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/InteractiveTutorial.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/InteractiveTutorial.cpp` (create)

**Acceptance Criteria:**
- Tutorial engine drives step-by-step flow
- SpotlightOverlay highlights correct target per step
- User actions are validated before advancing
- "Skip" button skips individual steps
- Tutorial progress persists across sessions

**Dependencies:** Task 09

---

### Task 11 -- Create ChangelogPanel for What's New

**Description:** Create a panel that displays the application changelog with version headers, bullet points, and category badges (Feature, Fix, Improvement).

**Implementation Details:** ChangelogPanel extends ThemeAwareWindow. It parses a `CHANGELOG.md` file from the application bundle. Each version renders as a section with: version number header (16px bold), release date (12px muted), and bulleted list of changes. Each change line has a category badge: blue "Feature" for new features, green "Fix" for bug fixes, yellow "Improvement" for enhancements. The panel scrolls. The most recent version is expanded by default; older versions are collapsed. A "View Full Changelog" link opens the complete CHANGELOG.md in the editor.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ChangelogPanel.cpp` (create)

**Acceptance Criteria:**
- Changelog renders from CHANGELOG.md
- Version sections with date headers
- Category badges for each change
- Most recent version expanded by default
- "View Full" link opens the file

**Dependencies:** None

---

### Task 12 -- Integrate What's New in Welcome Tab

**Description:** Show the latest version's changelog highlights in the Welcome tab when the app has been updated since the last launch.

**Implementation Details:** On startup, compare the current version with `app.lastSeenVersion` in Config. If the version has changed, show a "What's New in {version}" section in the WelcomeTab between Quick Actions and Walkthroughs. The section shows the top 5 changes from the latest version with category badges. A "See All Changes" link opens the full ChangelogPanel. After viewing, update `app.lastSeenVersion` to the current version. If the version has not changed, this section is hidden.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- "What's New" section appears after version upgrade
- Top 5 changes shown with badges
- "See All Changes" link opens full changelog
- Section hides after viewing (version stored)
- Section does not appear when version is unchanged

**Dependencies:** Tasks 01, 11

---

### Task 13 -- Create TipOfTheDay Component

**Description:** Create a Tip of the Day component that shows a random productivity tip with a "Did you know?" header.

**Implementation Details:** TipOfTheDay renders a card in the WelcomeTab (below walkthroughs) with: "Did You Know?" header, tip text (14px), and optional link to the related feature. Tips are stored as a JSON array in the application bundle (`tips.json`). Each tip: `{ text, category, related_command, link }`. The component selects a random tip that has not been shown in the last 30 days (tracked via Config). "Next Tip" and "Previous Tip" buttons allow cycling. Tips include: keyboard shortcuts, hidden features, workflow suggestions, extension recommendations.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TipOfTheDay.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/TipOfTheDay.cpp` (create)

**Acceptance Criteria:**
- Random tip displays on each startup
- Tips do not repeat within 30 days
- Next/Previous buttons cycle tips
- Related command links are clickable
- At least 30 tips in the initial set

**Dependencies:** Task 01

---

### Task 14 -- Implement Sample Project Opener

**Description:** Add a section to the Welcome tab that offers to open a sample project, giving new users something to explore immediately.

**Implementation Details:** Add "Try a Sample Project" section in the Welcome tab. Show 3 sample project cards: "Markdown Writing Project" (journal template with daily notes), "Documentation Wiki" (interlinked documents with backlinks), "Code Notebook" (mixed Markdown and code cells). Each card shows: title, description (2 lines), icon, and "Open" button. Clicking "Open" creates the sample project from a template in a temporary directory, then opens it as a workspace. Templates are stored as ZIP files in the application bundle and extracted on demand.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- 3 sample project cards render with descriptions
- Clicking "Open" creates and opens the project
- Sample projects contain relevant content
- Projects open in a temporary directory
- Sample section is visible to new users, dismissible for returning users

**Dependencies:** Task 01

---

### Task 15 -- Implement Keyboard Shortcut Cheat Sheet Link

**Description:** Add a link in the Welcome tab that opens the keyboard shortcut cheat sheet overlay.

**Implementation Details:** Add "Keyboard Shortcuts" link in the Quick Links section. Clicking opens the existing ShortcutOverlay panel (full-screen overlay showing all shortcuts categorized by context). The link shows the platform-specific shortcut to open the overlay: "Cmd+K Cmd+S" on macOS, "Ctrl+K Ctrl+S" on Windows/Linux. Also add a quick reference card in the Welcome tab showing the top 10 most useful shortcuts.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Link opens ShortcutOverlay
- Platform-specific shortcut shown
- Top 10 shortcuts card displays in Welcome tab
- Shortcuts are clickable (execute the command)
- Card is themed correctly

**Dependencies:** Task 04

---

### Task 16 -- Implement Theme Picker on First Run

**Description:** Show a theme selection section in the Welcome tab for first-time users, allowing them to choose between Dark, Light, and High Contrast themes.

**Implementation Details:** For first-time users (detected via `app.first_run_completed`), the Welcome tab shows a theme picker section at the top (before other content). The picker shows 3 theme preview cards side by side: Dark Modern, Light Modern, High Contrast. Each card shows a miniature preview of the editor with that theme applied (pre-rendered screenshots or live preview using ThemePreviewCard). Clicking a card applies the theme immediately. After selection, the theme section collapses to a single line: "Current Theme: {name} (Change)". This complements the FirstRunWizard theme page.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Theme picker visible only for first-time users
- 3 theme cards with previews
- Clicking applies theme immediately
- Section collapses after selection
- Works alongside FirstRunWizard

**Dependencies:** Task 01

---

### Task 17 -- Implement Telemetry Opt-In Prompt

**Description:** Show a telemetry opt-in/out prompt during first run, explaining what data is collected and providing clear choices.

**Implementation Details:** On first launch, after the FirstRunWizard (or in the Welcome tab for users who skip the wizard), show a non-modal banner: "Help improve MarkAmp Studio by sharing anonymous usage data." Two buttons: "Yes, share data" (primary) and "No, thanks" (secondary). A "Learn More" link explains what data is collected: feature usage counts, crash reports, performance metrics (no personal data, no file content). The choice persists via `app.telemetry.enabled` config key. The banner dismisses after a choice is made. If the user dismisses without choosing (X button), default to opt-out.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Telemetry prompt appears on first launch
- Two clear choices with descriptions
- "Learn More" link explains data collection
- Default is opt-out if dismissed without choosing
- Choice persists across restarts

**Dependencies:** Task 01

---

### Task 18 -- Enhance StartupPanel with WelcomeTab Integration

**Description:** Refactor the existing StartupPanel to delegate to WelcomeTab instead of rendering its own startup content. StartupPanel becomes a thin wrapper that decides whether to show the WelcomeTab.

**Implementation Details:** StartupPanel's responsibility narrows to: (1) checking `app.showWelcomeOnStartup` config, (2) if true, opening WelcomeTab as the first editor tab, (3) if false, opening the last workspace or empty editor. The existing `refreshRecentWorkspaces()`, `show_release_notes()`, `set_show_getting_started()` methods delegate to WelcomeTab and ChangelogPanel. The StartupPanel's own UI rendering is removed (it no longer draws its own content). This eliminates duplication between StartupPanel and WelcomeTab.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp` (modify)

**Acceptance Criteria:**
- StartupPanel delegates to WelcomeTab
- No duplicate startup UI rendering
- Config check determines whether to show Welcome
- Recent workspaces data flows to WelcomeTab
- Existing StartupPanel tests pass or are updated

**Dependencies:** Task 01

---

### Task 19 -- Enhance WalkthroughPanel with Step Media

**Description:** Add support for images and GIFs in walkthrough steps to make tutorials more visually engaging.

**Implementation Details:** Extend `WalkthroughStep` struct with `media_path` (optional string) and `media_type` (`Image`, `Gif`, `None`). When a step has media, the WalkthroughPanel renders the media below the step description as a 200px-wide image. GIFs animate automatically. Images are loaded from the application bundle (`resources/walkthroughs/` directory). The media area has a light border and rounded corners. If the media fails to load, the step renders without it (graceful fallback).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp` (modify)

**Acceptance Criteria:**
- Images render below step descriptions
- GIFs animate automatically
- Graceful fallback when media is missing
- Media area has border and rounded corners
- Media respects theme (border color adapts)

**Dependencies:** Task 08

---

### Task 20 -- Implement Walkthrough Extension Contribution

**Description:** Allow extensions to contribute their own walkthroughs that appear alongside built-in walkthroughs.

**Implementation Details:** Extensions define walkthroughs in their manifest: `contributes.walkthroughs: [{ id, title, description, steps: [{ id, title, description, action }] }]`. The WalkthroughPanel's `set_walkthroughs()` method (already exists) receives these from the extension loading pipeline. Extension walkthroughs render after built-in ones, in a separate "Extension Walkthroughs" section. Each extension walkthrough card shows the extension name and icon. Completion tracking works the same as built-in walkthroughs.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WalkthroughPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/ExtensionManifest.h` (modify if needed)

**Acceptance Criteria:**
- Extension walkthroughs appear in separate section
- Extension name and icon shown on cards
- Completion tracking works for extension walkthroughs
- Extension walkthroughs load from manifest
- No built-in walkthroughs are displaced

**Dependencies:** Task 08

---

### Task 21 -- Implement First-Time Workspace Setup Wizard

**Description:** When the user creates or opens a workspace for the first time, show a workspace setup wizard that configures common options.

**Implementation Details:** Triggered when a workspace is opened and no `.markamp/` directory exists. The wizard (using WizardDialog from Phase 44) has 3 steps: (1) "File Types" -- checkboxes for Markdown, Notebooks, Canvas, Code (determines which built-in walkthroughs to promote), (2) "Workspace Settings" -- line endings (LF/CRLF), encoding (UTF-8/UTF-16), spell check language, (3) "Git Integration" -- initialize git repo option, .gitignore template. On finish, create `.markamp/workspace.json` with the configured settings. Skippable via "Use Defaults" button.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Wizard appears on first workspace open
- 3 setup steps with appropriate controls
- "Use Defaults" skips with default settings
- Settings persist to workspace.json
- Wizard does not appear on subsequent opens

**Dependencies:** Phase 44 Task 07 (WizardDialog)

---

### Task 22 -- Implement Welcome Tab Responsive Layout

**Description:** Make the Welcome tab responsive to different window sizes: switch from two-column to single-column layout when the window is narrow.

**Implementation Details:** When the WelcomeTab width is below 800px, switch from two-column (60/40) to single-column layout where recent items appear below the quick actions and walkthroughs. When width is above 1200px, add a third column for tip of the day and sample projects. The layout recalculates on window resize events. All sections use flexible sizing with min/max constraints. Scrollbar appears when content exceeds viewport height.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)

**Acceptance Criteria:**
- Below 800px: single-column layout
- 800-1200px: two-column layout
- Above 1200px: three-column layout
- Layout recalculates on resize
- No content cutoff at any window size

**Dependencies:** Task 01

---

### Task 23 -- Add Welcome/Onboarding Commands to Command Palette

**Description:** Register all welcome and onboarding commands in the Command system.

**Implementation Details:** Register commands: `help.welcome` (opens Welcome tab), `help.whatsNew` (opens changelog for current version), `help.interactiveTutorial` (starts Getting Started tutorial), `help.keyboardShortcuts` (opens shortcut overlay), `help.tipOfTheDay` (shows random tip), `help.sampleProject` (opens sample project dialog), `help.about` (opens About dialog from Phase 44). All categorized under "Help" in the palette. `help.welcome` shortcut: none by default (accessed via palette or Help menu).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Command.cpp` (modify)

**Acceptance Criteria:**
- All 7 commands appear in Command Palette under "Help"
- Commands execute correct actions
- No shortcut conflicts
- Commands work from any context
- About dialog opens correctly

**Dependencies:** Tasks 01, 10, 11, 13

---

### Task 24 -- Implement Welcome Tab Accessibility

**Description:** Ensure the Welcome tab is accessible: all sections have correct roles, navigation works via keyboard, screen reader announces content.

**Implementation Details:** WelcomeTab has role `wxACC_ROLE_DOCUMENT`. Quick action buttons are in Tab order. Recent items list has role `wxACC_ROLE_LIST`. Walkthrough cards have role `wxACC_ROLE_GROUPING`. Tab key navigates between sections. Enter activates focused action. All images have alt text. Screen reader announces section headers. The spotlight overlay (tutorial) announces step instructions. Focus management: on tab open, focus lands on the first quick action button.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/WelcomeTab.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InteractiveTutorial.cpp` (modify)

**Acceptance Criteria:**
- All sections have correct accessible roles
- Tab navigation covers all interactive elements
- Screen reader announces section headers
- Spotlight overlay announces tutorial steps
- Focus lands on first action on tab open

**Dependencies:** Tasks 01-10

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new welcome/onboarding files to CMakeLists.txt and create unit tests.

**Implementation Details:** Add all new .h/.cpp files to CMakeLists.txt. Create `test_welcome_onboarding.cpp` with sections: (1) RecentItemsList renders correct number of items, (2) RecentItemsList filters by search text, (3) TipOfTheDay selects non-repeating tips, (4) TipOfTheDay respects 30-day cooldown, (5) InteractiveTutorial advances through steps, (6) InteractiveTutorial tracks completion, (7) ChangelogPanel parses version sections, (8) WelcomeTab respects "show on startup" setting, (9) Walkthrough completion percentage calculation, (10) Sample project extraction succeeds.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_welcome_onboarding.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- Logic tests work without GUI
- No undefined symbol errors
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- Welcome tab opens on startup with two-column layout
- Recent files/workspaces show with icons and timestamps
- Walkthroughs render with progress tracking
- Interactive tutorial spotlight highlights UI elements
- What's New section appears after version upgrade
- Tip of the Day rotates non-repeating tips
- Sample projects can be opened from Welcome tab
- FirstRunWizard and StartupPanel integrated with WelcomeTab
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
