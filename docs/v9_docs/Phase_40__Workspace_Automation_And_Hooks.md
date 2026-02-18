# Phase 40: Workspace Automation and Hooks

## Overview
The EventBus system provides a powerful event backbone but there is no user-facing automation layer: users cannot define custom workflows triggered by events (e.g., auto-tag on save, auto-export on commit, auto-sync on file change). This phase builds a workspace automation system on top of the EventBus.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 36 (Command system for automation actions)
- Phase 33 (Structured logging for automation audit)

## Tasks

### Task 1: Define Automation Rule Schema
**Files:** `src/core/AutomationEngine.h`, `src/core/AutomationEngine.cpp`
**Description:** Create AutomationEngine: define rule schema with trigger (event type), condition (predicate), and action (command). Rules in `.markamp/automations.yaml`.
**Acceptance Criteria:**
- Rule schema: name, trigger (event), condition (expression), action (command + args)
- Trigger: any EventBus event type
- Condition: JavaScript-like expression on event data
- Action: command ID with arguments
- Rules loaded from `.markamp/automations.yaml`
- `AutomationRuleLoadedEvent` emitted per rule

### Task 2: Wire Automation Trigger System
**Files:** `src/core/AutomationEngine.cpp`, `src/core/EventBus.cpp`
**Description:** AutomationEngine subscribes to trigger events. When event matches, evaluate condition. If condition true, execute action.
**Acceptance Criteria:**
- Trigger subscription: one EventBus subscription per rule
- Condition evaluation: compare event fields against values
- Action execution: call command by ID with arguments
- Condition operators: ==, !=, contains, startsWith, regex
- Event data accessible in condition: `event.file_path`, `event.content`
- Async action execution (non-blocking)

### Task 3: Wire Built-In Automation Triggers
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Built-in triggers with friendly names: on-file-save, on-file-create, on-file-delete, on-commit, on-sync, on-workspace-open, on-workspace-close.
**Acceptance Criteria:**
- `on-file-save`: fires after any file save
- `on-file-create`: fires when new file created
- `on-file-delete`: fires when file deleted
- `on-commit`: fires after git commit
- `on-sync`: fires after sync complete
- `on-workspace-open`: fires on workspace load
- Each trigger provides relevant event data

### Task 4: Wire Built-In Automation Actions
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Built-in actions beyond commands: run-shell-command, notify, update-frontmatter, add-tag, export-file, create-file-from-template.
**Acceptance Criteria:**
- `run-shell-command`: execute shell command with variables
- `notify`: show notification with message
- `update-frontmatter`: set/update YAML frontmatter fields
- `add-tag`: add tag to document
- `export-file`: export current file to format
- `create-file-from-template`: create file from template

### Task 5: Wire Automation Variables
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Variables available in automation expressions and actions: `$file_path`, `$file_name`, `$workspace`, `$date`, `$time`, `$branch`, `$user`.
**Acceptance Criteria:**
- `$file_path`: absolute path of affected file
- `$file_name`: filename without path
- `$workspace`: workspace root path
- `$date`: current date (ISO format)
- `$time`: current time (ISO format)
- `$branch`: current git branch
- Variables expanded in action arguments

### Task 6: Wire Automation UI
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/AutomationEngine.cpp`
**Description:** Automation panel in tool window: list rules, enable/disable, test, create new. Visual rule editor for non-technical users.
**Acceptance Criteria:**
- Automation panel shows all rules
- Toggle enable/disable per rule
- "Test" button simulates rule with sample event
- "New Rule" wizard: choose trigger, set condition, pick action
- Rule status: active, disabled, error
- Last triggered timestamp shown

### Task 7: Wire Automation Error Handling
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Automation errors handled gracefully: action failure logged, rule disabled after 5 consecutive failures, notification to user.
**Acceptance Criteria:**
- Action failure: error logged to "Automations" output channel
- Consecutive failures (5): rule auto-disabled with notification
- Error includes: rule name, trigger event, error message
- "Re-enable" action on disable notification
- Error count tracked per rule
- "Automations: Show Errors" command

### Task 8: Wire File Hooks
**Files:** `src/core/AutomationEngine.cpp`, `src/core/VaultService.cpp`
**Description:** Pre/post hooks for file operations: before-save, after-save, before-create, after-create. Pre-hooks can cancel the operation.
**Acceptance Criteria:**
- `before-save`: hook can modify content or cancel save
- `after-save`: hook runs after successful save
- `before-create`: hook can set initial content
- `after-create`: hook runs after file creation
- Pre-hook cancellation: operation aborted with message
- Hook timeout: 5 seconds max

### Task 9: Wire Scheduled Automations
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Cron-like scheduled automations: run actions at specified intervals or times. Used for periodic tasks like reindex, backup, sync.
**Acceptance Criteria:**
- Schedule syntax: `every: 30m` or `cron: "0 9 * * *"`
- Minimum interval: 5 minutes
- Schedule evaluated while application is running
- Missed schedules: run once on next opportunity
- Scheduled rules shown in automation panel with next run time
- "Run Now" option for scheduled rules

### Task 10: Wire Frontmatter Auto-Update
**Files:** `src/core/AutomationEngine.cpp`, `src/core/Config.h`
**Description:** Built-in automation: auto-update frontmatter on save. Update `modified` date, word count, reading time.
**Acceptance Criteria:**
- `modified: YYYY-MM-DD` updated on each save
- `word_count: 1234` updated on save (optional)
- `reading_time: "5 min"` updated on save (optional)
- Fields configurable: which fields to auto-update
- Toggle per-workspace
- Default: only `modified` date

### Task 11: Wire Automation Templates
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Pre-built automation templates for common workflows. Users can install and customize.
**Acceptance Criteria:**
- Template: "Auto-tag by folder" (tag based on parent folder)
- Template: "Auto-export on save" (export to HTML on .md save)
- Template: "Daily journal creation" (create daily note on workspace open)
- Template: "Link checker" (validate links on save)
- Template: "Frontmatter updater" (update modified date)
- Templates installable from gallery

### Task 12: Wire Automation Chaining
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Chain automations: output of one action becomes input to next. Define pipeline of actions for complex workflows.
**Acceptance Criteria:**
- Chain syntax in YAML: `actions: [action1, action2, action3]`
- Actions execute sequentially
- Output data passed between actions
- Chain stops on error (configurable: continue or stop)
- Chain timeout: sum of individual timeouts
- Chain execution logged as single activity

### Task 13: Wire Automation Rate Limiting
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Rate limiting for automations: prevent runaway rules from overwhelming the system.
**Acceptance Criteria:**
- Per-rule rate limit: max N executions per minute
- Global rate limit: max 100 automation actions per minute
- Rate limit exceeded: rule throttled with warning
- Burst allowed: 3x rate for short bursts
- Rate limit configurable per rule
- Rate limiting metrics in automation panel

### Task 14: Wire Extension Automation Hooks
**Files:** `src/core/PluginManager.cpp`, `src/core/AutomationEngine.cpp`
**Description:** Extensions can register custom triggers and actions. Extensions can also react to automation events.
**Acceptance Criteria:**
- Extension registers trigger: custom event name, schema
- Extension registers action: command with parameters
- Extension triggers appear in automation UI
- Extension actions appear in action picker
- Permission: `automation` scope required
- Extension triggers validated

### Task 15: Wire Automation Import/Export
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Export automation rules for sharing. Import rules from YAML files.
**Acceptance Criteria:**
- Export: selected rules to YAML file
- Import: load rules from YAML file
- Import validation: check triggers and actions exist
- Conflict resolution: skip or overwrite existing rules
- Share: export as shareable format
- Community automation library (future-ready)

### Task 16: Wire Automation Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register automation commands: "Automation: New Rule", "Automation: Show Panel", "Automation: Enable All", "Automation: Disable All", "Automation: Show Errors", "Automation: Import".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Automation:" prefix
- "New Rule" opens rule wizard
- "Show Panel" opens automation panel
- "Show Errors" shows error log

### Task 17: Wire Automation Logging
**Files:** `src/core/AutomationEngine.cpp`, `src/core/OutputChannelService.cpp`
**Description:** All automation activity logged to "Automations" output channel: triggers, condition evaluations, action executions, errors.
**Acceptance Criteria:**
- Trigger: "Rule 'X' triggered by event Y"
- Condition: "Condition evaluated: true/false"
- Action: "Action 'command' executed in Xms"
- Error: "Action failed: [error message]"
- Log level configurable: verbose, normal, errors-only
- Log filterable in output panel

### Task 18: Wire Automation Dry Run
**Files:** `src/core/AutomationEngine.cpp`
**Description:** Dry run mode: simulate automation without executing actions. Shows what would happen. Useful for testing new rules.
**Acceptance Criteria:**
- "Test Rule" button in automation panel
- Dry run: evaluate trigger and condition, log but don't execute
- Dry run result: "Would execute: [action] with [args]"
- Sample event generation for testing
- Dry run for all rules: "Automation: Dry Run All"

### Task 19: Wire Automation Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Automation settings: master enable/disable, global rate limit, hook timeout, logging level.
**Acceptance Criteria:**
- Master toggle: enable/disable all automations
- Global rate limit: automations per minute
- Hook timeout: seconds (default 5)
- Logging: verbose, normal, errors-only
- Auto-disable threshold: consecutive failures before disable

### Task 20: Add Automation Tests
**Files:** `tests/unit/test_automation_engine.cpp`
**Description:** Test automation system: rule loading, trigger matching, condition evaluation, action execution, error handling.
**Acceptance Criteria:**
- Rule parsing from YAML
- Trigger matching: correct events trigger correct rules
- Condition evaluation: all operators work
- Action execution: command called with correct args
- Error handling: failures counted and logged
- Rate limiting: excess executions throttled

## Testing Requirements
- Rule schema parsing and validation
- Trigger-to-rule matching
- Condition expression evaluation
- Action execution and error handling

## Phase Completion Criteria
- Automation rules in `.markamp/automations.yaml`
- Event-driven triggers with conditions
- Built-in actions and custom commands
- File hooks (before/after save, create)
- Scheduled automations
- Automation panel with management UI
- All tests pass
