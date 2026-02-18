# Phase 23: Task Management and Calendar

## Overview
TaskService exists in core for task tracking. CalendarEngine and KanbanEngine exist in UI. However, the task management workflow (extract tasks from Markdown, track completion, view in calendar/kanban) is not wired end-to-end. This phase builds a complete task management experience leveraging existing infrastructure.

## Prerequisites
- Phase 06 (Workbench navigation)
- Phase 07 (Editor for task checkbox interaction)
- Phase 21 (Attribute View for task database views)

## Tasks

### Task 1: Wire TaskService for Workspace-Wide Task Tracking
**Files:** `src/core/TaskService.cpp`, `src/core/TaskService.h`
**Description:** TaskService exists. Wire it to scan all Markdown files for task items (`- [ ]` and `- [x]`), maintain a workspace-wide task index, and update on file changes.
**Acceptance Criteria:**
- Scan all .md files for `- [ ]` and `- [x]` task items
- Task index stores: text, status, file path, line number, due date, priority, tags
- Index updates on file save
- Due date extracted from `@due(YYYY-MM-DD)` syntax
- Priority extracted from `@priority(high|medium|low)` syntax
- `TaskIndexUpdatedEvent` emitted on change

### Task 2: Wire Task Sidebar Panel
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/TaskService.cpp`
**Description:** Add a Tasks panel to the tool window area. Shows all tasks grouped by status (pending, completed) with filters.
**Acceptance Criteria:**
- Tasks panel accessible from View menu
- Tasks grouped: Today, Upcoming, Overdue, No Date, Completed
- Each task shows: checkbox, text, file link, due date
- Click checkbox toggles completion in source file
- Click task navigates to source file at task line

### Task 3: Wire Task Checkbox Interaction in Editor
**Files:** `src/ui/EditorPanel.cpp`, `src/core/TaskService.cpp`
**Description:** Clicking a task checkbox in the editor toggles between `- [ ]` and `- [x]`. Toggle emits TaskCompletedEvent with completion timestamp.
**Acceptance Criteria:**
- Click on `[ ]` in gutter area toggles to `[x]`
- Click on `[x]` toggles to `[ ]`
- Completion adds `@completed(YYYY-MM-DD)` metadata
- `TaskCompletedEvent` emitted with task text and file
- Undo support for toggle
- Strikethrough styling for completed tasks (configurable)

### Task 4: Wire CalendarEngine for Task Calendar View
**Files:** `src/ui/CalendarEngine.cpp`, `src/ui/CalendarEngine.h`
**Description:** CalendarEngine exists. Wire it to display tasks in a calendar view: month view with task dots, week view with task blocks, day view with task list.
**Acceptance Criteria:**
- Month view: days with task count dots
- Week view: tasks as blocks in time slots
- Day view: task list for selected day
- Click task navigates to source
- Drag task between days updates due date in source
- View switcher: Month, Week, Day

### Task 5: Wire KanbanEngine for Task Kanban View
**Files:** `src/ui/KanbanEngine.cpp`, `src/ui/KanbanEngine.h`
**Description:** KanbanEngine exists. Wire it to display tasks in kanban columns: To Do, In Progress, Done. Column assignment via task metadata.
**Acceptance Criteria:**
- Default columns: To Do, In Progress, Done
- Custom columns configurable
- Drag card between columns updates task status in source
- Card shows: task text, due date, priority indicator, file link
- Column card count shown in header
- "Add Task" button in each column

### Task 6: Wire Task Metadata Parsing
**Files:** `src/core/TaskService.cpp`
**Description:** Parse rich task metadata from Markdown: `- [ ] Task text @due(2026-03-01) @priority(high) @assign(user) @project(name) #tag`.
**Acceptance Criteria:**
- `@due(YYYY-MM-DD)` parsed as due date
- `@priority(high|medium|low)` parsed as priority
- `@assign(name)` parsed as assignee
- `@project(name)` parsed as project
- `#tag` parsed as task tag
- Metadata shown in task panels

### Task 7: Wire Task Recurring Schedules
**Files:** `src/core/TaskService.cpp`
**Description:** Support recurring tasks: `@repeat(daily|weekly|monthly|yearly)`. When completed, a new task is created with the next due date.
**Acceptance Criteria:**
- `@repeat(daily)` creates next task due tomorrow on completion
- `@repeat(weekly)` creates next task due in 7 days
- `@repeat(monthly)` creates next task due in 1 month
- New recurring task inserted below completed task in source
- Recurring indicator in task panels

### Task 8: Wire Task Priority Sorting and Coloring
**Files:** `src/core/TaskService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Tasks sorted by priority (high > medium > low > none) then by due date. Priority shown with color indicators in all views.
**Acceptance Criteria:**
- High priority: red indicator
- Medium priority: orange indicator
- Low priority: blue indicator
- No priority: no indicator
- Overdue tasks: red background in task panels
- Sort: overdue first, then by priority, then by due date

### Task 9: Wire Task Notifications
**Files:** `src/core/NotificationService.cpp`, `src/core/TaskService.cpp`
**Description:** Notifications for tasks: daily summary of due tasks on app start, overdue task alerts.
**Acceptance Criteria:**
- App start: "You have X tasks due today"
- Overdue tasks: "X tasks are overdue"
- Click notification opens task panel
- Configurable notification preferences
- Badge on Tasks activity bar item

### Task 10: Wire Task Quick Add
**Files:** `src/ui/CommandPalette.cpp`, `src/core/TaskService.cpp`
**Description:** Quick add task from anywhere: command palette shortcut creates a task in a designated inbox file with metadata.
**Acceptance Criteria:**
- "Task: Quick Add" command (Cmd+Shift+T)
- Input field for task text with metadata shortcuts
- Task added to inbox file (configurable path)
- Support inline metadata in quick add
- "Task: Open Inbox" navigates to inbox file

### Task 11: Wire Task Completion Statistics
**Files:** `src/core/TaskService.cpp`
**Description:** Track task completion statistics: tasks completed today, this week, this month. Show productivity trends.
**Acceptance Criteria:**
- Completed count: today, this week, this month
- Completion rate trend (last 7 days)
- Statistics shown in task panel header
- "Productivity" view in task panel
- Export statistics as Markdown report

### Task 12: Wire Task Filters and Saved Views
**Files:** `src/core/TaskService.cpp`
**Description:** Filter tasks by: project, tag, priority, due date range, assignee, file path. Save filter combinations as named views.
**Acceptance Criteria:**
- Filter by any metadata field
- Filters combine with AND logic
- Save filter as named view
- Named views listed in task panel sidebar
- Default views: Today, This Week, All, Completed

### Task 13: Wire Task-to-Document Links
**Files:** `src/core/TaskService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Tasks show their source document. Click navigates to exact line. Moving tasks between files updates the index.
**Acceptance Criteria:**
- Each task linked to source file and line number
- Click file link opens document at task line
- File rename updates task source links
- File delete removes tasks from index
- Task line number updates on document edit

### Task 14: Wire Task Project Grouping
**Files:** `src/core/TaskService.cpp`
**Description:** Tasks can be grouped by project. Projects extracted from `@project(name)` metadata or from file path patterns.
**Acceptance Criteria:**
- Group by project in task panel
- Project from `@project(name)` metadata
- Fallback: project from parent folder name
- Project summary: total tasks, completed, overdue
- Project filter in all task views

### Task 15: Wire Task Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register task commands: "Task: Quick Add", "Task: Open Today", "Task: Show Calendar", "Task: Show Kanban", "Task: Open Inbox", "Task: Statistics".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Task:" prefix
- "Open Today" shows today's tasks
- "Show Calendar" opens calendar view
- "Show Kanban" opens kanban view

### Task 16: Wire Task Theme Integration
**Files:** `src/ui/CalendarEngine.cpp`, `src/ui/KanbanEngine.cpp`
**Description:** Task views use theme tokens: calendar colors, kanban card styles, priority colors, overdue indicators.
**Acceptance Criteria:**
- Calendar uses theme background and text colors
- Kanban cards use theme card styling
- Priority colors consistent across views
- Overdue uses theme error color
- Today highlight uses theme accent

### Task 17: Wire Task Integration with Daily Notes
**Files:** `src/core/TaskService.cpp`, `src/core/DailyNoteService.cpp`
**Description:** Daily notes automatically include today's tasks. Tasks completed in daily note update the source file.
**Acceptance Criteria:**
- Daily note template includes `## Tasks Due Today` section
- Auto-populated with tasks due for that date
- Completing tasks in daily note updates source file
- Configurable: include/exclude task section
- Tasks from daily note linked to original source

### Task 18: Wire Task Export
**Files:** `src/core/TaskService.cpp`
**Description:** Export tasks to Markdown, CSV, or JSON. Export respects current filters and grouping.
**Acceptance Criteria:**
- Export to Markdown: grouped task list
- Export to CSV: one row per task with all metadata
- Export to JSON: structured task data
- Export respects active filters
- Export includes completion status and dates

### Task 19: Wire Task Bulk Operations
**Files:** `src/core/TaskService.cpp`
**Description:** Bulk operations on selected tasks: mark complete, change priority, change due date, move to project, tag, delete.
**Acceptance Criteria:**
- Multi-select tasks in panel (Shift+click, Cmd+click)
- Bulk actions in toolbar: Complete, Set Priority, Set Due Date
- Bulk tag assignment
- Bulk move to project
- All bulk operations update source files
- Undo support for bulk operations

### Task 20: Add Task System Tests
**Files:** `tests/unit/test_task_service.cpp`, `tests/unit/test_calendar.cpp`
**Description:** Test task system: extraction, metadata parsing, scheduling, calendar view, and kanban interactions.
**Acceptance Criteria:**
- Task extraction from Markdown
- Metadata parsing: due date, priority, project, tags
- Recurring task generation
- Calendar date assignment
- Kanban column assignment and movement
- Task completion updates source file

## Testing Requirements
- Task extraction from various Markdown formats
- Metadata parsing accuracy
- Calendar and kanban view data correctness
- Task completion propagation to source files

## Phase Completion Criteria
- Task extraction from Markdown workspace-wide
- Task panel with grouping and filtering
- Calendar view with tasks
- Kanban view with drag-and-drop
- Task metadata parsing (due, priority, project, tags)
- Recurring tasks
- All tests pass
