# Phase 43 -- Notification System V2

## Objective

Evolve the existing toast notification system into a full Notification Center with persistent notification history, categories, actions, grouping, progress tracking, do-not-disturb mode, and extension integration. The current NotificationManager handles basic toasts with fade animation. The NotificationModel has the data model for severity filtering and quiet hours. This phase bridges the two, adds a Notification Center panel, and completes all missing workflows.

## Prerequisites

- Phase 42 complete (Scrollbar and Scroll Behavior)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.h` -- existing toast system with fade animation
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` -- data model with severity filtering, quiet hours
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h` -- core notification service
- `/Users/ryanrentfro/code/markamp/src/core/ProgressService.h` -- existing progress tracking service
- Phase 41 components: ThemedButton, Badge, IconButton, RichTooltip

## VSCode / WebStorm Reference Behavior

- Bell icon in status bar with unread count badge
- Clicking bell opens notification center panel (dropdown or sidebar)
- Notifications grouped by source (Git, Extensions, System)
- Each notification has: icon, title, message, timestamp, actions (up to 2 buttons)
- Progress notifications show progress bar with cancel button
- "Do Not Disturb" mode suppresses all toast popups (but still records in center)
- Notification history persists across sessions (last 100 notifications)
- Grouped notifications collapse similar items (e.g., "5 extension updates available")
- Clear All button removes all notifications from the center

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_notification_system_v2.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Extend NotificationModel with Full Category Support

**Description:** Extend the existing NotificationModel to support notification categories beyond severity. Add category tags (Info, Warning, Error, Success, Progress) and source tracking (System, Git, Extension, Build, Search).

**Implementation Details:** Add `NotificationCategory` enum: `kInfo, kWarning, kError, kSuccess, kProgress`. Add `source` field to NotificationItem (already exists as string). Add `category` field. Add `by_category(NotificationCategory)` query method. Add `by_source_and_category()` combined filter. The existing `NotificationSeverity` maps to category: Info->kInfo, Warning->kWarning, Error->kError. Success and Progress are new categories not present in severity.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)

**Acceptance Criteria:**
- NotificationCategory enum includes all 5 categories
- by_category() returns correct filtered results
- Existing severity-based queries still work (backward compatible)
- Combined source+category filter works
- Unit test validates all filter combinations

**Dependencies:** None

---

### Task 02 -- Add Notification Actions to Model

**Description:** Extend NotificationItem to support action buttons (up to 3 per notification) with labels and callback identifiers. Actions enable in-notification interactions like "Retry", "Show Details", "Install Update".

**Implementation Details:** The existing `action_labels` vector in NotificationItem stores label strings. Add `action_ids` vector (`std::vector<std::string>`) to pair with labels. Add `NotificationActionEvent` to Events.h with fields: `notification_id`, `action_id`. When a user clicks an action button, the NotificationCenter publishes this event. The originator of the notification subscribes to this event filtered by their notification_id.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/Events.h` (modify)

**Acceptance Criteria:**
- NotificationItem supports up to 3 action labels with corresponding IDs
- NotificationActionEvent is declared and publishable via EventBus
- Action IDs are unique per notification
- Model rejects notifications with mismatched label/ID count

**Dependencies:** Task 01

---

### Task 03 -- Add Progress Notification Support

**Description:** Create a progress notification variant that displays a progress bar within the notification, a percentage, and an optional cancel button. Integrates with the existing ProgressService.

**Implementation Details:** Add `progress` field to NotificationItem: `std::optional<NotificationProgress>` where `NotificationProgress { double value; double max; bool cancellable; bool indeterminate; }`. Progress notifications do not auto-dismiss -- they persist until the progress completes or is cancelled. When `value >= max`, the notification transitions to a Success category. When cancelled, the notification is dismissed and a `ProgressCancelledEvent` is published. The existing `ProgressService` is extended to create notification items automatically when `show_notification` flag is set.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/ProgressService.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/ProgressService.cpp` (modify)

**Acceptance Criteria:**
- Progress notifications show a progress bar in the notification body
- Progress updates in real time as value changes
- Cancel button appears when cancellable is true
- Completed progress transitions to Success category
- Indeterminate mode shows shimmer animation

**Dependencies:** Task 01

---

### Task 04 -- Add Notification Grouping Logic

**Description:** Implement grouping logic that collapses similar notifications into a single grouped entry with a count badge. For example, "5 extension updates available" instead of 5 separate notifications.

**Implementation Details:** Add `group_key` field to NotificationItem (optional string). Notifications with the same `group_key` are collapsed in the model. Add `grouped_items()` query that returns `std::vector<NotificationGroup>` where `NotificationGroup { string group_key; string summary; int count; NotificationItem latest; }`. The summary is generated from the group key template: e.g., `group_key="extension.update"` with template `"{count} extension updates available"`. When a group is expanded, all individual items are shown. When collapsed, only the summary with count badge is shown.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)

**Acceptance Criteria:**
- Notifications with same group_key are collapsed
- Group shows count badge with number of collapsed items
- Group can be expanded to show individual items
- Adding a new notification to an existing group increments count
- Dismissing a group dismisses all items in the group

**Dependencies:** Task 01

---

### Task 05 -- Add Notification Priority and DND Override

**Description:** Implement notification priority levels that determine whether a notification can override Do Not Disturb mode. Urgent notifications (errors, security) break through DND.

**Implementation Details:** Add `NotificationPriority` enum: `kLow, kNormal, kHigh, kUrgent`. Urgent priority notifications always show toast popups even in DND mode. High priority notifications show in the notification center with a distinct visual indicator but do not show toasts in DND mode. Low priority notifications are only recorded in the notification center (never show toasts, even outside DND). The existing `should_suppress()` method is updated to check priority: urgent is never suppressed, high is suppressed only in DND, normal follows standard DND rules, low never shows toasts.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)

**Acceptance Criteria:**
- Urgent notifications always show toasts (even in DND)
- High priority notifications are recorded but suppressed in DND
- Low priority notifications never show toasts
- Priority affects visual rendering (urgent has red accent)
- DND status is queryable for UI indicators

**Dependencies:** Task 01

---

### Task 06 -- Add Notification Persistence to Disk

**Description:** Persist the last 100 notifications to disk so the notification history survives application restarts.

**Implementation Details:** Create `notification_history.json` in the workspace `.markamp/` directory. On notification push, append to the JSON file (using nlohmann/json). On application startup, load the file and populate the NotificationModel. Trim to the most recent 100 entries on load. Timestamps are stored as ISO 8601 strings. Progress notifications are not persisted (they are transient). Dismissed notifications are stored with `is_dismissed: true` but can be cleared by the user.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` (modify)

**Acceptance Criteria:**
- Notification history survives application restart
- Maximum 100 notifications are stored
- Oldest notifications are trimmed when limit is reached
- Progress notifications are not persisted
- Corrupted JSON file is handled gracefully (empty history, log warning)

**Dependencies:** Task 01

---

### Task 07 -- Create NotificationCenter Panel

**Description:** Build the Notification Center UI panel that appears when the user clicks the bell icon in the status bar. The panel displays all notifications with filtering, grouping, and actions.

**Implementation Details:** NotificationCenter extends ThemeAwareWindow. It is a `wxPopupTransientWindow` that appears as a dropdown from the bell icon in the status bar. Layout from top to bottom: (1) Header bar with "Notifications" title, filter dropdown (All/Errors/Warnings/Info), and "Clear All" icon button. (2) Scrollable list of notification items rendered as custom-painted rows. Each row shows: category icon (16x16), title (bold), message (normal), timestamp (relative: "2m ago"), and action buttons (ThemedButton, Ghost variant). Grouped notifications show a collapsible header with count badge. Empty state shows "No notifications" centered text.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (create)

**Acceptance Criteria:**
- Panel opens as dropdown from bell icon position
- All active notifications render with correct icons and formatting
- Filter dropdown filters by category in real time
- Grouped notifications show collapsible headers
- Clear All button dismisses all notifications
- Empty state shows "No notifications" message

**Dependencies:** Tasks 01-05, Phase 41 (ThemedButton, IconButton, ThemedDropdown, Badge)

---

### Task 08 -- Add Bell Icon with Unread Badge to Status Bar

**Description:** Add a notification bell icon to the status bar with an unread count badge that updates in real time.

**Implementation Details:** Add a new segment to StatusBarPanel (right-aligned, before the line/column indicator). The segment renders a bell icon (16x16) and overlays a Badge component (from Phase 41) showing the unread count. Badge is hidden when count is 0. Badge uses danger color for error notifications, warning color for warnings, default color otherwise (highest severity wins). Clicking the bell icon toggles the NotificationCenter popup. The icon has a subtle shake animation when a new notification arrives (100ms, 3-pixel horizontal oscillation).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Bell icon renders in status bar right section
- Unread badge shows correct count, hidden at 0
- Badge color reflects highest severity of unread notifications
- Click toggles notification center popup
- Shake animation plays on new notification arrival

**Dependencies:** Task 07, Phase 41 Task 12 (Badge)

---

### Task 09 -- Update NotificationManager to Route Through NotificationModel

**Description:** Refactor the existing NotificationManager to use NotificationModel as its data source instead of managing its own internal `std::deque<NotificationEntry>`. This unifies the toast system with the notification center.

**Implementation Details:** Remove the internal `toasts_` deque from NotificationManager. Instead, NotificationManager reads from NotificationModel's active notifications for rendering toasts. When `ShowNotification()` is called, it creates a NotificationItem and pushes it to NotificationModel. The model determines whether to show a toast (based on priority and DND) and the manager handles the visual rendering. The manager's animation timer continues to handle fade-in/fade-out of toast visuals.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp` (modify)

**Acceptance Criteria:**
- NotificationManager no longer has internal toast storage
- All notifications flow through NotificationModel
- Toast appearance respects DND and priority settings
- Existing toast fade animation still works
- No duplicate notifications between toast and center

**Dependencies:** Tasks 01, 05

---

### Task 10 -- Implement Notification Sound Option

**Description:** Add optional sound playback when notifications arrive, with per-category sound settings and a global mute toggle.

**Implementation Details:** Add settings: `notifications.sound.enabled` (bool, default false), `notifications.sound.errorSound` (string, path to WAV), `notifications.sound.warningSound`, `notifications.sound.infoSound`. Use platform audio APIs: `NSSound` on macOS (via MacPlatform), `PlaySound` on Windows (via WinPlatform), PulseAudio on Linux (via LinuxPlatform). Sound does not play in DND mode. Sound does not play for low-priority notifications. Provide 3 built-in sounds bundled with the app (subtle chime, alert, error buzz).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/platform/MacPlatform.mm` (modify)

**Acceptance Criteria:**
- Sound plays on notification arrival when enabled
- Different sounds for different categories
- Sound respects DND mode (no sound in DND)
- Sound can be muted globally via settings
- Built-in sounds are bundled and work without configuration

**Dependencies:** Task 05

---

### Task 11 -- Implement Do Not Disturb Toggle

**Description:** Add a Do Not Disturb (DND) toggle accessible from the status bar and command palette. When active, suppresses all toast popups except urgent-priority notifications.

**Implementation Details:** Add a DND toggle button next to the bell icon in the status bar (moon icon). Clicking toggles DND state via `NotificationModel::set_quiet_hours(bool)`. When DND is active, the bell icon gets a small "zzz" overlay indicator. DND state persists across sessions via Config. Add a command `notifications.toggleDoNotDisturb` in the Command system. Add a scheduled DND mode: `notifications.dnd.schedule.start` and `notifications.dnd.schedule.end` (time strings like "22:00" and "08:00") for automatic DND during sleeping hours.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)

**Acceptance Criteria:**
- DND toggle in status bar works with one click
- DND is accessible via command palette
- Bell icon shows DND indicator when active
- DND state persists across restarts
- Scheduled DND activates/deactivates at configured times

**Dependencies:** Task 05

---

### Task 12 -- Implement Notification Row Rendering

**Description:** Render individual notification rows in the NotificationCenter with proper layout, theming, and interactive elements (dismiss button, action buttons).

**Implementation Details:** Each notification row is custom-painted (not a separate wxPanel to avoid widget overhead). Row height: variable based on content (min 48px, max 96px). Layout: [category icon 16px | content area flex | dismiss X button 20px]. Content area: title (13px bold), message (12px normal, max 2 lines with ellipsis), timestamp (11px, muted color, relative format). Action buttons render at the bottom of the row using Ghost-variant ThemedButton (created dynamically on hover, recycled). Row hover shows subtle background highlight. Row swipe-left gesture dismisses (on supported trackpads).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)

**Acceptance Criteria:**
- Rows render with correct layout and typography
- Dismiss X button appears on hover, dismisses on click
- Action buttons render at row bottom
- Timestamp shows relative time ("2m ago", "1h ago", "Yesterday")
- Long messages truncate with ellipsis at 2 lines

**Dependencies:** Task 07

---

### Task 13 -- Implement Notification Filter by Source

**Description:** Add filtering in the NotificationCenter to show only notifications from a specific source (System, Git, Extensions, Build, etc.).

**Implementation Details:** Add a second filter dropdown (or tabs) in the NotificationCenter header: "All Sources", "System", "Git", "Extensions", "Build". When a source filter is selected, the notification list only shows items matching that source. Source filter combines with category filter (AND logic). Filter state resets when the center is closed and reopened. Add a source icon next to the source name in the filter dropdown for visual clarity.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)

**Acceptance Criteria:**
- Source filter dropdown shows all known sources
- Selecting a source filters the notification list
- Source and category filters combine (AND logic)
- "All Sources" shows all notifications
- Filter resets on center close/reopen

**Dependencies:** Task 07

---

### Task 14 -- Implement Mark All as Read

**Description:** Add a "Mark All as Read" action that clears the unread badge without dismissing notifications from the center.

**Implementation Details:** Add a "Mark All Read" icon button in the NotificationCenter header (envelope-open icon). Clicking it calls `NotificationModel::mark_all_read()` (new method) which iterates all items and sets `is_read = true`. The unread count on the bell badge drops to 0. Individual notifications can also be marked as read by clicking on them. Opening the NotificationCenter does NOT auto-mark notifications as read (explicit action required, matching Slack's behavior).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)

**Acceptance Criteria:**
- "Mark All Read" button clears unread badge to 0
- Individual click on notification marks it as read
- Opening center does not auto-mark as read
- Read notifications show dimmed styling in center
- Unread notifications show a subtle left border accent

**Dependencies:** Task 07

---

### Task 15 -- Implement Extension Notification API

**Description:** Expose a notification API to extensions so they can send notifications with custom categories, actions, and progress.

**Implementation Details:** Extend the PluginContext notification interface to support the full NotificationItem model. Extensions call `context.notifications.show({ title, message, category, priority, actions, progress })`. The core NotificationService validates the request (strips HTML, enforces max message length of 500 chars, limits to 3 actions) and routes to NotificationModel. Extension notifications are tagged with `source: "Extension: {extension_name}"`. Rate limiting: max 10 notifications per extension per minute (excess are queued).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` (modify)

**Acceptance Criteria:**
- Extensions can send notifications via PluginContext
- Extension name appears as notification source
- HTML is stripped from extension notification messages
- Rate limiting prevents notification spam (10/min/extension)
- Progress notifications from extensions update in real time

**Dependencies:** Task 09

---

### Task 16 -- Implement Notification Expiry and Auto-Cleanup

**Description:** Add automatic cleanup of old notifications. Notifications older than 7 days are automatically removed from the history. Dismissed notifications are removed after 24 hours.

**Implementation Details:** Add a periodic cleanup task that runs every hour (via wxTimer). The task checks all notifications: items older than 7 days (based on timestamp) are removed from the model. Dismissed items older than 24 hours are removed. Progress notifications that have been idle (no update) for 1 hour are auto-cancelled and removed. The cleanup respects pinned notifications (future feature: `is_pinned` flag that prevents auto-cleanup). Cleanup runs on application startup as well.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)

**Acceptance Criteria:**
- Notifications older than 7 days are auto-removed
- Dismissed notifications older than 24 hours are auto-removed
- Stale progress notifications are auto-cancelled after 1 hour
- Cleanup runs on startup and hourly thereafter
- Cleanup does not remove pinned notifications

**Dependencies:** Task 06

---

### Task 17 -- Implement Toast Position Configuration

**Description:** Allow the user to configure where toast notifications appear: bottom-right (default), bottom-left, top-right, top-left.

**Implementation Details:** Add setting `notifications.toast.position` (string: "bottom-right", "bottom-left", "top-right", "top-left"). The NotificationManager's UpdateLayout method positions toasts based on this setting. Toast stack grows upward from bottom positions and downward from top positions. The animation direction adjusts accordingly (slide from right for right positions, slide from left for left positions). Position change takes effect immediately.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp` (modify)

**Acceptance Criteria:**
- Toasts appear at configured corner of the window
- Stack direction is correct for each position
- Changing position moves existing toasts to new position
- Animation direction matches position
- Default position is bottom-right

**Dependencies:** Task 09

---

### Task 18 -- Implement Toast Width and Duration Configuration

**Description:** Allow the user to configure toast notification width and auto-dismiss duration.

**Implementation Details:** Add settings: `notifications.toast.width` (int, default 360, min 280, max 500), `notifications.toast.duration` (int, default 3000, min 1000, max 30000). The NotificationManager reads these settings and applies them to new toasts. Currently, width is hardcoded at `kToastWidth = 360` and duration at 3000ms in NotificationEntry. Replace with configurable values. Error notifications get 2x duration (they are more important). Progress notifications never auto-dismiss.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp` (modify)

**Acceptance Criteria:**
- Toast width respects configured value
- Auto-dismiss duration respects configured value
- Error notifications get double duration
- Progress notifications never auto-dismiss
- Settings are clamped to min/max bounds

**Dependencies:** Task 09

---

### Task 19 -- Add Notification Center Keyboard Navigation

**Description:** Add keyboard navigation to the NotificationCenter: arrow keys to navigate items, Enter to execute primary action, Delete to dismiss, Escape to close the center.

**Implementation Details:** The NotificationCenter maintains a `focused_index_` for the currently focused notification row. Up/Down arrow keys move focus. Enter executes the first action button (if any). Delete/Backspace dismisses the focused notification. Tab moves between the filter dropdown, notification list, and action buttons. Escape closes the notification center popup. Focus indicator is a 2px accent-colored left border on the focused row.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)

**Acceptance Criteria:**
- Arrow keys navigate notification list
- Enter executes primary action on focused notification
- Delete dismisses focused notification
- Escape closes the notification center
- Tab cycles between filter controls and notification list
- Focus indicator is visible on current item

**Dependencies:** Task 07

---

### Task 20 -- Implement Notification Snooze

**Description:** Add the ability to snooze a notification for a configurable duration (15 min, 1 hour, 4 hours, until tomorrow), hiding it temporarily.

**Implementation Details:** Add "Snooze" option to the notification row context menu (right-click). Snooze submenu shows duration options. Snoozed notifications are hidden from the active list but remain in the model with a `snoozed_until` timestamp. A periodic check (every minute) moves expired snoozed notifications back to the active list. Snoozed notifications show a clock icon in the notification center when viewing "All" (including snoozed). Add `snoozed_count()` query to NotificationModel.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)

**Acceptance Criteria:**
- Snooze hides notification for selected duration
- Notification reappears when snooze expires
- Snooze options: 15min, 1h, 4h, until tomorrow
- Snoozed count is queryable
- Snoozed notifications show clock icon when filter shows all

**Dependencies:** Task 07

---

### Task 21 -- Implement Notification Templates for Common Events

**Description:** Create notification templates for common system events: file save success, build complete, extension install complete, git push success, search complete.

**Implementation Details:** Create a `NotificationTemplates` namespace with factory functions: `file_saved(filename)`, `build_complete(success, duration)`, `extension_installed(name)`, `git_push_complete(branch, remote)`, `search_complete(query, count)`. Each factory creates a fully populated NotificationItem with appropriate category, priority, source, and actions. For example, `build_complete(false, 12.5s)` creates an Error notification with "Build Failed in 12.5s" title and a "Show Build Output" action.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` (modify)

**Acceptance Criteria:**
- All 5 template factories produce correctly populated NotificationItems
- Templates use appropriate categories and priorities
- Action buttons in templates are functional (e.g., "Show Build Output" opens output panel)
- Templates are used by the respective subsystems
- Template messages are clear and concise

**Dependencies:** Task 02

---

### Task 22 -- Add Settings UI for Notification Preferences

**Description:** Create a "Notifications" section in the Settings panel with all notification-related settings: DND toggle, sound settings, toast position, duration, extension notification permissions.

**Implementation Details:** Add "Notifications" section to SettingsPanel. Controls: DND toggle (ThemedToggleSwitch), sound enabled (ThemedToggleSwitch), toast position (ThemedDropdown), toast duration (NumberStepper), toast width (NumberStepper), per-extension notification permissions (list of extensions with ThemedDropdown: Allow/Block). Use the new Phase 41 controls throughout. Group settings into subsections: "Toast Appearance", "Do Not Disturb", "Sound", "Extension Permissions".

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (modify)

**Acceptance Criteria:**
- All notification settings appear in Settings panel
- Changes take effect immediately (no restart required)
- Per-extension permissions allow blocking noisy extensions
- Settings persist across restarts
- Subsection grouping makes settings discoverable

**Dependencies:** Tasks 10, 11, 17, 18

---

### Task 23 -- Wire Notification Center to Command Palette

**Description:** Add commands for notification management accessible via the Command Palette.

**Implementation Details:** Register commands: `notifications.showCenter` (toggles notification center), `notifications.clearAll` (dismisses all), `notifications.markAllRead`, `notifications.toggleDND`, `notifications.showLastNotification` (focuses the most recent notification). All commands are registered in the Command system with appropriate keybindings: `Cmd+Shift+N` for showing center (macOS), `Ctrl+Shift+N` (Windows/Linux). Commands appear in the Command Palette with descriptive names.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/Command.cpp` (modify)

**Acceptance Criteria:**
- All 5 commands appear in Command Palette
- Keyboard shortcuts work from any context
- Commands execute correct notification actions
- Commands are categorized under "Notifications" in palette
- Toggle commands reflect current state (e.g., DND on/off)

**Dependencies:** Task 07, Task 11

---

### Task 24 -- Add Notification Center Accessibility

**Description:** Ensure the NotificationCenter is fully accessible: screen reader announcements for new notifications, ARIA-equivalent roles for all elements, and keyboard-only operability.

**Implementation Details:** New notifications trigger a screen reader announcement via `wxAccessible::NotifyEvent(wxACC_EVENT_ALERT)` with the notification title. NotificationCenter has role `wxACC_ROLE_LIST`. Each notification row has role `wxACC_ROLE_LISTITEM` with name "{title}: {message}". Action buttons have role `wxACC_ROLE_PUSHBUTTON`. Filter dropdown has role `wxACC_ROLE_COMBOBOX`. All interactive elements are in the Tab order. The bell icon button announces its state ("Notifications, {count} unread").

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NotificationCenter.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- VoiceOver announces new notifications
- All notification center elements have correct roles
- Keyboard navigation covers all interactive elements
- Bell icon announces unread count
- Screen reader can read notification content

**Dependencies:** Task 19

---

### Task 25 -- Add CMake Integration and Comprehensive Unit Tests

**Description:** Add all new notification source files to CMakeLists.txt and create comprehensive Catch2 unit tests covering the full notification lifecycle.

**Implementation Details:** Add NotificationCenter.h/.cpp to CMakeLists.txt. Create `test_notification_system_v2.cpp` with sections: (1) Category filtering returns correct items, (2) Priority levels affect suppression in DND, (3) Grouping collapses notifications with same group_key, (4) Progress notifications update and complete correctly, (5) Persistence saves/loads 100 notifications, (6) Expiry removes old notifications, (7) Snooze hides and restores notifications, (8) Rate limiting caps extension notifications, (9) Action events fire with correct IDs, (10) Mark-all-read clears unread count.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_notification_system_v2.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- No undefined symbol errors
- Test coverage includes edge cases (empty state, max capacity, concurrent updates)
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- NotificationCenter panel opens from bell icon
- Toast system routes through unified NotificationModel
- DND mode suppresses toasts (except urgent)
- Progress notifications work with progress bar and cancel
- Grouping collapses similar notifications
- History persists across application restarts
- All keyboard and accessibility requirements met
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
