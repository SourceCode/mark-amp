# Phase 39: Notification and Activity System

## Overview
NotificationService (with TODO at line 45 for action button callback), NotificationManager in UI, and the Event system provide infrastructure for notifications and activity tracking. However, notifications are not consistently wired across the application: some events produce notifications while others are silent. There is no activity timeline or notification center. This phase builds a complete notification and activity system.

## Prerequisites
- Phase 18 (Tool window system for notification panel)
- Phase 33 (Structured logging for activity events)

## Tasks

### Task 1: Wire NotificationService Action Callbacks
**Files:** `src/core/NotificationService.cpp` (TODO at line 45)
**Description:** NotificationService has TODO for action button callbacks. Wire action buttons on notifications so clicking performs the associated action.
**Acceptance Criteria:**
- Notification actions have click handlers
- "Show" action opens relevant panel/file
- "Fix" action applies suggested fix
- "Dismiss" closes notification
- Actions executed via command IDs
- `NotificationActionClickedEvent` emitted

### Task 2: Wire Notification Center Panel
**Files:** `src/ui/NotificationManager.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** Notification center: panel that shows all notifications (current and history). Grouped by category with clear and dismiss actions.
**Acceptance Criteria:**
- Notification center accessible from status bar bell icon
- Notifications grouped: Errors, Warnings, Info
- Each notification: timestamp, title, message, actions
- "Clear All" button
- "Clear Category" per group
- Unread count badge on bell icon

### Task 3: Wire Notification Routing Rules
**Files:** `src/core/NotificationService.cpp`, `src/core/Config.h`
**Description:** Configurable notification routing: which events produce notifications, notification priority, and display behavior.
**Acceptance Criteria:**
- Route by source: Editor, Canvas, Sync, Extensions, Git
- Priority: critical (always show), normal (show toast), low (silent, in center)
- "Do Not Disturb" mode: only critical notifications
- Per-source enable/disable
- Routing rules in settings
- Default routes for all event types

### Task 4: Wire Toast Notification Display
**Files:** `src/ui/NotificationManager.cpp`
**Description:** Toast notifications: non-modal popups in corner. Auto-dismiss after timeout. Stack multiple toasts. Action buttons inline.
**Acceptance Criteria:**
- Toast appears in bottom-right corner
- Auto-dismiss: 5 seconds (info), 10 seconds (warning), manual (error)
- Stack: up to 3 visible, queue remainder
- Action buttons inline with toast
- Hover: pause auto-dismiss timer
- Close button on each toast

### Task 5: Wire Activity Timeline
**Files:** `src/core/OutputChannelService.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** Activity timeline: chronological log of all significant user and system actions. Provides audit trail and context.
**Acceptance Criteria:**
- Timeline shows: timestamp, icon, action, detail
- Actions: file open/save, search, navigation, sync, commit
- Clickable: navigate to referenced file/object
- Filter by activity type
- Time grouping: Today, Yesterday, This Week
- Exportable as Markdown report

### Task 6: Wire File Activity Tracking
**Files:** `src/core/VaultService.cpp`, `src/core/NotificationService.cpp`
**Description:** Track file activity: create, modify, delete, rename. Show in activity timeline. Recent file activity available in command palette.
**Acceptance Criteria:**
- File create: logged with path and method (new, template, import)
- File modify: logged with timestamp (not content)
- File delete: logged with path
- File rename: logged with old and new path
- "Activity: Show File History" command
- File activity persisted across sessions

### Task 7: Wire Extension Activity Tracking
**Files:** `src/core/PluginManager.cpp`, `src/core/NotificationService.cpp`
**Description:** Track extension activity: load, unload, error, update. Extensions can publish activity events.
**Acceptance Criteria:**
- Extension load: logged with name, version, time
- Extension error: logged with error details
- Extension update: logged with old and new version
- Extension activity in "Extensions" section of timeline
- Extensions can publish custom activity events
- Excessive activity rate-limited

### Task 8: Wire Sync Activity Notifications
**Files:** `src/core/CloudSyncService.cpp`, `src/core/NotificationService.cpp`
**Description:** Sync events produce appropriate notifications: sync start (silent), sync complete (info), sync conflict (warning), sync error (error).
**Acceptance Criteria:**
- Sync start: status bar indicator only
- Sync complete: toast with file count
- Sync conflict: warning toast with "Resolve" action
- Sync error: error toast with "Retry" action
- Sync notifications configurable
- Sync activity in timeline

### Task 9: Wire Git Activity Notifications
**Files:** `src/core/GitService.cpp`, `src/core/NotificationService.cpp`
**Description:** Git events produce notifications: commit (info), push/pull (info), merge conflict (warning), branch switch (info).
**Acceptance Criteria:**
- Commit: toast with message preview
- Push/pull: toast with sync status
- Merge conflict: warning with "Resolve" action
- Branch switch: toast with branch name
- Git notifications configurable
- Git activity in timeline

### Task 10: Wire Error Notification Aggregation
**Files:** `src/core/NotificationService.cpp`
**Description:** Aggregate rapid error notifications: if same error occurs 5+ times in 1 minute, show single notification with count.
**Acceptance Criteria:**
- Same error within 1 minute: aggregate into "X occurrences"
- Aggregated notification: "Show All" expands to list
- Different errors: show separately
- Aggregate threshold configurable
- Aggregate count badge on notification
- First occurrence shown immediately

### Task 11: Wire Progress Notifications
**Files:** `src/core/NotificationService.cpp`, `src/ui/NotificationManager.cpp`
**Description:** Long-running operations show progress notifications: export, sync, index build, batch operations.
**Acceptance Criteria:**
- Progress notification: title, progress bar, cancel button
- Progress updates in real-time (percentage)
- Completion: progress notification replaced with result
- Cancel: sends cancellation signal to operation
- Multiple progress notifications stacked
- Progress visible in notification center

### Task 12: Wire Notification Sound and Vibration
**Files:** `src/core/NotificationService.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Optional sound for notifications. Platform-native notification sound for errors and important events.
**Acceptance Criteria:**
- Sound on error notification (platform system sound)
- Sound on sync conflict
- Sound configurable: on/off per priority level
- macOS: NSSound
- Windows: SystemSounds
- Linux: libcanberra
- Master mute option

### Task 13: Wire Badge System for Panels
**Files:** `src/ui/ActivityBar.cpp`, `src/core/NotificationService.cpp`
**Description:** Activity bar items show notification badges: unread count for each panel (Problems: error count, Extensions: update count, Git: change count).
**Acceptance Criteria:**
- Problems badge: error + warning count
- Extensions badge: available updates count
- Git badge: uncommitted change count
- Tasks badge: due task count
- Flashcard badge: due card count
- Badges clear when panel is viewed

### Task 14: Wire Notification Persistence
**Files:** `src/core/NotificationService.cpp`
**Description:** Notifications persisted for current session. Important notifications persisted across sessions (sync conflicts, errors requiring action).
**Acceptance Criteria:**
- Session notifications: available until app close
- Persistent notifications: survive app restart
- Persistent types: conflicts, errors requiring action
- Auto-expire: persistent notifications expire after 7 days
- Storage: `.markamp/notifications.json`
- Startup: load persistent notifications

### Task 15: Wire "Do Not Disturb" Mode
**Files:** `src/core/NotificationService.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** DND mode: suppress all toast notifications. Notifications still logged to notification center. Toggle via status bar or command.
**Acceptance Criteria:**
- DND toggle in status bar (moon icon)
- DND: no toast notifications
- Critical errors still shown (bypass DND)
- DND indicator in status bar
- Scheduled DND: enable during specific hours
- "Toggle Do Not Disturb" command

### Task 16: Wire Notification Templates
**Files:** `src/core/NotificationService.cpp`
**Description:** Standardized notification templates for consistent messaging: file operation, sync status, error report, extension update.
**Acceptance Criteria:**
- Template: title pattern, message pattern, actions, icon
- File operation: "File [action]: [name]"
- Sync: "Sync [status]: [details]"
- Error: "[Source] Error: [message]"
- Templates produce consistent notification format
- Templates localization-ready

### Task 17: Wire Notification Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register notification commands: "Notifications: Show Center", "Notifications: Clear All", "Notifications: Toggle DND", "Activity: Show Timeline", "Activity: Export".
**Acceptance Criteria:**
- All commands registered in command palette
- "Show Center" opens notification center panel
- "Clear All" dismisses all notifications
- "Toggle DND" toggles Do Not Disturb
- "Show Timeline" opens activity timeline
- Keyboard shortcut for notification center

### Task 18: Wire Notification Theme Integration
**Files:** `src/ui/NotificationManager.cpp`
**Description:** Notifications use theme tokens: toast background, border, text, action button, severity colors.
**Acceptance Criteria:**
- Toast background from `notification_bg` token
- Error: red accent border
- Warning: yellow accent border
- Info: blue accent border or accent color
- Action buttons match theme button styling
- All 64 themes render notifications correctly

### Task 19: Wire Notification Accessibility
**Files:** `src/ui/NotificationManager.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Notifications accessible to screen readers. Toast announced. Actions keyboard-navigable.
**Acceptance Criteria:**
- Toast announced to screen reader on appear
- Announcement: priority + title + message
- Actions keyboard-focusable (Tab from toast)
- Notification center: full keyboard navigation
- Dismiss via Escape
- ARIA live region for toast area

### Task 20: Add Notification and Activity Tests
**Files:** `tests/unit/test_notification_system.cpp`
**Description:** Test notification system: routing, aggregation, persistence, DND mode, and activity timeline.
**Acceptance Criteria:**
- Notification routing: correct priority/display for each source
- Aggregation: rapid same-error condensed
- Persistence: save/load across sessions
- DND: toasts suppressed, center still receives
- Activity timeline: events logged chronologically
- Badge counts accurate

## Testing Requirements
- Notification routing and priority
- Toast display and auto-dismiss
- Aggregation of rapid notifications
- Activity timeline chronological ordering

## Phase Completion Criteria
- Notification center with grouped history
- Toast notifications with actions
- Activity timeline
- File, sync, git, and extension activity tracking
- Do Not Disturb mode
- Progress notifications for long operations
- All tests pass
