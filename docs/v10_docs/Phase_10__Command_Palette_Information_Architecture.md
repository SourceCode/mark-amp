# Phase 10: Command Palette Information Architecture

## Overview
Improve command palette UX so users can find, understand, and execute commands faster.

## Prerequisites
- Phase 05
- Phase 09

## Tasks

### Task 1: Add Rich Command Metadata
**Files:** `src/ui/CommandPalette.h`, `src/ui/CommandPalette.cpp`, `src/ui/MainFrame.cpp`
**Description:** Include category, scope, shortcut, and command source metadata.
**Acceptance Criteria:**
- Results show consistent metadata and grouping

### Task 2: Add Action Verbs and Aliases
**Files:** `src/ui/CommandPalette.cpp`
**Description:** Expand fuzzy matching with aliases and verb-first ranking.
**Acceptance Criteria:**
- Common intents rank expected commands first

### Task 3: Add Preview Pane for High-Impact Commands
**Files:** `src/ui/CommandPalette.cpp`
**Description:** Show brief description and side effects before execution for destructive commands.
**Acceptance Criteria:**
- High-impact actions present confirmation context

### Task 4: Add Recent and Pinned Commands
**Files:** `src/ui/CommandPalette.cpp`, `src/core/Config.cpp`
**Description:** Persist MRU and optionally pin frequent commands.
**Acceptance Criteria:**
- MRU behavior is deterministic and persisted

### Task 5: Add Command Palette Tests
**Files:** `tests/unit/test_command_palette_ranking.cpp`
**Description:** Test ranking, filtering, metadata rendering, and execution safety prompts.
**Acceptance Criteria:**
- Ranking and filter behavior covered by regression tests

## Testing Requirements
- Ranking quality checks with curated query set
- Keyboard-only command execution verification

## Phase Completion Criteria
- Command palette is a reliable primary control entry point
